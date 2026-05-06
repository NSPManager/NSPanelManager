import { useState } from "react";
// import Step2 from "./step2_select_controller";
// import Step3 from "./Step3";

const MultiStep_AddOrEditEntity = ({ id, type }: { id: number; type: "entity" | "scene" }) => {
  interface BaseData {
    id: number;
    friendly_name: string;
    type: string;
    room_id: number;
    entities_page_id: number;
    room_view_position: number;
  }
  interface EntityOrSceneData {
    controller: string;
    controlled_by_nspanel_main_page?: boolean;
  }
  interface IFetchData {
    status: "loading" | "success" | "error";
    result: {
      base: BaseData;
      entity?: EntityOrSceneData;
      scene?: EntityOrSceneData;
    };
  }

  const [hasFetchedEntityData, setHasFetchedEntityData] = useState(false);
  const [entityData, setEntityData] = useState<IFetchData>({} as IFetchData);

  if (!hasFetchedEntityData) {
    let url: string;
    if (type === "entity") {
      url = `/rest/entities/${id}`;
    } else {
      url = `/rest/scenes/${id}`;
    }
    fetch(url)
      .then((response) => response.json())
      .then((data) => {
        setEntityData({ ...entityData, ...data });
        setHasFetchedEntityData(true);
      });
  }

  return (
    <div
      className="draggable-entity-item bg-neutral/50 rounded-box text-neutral-content flex items-center justify-center indicator w-full h-full hover:outline-1 hover:outline-accent cursor-grab"
      title="Drag & drop to move this entity"
    >
      {/*<!-- Box indicators/buttons -->*/}
      {(() => {
        if (!hasFetchedEntityData) return null;
        if (type !== "entity") return null;
        if (entityData.result.entity === undefined) return null;
        if (entityData.result.entity.controlled_by_nspanel_main_page === undefined) return null;

        if (entityData.result.entity.controlled_by_nspanel_main_page) {
          return (
            <span className="indicator-item badge badge-secondary me-16 w-6 h-6 flex items-center justify-center" title="Controlled from main page">
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
        if (!hasFetchedEntityData) return null;
        if (entityData.result.base === undefined) return null;
        if (entityData.result.entity === undefined) return null;

        if (entityData.result.entity.controller == "home_assistant") {
          return <div className="status status-info shadow-none absolute top-2 left-2 cursor-default" title="Controlled by Home Assistant"></div>;
        } else if (entityData.result.entity.controller == "openhab") {
          return <div className="status status-warning shadow-none absolute top-2 left-2 cursor-default" title="Controlled by OpenHAB"></div>;
        } else if (entityData.result.entity.controller == "nspm_scene" || entityData.result.entity.controller == "nspm") {
          return <div className="status status-accent shadow-none absolute top-2 left-2 cursor-default" title="Controlled by NSPanel Manager"></div>;
        } else {
          return <div className="status status-error animate-ping absolute top-2 left-2 cursor-default" title="Unknown controller"></div>;
        }
      })()}
      <span className="text-sm m-2">{hasFetchedEntityData && entityData.result.base !== undefined ? entityData.result.base.friendly_name : ""}</span>
    </div>
  );
};

export default MultiStep_AddOrEditEntity;
