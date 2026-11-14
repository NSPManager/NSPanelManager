import { Notify } from "../NSPanelToastContainer";
import { type UseFormReturn } from "react-hook-form";
import { type InitialSetupFormValues, buildPatch, toFormValues } from "./form";
import { api } from "../../api/client";

const Step7_Finished = ({
  form,
  prevStep,
  isHomeAssistantAddon,
  dialogRef,
}: {
  form: UseFormReturn<InitialSetupFormValues>;
  prevStep: () => void;
  isHomeAssistantAddon: boolean;
  dialogRef: React.RefObject<HTMLDialogElement>;
}) => {
  const saveSettings = async () => {
    const {
      getValues,
      reset,
      formState: { dirtyFields },
    } = form;

    const body = buildPatch(getValues(), dirtyFields, isHomeAssistantAddon);
    try {
      const { data, error, response } = await api.PATCH("/rest/v2/settings", { body });
      if (data) {
        reset(toFormValues(data));
        Notify({ message: "Settings saved.", level: "success", duration: 5000 });
        dialogRef.current?.close();
        return;
      }
      if (response.status === 400 && error && !("detail" in error && error.detail)) {
        console.error("Got error ", error);

        const fieldErrors = Object.entries(error as Record<string, string[] | string>);
        if (fieldErrors.length === 0) {
          Notify({ message: "Nothing was saved.", level: "error", duration: 8000 });
        }

        for (const [field, messages] of fieldErrors) {
          for (const message of Array.isArray(messages) ? messages : [messages]) {
            Notify({
              message: `${field}: ${message}`,
              level: "error",
              duration: 8000,
            });
          }
        }
      } else {
        const detail = error && "detail" in error ? error.detail : `HTTP ${response.status}`;
        Notify({ message: `Couldn't save the settings: ${detail}`, level: "error" });
      }
    } catch (failure) {
      Notify({ message: `Couldn't save the settings: ${failure instanceof Error ? failure.message : String(failure)}`, level: "error" });
    }
  };

  return (
    <div>
      <div className="w-full">
        <div className="flex items-center justify-center space-x-3 rtl:space-x-reverse">
          <div className="bg-neutral/50 p-2 rounded-box">
            <img src="/static/logo.svg" alt="Logo" className="h-12" />
          </div>
        </div>

        <div className="mt-1 text-center">
          <span className="text-center">Version: %version%</span>
        </div>

        <p className="mt-2 text-center">
          Initial setup is now finished. If you have any NSPanels already flashed and connected to the same MQTT broker they should show up automatically. In
          case they do not, there is the documentation and also a discord where you can get help. There is also the Github page where you can report issues and
        </p>
        <p className="my-2 text-center font-bold">Thank you for your support and trust. Have fun and enjoy!</p>

        <div id="about_links" className="join join-vertical w-full">
          <a className="join-item btn btn-link" href="https://nspanelmanager.com" target="_blank">
            Documentation
          </a>
          <a className="join-item btn btn-link" href="https://github.com/NSPManager/NSPanelManager" target="_blank">
            GitHub
          </a>
          <a className="join-item btn btn-link" href="https://discord.gg/RwXvAH56fE" target="_blank">
            Discord
          </a>
          <div className="join-item flex items-center justify-center">
            <a href="https://www.buymeacoffee.com/nspanelmanager" className="mt-4" target="_blank" rel="nofollow">
              <img src="/static/bmc-button.png" alt="Buy Us A Coffee" height="41" width="174" />
            </a>
          </div>
        </div>

        <div className="flex gap-2 justify-end mt-8">
          <button type="button" className="btn btn-neutral" onClick={prevStep}>
            Back
          </button>
          <button type="button" className="btn btn-primary" onClick={saveSettings}>
            Save
          </button>
        </div>
      </div>
    </div>
  );
};

export default Step7_Finished;
