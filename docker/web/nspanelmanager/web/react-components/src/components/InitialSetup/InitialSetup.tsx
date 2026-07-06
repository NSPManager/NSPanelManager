import { useState, useRef, useEffect } from "react";
import Step1_Welcome from "./Step1_Welcome";
import Step2_Theme from "./Step2_Theme";
import Step3_ManagerSettings from "./Step3_ManagerSettings";
import Step4_MqttSettings from "./Step4_MqttSettings";
import Step5_HomeAssistantSettings from "./Step5_HomeAssistantSettings";
import Step6_OpenhabSettings from "./Step6_OpenhabSettings";
import { AnimatePresence } from "framer-motion";
import { motion } from "framer-motion";
import { useSettingsStore } from "../../stores/SettingsStore";
import Step7_Finished from "./Step7_Finished";

export type nextStepType = () => void;
export type handleChangeType = (input: string) => (e: React.ChangeEvent<HTMLInputElement>) => void;
export type handleButtonSelectEvent = (input: string, value: string) => () => void;
export interface formDataType {
  theme: string;
  manager_address: string;
  manager_port: number;
  mqtt_server: string;
  mqtt_port: number;
  mqtt_username: string;
  mqtt_password: string;
  home_assistant_address: string;
  home_assistant_token: string;
  openhab_address: string;
  openhab_token: string;
}

const InitialSetup = ({
  opened,
  isHomeAssistantAddon,
}: {
  room_id: number;
  entities_page_id: number;
  room_view_position: number;
  type: string;
  id?: number;
  onComplete?: () => void;
  opened: boolean;
  isHomeAssistantAddon: boolean;
}) => {
  const [step, setStep] = useState(1);
  const [hasPopulatedFromExistingSettings, setHasPopulatedFromExistingSettings] = useState(false);
  const [formData, setFormData] = useState<formDataType>({
    theme: "default",
    manager_address: "",
    manager_port: 8000,
    mqtt_server: "",
    mqtt_port: 1883,
    mqtt_username: "",
    mqtt_password: "",
    home_assistant_address: "",
    home_assistant_token: "",
    openhab_address: "",
    openhab_token: "",
  });

  const settingsStoreStatus = useSettingsStore((state) => state.status);
  if (settingsStoreStatus == "none") {
    useSettingsStore.getState().fetchData();
  } else if (settingsStoreStatus == "loaded" && !hasPopulatedFromExistingSettings) {
    console.log("Settings loaded, updating formData for initial setup.");
    setFormData((prev) => ({
      ...prev,
      theme: useSettingsStore.getState().settings?.theme ?? "default",
      manager_address: useSettingsStore.getState().settings?.manager_address ?? "",
      manager_port: useSettingsStore.getState().settings?.manager_port ?? 8000,
      mqtt_server: useSettingsStore.getState().settings?.mqtt_server ?? "",
      mqtt_port: useSettingsStore.getState().settings?.mqtt_port ?? 1883,
      home_assistant_address: useSettingsStore.getState().settings?.home_assistant_address ?? "",
      openhab_address: useSettingsStore.getState().settings?.openhab_address ?? "",
    }));
    setHasPopulatedFromExistingSettings(true);
  }

  const nextStep = () => setStep((prev) => prev + 1);
  const prevStep = () => setStep((prev) => prev - 1);
  const dialogRef = useRef<HTMLDialogElement>(null);
  const contentRef = useRef<HTMLDivElement>(null);
  // const prevStep = () => setStep((prev) => prev - 1);

  // Automatically switch between overflow-y-visible and overflow-y-auto depending on content height and max height
  const checkOverflow = () => {
    if (contentRef.current) {
      const maxHeightString = window.getComputedStyle(contentRef.current).maxHeight;
      let maxHeight = parseFloat(maxHeightString);
      if (maxHeightString.endsWith("%")) {
        // Convert percentage to pixels based on window height
        maxHeight = Math.trunc((window.innerHeight * maxHeight) / 100);
      }
      contentRef.current.classList.toggle("overflow-y-auto", contentRef.current.scrollHeight > maxHeight);
      contentRef.current.classList.toggle("overflow-y-visible", contentRef.current.scrollHeight <= maxHeight);
    }
  };

  useEffect(() => {
    const observer = new ResizeObserver((entries) => {
      entries.forEach(() => {
        checkOverflow();
      });
    });

    if (contentRef.current) {
      observer.observe(contentRef.current);
    }
  }, [contentRef]);

  return (
    <dialog ref={dialogRef} open={opened ?? false} className="modal duration-200">
      <div ref={contentRef} className={`modal-box max-h-11/12 min-w-180 bg-base-100 text-base-content`}>
        {(() => {
          return (
            <div className="flex justify-center mb-4 w-full">
              <ul className="steps w-full">
                <li
                  className={`step cursor-pointer ${step >= 1 ? "step-primary cursor-pointer" : ""}`}
                  onClick={() => {
                    setStep(1);
                  }}
                >
                  Welcome
                </li>
                <li
                  className={`step cursor-pointer ${step >= 2 ? "step-primary" : ""}`}
                  onClick={() => {
                    setStep(2);
                  }}
                >
                  Theme
                </li>
                <li
                  className={`step cursor-pointer ${step >= 3 ? "step-primary" : ""}`}
                  onClick={() => {
                    setStep(3);
                  }}
                >
                  Manager settings
                </li>
                <li
                  className={`step cursor-pointer ${step >= 4 ? "step-primary" : ""}`}
                  onClick={() => {
                    setStep(4);
                  }}
                >
                  MQTT settings
                </li>
                <li
                  className={`step cursor-pointer ${step >= 5 ? "step-primary" : ""}`}
                  onClick={() => {
                    setStep(5);
                  }}
                >
                  Home Assistant settings
                </li>
                <li
                  className={`step cursor-pointer ${step >= 6 ? "step-primary" : ""}`}
                  onClick={() => {
                    setStep(6);
                  }}
                >
                  OpenHAB settings
                </li>
                <li
                  className={`step cursor-pointer ${step >= 7 ? "step-primary" : ""}`}
                  onClick={() => {
                    setStep(7);
                  }}
                >
                  Finished
                </li>
              </ul>
            </div>
          );
        })()}

        <AnimatePresence mode="wait">
          <motion.div
            layout
            key={`step.${step}`}
            initial={{ opacity: 0, x: 30 }}
            animate={{ opacity: 1, x: 0 }}
            exit={{ opacity: 0, x: -30 }}
            transition={{ duration: 0.1 }}
          >
            {(() => {
              if (step == 1) {
                return <Step1_Welcome nextStep={nextStep} />;
              } else if (step == 2) {
                return <Step2_Theme formData={formData} setFormData={setFormData} nextStep={nextStep} prevStep={prevStep} />;
              } else if (step == 3) {
                return <Step3_ManagerSettings formData={formData} setFormData={setFormData} nextStep={nextStep} prevStep={prevStep} />;
              } else if (step == 4) {
                return <Step4_MqttSettings formData={formData} setFormData={setFormData} nextStep={nextStep} prevStep={prevStep} />;
              } else if (step == 5) {
                return (
                  <Step5_HomeAssistantSettings
                    formData={formData}
                    setFormData={setFormData}
                    nextStep={nextStep}
                    prevStep={prevStep}
                    isHomeAssistantAddon={isHomeAssistantAddon}
                  />
                );
              } else if (step == 6) {
                return <Step6_OpenhabSettings formData={formData} setFormData={setFormData} nextStep={nextStep} prevStep={prevStep} />;
              } else if (step == 7) {
                return <Step7_Finished formData={formData} prevStep={prevStep} />;
              }
            })()}
          </motion.div>
        </AnimatePresence>
      </div>
      <div className="modal-backdrop"></div>
    </dialog>
  );
};

export default InitialSetup;
