import { create } from "zustand";
import { api, type Room } from "../api/client";

interface ISettingsStore {
  status: "none" | "loading" | "loaded";
  rooms: Room[] | null;
  fetchData: () => Promise<void>;
}

export const useRoomsStore = create<ISettingsStore>((set) => ({
  status: "none",
  rooms: null,
  fetchData: async () => {
    set({ status: "loading" });
    const { data } = await api.GET("/rest/rooms");
    if (data) {
      set((state) => ({ ...state, status: "loaded", rooms: data.rooms }));
    }
  },
}));
