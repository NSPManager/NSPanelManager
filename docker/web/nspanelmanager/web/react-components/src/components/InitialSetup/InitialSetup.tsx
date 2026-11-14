import { useState, useRef, useEffect, useCallback } from "react";
import { useForm } from "react-hook-form";
import Step1_Welcome from "./Step1_Welcome";
import Step2_Theme from "./Step2_Theme";
import Step3_ManagerSettings from "./Step3_ManagerSettings";
import Step4_MqttSettings from "./Step4_MqttSettings";
import Step5_HomeAssistantSettings from "./Step5_HomeAssistantSettings";
import Step6_OpenhabSettings from "./Step6_OpenhabSettings";
import { AnimatePresence } from "framer-motion";
import { motion } from "framer-motion";
import Step7_Finished from "./Step7_Finished";
import { api, type Settings } from "../../api/client";
import { type InitialSetupFormValues, toFormValues } from "./form";
import { Alert } from "../common/layout";

export type nextStepType = () => void;
export type handleChangeType = (input: string) => (e: React.ChangeEvent<HTMLInputElement>) => void;
export type handleButtonSelectEvent = (input: string, value: string) => () => void;

type LoadState = { status: "loading" } | { status: "error"; message: string } | { status: "loaded"; settings: Settings };

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
  const [state, setState] = useState<LoadState>({ status: "loading" });

  // Sets the state only once the request finishes.
  const fetchSettings = useCallback(() => {
    api
      .GET("/rest/v2/settings")
      .then(({ data, response }) => setState(data ? { status: "loaded", settings: data } : { status: "error", message: `HTTP ${response.status}` }))
      .catch((error) => setState({ status: "error", message: error instanceof Error ? error.message : String(error) }));
  }, []);

  useEffect(fetchSettings, [fetchSettings]);

  const retry = () => {
    setState({ status: "loading" });
    fetchSettings();
  };

  const dialogRef = useRef<HTMLDialogElement>(null);
  const contentRef = useRef<HTMLDivElement>(null);

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
      {state.status === "loading" && (
        <div className="flex items-center justify-center modal-box max-h-11/12 min-w-180 min-h-60 bg-base-100 text-base-content">
          <span className="loading loading-spinner loading"></span>
          <span className="ml-2 text-xl">Initializing first setup</span>
        </div>
      )}
      {state.status === "error" && (
        <>
          <Alert level="error">Couldn't load settings: {state.message}</Alert>
          <button type="button" className="btn" onClick={retry}>
            Retry
          </button>
        </>
      )}
      {state.status === "loaded" && dialogRef != null && (
        <InitialSetupForm settings={state.settings} dialogRef={dialogRef} isHomeAssistantAddon={isHomeAssistantAddon} />
      )}

      <div className="modal-backdrop"></div>
    </dialog>
  );
};

const InitialSetupForm = ({
  dialogRef,
  isHomeAssistantAddon,
  settings,
}: {
  dialogRef: React.RefObject<HTMLDialogElement | null>;
  isHomeAssistantAddon: boolean;
  settings: Settings;
}) => {
  const [step, setStep] = useState(1);
  const form = useForm<InitialSetupFormValues>({ defaultValues: toFormValues(settings) });
  const { register, setValue, getValues, watch } = form;

  const nextStep = () => setStep((prev) => prev + 1);
  const prevStep = () => setStep((prev) => prev - 1);
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
    <>
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
                return <Step2_Theme setValue={setValue} nextStep={nextStep} prevStep={prevStep} watch={watch} />;
              } else if (step == 3) {
                return <Step3_ManagerSettings setValue={setValue} getValues={getValues} register={register} nextStep={nextStep} prevStep={prevStep} />;
              } else if (step == 4) {
                return (
                  <Step4_MqttSettings setValue={setValue} getValues={getValues} register={register} watch={watch} nextStep={nextStep} prevStep={prevStep} />
                );
              } else if (step == 5) {
                return (
                  <Step5_HomeAssistantSettings
                    getValues={getValues}
                    register={register}
                    nextStep={nextStep}
                    prevStep={prevStep}
                    isHomeAssistantAddon={isHomeAssistantAddon}
                  />
                );
              } else if (step == 6) {
                return <Step6_OpenhabSettings getValues={getValues} register={register} nextStep={nextStep} prevStep={prevStep} />;
              } else if (step == 7) {
                return <Step7_Finished form={form} isHomeAssistantAddon={isHomeAssistantAddon} prevStep={prevStep} dialogRef={dialogRef} />;
              }
            })()}
          </motion.div>
        </AnimatePresence>
      </div>
      <div className="modal-backdrop"></div>
    </>
  );
};

export default InitialSetup;
