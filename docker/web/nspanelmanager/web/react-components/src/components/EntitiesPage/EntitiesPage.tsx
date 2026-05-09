import { useState } from "react";
import GenericEntityBox from "./GenericEntityBox";
import GenericSceneBox from "./GenericSceneBox";
import EntitiesPageDropTarget from "./EntitiesPageDropTarget";
import { type IEntityOrSceneData } from "./EntitiesPagesView";

const EntitiesPage = ({
  room_id,
  id,
  can_remove,
  type,
  number_of_entities,
  entities,
}: {
  room_id: number;
  id: number;
  can_remove: boolean;
  type: string;
  number_of_entities: number;
  entities: IEntityOrSceneData[];
}) => {
  const [pageData, _setPageData] = useState({
    room_id: room_id,
    id: id,
    can_remove: can_remove,
    type: type, // scene or entity
    number_of_entities: number_of_entities, // Number of entities on page. 4, 8 or 12
    entities: entities,
  });

  const gridRows = pageData.number_of_entities === 12 ? "grid-rows-6" : pageData.number_of_entities === 4 ? "grid-rows-2" : "grid-rows-4";

  return (
    <li
      id="nspanel_entities_page_{{ page.id }}"
      data-is-scenes-page="{{ is_scenes_page }}"
      data-page-id="{{ page.id }}"
      hx-swap-oob="true"
      className={`bg-base-200 float-left m-2 rounded-box p-3 grid grid-cols-2 ${gridRows} gap-4 w-full max-w-[24rem] aspect-square indicator nspanel-entities-page`}
    >
      {(() => {
        if (!entities || entities.length === 0) return [];

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
          } else {
            for (let j = 0; j < entities.length; j++) {
              if (entities[j].base.room_view_position === i && entities[j].base.type === "scene" && entities[j].base.entities_page_id === pageData.id) {
                item = entities[j];
                break;
              }
            }
          }

          const entitiesPageDropTargetId = JSON.stringify({ type: pageData.type, entities_page_id: pageData.id, room_view_position: i });
          if (item != null) {
            if (item.base.type === "scene") {
              items.push(
                <EntitiesPageDropTarget id={entitiesPageDropTargetId} type={pageData.type}>
                  <GenericSceneBox scene={item}></GenericSceneBox>
                </EntitiesPageDropTarget>,
              );
            } else {
              items.push(
                <EntitiesPageDropTarget id={entitiesPageDropTargetId} type={pageData.type}>
                  <GenericEntityBox entity={item}></GenericEntityBox>
                </EntitiesPageDropTarget>,
              );
            }
          } else {
            items.push(<EntitiesPageDropTarget id={entitiesPageDropTargetId} type={pageData.type}></EntitiesPageDropTarget>);
          }
        }
        return items;
      })()}
      <button
        disabled={!can_remove}
        className="indicator-item indicator-bottom indicator-end badge not-disabled:badge-neutral not-disabled:hover:badge-error w-6 h-6 flex items-center justify-center not-disabled:cursor-pointer"
        title={`${can_remove ? "Delete page" : "Cannot delete last page"}`}
      >
        ✕
      </button>
      <button
        className="indicator-item indicator-bottom indicator-end badge badge-info w-6 h-6 flex items-center justify-center me-8 cursor-pointer"
        title="Edit page"
      >
        <span className="mdi mdi-pencil"></span>
      </button>
      {can_remove && (
        <span
          className="indicator-item indicator-bottom indicator-center badge badge-neutral hover:badge-info w-6 h-6 flex items-center justify-center nspanel-entities-page-move-handle cursor-grab"
          title="Drag & drop to move this page"
        >
          <span className="mdi mdi-hand-back-right"></span>
        </span>
      )}
    </li>
  );
};

export default EntitiesPage;
