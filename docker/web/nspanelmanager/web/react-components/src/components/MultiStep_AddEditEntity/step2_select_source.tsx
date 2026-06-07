import { type handleButtonSelectEvent } from "./MultiStep_AddEditEntity";

const MultiStep_AddEditEntity_Step2 = ({
  handleButtonSelectEvent,
  entity_type,
  next_step,
}: {
  handleButtonSelectEvent: handleButtonSelectEvent;
  entity_type: string;
  next_step: () => void;
}) => {
  const available_controllers = {
    light: ["home_assistant", "openhab"],
    switch: ["home_assistant", "openhab"],
    button: ["home_assistant", "nspm"],
    thermostat: ["home_assistant", "openhab"],
  };

  return (
    <div>
      <div className="flex justify-center mb-4 duration-500 transition-transform ease-linear w-full">
        <ul className="steps">
          <li className="step step-primary">Type</li>
          <li className="step step-primary">Controller</li>
          <li className="step">Entity</li>
        </ul>
      </div>

      <div className="grid grid-cols-3 gap-4 w-full">
        {available_controllers[entity_type]?.includes("home_assistant") && (
          <div
            className="group bg-base-100 rounded-box border border-neutral border-dashed p-6 cursor-pointer hover:bg-home-assistant/10"
            onClick={() => {
              handleButtonSelectEvent("controller", "home_assistant")();
              next_step();
            }}
          >
            <div className="flex justify-center items-center p-4">
              <img src="/static/home-assistant-logo/home-assistant-logomark-color-on-light.svg" alt="Home Assistant logo" className="h-12 w-12" />
            </div>
            <div className="flex justify-center items-center">
              <span className="text-base-content/50 group-hover:text-base-content text-center">Home Assistant</span>
            </div>
          </div>
        )}

        {available_controllers[entity_type]?.includes("openhab") && (
          <div
            className="group bg-base-100 rounded-box border border-neutral border-dashed p-6 cursor-pointer hover:bg-openhab/10"
            onClick={() => {
              handleButtonSelectEvent("controller", "openhab")();
              next_step();
            }}
          >
            <div className="flex justify-center items-center p-4">
              <img src="/static/openhab-logo/openHAB_darkBG_appicon.svg" alt="OpenHAB logo" className="h-12 w-12" />
            </div>
            <div className="flex justify-center items-center">
              <span className="text-base-content/50 group-hover:text-base-content text-center">OpenHAB</span>
            </div>
          </div>
        )}

        {available_controllers[entity_type]?.includes("nspm") && (
          <div
            className="group bg-base-100 rounded-box border border-neutral border-dashed p-6 cursor-pointer hover:bg-accent/10"
            onClick={() => {
              handleButtonSelectEvent("controller", "nspm")();
              next_step();
            }}
          >
            <div className="flex justify-center items-center p-4">
              <img src="/static/icon.svg" alt="NSPanel Manager logo" className="h-12 w-12" />
            </div>
            <div className="flex justify-center items-center">
              <span className="text-base-content/50 group-hover:text-base-content text-center">NSPanel Manager</span>
            </div>
          </div>
        )}
      </div>
    </div>
  );
};

export default MultiStep_AddEditEntity_Step2;
