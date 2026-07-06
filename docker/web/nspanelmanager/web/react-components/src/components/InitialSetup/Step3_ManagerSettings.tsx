import { useRef } from "react";
import { type formDataType } from "./InitialSetup";
import { Notify } from "../NSPanelToastContainer";

const Step3_ManagerSettings = ({
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
  const managerAddressRef = useRef<HTMLInputElement>(null);
  const managerPortRef = useRef<HTMLInputElement>(null);

  const getIPFromURL = () => {
    fetch("/rest/get_ip_by_hostname", {
      method: "POST",
      headers: {
        "Content-Type": "application/json",
      },
      body: JSON.stringify({ hostname: window.location.hostname }),
    })
      .then((response) => response.json())
      .then((data) => {
        if (data.ip) {
          if (managerAddressRef.current) {
            managerAddressRef.current.value = data.ip;
            setFormData({ ...formData, manager_address: data.ip });
          }
        } else {
          Notify({ message: 'Failed to get IP from URL. No "ip" field in response.', level: "error" });
        }
      })
      .catch(() => {
        Notify({ message: "Failed to get IP from URL.", level: "error" });
      });
  };

  const testManagerAddressAndPort = () => {
    const params = {
      address: formData.manager_address,
      port: formData.manager_port.toString(),
    };
    const queryString = new URLSearchParams(params).toString();

    fetch("/rest/mqttmanager/test?" + queryString, {
      method: "GET",
    })
      .then((response) => {
        if (response.ok) {
          Notify({ message: "Manager address and port are valid.", level: "success", duration: 5000 });
        } else {
          response.json().then((data) => {
            Notify({ message: "Failed to test Manager address and port. Error: " + data.message, level: "error", duration: 5000 });
          });
        }
      })
      .catch((error) => {
        Notify({ message: "Test failed. Error: " + error.message, level: "error", duration: 5000 });
      });
  };

  return (
    <div>
      <div className="w-full">
        <div className="flex justify-center mb-4 duration-500 transition-transform ease-linear">
          {/* Name */}
          <div className="w-full">
            <label htmlFor="add_new_button_name" className="block mb-2 text-sm font-medium">
              Manager IP
            </label>
            <div className="flex">
              <input
                className="outline-none bg-base-300 border-neutral rounded-md rounded-r-none border focus:ring-0 focus:border-accent block flex-1 min-w-0 w-full text-sm p-2.5 peer/search_text"
                type="text"
                onChange={(e) => setFormData({ ...formData, manager_address: e.target.value })}
                value={formData.manager_address}
                required
                ref={managerAddressRef}
              />
              <button type="button" className="btn btn-info rounded-l-none h-auto" onClick={() => getIPFromURL()}>
                Load from URL
              </button>
            </div>
          </div>
        </div>
        <div className="flex justify-center mb-4 duration-500 transition-transform ease-linear">
          {/* Port */}
          <div className="w-full">
            <label htmlFor="manager_port" className="block mb-2 text-sm font-medium">
              Manager Port
            </label>
            <div className="flex">
              <input
                className="outline-none bg-base-300 border-neutral rounded-md rounded-r-none border focus:ring-0 focus:border-accent block flex-1 min-w-0 w-full text-sm p-2.5 peer/search_text"
                type="number"
                onChange={(e) => setFormData({ ...formData, manager_port: parseInt(e.target.value) })}
                value={formData.manager_port}
                required
                ref={managerPortRef}
              />
              <button
                type="button"
                className="btn btn-info rounded-l-none h-auto"
                onClick={() =>
                  managerPortRef.current &&
                  (managerPortRef.current.value = window.location.port) &&
                  setFormData({ ...formData, manager_port: parseInt(window.location.port) })
                }
              >
                Load from URL
              </button>
            </div>
          </div>
        </div>
        <button className="btn btn-accent w-full" onClick={testManagerAddressAndPort}>
          Test settings
        </button>
      </div>
      <div className="flex gap-2 justify-end mt-8">
        <button type="submit" className="btn btn-neutral" onClick={prevStep}>
          Back
        </button>
        <button type="submit" className="btn btn-primary" onClick={nextStep} disabled={!formData.manager_address || !formData.manager_port}>
          Next
        </button>
      </div>
    </div>
  );
};

export default Step3_ManagerSettings;
