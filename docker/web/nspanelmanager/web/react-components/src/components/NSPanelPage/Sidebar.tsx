import { useEffect, useRef, forwardRef } from "react";
import { useEntityStatesStore, type INSPanelStatusData } from "../../stores/EntityStore";
import { Icon } from "@mdi/react";
import {
  mdiMemory,
  mdiThermometer,
  mdiWaterPercent,
  mdiWifiStrength1Alert,
  mdiWifiStrength2,
  mdiWifiStrength3,
  mdiWifiStrength4,
  mdiWifiStrengthAlertOutline,
  mdiInformationOutline,
  mdiRestart,
  mdiWeb,
  mdiUpload,
  mdiTableArrowUp,
  mdiMinusCircle,
} from "@mdi/js";
import { useStompStore } from "../../stores/StompStore";
import { Notify } from "../NSPanelToastContainer";

const NSPanelStatus = ({ status }: { status: INSPanelStatusData }) => {
  if (status.state == "online") {
    return (
      <div className="list-row flex items-center justify-start gap-1 py-3 px-2">
        <Icon path={mdiInformationOutline} className="ms-2 w-4 h-4 inline-block"></Icon>
        <span className="text-base">
          State: <span className="text-success">Online</span>
        </span>
      </div>
    );
  } else if (status.state == "offline") {
    return (
      <div className="list-row flex items-center justify-start gap-1 py-3 px-2">
        <Icon path={mdiInformationOutline} className="ms-2 w-4 h-4 inline-block"></Icon>
        <span className="text-base">
          State: <span className="text-error">Offline</span>
        </span>
      </div>
    );
  } else if (status.state == "rebooting") {
    return (
      <div className="list-row flex items-center justify-start gap-1 py-3 px-2">
        <Icon path={mdiInformationOutline} className="ms-2 w-4 h-4 inline-block"></Icon>
        <span className="text-base">
          State: <span className="text-info">Rebooting</span>
        </span>
      </div>
    );
  } else if (status.state == "updating_fw") {
    return (
      <div className="list-row flex items-center justify-start gap-1 py-3 px-2 relative">
        <Icon path={mdiInformationOutline} className="ms-2 w-4 h-4 inline-block"></Icon>
        <span className="text-base">
          State: <span className="text-primary">Updating firmware ({status.update_progress}%)</span>
        </span>
        <div className="absolute left-0 bottom-0 w-full">
          <div className="min-h-1 bg-primary mx-2" style={{ width: `${status.update_progress}%` }}></div>
        </div>
      </div>
    );
  } else if (status.state == "updating_fs") {
    return (
      <div className="list-row flex items-center justify-start gap-1 py-3 px-2 relative">
        <Icon path={mdiInformationOutline} className="ms-2 w-4 h-4 inline-block"></Icon>
        <span className="text-base">
          State: <span className="text-primary">Updating filesystem ({status.update_progress}%)</span>
        </span>
        <div className="absolute left-0 bottom-0 w-full">
          <div className="min-h-1 bg-primary mx-2" style={{ width: `${status.update_progress}%` }}></div>
        </div>
      </div>
    );
  } else if (status.state == "updating_tft") {
    return (
      <div className="list-row flex items-center justify-start gap-1 py-3 px-2 relative">
        <Icon path={mdiInformationOutline} className="ms-2 w-4 h-4 inline-block"></Icon>
        <span className="text-base">
          State: <span className="text-primary">Updating GUI ({status.update_progress}%)</span>
        </span>
        <div className="absolute left-0 bottom-0 w-full">
          <div className="min-h-1 bg-primary mx-2" style={{ width: `${status.update_progress}%` }}></div>
        </div>
      </div>
    );
  } else {
    return (
      <div className="list-row flex items-center justify-start gap-1 py-3 px-2">
        <Icon path={mdiInformationOutline} className="ms-2 w-4 h-4 inline-block"></Icon>
        <span className="text-base">UNKNOWN STATE! {status.state}</span>
      </div>
    );
  }
};

const NSPanelWiFiStatus = ({ status }: { status: INSPanelStatusData }) => {
  if (status.model != "sonoff" && status.model != "custom") return null;
  if (status.state == "offline" || status.state == "unknown" || status.state == "waiting" || status.state == "rebooting") {
    return (
      <div className="list-row flex items-center justify-start gap-1 text-base py-3 px-2">
        <Icon path={mdiWifiStrengthAlertOutline} className="ms-2 w-4 h-4 inline-block me-1"></Icon>Wifi: -
      </div>
    );
  } else if (status.rssi <= -80) {
    return (
      <div className="list-row flex items-center justify-start gap-1 text-base py-3 px-2">
        <Icon path={mdiWifiStrength1Alert} className="ms-2 w-4 h-4 inline-block"></Icon>
        Wifi: {status.rssi} dBm
      </div>
    );
  } else if (status.rssi <= -70) {
    return (
      <div className="list-row flex items-center justify-start gap-1 text-base py-3 px-2">
        <Icon path={mdiWifiStrength2} className="ms-2 w-4 h-4 inline-block"></Icon>
        Wifi: {status.rssi} dBm
      </div>
    );
  } else if (status.rssi <= -55) {
    return (
      <div className="list-row flex items-center justify-start gap-1 text-base py-3 px-2">
        <Icon path={mdiWifiStrength3} className="ms-2 w-4 h-4 inline-block"></Icon>
        Wifi: {status.rssi} dBm
      </div>
    );
  } else {
    return (
      <div className="list-row flex items-center justify-start gap-1 text-base py-3 px-2">
        <Icon path={mdiWifiStrength4} className="ms-2 w-4 h-4 inline-block"></Icon>
        Wifi: {status.rssi} dBm
      </div>
    );
  }
};

const NSPanelRamStatus = ({ status }: { status: INSPanelStatusData }) => {
  if (status.model != "sonoff" && status.model != "custom") return null;
  if (status.state == "offline" || status.state == "unknown" || status.state == "waiting" || status.state == "rebooting") {
    return (
      <div className="list-row flex items-center justify-start gap-1 text-base py-3 px-2">
        <Icon path={mdiMemory} className="ms-2 w-4 h-4 inline-block me-1"></Icon>RAM: -
      </div>
    );
  } else {
    return (
      <div className="list-row flex items-center justify-start gap-1 text-base py-3 px-2">
        <Icon path={mdiMemory} className="ms-2 w-4 h-4 inline-block"></Icon>
        RAM: {status.ram_usage}%
      </div>
    );
  }
};

const NSPanelTemperatureStatus = ({ status }: { status: INSPanelStatusData }) => {
  if (status.model != "sonoff" && status.model != "custom") return null;
  if (status.state == "offline" || status.state == "unknown" || status.state == "waiting" || status.state == "rebooting") {
    return (
      <div className="list-row flex items-center justify-start gap-1 text-base py-3 px-2">
        <Icon path={mdiThermometer} className="ms-2 w-4 h-4 inline-block me-1"></Icon>Temperature: -
      </div>
    );
  } else {
    return (
      <div className="list-row flex items-center justify-start gap-1 text-base py-3 px-2">
        <Icon path={mdiThermometer} className="ms-2 w-4 h-4 inline-block"></Icon>
        Temp: {status.temperature.toFixed(1)}
        {status.temperature_unit}
      </div>
    );
  }
};

const NSPanelHumidityStatus = ({ status }: { status: INSPanelStatusData }) => {
  if (status.model != "custom") return null;
  if (status.state == "offline" || status.state == "unknown" || status.state == "waiting" || status.state == "rebooting") {
    return (
      <div className="list-row flex items-center justify-start gap-1 text-base py-3 px-2">
        <Icon path={mdiWaterPercent} className="ms-2 w-4 h-4 inline-block me-1"></Icon>Humidity: -
      </div>
    );
  } else {
    return (
      <div className="list-row flex items-center justify-start gap-1 text-base py-3 px-2">
        <Icon path={mdiWaterPercent} className="ms-2 w-4 h-4 inline-block"></Icon>
        Humidity: {status.humidity?.toFixed(1)}%
      </div>
    );
  }
};

const NSPanelPressureStatus = ({ status }: { status: INSPanelStatusData }) => {
  if (status.model != "custom") return null;
  if (status.state == "offline" || status.state == "unknown" || status.state == "waiting" || status.state == "rebooting") {
    return (
      <div className="list-row flex items-center justify-start gap-1 text-base py-3 px-2">
        <Icon path={mdiWaterPercent} className="ms-2 w-4 h-4 inline-block me-1"></Icon>Pressure: -
      </div>
    );
  } else {
    return (
      <div className="list-row flex items-center justify-start gap-1 text-base py-3 px-2">
        <Icon path={mdiWaterPercent} className="ms-2 w-4 h-4 inline-block"></Icon>
        Pressure: {(status.pressure && `${(status.pressure / 100).toFixed(1)} hPa`) || "-"}
      </div>
    );
  }
};

const NSPanelRebootButton = ({ status }: { status: INSPanelStatusData }) => {
  if (status.model != "sonoff" && status.model != "custom") {
    return null;
  }

  return (
    <div
      className="list-row flex items-center justify-start gap-1 py-3 px-2 group cursor-pointer hover:bg-base-300"
      onClick={() => {
        useStompStore.getState().send(`nspanel/${status.mac}/command`, "reboot");
        Notify({ message: `Sent reboot command to ${status.name}.`, level: "success", duration: 2000 });
      }}
    >
      <Icon path={mdiRestart} className="ms-2 w-4 h-4 inline-block group-hover:text-warning"></Icon>
      Reboot
    </div>
  );
};

const NSPanelVisitButton = ({ status }: { status: INSPanelStatusData }) => {
  if (status.model != "sonoff" && status.model != "custom") {
    return null;
  }

  return (
    <a
      className={`list-row flex items-center justify-start gap-1 py-3 px-2 group cursor-pointer hover:bg-base-300 ${status.state === "offline" ? "line-through" : ""}`}
      href={status.state === "offline" ? "#" : `http://${status.ip_address}/`}
      target="_blank"
    >
      <Icon path={mdiWeb} className="ms-2 w-4 h-4 inline-block group-hover:text-info"></Icon>
      Visit
    </a>
  );
};

const NSPanelFirmwareUpdateButton = ({ status }: { status: INSPanelStatusData }) => {
  if (status.model != "sonoff" && status.model != "custom") return;

  return (
    <div
      className="list-row flex items-center justify-start gap-1 py-3 px-2 group cursor-pointer hover:bg-base-300"
      onClick={() => {
        if (status.state === "offline") return;
        useStompStore.getState().send(`nspanel/${status.mac}/command`, "firmware_update");
        Notify({ message: `Sent firmware update command.`, level: "success", duration: 2000 });
      }}
    >
      <Icon path={mdiUpload} className={`ms-2 w-4 h-4 inline-block group-hover:text-success ${status.state === "offline" ? "line-through" : ""}`}></Icon>
      Update firmware
    </div>
  );
};

const NSPanelForceFirmwareUpdateButton = ({ status }: { status: INSPanelStatusData }) => {
  if (status.model != "sonoff" && status.model != "custom") return;

  return (
    <div
      className="list-row flex items-center justify-start gap-1 py-3 px-2 group cursor-pointer hover:bg-base-300"
      onClick={() => {
        if (status.state === "offline") return;
        useStompStore.getState().send(`nspanel/${status.mac}/command`, "firmware_update_force");
        Notify({ message: `Sent force firmware update command.`, level: "success", duration: 2000 });
      }}
    >
      <Icon path={mdiUpload} className={`ms-2 w-4 h-4 inline-block group-hover:text-success ${status.state === "offline" ? "line-through" : ""}`}></Icon>
      Force update firmware
    </div>
  );
};

const NSPanelGuiUpdateButton = ({ status }: { status: INSPanelStatusData }) => {
  if (status.model != "sonoff" && status.model != "custom") return;

  return (
    <div
      className="list-row flex items-center justify-start gap-1 py-3 px-2 group cursor-pointer hover:bg-base-300"
      onClick={() => {
        if (status.state === "offline") return;
        useStompStore.getState().send(`nspanel/${status.mac}/command`, "gui_update");
        Notify({ message: `Sent GUI update command.`, level: "success", duration: 2000 });
      }}
    >
      <Icon path={mdiTableArrowUp} className={`ms-2 w-4 h-4 inline-block group-hover:text-success ${status.state === "offline" ? "line-through" : ""}`}></Icon>
      Update GUI
    </div>
  );
};

const NSPanelDeleteButton = ({ delete_confirm_dialog_ref }: { delete_confirm_dialog_ref: React.RefObject<HTMLDialogElement | null> }) => {
  return (
    <div
      className="list-row flex items-center justify-start gap-1 py-3 px-2 group cursor-pointer hover:bg-base-300"
      onClick={() => {
        if (delete_confirm_dialog_ref.current) {
          delete_confirm_dialog_ref.current.showModal();
        }
      }}
    >
      <Icon path={mdiMinusCircle} className={`ms-2 w-4 h-4 inline-block group-hover:text-error`}></Icon>
      Delete
    </div>
  );
};

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

const NSPanelDeleteConfirmDialog = forwardRef(
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
                fetch(`/rest/nspanel/${status.id}`, { method: "DELETE", headers: { "X-CSRFToken": getCookie("csrftoken") } })
                  .then(() => {
                    window.location.href = "/";
                  })
                  .catch((error) => {
                    Notify({ message: `Failed to delete NSPanel: ${error}`, level: "error", duration: 10000 });
                  });
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

export default function Sidebar({ nspanel_mac }: { nspanel_mac: string }) {
  const { nspanels, status, fetchData } = useEntityStatesStore();
  useEffect(() => {
    if (status === "none") {
      fetchData();
    }
  }, [status, fetchData]);

  const delete_confirm_dialog_ref = useRef<HTMLDialogElement>(null);

  if (status == "loading" || status == "none") {
    return (
      <div className="border-base-300 bg-base-100 p-2 rounded-box list">
        <span className="text-lg">Loading...</span>
      </div>
    );
  } else if (status == "loaded" && !nspanels[nspanel_mac]) {
    return <span className="text-error text-md">NSPanel state not found for panel with MAC: {nspanel_mac}!</span>;
  }

  return (
    <>
      <NSPanelDeleteConfirmDialog ref={delete_confirm_dialog_ref} delete_nspanel_dialog_ref={delete_confirm_dialog_ref} status={nspanels[nspanel_mac]} />

      <div>
        <span className="font-medium text-lg">Status</span>
        <div className="border-base-300 bg-base-100 rounded-box list">
          <NSPanelStatus status={nspanels[nspanel_mac]} />
          <NSPanelWiFiStatus status={nspanels[nspanel_mac]} />
          <NSPanelRamStatus status={nspanels[nspanel_mac]} />
          <NSPanelTemperatureStatus status={nspanels[nspanel_mac]} />
          <NSPanelHumidityStatus status={nspanels[nspanel_mac]} />
          <NSPanelPressureStatus status={nspanels[nspanel_mac]} />
        </div>
      </div>

      <div className="mt-2">
        <span className="font-medium text-lg">Actions</span>
        <ul className="border-base-300 bg-base-100 rounded-box list w-full">
          <NSPanelRebootButton status={nspanels[nspanel_mac]} />
          <NSPanelVisitButton status={nspanels[nspanel_mac]} />
          <NSPanelFirmwareUpdateButton status={nspanels[nspanel_mac]} />
          <NSPanelForceFirmwareUpdateButton status={nspanels[nspanel_mac]} />
          <NSPanelGuiUpdateButton status={nspanels[nspanel_mac]} />
          <NSPanelDeleteButton delete_confirm_dialog_ref={delete_confirm_dialog_ref} />
        </ul>
      </div>
    </>
  );
}
