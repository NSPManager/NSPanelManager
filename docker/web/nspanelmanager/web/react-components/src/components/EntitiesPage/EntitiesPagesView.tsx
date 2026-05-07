import { useState } from "react";
import EntitiesPage from "./EntityPage";

const EntitiesPagesView = ({ room_id, type }: { room_id: number; type: string }) => {
  const [viewData, _setViewData] = useState({
    room_id: room_id,
    type: type, // "entity" or "scene"
  });

  interface EntitiesPageData {
    id: number;
    display_order: number;
    number_of_entities: number;
    type: string;
    room_id: number;
  }

  interface EntitiesPages {
    status: string;
    entities_pages: EntitiesPageData[];
  }

  const [hasFetchedEntitiesPages, setHasFetchedEntitiesPages] = useState(false);
  const [entitiesPages, setEntitiesPages] = useState<EntitiesPages>({ status: "loading", entities_pages: [] });

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
    fetch(`/rest/rooms/${room_id}/entities_pages`, {
      credentials: "same-origin",
      method: "GET",
      mode: "same-origin",
      headers: { "Content-Type": "application/json", "X-CSRFToken": getCookie("csrftoken") },
    })
      .then((response) => response.json())
      .then((data) => {
        setEntitiesPages(data);
        setHasFetchedEntitiesPages(true);
      });
  }
  if (!hasFetchedEntitiesPages) {
    fetchEntities();
  }

  return (
    <ul className="list-none w-full nspanel-entities-pages">
      {(() => {
        if (!("entities_pages" in entitiesPages)) return [];
        if (!hasFetchedEntitiesPages) return [];

        const items = [];
        for (const entity_page of entitiesPages.entities_pages) {
          if (entity_page.type !== type) continue;

          items.push(
            <EntitiesPage
              id={entity_page.id}
              room_id={viewData.room_id}
              type={viewData.type}
              number_of_entities={entity_page.number_of_entities}
              can_remove={entitiesPages.entities_pages.length > 1}
            ></EntitiesPage>,
          );
        }
        return items;
      })()}
    </ul>
  );
};

export default EntitiesPagesView;
