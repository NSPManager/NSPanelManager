import Select, { type OptionProps } from "react-select";
import { type ClassNamesConfig, type GroupBase } from "react-select";
import { useAvailableEntitiesStore } from "../../../stores/AvailableEntitiesStore";
import { type IOptionType } from "../../../stores/AvailableEntitiesStore";
import { useForm } from "react-hook-form";
import { zodResolver } from "@hookform/resolvers/zod";
import z from "zod";
import { useEntitiesPagesStore } from "../../../stores/EntitiesPagesStore";
import { useEffect } from "react";

const schema = z.object({
  id: z.number().nullable(),
  type: z.string(),
  entity_type: z.string(),
  room_id: z.number(),
  entities_page_id: z.number(),
  room_view_position: z.number(),
  friendly_name: z.string().min(1),
  controller: z.string(),
  home_assistant_name: z.string().optional(),
  openhab_item_switch: z.string().optional(),
});
export type SwitchFormData = z.infer<typeof schema>;

const CustomOption: React.FC<OptionProps<IOptionType>> = ({ innerProps, isDisabled, isFocused, isSelected, children, data }) => {
  if (isDisabled) {
    return null;
  }

  return (
    <div {...innerProps} className={`p-1 ${isSelected ? "bg-primary/20 rounded-sm" : ""} ${isFocused ? "bg-primary/20 rounded-sm" : ""}`}>
      <span className={`mdi ${data.icon} me-2`}></span>
      {children}
    </div>
  );
};

const select_components = {
  Option: CustomOption,
};

const MultiStep_AddEditEntity_Step3_Switch = ({
  controller,
  room_id,
  entities_page_id,
  room_view_position,
  id,
  onComplete,
}: {
  id?: number;
  controller: string;
  room_id: number;
  entities_page_id: number;
  room_view_position: number;
  onComplete?: () => void;
}) => {
  const {
    handleSubmit,
    register,
    reset,
    getValues,
    setValue,
    formState: { isValid },
  } = useForm<SwitchFormData>({
    resolver: zodResolver(schema),
    defaultValues: {
      id: id ?? null,
      controller: controller,
      type: "entity",
      entity_type: "light",
      room_id: room_id,
      entities_page_id: entities_page_id,
      room_view_position: room_view_position,
      openhab_item_switch: "",
      home_assistant_name: "",
    },
  });

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

  useEffect(() => {
    if (id != null) {
      const entityData = useEntitiesPagesStore.getState().entities.find((entity) => entity.id == id);
      reset(entityData);
    }
  }, [id, useEntitiesPagesStore.getState().entities]);

  function saveEntity(data: SwitchFormData) {
    // PUT request using fetch with error handling
    fetch("/rest/entities/switches", {
      credentials: "same-origin",
      method: "PUT",
      mode: "same-origin",
      headers: { "Content-Type": "application/json", "X-CSRFToken": getCookie("csrftoken") },
      body: JSON.stringify(data),
    })
      .then(async (response) => {
        const data = await response.json();
        console.log("Entity created/updated. Got response:", data);

        // check for error response
        if (!response.ok) {
          // get error message from body or default to response status
          const error = (data && data.message) || response.status;
          return Promise.reject(error);
        }
        onComplete?.();
      })
      .catch((error) => {
        // setErrorMessage(error);
        console.error("There was an error!", error);
      });
  }

  const classNames: ClassNamesConfig<{ value: string; label: string }, false, GroupBase<{ value: string; label: string }>> = {
    control: (state) => `${state.isFocused ? "border" : "border-0"} border-accent p-2.5 text-sm rounded-box bg-base-300 rounded-md`,
    menu: () => "bg-base-300 p-2.5 rounded-box",
    option: (state) => `p-1 ${state.isSelected ? "bg-primary/20 rounded-sm" : ""} ${state.isFocused ? "bg-primary/20 rounded-sm" : ""}`,
  };

  return (
    <form onSubmit={handleSubmit(saveEntity)}>
      <input type="hidden" {...register("id")} />
      <input type="hidden" {...register("controller")} />
      <input type="hidden" {...register("type")} />
      <input type="hidden" {...register("entity_type")} />
      <input type="hidden" {...register("room_id")} />
      <input type="hidden" {...register("entities_page_id")} />
      <input type="hidden" {...register("room_view_position")} />

      <div className="w-full">
        <div className="flex justify-center mb-4 duration-500 transition-transform ease-linear">
          {/* Name */}
          <div className="w-full">
            <label htmlFor="add_new_switch_name" className="block mb-2 text-sm font-medium">
              Name
            </label>
            <div className="flex flex-row-reverse">
              <input
                className="outline-none bg-base-300 border-neutral rounded-md border focus:ring-0 focus:border-accent block flex-1 min-w-0 w-full text-sm p-2.5 peer/search_text"
                type="text"
                id="add_new_switch_name"
                {...register("friendly_name")}
                required
              />
            </div>
          </div>
        </div>

        {controller == "home_assistant" && (
          <>
            <label className="block mb-2 text-sm font-medium">Home Assistant entity</label>
            <Select<IOptionType>
              options={useAvailableEntitiesStore
                .getState()
                .home_assistant_options.filter((option) => option.value.startsWith("switch.") || option.value.startsWith("input_boolean."))}
              classNames={classNames}
              onChange={(newValue) => {
                setValue("home_assistant_name", newValue ? newValue.value : "");
              }}
              unstyled
              components={select_components}
              value={useAvailableEntitiesStore.getState().home_assistant_options.find((option) => option.value === getValues("home_assistant_name"))}
              styles={{
                input: (base) => ({
                  ...base,
                  "input:focus": {
                    boxShadow: "none",
                  },
                }),
                // On mobile, the label will truncate automatically, so we want to
                // override that behaviour.
                multiValueLabel: (base) => ({
                  ...base,
                  whiteSpace: "normal",
                  overflow: "visible",
                }),
                control: (base) => ({
                  ...base,
                  transition: "none",
                }),
              }}
            />
          </>
        )}

        {controller == "openhab" && (
          <>
            <label className="block mb-2 mt-4 text-sm font-medium">OpenHAB item</label>
            <Select<IOptionType>
              options={useAvailableEntitiesStore.getState().openhab_options}
              classNames={classNames}
              onChange={(newValue) => setValue("openhab_item_switch", newValue ? newValue.value : "")}
              value={useAvailableEntitiesStore.getState().openhab_options.find((option) => option.value === getValues("openhab_item_switch"))}
              unstyled
              components={select_components}
              styles={{
                input: (base) => ({
                  ...base,
                  "input:focus": {
                    boxShadow: "none",
                  },
                }),
                // On mobile, the label will truncate automatically, so we want to
                // override that behaviour.
                multiValueLabel: (base) => ({
                  ...base,
                  whiteSpace: "normal",
                  overflow: "visible",
                }),
                control: (base) => ({
                  ...base,
                  transition: "none",
                }),
              }}
            />
          </>
        )}
      </div>

      <div className="flex mt-4 justify-end items-center">
        <button type="submit" className="btn btn-info" disabled={!isValid}>
          Save
        </button>
      </div>
    </form>
  );
};

export default MultiStep_AddEditEntity_Step3_Switch;
