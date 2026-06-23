import { useEffect, useState } from "react";
import EntitiesPage from "./EntitiesPage";
import { DragDropProvider } from "@dnd-kit/react";
import { isSortable } from "@dnd-kit/react/sortable";
import { useEntitiesPagesStore, type IDragingItemData } from "../../stores/EntitiesPagesStore";
import { useRoomsStore } from "../../stores/RoomsStore";
import { type IEntityOrSceneData, type IEntityPageData } from "../../stores/EntitiesPagesStore";

const EntitiesPagesView = ({ room_id, type }: { room_id?: number; type: string }) => {
  const { entities_pages, fetchData, removeEntitiesPage, entities, scenes, setScenePosition, setEntityPosition } = useEntitiesPagesStore();
  const [dragingItem, setDragingItem] = useState<IDragingItemData | undefined>(undefined);
  const [addNewPageTypeOpen, setAddNewPageTypeOpen] = useState(false);

  useEffect(() => {
    fetchData(room_id);
  }, [room_id, fetchData]);
  useEffect(() => {
    if (useRoomsStore.getState().status == "none") {
      useRoomsStore.getState().fetchData();
    }
  }, []);

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
    fetch(`/rest/entities_pages/${id}`, {
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

  async function createEntitiesPage(number_of_entities: number) {
    console.log("Creating entities page", type);
    let url = `/rest/global/entities_pages`;
    if (room_id !== undefined) {
      url = `/rest/rooms/${room_id}/entities_pages`;
    }
    fetch(url, {
      credentials: "same-origin",
      method: "PUT",
      mode: "same-origin",
      headers: { "Content-Type": "application/json", "X-CSRFToken": getCookie("csrftoken") },
      body: JSON.stringify({
        type: number_of_entities,
        is_scenes_page: type == "scene",
      }),
    })
      .then(async (response) => {
        if (response.ok) {
          console.log("Successfully created entities page", type, "number of entities", number_of_entities);
          fetchData(room_id);
        }
      })
      .catch((error) => {
        console.error("Error creating entities page", error);
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

  async function saveEntitiesPagesOrder(entities_pages: IEntityPageData[]) {
    // PUT request using fetch with error handling
    fetch(`/rest/entities_pages/order`, {
      credentials: "same-origin",
      method: "PUT",
      mode: "same-origin",
      headers: { "Content-Type": "application/json", "X-CSRFToken": getCookie("csrftoken") },
      body: JSON.stringify({
        order: entities_pages.map((page) => [page.id, page.display_order]),
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
    <>
      <div className="flex items-center justify-start mb-4">
        <span className="text-xl">
          {useRoomsStore.getState().rooms?.find((r) => r.id == room_id)?.name}{" "}
          {(() => {
            if (type == "entity") return "entities";
            if (type == "scene") return "scenes";
            return "UNKNOWN ENTITY PAGE TYPE";
          })()}
        </span>
        <div
          className={`dropdown dropdown-top dropdown-center ${addNewPageTypeOpen ? "dropdown-open" : ""} me-8 flex indicator-item indicator-bottom indicator-end`}
        >
          <div onClick={() => setAddNewPageTypeOpen(!addNewPageTypeOpen)} className="btn btn-xs btn-circle btn-success ml-2">
            <span className="mdi mdi-plus"></span>
          </div>
          <div className="dropdown-content bg-base-100 join rounded-box z-1 p-1 shadow-sm">
            <button
              onClick={() => {
                setAddNewPageTypeOpen(false);
                createEntitiesPage(4);
              }}
              className={`btn join-item btn-lg btn-square`}
            >
              4
            </button>
            <button
              onClick={() => {
                setAddNewPageTypeOpen(false);
                createEntitiesPage(8);
              }}
              className={`btn join-item btn-lg btn-square`}
            >
              8
            </button>
            <button
              onClick={() => {
                setAddNewPageTypeOpen(false);
                createEntitiesPage(12);
              }}
              className={`btn join-item btn-lg btn-square`}
            >
              12
            </button>
          </div>
        </div>
      </div>
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
          setDragingItem(source.data as IDragingItemData);
        }}
        onDragEnd={(event) => {
          setDragingItem(undefined);

          if (event.canceled) return;
          const { source, target } = event.operation;
          if (source == null || target == null) return;

          const sourceData = source.data as IDragingItemData;
          const targetData = target.data as IDragingItemData;

          if (
            sourceData.config &&
            targetData.config &&
            sourceData.config.entities_page_id == targetData.config.entities_page_id &&
            sourceData.config.room_view_position == targetData.config.room_view_position
          ) {
            return; // We did in fact not move the item.
          }

          // if (over && over.data.current.accepts.includes(active.data.current.type)) {
          if (sourceData.type == "entity" && sourceData.config) {
            const entity = entities.find((e) => sourceData.config && e.id == sourceData.config.id);
            if (!entity) return;

            let existingEntity = entities.find(
              (e) =>
                targetData.config && e.entities_page_id == targetData.config.entities_page_id && e.room_view_position == targetData.config.room_view_position,
            );
            if (existingEntity != null) {
              existingEntity = JSON.parse(JSON.stringify(existingEntity)); // Create deep copy of existing entity to avoid modifying the original
              if (existingEntity) {
                setEntityPosition(existingEntity.id, entity.entities_page_id, entity.room_view_position);
              }
            }

            const updatedEntity = JSON.parse(JSON.stringify(entity)); // Create a copy of the entity to update
            if (targetData.config) {
              setEntityPosition(updatedEntity.id, targetData.config.entities_page_id, targetData.config.room_view_position);
              saveEntitiesOrder(useEntitiesPagesStore.getState().entities, []);
            } else {
              console.error("Failed to set new entity position. Target data has no config");
            }
          } else if (sourceData.type == "scene" && sourceData.config) {
            const scene = scenes.find((e) => sourceData.config && e.id == sourceData.config.id);
            if (!scene) return;

            let existingScene = scenes.find(
              (e) =>
                targetData.config && e.entities_page_id == targetData.config.entities_page_id && e.room_view_position == targetData.config.room_view_position,
            );
            if (existingScene != null) {
              existingScene = JSON.parse(JSON.stringify(existingScene)); // Create deep copy of existing entity to avoid modifying the original
              if (existingScene) {
                setScenePosition(existingScene.id, scene.entities_page_id, scene.room_view_position);
              }
            }

            const updatedEntity = JSON.parse(JSON.stringify(scene)); // Create a copy of the entity to update
            if (targetData.config) {
              setScenePosition(updatedEntity.id, targetData.config.entities_page_id, targetData.config.room_view_position);
              saveEntitiesOrder([], useEntitiesPagesStore.getState().scenes);
            } else {
              console.error("Failed to set new scene position as target as not config");
            }
          } else if (sourceData.type == "entity_page" && sourceData.entity_page_config) {
            if (isSortable(source)) {
              const { initialIndex, index } = source;
              console.log("Initial index: ", initialIndex, " index: ", index);

              if (initialIndex == index) return; // We did in fact not move the page.
              const sourcePage = useEntitiesPagesStore.getState().entities_pages.find((page) => page.display_order == initialIndex);
              if (!sourcePage) {
                console.error("Failed to find source page data. Cannot continue.");
                return;
              }

              // Move all pages with a higher display_order higher than the index to +1
              // useEntitiesPagesStore.getState().entities_pages.forEach((page) => {
              //   if (page.id == sourcePage.id) {
              //     page.display_order = index;
              //   } else if (page.display_order >= sourcePage.display_order && page.display_order < sourcePage.display_order) {
              //     page.display_order += 1;
              //   }
              // });
              const newEntitiesPagesOrder = [...useEntitiesPagesStore.getState().entities_pages.filter((page) => page.type == type)];
              const [removed] = newEntitiesPagesOrder.splice(initialIndex, 1);
              newEntitiesPagesOrder.splice(index, 0, removed);
              newEntitiesPagesOrder.forEach((page, i) => {
                page.display_order = i;
              });
              useEntitiesPagesStore.setState({
                entities_pages: [...newEntitiesPagesOrder, ...useEntitiesPagesStore.getState().entities_pages.filter((page) => page.type != type)],
              });
              saveEntitiesPagesOrder(newEntitiesPagesOrder);
            }
          }
        }}
      >
        <ul className="list-none w-full nspanel-entities-pages">
          {entities_pages
            .filter((entity_page) => entity_page.type === type)
            .map((entity_page) => (
              <EntitiesPage
                key={`entity_page-${entity_page.id}`}
                id={entity_page.id}
                draging_item={dragingItem}
                deleteEntitiesPage={deleteEntitiesPage}
              ></EntitiesPage>
            ))}
        </ul>
      </DragDropProvider>
    </>
  );
};

export default EntitiesPagesView;
