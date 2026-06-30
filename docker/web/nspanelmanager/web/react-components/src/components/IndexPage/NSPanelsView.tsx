import { useEffect } from "react";
import { useEntityStatesStore } from "../../stores/EntityStore.ts";
import NSPanelBox from "./NSPanelBox.tsx";

const NSPanelsIndexView = () => {
  const { nspanels, status, fetchData } = useEntityStatesStore();

  useEffect(() => {
    if (status === "none") {
      fetchData();
    }
  }, [status, fetchData]);

  return (
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
  );
};

export default NSPanelsIndexView;
