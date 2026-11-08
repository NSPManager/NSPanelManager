import { describe, expect, it } from "vitest";
import { buildPatch, PAGE_FIELDS, toFormValues } from "./form";
import { settingsFixture } from "./testFixtures";

describe("toFormValues", () => {
  it("copies the page's settings and leaves the secrets blank", () => {
    const values = toFormValues(settingsFixture({ manager_port: null }));

    expect(Object.keys(values).sort()).toEqual(
      [...PAGE_FIELDS, "mqtt_password", "home_assistant_token", "openhab_token", "clear_mqtt_password", "clear_home_assistant_token", "clear_openhab_token"].sort(),
    );
    expect(values.mqtt_port).toBe(1883);
    expect(values.manager_port).toBeNull();
    expect(values.mqtt_password).toBe("");
    expect(values.clear_mqtt_password).toBe(false);
  });
});

describe("buildPatch", () => {
  const values = () => toFormValues(settingsFixture());

  it("sends only the changed settings", () => {
    expect(buildPatch({ ...values(), mqtt_port: 1884, mqtt_server: "other" }, { mqtt_port: true }, false)).toEqual({ mqtt_port: 1884 });
  });

  it("sends nothing when nothing changed", () => {
    expect(buildPatch(values(), {}, false)).toEqual({});
  });

  it("sends a typed secret, or blank to clear one, and otherwise leaves secrets alone", () => {
    expect(buildPatch({ ...values(), mqtt_password: "new" }, { mqtt_password: true }, false)).toEqual({ mqtt_password: "new" });
    expect(buildPatch({ ...values(), mqtt_password: "typed", clear_mqtt_password: true }, { clear_mqtt_password: true }, false)).toEqual({ mqtt_password: "" });
    expect(buildPatch({ ...values(), mqtt_password: "" }, {}, false)).toEqual({});
  });

  it("never sends the Home Assistant settings in the add-on", () => {
    const changed = { ...values(), home_assistant_address: "http://elsewhere", home_assistant_token: "t", mqtt_port: 1884 };

    expect(buildPatch(changed, { home_assistant_address: true, home_assistant_token: true, mqtt_port: true }, true)).toEqual({ mqtt_port: 1884 });
    expect(buildPatch(changed, { home_assistant_address: true, home_assistant_token: true, mqtt_port: true }, false)).toEqual({
      home_assistant_address: "http://elsewhere",
      home_assistant_token: "t",
      mqtt_port: 1884,
    });
  });
});
