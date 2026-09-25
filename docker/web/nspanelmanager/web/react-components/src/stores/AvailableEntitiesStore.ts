import { create } from "zustand";

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

export interface IOptionType {
  value: string;
  label: string;
  icon?: string;
}

interface IAvailableEntitiesStore {
  status: "none" | "loading" | "loaded";
  home_assistant_entities: any[];
  home_assistant_options: IOptionType[];
  openhab_items: any[];
  openhab_options: IOptionType[];
  fetchEntities: () => Promise<void>;
}

function getOptionsFromItems(items: any[]): IOptionType[] {
  const options: IOptionType[] = [];
  for (const item of items) {
    let mdi_icon = "mdi-help"; // Default to a question mark when no icon is found for a given entity type.
    if (item.item_id.startsWith("light")) {
      mdi_icon = "mdi-lightbulb";
    } else if (item.item_id.startsWith("input_boolean")) {
      mdi_icon = "mdi-toggle-switch-variant";
    } else if (item.item_id.startsWith("switch")) {
      mdi_icon = "mdi-toggle-switch-variant";
    } else if (item.item_id.startsWith("button") || item.item_id.startsWith("input_button")) {
      mdi_icon = "mdi-gesture-tap-button";
    } else if (item.item_id.startsWith("climate")) {
      mdi_icon = "mdi-thermostat";
    } else if (item.item_id.startsWith("scene")) {
      mdi_icon = "mdi-palette";
    } else if (item.item_id.startsWith("script")) {
      mdi_icon = "mdi-code-json";
    }

    options.push({
      value: item.item_id,
      label: item.label,
      icon: mdi_icon,
    });
  }
  return options;
}

export const useAvailableEntitiesStore = create<IAvailableEntitiesStore>((set) => ({
  status: "none",
  home_assistant_entities: [],
  home_assistant_options: [],
  openhab_items: [],
  openhab_options: [],
  fetchEntities: async () => {
    set({ status: "loading" });

    fetch(`/rest/home_assistant/entities`, {
      credentials: "same-origin",
      method: "GET",
      mode: "same-origin",
      headers: { "Content-Type": "application/json", "X-CSRFToken": getCookie("csrftoken") },
    })
      .then((response) => response.json())
      .then((data) => {
        set((state) => ({ ...state, status: "loading", home_assistant_entities: data.items }));
        set((state) => ({ ...state, home_assistant_options: getOptionsFromItems(data.items) }));

        fetch(`/rest/openhab/items`, {
          credentials: "same-origin",
          method: "GET",
          mode: "same-origin",
          headers: { "Content-Type": "application/json", "X-CSRFToken": getCookie("csrftoken") },
        })
          .then((response) => response.json())
          .then((data) => {
            set((state) => ({ ...state, status: "loaded", openhab_items: data.items, openhab_options: getOptionsFromItems(data.items) }));
          });
      });
  },
}));
