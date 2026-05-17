import { useState, useRef } from "react";
import GenericEntityBox from "./GenericEntityBox";
import GenericSceneBox from "./GenericSceneBox";
import EntitiesPageDropTarget from "./EntitiesPageDropTarget";
import { useEntitiesPagesStore } from "./EntitiesPagesStore";
import { type IEntityOrSceneData } from "./EntitiesPagesStore";

const EntitiesPage = ({
  id,
  draging_item,
  deleteEntitiesPage,
}: {
  id: number;
  draging_item?: IEntityOrSceneData | undefined;
  deleteEntitiesPage: (id: number) => void;
}) => {
  const { entities_pages, entities, scenes } = useEntitiesPagesStore();
  const page = entities_pages.find((p) => p.id === id);
  const [pageData, setPageData] = useState({
    room_id: page ? page.room_id : -1,
    id: id,
    can_remove: page ? entities_pages.filter((p) => p.type == page.type).length > 1 : false,
    type: page ? page.type : "unknown", // scene or entity
    number_of_entities: page ? page.number_of_entities : 12, // Number of entities on page. 4, 8 or 12
  });
  const [editPageTypeOpen, setEditPageTypeOpen] = useState(false);
  const gridRows = pageData.number_of_entities === 12 ? "grid-rows-6" : pageData.number_of_entities === 8 ? "grid-rows-4" : "grid-rows-2";
  const dialogRef = useRef<HTMLDialogElement>(null);

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

  async function setPageNumberOfEntities(type: number) {
    setPageData({ ...pageData, number_of_entities: type });
    setEditPageTypeOpen(false);

    // PUT request using fetch with error handling
    fetch(`/rest/rooms/${pageData.room_id}/entities_pages/${pageData.id}`, {
      credentials: "same-origin",
      method: "PUT",
      mode: "same-origin",
      headers: { "Content-Type": "application/json", "X-CSRFToken": getCookie("csrftoken") },
      body: JSON.stringify({
        ...pageData,
        number_of_entities: type,
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
      <dialog ref={dialogRef} className="modal">
        <div className="modal-box">
          <h3 className="text-lg font-bold">Delete entities page</h3>
          <p className="py-4">Are you sure you want to delete this entities page and all its entities?</p>
          <div className="flex justify-end join">
            <button onClick={() => dialogRef.current?.close()} className="btn btn-neutral join-item">
              Cancel
            </button>
            <button
              onClick={() => {
                dialogRef.current?.close();
                deleteEntitiesPage(pageData.id);
              }}
              className="btn btn-error join-item"
            >
              Delete
            </button>
          </div>
        </div>
        <form method="dialog" className="modal-backdrop">
          <button>close</button>
        </form>
      </dialog>
      <li
        id="nspanel_entities_page_{{ page.id }}"
        hx-swap-oob="true"
        className={`bg-base-200 float-left m-2 rounded-box p-3 grid grid-cols-2 ${gridRows} gap-4 w-full max-w-[24rem] aspect-square indicator nspanel-entities-page`}
      >
        {(() => {
          const items = [];
          for (let i = 0; i < pageData.number_of_entities; i++) {
            let item = null;
            if (pageData.type == "entity") {
              for (let j = 0; j < entities.length; j++) {
                if (entities[j].base.room_view_position === i && entities[j].base.type === "entity" && entities[j].base.entities_page_id === pageData.id) {
                  item = entities[j];
                  break;
                }
              }
            } else if (pageData.type == "scene") {
              for (let j = 0; j < scenes.length; j++) {
                if (scenes[j].base.room_view_position === i && scenes[j].base.type === "scene" && scenes[j].base.entities_page_id === pageData.id) {
                  item = scenes[j];
                  break;
                }
              }
            } else {
              console.error("Unknown entities page type: ", pageData.type);
            }

            if (item != null) {
              if (item.base.type === "scene") {
                items.push(
                  <EntitiesPageDropTarget key={i} entities_page_id={pageData.id} room_view_position={i} type={pageData.type} draging_item={draging_item}>
                    <GenericSceneBox key={item.base.id} scene={item}></GenericSceneBox>
                  </EntitiesPageDropTarget>,
                );
              } else {
                items.push(
                  <EntitiesPageDropTarget key={i} entities_page_id={pageData.id} room_view_position={i} type={pageData.type} draging_item={draging_item}>
                    <GenericEntityBox key={item.base.id} entity={item}></GenericEntityBox>
                  </EntitiesPageDropTarget>,
                );
              }
            } else {
              items.push(
                <EntitiesPageDropTarget
                  key={i}
                  entities_page_id={pageData.id}
                  room_view_position={i}
                  type={pageData.type}
                  draging_item={draging_item}
                ></EntitiesPageDropTarget>,
              );
            }
          }
          return items;
        })()}
        <button
          disabled={!pageData.can_remove}
          className="indicator-item indicator-bottom indicator-end badge not-disabled:badge-neutral not-disabled:hover:badge-error w-6 h-6 flex items-center justify-center not-disabled:cursor-pointer"
          title={`${pageData.can_remove ? "Delete page" : "Cannot delete last page"}`}
          onClick={() => dialogRef.current?.showModal()}
        >
          ✕
        </button>
        <div
          className={`dropdown dropdown-top dropdown-center ${editPageTypeOpen ? "dropdown-open" : ""} me-8 flex indicator-item indicator-bottom indicator-end`}
        >
          <div onClick={() => setEditPageTypeOpen(!editPageTypeOpen)} className="badge badge-info w-6 h-6 flex items-center justify-center cursor-pointer">
            <span className="mdi mdi-pencil"></span>
          </div>
          <div className="dropdown-content bg-base-100 join rounded-box z-1 w-52 p-1 shadow-sm">
            <button
              onClick={() => setPageNumberOfEntities(4)}
              className={`btn join-item btn-lg btn-square ${pageData.number_of_entities === 4 ? "btn-active btn-accent" : ""}`}
            >
              4
            </button>
            <button
              onClick={() => setPageNumberOfEntities(8)}
              className={`btn join-item btn-lg btn-square ${pageData.number_of_entities === 8 ? "btn-active btn-accent" : ""}`}
            >
              8
            </button>
            <button
              onClick={() => setPageNumberOfEntities(12)}
              className={`btn join-item btn-lg btn-square ${pageData.number_of_entities === 12 ? "btn-active btn-accent" : ""}`}
            >
              12
            </button>
          </div>
        </div>
        {pageData.can_remove && (
          <span
            className="indicator-item indicator-bottom indicator-center badge badge-neutral hover:badge-info w-6 h-6 flex items-center justify-center nspanel-entities-page-move-handle cursor-grab"
            title="Drag & drop to move this page"
          >
            <span className="mdi mdi-drag"></span>
          </span>
        )}
      </li>
    </>
  );
};

export default EntitiesPage;
