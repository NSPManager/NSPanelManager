import { useEffect, useRef, useState, useCallback } from "react";
import { useEntityStatesStore } from "../../stores/EntityStore.ts";
import RoomBox from "./RoomBox.tsx";
import { useStompStore } from "../../stores/StompStore.tsx";
import { Notify } from "../NSPanelToastContainer.tsx";
import { Icon } from "@mdi/react";
import { mdiMenuDownOutline, mdiRestart, mdiTableArrowUp, mdiUpload } from "@mdi/js";
import { api, type Room } from "../../api/client";

type LoadState = { status: "loading" } | { status: "error"; message: string } | { status: "loaded"; rooms: Room[] };

const RoomsView = () => {
  const popoverRef = useRef<HTMLUListElement>(null);
  const [state, setState] = useState<LoadState>({ status: "loading" });

  // Sets the state only once the request finishes.
  const fetchSettings = useCallback(() => {
    api
      .GET("/rest/v2/rooms")
      .then(({ data, response }) => setState(data ? { status: "loaded", rooms: data } : { status: "error", message: `HTTP ${response.status}` }))
      .catch((error) => setState({ status: "error", message: error instanceof Error ? error.message : String(error) }));
  }, []);

  useEffect(fetchSettings, [fetchSettings]);

  return (
    <div>
      <div className="flex items-center justify-between">
        <span className="text-2xl">Rooms</span>
        <div>
          <button className="btn btn-info font-medium">New room</button>
        </div>
      </div>
      <div>
        {(() => {
          if (state.status != "loaded") {
            return (
              <div className="flex w-full items-center justify-center text-2xl">
                <span className="loading loading-spinner loading-sm mr-2"></span>
                <span className="font-medium">Loading rooms...</span>
              </div>
            );
          }

          if (state.status == "loaded" && state.rooms.length > 0) {
            return (
              <div className="grid md:grid-cols-3 gap-4 mt-4 items-stretch" id="rooms_container">
                {state.rooms.map((room) => {
                  return <RoomBox room={room}></RoomBox>;
                })}
              </div>
            );
          } else {
            return (
              <div className="grid md:grid-cols-3 gap-4 mt-4 items-start" id="rooms_container">
                <div className="col-span-3 w-full text-center text-xl text-base-content">
                  No Rooms found. Getting started?{" "}
                  <a href="https://nspanelmanager.com/docs/getting-started/flash-nspanel" target="_blank" className="text-accent">
                    Read the documentation.
                  </a>
                </div>
              </div>
            );
          }
        })()}
      </div>
    </div>
  );
};

export default RoomsView;
