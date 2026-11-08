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
    patchReply = { status: 400, body: { screen_dim_level: ["Ensure this value is greater than or equal to 1."], buzzer_sound_touch: ["Unknown setting."] } };

    await user.clear(screen.getByLabelText("Screen brightness (%)"));
    await user.type(screen.getByLabelText("Screen brightness (%)"), "0");
    await user.click(saveButton());

    expect(await screen.findByText("Ensure this value is greater than or equal to 1.")).toBeInTheDocument();
    expect(screen.getByLabelText("Screen brightness (%)")).toHaveAttribute("aria-invalid", "true");
    expect(Notify).toHaveBeenCalledWith(expect.objectContaining({ level: "error", message: "Nothing was saved. buzzer_sound_touch: Unknown setting." }));
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
