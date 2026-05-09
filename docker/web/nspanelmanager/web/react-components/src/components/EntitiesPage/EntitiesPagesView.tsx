import { useState } from "react";
import EntitiesPage from "./EntitiesPage";
import { DragDropProvider } from "@dnd-kit/react";

export interface IEntityOrSceneData {
  base: {
    id: number;
    friendly_name: string;
    room_view_position: number | null;
    type: "scene" | "entity";
    entities_page_id: number;
  };
  entity?: any;
  scene?: any;
}

export interface IEntityPageData {
  status: string;
  entities: IEntityOrSceneData[];
  scenes: IEntityOrSceneData[];
}

const EntitiesPagesView = ({ room_id, type }: { room_id: number; type: string }) => {
  const [viewData, _setViewData] = useState({
    room_id: room_id,
    type: type, // "entity" or "scene"
  });

  interface EntitiesPageData {
    id: number;
    display_order: number;
    number_of_entities: number;
    type: string;
    room_id: number;
  }
  interface EntitiesPages {
    status: string;
    entities_pages: EntitiesPageData[];
  }

  const [hasFetchedEntitiesPages, setHasFetchedEntitiesPages] = useState(false);
  const [hasFetchedEntities, setHasFetchedEntities] = useState(false);
  const [entitiesPages, setEntitiesPages] = useState<EntitiesPages>({ status: "loading", entities_pages: [] });
  const [entities, setEntities] = useState<IEntityOrSceneData[]>([]);
  const [scenes, setScenes] = useState<IEntityOrSceneData[]>([]);

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

  if (!hasFetchedEntitiesPages) {
    fetch(`/rest/rooms/${room_id}/entities_pages`, {
      credentials: "same-origin",
      method: "GET",
      mode: "same-origin",
      headers: { "Content-Type": "application/json", "X-CSRFToken": getCookie("csrftoken") },
    })
      .then((response) => response.json())
      .then((data) => {
        setEntitiesPages(data);
        setHasFetchedEntitiesPages(true);
      });
  }

  if (hasFetchedEntitiesPages && !hasFetchedEntities) {
    for (const page of entitiesPages.entities_pages) {
      fetch(`/rest/rooms/${page.room_id}/entities_pages/${page.id}`, {
        credentials: "same-origin",
        method: "GET",
        mode: "same-origin",
        headers: { "Content-Type": "application/json", "X-CSRFToken": getCookie("csrftoken") },
      })
        .then((response) => response.json())
        .then((data) => {
          setEntities((prevEntities) => [...prevEntities, ...data.entities]);
          setScenes((prevScenes) => [...prevScenes, ...data.scenes]);
        });
    }
    setHasFetchedEntities(true);
  }

  function handleDragEnd(event) {
    if (event.canceled) return;
    const { source, target } = event.operation;
    const sourceData = JSON.parse(source.id);
    const targetData = JSON.parse(target.id);

    if (sourceData.base.entities_page_id == targetData.entities_page_id && sourceData.base.room_view_position == targetData.room_view_position) {
      return; // We did in fact not move the item.
    }

    // if (over && over.data.current.accepts.includes(active.data.current.type)) {
    if (sourceData.base.type == "entity") {
      for (const entity of entities) {
        if (entity.base.id == sourceData.base.id) {
          const updatedEntities = entities.filter((e) => e.base.id != sourceData.base.id);
          const updatedEntity = entity;
          updatedEntity.base.entities_page_id = targetData.entities_page_id;
          updatedEntity.base.room_view_position = targetData.room_view_position;
          setEntities([...updatedEntities, updatedEntity]);
          break;
        }
      }
    }

    // }
  }

  return (
    <DragDropProvider onDragEnd={handleDragEnd}>
      <ul className="list-none w-full nspanel-entities-pages">
        {(() => {
          if (!("entities_pages" in entitiesPages)) return [];
          if (!hasFetchedEntitiesPages) return [];

          const items = [];
          for (const entity_page of entitiesPages.entities_pages) {
            if (entity_page.type !== type) continue;

            items.push(
              <EntitiesPage
                id={entity_page.id}
                room_id={viewData.room_id}
                type={viewData.type}
                number_of_entities={entity_page.number_of_entities}
                can_remove={entitiesPages.entities_pages.length > 1}
                entities={entity_page.type == "entity" ? entities : scenes}
              ></EntitiesPage>,
            );
          }
          return items;
        })()}
      </ul>
    </DragDropProvider>
  );
};

export default EntitiesPagesView;
