import { useEffect, useState, useCallback, useRef } from "react";
import RoomBox from "./RoomBox.tsx";
import { Notify } from "../NSPanelToastContainer.tsx";
import { api, type Room } from "../../api/client";
import { DragDropProvider } from "@dnd-kit/react";
import { move } from "@dnd-kit/helpers";
import { useEntityStatesStore } from "../../stores/EntityStore.ts";
import NewRoomDialog from "./NewRoomDialog.tsx";

type LoadState = { status: "loading" } | { status: "error"; message: string } | { status: "loaded"; rooms: Room[] };

const RoomsView = () => {
  const [state, setState] = useState<LoadState>({ status: "loading" });
  const { status: entityStatesStatus, fetchData } = useEntityStatesStore();
  const newRoomDialogRef = useRef<HTMLDialogElement>(null);

  // Sets the state only once the request finishes.
  const fetchSettings = useCallback(() => {
    api
      .GET("/rest/v2/rooms")
      .then(({ data, response }) => setState(data ? { status: "loaded", rooms: data } : { status: "error", message: `HTTP ${response.status}` }))
      .catch((error) => setState({ status: "error", message: error instanceof Error ? error.message : String(error) }));
  }, []);

  useEffect(() => {
    if (entityStatesStatus === "none") {
      fetchData();
    }
  }, [entityStatesStatus, fetchData]);

  useEffect(fetchSettings, [fetchSettings]);

  return (
    <div>
      <NewRoomDialog dialogRef={newRoomDialogRef} onCreated={fetchSettings} />

      <div className="flex items-center justify-between">
        <div className="flex items-center justify-start gap-1">
          <span className="text-2xl">Rooms</span>
          {(() => {
            if (entityStatesStatus != "loaded") {
              return <span className="loading loading-spinner loading-sm mr-2"></span>;
            }
            return null;
          })()}
        </div>
        <div>
          <button className="btn btn-info font-medium" onClick={() => newRoomDialogRef.current?.showModal()}>
            New room
          </button>
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
              <DragDropProvider
                onDragStart={(event) => {
                  console.log("Drag start: ", event);
                }}
                onDragEnd={(event) => {
                  if (event.canceled) return;
                  const previousState = state;

                  const reordered = move(state.rooms, event).map((room, index) => ({
                    ...room,
                    displayOrder: index, // use index + 1 if your backend is 1-based
                  }));

                  api
                    .PATCH("/rest/v2/rooms", {
                      body: {
                        rooms: reordered.map((room, index) => ({
                          id: room.id,
                          displayOrder: index + 1, // your serializer's min_value is 1
                        })),
                      },
                    })
                    .then(({ data, error, response }) => {
                      if (data) {
                        setState({ status: "loaded", rooms: data });
                        return;
                      }

                      // HTTP error (400, 403, 415, ...)
                      setState(previousState);
                      console.error("Failed to save room", error);
                      Notify({
                        message: `Failed to save room order (HTTP ${response.status})`,
                        level: "error",
                        duration: 5000,
                      });
                    })
                    .catch((error) => {
                      // Network failure
                      setState(previousState);
                      Notify({
                        message: error instanceof Error ? error.message : String(error),
                        level: "error",
                        duration: 5000,
                      });
                    });
                }}
              >
                <ul className="grid md:grid-cols-3 gap-4 mt-4 items-stretch" id="rooms_container">
                  {state.rooms.map((room, index) => {
                    return <RoomBox key={`room-id-${room.id}`} room={room} index={index}></RoomBox>;
                  })}
                </ul>
              </DragDropProvider>
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
