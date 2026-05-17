import { create } from "zustand";

interface IEntitiesPageData {
  id: number;
  display_order: number;
  number_of_entities: number;
  type: string;
  room_id: number;
}

export interface IEntityOrSceneData {
  base: {
    id: number;
    friendly_name: string;
    room_view_position: number;
    type: "scene" | "entity";
    entity_type: string;
    entities_page_id: number;
  };
  entity?: any;
  scene?: any;
}

interface IEntitiesPages {
  status: string;
  room_id: number | null;
  entities_pages: IEntitiesPageData[];
  entities: IEntityOrSceneData[];
  scenes: IEntityOrSceneData[];
  fetchData: (room_id: number) => Promise<void>;
  removeEntitiesPage: (id: number) => void;
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
  fetchData: async (room_id: number) => {
    set({ status: "loading", room_id: room_id });
    fetch(`/rest/rooms/${room_id}/entities_pages`, {
      credentials: "same-origin",
      method: "GET",
      mode: "same-origin",
      headers: { "Content-Type": "application/json", "X-CSRFToken": getCookie("csrftoken") },
    })
      .then((response) => response.json())
      .then((data) => {
        set((state) => ({ ...state, status: "loaded", room_id, entities_pages: data.entities_pages }));

        for (const page of data.entities_pages) {
          fetch(`/rest/rooms/${page.room_id}/entities_pages/${page.id}`, {
            credentials: "same-origin",
            method: "GET",
            mode: "same-origin",
            headers: { "Content-Type": "application/json", "X-CSRFToken": getCookie("csrftoken") },
          })
            .then((response) => response.json())
            .then((data) => {
              set((state) => ({ ...state, entities: [...state.entities, ...data.entities], scenes: [...state.scenes, ...data.scenes] }));
            });
        }
      });
  },
  removeEntitiesPage: (id: number) => {
    set((state) => ({
      entities_pages: state.entities_pages.filter((page) => page.id !== id),
    }));
  },
  setScenePosition: (id: number, entities_page_id: number, room_view_position: number) => {
    set((state) => ({
      ...state,
      scenes: state.scenes.map((scene) =>
        scene.base.id === id ? { ...scene, base: { ...scene.base, entities_page_id: entities_page_id, room_view_position: room_view_position } } : scene,
      ),
    }));
  },
  setEntityPosition: (id: number, entities_page_id: number, room_view_position: number) => {
    set((state) => ({
      ...state,
      entities: state.entities.map((entity) =>
        entity.base.id === id ? { ...entity, base: { ...entity.base, entities_page_id: entities_page_id, room_view_position: room_view_position } } : entity,
      ),
    }));
  },
  removeEntity: (id: number) => {
    set((state) => ({
      entities: state.entities.filter((entity) => entity.base.id !== id),
    }));
  },
  removeScene: (id: number) => {
    set((state) => ({
      scenes: state.scenes.filter((scene) => scene.base.id !== id),
    }));
  },
}));
