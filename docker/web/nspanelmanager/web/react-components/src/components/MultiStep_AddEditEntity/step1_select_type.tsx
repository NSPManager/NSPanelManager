import { useState } from "react";
import { type handleButtonSelectEvent } from "./MultiStep_AddEditEntity";

const MultiStep_AddEditEntity_Step1 = ({ handleButtonSelectEvent, next_step }: { handleButtonSelectEvent: handleButtonSelectEvent; next_step: () => void }) => {
  const [lightHovered, setLightHovered] = useState(false);
  const [switchHovered, setSwitchHovered] = useState(false);
  const [buttonHovered, setButtonHovered] = useState(false);
  const [thermostatHovered, setThermostatHovered] = useState(false);

  return (
    <div>
      <div className="flex justify-center mb-4 duration-500 transition-transform ease-linear w-full">
        <ul className="steps">
          <li className="step step-primary">Type</li>
          <li className="step">Source</li>
          <li className="step">Entity</li>
        </ul>
      </div>

      <div className="grid grid-cols-3 gap-4 w-full">
        <div
          onMouseEnter={() => setLightHovered(true)}
          onMouseLeave={() => setLightHovered(false)}
          className="group bg-base-100 rounded-box border border-neutral border-dashed p-6 cursor-pointer hover:bg-yellow-500/10"
          onClick={() => {
            handleButtonSelectEvent("entity_type", "light")();
            next_step();
          }}
        >
          <div className="flex justify-center items-center">
            <span className={`mdi ${lightHovered ? "mdi-lightbulb-on" : "mdi-lightbulb"} text-3xl text-neutral-content/50 group-hover:text-yellow-500`}></span>
          </div>
          <div className="flex justify-center items-center">
            <span className="text-base-content/50 group-hover:text-base-content">Light</span>
          </div>
        </div>
        <div
          onMouseEnter={() => setSwitchHovered(true)}
          onMouseLeave={() => setSwitchHovered(false)}
          className="group bg-base-100 rounded-box border border-neutral border-dashed p-6 cursor-pointer hover:bg-yellow-500/10"
          onClick={() => {
            handleButtonSelectEvent("entity_type", "switch")();
            next_step();
          }}
        >
          <div className="flex justify-center items-center">
            <span
              className={`mdi ${switchHovered ? "mdi-toggle-switch-variant" : "mdi-toggle-switch-variant-off"} text-3xl text-neutral-content/50 group-hover:text-yellow-500`}
            ></span>
          </div>
          <div className="flex justify-center items-center">
            <span className="text-base-content/50 group-hover:text-base-content">Switch</span>
          </div>
        </div>
        <div
          onMouseEnter={() => setButtonHovered(true)}
          onMouseLeave={() => setButtonHovered(false)}
          className="group bg-base-100 rounded-box border border-neutral border-dashed p-6 cursor-pointer hover:bg-yellow-500/10"
          onClick={() => {
            handleButtonSelectEvent("entity_type", "button")();
            next_step();
          }}
        >
          <div className="flex justify-center items-center">
            <span
              className={`mdi ${buttonHovered ? "mdi-gesture-tap-button" : "mdi-button-pointer"} text-3xl text-neutral-content/50 group-hover:text-yellow-500`}
            ></span>
          </div>
          <div className="flex justify-center items-center">
            <span className="text-base-content/50 group-hover:text-base-content">Button</span>
          </div>
        </div>
        <div
          onMouseEnter={() => setThermostatHovered(true)}
          onMouseLeave={() => setThermostatHovered(false)}
          className="group bg-base-100 rounded-box border border-neutral border-dashed p-6 cursor-pointer hover:bg-yellow-500/10"
          onClick={() => {
            handleButtonSelectEvent("entity_type", "thermostat")();
            next_step();
          }}
        >
          <div className="flex justify-center items-center">
            <span
              className={`mdi ${thermostatHovered ? "mdi-thermostat-cog" : "mdi-thermostat"} text-3xl text-neutral-content/50 group-hover:text-yellow-500`}
            ></span>
          </div>
          <div className="flex justify-center items-center">
            <span className="text-base-content/50 group-hover:text-base-content">Thermostat/climate</span>
          </div>
        </div>
      </div>
    </div>
  );
};

export default MultiStep_AddEditEntity_Step1;
