import { describe, expect, it } from "vitest";
import { parseRtttl } from "../../lib/rtttl";
import { BUZZER_EVENTS, PRESETS } from "./buzzer";

describe("buzzer presets", () => {
  it.each(Object.values(PRESETS).map((preset) => [preset.name, preset] as const))("%s is RTTTL the panel accepts", (_, preset) => {
    expect(parseRtttl(preset.rtttl)).toMatchObject({ ok: true });
  });

  it("plays the softer touch sounds as intended", () => {
    // [frequency Hz, duration ms], as the panel plays them
    expect(parseRtttl(PRESETS.pip.rtttl)).toMatchObject({ steps: [[2093, 4]] });
    expect(parseRtttl(PRESETS.midTick.rtttl)).toMatchObject({ steps: [[1568, 6]] });
    expect(parseRtttl(PRESETS.tuk.rtttl)).toMatchObject({ steps: [[1319, 4], [1047, 4]] });
    expect(parseRtttl(PRESETS.tock.rtttl)).toMatchObject({ steps: [[880, 4]] });
  });

  it("offers every sounding preset for every event", () => {
    const sounding = Object.values(PRESETS).filter((preset) => preset !== PRESETS.silent);
    for (const event of BUZZER_EVENTS) {
      expect(event.presets).toEqual(expect.arrayContaining(sounding));
    }
  });
});
