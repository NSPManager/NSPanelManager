// The buzzer sounds: three RTTTL settings, one per panel event. A preset only fills in the RTTTL; the
// RTTTL is what is stored, so a preset is recognised by comparing strings.
//
// What the panel does with each setting (NSPanelManager-firmware BuzzerManager):
//   * Blank: no sound. Except "scene activated": a scene press then plays the touch sound, like any press.
//   * RTTTL the panel can't parse: no sound (the API rejects it, see web/rest_v2/rtttl.py).

export type BuzzerSetting = "buzzer_sound_touch" | "buzzer_sound_scene_activated" | "buzzer_sound_scene_saved";

export type Preset = { id: string; name: string; rtttl: string };

export const PRESETS = {
  tick: { id: "tick", name: "Tick", rtttl: "tick:d=64,o=7,b=625:b" },
  chime: { id: "chime", name: "Chime", rtttl: "chime:d=32,o=6,b=190:g6,16p,32c7" },
  confirm: { id: "confirm", name: "Confirm", rtttl: "confirm:d=16,o=6,b=357:e,g" },
  rising: { id: "rising", name: "Rising", rtttl: "rising:d=16,o=6,b=180:c,e,g" },
  saved: { id: "saved", name: "Saved", rtttl: "saved:d=16,o=6,b=200:c,e,g,8c7" },
  // A 4 ms pause: a scene press then plays this instead of the touch sound, i.e. nothing.
  silent: { id: "silent", name: "Silent", rtttl: "silent:d=64,o=6,b=900:p" },
} as const satisfies Record<string, Preset>;

export type BuzzerEvent = {
  name: BuzzerSetting;
  label: string;
  // What the dropdown calls a blank setting
  blankLabel: string;
  hint: string;
  // In the order they are offered: the ones meant for this event first
  presets: readonly Preset[];
};

export const BUZZER_EVENTS: readonly BuzzerEvent[] = [
  {
    name: "buzzer_sound_touch",
    label: "Screen touch",
    blankLabel: "Off",
    hint: "Played on every press of the screen.",
    presets: [PRESETS.tick, PRESETS.chime, PRESETS.confirm, PRESETS.rising, PRESETS.saved],
  },
  {
    name: "buzzer_sound_scene_activated",
    label: "Scene activated",
    blankLabel: "Same as screen touch",
    hint: "Played instead of the touch sound when a scene is activated from the panel.",
    presets: [PRESETS.silent, PRESETS.confirm, PRESETS.rising, PRESETS.chime, PRESETS.tick, PRESETS.saved],
  },
  {
    name: "buzzer_sound_scene_saved",
    label: "Scene saved",
    blankLabel: "Off",
    hint: "Played when a scene is saved with a long press on the panel.",
    presets: [PRESETS.saved, PRESETS.rising, PRESETS.confirm, PRESETS.chime, PRESETS.tick],
  },
];

export const BLANK = "blank";
export const CUSTOM = "custom";

/** The dropdown choice for a stored value: BLANK, CUSTOM or a preset id. */
export function choiceFor(event: BuzzerEvent, rtttl: string): string {
  if (rtttl === "") {
    return BLANK;
  }
  return event.presets.find((preset) => preset.rtttl === rtttl)?.id ?? CUSTOM;
}
