import { Icon } from "@mdi/react";
import { mdiInformationOutline } from "@mdi/js";
import { useState } from "react";

import { type UseFormSetValue, type UseFormGetValues, type UseFormRegister, type UseFormWatch } from "react-hook-form";
import { type InitialSetupFormValues } from "./form";

const Step4_MqttSettings = ({
  setValue,
  getValues,
  register,
  watch,
  nextStep,
  prevStep,
}: {
  setValue: UseFormSetValue<InitialSetupFormValues>;
  getValues: UseFormGetValues<InitialSetupFormValues>;
  register: UseFormRegister<InitialSetupFormValues>;
  watch: UseFormWatch<InitialSetupFormValues>;
  nextStep: () => void;
  prevStep: () => void;
}) => {
  const [noLogin, setNoLogin] = useState(getValues("mqtt_username") == "" && getValues("mqtt_password") == "");
  const address = watch("mqtt_server");
  const port = watch("mqtt_port");

  return (
    <div>
      <div className="alert alert-info my-4">
        <Icon path={mdiInformationOutline} className="w-6" />
        <span>
          If you are running the MQTT broker (for example Mosquitto) as an add on in Home Assistant. Then enter the address for Home Assistant and the port for
          MQTT (1883 if you have not changed it).
        </span>
      </div>
      <div className="w-full">
        <div className="flex justify-center mb-4 duration-500 transition-transform ease-linear">
          {/* Name */}
          <div className="w-full">
            <label htmlFor="add_new_button_name" className="block mb-2 text-sm font-medium">
              MQTT Broker Address
            </label>
            <div className="flex">
              <input
                className="outline-none bg-base-300 border-neutral rounded-md border focus:ring-0 focus:border-accent block flex-1 min-w-0 w-full text-sm p-2.5 peer/search_text"
                type="text"
                {...register("mqtt_server")}
                required
              />
            </div>
          </div>
        </div>
        <div className="flex justify-center mb-4 duration-500 transition-transform ease-linear">
          {/* Port */}
          <div className="w-full">
            <label htmlFor="manager_port" className="block mb-2 text-sm font-medium">
              MQTT Broker Port
            </label>
            <div className="flex">
              <input
                className="outline-none bg-base-300 border-neutral rounded-md border focus:ring-0 focus:border-accent block flex-1 min-w-0 w-full text-sm p-2.5 peer/search_text"
                type="number"
                {...register("mqtt_port", { valueAsNumber: true })}
                required
              />
            </div>
          </div>
        </div>
        <fieldset className="fieldset bg-base-100 border-base-300 rounded-box w-full border p-4">
          <legend className="fieldset-legend">Login options</legend>
          <label className="label">
            <input
              type="checkbox"
              className="checkbox checkbox-sm checkbox-primary"
              checked={noLogin}
              onChange={(e) => {
                if (e.currentTarget.checked) {
                  setValue("mqtt_username", "");
                  setValue("mqtt_password", "");
                  setValue("clear_mqtt_password", true);
                  setNoLogin(true);
                } else {
                  setNoLogin(false);
                }
              }}
            />
            Anonymous login
          </label>

          <div className="flex justify-center my-4 duration-500 transition-transform ease-linear" hidden={noLogin}>
            {/* Username */}
            <div className="w-full">
              <label htmlFor="mqtt_username" className="block mb-2 text-sm font-medium">
                MQTT Broker Username
              </label>
              <div className="flex">
                <input
                  className="outline-none bg-base-300 border-neutral rounded-md border focus:ring-0 focus:border-accent block flex-1 min-w-0 w-full text-sm p-2.5 peer/search_text"
                  type="text"
                  id="mqtt_username"
                  {...register("mqtt_username")}
                  required
                />
              </div>
            </div>
          </div>

          <div className="flex justify-center mb-4 duration-500 transition-transform ease-linear" hidden={noLogin == true}>
            {/* Password */}
            <div className="w-full">
              <label htmlFor="mqtt_password" className="block mb-2 text-sm font-medium">
                MQTT Broker Password
              </label>
              <div className="flex">
                <input
                  className="outline-none bg-base-300 border-neutral rounded-md border focus:ring-0 focus:border-accent block flex-1 min-w-0 w-full text-sm p-2.5 peer/search_text"
                  type="password"
                  id="mqtt_password"
                  {...register("mqtt_password")}
                  required
                />
              </div>
            </div>
          </div>
        </fieldset>
      </div>
      <div className="flex gap-2 justify-end mt-8">
        <button type="button" className="btn btn-neutral" onClick={prevStep}>
          Back
        </button>
        <button type="button" className="btn btn-primary" onClick={nextStep} disabled={address == "" || port == null || port < 1 || port > 65535}>
          Next
        </button>
      </div>
    </div>
  );
};

export default Step4_MqttSettings;
