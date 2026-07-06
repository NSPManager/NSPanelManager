import { create } from "zustand";

interface ISettings {
  button_long_press_time: number;
  clock_us_style: boolean;
  color_temp_max: number;
  color_temp_min: number;
  theme: string;
  date_format: string;
  is_home_assistant_addon: boolean;
  home_assistant_address: string;
  home_assistant_token_set: boolean;
  location_latitude: string;
  location_longitude: string;
  manager_address: string;
  manager_port: number;
  max_live_log_messages: number;
  max_log_buffer_size: number;
  min_button_push_time: number;
  mqttmanager_log_level: number;
  mqtt_password_set: boolean;
  mqtt_port: number;
  mqtt_server: string;
  mqtt_username: string;
  openhab_address: string;
  openhab_token_set: boolean;
  openhab_brightness_channel_max: number;
  openhab_brightness_channel_min: number;
  openhab_rgb_channel_name: string;
  outside_temp_sensor_entity_id: string;
  outside_temp_sensor_provider: string;
  weather_precipitation_format: string;
  raise_to_100_light_level: boolean;
  all_rooms_status_backoff_time: number;
  reverse_color_temp: boolean;
  screen_dim_level: number;
  screensaver_activation_timeout: number;
  screensaver_dim_level: number;
  screensaver_mode: string;
  show_screensaver_inside_temperature: boolean;
  show_screensaver_outside_temperature: boolean;
  special_mode_release_time: number;
  special_mode_trigger_time: number;
  turn_on_behavior: string;
  use_fahrenheit: boolean;
  weather_update_interval: number;
  weather_wind_speed_format: string;
  mqtt_wait_time: number;
  optimistic_mode: boolean;
  light_turn_on_brightness: number;
  default_nspanel_type: string;
}

interface ISettingsStore {
  status: "none" | "loading" | "loaded";
  settings: ISettings | null;
  fetchData: () => Promise<void>;
}

function getCookie(name: string) {
  let cookieValue = "";
  if (document.cookie && document.cookie !== "") {
    const cookies = document.cookie.split(";");
    for (let i = 0; i < cookies.length; i++) {
      const cookie = cookies[i].trim();
      if (cookie.substring(0, name.length + 1) === name + "=") {
        cookieValue = decodeURIComponent(cookie.substring(name.length + 1));
        break;
      }
    }
  }
  return cookieValue;
}

export const useSettingsStore = create<ISettingsStore>((set) => ({
  status: "none",
  settings: null,
  fetchData: async () => {
    set({ status: "loading" });
    fetch(`/rest/settings`, {
      credentials: "same-origin",
      method: "GET",
      mode: "same-origin",
      headers: { "Content-Type": "application/json", "X-CSRFToken": getCookie("csrftoken") },
    })
      .then((response) => response.json())
      .then((data) => {
        set((state) => ({ ...state, settings: data.settings }));
        set((state) => ({ ...state, status: "loaded" }));
      });
  },
}));
