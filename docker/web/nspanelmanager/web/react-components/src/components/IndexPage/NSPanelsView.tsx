import { useState } from "react";
import { stomp_subscribe } from "./stomp_wrapper";

const NSPanelsIndexView = () => {
  const [nspanels, setNSPanels] = useState([]);

  stomp_subscribe("entity_states", (message) => {
    console.log("Got entity states: ", message.body);
  });

  return (
    <div>
      {(() => {
        if (nspanels && nspanels.length > 0) {
          return (
            <div className="grid md:grid-cols-3 gap-4 mt-4 items-start" id="nspanels_container">
              {nspanels.map((nspanel) => (
                <div key={nspanel.id} className="col-span-3 w-full text-center text-xl text-base-content">
                  <span>
                    {nspanel.id} -- {nspanel.mac}
                  </span>
                </div>
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
