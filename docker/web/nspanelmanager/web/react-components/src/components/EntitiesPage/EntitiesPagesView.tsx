import { useEffect, useState } from "react";
import EntitiesPage from "./EntitiesPage";
import { DragDropProvider } from "@dnd-kit/react";
import { useEntitiesPagesStore } from "./EntitiesPagesStore";
import { type IEntityOrSceneData } from "./EntitiesPagesStore";

const EntitiesPagesView = ({ room_id, type }: { room_id: number; type: string }) => {
  const { entities_pages, fetchData, removeEntitiesPage, entities, scenes, setScenePosition, setEntityPosition } = useEntitiesPagesStore();
  const [dragingItem, setDragingItem] = useState<IEntityOrSceneData | undefined>(undefined);

  useEffect(() => {
    fetchData(room_id);
  }, [room_id, fetchData]);

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

  async function deleteEntitiesPage(id: number) {
    console.log("Deleting entities page", id);
    fetch(`/rest/rooms/${room_id}/entities_pages/${id}`, {
      credentials: "same-origin",
      method: "DELETE",
      mode: "same-origin",
      headers: { "Content-Type": "application/json", "X-CSRFToken": getCookie("csrftoken") },
    })
      .then(async (response) => {
        if (response.ok) {
          console.log("Successfully deleted entities page", id);
          removeEntitiesPage(id);
        }
      })
      .catch((error) => {
        console.error("Error deleting entities page", error);
      });
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
          const entity = entities.find((e) => e.base.id == sourceData.base.id);
          if (!entity) return;

          let existingEntity = entities.find(
            (e) => e.base.entities_page_id == targetData.entities_page_id && e.base.room_view_position == targetData.room_view_position,
          );
          if (existingEntity != null) {
            existingEntity = JSON.parse(JSON.stringify(existingEntity)); // Create deep copy of existing entity to avoid modifying the original
            if (existingEntity) {
              setEntityPosition(existingEntity.base.id, entity.base.entities_page_id, entity.base.room_view_position);
            }
          }

          const updatedEntity = JSON.parse(JSON.stringify(entity)); // Create a copy of the entity to update
          setEntityPosition(updatedEntity.base.id, targetData.entities_page_id, targetData.room_view_position);
          saveEntitiesOrder(useEntitiesPagesStore.getState().entities, []);
        } else {
          const scene = scenes.find((e) => e.base.id == sourceData.base.id);
          if (!scene) return;

          let existingScene = scenes.find(
            (e) => e.base.entities_page_id == targetData.entities_page_id && e.base.room_view_position == targetData.room_view_position,
          );
          if (existingScene != null) {
            existingScene = JSON.parse(JSON.stringify(existingScene)); // Create deep copy of existing entity to avoid modifying the original
            if (existingScene) {
              setScenePosition(existingScene.base.id, scene.base.entities_page_id, scene.base.room_view_position);
            }
          }

          const updatedEntity = JSON.parse(JSON.stringify(scene)); // Create a copy of the entity to update
          setScenePosition(updatedEntity.base.id, targetData.entities_page_id, targetData.room_view_position);
          saveEntitiesOrder([], useEntitiesPagesStore.getState().scenes);
        }
      }}
    >
      <ul className="list-none w-full nspanel-entities-pages">
        {(() => {
          const items = [];
          for (const entity_page of entities_pages) {
            if (entity_page.type !== type) continue;
            items.push(
              <EntitiesPage key={entity_page.id} id={entity_page.id} draging_item={dragingItem} deleteEntitiesPage={deleteEntitiesPage}></EntitiesPage>,
            );
          }
          return items;
        })()}
      </ul>
    </DragDropProvider>
  );
};

export default EntitiesPagesView;
