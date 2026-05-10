import { useDroppable } from "@dnd-kit/react";
import { forwardRef } from "react";
import type { IEntityOrSceneData } from "./EntitiesPagesView";
// import Step2 from "./step2_select_controller";
// import Step3 from "./Step3";

const EntitiesDropTarget = forwardRef(
  ({
    entities_page_id,
    room_view_position,
    type,
    children,
    draging_item,
  }: {
    entities_page_id: number;
    room_view_position: number;
    type: string;
    children?: React.ReactNode;
    draging_item: IEntityOrSceneData | undefined;
  }) => {
    const { ref, isDropTarget } = useDroppable({
      id: `entities_page-${entities_page_id}-room_view_position-${room_view_position}`,
      data: {
        accepts: type,
        config: {
          type: type,
          entities_page_id: entities_page_id,
          room_view_position: room_view_position,
        },
      },
    });

    const isChildDragging =
      draging_item !== undefined && draging_item.base.entities_page_id === entities_page_id && draging_item.base.room_view_position === room_view_position;

    return (
      <div ref={ref}>
        {(() => {
          if (isChildDragging) {
            return (
              <div
                className={`rounded-box p-1 items-center justify-center w-full h-full bg-conic/[from_var(--border-angle)] from-base-200 via-accent/50 to-base-200 animate-rotate-border from-30% to-60%`}
              >
                <div className="flex items-center justify-center rounded-box w-full h-full bg-base-200">
                  <span className="italic font-thin">{draging_item.base.friendly_name}</span>
                </div>
                {children}
              </div>
            );
          } else if (children) {
            return (
              <div className={`rounded-box ${isDropTarget ? "border-2 border-solid border-accent" : ""} items-center justify-center w-full h-full`}>
                {children}
              </div>
            );
          }

          return (
            <div
              className={`rounded-box ${isDropTarget ? "border-2 border-solid border-accent" : "border-dashed border-2 border-neutral/50"} flex items-center justify-center indicator w-full h-full`}
            >
              <span className="italic font-thin">
                {isDropTarget && draging_item !== undefined ? draging_item.base.friendly_name : "No " + (type === "scene" ? "scene" : "entity") + " set"}
              </span>
              <button
                hidden={isDropTarget}
                className="btn btn-neutral hover:btn-success btn-xs btn-circle ms-2 flex items-center justify-center"
                title={`Add ${type === "scene" ? "scene" : "entity"}`}
              >
                <span className="mdi mdi-plus"></span>
              </button>
            </div>
          );
        })()}
      </div>
    );
  },
);

export default EntitiesDropTarget;
