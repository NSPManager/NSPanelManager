import { type formDataType } from "./InitialSetup";
import { Notify } from "../NSPanelToastContainer";

const Step7_Finished = ({ formData, prevStep }: { formData: formDataType; prevStep: () => void }) => {
  function getCookie(name: string) {
    let cookieValue = "";
    if (document.cookie && document.cookie !== "") {
      const cookies = document.cookie.split(";");
      for (let i = 0; i < cookies.length; i++) {
        const cookie = cookies[i].trim();
        if (cookie.substring(0, name.length + 1) === name + "=") {
          cookieValue = decodeURIComponent(cookie.substring(name.length + 1));
          break;
        }
      }
    }
    return cookieValue;
  }

  const saveSettings = () => {
    fetch("/rest/settings", {
      method: "POST",
      headers: {
        "Content-Type": "application/json",
        "X-CSRFToken": getCookie("csrftoken"),
      },
      body: JSON.stringify({ settings: formData }),
    })
      .then((response) => {
        if (response.ok) {
          Notify({ message: "Settings saved successfully.", level: "success", duration: 5000 });
          window.location.reload();
        } else {
          response.json().then((data) => {
            Notify({ message: "Failed to save settings. Error: " + data.message, level: "error", duration: 5000 });
          });
        }
      })
      .catch((error) => {
        Notify({ message: "Failed to save settings. Error: " + error.message, level: "error", duration: 5000 });
      });
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
          contribute.
        </p>
        <p className="my-2 text-center font-bold">Thank you for your support and trust and have fun!</p>

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
          <button type="submit" className="btn btn-neutral" onClick={prevStep}>
            Back
          </button>
          <button type="submit" className="btn btn-primary" onClick={saveSettings}>
            Save
          </button>
        </div>
      </div>
    </div>
  );
};

export default Step7_Finished;
