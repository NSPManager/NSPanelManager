import Select, { type OptionProps } from "react-select";
import { type ClassNamesConfig, type GroupBase } from "react-select";
import { useAvailableEntitiesStore } from "../../../stores/AvailableEntitiesStore";
import { type IOptionType } from "../../../stores/AvailableEntitiesStore";
import { useForm } from "react-hook-form";
import { zodResolver } from "@hookform/resolvers/zod";
import z from "zod";
import { useEntitiesPagesStore } from "../../../stores/EntitiesPagesStore";
import { useEffect } from "react";
import { Notify } from "../../NSPanelToastContainer";

const schema = z
  .object({
    id: z.number().nullable(),
    type: z.string(),
    entity_type: z.string(),
    room_id: z.number(),
    entities_page_id: z.number(),
    room_view_position: z.number(),
    friendly_name: z.string().min(1),
    controller: z.string(),
    home_assistant_name: z.string(),
    openhab_item: z.string(),
    openhab_tilt_item: z.string(),
    invert_positions: z.boolean().optional().nullable(),

    position_preset1: z.number(),
    position_preset2: z.number(),
    position_preset3: z.number(),
    tilt_preset1: z.number(),
    tilt_preset2: z.number(),
    tilt_preset3: z.number(),
  })
  .superRefine((data, ctx) => {
    if (data.controller == "home_assistant") {
      if (!data.home_assistant_name.startsWith("cover.")) {
        ctx.addIssue({ code: "custom", path: ["home_assistant_name"], message: "Select a Home Assistant cover entity" });
      }
    } else if (data.controller == "openhab") {
      if (data.openhab_item.trim() == "") {
        ctx.addIssue({ code: "custom", path: ["openhab_item"], message: "Select an OpenHAB Rollershutter item" });
      }
    }
  });
export type CoverFormData = z.infer<typeof schema>;

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

const selectStyles = {
  input: (base: object) => ({
    ...base,
    "input:focus": {
      boxShadow: "none",
    },
  }),
  multiValueLabel: (base: object) => ({
    ...base,
    whiteSpace: "normal" as const,
    overflow: "visible" as const,
  }),
  control: (base: object) => ({
    ...base,
    transition: "none",
  }),
};

const MultiStep_AddEditEntity_Step3_Cover = ({
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
  const defaultValues: CoverFormData = {
    id: id ?? null,
    controller: controller,
    type: "entity",
    entity_type: "cover",
    friendly_name: "",
    room_id: room_id,
    entities_page_id: entities_page_id,
    room_view_position: room_view_position,
    home_assistant_name: "",
    openhab_item: "",
    openhab_tilt_item: "",
    invert_positions: false,
    position_preset1: 0,
    position_preset2: 0,
    position_preset3: 0,
    tilt_preset1: 0,
    tilt_preset2: 0,
    tilt_preset3: 0,
  };

  const {
    handleSubmit,
    register,
    reset,
    setValue,
    watch,
    formState: { isValid, errors },
  } = useForm<CoverFormData>({
    resolver: zodResolver(schema),
    mode: "onChange",
    defaultValues: defaultValues,
  });

  const home_assistant_name = watch("home_assistant_name");
  const openhab_item = watch("openhab_item");
  const openhab_tilt_item = watch("openhab_tilt_item");
  const invert_positions = watch("invert_positions");
  watch("position_preset1");
  watch("position_preset2");
  watch("position_preset3");
  watch("tilt_preset1");
  watch("tilt_preset2");
  watch("tilt_preset3");

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

  const entity = useEntitiesPagesStore((s) => s.entities.find((e) => e.id == id));
  useEffect(() => {
    if (!entity) return;
    reset({ ...defaultValues, ...entity });
  }, [entity?.id]);

  function saveEntity(data: CoverFormData) {
    fetch("/rest/entities/covers", {
      credentials: "same-origin",
      method: "PUT",
      mode: "same-origin",
      headers: { "Content-Type": "application/json", "X-CSRFToken": getCookie("csrftoken") },
      body: JSON.stringify(data),
    })
      .then(async (response) => {
        const data = await response.json();

        if (!response.ok) {
          const error = (data && data.message) || response.status;
          return Promise.reject(error);
        }
        onComplete?.();
      })
      .catch((error) => {
        Notify({ message: `Error saving entity. Error: ${error}`, level: "error", duration: 10000 });
      });
  }

  const ha_entity = useAvailableEntitiesStore.getState().home_assistant_entities.find((entity) => entity.item_id == home_assistant_name);
  const supported_features = Number(ha_entity?.item?.attributes?.supported_features ?? 0);
  const supports_position = (supported_features & 4) !== 0;
  const supports_tilt = (supported_features & 128) !== 0;

  const openhab_cover_options = useAvailableEntitiesStore
    .getState()
    .openhab_items.filter((item) => item.item?.type === "Rollershutter")
    .map((item) => ({ value: item.item_id, label: item.label, icon: "mdi-window-shutter", type: item.type }));

  const classNames: ClassNamesConfig<IOptionType, false, GroupBase<IOptionType>> = {
    control: (state: { isFocused: boolean }) => `${state.isFocused ? "border" : "border-0"} border-accent p-2.5 text-sm rounded-box bg-base-300 rounded-md`,
    menu: () => "bg-base-300 p-2.5 rounded-box",
    option: (state: { isSelected: boolean; isFocused: boolean }) =>
      `p-1 ${state.isSelected ? "bg-primary/20 rounded-sm" : ""} ${state.isFocused ? "bg-primary/20 rounded-sm" : ""}`,
  };

  const text_input_class =
    "outline-none bg-base-300 border-neutral first:rounded-l-md last:rounded-r-md border focus:ring-0 focus:border-accent block flex-1 min-w-0 w-full text-sm p-2.5";

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
          <div className="w-full">
            <label htmlFor="add_new_cover_name" className="block mb-2 text-sm font-medium">
              Name
            </label>
            <div className="flex flex-row-reverse">
              <input className={text_input_class} type="text" id="add_new_cover_name" {...register("friendly_name")} required />
            </div>
          </div>
        </div>

        {controller == "home_assistant" && (
          <>
            <label className="block mb-2 text-sm font-medium">Home Assistant entity</label>
            <Select<IOptionType>
              options={useAvailableEntitiesStore.getState().home_assistant_options.filter((option) => option.value.startsWith("cover."))}
              classNames={classNames}
              onChange={(newValue) => {
                setValue("home_assistant_name", newValue ? newValue.value : "", { shouldValidate: true });
              }}
              unstyled
              components={select_components}
              value={useAvailableEntitiesStore.getState().home_assistant_options.find((option) => option.value === home_assistant_name) ?? null}
              styles={selectStyles}
            />
            {ha_entity && (
              <span className="block mt-2 text-xs text-base-content/50">
                Detected: {supports_position ? "position" : "open/close only"}
                {supports_tilt ? ", tilt" : ""}
              </span>
            )}
            {errors.home_assistant_name && <span className="block mt-1 text-xs text-error">Select a Home Assistant cover entity.</span>}
          </>
        )}

        {controller == "openhab" && (
          <>
            <label className="block mb-2 text-sm font-medium">OpenHAB Rollershutter item</label>
            <Select<IOptionType>
              options={openhab_cover_options}
              classNames={classNames}
              onChange={(newValue) => setValue("openhab_item", newValue ? newValue.value : "", { shouldValidate: true })}
              unstyled
              components={select_components}
              value={openhab_cover_options.find((option) => option.value === openhab_item) ?? null}
              styles={selectStyles}
            />
            {errors.openhab_item && <span className="block mt-1 text-xs text-error">Select an OpenHAB Rollershutter item.</span>}

            <label className="block mb-2 text-sm font-medium">OpenHAB Tilt item</label>
            <Select<IOptionType>
              options={openhab_cover_options}
              classNames={classNames}
              onChange={(newValue) => setValue("openhab_tilt_item", newValue ? newValue.value : "", { shouldValidate: true })}
              unstyled
              components={select_components}
              value={openhab_cover_options.find((option) => option.value === openhab_tilt_item) ?? null}
              styles={selectStyles}
            />
            {errors.openhab_tilt_item && <span className="block mt-1 text-xs text-error">Select an OpenHAB Rollershutter item.</span>}
          </>
        )}

        <div className="form-control mt-4">
          <label className="label cursor-pointer justify-start gap-2">
            <input
              type="checkbox"
              className="checkbox checkbox-sm checkbox-accent"
              checked={invert_positions}
              onChange={(e) => setValue("invert_positions", e.target.checked)}
            />
            <span className="label-text">Invert positions</span>
          </label>
        </div>

        {((controller == "home_assistant" && supports_position) || controller == "openhab") && (
          <div className="mt-4">
            <span className="label">Position presets</span>
            <div className="join w-full">
              <input
                type="number"
                {...register("position_preset1", { valueAsNumber: true })}
                min={0}
                max={100}
                className={`join-item ${text_input_class}`}
              ></input>
              <input
                type="number"
                {...register("position_preset2", { valueAsNumber: true })}
                min={0}
                max={100}
                className={`join-item ${text_input_class}`}
              ></input>
              <input
                type="number"
                {...register("position_preset3", { valueAsNumber: true })}
                min={0}
                max={100}
                className={`join-item ${text_input_class}`}
              ></input>
            </div>
          </div>
        )}

        {((controller == "home_assistant" && supports_tilt) || (controller == "openhab" && openhab_tilt_item != "")) && (
          <div className="mt-4">
            <span className="label">Tilt presets</span>
            <div className="join w-full">
              <input type="number" {...register("tilt_preset1", { valueAsNumber: true })} min={0} max={100} className={`join-item ${text_input_class}`}></input>
              <input type="number" {...register("tilt_preset2", { valueAsNumber: true })} min={0} max={100} className={`join-item ${text_input_class}`}></input>
              <input type="number" {...register("tilt_preset3", { valueAsNumber: true })} min={0} max={100} className={`join-item ${text_input_class}`}></input>
            </div>
          </div>
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

export default MultiStep_AddEditEntity_Step3_Cover;
