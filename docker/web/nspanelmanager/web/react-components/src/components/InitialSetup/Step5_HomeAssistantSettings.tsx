import { Icon } from "@mdi/react";
import { mdiInformationOutline } from "@mdi/js";
import { Notify } from "../NSPanelToastContainer";
import { type UseFormGetValues, type UseFormRegister } from "react-hook-form";
import { type InitialSetupFormValues } from "./form";

const Step5_HomeAssistantSettings = ({
  getValues,
  register,
  isHomeAssistantAddon,
  nextStep,
  prevStep,
}: {
  getValues: UseFormGetValues<InitialSetupFormValues>;
  register: UseFormRegister<InitialSetupFormValues>;
  isHomeAssistantAddon: boolean;
  nextStep: () => void;
  prevStep: () => void;
}) => {
  const testHomeAssistant = () => {
    const address = getValues("home_assistant_address");
    const token = getValues("home_assistant_token");

    if (address == null) {
      Notify({ message: "Home Assistant address is required to test the connection", level: "error", duration: 5000 });
      return;
    }

    if (token == null) {
      Notify({ message: "Home Assistant token is required to test the connection", level: "error", duration: 5000 });
      return;
    }

    const params = {
      address: address,
      token: token,
    };
    const queryString = new URLSearchParams(params).toString();

    fetch("/rest/home_assistant/test?" + queryString, {
      method: "GET",
    })
      .then((response) => {
        if (response.ok) {
          Notify({ message: "Home Assistant address and token are valid.", level: "success", duration: 5000 });
        } else {
          response.json().then((data) => {
            Notify({ message: "Failed to test Home Assistant address and token. Error: " + data.message, level: "error", duration: 5000 });
          });
        }
      })
      .catch((error) => {
        Notify({ message: "Test failed. Error: " + error.message, level: "error", duration: 5000 });
      });
  };

  return (
    <div>
      <div className="alert alert-info my-4" hidden={isHomeAssistantAddon}>
        <Icon path={mdiInformationOutline} className="w-6" />
        <span>Home Assistant is optional. You may also run NSPanel Manager using OpenHAB.</span>
      </div>
      <div className="alert alert-info my-4" hidden={!isHomeAssistantAddon}>
        <Icon path={mdiInformationOutline} className="w-6" />
        <span>You are running NSPanel Manager as a Home Assistant add on. These settings are managed for you by Home Assistant.</span>
      </div>
      <div className="alert alert-info my-4" hidden={isHomeAssistantAddon}>
        <Icon path={mdiInformationOutline} className="w-6" />
        <div>
          <span>Enter Home Assistant address in the form of </span>
          <span className="italic">http(s)://address:port/</span>
        </div>
      </div>
      <div className="w-full" hidden={isHomeAssistantAddon}>
        <div className="flex justify-center mb-4 duration-500 transition-transform ease-linear">
          {/* Name */}
          <div className="w-full">
            <label htmlFor="add_new_button_name" className="block mb-2 text-sm font-medium">
              Home Assistant address
            </label>
            <div className="flex">
              <input
                className="outline-none bg-base-300 border-neutral rounded-md border focus:ring-0 focus:border-accent block flex-1 min-w-0 w-full text-sm p-2.5"
                type="text"
                {...register("home_assistant_address")}
                disabled={isHomeAssistantAddon}
                required
              />
            </div>
          </div>
        </div>
        <div className="flex justify-center mb-4 duration-500 transition-transform ease-linear" hidden={isHomeAssistantAddon}>
          {/* Port */}
          <div className="w-full">
            <label htmlFor="manager_port" className="block mb-2 text-sm font-medium">
              Home Assistant Token
            </label>
            <div className="flex">
              <input
                className="outline-none bg-base-300 border-neutral rounded-md border focus:ring-0 focus:border-accent block flex-1 min-w-0 w-full text-sm p-2.5"
                type="password"
                {...register("home_assistant_token")}
                disabled={isHomeAssistantAddon}
                required
              />
            </div>
          </div>
        </div>
      </div>
      <button className="btn btn-accent w-full" onClick={testHomeAssistant} hidden={isHomeAssistantAddon}>
        Test settings
      </button>
      <div className="flex gap-2 justify-end mt-8">
        <button type="submit" className="btn btn-neutral" onClick={prevStep}>
          Back
        </button>
        <button type="submit" className="btn btn-primary" onClick={nextStep}>
          Next
        </button>
      </div>
    </div>
  );
};

export default Step5_HomeAssistantSettings;
