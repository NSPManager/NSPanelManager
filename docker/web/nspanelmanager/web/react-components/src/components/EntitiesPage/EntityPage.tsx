import { useState } from "react";
import GenericEntityBox from "./GenericEntityBox";
import GenericSceneBox from "./GenericSceneBox";

// import Step2 from "./step2_select_controller";
// import Step3 from "./Step3";

const EntitiesPage = ({
  room_id,
  id,
  can_remove,
  type,
  number_of_entities,
}: {
  room_id: number;
  id: number;
  can_remove: boolean;
  type: string;
  number_of_entities: number;
}) => {
  const [pageData, _setPageData] = useState({
    room_id: room_id,
    id: id,
    can_remove: can_remove,
    type: type, // scene or entity
    number_of_entities: number_of_entities, // Number of entities on page. 4, 8 or 12
  });

  interface EntityOrSceneData {
    entity_id: number;
    room_view_position: number;
    type: "scene" | "entity";
  }

  interface EntitiesPages {
    status: string;
    entities: EntityOrSceneData[];
    scenes: EntityOrSceneData[];
  }

  const [hasFetchedEntities, setHasFetchedEntities] = useState(false);
  const [entities, setEntities] = useState<EntitiesPages>({ status: "loading", entities: [], scenes: [] });

  const gridRows = pageData.number_of_entities === 12 ? "grid-rows-6" : pageData.number_of_entities === 4 ? "grid-rows-2" : "grid-rows-4";

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

  function fetchEntities() {
    fetch(`/rest/rooms/${room_id}/entities_pages/${id}`, {
      credentials: "same-origin",
      method: "GET",
      mode: "same-origin",
      headers: { "Content-Type": "application/json", "X-CSRFToken": getCookie("csrftoken") },
    })
      .then((response) => response.json())
      .then((data) => {
        setEntities(data);
        setHasFetchedEntities(true);
      });
  }
  if (!hasFetchedEntities) {
    fetchEntities();
  }

  return (
    <li
      id="nspanel_entities_page_{{ page.id }}"
      data-is-scenes-page="{{ is_scenes_page }}"
      data-page-id="{{ page.id }}"
      hx-swap-oob="true"
      className={`bg-base-200 float-left m-2 rounded-box p-3 grid grid-cols-2 ${gridRows} gap-4 w-full max-w-[24rem] aspect-square indicator nspanel-entities-page`}
    >
      {(() => {
        if (!(entities && "entities" in entities)) return [];

        const items = [];
        for (let i = 0; i < pageData.number_of_entities; i++) {
          let item = null;
          if (pageData.type == "entity") {
            for (let j = 0; j < entities.entities.length; j++) {
              if (entities.entities[j].room_view_position === i) {
                item = entities.entities[j];
                break;
              }
            }
          } else {
            for (let j = 0; j < entities.scenes.length; j++) {
              if (entities.scenes[j].room_view_position === i) {
                item = entities.scenes[j];
                break;
              }
            }
          }

          if (item != null) {
            if (item.type === "scene") {
              items.push(<GenericSceneBox id={item.entity_id}></GenericSceneBox>);
            } else {
              items.push(<GenericEntityBox id={item.entity_id}></GenericEntityBox>);
            }
          } else {
            items.push(
              <div className="rounded-box border-neutral/50 border-dashed border-2 flex items-center justify-center indicator w-full h-full">
                <span className="italic font-thin">No {type === "scene" ? "scene" : "entity"} set</span>
                <button
                  className="btn btn-neutral hover:btn-success btn-xs btn-circle ms-2 flex items-center justify-center"
                  title={`Add ${type === "scene" ? "scene" : "entity"}`}
                >
                  <span className="mdi mdi-plus"></span>
                </button>
              </div>,
            );
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
