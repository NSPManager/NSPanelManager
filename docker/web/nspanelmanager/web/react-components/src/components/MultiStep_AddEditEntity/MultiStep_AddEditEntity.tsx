import { useState } from "react";
import MultiStep_AddEditEntity_Step1 from "./step1_select_type";
import MultiStep_AddEditEntity_Step2 from "./step2_select_source";
import MultiStep_AddEditEntity_Step3_Light from "./entity_types/step3_edit_light";
import MultiStep_AddEditEntity_Step3_Switch from "./entity_types/step3_edit_switch";
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
}: {
  room_id: number;
  entities_page_id: number;
  room_view_position: number;
  type: string;
  id?: number;
  onComplete?: () => void;
}) => {
  const [step, setStep] = useState(1);
  const [formData, setFormData] = useState<formDataType>({
    room_id: room_id,
    entities_page_id: entities_page_id,
    room_view_position: room_view_position,
    controller: "",
    type: type,
    entity_type: "",
    friendly_name: "",
    id: null,
  });

  const nextStep = () => setStep((prev) => prev + 1);
  // const prevStep = () => setStep((prev) => prev - 1);

  const handleChange = (input: string) => (e: React.ChangeEvent<HTMLInputElement>) => {
    setFormData({ ...formData, [input]: e.target.value });
    e.preventDefault();
  };

  const handleButtonSelectEvent = (input: string, value: string) => () => {
    setFormData({ ...formData, [input]: value });
  };

  if (id != null && formData.id == null) {
    // Fetch existing entity type and populate base data.
    fetch(`/rest/entities/${id}`)
      .then((response) => response.json())
      .then((data) => {
        setFormData({ ...formData, ...data.result.base });
      });
  }

  if (id == null && step == 1) {
    return <MultiStep_AddEditEntity_Step1 handleButtonSelectEvent={handleButtonSelectEvent} next_step={nextStep} />;
  } else if (id == null && step == 2) {
    return <MultiStep_AddEditEntity_Step2 handleButtonSelectEvent={handleButtonSelectEvent} next_step={nextStep} />;
  } else {
    switch (formData.entity_type) {
      case "light":
        return <MultiStep_AddEditEntity_Step3_Light handleChange={handleChange} values={formData} onComplete={onComplete} />;
      case "switch":
        return <MultiStep_AddEditEntity_Step3_Switch handleChange={handleChange} values={formData} onComplete={onComplete} />;
      default:
        console.log("Unknown entity type while trying to edit entity. Type: ", formData.entity_type);
        return null;
    }
  }
};

export default MultiStep_AddOrEditEntity;
