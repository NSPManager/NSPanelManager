import type { Settings } from "../../api/client";

// Labels for each allowed value. `satisfies` makes tsc fail if a value is missing or one the server
// doesn't allow (the types come from docker/web/openapi.yaml), so these can't drift from the API.
type Choices<K extends keyof Settings> = Record<Exclude<Extract<Settings[K], string>, "">, string>;

export const LOG_LEVELS = { error: "Error", warning: "Warning", info: "Info", debug: "Debug", trace: "Trace" } satisfies Choices<"mqttmanager_log_level">;

export const NSPANEL_TYPES = { eu: "EU", us: "US" } satisfies Choices<"default_nspanel_type">;

export const TURN_ON_BEHAVIORS = { color_temp: "Color temperature", restore: "Restore" } satisfies Choices<"turn_on_behavior">;

export const SCREENSAVER_MODES = {
  with_background: "Show with background (clock, date and weather)",
  without_background: "Show without background (clock, date and weather)",
  datetime_with_background: "Show with background (no weather)",
  datetime_without_background: "Show without background (no weather)",
  no_screensaver: "No screensaver (turn off screen)",
} satisfies Choices<"screensaver_mode">;

export const options = <T extends string>(choices: Record<T, string>) => Object.entries(choices) as [T, string][];
