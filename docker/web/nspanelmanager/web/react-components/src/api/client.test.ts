import { afterEach, beforeEach, describe, expect, it, vi } from "vitest";
import { api, getCookie, type SettingsUpdateRequest } from "./client";

function jsonResponse(body: unknown, status = 200) {
  return new Response(JSON.stringify(body), { status, headers: { "Content-Type": "application/json" } });
}

describe("api client", () => {
  let fetchMock: ReturnType<typeof vi.fn>;

  beforeEach(() => {
    fetchMock = vi.fn();
    vi.stubGlobal("fetch", fetchMock);
    document.cookie = "csrftoken=token-123";
  });

  afterEach(() => {
    vi.unstubAllGlobals();
    document.cookie = "csrftoken=; expires=Thu, 01 Jan 1970 00:00:00 GMT";
  });

  it("PATCHes settings as JSON with the CSRF token", async () => {
    fetchMock.mockResolvedValue(jsonResponse({ mqtt_port: 1884 }));
    const body: SettingsUpdateRequest = { mqtt_port: 1884, reverse_color_temp: true };

    const { data } = await api.PATCH("/rest/v2/settings", { body });

    const request: Request = fetchMock.mock.calls[0][0];
    expect(new URL(request.url).pathname).toBe("/rest/v2/settings");
    expect(request.method).toBe("PATCH");
    expect(request.headers.get("X-CSRFToken")).toBe("token-123");
    expect(request.headers.get("Content-Type")).toBe("application/json");
    expect(await request.json()).toEqual({ mqtt_port: 1884, reverse_color_temp: true });
    expect(data?.mqtt_port).toBe(1884);
  });

  it("returns field errors from a 400", async () => {
    fetchMock.mockResolvedValue(jsonResponse({ mqtt_port: ["Ensure this value is less than or equal to 65535."] }, 400));

    const { data, error } = await api.PATCH("/rest/v2/settings", { body: { mqtt_port: 70000 } });

    expect(data).toBeUndefined();
    expect(error && "mqtt_port" in error ? error.mqtt_port : undefined).toEqual(["Ensure this value is less than or equal to 65535."]);
  });

  it("GETs settings without a CSRF token", async () => {
    fetchMock.mockResolvedValue(jsonResponse({ mqtt_port: 1883 }));

    await api.GET("/rest/v2/settings");

    const request: Request = fetchMock.mock.calls[0][0];
    expect(request.method).toBe("GET");
    expect(request.headers.has("X-CSRFToken")).toBe(false);
  });

  it("reads cookies by exact name", () => {
    document.cookie = "xcsrftoken=wrong";
    expect(getCookie("csrftoken")).toBe("token-123");
    expect(getCookie("missing")).toBe("");
  });
});
