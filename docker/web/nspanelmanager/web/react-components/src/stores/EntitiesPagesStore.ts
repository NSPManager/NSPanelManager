import { create } from "zustand";

interface IEntitiesPageData {
  id: number;
  display_order: number;
  number_of_entities: number;
  type: string;
  room_id: number;
}

export interface IEntityOrSceneData {
  id: number;
  friendly_name: string;
  room_view_position: number;
  type: "scene" | "entity";
  entity_type: string;
  entities_page_id: number;
  controller: string;
  controlled_by_nspanel_main_page: boolean;
  step_size?: number;
}

export interface IEntityPageData {
  id: number;
  display_order: number;
  number_of_entities: number;
  type: string;
  room_id: number;
}

export interface IDragingItemData {
  type: "entity" | "scene" | "entity_page";
  config?: IEntityOrSceneData;
  entity_page_config?: IEntityPageData;
}

interface IEntitiesPages {
  status: string;
  room_id: number | null;
  entities_pages: IEntitiesPageData[];
  entities: IEntityOrSceneData[];
  scenes: IEntityOrSceneData[];
  fetchData: (room_id?: number) => Promise<void>;
  removeEntitiesPage: (id: number) => void;
  createEntitiesPage: (room_id: number, is_scenes_page: boolean, is_global_scenes_page: boolean) => void;
  setEntityPosition: (id: number, entities_page_id: number, room_view_position: number) => void;
  setScenePosition: (id: number, entities_page_id: number, room_view_position: number) => void;
  removeEntity: (id: number) => void;
  removeScene: (id: number) => void;
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

export const useEntitiesPagesStore = create<IEntitiesPages>((set) => ({
  status: "loading",
  room_id: null,
  entities_pages: [],
  entities: [],
  scenes: [],
  fetchData: async (room_id?: number) => {
    set({ status: "loading", room_id: room_id });
    let url = `/rest/global/entities_pages`;
    if (room_id !== undefined) {
      url = `/rest/rooms/${room_id}/entities_pages`;
    }
    fetch(url, {
      credentials: "same-origin",
      method: "GET",
      mode: "same-origin",
      headers: { "Content-Type": "application/json", "X-CSRFToken": getCookie("csrftoken") },
    })
      .then((response) => response.json())
      .then((data) => {
        set((state) => ({ ...state, room_id, entities_pages: data.entities_pages, entities: [], scenes: [] }));

        for (const page of data.entities_pages) {
          set((state) => ({ ...state, entities: [...state.entities, ...page.entities], scenes: [...state.scenes, ...page.scenes] }));
        }

        set({ status: "loaded" });
      });
  },
  removeEntitiesPage: (id: number) => {
    set((state) => ({
      entities_pages: state.entities_pages.filter((page) => page.id !== id),
    }));
  },
  createEntitiesPage: (room_id: number, is_scenes_page: boolean) => {
    set((state) => ({
      entities_pages: [
        ...state.entities_pages,
        { id: state.entities_pages.length + 1, display_order: 0, number_of_entities: 0, type: is_scenes_page ? "scene" : "entity", room_id: room_id },
      ],
    }));
  },
  setScenePosition: (id: number, entities_page_id: number, room_view_position: number) => {
    set((state) => ({
      ...state,
      scenes: state.scenes.map((scene) => (scene.id === id ? { ...scene, entities_page_id: entities_page_id, room_view_position: room_view_position } : scene)),
    }));
  },
  setEntityPosition: (id: number, entities_page_id: number, room_view_position: number) => {
    set((state) => ({
      ...state,
      entities: state.entities.map((entity) =>
        entity.id === id ? { ...entity, entities_page_id: entities_page_id, room_view_position: room_view_position } : entity,
      ),
    }));
  },
  removeEntity: (id: number) => {
    set((state) => ({
      entities: state.entities.filter((entity) => entity.id !== id),
    }));
  },
  removeScene: (id: number) => {
    set((state) => ({
      scenes: state.scenes.filter((scene) => scene.id !== id),
    }));
  },
}));
