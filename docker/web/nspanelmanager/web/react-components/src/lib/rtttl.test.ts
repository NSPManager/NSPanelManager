import { describe, expect, it } from "vitest";
// The panel firmware's own parser output. The server's rtttl.py is tested against the same file.
import fixture from "../../../tests/rtttl_vectors.json";
import { parseRtttl } from "./rtttl";

type Vector = { rtttl: string; steps?: number[][]; error?: "invalid" | "too_long" };
const vectors = fixture.vectors as Vector[];

describe("parseRtttl", () => {
  it("has vectors for every outcome", () => {
    expect(vectors.length).toBeGreaterThan(50);
    expect(new Set(vectors.map((vector) => vector.error ?? "ok"))).toEqual(new Set(["ok", "invalid", "too_long"]));
  });

  it.each(vectors.map((vector) => [JSON.stringify(vector.rtttl).slice(0, 60), vector] as const))("matches the firmware for %s", (_, vector) => {
    const result = parseRtttl(vector.rtttl);
    if (vector.steps) {
      expect(result).toEqual({ ok: true, steps: vector.steps, durationMs: vector.steps.reduce((total, step) => total + step[1], 0) });
    } else {
      expect(result).toMatchObject({ ok: false, tooLong: vector.error === "too_long" });
    }
  });

  it("does not fold Unicode", () => {
    for (const text of ["::c ", "::١c", "::c٢", "：：c"]) {
      expect(parseRtttl(text).ok).toBe(false);
    }
  });

  // The same messages as web/rest_v2/rtttl.py, so the inline error and a server error read alike.
  it.each([
    ["c,d", "name:defaults:notes"],
    [":b=901:c", "b must be 1 to 900"],
    [":o=9:c", "o must be 1 to 8"],
    [":d=3:c", "d must be 1, 2, 4"],
    [":x=1:c", "Unknown default 'x'"],
    ["::c,e,q", "Note 3 ('q')"],
    ["::c,,d", "Note 2 ('')"],
    ["x:d=1,o=6,b=8:c,d", "at most 30 seconds"],
  ])("explains %s", (text, message) => {
    const result = parseRtttl(text);
    expect(result.ok).toBe(false);
    expect(!result.ok && result.error).toContain(message);
  });
});
