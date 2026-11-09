import Select, { type OptionProps } from "react-select";
import { type ClassNamesConfig, type GroupBase } from "react-select";
import { useEntityStatesStore, type INSPanelStatusData, type INSPanelWarningData } from "../../stores/EntityStore.ts";
import { useRef, useState, useEffect, forwardRef } from "react";
import { Notify } from "../NSPanelToastContainer.tsx";
import { useStompStore } from "../../stores/StompStore.tsx";
import { motion, useMotionValue, useSpring, useMotionTemplate } from "framer-motion";
import { Icon } from "@mdi/react";
import { api, type Room } from "../../api/client";
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
  mdiMenu,
} from "@mdi/js";
import { number } from "zod/v3";

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

const delete_room = (nspanel_id: number) => {
  fetch(`/rest/nspanel/${nspanel_id}`, { method: "DELETE", headers: { "X-CSRFToken": getCookie("csrftoken") } })
    .then((response) => {
      console.log(response);
    })
    .catch((error) => {
      Notify({ message: `Failed to delete NSPanel: ${error}`, level: "error", duration: 10000 });
    });
};

const DeleteRoomConfirmDialog = forwardRef(
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
          <h3 className="text-lg font-bold">Delete Room</h3>
          <p className="py-4">Are you sure you want to delete Room '{status.name}'?</p>
          <div className="flex justify-end join">
            <button onClick={() => delete_nspanel_dialog_ref.current?.close()} className="btn btn-neutral join-item">
              Cancel
            </button>
            <button
              onClick={() => {
                delete_nspanel_dialog_ref.current?.close();
                delete_room(status.id);
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

const NSPanelBox = ({ nspanel }: { nspanel: INSPanelStatusData }) => {
  const state = nspanel.state;
  return (
    <div className="badge badge-md bg-base-300">
      {(() => {
        if (state == "online") {
          return <span className="indicator-item status status-success"></span>;
        } else if (state == "offline") {
          return <span className="indicator-item status status-error"></span>;
        } else if (state == "rebooting") {
          return <span className="indicator-item status status-info animate-pulse"></span>;
        } else if (state == "waiting") {
          return <span className="indicator-item status status-info animate-pulse"></span>;
        } else if (state == "updating_fw" || state == "updating_fs" || state == "updating_tft") {
          return <span className="indicator-item status status-info animate-pulse"></span>;
        } else {
          return <span className="indicator-item status status-error animate-ping">Unknown state: {state}</span>;
        }
      })()}

      <a href={`/nspanel/${nspanel.id}`}>{nspanel.name}</a>
    </div>
  );
};

const StatusHeader = ({ room }: { room: Room }) => {
  const { nspanels, status, fetchData } = useEntityStatesStore();

  useEffect(() => {
    if (status === "none") {
      fetchData();
    }
  }, [status, fetchData]);

  const room_nspanels = [];
  let any_panel_online = false;
  Object.values(nspanels).forEach((nspanel) => {
    if (nspanel.default_room_id == room.id) {
      room_nspanels.push(nspanel);

      if (
        nspanel.state == "online" ||
        nspanel.state == "rebooting" ||
        nspanel.state == "updating_fs" ||
        nspanel.state == "updating_fw" ||
        nspanel.state == "updating_tft"
      ) {
        any_panel_online = true;
      }
    }
  });

  if (any_panel_online) {
    return <div className="min-h-1 overflow-hidden w-full transition-all ease-linear duration-300 bg-success rounded-t-field" id="panel_header-${id}"></div>;
  } else if (room_nspanels.length > 0 && !any_panel_online) {
    return <div className="min-h-1 overflow-hidden w-full transition-all ease-linear duration-300 bg-error rounded-t-field" id="panel_header-${id}"></div>;
  } else {
    return <div className="min-h-1 overflow-hidden w-full transition-all ease-linear duration-300 bg-base-100 rounded-t-field" id="panel_header-${id}"></div>;
  }
};
const RoomBoxContent = ({ room }: { room: Room }) => {
  const popoverRef = useRef<HTMLUListElement>(null);
  const boxRef = useRef<HTMLDivElement>(null);
  const delete_nspanel_dialog_ref = useRef<HTMLDialogElement>(null);

  const mouseX = useMotionValue(0);
  const mouseY = useMotionValue(0);
  const springX = useSpring(mouseX, { stiffness: 300, damping: 30 });
  const springY = useSpring(mouseY, { stiffness: 300, damping: 30 });

  const clipPath = useMotionTemplate`radial-gradient(circle at ${springX}px ${springY}px, black 0%, black 15%, transparent 50%)`;
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

  const { nspanels, status, fetchData } = useEntityStatesStore();

  useEffect(() => {
    if (status === "none") {
      fetchData();
    }
  }, [status, fetchData]);

  const room_nspanels = [];
  let number_panels_online = 0;
  let bg_class = "bg-base-100";
  Object.values(nspanels).forEach((nspanel) => {
    if (nspanel.default_room_id == room.id) {
      room_nspanels.push(nspanel);

      if (
        nspanel.state == "online" ||
        nspanel.state == "rebooting" ||
        nspanel.state == "updating_fs" ||
        nspanel.state == "updating_fw" ||
        nspanel.state == "updating_tft"
      ) {
        number_panels_online++;
      }
    }
  });

  if (room_nspanels.length > 0 && number_panels_online > 0) {
    bg_class = "bg-success";
  } else if (room_nspanels.length > 0 && number_panels_online <= 0) {
    bg_class = "bg-error";
  }

  return (
    <>
      <div className="relative h-full group/box" ref={boxRef}>
        <motion.div
          style={{ WebkitMaskImage: clipPath, maskImage: clipPath, pointerEvents: "none" }}
          className="absolute tile-background w-full h-full group overflow-hidden mb-1 opacity-0 group-hover/box:opacity-5"
        >
          <div className="grid grid-cols-8 w-full h-full inset-0">
            <div
              className={`w-full h-full ${bg_class} [animation-name:tile-background] [animation-duration:8s] [animation-iteration-count:infinite] opacity-0 [animation-delay:-2s] tile-1 group/box:opacity-50`}
            ></div>
            <div
              className={`w-full h-full ${bg_class} [animation-name:tile-background] [animation-duration:8s] [animation-iteration-count:infinite] opacity-0 [animation-delay:-6s] tile-2 group/box:opacity-50`}
            ></div>
            <div
              className={`w-full h-full ${bg_class} [animation-name:tile-background] [animation-duration:8s] [animation-iteration-count:infinite] opacity-0 [animation-delay:-4s] tile-3 group/box:opacity-50`}
            ></div>
            <div
              className={`w-full h-full ${bg_class} [animation-name:tile-background] [animation-duration:8s] [animation-iteration-count:infinite] opacity-0 [animation-delay:-8s] tile-4 group/box:opacity-50`}
            ></div>
            <div
              className={`w-full h-full ${bg_class} [animation-name:tile-background] [animation-duration:8s] [animation-iteration-count:infinite] opacity-0 [animation-delay:-4s] tile-1 group/box:opacity-50`}
            ></div>
            <div
              className={`w-full h-full ${bg_class} [animation-name:tile-background] [animation-duration:8s] [animation-iteration-count:infinite] opacity-0 [animation-delay:-2s] tile-2 group/box:opacity-50`}
            ></div>
            <div
              className={`w-full h-full ${bg_class} [animation-name:tile-background] [animation-duration:8s] [animation-iteration-count:infinite] opacity-0 [animation-delay:-8s] tile-3 group/box:opacity-50`}
            ></div>
            <div
              className={`w-full h-full ${bg_class} [animation-name:tile-background] [animation-duration:8s] [animation-iteration-count:infinite] opacity-0 [animation-delay:-2s] tile-4 group/box:opacity-50`}
            ></div>

            <div
              className={`w-full h-full ${bg_class} [animation-name:tile-background] [animation-duration:8s] [animation-iteration-count:infinite] opacity-0 [animation-delay:-4s] tile-1 group/box:opacity-50`}
            ></div>
            <div
              className={`w-full h-full ${bg_class} [animation-name:tile-background] [animation-duration:8s] [animation-iteration-count:infinite] opacity-0 [animation-delay:-8s] tile-2 group/box:opacity-50`}
            ></div>
            <div
              className={`w-full h-full ${bg_class} [animation-name:tile-background] [animation-duration:8s] [animation-iteration-count:infinite] opacity-0 [animation-delay:-6s] tile-3 group/box:opacity-50`}
            ></div>
            <div
              className={`w-full h-full ${bg_class} [animation-name:tile-background] [animation-duration:8s] [animation-iteration-count:infinite] opacity-0 [animation-delay:-4s] tile-4 group/box:opacity-50`}
            ></div>
            <div
              className={`w-full h-full ${bg_class} [animation-name:tile-background] [animation-duration:8s] [animation-iteration-count:infinite] opacity-0 [animation-delay:-8s] tile-1 group/box:opacity-50`}
            ></div>
            <div
              className={`w-full h-full ${bg_class} [animation-name:tile-background] [animation-duration:8s] [animation-iteration-count:infinite] opacity-0 [animation-delay:-6s] tile-2 group/box:opacity-50`}
            ></div>
            <div
              className={`w-full h-full ${bg_class} [animation-name:tile-background] [animation-duration:8s] [animation-iteration-count:infinite] opacity-0 [animation-delay:-2s] tile-3 group/box:opacity-50`}
            ></div>
            <div
              className={`w-full h-full ${bg_class} [animation-name:tile-background] [animation-duration:8s] [animation-iteration-count:infinite] opacity-0 [animation-delay:-4s] tile-4 group/box:opacity-50`}
            ></div>

            <div
              className={`w-full h-full col-span-4 ${bg_class} [animation-name:tile-background] [animation-duration:8s] [animation-iteration-count:infinite] opacity-0 [animation-delay:-6s] tile-1 group/box:opacity-50`}
            ></div>
            <div
              className={`w-full h-full col-span-4 ${bg_class} [animation-name:tile-background] [animation-duration:8s] [animation-iteration-count:infinite] opacity-0 [animation-delay:-2s] tile-2 group/box:opacity-50`}
            ></div>
          </div>

          {/*Vertical lines*/}
          <div className="absolute left-1/8 top-0 w-px h-full bg-base-content"></div>
          <div className="absolute left-2/8 top-0 w-px h-full bg-base-content"></div>
          <div className="absolute left-3/8 top-0 w-px h-full bg-base-content"></div>
          <div className="absolute left-4/8 top-0 w-px h-full bg-base-content"></div>
          <div className="absolute left-5/8 top-0 w-px h-full bg-base-content"></div>
          <div className="absolute left-6/8 top-0 w-px h-full bg-base-content"></div>
          <div className="absolute left-7/8 top-0 w-px h-full bg-base-content"></div>
          <div className="absolute left-8/8 top-0 w-px h-full bg-base-content"></div>

          {/*Horizontal lines*/}
          <div className="absolute left-0 top-1/3 w-full h-px bg-base-content"></div>
          <div className="absolute left-0 top-2/3 w-full h-px bg-base-content"></div>
          <div className="absolute left-0 top-3/3 w-full h-px bg-base-content"></div>
        </motion.div>
        <div className="p-2 h-full">
          <div className="flex justify-between mx-auto">
            <a href={`/rooms/${room.id}`} className="font-medium text-lg flex items-center">
              <Icon path={mdiMenu} className="w-5 h-5 inline-block me-1"></Icon>
              {room.friendly_name}
            </a>
            <div className="flex items-center justify-end">
              {(() => {
                if (room_nspanels.length <= 0) {
                  return <span className="flex items-end justify-end text-right italic font-light text-sm">No nspanels</span>;
                } else {
                  return (
                    <span className="flex items-end justify-end text-right italic font-light text-xs">
                      {number_panels_online} of {room_nspanels.length} online
                    </span>
                  );
                }
              })()}
            </div>
          </div>

          <div className="flex flex-wrap gap-1 mt-2">
            {room_nspanels.map((nspanel) => (
              <NSPanelBox nspanel={nspanel}></NSPanelBox>
            ))}
          </div>
        </div>
      </div>
    </>
  );
};

const RoomBox = ({ room: room }: { room: Room }) => {
  // const nspanel_status = useEntityStatesStore.getState().nspanels[mac];

  return (
    <div className="text-base-content rounded-b-box rounded-t-field nspanel-box bg-base-200 hover:bg-base-100 transition-all duration-200 shadow-xl shadow-neutral/3 hover:shadow-neutral/10 overflow-visible h-34">
      <div className="overflow-hidden">
        <StatusHeader room={room} />
      </div>
      <RoomBoxContent room={room}></RoomBoxContent>
    </div>
  );
};

export default RoomBox;
