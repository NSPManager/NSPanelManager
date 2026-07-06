import { type formDataType } from "./InitialSetup";
import { Icon } from "@mdi/react";
import { mdiInformationOutline } from "@mdi/js";
import { useState } from "react";

const Step4_MqttSettings = ({
  formData,
  setFormData,
  nextStep,
  prevStep,
}: {
  formData: formDataType;
  setFormData: (data: formDataType) => void;
  nextStep: () => void;
  prevStep: () => void;
}) => {
  const [anonymousLogin, setAnonymousLogin] = useState(formData.mqtt_username === "");

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
                onChange={(e) => setFormData({ ...formData, mqtt_server: e.target.value })}
                value={formData.mqtt_server}
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
                onChange={(e) => setFormData({ ...formData, mqtt_port: parseInt(e.target.value) })}
                value={formData.mqtt_port}
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
              checked={anonymousLogin}
              onChange={() => {
                setAnonymousLogin(!anonymousLogin);
                setFormData({ ...formData, mqtt_username: "", mqtt_password: "" });
              }}
            />
            Anonymous login
          </label>

          <div className="flex justify-center my-4 duration-500 transition-transform ease-linear" hidden={anonymousLogin}>
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
                  onChange={(e) => setFormData({ ...formData, mqtt_username: e.target.value })}
                  value={formData.mqtt_username}
                  disabled={anonymousLogin}
                  required
                />
              </div>
            </div>
          </div>

          <div className="flex justify-center mb-4 duration-500 transition-transform ease-linear" hidden={anonymousLogin}>
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
                  onChange={(e) => setFormData({ ...formData, mqtt_password: e.target.value })}
                  value={formData.mqtt_password}
                  disabled={anonymousLogin}
                  required
                />
              </div>
            </div>
          </div>
        </fieldset>
      </div>
      <div className="flex gap-2 justify-end mt-8">
        <button type="submit" className="btn btn-neutral" onClick={prevStep}>
          Back
        </button>
        <button type="submit" className="btn btn-primary" onClick={nextStep} disabled={!formData.mqtt_server || !formData.mqtt_port}>
          Next
        </button>
      </div>
    </div>
  );
};

export default Step4_MqttSettings;
