import { Notify } from "../NSPanelToastContainer";
import { type UseFormSetValue, type UseFormGetValues, type UseFormRegister } from "react-hook-form";
import { type InitialSetupFormValues } from "./form";

const Step3_ManagerSettings = ({
  setValue,
  getValues,
  register,
  nextStep,
  prevStep,
}: {
  setValue: UseFormSetValue<InitialSetupFormValues>;
  getValues: UseFormGetValues<InitialSetupFormValues>;
  register: UseFormRegister<InitialSetupFormValues>;
  nextStep: () => void;
  prevStep: () => void;
}) => {
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
          setValue("manager_address", data.ip);
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
      address: getValues("manager_address"),
      port: getValues("manager_port").toString(),
    };
    const queryString = new URLSearchParams(params).toString();

    fetch("/rest/mqttmanager/test?" + queryString, {
      method: "GET",
      signal: AbortSignal.timeout(2000),
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
                {...register("manager_address")}
                required
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
                {...register("manager_port", { valueAsNumber: true })}
                required
              />
              <button
                type="button"
                className="btn btn-info rounded-l-none h-auto"
                onClick={() => {
                  setValue("manager_port", parseInt(window.location.port));
                }}
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
        <button type="button" className="btn btn-neutral" onClick={prevStep}>
          Back
        </button>
        <button type="button" className="btn btn-primary" onClick={nextStep}>
          Next
        </button>
      </div>
    </div>
  );
};

export default Step3_ManagerSettings;
