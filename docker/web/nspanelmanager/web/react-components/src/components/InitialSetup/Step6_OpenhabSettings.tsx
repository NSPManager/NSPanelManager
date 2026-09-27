import { type formDataType } from "./InitialSetup";
import { Icon } from "@mdi/react";
import { mdiInformationOutline } from "@mdi/js";
import { Notify } from "../NSPanelToastContainer";

const Step6_OpenhabSettings = ({
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
  const testOpenhab = () => {
    const params = {
      address: formData.openhab_address,
      token: formData.openhab_token,
    };
    const queryString = new URLSearchParams(params).toString();

    fetch("/rest/openhab/test?" + queryString, {
      method: "GET",
    })
      .then((response) => {
        if (response.ok) {
          Notify({ message: "OpenHAB address and token are valid.", level: "success", duration: 5000 });
        } else {
          response.json().then((data) => {
            Notify({ message: "Failed to test OpenHAB address and token. Error: " + data.message, level: "error", duration: 5000 });
          });
        }
      })
      .catch((error) => {
        Notify({ message: "Test failed. Error: " + error.message, level: "error", duration: 5000 });
      });
  };

  return (
    <div>
      <div className="alert alert-info my-4">
        <Icon path={mdiInformationOutline} className="w-6" />
        <span>OpenHAB is optional. You may also run NSPanel Manager using Home Assistant.</span>
      </div>
      <div className="alert alert-info my-4">
        <Icon path={mdiInformationOutline} className="w-6" />
        <div>
          <span>Enter OpenHAB address in the form of </span>
          <span className="italic">http(s)://address:port/</span>
        </div>
      </div>
      <div className="w-full">
        <div className="flex justify-center mb-4 duration-500 transition-transform ease-linear">
          {/* Name */}
          <div className="w-full">
            <label htmlFor="add_new_button_name" className="block mb-2 text-sm font-medium">
              OpenHAB address
            </label>
            <div className="flex">
              <input
                className="outline-none bg-base-300 border-neutral rounded-md border focus:ring-0 focus:border-accent block flex-1 min-w-0 w-full text-sm p-2.5"
                type="text"
                onChange={(e) => setFormData({ ...formData, openhab_address: e.target.value })}
                value={formData.openhab_address}
                required
              />
            </div>
          </div>
        </div>
        <div className="flex justify-center mb-4 duration-500 transition-transform ease-linear">
          {/* Port */}
          <div className="w-full">
            <label htmlFor="manager_port" className="block mb-2 text-sm font-medium">
              OpenHAB Token
            </label>
            <div className="flex">
              <input
                className="outline-none bg-base-300 border-neutral rounded-md border focus:ring-0 focus:border-accent block flex-1 min-w-0 w-full text-sm p-2.5"
                type="password"
                onChange={(e) => setFormData({ ...formData, openhab_token: e.target.value })}
                value={formData.openhab_token}
                required
              />
            </div>
          </div>
        </div>
      </div>
      <button className="btn btn-accent w-full" onClick={testOpenhab}>
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

export default Step6_OpenhabSettings;
