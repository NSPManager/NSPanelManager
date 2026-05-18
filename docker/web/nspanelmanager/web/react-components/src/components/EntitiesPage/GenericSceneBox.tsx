import { useDraggable } from "@dnd-kit/react";
import type { IEntityOrSceneData } from "./EntitiesPagesStore";
import { useEntitiesPagesStore } from "./EntitiesPagesStore";
import { useRef } from "react";
// import Step2 from "./step2_select_controller";
// import Step3 from "./Step3";

const GenericSceneBox = ({ scene }: { scene: IEntityOrSceneData }) => {
  const { ref } = useDraggable({ id: `scene-${scene.base.id}`, data: { type: "scene", config: scene } });
  const { entities_pages, removeScene } = useEntitiesPagesStore();
  const entity_page = entities_pages.find((page) => page.id === scene.base.entities_page_id);
  const removeSceneDialogRef = useRef<HTMLDialogElement>(null);

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

  async function deleteScene(id: number) {
    console.log("Deleting scene", id);
    if (entity_page) {
      fetch(`/rest/scenes/${id}`, {
        credentials: "same-origin",
        method: "DELETE",
        mode: "same-origin",
        headers: { "Content-Type": "application/json", "X-CSRFToken": getCookie("csrftoken") },
      })
        .then(async (response) => {
          if (response.ok) {
            console.log("Successfully deleted scene", id);
            removeScene(id);
          }
        })
        .catch((error) => {
          console.error("Error deleting scene", error);
        });
    }
  }

  return (
    <div
      ref={ref}
      className="draggable-entity-item bg-neutral/50 rounded-box text-neutral-content flex items-center justify-center indicator w-full h-full hover:outline-1 hover:outline-accent cursor-grab"
      title="Drag & drop to move this entity"
    >
      <dialog ref={removeSceneDialogRef} className="modal">
        <div className="modal-box">
          <h3 className="text-lg font-bold">Delete scene</h3>
          <p className="py-4">Are you sure you want to delete scene "{scene.base.friendly_name}"?</p>
          <div className="flex justify-end join">
            <button onClick={() => removeSceneDialogRef.current?.close()} className="btn btn-neutral join-item">
              Cancel
            </button>
            <button
              onClick={() => {
                removeSceneDialogRef.current?.close();
                deleteScene(scene.base.id);
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
      {/*<!-- Box indicators/buttons -->*/}
      <button className="indicator-item badge badge-info me-8 w-6 h-6 flex items-center justify-center cursor-pointer" title="Edit scene">
        <span className="mdi mdi-pencil"></span>
      </button>
      <button
        className="indicator-item badge badge-warning hover:badge-error w-6 h-6 flex items-center justify-center cursor-pointer"
        title="Remove scene from page/room"
        onClick={() => {
          removeSceneDialogRef.current?.showModal();
        }}
      >
        <span className="mdi mdi-close"></span>
      </button>

      {/*<!-- "Status badge" (dot) before name to indicate entity controller -->*/}
      {(() => {
        if (scene.scene === undefined) return null;

        if (scene.scene.scene_type == "home_assistant") {
          return <div className="status status-info shadow-none absolute top-2 left-2 cursor-default" title="Controlled by Home Assistant"></div>;
        } else if (scene.scene.scene_type == "openhab") {
          return <div className="status status-warning shadow-none absolute top-2 left-2 cursor-default" title="Controlled by OpenHAB"></div>;
        } else if (scene.scene.scene_type == "nspm_scene" || scene.scene.scene_type == "nspm") {
          return <div className="status status-accent shadow-none absolute top-2 left-2 cursor-default" title="Controlled by NSPanel Manager"></div>;
        } else {
          return <div className="status status-error animate-ping absolute top-2 left-2 cursor-default" title="Unknown controller"></div>;
        }
      })()}
      <span className="text-sm m-2">{scene.base.friendly_name}</span>
    </div>
  );
};

export default GenericSceneBox;
