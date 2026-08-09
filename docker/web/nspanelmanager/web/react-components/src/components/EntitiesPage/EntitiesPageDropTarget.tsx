import { useDroppable } from "@dnd-kit/react";
import { forwardRef, useState } from "react";
import MultiStep_AddEditEntity from "../MultiStep_AddEditEntity/MultiStep_AddEditEntity";
import { useEntitiesPagesStore, type IDragingItemData } from "../../stores/EntitiesPagesStore";

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
    draging_item: IDragingItemData | undefined | null;
  }) => {
    const [addEditEntityDialogOpened, setAddEditEntityDialogOpened] = useState(false);
    const { entities_pages, fetchData } = useEntitiesPagesStore();
    const entities_page = entities_pages.find((page) => page.id === entities_page_id);
    const { ref, isDropTarget } = useDroppable({
      id: `entities_page-${entities_page_id}-room_view_position-${room_view_position}`,
      data: {
        accepts: type,
        type: "entities_page_drop_target",
        config: {
          type: type,
          entities_page_id: entities_page_id,
          room_view_position: room_view_position,
        },
      },
    });

    if (entities_page === undefined) return null;

    const isChildDragging =
      draging_item &&
      draging_item.config &&
      draging_item.config.entities_page_id === entities_page_id &&
      draging_item.config.room_view_position === room_view_position;

    return (
      <div ref={ref}>
        {(() => {
          if (isChildDragging) {
            return (
              <div
                className={`rounded-box p-1 items-center justify-center w-full h-full bg-conic/[from_var(--border-angle)] from-base-200 via-accent/50 to-base-200 animate-rotate-border from-30% to-60%`}
              >
                <div className="flex items-center justify-center rounded-box w-full h-full bg-base-200">
                  <span className="italic font-thin">{draging_item.config && draging_item.config.friendly_name}</span>
                </div>
                {children}
              </div>
            );
          } else if (children) {
            return (
              <div
                className={`rounded-box ${draging_item && draging_item.type != "entity_page" && isDropTarget ? "border-2 border-solid border-accent" : ""} items-center justify-center w-full h-full`}
              >
                {children}
              </div>
            );
          }

          return (
            <div
              className={`rounded-box ${draging_item && draging_item.type != "entity_page" && isDropTarget ? "border-2 border-solid border-accent" : "border-dashed border-2 border-neutral/50"} flex items-center justify-center indicator w-full h-full`}
            >
              <MultiStep_AddEditEntity
                type={type}
                room_id={entities_page.room_id}
                entities_page_id={entities_page_id}
                room_view_position={room_view_position}
                onComplete={() => {
                  fetchData(entities_page.room_id);
                  setAddEditEntityDialogOpened(false);
                }}
                opened={addEditEntityDialogOpened}
                setOpened={setAddEditEntityDialogOpened}
              />
              <span className="italic font-thin">
                {draging_item?.type !== "entity_page" && isDropTarget && draging_item && draging_item.config
                  ? draging_item.config.friendly_name
                  : "No " + (type === "scene" ? "scene" : "entity") + " set"}
              </span>
              <button
                hidden={draging_item?.type !== "entity_page" && isDropTarget}
                className="btn btn-neutral hover:btn-success btn-xs btn-circle ms-2 flex items-center justify-center"
                title={`Add ${type === "scene" ? "scene" : "entity"}`}
                onClick={() => setAddEditEntityDialogOpened(true)}
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
