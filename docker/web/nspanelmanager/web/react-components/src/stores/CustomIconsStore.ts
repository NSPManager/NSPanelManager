import { create } from "zustand";

// A Material Design Icon chosen as a custom icon. It is stored, where it is used, as `character`:
// a character in Unicode's private use area that the panels draw from the icon pack the manager
// renders. See web/icon_pack.py.
export interface ICustomIcon {
  name: string; // MDI name, e.g. "lamp"
  code_point: number;
  character: string;
}

interface CustomIconsState {
  icons: ICustomIcon[];
  // Names of all icons that can be chosen, loaded when the icon picker first opens.
  available: string[] | null;
  fetchIcons: () => Promise<void>;
  fetchAvailable: () => Promise<void>;
  // Register an MDI icon as a custom icon (registering it again is harmless) and return it.
  register: (name: string) => Promise<ICustomIcon>;
}

export const PRIVATE_USE_FIRST = 0xe000;
export const PRIVATE_USE_LAST = 0xf8ff;

export function isCustomIcon(icon: string | undefined | null): icon is string {
  if (!icon || icon.length !== 1) return false;
  const code = icon.charCodeAt(0);
  return code >= PRIVATE_USE_FIRST && code <= PRIVATE_USE_LAST;
}

function getCookie(name: string) {
  for (const cookie of document.cookie ? document.cookie.split(";") : []) {
    const trimmed = cookie.trim();
    if (trimmed.startsWith(name + "=")) {
      return decodeURIComponent(trimmed.substring(name.length + 1));
    }
  }
  return "";
}

export const useCustomIconsStore = create<CustomIconsState>()((set, get) => ({
  icons: [],
  available: null,

  fetchIcons: async () => {
    const response = await fetch("/rest/icons", { credentials: "same-origin" });
    if (response.ok) {
      set({ icons: (await response.json()).icons });
    }
  },

  fetchAvailable: async () => {
    if (get().available !== null) return;
    const response = await fetch("/rest/icons/available", { credentials: "same-origin" });
    if (response.ok) {
      set({ available: (await response.json()).names });
    }
  },

  register: async (name: string) => {
    const response = await fetch("/rest/icons", {
      credentials: "same-origin",
      method: "POST",
      mode: "same-origin",
      headers: { "Content-Type": "application/json", "X-CSRFToken": getCookie("csrftoken") },
      body: JSON.stringify({ name: name }),
    });
    const data = await response.json();
    if (!response.ok) {
      throw new Error(data.message || `${response.status}`);
    }
    const icon = data as ICustomIcon;
    if (!get().icons.some((existing) => existing.code_point === icon.code_point)) {
      set({ icons: [...get().icons, icon] });
    }
    return icon;
  },
}));
