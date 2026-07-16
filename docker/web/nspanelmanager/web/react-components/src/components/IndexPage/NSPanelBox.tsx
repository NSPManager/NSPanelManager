import Select, { type OptionProps } from "react-select";
import { type ClassNamesConfig, type GroupBase } from "react-select";
import { useEntityStatesStore, type INSPanelStatusData, type INSPanelWarningData } from "../../stores/EntityStore.ts";
import { useRef, useState, useEffect, forwardRef } from "react";
import { Notify } from "../NSPanelToastContainer.tsx";
import { useStompStore } from "../../stores/StompStore.tsx";
import { motion, useMotionValue, useSpring, useMotionTemplate } from "framer-motion";
import { Icon } from "@mdi/react";
import {
  mdiAlertCircle,
  mdiGauge,
  mdiCog,
  mdiMemory,
  mdiMinusCircle,
  mdiRestart,
  mdiTableArrowUp,
  mdiThermometer,
  mdiUpload,
  mdiWaterPercent,
  mdiWeb,
  mdiWifiStrength1Alert,
  mdiWifiStrength2,
  mdiWifiStrength3,
  mdiWifiStrength4,
  mdiWifiStrengthAlertOutline,
} from "@mdi/js";

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

const delete_nspanel = (nspanel_id: number) => {
  fetch(`/rest/nspanel/${nspanel_id}`, { method: "DELETE", headers: { "X-CSRFToken": getCookie("csrftoken") } })
    .then((response) => {
      console.log(response);
    })
    .catch((error) => {
      Notify({ message: `Failed to delete NSPanel: ${error}`, level: "error", duration: 10000 });
    });
};

const DeleteNSPanelConfirmDialog = forwardRef(
  (
    {
      status,
      delete_nspanel_dialog_ref,
    }: {
      status: INSPanelStatusData;
      delete_nspanel_dialog_ref: React.RefObject<HTMLDialogElement | null>;
    },
    ref: React.Ref<HTMLDialogElement>,
  ) => {
    return (
      <dialog ref={ref} className="modal">
        <div className="modal-box">
          <h3 className="text-lg font-bold">Delete NSPanel</h3>
          <p className="py-4">Are you sure you want to delete NSPanel '{status.name}'?</p>
          <div className="flex justify-end join">
            <button onClick={() => delete_nspanel_dialog_ref.current?.close()} className="btn btn-neutral join-item">
              Cancel
            </button>
            <button
              onClick={() => {
                delete_nspanel_dialog_ref.current?.close();
                delete_nspanel(status.id);
              }}
              className="btn btn-error join-item"
            >
              Delete
            </button>
          </div>
        </div>
        <form method="dialog" className="modal-backdrop">
          <button>close</button>
        </form>
      </dialog>
    );
  },
);

const NSPanelStatusHeader = ({ state, update_progress }: { state: string; update_progress: number }) => {
  if (state == "online") {
    return <div className="min-h-1 overflow-hidden w-full transition-all ease-linear duration-300 bg-success rounded-t-field" id="panel_header-${id}"></div>;
  } else if (state == "offline") {
    return <div className="min-h-1 overflow-hidden w-full transition-all ease-linear duration-300 bg-error rounded-t-field" id="panel_header-${id}"></div>;
  } else if (state == "rebooting") {
    return (
      <div className="min-h-1 overflow-hidden w-full bg-info rounded-t-field animate-progress-indeterminate origin-left-right" id="panel_header-${id}"></div>
    );
  } else if (state == "waiting") {
    return (
      <div
        className="min-h-1 overflow-hidden w-full transition-all ease-linear duration-300 bg-info animate-pulse rounded-t-field"
        id="panel_header-${id}"
      ></div>
    );
  } else if (state == "updating_fw" || state == "updating_fs" || state == "updating_tft") {
    if (update_progress == 0) {
      return (
        <div
          className="min-h-1 overflow-hidden w-full transition-all ease-linear duration-300 bg-info animate-pulse rounded-t-field"
          id="panel_header-${id}"
        ></div>
      );
    } else {
      return (
        <div
          className="min-h-1 overflow-hidden w-full transition-all ease-linear duration-1000 bg-info rounded-t-field"
          style={{ width: `${update_progress}%` }}
          id="panel_header-${id}"
        ></div>
      );
    }
  } else if (state == "awaiting_accept") {
    return (
      <div
        className="min-h-1 overflow-hidden w-full transition-all ease-linear duration-300 bg-accent animate-pulse rounded-t-field"
        id="panel_header-${id}"
      ></div>
    );
  } else {
    return (
      <div
        className="min-h-1 overflow-hidden w-full transition-all ease-linear duration-300 bg-warning text-warning-content animate-pulse rounded-t-field"
        id="panel_header-${id}"
      >
        Unknown state: {state}
      </div>
    );
  }
};

const NSPanelWarnings = ({ warnings }: { warnings: INSPanelWarningData[] }) => {
  if (warnings.length === 0) return null;

  let highest_warning_level = 5;
  let badge_color = "status-base-content";
  warnings.forEach((warning) => {
    switch (warning.level) {
      case "error":
        highest_warning_level = 1;
        badge_color = "status-error";
        break;
      case "warning":
        if (highest_warning_level > 2) highest_warning_level = 2;
        badge_color = "status-warning";
        break;
      case "info":
        if (highest_warning_level > 3) highest_warning_level = 3;
        badge_color = "status-info";
        break;
      case "debug":
        if (highest_warning_level > 4) highest_warning_level = 4;
        badge_color = "status-base-content";
        break;
    }
  });

  return (
    <div className="dropdown dropdown-hover dropdown-end p-0 m-0">
      <div className="flex items-end justify-end px-1">
        <div className={`status ${badge_color} status-lg`} tabIndex={1} role="button"></div>
      </div>
      <ul tabIndex={-1} className="dropdown-content list backdrop-blur-md backdrop-brightness-80 border border-warning/50 w-92 rounded-box shadow-md">
        {warnings.map((warning, index) => (
          <li key={index} className="list-row">
            <div>
              {warning.level == "error" && <Icon path={mdiAlertCircle} className="w-4 h-4 text-error"></Icon>}
              {warning.level == "warning" && <Icon path={mdiAlertCircle} className="w-4 h-4 text-warning"></Icon>}
              {warning.level == "info" && <Icon path={mdiAlertCircle} className="w-4 h-4 text-info"></Icon>}
              {warning.level == "debug" && <Icon path={mdiAlertCircle} className="w-4 h-4 text-base-content"></Icon>}
            </div>
            <div>
              <div>{warning.text}</div>
              <div className="text-xs uppercase font-semibold opacity-60">{warning.level.charAt(0).toUpperCase() + warning.level.slice(1)}</div>
            </div>
          </li>
        ))}
      </ul>
    </div>
  );
};

const AcceptedNSPanelContent = ({ status }: { status: INSPanelStatusData }) => {
  const popoverRef = useRef<HTMLUListElement>(null);
  const boxRef = useRef<HTMLDivElement>(null);
  const delete_nspanel_dialog_ref = useRef<HTMLDialogElement>(null);

  const mouseX = useMotionValue(0);
  const mouseY = useMotionValue(0);
  const springX = useSpring(mouseX, { stiffness: 300, damping: 30 });
  const springY = useSpring(mouseY, { stiffness: 300, damping: 30 });

  const clipPath = useMotionTemplate`radial-gradient(circle at ${springX}px ${springY}px, black 0%, black 15%, transparent 70%)`;
  useEffect(() => {
    const handleMouseMove = (e: MouseEvent) => {
      const boundingRect = boxRef.current?.getBoundingClientRect();
      if (!boundingRect) return;

      mouseX.set(e.clientX - boundingRect.left);
      mouseY.set(e.clientY - boundingRect.top);
    };
    window.addEventListener("mousemove", handleMouseMove);
    return () => {
      window.removeEventListener("mousemove", handleMouseMove);
    };
  }, [mouseX, mouseY]);

  return (
    <>
      <div className="relative h-auto" ref={boxRef}>
        <motion.div
          style={{ WebkitMaskImage: clipPath, maskImage: clipPath }}
          className="absolute tile-background w-full h-full group overflow-hidden mb-1 opacity-0 hover:opacity-5"
        >
          <div className="grid grid-cols-8 w-full h-full inset-0">
            <div className="w-full h-full bg-accent [animation-name:tile-background] [animation-duration:8s] [animation-iteration-count:infinite] opacity-0 [animation-delay:-2s] tile-1 group-hover:opacity-50"></div>
            <div className="w-full h-full bg-accent [animation-name:tile-background] [animation-duration:8s] [animation-iteration-count:infinite] opacity-0 [animation-delay:-6s] tile-2 group-hover:opacity-50"></div>
            <div className="w-full h-full bg-accent [animation-name:tile-background] [animation-duration:8s] [animation-iteration-count:infinite] opacity-0 [animation-delay:-4s] tile-3 group-hover:opacity-50"></div>
            <div className="w-full h-full bg-accent [animation-name:tile-background] [animation-duration:8s] [animation-iteration-count:infinite] opacity-0 [animation-delay:-2s] tile-4 group-hover:opacity-50"></div>
            <div className="w-full h-full bg-accent [animation-name:tile-background] [animation-duration:8s] [animation-iteration-count:infinite] opacity-0 [animation-delay:-4s] tile-1 group-hover:opacity-50"></div>
            <div className="w-full h-full bg-accent [animation-name:tile-background] [animation-duration:8s] [animation-iteration-count:infinite] opacity-0 [animation-delay:-2s] tile-2 group-hover:opacity-50"></div>
            <div className="w-full h-full bg-accent [animation-name:tile-background] [animation-duration:8s] [animation-iteration-count:infinite] opacity-0 [animation-delay:-2s] tile-3 group-hover:opacity-50"></div>
            <div className="w-full h-full bg-accent [animation-name:tile-background] [animation-duration:8s] [animation-iteration-count:infinite] opacity-0 [animation-delay:-4s] tile-4 group-hover:opacity-50"></div>

            <div className="w-full h-full bg-accent [animation-name:tile-background] [animation-duration:8s] [animation-iteration-count:infinite] opacity-0 [animation-delay:-4s] tile-1 group-hover:opacity-50"></div>
            <div className="w-full h-full bg-accent [animation-name:tile-background] [animation-duration:8s] [animation-iteration-count:infinite] opacity-0 [animation-delay:-2s] tile-2 group-hover:opacity-50"></div>
            <div className="w-full h-full bg-accent [animation-name:tile-background] [animation-duration:8s] [animation-iteration-count:infinite] opacity-0 [animation-delay:-6s] tile-3 group-hover:opacity-50"></div>
            <div className="w-full h-full bg-accent [animation-name:tile-background] [animation-duration:8s] [animation-iteration-count:infinite] opacity-0 [animation-delay:-4s] tile-4 group-hover:opacity-50"></div>
            <div className="w-full h-full bg-accent [animation-name:tile-background] [animation-duration:8s] [animation-iteration-count:infinite] opacity-0 [animation-delay:-2s] tile-1 group-hover:opacity-50"></div>
            <div className="w-full h-full bg-accent [animation-name:tile-background] [animation-duration:8s] [animation-iteration-count:infinite] opacity-0 [animation-delay:-6s] tile-2 group-hover:opacity-50"></div>
            <div className="w-full h-full bg-accent [animation-name:tile-background] [animation-duration:8s] [animation-iteration-count:infinite] opacity-0 [animation-delay:-4s] tile-3 group-hover:opacity-50"></div>
            <div className="w-full h-full bg-accent [animation-name:tile-background] [animation-duration:8s] [animation-iteration-count:infinite] opacity-0 [animation-delay:-2s] tile-4 group-hover:opacity-50"></div>

            <div className="w-full h-full col-span-4 bg-accent [animation-name:tile-background] [animation-duration:8s] [animation-iteration-count:infinite] opacity-0 [animation-delay:-6s] tile-1 group-hover:opacity-50"></div>
            <div className="w-full h-full col-span-4 bg-accent [animation-name:tile-background] [animation-duration:8s] [animation-iteration-count:infinite] opacity-0 [animation-delay:-2s] tile-2 group-hover:opacity-50"></div>
          </div>

          {/*Vertical lines*/}
          <div className="absolute left-1/8 top-0 w-px h-full bg-white"></div>
          <div className="absolute left-2/8 top-0 w-px h-full bg-white"></div>
          <div className="absolute left-3/8 top-0 w-px h-full bg-white"></div>
          <div className="absolute left-4/8 top-0 w-px h-full bg-white"></div>
          <div className="absolute left-5/8 top-0 w-px h-full bg-white"></div>
          <div className="absolute left-6/8 top-0 w-px h-full bg-white"></div>
          <div className="absolute left-7/8 top-0 w-px h-full bg-white"></div>
          <div className="absolute left-8/8 top-0 w-px h-full bg-white"></div>

          {/*Horizontal lines*/}
          <div className="absolute left-0 top-1/3 w-full h-px bg-white"></div>
          <div className="absolute left-0 top-2/3 w-full h-px bg-white"></div>
          <div className="absolute left-0 top-3/3 w-full h-px bg-white"></div>
        </motion.div>
        <DeleteNSPanelConfirmDialog status={status} ref={delete_nspanel_dialog_ref} delete_nspanel_dialog_ref={delete_nspanel_dialog_ref} />
        <div className="p-2 h-full">
          <div className="flex justify-between mx-auto">
            <a href={`/nspanel/${status.id}`} className="font-medium text-lg">
              {status.name}
            </a>
            <div className="flex items-center justify-end">
              {(() => {
                if (status.state === "online") {
                  return null; // No status text to show, panel is working as excpected.
                } else if (status.state === "offline") {
                  return <span className="flex items-end justify-end text-right italic font-light text-sm">Offline</span>;
                } else if (status.state === "rebooting") {
                  return <span className="flex items-end justify-end text-right italic font-light text-sm">Rebooting</span>;
                } else if (status.state === "waiting") {
                  return <span className="flex items-end justify-end text-right italic font-light text-sm">Waiting</span>;
                } else if (status.state === "updating_fw") {
                  return <span className="flex items-end justify-end text-right italic font-light text-sm">Updating firmware, {status.update_progress}%</span>;
                } else if (status.state === "updating_fs") {
                  return (
                    <span className="flex items-end justify-end text-right italic font-light text-sm">Updating filesystem, {status.update_progress}%</span>
                  );
                } else if (status.state === "updating_tft") {
                  return <span className="flex items-end justify-end text-right italic font-light text-sm">Updating GUI, {status.update_progress}%</span>;
                } else {
                  return <span className="flex items-end justify-end text-right italic font-light text-sm text-warning">Unknown status: {status.state}</span>;
                }
                return null;
              })()}
              {status.state != "offline" && <NSPanelWarnings warnings={status.warnings}></NSPanelWarnings>}
            </div>
          </div>

          <div className="flex justify-between items-end">
            <div>
              <div className="my-1">
                {(() => {
                  if (status.model != "sonoff" && status.model != "custom") return null;
                  if (status.state == "offline" || status.state == "unknown" || status.state == "waiting" || status.state == "rebooting") {
                    return (
                      <span className="icon-text text-sm text-base-content/50">
                        <Icon path={mdiWifiStrengthAlertOutline} className="w-4 h-4 inline-block me-1"></Icon>-
                      </span>
                    );
                  } else if (status.rssi <= -80) {
                    return (
                      <span className="icon-text text-sm">
                        <Icon path={mdiWifiStrength1Alert} className="w-4 h-4 inline-block me-1"></Icon>
                        {status.rssi} dBm
                      </span>
                    );
                  } else if (status.rssi <= -70) {
                    return (
                      <span className="icon-text text-sm">
                        <Icon path={mdiWifiStrength2} className="w-4 h-4 inline-block me-1"></Icon>
                        {status.rssi} dBm
                      </span>
                    );
                  } else if (status.rssi <= -55) {
                    return (
                      <span className="icon-text text-sm">
                        <Icon path={mdiWifiStrength3} className="w-4 h-4 inline-block me-1"></Icon>
                        {status.rssi} dBm
                      </span>
                    );
                  } else {
                    return (
                      <span className="icon-text text-sm">
                        <Icon path={mdiWifiStrength4} className="w-4 h-4 inline-block me-1"></Icon>
                        {status.rssi} dBm
                      </span>
                    );
                  }
                })()}
              </div>
              <div className="my-1">
                {(() => {
                  if (status.model != "sonoff" && status.model != "custom") return null;
                  if (status.state == "offline" || status.state == "unknown" || status.state == "waiting" || status.state == "rebooting") {
                    return (
                      <div className="flex items-center justify-start text-base-content/50">
                        <div className="icon-text">
                          <Icon path={mdiThermometer} className="w-4 h-4 inline-block me-1"></Icon>-
                          <span id="temperature-${nspanel_id}" className="text-sm">
                            -
                          </span>
                        </div>
                        {status.model == "custom" && (
                          <>
                            <span className="mx-1">|</span>
                            <div className="icon-text text-base-content/50">
                              <Icon path={mdiWaterPercent} className="w-4 h-4 inline-block me-1"></Icon>-
                              <span id="humidity-${nspanel_id}" className="text-sm">
                                -
                              </span>
                            </div>
                            <span className="mx-1">|</span>
                            <div className="icon-text text-base-content/50">
                              <Icon path={mdiGauge} className="w-4 h-4 inline-block me-1"></Icon>-
                              <span id="pressure-${nspanel_id}" className="text-sm">
                                -
                              </span>
                            </div>
                          </>
                        )}
                      </div>
                    );
                  } else {
                    return (
                      <div className="flex items-center justify-start">
                        <div className="icon-text">
                          <Icon path={mdiThermometer} className="w-4 h-4 inline-block me-1"></Icon>-
                          <span id="temperature-${nspanel_id}" className="text-sm">
                            {status.temperature.toFixed(1)}
                            {status.temperature_unit}
                          </span>
                        </div>
                        {status.model == "custom" && (
                          <>
                            <span className="mx-1">|</span>
                            <div className="icon-text">
                              <Icon path={mdiWaterPercent} className="w-4 h-4 inline-block me-1"></Icon>
                              <span id="humidity-${nspanel_id}" className="text-sm">
                                {(status.humidity && `${status.humidity.toFixed(1)}%`) || "-"}
                              </span>
                            </div>
                            <span className="mx-1">|</span>
                            <div className="icon-text">
                              <Icon path={mdiGauge} className="w-4 h-4 inline-block me-1"></Icon>
                              <span id="pressure-${nspanel_id}" className="text-sm">
                                {(status.pressure && `${(status.pressure / 100).toFixed(1)} hPa`) || "-"}
                              </span>
                            </div>
                          </>
                        )}
                      </div>
                    );
                  }
                  return null;
                })()}
              </div>
              <div className="my-1">
                {(() => {
                  if (status.model != "sonoff" && status.model != "custom") return null;
                  if (status.state == "offline" || status.state == "unknown" || status.state == "waiting" || status.state == "rebooting") {
                    return (
                      <div className="icon-text text-base-content/50">
                        <Icon path={mdiMemory} className="w-4 h-4 inline-block me-1"></Icon>
                        <span id="ram-${nspanel_id}" className="text-sm">
                          -
                        </span>
                      </div>
                    );
                  } else {
                    return (
                      <div className="icon-text">
                        <Icon path={mdiMemory} className="w-4 h-4 inline-block me-1"></Icon>
                        <span id="ram-${nspanel_id}" className="text-sm">
                          {status.ram_usage.toFixed(0)}%
                        </span>
                      </div>
                    );
                  }
                  return null;
                })()}
              </div>
            </div>
          </div>

          <div className="absolute right-2 bottom-2">
            <button
              className="font-medium text-sm px-1 py-2 text-center inline-flex items-center cursor-pointer"
              popoverTarget={`nspanel-actions-${status.id}`}
              style={{ anchorName: `--anchor-nspanel-actions-${status.id}` }}
            >
              <Icon path={mdiCog} className="w-4 h-4"></Icon>
            </button>
            <ul
              className="dropdown menu w-52 rounded-box bg-base-100 shadow-sm"
              popover="auto"
              id={`nspanel-actions-${status.id}`}
              style={{ anchorName: `--anchor-nspanel-actions-${status.id}` }}
              ref={popoverRef}
            >
              {status.model === "sonoff" || status.model === "custom" ? (
                <>
                  <li>
                    <button
                      onClick={() => {
                        useStompStore.getState().send(`nspanel/${status.mac}/command`, "reboot");
                        Notify({ message: `Sent reboot command to ${status.name}.`, level: "success", duration: 2000 });
                        popoverRef.current?.hidePopover();
                      }}
                      id="reboot-{{ id }}"
                      hx-swap="none"
                      className="block px-4 py-2 cursor-pointer nspanel-reboot-button group"
                    >
                      <Icon path={mdiRestart} className="w-4 h-4 inline-block me-1 group-hover:text-warning"></Icon>
                      Reboot
                    </button>
                  </li>
                  <li>
                    <a
                      href={status.state === "offline" ? "#" : `http://${status.ip_address}/`}
                      onClick={() => (status.state != "offline" ? popoverRef.current?.hidePopover() : null)}
                      id="visit-{{ id }}"
                      hx-swap="none"
                      className={`block px-4 py-2 cursor-pointer nspanel-visit-button group ${status.state === "offline" ? "line-through" : ""}`}
                      target="_blank"
                      aria-disabled={status.state === "offline"}
                    >
                      <Icon path={mdiWeb} className="w-4 h-4 inline-block me-1 group-hover:text-info"></Icon>
                      Visit
                    </a>
                  </li>
                  <li className="border-t border-neutral-content"></li>
                  <li>
                    <a
                      onClick={() => {
                        if (status.state !== "offline") {
                          useStompStore.getState().send(`nspanel/${status.mac}/command`, "firmware_update");
                          Notify({ message: `Sent firmware update command to ${status.name}.`, level: "success", duration: 2000 });
                          popoverRef.current?.hidePopover();
                        }
                      }}
                      id="firmware-update-{{ nspanel.data.id }}"
                      className={`block px-4 py-2 cursor-pointer group ${status.state === "offline" ? "line-through" : ""}`}
                      aria-disabled={status.state === "offline"}
                    >
                      <Icon path={mdiUpload} className="w-4 h-4 inline-block me-1 group-hover:text-success"></Icon>
                      Update firmware
                    </a>
                  </li>
                  <li>
                    <a
                      onClick={() => {
                        if (status.state !== "offline") {
                          useStompStore.getState().send(`nspanel/${status.mac}/command`, "gui_update");
                          Notify({ message: `Sent GUI update command to ${status.name}.`, level: "success", duration: 2000 });
                          popoverRef.current?.hidePopover();
                        }
                      }}
                      id="screen-update-{{ nspanel.data.id }}"
                      className={`block px-4 py-2 cursor-pointer group ${status.state === "offline" ? "line-through" : ""}`}
                      aria-disabled={status.state === "offline"}
                    >
                      <Icon path={mdiTableArrowUp} className="w-4 h-4 inline-block me-1 group-hover:text-success"></Icon>
                      Update GUI
                    </a>
                  </li>
                  <li className="border-t border-neutral-content"></li>
                </>
              ) : null}
              <li>
                <button
                  onClick={() => {
                    delete_nspanel_dialog_ref.current?.showModal();
                    popoverRef.current?.hidePopover();
                  }}
                  id="delete-{{ id }}"
                  hx-swap="none"
                  className="block px-4 py-2 cursor-pointer group"
                >
                  <Icon path={mdiMinusCircle} className="w-4 h-4 inline-block me-1 group-hover:text-error"></Icon>
                  Delete
                </button>
              </li>
            </ul>
          </div>
        </div>
      </div>
    </>
  );
};

const deny_nspanel = (nspanel_id: number) => {
  fetch(`/rest/nspanel/${nspanel_id}/deny`, {
    method: "POST",
    headers: {
      "X-CSRFToken": getCookie("csrftoken"),
    },
  })
    .then((response) => response.json())
    .then((data) => {
      console.log(data);
    });
};

const accept_nspanel = ({ nspanel_id, room_id }: { nspanel_id: number; room_id: number }) => {
  fetch(`/rest/nspanel/${nspanel_id}/accept`, {
    method: "POST",
    headers: {
      "X-CSRFToken": getCookie("csrftoken"),
    },
    body: JSON.stringify({ room_id: room_id }),
  })
    .then((response) => response.json())
    .then((data) => {
      console.log(data);
    });
};

const AcceptNewNSPanelDialog = forwardRef(
  (
    {
      status,
      accept_new_nspanel_dialog_ref,
    }: {
      status: INSPanelStatusData;
      accept_new_nspanel_dialog_ref: React.RefObject<HTMLDialogElement | null>;
    },
    ref: React.Ref<HTMLDialogElement>,
  ) => {
    interface IOptionType {
      value: number;
      label: string;
    }
    const [selectedRoom, setSelectedRoom] = useState<IOptionType | null>(null);
    const [availableRooms, setAvailableRooms] = useState<IOptionType[]>([]);
    const classNames: ClassNamesConfig<{ value: number; label: string }, false, GroupBase<{ value: number; label: string }>> = {
      control: (state) => `${state.isFocused ? "border" : "border-0"} border-accent p-2.5 text-sm rounded-box bg-base-300 rounded-md`,
      menu: () => "bg-base-300 p-2.5 rounded-box",
      option: (state) => `p-1 ${state.isSelected ? "bg-primary/20 rounded-sm" : ""} ${state.isFocused ? "bg-primary/20 rounded-sm" : ""}`,
    };
    const CustomOption: React.FC<OptionProps<IOptionType>> = ({ innerProps, isDisabled, isFocused, isSelected, children }) => {
      if (isDisabled) {
        return null;
      }

      return (
        <div {...innerProps} className={`p-1 ${isSelected ? "bg-primary/20 rounded-sm" : ""} ${isFocused ? "bg-primary/20 rounded-sm" : ""}`}>
          {children}
        </div>
      );
    };

    if (availableRooms.length === 0) {
      const rooms = useEntityStatesStore.getState().rooms;
      for (const room of Object.values(rooms)) {
        setAvailableRooms((prev) => [...prev, { value: room.id, label: room.friendly_name }]);
      }
    }

    // Automatically switch between overflow-y-visible and overflow-y-auto depending on content height and max height
    const contentRef = useRef<HTMLDivElement>(null);
    const checkOverflow = () => {
      if (contentRef.current) {
        const maxHeightString = window.getComputedStyle(contentRef.current).maxHeight;
        let maxHeight = parseFloat(maxHeightString);
        if (maxHeightString.endsWith("%")) {
          // Convert percentage to pixels based on window height
          maxHeight = Math.trunc((window.innerHeight * maxHeight) / 100);
        }
        contentRef.current.classList.toggle("overflow-y-auto", contentRef.current.scrollHeight > maxHeight);
        contentRef.current.classList.toggle("overflow-y-visible", contentRef.current.scrollHeight <= maxHeight);
      }
    };

    useEffect(() => {
      const observer = new ResizeObserver((entries) => {
        entries.forEach(() => {
          checkOverflow();
        });
      });

      if (contentRef.current) {
        observer.observe(contentRef.current);
      }
    }, [contentRef]);

    return (
      <dialog ref={ref} className="modal">
        <div className="modal-box" ref={contentRef}>
          <h3 className="text-lg font-bold">Accept NSPanel</h3>
          <label className="block mb-2 text-sm font-medium">Select room</label>
          <Select<IOptionType>
            options={availableRooms}
            classNames={classNames}
            onChange={(newValue) => {
              setSelectedRoom(newValue);
            }}
            value={selectedRoom}
            unstyled
            components={{
              Option: CustomOption,
            }}
            styles={{
              input: (base) => ({
                ...base,
                "input:focus": {
                  boxShadow: "none",
                },
              }),
              // On mobile, the label will truncate automatically, so we want to
              // override that behaviour.
              multiValueLabel: (base) => ({
                ...base,
                whiteSpace: "normal",
                overflow: "visible",
              }),
              control: (base) => ({
                ...base,
                transition: "none",
              }),
            }}
          />
          <div className="flex justify-end join pt-2">
            <button onClick={() => accept_new_nspanel_dialog_ref.current?.close()} className="btn btn-neutral join-item">
              Cancel
            </button>
            <button
              onClick={() => {
                if (!selectedRoom) return;
                accept_new_nspanel_dialog_ref.current?.close();
                accept_nspanel({ nspanel_id: status.id, room_id: selectedRoom.value });
              }}
              className="btn btn-success join-item"
            >
              Accept
            </button>
          </div>
        </div>
        <form method="dialog" className="modal-backdrop">
          <button>close</button>
        </form>
      </dialog>
    );
  },
);

const NewNSPanelContent = ({ status }: { status: INSPanelStatusData }) => {
  const accept_new_nspanel_dialog_ref = useRef<HTMLDialogElement>(null);

  return (
    <div className="flex items-center justify-center h-full w-full">
      <AcceptNewNSPanelDialog status={status} ref={accept_new_nspanel_dialog_ref} accept_new_nspanel_dialog_ref={accept_new_nspanel_dialog_ref} />
      <div>
        <div className="flex w-full items-/center justify-center pt-2">
          <span className="nspanel-name">NSPanel '{status.name}' is awaiting accept.</span>
        </div>
        <div className="flex w-full items-center justify-center pt-2 pb-4">
          <div>
            <button className="btn btn-soft btn-error m-1 rounded-btn" onClick={() => deny_nspanel(status.id)}>
              Deny
            </button>
            <button className="btn btn-soft btn-info m-1 rounded-btn" onClick={() => accept_new_nspanel_dialog_ref.current?.showModal()}>
              Accept
            </button>
          </div>
        </div>
      </div>
    </div>
  );
};

const NSPanelBox = ({ mac: mac }: { mac: string }) => {
  const nspanel_status = useEntityStatesStore.getState().nspanels[mac];

  return (
    <div className="text-base-content rounded-b-box rounded-t-field nspanel-box bg-base-200 hover:bg-base-100 transition-all duration-200 shadow-xl shadow-neutral/3 hover:shadow-neutral/10 overflow-visible h-34">
      <div className="overflow-hidden">
        <NSPanelStatusHeader state={nspanel_status?.state ?? ""} update_progress={nspanel_status?.update_progress ?? 0} />
      </div>

      {(nspanel_status?.accepted && <AcceptedNSPanelContent status={nspanel_status} />) || <NewNSPanelContent status={nspanel_status} />}
    </div>
  );
};

export default NSPanelBox;
