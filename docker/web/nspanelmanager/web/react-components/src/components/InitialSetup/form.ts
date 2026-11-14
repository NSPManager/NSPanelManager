import type { Settings, SettingsUpdateRequest } from "../../api/client";

// The settings this page edits. Weather and time, and the theme, are edited elsewhere.
export const INITIAL_SETUP_FIELDS = [
  "manager_address",
  "manager_port",
  "mqtt_server",
  "mqtt_port",
  "mqtt_username",
  "home_assistant_address",
  "openhab_address",
  "theme",
] as const satisfies readonly (keyof Settings & keyof SettingsUpdateRequest)[];

export const SECRETS = ["mqtt_password", "home_assistant_token", "openhab_token"] as const;

export type PageField = (typeof INITIAL_SETUP_FIELDS)[number];
export type Secret = (typeof SECRETS)[number];

// Values are null when nothing valid is stored; such a field is left blank and sent only if filled in.
export type InitialSetupFormValues = { [K in PageField]: Settings[K] } & { [K in Secret]: string } & { [K in Secret as `clear_${K}`]: boolean };

// Set by the Home Assistant add-on, so read-only there.
export const ADDON_MANAGED: readonly string[] = ["home_assistant_address", "home_assistant_token"];

// The screensaver shows the outside temperature only in the modes without weather.
export const MODES_WITH_OUTSIDE_TEMPERATURE: readonly string[] = ["datetime_with_background", "datetime_without_background"];

export function toFormValues(settings: Settings): InitialSetupFormValues {
  const values = {} as Record<string, unknown>;
  for (const name of INITIAL_SETUP_FIELDS) {
    values[name] = settings[name];
  }
  for (const secret of SECRETS) {
    values[secret] = "";
    values[`clear_${secret}`] = false;
  }
  return values as InitialSetupFormValues;
}

/**
 * The PATCH body for a submitted form: only the settings that changed. Secrets are sent when a new
 * value is typed, or as "" when "clear" is ticked. Add-on managed settings are never sent in the add-on.
 */
export function buildPatch(
  values: InitialSetupFormValues,
  dirty: Partial<Record<keyof InitialSetupFormValues, unknown>>,
  isHomeAssistantAddon: boolean,
): SettingsUpdateRequest {
  const body: Record<string, unknown> = {};
  const allowed = (name: string) => !(isHomeAssistantAddon && ADDON_MANAGED.includes(name));
  for (const name of INITIAL_SETUP_FIELDS) {
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
