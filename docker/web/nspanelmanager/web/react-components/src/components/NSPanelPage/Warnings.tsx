import { useEffect } from "react";
import { useEntityStatesStore, type INSPanelWarningData } from "../../stores/EntityStore";
import { Icon } from "@mdi/react";
import { mdiAlertCircle } from "@mdi/js";

const NSPanelWarning = ({ warning }: { warning: INSPanelWarningData }) => {
  return (
    <div key={warning.text} className="block p-3">
      <div className="grid grid-cols-[auto_1fr] items-start justify-start gap-2 w-full">
        <div className="grid-cols-subgrid col-span-2 flex items-center justify-start">
          {warning.level == "error" && <Icon path={mdiAlertCircle} className="w-4 h-4 text-error mr-2"></Icon>}
          {warning.level == "warning" && <Icon path={mdiAlertCircle} className="w-4 h-4 text-warning mr-2"></Icon>}
          {warning.level == "info" && <Icon path={mdiAlertCircle} className="w-4 h-4 text-info mr-2"></Icon>}
          {warning.level == "debug" && <Icon path={mdiAlertCircle} className="w-4 h-4 text-base-content mr-2"></Icon>}
          <div className="flex">{warning.text}</div>
        </div>
        <div className="grid-cols-subgrid col-span-2 col-start-2">
          <div className="text-xs pl-4 uppercase font-semibold opacity-60">{warning.level.charAt(0).toUpperCase() + warning.level.slice(1)}</div>
        </div>
      </div>
    </div>
  );
};

export default function NSPanelWarnings({ nspanel_mac }: { nspanel_mac: string }) {
  const { nspanels, status, fetchData } = useEntityStatesStore();
  useEffect(() => {
    if (status === "none") {
      fetchData();
    }
  }, [status, fetchData]);

  if (status == "loading" || status == "none") {
    return null;
  } else if (status == "loaded" && !nspanels[nspanel_mac]) {
    return <span className="text-error text-md">NSPanel state not found for panel with MAC: {nspanel_mac}!</span>;
  }

  return (
    <div className="divide-y divide-neutral w-full backdrop-blur-none backdrop-brightness-75 rounded-box border border-warning/50">
      {nspanels[nspanel_mac].warnings.map((warning) => {
        return <NSPanelWarning warning={warning} />;
      })}
    </div>
  );
}
