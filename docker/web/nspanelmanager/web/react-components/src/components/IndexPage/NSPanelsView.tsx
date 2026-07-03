import { useEffect, useRef } from "react";
import { useEntityStatesStore } from "../../stores/EntityStore.ts";
import NSPanelBox from "./NSPanelBox.tsx";
import { useStompStore } from "../../stores/StompStore.tsx";
import { Notify } from "../NSPanelToastContainer.tsx";

const NSPanelsIndexView = () => {
  const { nspanels, status, fetchData } = useEntityStatesStore();
  const popoverRef = useRef<HTMLUListElement>(null);

  useEffect(() => {
    if (status === "none") {
      fetchData();
    }
  }, [status, fetchData]);

  return (
    <div>
      <div className="flex items-center justify-between">
        <span className="text-2xl">NSPanels</span>
        <div>
          <button className="btn btn-soft font-medium" popoverTarget={`nspanels-actions`} style={{ anchorName: `--anchor-nspanels-actions` }}>
            Actions<span className="mdi mdi-menu-down-outline"></span>
          </button>
          <ul
            className="dropdown menu w-52 rounded-box bg-base-100 shadow-sm"
            popover="auto"
            id={`nspanels-actions`}
            style={{ anchorName: `--anchor-nspanels-actions` }}
            ref={popoverRef}
          >
            <li>
              <button
                onClick={() => {
                  const nspanels = useEntityStatesStore.getState().nspanels;
                  for (const [mac, status] of Object.entries(nspanels)) {
                    useStompStore.getState().send(`nspanel/${mac}/command`, "reboot");
                  }
                  Notify({ message: `Send reboot command to all NSPanels.`, level: "success", duration: 2000 });
                  popoverRef.current?.hidePopover();
                }}
                id="reboot-{{ id }}"
                hx-swap="none"
                className="block px-4 py-2 cursor-pointer nspanel-reboot-button group"
              >
                <span className="mdi mdi-restart pr-2 group-hover:text-warning"></span>Reboot all NSPanels
              </button>
            </li>
            <li className="border-t border-neutral-content"></li>
            <li>
              <a
                onClick={() => {
                  const nspanels = useEntityStatesStore.getState().nspanels;
                  for (const [mac, status] of Object.entries(nspanels)) {
                    if (status.state == "online") {
                      useStompStore.getState().send(`nspanel/${mac}/command`, "firmware_update");
                      Notify({ message: `Send FW update command to ${status.name}.`, level: "success", duration: 2000 });
                    }
                  }
                  popoverRef.current?.hidePopover();
                }}
                id="firmware-update-all"
                className={`block px-4 py-2 cursor-pointer group`}
              >
                <span className="mdi mdi-upload pr-2 group-hover:text-success"></span>Update firmware on all NSPanels
              </a>
            </li>
            <li>
              <a
                onClick={() => {
                  const nspanels = useEntityStatesStore.getState().nspanels;
                  for (const [mac, status] of Object.entries(nspanels)) {
                    if (status.state == "online") {
                      useStompStore.getState().send(`nspanel/${mac}/command`, "gui_update");
                      Notify({ message: `Send GUI update command to ${status.name}.`, level: "success", duration: 2000 });
                    }
                  }
                  popoverRef.current?.hidePopover();
                }}
                id="screen-update-all"
                className={`block px-4 py-2 cursor-pointer group`}
              >
                <span className="mdi mdi-table-arrow-up pr-2 group-hover:text-success"></span>Update GUI on all NSPanels
              </a>
            </li>
            <li className="border-t border-neutral-content"></li>
            <li>
              <button
                onClick={() => {
                  fetch("/api/restart_mqtt_manager", { method: "POST" })
                    .then((response) => {
                      if (response.ok) {
                        Notify({ message: `MQTTManager restarted successfully.`, level: "success", duration: 2000 });
                      } else {
                        Notify({ message: `Failed to restart MQTTManager.`, level: "error", duration: 2000 });
                      }
                    })
                    .catch((error) => {
                      Notify({ message: `Failed to restart MQTTManager.`, level: "error", duration: 2000 });
                      console.error("Got error while trying to restart MQTTManager process. Error:", error);
                    });
                  popoverRef.current?.hidePopover();
                }}
                id="restart-mqtt-manager"
                className="block px-4 py-2 cursor-pointer group"
              >
                <span className="mdi mdi-restart pr-2 group-hover:text-warning"></span>Restart MQTTManager
              </button>
            </li>
          </ul>
        </div>
      </div>
      <div>
        {(() => {
          // If we haven't loaded any NSPanels yet, show a loading indicator
          if (status != "loaded") {
            return (
              <div className="flex w-full items-center justify-center text-2xl">
                <span className="loading loading-spinner loading-sm mr-2"></span>
                <span className="font-medium">Loading NSPanels...</span>
              </div>
            );
          }

          if (nspanels && Object.keys(nspanels).length > 0) {
            return (
              <div className="grid md:grid-cols-3 gap-4 mt-4 items-stretch" id="nspanels_container">
                {Object.entries(nspanels).map(([mac, _status]) => (
                  <NSPanelBox key={mac} id={mac} />
                ))}
              </div>
            );
          } else {
            return (
              <div className="grid md:grid-cols-3 gap-4 mt-4 items-start" id="nspanels_container">
                <div className="col-span-3 w-full text-center text-xl text-base-content">
                  No NSPanels found. Getting started?{" "}
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

export default NSPanelsIndexView;
