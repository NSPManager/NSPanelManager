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
  const [dragingItem, setDragingItem] = useState<IEntityOrSceneData | undefined>(undefined);
  // const [sourceDragDroppable, setSourceDragDroppable] = useState<Droppable>(undefined);
  // const [entitiesPagesElements, setEntitiesPagesElements] = useState<React.ReactNode[]>([]);

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

  async function saveEntitiesOrder(entities: IEntityOrSceneData[], scenes: IEntityOrSceneData[]) {
    // PUT request using fetch with error handling
    fetch(`/rest/rooms/${room_id}/entities/order`, {
      credentials: "same-origin",
      method: "PUT",
      mode: "same-origin",
      headers: { "Content-Type": "application/json", "X-CSRFToken": getCookie("csrftoken") },
      body: JSON.stringify({
        entities: entities,
        scenes: scenes,
      }),
    })
      .then(async (response) => {
        const data = await response.json();

        // check for error response
        if (!response.ok) {
          // get error message from body or default to response status
          const error = (data && data.message) || response.status;
          return Promise.reject(error);
        }

        // for (let i = 0; i < document.getElementsByTagName("dialog").length; i++) {
        //   const dialog = document.getElementsByTagName("dialog")[i];
        //   if (dialog.id == "modal") {
        //     dialog.close();
        //   }
        // }

        // setPostId(data.id);
      })
      .catch((error) => {
        // setErrorMessage(error);
        console.error("There was an error!", error);
      });
  }

  return (
    <DragDropProvider
      // plugins={(defaults) => [
      //   ...defaults,
      //   Feedback.configure({
      //     feedback: "move",
      //   }),
      // ]}
      onDragStart={(event) => {
        const { source } = event.operation;
        if (source == null) return;
        // console.log(source.droppable);
        setDragingItem(source.data.config);
        // setSourceDragDroppable(droppable);
      }}
      onDragEnd={(event) => {
        setDragingItem(undefined);

        if (event.canceled) return;
        const { source, target } = event.operation;
        if (source == null || target == null) return;

        const sourceData = source.data.config;
        const targetData = target.data.config;

        if (sourceData.base.entities_page_id == targetData.entities_page_id && sourceData.base.room_view_position == targetData.room_view_position) {
          return; // We did in fact not move the item.
        }

        // if (over && over.data.current.accepts.includes(active.data.current.type)) {
        if (sourceData.base.type == "entity") {
          let existingEntity = entities.find(
            (e) => e.base.entities_page_id == targetData.entities_page_id && e.base.room_view_position == targetData.room_view_position,
          );
          if (existingEntity != null) {
            existingEntity = JSON.parse(JSON.stringify(existingEntity)); // Create deep copy of existing entity to avoid modifying the original
          }

          for (const entity of entities) {
            if (entity.base.id == sourceData.base.id) {
              // Swap positions with existing entity
              if (existingEntity != null) {
                existingEntity.base.entities_page_id = entity.base.entities_page_id;
                existingEntity.base.room_view_position = entity.base.room_view_position;
              }

              // Get all untouched elements
              let updatedEntities: IEntityOrSceneData[] = [];
              if (existingEntity != null) {
                updatedEntities = entities.filter((e) => e.base.id != sourceData.base.id && e.base.id != existingEntity.base.id);
              } else {
                updatedEntities = entities.filter((e) => e.base.id != sourceData.base.id);
              }

              // Updated moved entity position
              const updatedEntity = JSON.parse(JSON.stringify(entity)); // Create a copy of the entity to update
              updatedEntity.base.entities_page_id = targetData.entities_page_id;
              updatedEntity.base.room_view_position = targetData.room_view_position;

              if (existingEntity != null) {
                setEntities([...updatedEntities, updatedEntity, existingEntity]);
                saveEntitiesOrder([...updatedEntities, updatedEntity, existingEntity], []);
              } else {
                setEntities([...updatedEntities, updatedEntity]);
                saveEntitiesOrder([...updatedEntities, updatedEntity], []);
              }
              break;
            }
          }
        } else {
          let existingEntity = scenes.find(
            (e) => e.base.entities_page_id == targetData.entities_page_id && e.base.room_view_position == targetData.room_view_position,
          );
          if (existingEntity != null) {
            existingEntity = JSON.parse(JSON.stringify(existingEntity)); // Create deep copy of existing entity to avoid modifying the original
          }

          for (const entity of scenes) {
            if (entity.base.id == sourceData.base.id) {
              // Swap positions with existing entity
              if (existingEntity != null) {
                existingEntity.base.entities_page_id = entity.base.entities_page_id;
                existingEntity.base.room_view_position = entity.base.room_view_position;
              }

              // Get all untouched elements
              let updatedScenes: IEntityOrSceneData[] = [];
              if (existingEntity != null) {
                updatedScenes = scenes.filter((e) => e.base.id != sourceData.base.id && e.base.id != existingEntity.base.id);
              } else {
                updatedScenes = scenes.filter((e) => e.base.id != sourceData.base.id);
              }

              // Updated moved entity position
              const updatedEntity = JSON.parse(JSON.stringify(entity)); // Create a copy of the entity to update
              updatedEntity.base.entities_page_id = targetData.entities_page_id;
              updatedEntity.base.room_view_position = targetData.room_view_position;

              if (existingEntity != null) {
                setScenes([...updatedScenes, updatedEntity, existingEntity]);
                saveEntitiesOrder([], [...updatedScenes, updatedEntity, existingEntity]);
              } else {
                setScenes([...updatedScenes, updatedEntity]);
                saveEntitiesOrder([], [...updatedScenes, updatedEntity]);
              }
              break;
            }
          }
        }
      }}
    >
      <ul className="list-none w-full nspanel-entities-pages">
        {(() => {
          const items = [];
          for (const entity_page of entitiesPages.entities_pages) {
            if (entity_page.type !== type) continue;

            items.push(
              <EntitiesPage
                key={entity_page.id}
                id={entity_page.id}
                room_id={viewData.room_id}
                type={viewData.type}
                number_of_entities={entity_page.number_of_entities}
                can_remove={entitiesPages.entities_pages.filter((p) => p.type == viewData.type).length > 1}
                entities={entity_page.type == "entity" ? entities : scenes}
                draging_item={dragingItem}
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
