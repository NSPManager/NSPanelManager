import { useState, useRef, useEffect } from "react";
import MultiStep_AddEditEntity_Step1 from "./step1_select_type";
import MultiStep_AddEditEntity_Step2 from "./step2_select_source";
import MultiStep_AddEditEntity_Step3_Light from "./entity_types/step3_edit_light";
import MultiStep_AddEditEntity_Step3_Switch from "./entity_types/step3_edit_switch";
import MultiStep_AddEditEntity_Step3_Button from "./entity_types/step3_edit_button";
import MultiStep_AddEditEntity_Step3_Thermostat from "./entity_types/step3_edit_thermostat";
import MultiStep_AddEditEntity_Step3_Scene from "./entity_types/step3_edit_scene";
import { useAvailableEntitiesStore } from "../../stores/AvailableEntitiesStore";
import { useEntitiesPagesStore } from "../../stores/EntitiesPagesStore";
import { useSettingsStore } from "../../stores/SettingsStore";
import { AnimatePresence } from "framer-motion";
import { motion } from "framer-motion";
// import Step2 from "./step2_select_controller";
// import Step3 from "./Step3";

export type nextStepType = () => void;
export type handleChangeType = (input: string) => (e: React.ChangeEvent<HTMLInputElement>) => void;
export type handleButtonSelectEvent = (input: string, value: string) => () => void;
export interface formDataType {
  [key: string]: string | number | boolean | null;
}

const MultiStep_AddOrEditEntity = ({
  room_id,
  entities_page_id,
  room_view_position,
  type,
  id,
  onComplete,
  onClose,
  opened,
  setOpened,
}: {
  room_id: number;
  entities_page_id: number;
  room_view_position: number;
  type: string;
  id?: number;
  onComplete?: () => void;
  onClose?: () => void;
  opened: boolean;
  setOpened: (opened: boolean) => void;
}) => {
  const [step, setStep] = useState(1);
  const defaultFormData = {
    room_id: room_id,
    entities_page_id: entities_page_id,
    room_view_position: room_view_position,
    controller: "",
    type: type,
    entity_type: "",
    friendly_name: "",
    id: null,
  };
  const [formData, setFormData] = useState<formDataType>(defaultFormData);
  const [entitiesPageType, setEntitiesPageType] = useState<string>("");

  useEffect(() => {
    if (useAvailableEntitiesStore.getState().status == "none") {
      useAvailableEntitiesStore.getState().fetchEntities();
    }
    if (useSettingsStore.getState().status == "none") {
      useSettingsStore.getState().fetchData();
    }
  }, []);

  const nextStep = () => setStep((prev) => prev + 1);
  const dialogRef = useRef<HTMLDialogElement>(null);
  const contentRef = useRef<HTMLDivElement>(null);
  // const prevStep = () => setStep((prev) => prev - 1);

  const handleButtonSelectEvent = (input: string, value: string) => () => {
    setFormData({ ...formData, [input]: value });
  };

  useEffect(() => {
    if (id != null) {
      const entityData = useEntitiesPagesStore.getState().entities.find((entity) => entity.id == id);
      setFormData({ ...formData, ...entityData });
      setStep(3); // Entity already has all configuration. Simply show the final step.
    }
    const entityPage = useEntitiesPagesStore.getState().entities_pages.find((page) => page.id == entities_page_id);
    setEntitiesPageType(entityPage?.type ?? "");
  }, [id, useEntitiesPagesStore.getState().entities, useEntitiesPagesStore.getState().entities_pages]);

  // if (id != null && formData.id == null) {
  //   // Fetch existing entity type and populate base data.
  //   fetch(`/rest/entities/${id}`)
  //     .then((response) => response.json())
  //     .then((data) => {
  //       setFormData({ ...formData, ...data });
  //     });
  // }

  const onDialogClose = () => {
    // Clear form data and close dialog
    dialogRef.current?.close();

    setTimeout(() => {
      setOpened(false);
      if (onClose) onClose();
      if (id != null) {
        const entityData = useEntitiesPagesStore.getState().entities.find((entity) => entity.id == id);
        setFormData({ ...formData, ...entityData });
      }
      setStep(1);
    }, 220);
  };

  // Automatically switch between overflow-y-visible and overflow-y-auto depending on content height and max height
  const checkOverflow = () => {
    if (contentRef.current) {
      const maxHeightString = window.getComputedStyle(contentRef.current).maxHeight;
      let maxHeight = parseFloat(maxHeightString);
      if (maxHeightString.endsWith("%")) {
        // Convert percentage to pixels based on window height
        maxHeight = Math.trunc((window.innerHeight * maxHeight) / 100);
        contentRef.current.classList.toggle("overflow-y-auto", contentRef.current.scrollHeight > maxHeight);
        contentRef.current.classList.toggle("overflow-y-visible", contentRef.current.scrollHeight <= maxHeight);
      }
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
    <dialog ref={dialogRef} open={opened ?? false} onClose={onDialogClose} className="modal duration-200">
      <div ref={contentRef} className={`modal-box max-h-11/12 bg-base-100 text-base-content`}>
        {/* Upper right close button */}
        <button
          className="btn btn-sm btn-circle btn-ghost absolute right-2 top-2"
          onClick={() => {
            setOpened(false);
            onDialogClose();
          }}
        >
          ✕
        </button>

        {(() => {
          return (
            <div className="flex justify-center mb-4 w-full">
              <ul className="steps w-full">
                <li
                  className={`step ${step >= 1 ? "step-primary cursor-pointer" : ""}`}
                  onClick={() => {
                    setStep(1);
                  }}
                >
                  Type
                </li>
                <li
                  className={`step ${step >= 2 ? "step-primary" : ""} ${formData.entity_type == "" ? "cursor-not-allowed" : "cursor-pointer"}`}
                  onClick={() => {
                    if (formData.entity_type != "") {
                      setStep(2);
                    }
                  }}
                >
                  Controller
                </li>
                <li
                  className={`step ${step >= 3 ? "step-primary" : ""} ${formData.entity_type == "" ? "cursor-not-allowed" : "cursor-pointer"}`}
                  onClick={() => {
                    if (formData.entity_type != "") {
                      setStep(3);
                    }
                  }}
                >
                  Configure {formData.entity_type}
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
              if (id == null && step == 1 && entitiesPageType == "entity") {
                return <MultiStep_AddEditEntity_Step1 handleButtonSelectEvent={handleButtonSelectEvent} next_step={nextStep} />;
              } else if (id == null && step == 1 && entitiesPageType == "scene") {
                setFormData({ ...formData, entity_type: "scene" });
                setStep(2);
              }

              if (id == null && step == 2) {
                return (
                  <MultiStep_AddEditEntity_Step2
                    handleButtonSelectEvent={handleButtonSelectEvent}
                    entity_type={String(formData.entity_type)}
                    next_step={nextStep}
                  />
                );
              } else if (formData.entity_type != "") {
                switch (formData.entity_type) {
                  case "light":
                    return (
                      <MultiStep_AddEditEntity_Step3_Light
                        controller={String(formData.controller)}
                        room_id={room_id}
                        entities_page_id={entities_page_id}
                        room_view_position={room_view_position}
                        id={id}
                        onComplete={onComplete}
                      />
                    );
                  case "switch":
                    return (
                      <MultiStep_AddEditEntity_Step3_Switch
                        controller={String(formData.controller)}
                        room_id={room_id}
                        id={id}
                        entities_page_id={entities_page_id}
                        room_view_position={room_view_position}
                        onComplete={onComplete}
                      />
                    );
                  case "button":
                    return (
                      <MultiStep_AddEditEntity_Step3_Button
                        controller={String(formData.controller)}
                        room_id={room_id}
                        id={id}
                        entities_page_id={entities_page_id}
                        room_view_position={room_view_position}
                        onComplete={onComplete}
                      />
                    );
                  case "thermostat":
                    return (
                      <MultiStep_AddEditEntity_Step3_Thermostat
                        controller={String(formData.controller)}
                        room_id={room_id}
                        id={id}
                        entities_page_id={entities_page_id}
                        room_view_position={room_view_position}
                        onComplete={onComplete}
                      />
                    );
                  case "scene":
                    return (
                      <MultiStep_AddEditEntity_Step3_Scene
                        controller={String(formData.controller)}
                        room_id={room_id}
                        id={id}
                        entities_page_id={entities_page_id}
                        room_view_position={room_view_position}
                        onComplete={onComplete}
                      />
                    );
                  default:
                    console.error("Unknown entity type while trying to edit entity. Type: ", formData.entity_type);
                    return <span className="text-lg text-error">Unknown entity type: {formData.entity_type}</span>;
                }
              }
            })()}
          </motion.div>
        </AnimatePresence>
      </div>
      <form method="dialog" className="modal-backdrop">
        <button
          onClick={() => {
            setOpened(false);
            onDialogClose();
          }}
        >
          close
        </button>
      </form>
    </dialog>
  );
};

export default MultiStep_AddOrEditEntity;
