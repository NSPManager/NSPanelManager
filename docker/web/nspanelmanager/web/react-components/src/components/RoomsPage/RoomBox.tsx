import { useEntityStatesStore, type INSPanelStatusData } from "../../stores/EntityStore.ts";
import { useRef, useEffect, useState } from "react";
import { motion, useMotionValue, useSpring, useMotionTemplate } from "framer-motion";
import { Icon } from "@mdi/react";
import { type Room } from "../../api/client";
import { mdiMenu } from "@mdi/js";
import { useSortable } from "@dnd-kit/react/sortable";

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

const RoomBoxContent = ({ room, setHandle }: { room: Room; setHandle: React.Dispatch<React.SetStateAction<Element | null>> }) => {
  const boxRef = useRef<HTMLLIElement>(null);

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

  const room_nspanels: INSPanelStatusData[] = [];
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
      <div className="relative h-full group/box">
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
            <div className="flex items-center">
              <div ref={setHandle} className="flex">
                <Icon path={mdiMenu} className="w-5 h-5 inline-block me-1 cursor-grab"></Icon>
              </div>
              <a href={`/rooms/${room.id}`} className="font-medium text-lg flex items-center">
                {room.friendly_name}
              </a>
            </div>
            <div className="flex items-center justify-end">
              {(() => {
                if (room_nspanels.length <= 0) {
                  return <span className="flex items-end justify-end text-right italic font-light text-sm">No panels</span>;
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

const RoomBox = ({ room: room, index: index }: { room: Room; index: number }) => {
  const id = room.id;

  // Sortable entity page vars
  const [handle, setHandle] = useState<Element | null>(null);
  const { ref } = useSortable({
    id,
    index: index,
    data: { type: "room", room: room },
    handle: handle,
    disabled: false,
  });

  return (
    <li
      ref={ref}
      className="text-base-content rounded-b-box rounded-t-field nspanel-box bg-base-200 hover:bg-base-100 transition-all duration-200 shadow-xl shadow-neutral/3 hover:shadow-neutral/10 overflow-visible h-34"
    >
      <div className="overflow-hidden">
        <StatusHeader room={room} />
      </div>
      <RoomBoxContent room={room} setHandle={setHandle}></RoomBoxContent>
    </li>
  );
};

export default RoomBox;
