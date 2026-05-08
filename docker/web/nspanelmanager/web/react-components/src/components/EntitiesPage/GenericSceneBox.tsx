import { useState, forwardRef } from "react";
import { useDraggable } from "@dnd-kit/react";
// import Step2 from "./step2_select_controller";
// import Step3 from "./Step3";

const GenericSceneBox = forwardRef(({ id }: { id: number }) => {
  interface SceneData {
    id: number;
    friendly_name: string;
    type: string;
    room_id: number;
    entities_page_id: number;
    room_view_position: number;
    controller: string;
    backend_name: string;
  }
  interface IFetchData {
    status: "loading" | "success" | "error";
    result: SceneData;
  }

  const [hasFetchedEntityData, setHasFetchedEntityData] = useState(false);
  const [entityData, setEntityData] = useState<IFetchData>({} as IFetchData);
  const nodeRef = useDraggable({ id: `scene-${id}` });

  console.log("Scene ID: ", id);
  if (!hasFetchedEntityData) {
    console.log("Fetching scene ID: ", id);
    fetch(`/rest/scenes/${id}`)
      .then((response) => response.json())
      .then((data) => {
        setEntityData({ ...entityData, ...data });
        setHasFetchedEntityData(true);
      });
  }

  return (
    <div
      ref={nodeRef.ref}
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
        if (!hasFetchedEntityData) return null;

        if (entityData.result.controller == "home_assistant") {
          return <div className="status status-info shadow-none absolute top-2 left-2 cursor-default" title="Controlled by Home Assistant"></div>;
        } else if (entityData.result.controller == "openhab") {
          return <div className="status status-warning shadow-none absolute top-2 left-2 cursor-default" title="Controlled by OpenHAB"></div>;
        } else if (entityData.result.controller == "nspm_scene" || entityData.result.controller == "nspm") {
          return <div className="status status-accent shadow-none absolute top-2 left-2 cursor-default" title="Controlled by NSPanel Manager"></div>;
        } else {
          return <div className="status status-error animate-ping absolute top-2 left-2 cursor-default" title="Unknown controller"></div>;
        }
      })()}
      <span className="text-sm m-2">{hasFetchedEntityData ? entityData.result.friendly_name : ""}</span>
    </div>
  );
});

export default GenericSceneBox;
