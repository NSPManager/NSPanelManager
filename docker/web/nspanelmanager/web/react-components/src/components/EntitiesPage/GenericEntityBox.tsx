import { useDraggable } from "@dnd-kit/react";
import { type IEntityOrSceneData } from "./EntitiesPagesView";
// import Step2 from "./step2_select_controller";
// import Step3 from "./Step3";

const GenericEntityBox = ({ entity }: { entity: IEntityOrSceneData }) => {
  const { ref } = useDraggable({
    id: `entity-${entity.base.id}`,
    data: { type: "entity", config: entity },
    // plugins: [Feedback.configure({ feedback: "clone" })],
  });

  return (
    <div
      ref={ref}
      className="draggable-entity-item bg-neutral/50 rounded-box text-neutral-content flex items-center justify-center indicator w-full h-full hover:outline-1 hover:outline-accent cursor-grab"
      title="Drag & drop to move this entity"
    >
      {/*<!-- Box indicators/buttons -->*/}
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
      <button className="indicator-item badge badge-info me-8 w-6 h-6 flex items-center justify-center cursor-pointer" title="Edit entity">
        <span className="mdi mdi-pencil"></span>
      </button>
      <button
        className="indicator-item badge badge-warning hover:badge-error w-6 h-6 flex items-center justify-center cursor-pointer"
        title="Remove entity from page/room"
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
