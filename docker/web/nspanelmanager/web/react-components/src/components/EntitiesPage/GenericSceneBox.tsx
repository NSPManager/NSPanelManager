import { useDraggable } from "@dnd-kit/react";
import type { IEntityOrSceneData } from "./EntitiesPagesStore";
// import Step2 from "./step2_select_controller";
// import Step3 from "./Step3";

const GenericSceneBox = ({ scene }: { scene: IEntityOrSceneData }) => {
  const { ref } = useDraggable({ id: `scene-${scene.base.id}`, data: { type: "scene", config: scene } });

  return (
    <div
      ref={ref}
      className="draggable-entity-item bg-neutral/50 rounded-box text-neutral-content flex items-center justify-center indicator w-full h-full hover:outline-1 hover:outline-accent cursor-grab"
      title="Drag & drop to move this entity"
    >
      {/*<!-- Box indicators/buttons -->*/}
      <button className="indicator-item badge badge-info me-8 w-6 h-6 flex items-center justify-center cursor-pointer" title="Edit scene">
        <span className="mdi mdi-pencil"></span>
      </button>
      <button
        className="indicator-item badge badge-warning hover:badge-error w-6 h-6 flex items-center justify-center cursor-pointer"
        title="Remove scene from page/room"
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
