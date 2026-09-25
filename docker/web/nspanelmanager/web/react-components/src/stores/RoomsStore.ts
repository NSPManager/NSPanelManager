import { create } from "zustand";

interface IRoomSettings {
  id: number;
  name: string;
}

interface ISettingsStore {
  status: "none" | "loading" | "loaded";
  rooms: IRoomSettings[] | null;
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

export const useRoomsStore = create<ISettingsStore>((set) => ({
  status: "none",
  rooms: null,
  fetchData: async () => {
    set({ status: "loading" });
    fetch(`/rest/rooms`, {
      credentials: "same-origin",
      method: "GET",
      mode: "same-origin",
      headers: { "Content-Type": "application/json", "X-CSRFToken": getCookie("csrftoken") },
    })
      .then((response) => response.json())
      .then((data) => {
        set((state) => ({ ...state, status: "loaded", rooms: data.rooms }));
      });
  },
}));
