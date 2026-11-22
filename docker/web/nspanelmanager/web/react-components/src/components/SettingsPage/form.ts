import type { Settings, SettingsUpdateRequest } from "../../api/client";

// The settings this page edits. Weather and time, and the theme, are edited elsewhere.
export const PAGE_FIELDS = [
  "manager_address",
  "manager_port",
  "mqttmanager_log_level",
  "max_live_log_messages",
  "max_log_buffer_size",
  "default_nspanel_type",
  "color_temp_min",
  "color_temp_max",
  "reverse_color_temp",
  "turn_on_behavior",
  "raise_to_100_light_level",
  "min_button_push_time",
  "button_long_press_time",
  "special_mode_trigger_time",
  "special_mode_release_time",
  "screen_dim_level",
  "screensaver_dim_level",
  "screensaver_activation_timeout",
  "screensaver_mode",
  "show_screensaver_inside_temperature",
  "show_screensaver_outside_temperature",
  "optimistic_mode",
  "room_status_backoff_time",
  "all_rooms_status_backoff_time",
  "light_turn_on_brightness",
  "mqtt_server",
  "mqtt_port",
  "mqtt_username",
  "home_assistant_address",
  "openhab_address",
  "buzzer_sound_touch",
  "buzzer_sound_scene_activated",
  "buzzer_sound_scene_saved",
] as const satisfies readonly (keyof Settings & keyof SettingsUpdateRequest)[];

export const SECRETS = ["mqtt_password", "home_assistant_token", "openhab_token"] as const;

export type PageField = (typeof PAGE_FIELDS)[number];
export type Secret = (typeof SECRETS)[number];

// Values are null when nothing valid is stored; such a field is left blank and sent only if filled in.
export type SettingsFormValues = { [K in PageField]: Settings[K] } & { [K in Secret]: string } & { [K in Secret as `clear_${K}`]: boolean };

// Set by the Home Assistant add-on, so read-only there.
export const ADDON_MANAGED: readonly string[] = ["home_assistant_address", "home_assistant_token"];

// The screensaver shows the outside temperature only in the modes without weather.
export const MODES_WITH_OUTSIDE_TEMPERATURE: readonly string[] = ["datetime_with_background", "datetime_without_background"];

export function toFormValues(settings: Settings): SettingsFormValues {
  const values = {} as Record<string, unknown>;
  for (const name of PAGE_FIELDS) {
    values[name] = settings[name];
  }
  for (const secret of SECRETS) {
    values[secret] = "";
    values[`clear_${secret}`] = false;
  }
  return values as SettingsFormValues;
}

/**
 * The PATCH body for a submitted form: only the settings that changed. Secrets are sent when a new
 * value is typed, or as "" when "clear" is ticked. Add-on managed settings are never sent in the add-on.
 */
export function buildPatch(values: SettingsFormValues, dirty: Partial<Record<keyof SettingsFormValues, unknown>>, isHomeAssistantAddon: boolean): SettingsUpdateRequest {
  const body: Record<string, unknown> = {};
  const allowed = (name: string) => !(isHomeAssistantAddon && ADDON_MANAGED.includes(name));
  for (const name of PAGE_FIELDS) {
    if (dirty[name] && allowed(name)) {
      body[name] = values[name];
    }
  }
  for (const secret of SECRETS) {
    if (!allowed(secret)) {
      continue;
    }
    if (values[`clear_${secret}`]) {
      body[secret] = "";
    } else if (values[secret]) {
      body[secret] = values[secret];
    }
  }
  return body as SettingsUpdateRequest;
}
