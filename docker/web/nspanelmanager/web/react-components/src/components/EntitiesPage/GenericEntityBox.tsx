import { useRef } from "react";
import { useDraggable } from "@dnd-kit/react";
import { type IEntityOrSceneData } from "./EntitiesPagesStore";
import { useEntitiesPagesStore } from "./EntitiesPagesStore";
import MultiStep_AddOrEditEntity from "../MultiStep_AddEditEntity/MultiStep_AddEditEntity";

const GenericEntityBox = ({ entity }: { entity: IEntityOrSceneData }) => {
  const { ref } = useDraggable({
    id: `entity-${entity.base.id}`,
    data: { type: "entity", config: entity },
  });
  const { entities_pages, removeEntity, fetchData } = useEntitiesPagesStore();
  const entity_page = entities_pages.find((page) => page.id === entity.base.entities_page_id);
  const removeEntityDialogRef = useRef<HTMLDialogElement>(null);
  const editEntityDialogRef = useRef<HTMLDialogElement>(null);

  if (entity_page == undefined) return null;

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

  async function deleteEntity(id: number) {
    console.log("Deleting entity", id);
    const entity_page = entities_pages.find((page) => page.id === entity.base.entities_page_id);
    if (entity_page) {
      fetch(`/rest/entities/${id}`, {
        credentials: "same-origin",
        method: "DELETE",
        mode: "same-origin",
        headers: { "Content-Type": "application/json", "X-CSRFToken": getCookie("csrftoken") },
      })
        .then(async (response) => {
          if (response.ok) {
            console.log("Successfully deleted entity", id);
            removeEntity(id);
          }
        })
        .catch((error) => {
          console.error("Error deleting entity", error);
        });
    }
  }

  return (
    <div
      ref={ref}
      className="draggable-entity-item bg-neutral/50 rounded-box text-neutral-content flex items-center justify-center indicator w-full h-full hover:outline-1 hover:outline-accent cursor-grab"
      title="Drag & drop to move this entity"
    >
      <dialog ref={removeEntityDialogRef} className="modal">
        <div className="modal-box">
          <h3 className="text-lg font-bold">Delete entities page</h3>
          <p className="py-4">Are you sure you want to delete entity "{entity.base.friendly_name}"?</p>
          <div className="flex justify-end join">
            <button onClick={() => removeEntityDialogRef.current?.close()} className="btn btn-neutral join-item">
              Cancel
            </button>
            <button
              onClick={() => {
                removeEntityDialogRef.current?.close();
                deleteEntity(entity.base.id);
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
      <dialog ref={editEntityDialogRef} className="modal">
        <div className="modal-box overflow-y-visible">
          <button className="btn btn-sm btn-circle btn-ghost absolute right-2 top-2" onClick={() => editEntityDialogRef.current?.close()}>
            ✕
          </button>
          <MultiStep_AddOrEditEntity
            type={entity.base.entity_type}
            room_id={entity_page.room_id}
            entities_page_id={entity.base.entities_page_id}
            room_view_position={entity.base.room_view_position}
            id={entity.base.id}
            onComplete={() => {
              editEntityDialogRef.current?.close();
              fetchData(entity_page.room_id);
            }}
          />
        </div>
        <form method="dialog" className="modal-backdrop">
          <button>close</button>
        </form>
      </dialog>
      {/* Box indicators/buttons */}
      {(() => {
        if (entity.entity === undefined) return null;

        if (entity.entity.controlled_by_nspanel_main_page) {
          return (
            <span
              className="indicator-item badge badge-secondary me-16 w-6 h-6 flex items-center justify-center cursor-default"
              title="Controlled from main page"
            >
              <span className="mdi mdi-home"></span>
            </span>
          );
        }
        return null;
      })()}
      <button
        className="indicator-item badge badge-info me-8 w-6 h-6 flex items-center justify-center cursor-pointer"
        title="Edit entity"
        onClick={() => {
          editEntityDialogRef.current?.showModal();
        }}
      >
        <span className="mdi mdi-pencil"></span>
      </button>
      <button
        className="indicator-item badge badge-warning hover:badge-error w-6 h-6 flex items-center justify-center cursor-pointer"
        title="Remove entity from page/room"
        onClick={() => {
          removeEntityDialogRef.current?.showModal();
        }}
      >
        <span className="mdi mdi-close"></span>
      </button>

      {/*<!-- "Status badge" (dot) before name to indicate entity controller -->*/}
      {(() => {
        if (entity.entity === undefined) return null;

        if (entity.entity.controller == "home_assistant") {
          return <div className="status status-info shadow-none absolute top-2 left-2 cursor-default" title="Controlled by Home Assistant"></div>;
        } else if (entity.entity.controller == "openhab") {
          return <div className="status status-warning shadow-none absolute top-2 left-2 cursor-default" title="Controlled by OpenHAB"></div>;
        } else if (entity.entity.controller == "nspm_scene" || entity.entity.controller == "nspm") {
          return <div className="status status-accent shadow-none absolute top-2 left-2 cursor-default" title="Controlled by NSPanel Manager"></div>;
        } else {
          return <div className="status status-error animate-ping absolute top-2 left-2 cursor-default" title="Unknown controller"></div>;
        }
      })()}
      <span className="text-sm m-2">{entity.base.friendly_name}</span>
    </div>
  );
};

export default GenericEntityBox;
