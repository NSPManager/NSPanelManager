import Select, { type OptionProps } from "react-select";
import { type ClassNamesConfig, type GroupBase } from "react-select";
import { useAvailableEntitiesStore } from "../../AvailableEntitiesStore";
import { type IOptionType } from "../../AvailableEntitiesStore";
import { useFieldArray, useForm } from "react-hook-form";
import { zodResolver } from "@hookform/resolvers/zod";
import z from "zod";
import { useEntitiesPagesStore } from "../../EntitiesPage/EntitiesPagesStore";
import { useEffect } from "react";

const schema = z.object({
  type: z.string(),
  entity_type: z.string(),
  room_id: z.number(),
  entities_page_id: z.number(),
  room_view_position: z.number(),
  friendly_name: z.string(),
  controller: z.string(),
  home_assistant_name: z.string().optional(),
  openhab_fan_mode_item: z.string().optional(),
  openhab_hvac_mode_item: z.string().optional(),
  openhab_preset_item: z.string().optional(),
  openhab_swing_item: z.string().optional(),
  openhab_swingh_item: z.string().optional(),
  openhab_temperature_item: z.string().optional(),
  fan_modes: z.array(
    z.object({
      icon: z.string(),
      label: z.string(),
      value: z.string(),
    }),
  ),
  hvac_modes: z.array(
    z.object({
      icon: z.string(),
      label: z.string(),
      value: z.string(),
    }),
  ),
  preset_modes: z.array(
    z.object({
      icon: z.string(),
      label: z.string(),
      value: z.string(),
    }),
  ),
  swing_modes: z.array(
    z.object({
      icon: z.string(),
      label: z.string(),
      value: z.string(),
    }),
  ),
  swingh_modes: z.array(
    z.object({
      icon: z.string(),
      label: z.string(),
      value: z.string(),
    }),
  ),
});
export type ThermostatFormData = z.infer<typeof schema>;

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

const MultiStep_AddEditEntity_Step3_Thermostat = ({
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
    control,
    handleSubmit,
    register,
    reset,
    getValues,
    setValue,
    formState: { isValid },
  } = useForm<ThermostatFormData>({
    resolver: zodResolver(schema),
    defaultValues: {
      controller: controller,
      type: "entity",
      entity_type: "light",
      room_id: room_id,
      entities_page_id: entities_page_id,
      room_view_position: room_view_position,
      home_assistant_name: "",
      openhab_fan_mode_item: "",
      openhab_hvac_mode_item: "",
      openhab_preset_item: "",
      openhab_swing_item: "",
      openhab_swingh_item: "",
      openhab_temperature_item: "",
      fan_modes: [],
      hvac_modes: [],
      preset_modes: [],
      swing_modes: [],
      swingh_modes: [],
    },
  });

  const {
    fields: fanModeFields,
    append: appendFanModeField,
    remove: removeFanModeField,
  } = useFieldArray({
    control,
    name: "fan_modes",
  });

  const {
    fields: hvacModeFields,
    append: appendHvacModeField,
    remove: removeHvacModeField,
  } = useFieldArray({
    control,
    name: "hvac_modes",
  });

  const {
    fields: presetModeFields,
    append: appendPresetModeField,
    remove: removePresetModeField,
  } = useFieldArray({
    control,
    name: "preset_modes",
  });

  const {
    fields: swingModeFields,
    append: appendSwingModeField,
    remove: removeSwingModeField,
  } = useFieldArray({
    control,
    name: "swing_modes",
  });

  const {
    fields: swinghModeFields,
    append: appendSwinghModeField,
    remove: removeSwinghModeField,
  } = useFieldArray({
    control,
    name: "swingh_modes",
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

  function saveEntity(data: ThermostatFormData) {
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

  function loadHaEntitySettings() {
    const selected_entity = useAvailableEntitiesStore.getState().home_assistant_entities.find((entity) => entity.item_id == getValues("home_assistant_name"));
    if (!selected_entity) return;

    // Remove all fields and reset to default values
    for (let i = fanModeFields.length - 1; i >= 0; i--) {
      removeFanModeField(i);
    }
    for (let i = hvacModeFields.length - 1; i >= 0; i--) {
      removeHvacModeField(i);
    }
    for (let i = presetModeFields.length - 1; i >= 0; i--) {
      removePresetModeField(i);
    }
    for (let i = swingModeFields.length - 1; i >= 0; i--) {
      removeSwingModeField(i);
    }
    for (let i = swinghModeFields.length - 1; i >= 0; i--) {
      removeSwinghModeField(i);
    }

    const attributes = selected_entity.item.attributes;

    if ("fan_modes" in attributes) {
      const fan_modes = attributes["fan_modes"];
      fan_modes.forEach((mode) => {
        appendFanModeField({ icon: "", label: mode, value: mode });
      });
    }

    if ("hvac_modes" in attributes) {
      const hvac_modes = attributes["hvac_modes"];
      hvac_modes.forEach((mode) => {
        appendHvacModeField({ icon: "", label: mode, value: mode });
      });
    }

    if ("preset_modes" in attributes) {
      const presets = attributes["preset_modes"];
      presets.forEach((mode) => {
        appendPresetModeField({ icon: "", label: mode, value: mode });
      });
    }

    if ("swing_modes" in attributes) {
      const swing_options = attributes["swing_modes"];
      swing_options.forEach((mode) => {
        appendSwingModeField({ icon: "", label: mode, value: mode });
      });
    }

    if ("swing_horizontal_modes" in attributes) {
      const swingh_options = attributes["swing_horizontal_modes"];
      swingh_options.forEach((mode) => {
        appendSwinghModeField({ icon: "", label: mode, value: mode });
      });
    }
  }

  const classNames: ClassNamesConfig<{ value: string; label: string }, false, GroupBase<{ value: string; label: string }>> = {
    control: (state) => `${state.isFocused ? "border" : "border-0"} border-accent p-2.5 text-sm rounded-box bg-base-300 rounded-md`,
    menu: () => "bg-base-300 p-2.5 rounded-box",
    option: (state) => `p-1 ${state.isSelected ? "bg-primary/20 rounded-sm" : ""} ${state.isFocused ? "bg-primary/20 rounded-sm" : ""}`,
  };

  return (
    <form onSubmit={handleSubmit(saveEntity)}>
      <input type="hidden" {...register("controller")} />
      <input type="hidden" {...register("type")} />
      <input type="hidden" {...register("entity_type")} />
      <input type="hidden" {...register("room_id")} />
      <input type="hidden" {...register("entities_page_id")} />
      <input type="hidden" {...register("room_view_position")} />

      <div className="flex justify-center mb-4 duration-500 transition-transform ease-linear w-full">
        <ul className="steps">
          <li className="step step-primary">Type</li>
          <li className="step step-primary">Controller</li>
          <li className="step step-primary">Thermostat</li>
        </ul>
      </div>

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
          <div className="mb-4">
            <div>
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
            </div>
            <div className="mt-1">
              <button type="button" onClick={loadHaEntitySettings} className="btn btn-xs w-full btn-info">
                Load settings from HA entity
              </button>
            </div>
          </div>
        )}

        {/* Temperature */}
        {controller == "openhab" && (
          <div className="mt-4 border-1 border-primary rounded-box p-4 pt-2">
            <span className="text-lg mb-4">Temperature</span>
            <div className="">
              <label className="block mb-2 text-sm font-medium">OpenHAB target temperature item</label>
              <Select<IOptionType>
                options={useAvailableEntitiesStore.getState().openhab_options}
                classNames={classNames}
                onChange={(newValue) => setValue("openhab_temperature_item", newValue ? newValue.value : "")}
                value={useAvailableEntitiesStore.getState().openhab_options.find((option) => option.value === getValues("openhab_temperature_item"))}
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
            </div>
          </div>
        )}

        {/* Fan modes */}
        <div className="mt-4 border-1 border-primary rounded-box p-4 pt-2">
          <span className="text-lg mb-4">Fan modes</span>
          {controller == "openhab" && (
            <div className="mb-4">
              <label className="block mb-2 text-sm font-medium">OpenHAB fan mode item</label>
              <Select<IOptionType>
                options={useAvailableEntitiesStore.getState().openhab_options}
                classNames={classNames}
                onChange={(newValue) => setValue("openhab_fan_mode_item", newValue ? newValue.value : "")}
                value={useAvailableEntitiesStore.getState().openhab_options.find((option) => option.value === getValues("openhab_fan_mode_item"))}
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
            </div>
          )}
          <div id={`fan_mode_options`}>
            {fanModeFields.map((field, index) => (
              <div key={field.id} className="flex items-center justify-center w-full mb-1">
                <input
                  type="text"
                  {...register(`fan_modes.${index}.label`)}
                  className="rounded-l-box outline-none bg-base-300 border-neutral border focus:ring-0 focus:border-accent block flex-1 min-w-0 w-full text-sm p-2.5"
                />
                <input
                  type="text"
                  {...register(`fan_modes.${index}.value`)}
                  className="rounded-r-box outline-none bg-base-300 border-neutral border focus:ring-0 focus:border-accent block flex-1 min-w-0 w-full text-sm p-2.5"
                />
                <div className="flex justify-center items-center pl-1 py-2.5 h-full">
                  <button
                    type="button"
                    className="btn btn-xs btn-circle btn-error join-item ring-0 inset-ring-0 focus:ring-0"
                    title="Remove fan mode"
                    onClick={() => removeFanModeField(index)}
                  >
                    ✕
                  </button>
                </div>
              </div>
            ))}
            <button
              type="button"
              className="btn btn-xs w-full btn-success"
              title="Add fan mode"
              onClick={() => appendFanModeField({ icon: "", label: "", value: "" })}
            >
              Add fan mode
            </button>
          </div>
        </div>

        {/* HVAC modes */}
        <div className="mt-4 border-1 border-primary rounded-box p-4 pt-2">
          <span className="input_field_label block mb-2 text-sm font-medium">HVAC modes</span>
          {controller == "openhab" && (
            <div className="mb-4">
              <label className="block mb-2 text-sm font-medium">OpenHAB HVAC mode item</label>
              <Select<IOptionType>
                options={useAvailableEntitiesStore.getState().openhab_options}
                classNames={classNames}
                onChange={(newValue) => setValue("openhab_hvac_mode_item", newValue ? newValue.value : "")}
                value={useAvailableEntitiesStore.getState().openhab_options.find((option) => option.value === getValues("openhab_hvac_mode_item"))}
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
            </div>
          )}
          <div id={`hvac_mode_options`}>
            {hvacModeFields.map((field, index) => (
              <div key={field.id} className="flex items-center justify-center w-full mb-1">
                <input
                  type="text"
                  {...register(`hvac_modes.${index}.label`)}
                  className="rounded-l-box outline-none bg-base-300 border-neutral border focus:ring-0 focus:border-accent block flex-1 min-w-0 w-full text-sm p-2.5"
                />
                <input
                  type="text"
                  {...register(`hvac_modes.${index}.value`)}
                  className="rounded-r-box outline-none bg-base-300 border-neutral border focus:ring-0 focus:border-accent block flex-1 min-w-0 w-full text-sm p-2.5"
                />
                <div className="flex justify-center items-center pl-1 py-2.5 h-full">
                  <button
                    type="button"
                    className="btn btn-xs btn-circle btn-error join-item ring-0 inset-ring-0 focus:ring-0"
                    title="Remove HVAC mode"
                    onClick={() => removeHvacModeField(index)}
                  >
                    ✕
                  </button>
                </div>
              </div>
            ))}
            <button
              type="button"
              className="btn btn-xs w-full btn-success"
              title="Add HVAC mode"
              onClick={() => appendHvacModeField({ icon: "", label: "", value: "" })}
            >
              Add HVAC mode
            </button>
          </div>
        </div>

        {/* Preset modes */}
        <div className="mt-4 border-1 border-primary rounded-box p-4 pt-2">
          <span className="input_field_label block mb-2 text-sm font-medium">Presets</span>
          {controller == "openhab" && (
            <div className="mb-4">
              <label className="block mb-2 text-sm font-medium">OpenHAB preset item</label>
              <Select<IOptionType>
                options={useAvailableEntitiesStore.getState().openhab_options}
                classNames={classNames}
                onChange={(newValue) => setValue("openhab_preset_item", newValue ? newValue.value : "")}
                value={useAvailableEntitiesStore.getState().openhab_options.find((option) => option.value === getValues("openhab_preset_item"))}
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
            </div>
          )}
          <div id={`preset_mode_options`}>
            {presetModeFields.map((field, index) => (
              <div key={field.id} className="flex items-center justify-center w-full mb-1">
                <input
                  type="text"
                  {...register(`preset_modes.${index}.label`)}
                  className="rounded-l-box outline-none bg-base-300 border-neutral border focus:ring-0 focus:border-accent block flex-1 min-w-0 w-full text-sm p-2.5"
                />
                <input
                  type="text"
                  {...register(`preset_modes.${index}.value`)}
                  className="rounded-r-box outline-none bg-base-300 border-neutral border focus:ring-0 focus:border-accent block flex-1 min-w-0 w-full text-sm p-2.5"
                />
                <div className="flex justify-center items-center pl-1 py-2.5 h-full">
                  <button
                    type="button"
                    className="btn btn-xs btn-circle btn-error join-item ring-0 inset-ring-0 focus:ring-0"
                    title="Remove preset mode"
                    onClick={() => removePresetModeField(index)}
                  >
                    ✕
                  </button>
                </div>
              </div>
            ))}
            <button
              type="button"
              className="btn btn-xs w-full btn-success"
              title="Add preset mode"
              onClick={() => appendPresetModeField({ icon: "", label: "", value: "" })}
            >
              Add preset
            </button>
          </div>
        </div>

        {/* Swing modes */}
        <div className="mt-4 border-1 border-primary rounded-box p-4 pt-2">
          <span className="input_field_label block mb-2 text-sm font-medium">Swing modes</span>
          {controller == "openhab" && (
            <div className="mb-4">
              <label className="block mb-2 text-sm font-medium">OpenHAB swing item</label>
              <Select<IOptionType>
                options={useAvailableEntitiesStore.getState().openhab_options}
                classNames={classNames}
                onChange={(newValue) => setValue("openhab_swing_item", newValue ? newValue.value : "")}
                value={useAvailableEntitiesStore.getState().openhab_options.find((option) => option.value === getValues("openhab_swing_item"))}
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
            </div>
          )}
          <div id={`swing_mode_options`}>
            {swingModeFields.map((field, index) => (
              <div key={field.id} className="flex items-center justify-center w-full mb-1">
                <input
                  type="text"
                  {...register(`swing_modes.${index}.label`)}
                  className="rounded-l-box outline-none bg-base-300 border-neutral border focus:ring-0 focus:border-accent block flex-1 min-w-0 w-full text-sm p-2.5"
                />
                <input
                  type="text"
                  {...register(`swing_modes.${index}.value`)}
                  className="rounded-r-box outline-none bg-base-300 border-neutral border focus:ring-0 focus:border-accent block flex-1 min-w-0 w-full text-sm p-2.5"
                />
                <div className="flex justify-center items-center pl-1 py-2.5 h-full">
                  <button
                    type="button"
                    className="btn btn-xs btn-circle btn-error join-item ring-0 inset-ring-0 focus:ring-0"
                    title="Remove swing mode"
                    onClick={() => removeSwingModeField(index)}
                  >
                    ✕
                  </button>
                </div>
              </div>
            ))}
            <button
              type="button"
              className="btn btn-xs w-full btn-success"
              title="Add swing mode"
              onClick={() => appendSwingModeField({ icon: "", label: "", value: "" })}
            >
              Add swing mode
            </button>
          </div>
        </div>

        {/* Swing Horizontal modes */}
        <div className="mt-4 border-1 border-primary rounded-box p-4 pt-2">
          <span className="input_field_label block mb-2 text-sm font-medium">Horizontal swing modes</span>
          {controller == "openhab" && (
            <div className="mb-4">
              <label className="block mb-2 text-sm font-medium">OpenHAB horizontal swing item</label>
              <Select<IOptionType>
                options={useAvailableEntitiesStore.getState().openhab_options}
                classNames={classNames}
                onChange={(newValue) => setValue("openhab_swingh_item", newValue ? newValue.value : "")}
                value={useAvailableEntitiesStore.getState().openhab_options.find((option) => option.value === getValues("openhab_swingh_item"))}
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
            </div>
          )}
          <div id={`swing_mode_options`}>
            {swinghModeFields.map((field, index) => (
              <div key={field.id} className="flex items-center justify-center w-full mb-1">
                <input
                  type="text"
                  {...register(`swingh_modes.${index}.label`)}
                  className="rounded-l-box outline-none bg-base-300 border-neutral border focus:ring-0 focus:border-accent block flex-1 min-w-0 w-full text-sm p-2.5"
                />
                <input
                  type="text"
                  {...register(`swingh_modes.${index}.value`)}
                  className="rounded-r-box outline-none bg-base-300 border-neutral border focus:ring-0 focus:border-accent block flex-1 min-w-0 w-full text-sm p-2.5"
                />
                <div className="flex justify-center items-center pl-1 py-2.5 h-full">
                  <button
                    type="button"
                    className="btn btn-xs btn-circle btn-error join-item ring-0 inset-ring-0 focus:ring-0"
                    title="Remove swing mode"
                    onClick={() => removeSwinghModeField(index)}
                  >
                    ✕
                  </button>
                </div>
              </div>
            ))}
            <button
              type="button"
              className="btn btn-xs w-full btn-success"
              title="Add horizontal swing mode"
              onClick={() => appendSwinghModeField({ icon: "", label: "", value: "" })}
            >
              Add horizontal swing mode
            </button>
          </div>
        </div>
      </div>

      <div className="flex mt-4 justify-end items-center">
        <button type="submit" className="btn btn-info" disabled={!isValid}>
          Save
        </button>
      </div>
    </form>
  );
};

export default MultiStep_AddEditEntity_Step3_Thermostat;
