import { create } from "zustand";
import { useStompStore } from "./StompStore";

export interface IRoomData {
  id: number;
  friendly_name: string;
}

interface INSPanelInfoData {
  id: number;
  mac: string;
}

export interface INSPanelWarningData {
  level: "error" | "warning" | "info" | "debug";
  text: string;
}

export interface INSPanelStatusData {
  id: number;
  mac: string;
  name: string;
  ip_address: string;
  rssi: number;
  temperature: number;
  temperature_unit: string;
  humidity?: number;
  pressure?: number;
  ram_usage: number;
  update_progress: number;
  warnings: INSPanelWarningData[];
  accepted: boolean;
  denied: boolean;
  model: "web" | "sonoff" | "custom" | "unknown";
  state: "online" | "offline" | "updating_fw" | "updating_fs" | "updating_tft" | "waiting" | "awaiting_accept" | "rebooting" | "unknown";
}

// interface IEntityStatesData {
//   nspanels: INSPanelInfoData[];
// }

// export interface INSpanelData {
//   type: "entity" | "scene" | "entity_page";
//   config?: IEntityOrSceneData;
//   entity_page_config?: IEntityPageData;
// }

interface IEntityStatesStore {
  status: "none" | "loading" | "loaded";
  _nspanels: INSPanelInfoData[];
  nspanels: { [key: string]: INSPanelStatusData };
  rooms: { [key: number]: IRoomData };
  fetchData: () => Promise<void>;
  _update_nspanel_status: (mac: string, status: INSPanelStatusData) => void;
  _attachNSPanelStatusDataSubscriptions: () => Promise<void>;
}

// function getCookie(name: string) {
//   let cookieValue = "";
//   if (document.cookie && document.cookie !== "") {
//     const cookies = document.cookie.split(";");
//     for (let i = 0; i < cookies.length; i++) {
//       const cookie = cookies[i].trim();
//       if (cookie.substring(0, name.length + 1) === name + "=") {
//         cookieValue = decodeURIComponent(cookie.substring(name.length + 1));
//         break;
//       }
//     }
//   }
//   return cookieValue;
// }

const nspanel_stomp_status_update_callback = (message: any) => {
  const json_data = JSON.parse(message.body);
  useEntityStatesStore.getState()._update_nspanel_status(json_data.mac, json_data);
};

export const useEntityStatesStore = create<IEntityStatesStore>((set) => ({
  status: "none",
  _nspanels: [],
  nspanels: {},
  rooms: {},
  fetchData: async () => {
    set({ status: "loading" });
    if (useStompStore.getState().status == "none") {
      useStompStore.getState().connect();
    }
    useStompStore.getState().subscribe("entity_states", (message) => {
      const json_data = JSON.parse(message.body);
      set({ _nspanels: json_data.nspanels, status: "loaded" });
      useEntityStatesStore.getState()._attachNSPanelStatusDataSubscriptions();
    });

    fetch("/rest/rooms")
      .then((response) => response.json())
      .then((data) => {
        for (const room of data.rooms) {
          set({ rooms: { ...useEntityStatesStore.getState().rooms, [room.id]: room } });
        }
      });
  },
  _update_nspanel_status: (mac: string, status: INSPanelStatusData) => {
    set({ nspanels: { ...useEntityStatesStore.getState().nspanels, [mac]: status } });
  },
  _attachNSPanelStatusDataSubscriptions: async () => {
    for (const panel of Object.entries(useEntityStatesStore.getState().nspanels)) {
      let nspanel_removed = true;
      for (const existingPanel of useEntityStatesStore.getState()._nspanels) {
        if (existingPanel.mac === panel[0]) {
          nspanel_removed = false;
          break;
        }
      }
      if (nspanel_removed) {
        console.log("Removing ", panel[0]);
        const current_nspanels = useEntityStatesStore.getState().nspanels;
        delete current_nspanels[panel[0]];
        useStompStore.getState().unsubscribe(`nspanel/${panel[0]}/status`, nspanel_stomp_status_update_callback);
        set({ nspanels: { ...current_nspanels } });
      }
    }

    for (const panel of useEntityStatesStore.getState()._nspanels) {
      useStompStore.getState().subscribe(`nspanel/${panel.mac}/status`, nspanel_stomp_status_update_callback);
    }
  },
}));
