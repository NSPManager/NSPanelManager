import { render, screen, waitFor, within } from "@testing-library/react";
import userEvent from "@testing-library/user-event";
import { afterEach, beforeEach, describe, expect, it, vi } from "vitest";
import type { Settings } from "../../api/client";
import { Notify } from "../NSPanelToastContainer";
import SettingsPage from "./SettingsPage";
import { settingsFixture } from "./testFixtures";

vi.mock("../NSPanelToastContainer", () => ({ Notify: vi.fn() }));

type Reply = { status: number; body: unknown };

// A fake server: GET returns `stored`; PATCH merges the body into it (or returns `patchReply`).
let stored: Settings;
let patchReply: Reply | null;
let getReply: Reply | null;
const requests: { method: string; url: string; headers: Headers; body: unknown }[] = [];

function json(reply: Reply) {
  return new Response(JSON.stringify(reply.body), { status: reply.status, headers: { "Content-Type": "application/json" } });
}

const fetchMock = vi.fn(async (input: Request | string, init?: RequestInit) => {
  const request = typeof input === "string" ? new Request(new URL(input, "http://localhost"), init) : input;
  const text = await request.clone().text();
  const url = new URL(request.url);
  requests.push({ method: request.method, url: url.pathname + url.search, headers: request.headers, body: text ? JSON.parse(text) : undefined });
  if (url.pathname === "/rest/v2/settings" && request.method === "GET") {
    return json(getReply ?? { status: 200, body: stored });
  }
  if (url.pathname === "/rest/v2/settings" && request.method === "PATCH") {
    if (patchReply) {
      return json(patchReply);
    }
    const body = JSON.parse(text);
    stored = { ...stored, ...body };
    for (const secret of ["mqtt_password", "home_assistant_token", "openhab_token"]) {
      if (secret in body) {
        delete (stored as Record<string, unknown>)[secret];
        (stored as Record<string, unknown>)[`${secret}_set`] = body[secret] !== "";
      }
    }
    return json({ status: 200, body: stored });
  }
  if (url.pathname === "/rest/get_ip_by_hostname") {
    return json({ status: 200, body: { ip: "10.0.0.5" } });
  }
  if (url.pathname.endsWith("/test")) {
    return json({ status: 200, body: { status: "success" } });
  }
  throw new Error(`Unexpected request: ${request.method} ${url.pathname}`);
});

const patches = () => requests.filter((request) => request.method === "PATCH");
const saveButton = () => screen.getByRole("button", { name: "Save" });

async function renderPage(settings: Partial<Settings> = {}) {
  stored = settingsFixture(settings);
  const user = userEvent.setup();
  render(<SettingsPage logDownloadUrl="/ingress/download_mqttmanager_log" />);
  await screen.findByLabelText("MQTT port");
  return user;
}

async function save(user: ReturnType<typeof userEvent.setup>) {
  await user.click(saveButton());
  await waitFor(() => expect(saveButton()).toBeDisabled());
}

beforeEach(() => {
  vi.stubGlobal("fetch", fetchMock);
  document.cookie = "csrftoken=token-123";
  requests.length = 0;
  patchReply = null;
  getReply = null;
});

afterEach(() => {
  vi.unstubAllGlobals();
  vi.mocked(Notify).mockClear();
});

describe("loading", () => {
  it("shows the stored settings", async () => {
    await renderPage();

    expect(screen.getByLabelText("MQTT port")).toHaveValue(1883);
    expect(screen.getByLabelText("MQTT server")).toHaveValue("mqtt.local");
    expect(screen.getByLabelText("Screensaver mode")).toHaveValue("with_background");
    expect(within(screen.getByRole("group", { name: "Entity update mode" })).getByLabelText("Optimistic")).toBeChecked();
    expect(within(screen.getByRole("group", { name: "Color temperature slider" })).getByLabelText("Keep default")).toBeChecked();
    expect(screen.getByRole("link", { name: /Download/ })).toHaveAttribute("href", "/ingress/download_mqttmanager_log");
    expect(saveButton()).toBeDisabled();
  });

  it("shows a load error and retries", async () => {
    stored = settingsFixture();
    getReply = { status: 500, body: {} };
    const user = userEvent.setup();
    render(<SettingsPage />);

    expect(await screen.findByRole("alert")).toHaveTextContent("Couldn't load the settings: HTTP 500");

    getReply = null;
    await user.click(screen.getByRole("button", { name: "Retry" }));
    expect(await screen.findByLabelText("MQTT port")).toHaveValue(1883);
  });

  it("marks settings with no valid stored value, and doesn't send them unless filled in", async () => {
    const user = await renderPage({ mqtt_port: null, screensaver_mode: null });

    expect(screen.getByLabelText("MQTT port")).toHaveValue(null);
    expect(screen.getAllByText("No valid value is stored. Enter one.").length).toBe(2);

    await user.clear(screen.getByLabelText("MQTT server"));
    await user.type(screen.getByLabelText("MQTT server"), "broker");
    await save(user);

    expect(patches()[0].body).toEqual({ mqtt_server: "broker" });
  });
});

describe("saving", () => {
  it("sends only the changed settings, with the CSRF token", async () => {
    const user = await renderPage();

    await user.clear(screen.getByLabelText("MQTT port"));
    await user.type(screen.getByLabelText("MQTT port"), "1884");
    await user.selectOptions(screen.getByLabelText("Screensaver mode"), "no_screensaver");
    await user.click(within(screen.getByRole("group", { name: "Color temperature slider" })).getByLabelText("Reverse"));
    await save(user);

    expect(patches()).toHaveLength(1);
    expect(patches()[0].body).toEqual({ mqtt_port: 1884, screensaver_mode: "no_screensaver", reverse_color_temp: true });
    expect(patches()[0].headers.get("X-CSRFToken")).toBe("token-123");
    expect(Notify).toHaveBeenCalledWith(expect.objectContaining({ level: "success" }));
  });

  it("is clean again after saving", async () => {
    const user = await renderPage();

    await user.clear(screen.getByLabelText("MQTT port"));
    await user.type(screen.getByLabelText("MQTT port"), "1884");
    await save(user);

    expect(screen.getByLabelText("MQTT port")).toHaveValue(1884);
    expect(saveButton()).toBeDisabled();
  });

  it("sends nothing for a change that was undone", async () => {
    const user = await renderPage();

    await user.clear(screen.getByLabelText("MQTT port"));
    await user.type(screen.getByLabelText("MQTT port"), "1883");

    expect(saveButton()).toBeDisabled();
  });

  it("discards changes", async () => {
    const user = await renderPage();

    await user.clear(screen.getByLabelText("MQTT server"));
    await user.type(screen.getByLabelText("MQTT server"), "other");
    await user.click(screen.getByRole("button", { name: "Discard changes" }));

    expect(screen.getByLabelText("MQTT server")).toHaveValue("mqtt.local");
    expect(saveButton()).toBeDisabled();
  });

  it("sends a blank number as null so the server reports it", async () => {
    const user = await renderPage();

    await user.clear(screen.getByLabelText("MQTT port"));
    await save(user);

    expect(patches()[0].body).toEqual({ mqtt_port: null });
  });

  it("shows field errors next to their fields and saves nothing", async () => {
    const user = await renderPage();
    patchReply = { status: 400, body: { screen_dim_level: ["Ensure this value is greater than or equal to 1."], theme: ["Unknown setting."] } };

    await user.clear(screen.getByLabelText("Screen brightness (%)"));
    await user.type(screen.getByLabelText("Screen brightness (%)"), "0");
    await user.click(saveButton());

    expect(await screen.findByText("Ensure this value is greater than or equal to 1.")).toBeInTheDocument();
    expect(screen.getByLabelText("Screen brightness (%)")).toHaveAttribute("aria-invalid", "true");
    expect(Notify).toHaveBeenCalledWith(expect.objectContaining({ level: "error", message: "Nothing was saved. theme: Unknown setting." }));
  });

  it("reports other errors", async () => {
    const user = await renderPage();
    patchReply = { status: 403, body: { detail: "CSRF Failed: CSRF token missing." } };

    await user.clear(screen.getByLabelText("MQTT port"));
    await user.type(screen.getByLabelText("MQTT port"), "1884");
    await user.click(saveButton());

    await waitFor(() => expect(Notify).toHaveBeenCalledWith(expect.objectContaining({ level: "error", message: "Couldn't save the settings: CSRF Failed: CSRF token missing." })));
  });
});

describe("secrets", () => {
  it("keeps a stored secret when left blank", async () => {
    const user = await renderPage();

    await user.type(screen.getByLabelText("MQTT username"), "x");
    await save(user);

    expect(patches()[0].body).toEqual({ mqtt_username: "nspmx" });
  });

  it("sends a new secret", async () => {
    const user = await renderPage();

    await user.type(screen.getByLabelText("MQTT password"), "hunter2");
    await save(user);

    expect(patches()[0].body).toEqual({ mqtt_password: "hunter2" });
    expect(screen.getByLabelText("MQTT password")).toHaveValue("");
  });

  it("clears a stored secret", async () => {
    const user = await renderPage();

    await user.click(screen.getByLabelText("Clear the stored mqtt password"));
    expect(screen.getByLabelText("MQTT password")).toBeDisabled();
    await save(user);

    expect(patches()[0].body).toEqual({ mqtt_password: "" });
    expect(screen.queryByLabelText("Clear the stored mqtt password")).not.toBeInTheDocument();
  });

  it("offers clear only for a stored secret", async () => {
    await renderPage();

    expect(screen.getByLabelText("Clear the stored mqtt password")).toBeInTheDocument();
    expect(screen.queryByLabelText("Clear the stored openhab token")).not.toBeInTheDocument();
  });
});

describe("Home Assistant add-on", () => {
  it("locks the Home Assistant settings", async () => {
    await renderPage({ is_home_assistant_addon: true });

    expect(screen.getByText(/managed by Home Assistant and are locked/)).toBeInTheDocument();
    expect(screen.getByLabelText("Home Assistant address")).toBeDisabled();
    expect(screen.getByLabelText("Home Assistant token")).toBeDisabled();
    expect(screen.queryByLabelText("Clear the stored home assistant token")).not.toBeInTheDocument();
    expect(screen.queryByRole("button", { name: "Test Home Assistant" })).not.toBeInTheDocument();
  });
});

describe("conditional settings", () => {
  it("shows the outside temperature setting only for the screensaver modes without weather", async () => {
    const user = await renderPage();
    const outside = () => screen.queryByRole("group", { name: "Screensaver outside temperature" });

    expect(outside()).not.toBeInTheDocument();
    await user.selectOptions(screen.getByLabelText("Screensaver mode"), "datetime_with_background");
    expect(outside()).toBeInTheDocument();
    await user.selectOptions(screen.getByLabelText("Screensaver mode"), "datetime_without_background");
    expect(outside()).toBeInTheDocument();
    await user.selectOptions(screen.getByLabelText("Screensaver mode"), "no_screensaver");
    expect(outside()).not.toBeInTheDocument();
  });
});

describe("helpers", () => {
  it("loads the manager address and port from the page URL", async () => {
    const user = await renderPage();

    const [addressButton, portButton] = screen.getAllByRole("button", { name: "Load from URL" });
    await user.click(addressButton);
    await waitFor(() => expect(screen.getByLabelText("Manager address")).toHaveValue("10.0.0.5"));
    await user.click(portButton);
    expect(screen.getByLabelText("Manager port")).toHaveValue(Number(window.location.port || 80));

    const lookup = requests.find((request) => request.url === "/rest/get_ip_by_hostname");
    expect(lookup?.body).toEqual({ hostname: window.location.hostname });
    expect(lookup?.headers.get("X-CSRFToken")).toBe("token-123");
    expect(saveButton()).toBeEnabled();
  });

  it("tests the manager address with the values in the form", async () => {
    const user = await renderPage();

    await user.click(screen.getByRole("button", { name: "Test manager address" }));

    await waitFor(() => expect(Notify).toHaveBeenCalledWith(expect.objectContaining({ level: "success" })));
    expect(requests.some((request) => request.url === "/rest/mqttmanager/test?address=192.168.1.10&port=8000")).toBe(true);
  });

  it("tests Home Assistant only with a typed token", async () => {
    const user = await renderPage();
    const test = () => screen.getByRole("button", { name: "Test Home Assistant" });

    expect(test()).toBeDisabled();
    await user.type(screen.getByLabelText("Home Assistant token"), "abc");
    await user.click(test());

    await waitFor(() => expect(requests.some((request) => request.url === "/rest/home_assistant/test?address=http%3A%2F%2Fha.local%3A8123&token=abc")).toBe(true));
  });
});

// Records what the preview schedules. Oscillators end only when a test calls end().
class FakeParam {
  value = 0;
  readonly events: [string, number, number][] = [];
  setValueAtTime(value: number, time: number) {
    this.events.push(["set", value, time]);
  }
  linearRampToValueAtTime(value: number, time: number) {
    this.events.push(["ramp", value, time]);
  }
}

class FakeNode {
  connect<T>(target: T) {
    return target;
  }
  disconnect() {}
}

class FakeOscillator extends FakeNode {
  type = "sine";
  frequency = new FakeParam();
  onended: (() => void) | null = null;
  stopped = false;
  start() {}
  stop(time?: number) {
    if (time === undefined) {
      this.stopped = true;
      this.onended?.();
    }
  }
  end() {
    this.onended?.();
  }
}

const oscillators: FakeOscillator[] = [];
const filters: { type: string }[] = [];

class FakeAudioContext {
  state = "running";
  currentTime = 0;
  destination = new FakeNode();
  createOscillator() {
    const oscillator = new FakeOscillator();
    oscillators.push(oscillator);
    return oscillator;
  }
  createGain() {
    return Object.assign(new FakeNode(), { gain: new FakeParam() });
  }
  createBiquadFilter() {
    const filter = Object.assign(new FakeNode(), { type: "", frequency: new FakeParam(), Q: new FakeParam() });
    filters.push(filter);
    return filter;
  }
  resume() {
    return Promise.resolve();
  }
}

// The frequencies a preview played, in order.
const played = (oscillator: FakeOscillator) => oscillator.frequency.events.map(([, frequency]) => frequency);

describe("buzzer sounds", () => {
  const choice = (label: string) => screen.getByLabelText(label) as HTMLSelectElement;
  const rtttl = (label: string) => screen.getByLabelText(`${label} RTTTL`) as HTMLInputElement;
  const previewButton = (label: string) => screen.getByRole("button", { name: new RegExp(`(Preview|Stop) the ${label.toLowerCase()}`) });

  beforeEach(() => {
    vi.stubGlobal("AudioContext", FakeAudioContext);
    oscillators.length = 0;
    filters.length = 0;
  });

  it("is off by default", async () => {
    await renderPage();

    expect(choice("Screen touch")).toHaveDisplayValue("Off");
    expect(choice("Scene activated")).toHaveDisplayValue("Same as screen touch");
    expect(choice("Scene saved")).toHaveDisplayValue("Off");
    for (const label of ["Screen touch", "Scene activated", "Scene saved"]) {
      expect(rtttl(label)).toHaveValue("");
      expect(previewButton(label)).toBeDisabled();
    }
  });

  it("shows stored sounds as their preset, or as custom", async () => {
    await renderPage({ buzzer_sound_touch: "chime:d=32,o=6,b=190:g6,16p,32c7", buzzer_sound_scene_activated: "silent:d=64,o=6,b=900:p", buzzer_sound_scene_saved: "mine:d=8:c,d" });

    expect(choice("Screen touch")).toHaveDisplayValue("Chime");
    expect(choice("Scene activated")).toHaveDisplayValue("Silent");
    expect(choice("Scene saved")).toHaveDisplayValue("Custom…");
    expect(rtttl("Scene saved")).toHaveValue("mine:d=8:c,d");
    expect(screen.getByText(/Played on every press of the screen. Plays for 157 ms./)).toBeInTheDocument();
    expect(screen.getByText(/instead of the touch sound .* Silent\./)).toBeInTheDocument();
  });

  it("saves a chosen preset as its RTTTL, and nothing else", async () => {
    const user = await renderPage();

    await user.selectOptions(choice("Screen touch"), "Tick");
    expect(rtttl("Screen touch")).toHaveValue("tick:d=64,o=7,b=625:b");
    await save(user);

    expect(patches()).toHaveLength(1);
    expect(patches()[0].body).toEqual({ buzzer_sound_touch: "tick:d=64,o=7,b=625:b" });
  });

  it("turns a sound off by saving it blank", async () => {
    const user = await renderPage({ buzzer_sound_scene_saved: "saved:d=16,o=6,b=200:c,e,g,8c7" });

    await user.selectOptions(choice("Scene saved"), "Off");
    await save(user);

    expect(patches()[0].body).toEqual({ buzzer_sound_scene_saved: "" });
  });

  it("lets a preset be edited as a custom sound", async () => {
    const user = await renderPage({ buzzer_sound_touch: "tick:d=64,o=7,b=625:b" });

    await user.selectOptions(choice("Screen touch"), "Custom…");
    expect(choice("Screen touch")).toHaveDisplayValue("Custom…");
    expect(rtttl("Screen touch")).toHaveFocus();
    await user.type(rtttl("Screen touch"), ",b");
    await save(user);

    expect(patches()[0].body).toEqual({ buzzer_sound_touch: "tick:d=64,o=7,b=625:b,b" });
  });

  it("shows invalid RTTTL as it is typed and doesn't save it", async () => {
    const user = await renderPage();

    await user.selectOptions(choice("Scene saved"), "Custom…");
    await user.type(rtttl("Scene saved"), "x:b=999:c");

    expect(screen.getByText("b must be 1 to 900.")).toBeInTheDocument();
    expect(rtttl("Scene saved")).toHaveAttribute("aria-invalid", "true");
    expect(previewButton("Scene saved")).toBeDisabled();
    await user.click(saveButton());
    expect(patches()).toHaveLength(0);

    await user.clear(rtttl("Scene saved"));
    await user.type(rtttl("Scene saved"), "x:b=900:c");
    expect(screen.queryByText("b must be 1 to 900.")).not.toBeInTheDocument();
    await save(user);
    expect(patches()[0].body).toEqual({ buzzer_sound_scene_saved: "x:b=900:c" });
  });

  it("flags a stored sound the panel can't play", async () => {
    await renderPage({ buzzer_sound_touch: "beep" });

    expect(choice("Screen touch")).toHaveDisplayValue("Custom…");
    expect(screen.getByText(/Expected name:defaults:notes/)).toBeInTheDocument();
  });

  it("shows a server error next to the sound", async () => {
    const user = await renderPage();
    patchReply = { status: 400, body: { buzzer_sound_touch: ["Rejected by the server."] } };

    await user.selectOptions(choice("Screen touch"), "Chime");
    await user.click(saveButton());

    expect(await screen.findByText("Rejected by the server.")).toBeInTheDocument();
  });

  it("previews the panel's steps, one sound at a time", async () => {
    const user = await renderPage({ buzzer_sound_touch: "chime:d=32,o=6,b=190:g6,16p,32c7", buzzer_sound_scene_saved: "confirm:d=16,o=6,b=357:e,g" });

    await user.click(previewButton("Screen touch"));
    expect(oscillators).toHaveLength(1);
    expect(oscillators[0].type).toBe("square");
    expect(played(oscillators[0])).toEqual([1568, 2093]);
    expect(previewButton("Screen touch")).toHaveAccessibleName("Stop the screen touch preview");

    // Starting another preview stops the first
    await user.click(previewButton("Scene saved"));
    expect(oscillators[0].stopped).toBe(true);
    expect(played(oscillators[1])).toEqual([1319, 1568]);
    expect(previewButton("Screen touch")).toHaveAccessibleName("Preview the screen touch sound");

    // And it ends by itself
    oscillators[1].end();
    await waitFor(() => expect(previewButton("Scene saved")).toHaveAccessibleName("Preview the scene saved sound"));
    expect(patches()).toHaveLength(0);
  });

  it("previews the touch sound for a blank scene activated sound", async () => {
    const user = await renderPage({ buzzer_sound_touch: "tick:d=64,o=7,b=625:b" });

    await user.click(previewButton("Scene activated"));
    expect(played(oscillators[0])).toEqual([3951]);
  });

  it("simulates the piezo unless that is turned off", async () => {
    const user = await renderPage({ buzzer_sound_touch: "tick:d=64,o=7,b=625:b" });

    await user.click(previewButton("Screen touch"));
    expect(filters).toEqual([expect.objectContaining({ type: "bandpass" })]);

    await user.click(screen.getByLabelText(/sound like the panel's buzzer/));
    await user.click(previewButton("Screen touch")); // stop
    await user.click(previewButton("Screen touch"));
    expect(oscillators).toHaveLength(2);
    expect(filters).toHaveLength(1);
  });

  it("says when the browser can't preview", async () => {
    vi.stubGlobal("AudioContext", undefined);
    await renderPage({ buzzer_sound_touch: "tick:d=64,o=7,b=625:b" });

    expect(screen.getByText("This browser can't play the preview.")).toBeInTheDocument();
    expect(previewButton("Screen touch")).toBeDisabled();
  });
});
