import Select, { type OptionProps } from "react-select";
import { type ClassNamesConfig, type GroupBase } from "react-select";
import { useAvailableEntitiesStore } from "../../../stores/AvailableEntitiesStore";
import { type IOptionType } from "../../../stores/AvailableEntitiesStore";
import { useFieldArray, useForm } from "react-hook-form";
import { zodResolver } from "@hookform/resolvers/zod";
import z from "zod";
import { useEntitiesPagesStore } from "../../../stores/EntitiesPagesStore";
import { useEffect, useState } from "react";

const schema = z.object({
  id: z.number().nullable(),
  type: z.string(),
  entity_type: z.string(),
  room_id: z.number(),
  entities_page_id: z.number(),
  room_view_position: z.number(),
  friendly_name: z.string().min(1),
  step_size: z.float32(),
  controller: z.string(),
  home_assistant_name: z.string().optional(),
  openhab_fan_mode_item: z.string().optional(),
  openhab_hvac_mode_item: z.string().optional(),
  openhab_preset_mode_item: z.string().optional(),
  openhab_swing_mode_item: z.string().optional(),
  openhab_swingh_mode_item: z.string().optional(),
  openhab_temperature_item: z.string().optional(),
  fan_modes: z.array(
    z
      .object({
        icon: z.string(),
        label: z.string().min(1),
        value: z.string().min(1),
      })
      .optional(),
  ),
  hvac_modes: z.array(
    z
      .object({
        icon: z.string(),
        label: z.string().min(1),
        value: z.string().min(1),
      })
      .optional(),
  ),
  preset_modes: z
    .array(
      z.object({
        icon: z.string(),
        label: z.string().min(1),
        value: z.string().min(1),
      }),
    )
    .optional(),
  swing_modes: z
    .array(
      z.object({
        icon: z.string(),
        label: z.string().min(1),
        value: z.string().min(1),
      }),
    )
    .optional(),
  swingh_modes: z
    .array(
      z.object({
        icon: z.string(),
        label: z.string().min(1),
        value: z.string().min(1),
      }),
    )
    .optional(),
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

const IconSelector = ({ value, onChange }: { value: string; onChange: (icon: string) => void }) => {
  const climateIcons = [
    {
      name: "Off",
      icon: "h",
    },
    {
      name: "Heating",
      icon: "!",
    },
    {
      name: "Cooling",
      icon: "8",
    },
    {
      name: "Hot/Cold",
      icon: "#",
    },
    {
      name: "Thermostat",
      icon: "7",
    },
    {
      name: "Thermostat Auto",
      icon: "$",
    },
    {
      name: "Dry",
      icon: "%",
    },
    {
      name: "Eco",
      icon: "&",
    },
    {
      name: "Away",
      icon: "'",
    },
    {
      name: "Home",
      icon: "(",
    },
    {
      name: "Sleep",
      icon: ")",
    },
    {
      name: "Boost",
      icon: "*",
    },
    {
      name: "Comfort",
      icon: "+",
    },
    {
      name: "Activity",
      icon: ",",
    },
    {
      name: "Defrosting",
      icon: "-",
    },
    {
      name: "Swing Both",
      icon: ".",
    },
    {
      name: "Swing Horizontal",
      icon: "/",
    },
    {
      name: "Swing Vertical",
      icon: "0",
    },
    {
      name: "Fan Off",
      icon: "1",
    },
    {
      name: "Fan Auto",
      icon: "2",
    },
    {
      name: "Fan",
      icon: "3",
    },
    {
      name: "Fan 1",
      icon: "4",
    },
    {
      name: "Fan 2",
      icon: "5",
    },
    {
      name: "Fan 3",
      icon: "6",
    },
  ];
  const [selectedIcon, setSelectedIcon] = useState(value);
  const [detailsOpen, setDetailsOpen] = useState(false);

  const select_icon = (icon: string) => {
    setSelectedIcon(icon);
    onChange(icon);
    setDetailsOpen(false);
  };

  return (
    <details open={detailsOpen} onToggle={(e) => setDetailsOpen(e.currentTarget.open)} className="dropdown dropdown-end h-auto" id="dropdown_icon_selector">
      <summary id="icon_selector_button" title="Select icon" className="btn rounded-box h-full border-neutral border-y border-x-0 font-nspm-mdi">
        {selectedIcon}
      </summary>
      <div className="dropdown-content bg-base-100 text-base-content rounded-box rounded-tr-none w-56 border border-primary">
        <div className="flex justify-center pb-1 mb-2 mt-1 border-b border-primary">
          <span className="text-sm">Select Icon</span>
        </div>
        <div className="grid grid-cols-4 w-full">
          {climateIcons.map((icon) => (
            <button
              type="button"
              title={icon.name}
              className="btn btn-ghost rounded-none hover:btn-info icon-select-button"
              data-icon_value={icon.icon}
              onClick={() => {
                select_icon(icon.icon);
              }}
            >
              <span className="font-nspm-mdi">{icon.icon}</span>
            </button>
          ))}
        </div>
      </div>
    </details>
  );
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
      id: id ?? null,
      controller: controller,
      type: "entity",
      entity_type: "light",
      room_id: room_id,
      entities_page_id: entities_page_id,
      room_view_position: room_view_position,
      home_assistant_name: "",
      openhab_fan_mode_item: "",
      openhab_hvac_mode_item: "",
      openhab_preset_mode_item: "",
      openhab_swing_mode_item: "",
      openhab_swingh_mode_item: "",
      openhab_temperature_item: "",
      step_size: 0.5,
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
    fetch("/rest/entities/thermostats", {
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
      fan_modes.forEach((mode: any) => {
        appendFanModeField({ icon: "", label: mode, value: mode });
      });
    }

    if ("hvac_modes" in attributes) {
      const hvac_modes = attributes["hvac_modes"];
      hvac_modes.forEach((mode: any) => {
        appendHvacModeField({ icon: "", label: mode, value: mode });
      });
    }

    if ("preset_modes" in attributes) {
      const presets = attributes["preset_modes"];
      presets.forEach((mode: any) => {
        appendPresetModeField({ icon: "", label: mode, value: mode });
      });
    }

    if ("swing_modes" in attributes) {
      const swing_options = attributes["swing_modes"];
      swing_options.forEach((mode: any) => {
        appendSwingModeField({ icon: "", label: mode, value: mode });
      });
    }

    if ("swing_horizontal_modes" in attributes) {
      const swingh_options = attributes["swing_horizontal_modes"];
      swingh_options.forEach((mode: any) => {
        appendSwinghModeField({ icon: "", label: mode, value: mode });
      });
    }
  }

  const classNames: ClassNamesConfig<{ value: string; label: string }, false, GroupBase<{ value: string; label: string }>> = {
    control: (state) => `${state.isFocused ? "border" : "border-0"} border-accent p-2.5 text-sm rounded-box bg-base-300 text-base-content rounded-md`,
    menu: () => "bg-base-300 p-2.5 rounded-box text-base-content",
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
                className="outline-none bg-base-300 text-base-content border-neutral rounded-md border focus:ring-0 focus:border-accent block flex-1 min-w-0 w-full text-sm p-2.5 peer/search_text"
                type="text"
                id="add_new_switch_name"
                {...register("friendly_name")}
                required
              />
            </div>
          </div>
        </div>

        <div className="mb-4">
          <label className="block mb-2 text-sm font-medium">Step size</label>
          <div className="join w-full">
            <button
              type="button"
              onClick={() => {
                if (getValues("step_size") > 10) {
                  setValue("step_size", 10);
                } else if (getValues("step_size") > 0.1) {
                  setValue("step_size", Math.round(getValues("step_size") * 10 - 1) / 10);
                } else {
                  setValue("step_size", 0.1);
                }
              }}
              className="btn h-auto rounded-l-md border-neutral border border-r-0 "
            >
              -
            </button>
            <input
              type="number"
              className="join-item number-apperance-none outline-none bg-base-300 text-base-content border-neutral border focus:ring-0 focus:border-accent block min-w-0 w-full text-sm p-2.5"
              step="0.1"
              min="0.1"
              max="10"
              {...register("step_size")}
            />
            <button
              type="button"
              onClick={() => {
                if (getValues("step_size") < 0.1) {
                  setValue("step_size", 0.1);
                } else if (getValues("step_size") < 10) {
                  setValue("step_size", Math.round(getValues("step_size") * 10 + 1) / 10);
                } else {
                  setValue("step_size", 10);
                }
              }}
              className="btn h-auto rounded-r-md border-neutral border border-l-0"
            >
              +
            </button>
          </div>
        </div>

        {controller == "home_assistant" && (
          <div className="mb-4">
            <div>
              <label className="block mb-2 text-sm font-medium">Home Assistant entity</label>
              <Select<IOptionType>
                options={useAvailableEntitiesStore.getState().home_assistant_options.filter((option) => option.value.startsWith("climate."))}
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
          <div className="mt-4 border border-primary rounded-box p-4 pt-2">
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
        <div className="mt-4 border border-primary rounded-box p-4 pt-2">
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
            <div className="grid grid-cols-[1fr_1fr_min-content_min-content] items-stretch justify-center w-full mb-1 gap-1 mt-2">
              <span className="text-sm ml-2">Label</span>
              <span className="text-sm ml-2">Value</span>
              <span className="text-sm ml-2">Icon</span>
              <span className="text-sm ml-2"></span>
              {(() => {
                const elements = [];
                fanModeFields.forEach((field, index) => {
                  elements.push(
                    <input
                      type="text"
                      {...register(`fan_modes.${index}.label`)}
                      className="rounded-box outline-none bg-base-300 text-base-content border-neutral border focus:ring-0 focus:border-accent block flex-1 min-w-0 w-full text-sm p-2.5"
                      title="Label"
                    />,
                  );
                  elements.push(
                    <input
                      type="text"
                      {...register(`fan_modes.${index}.value`)}
                      className="rounded-box outline-none bg-base-300 text-base-content border-neutral border focus:ring-0 focus:border-accent block flex-1 min-w-0 w-full text-sm p-2.5"
                      title="Value"
                    />,
                  );
                  elements.push(<IconSelector value={field.icon} onChange={(icon) => setValue(`fan_modes.${index}.icon`, icon)} />);
                  elements.push(
                    <button
                      type="button"
                      className="btn h-auto rounded-box border border-neutral border-l-0 text-base-content bg-error/20 hover:bg-error/70 join-item ring-0 inset-ring-0 focus:ring-0"
                      title="Remove fan mode"
                      onClick={() => removeFanModeField(index)}
                    >
                      ✕
                    </button>,
                  );
                });

                return elements;
              })()}
            </div>
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
        <div className="mt-4 border border-primary rounded-box p-4 pt-2">
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
            <div className="grid grid-cols-[1fr_1fr_min-content_min-content] items-stretch justify-center w-full mb-1 gap-1 mt-2">
              <span className="text-sm ml-2">Label</span>
              <span className="text-sm ml-2">Value</span>
              <span className="text-sm ml-2">Icon</span>
              <span className="text-sm ml-2"></span>
              {(() => {
                const elements = [];
                hvacModeFields.forEach((field, index) => {
                  elements.push(
                    <input
                      type="text"
                      {...register(`hvac_modes.${index}.label`)}
                      className="rounded-box outline-none bg-base-300 text-base-content border-neutral border focus:ring-0 focus:border-accent block flex-1 min-w-0 w-full text-sm p-2.5"
                      title="Label"
                    />,
                  );
                  elements.push(
                    <input
                      type="text"
                      {...register(`hvac_modes.${index}.value`)}
                      className="rounded-box outline-none bg-base-300 text-base-content border-neutral border focus:ring-0 focus:border-accent block flex-1 min-w-0 w-full text-sm p-2.5"
                      title="Value"
                    />,
                  );
                  elements.push(<IconSelector value={field.icon} onChange={(icon) => setValue(`hvac_modes.${index}.icon`, icon)} />);
                  elements.push(
                    <button
                      type="button"
                      className="btn h-auto rounded-box border border-neutral border-l-0 text-base-content bg-error/20 hover:bg-error/70 join-item ring-0 inset-ring-0 focus:ring-0"
                      title="Remove HVAC mode"
                      onClick={() => removeHvacModeField(index)}
                    >
                      ✕
                    </button>,
                  );
                });

                return elements;
              })()}
            </div>
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
        <div className="mt-4 border border-primary rounded-box p-4 pt-2">
          <span className="input_field_label block mb-2 text-sm font-medium">Presets</span>
          {controller == "openhab" && (
            <div className="mb-4">
              <label className="block mb-2 text-sm font-medium">OpenHAB preset item</label>
              <Select<IOptionType>
                options={useAvailableEntitiesStore.getState().openhab_options}
                classNames={classNames}
                onChange={(newValue) => setValue("openhab_preset_mode_item", newValue ? newValue.value : "")}
                value={useAvailableEntitiesStore.getState().openhab_options.find((option) => option.value === getValues("openhab_preset_mode_item"))}
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
            <div className="grid grid-cols-[1fr_1fr_min-content_min-content] items-stretch justify-center w-full mb-1 gap-1 mt-2">
              <span className="text-sm ml-2">Label</span>
              <span className="text-sm ml-2">Value</span>
              <span className="text-sm ml-2">Icon</span>
              <span className="text-sm ml-2"></span>
              {(() => {
                const elements = [];
                presetModeFields.forEach((field, index) => {
                  elements.push(
                    <input
                      type="text"
                      {...register(`preset_modes.${index}.label`)}
                      className="rounded-box outline-none bg-base-300 text-base-content border-neutral border focus:ring-0 focus:border-accent block flex-1 min-w-0 w-full text-sm p-2.5"
                      title="Label"
                    />,
                  );
                  elements.push(
                    <input
                      type="text"
                      {...register(`preset_modes.${index}.value`)}
                      className="rounded-box outline-none bg-base-300 text-base-content border-neutral border focus:ring-0 focus:border-accent block flex-1 min-w-0 w-full text-sm p-2.5"
                      title="Value"
                    />,
                  );
                  elements.push(<IconSelector value={field.icon} onChange={(icon) => setValue(`preset_modes.${index}.icon`, icon)} />);
                  elements.push(
                    <button
                      type="button"
                      className="btn h-auto rounded-box border border-neutral border-l-0 text-base-content bg-error/20 hover:bg-error/70 join-item ring-0 inset-ring-0 focus:ring-0"
                      title="Remove preset mode"
                      onClick={() => removePresetModeField(index)}
                    >
                      ✕
                    </button>,
                  );
                });

                return elements;
              })()}
            </div>
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
        <div className="mt-4 border border-primary rounded-box p-4 pt-2">
          <span className="input_field_label block mb-2 text-sm font-medium">Swing modes</span>
          {controller == "openhab" && (
            <div className="mb-4">
              <label className="block mb-2 text-sm font-medium">OpenHAB swing item</label>
              <Select<IOptionType>
                options={useAvailableEntitiesStore.getState().openhab_options}
                classNames={classNames}
                onChange={(newValue) => setValue("openhab_swing_mode_item", newValue ? newValue.value : "")}
                value={useAvailableEntitiesStore.getState().openhab_options.find((option) => option.value === getValues("openhab_swing_mode_item"))}
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
            <div className="grid grid-cols-[1fr_1fr_min-content_min-content] items-stretch justify-center w-full mb-1 gap-1 mt-2">
              <span className="text-sm ml-2">Label</span>
              <span className="text-sm ml-2">Value</span>
              <span className="text-sm ml-2">Icon</span>
              <span className="text-sm ml-2"></span>
              {(() => {
                const elements = [];
                swingModeFields.forEach((field, index) => {
                  elements.push(
                    <input
                      type="text"
                      {...register(`swing_modes.${index}.label`)}
                      className="rounded-box outline-none bg-base-300 text-base-content border-neutral border focus:ring-0 focus:border-accent block flex-1 min-w-0 w-full text-sm p-2.5"
                      title="Label"
                    />,
                  );
                  elements.push(
                    <input
                      type="text"
                      {...register(`swing_modes.${index}.value`)}
                      className="rounded-box outline-none bg-base-300 text-base-content border-neutral border focus:ring-0 focus:border-accent block flex-1 min-w-0 w-full text-sm p-2.5"
                      title="Value"
                    />,
                  );
                  elements.push(<IconSelector value={field.icon} onChange={(icon) => setValue(`swing_modes.${index}.icon`, icon)} />);
                  elements.push(
                    <button
                      type="button"
                      className="btn h-auto rounded-box border border-neutral border-l-0 text-base-content bg-error/20 hover:bg-error/70 join-item ring-0 inset-ring-0 focus:ring-0"
                      title="Remove swing mode"
                      onClick={() => removeSwingModeField(index)}
                    >
                      ✕
                    </button>,
                  );
                });

                return elements;
              })()}
            </div>

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
        <div className="mt-4 border border-primary rounded-box p-4 pt-2">
          <span className="input_field_label block mb-2 text-sm font-medium">Horizontal swing modes</span>
          {controller == "openhab" && (
            <div className="mb-4">
              <label className="block mb-2 text-sm font-medium">OpenHAB horizontal swing item</label>
              <Select<IOptionType>
                options={useAvailableEntitiesStore.getState().openhab_options}
                classNames={classNames}
                onChange={(newValue) => setValue("openhab_swingh_mode_item", newValue ? newValue.value : "")}
                value={useAvailableEntitiesStore.getState().openhab_options.find((option) => option.value === getValues("openhab_swingh_mode_item"))}
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
            <div className="grid grid-cols-[1fr_1fr_min-content_min-content] items-stretch justify-center w-full mb-1 gap-1 mt-2">
              <span className="text-sm ml-2">Label</span>
              <span className="text-sm ml-2">Value</span>
              <span className="text-sm ml-2">Icon</span>
              <span className="text-sm ml-2"></span>
              {(() => {
                const elements = [];
                swinghModeFields.forEach((field, index) => {
                  elements.push(
                    <input
                      type="text"
                      {...register(`swingh_modes.${index}.label`)}
                      className="rounded-box outline-none bg-base-300 text-base-content border-neutral border focus:ring-0 focus:border-accent block flex-1 min-w-0 w-full text-sm p-2.5"
                      title="Label"
                    />,
                  );
                  elements.push(
                    <input
                      type="text"
                      {...register(`swingh_modes.${index}.value`)}
                      className="rounded-box outline-none bg-base-300 text-base-content border-neutral border focus:ring-0 focus:border-accent block flex-1 min-w-0 w-full text-sm p-2.5"
                      title="Value"
                    />,
                  );
                  elements.push(<IconSelector value={field.icon} onChange={(icon) => setValue(`swingh_modes.${index}.icon`, icon)} />);
                  elements.push(
                    <button
                      type="button"
                      className="btn h-auto rounded-box border border-neutral border-l-0 text-base-content bg-error/20 hover:bg-error/70 join-item ring-0 inset-ring-0 focus:ring-0"
                      title="Remove horizontal swing mode"
                      onClick={() => removeSwinghModeField(index)}
                    >
                      ✕
                    </button>,
                  );
                });

                return elements;
              })()}
            </div>
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
