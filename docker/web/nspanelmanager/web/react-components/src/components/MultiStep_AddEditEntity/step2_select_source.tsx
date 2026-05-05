import { type handleButtonSelectEvent } from "./MultiStep_AddEditEntity";

const MultiStep_AddEditEntity_Step2 = ({ handleButtonSelectEvent, next_step }: { handleButtonSelectEvent: handleButtonSelectEvent; next_step: () => void }) => {
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

        <div
          className="group bg-base-100 rounded-box border border-neutral border-dashed p-6 cursor-pointer hover:bg-accent/10"
          onClick={() => {
            handleButtonSelectEvent("controller", "accent")();
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
      </div>
    </div>
  );
};

export default MultiStep_AddEditEntity_Step2;
