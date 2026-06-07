import Select, { type OptionProps } from "react-select";
import { type ClassNamesConfig, type GroupBase } from "react-select";
import { useEffect } from "react";
import { useForm } from "react-hook-form";
import z from "zod";
import { zodResolver } from "@hookform/resolvers/zod";
import { useAvailableEntitiesStore } from "../../AvailableEntitiesStore";
import { type IOptionType } from "../../AvailableEntitiesStore";
import { useEntitiesPagesStore } from "../../EntitiesPage/EntitiesPagesStore";

const schema = z.object({
  type: z.string(),
  entity_type: z.string(),
  room_id: z.number(),
  entities_page_id: z.number(),
  room_view_position: z.number(),
  friendly_name: z.string(),
  controller: z.string(),
  can_color_temperature: z.boolean().optional(),
  can_dim: z.boolean().optional(),
  can_rgb: z.boolean().optional(),
  controlled_by_nspanel_main_page: z.boolean().optional(),
  home_assistant_name: z.string().optional(),
  is_ceiling_light: z.boolean().optional(),
  openhab_item_color_temp: z.string().optional(),
  openhab_item_dimmer: z.string().optional(),
  openhab_item_rgb: z.string().optional(),
});
export type LightFormData = z.infer<typeof schema>;

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

const MultiStep_AddEditEntity_Step3_Light = ({
  controller,
  room_id,
  entities_page_id,
  room_view_position,
  id,
  onComplete,
}: {
  controller: string;
  room_id: number;
  entities_page_id: number;
  room_view_position: number;
  id?: number;
  onComplete?: () => void;
}) => {
  const {
    handleSubmit,
    register,
    reset,
    getValues,
    setValue,
    formState: { isValid },
  } = useForm<LightFormData>({
    resolver: zodResolver(schema),
    defaultValues: {
      controller: controller,
      type: "entity",
      entity_type: "light",
      room_id: room_id,
      entities_page_id: entities_page_id,
      room_view_position: room_view_position,
      can_color_temperature: false,
      can_dim: false,
      can_rgb: false,
      controlled_by_nspanel_main_page: false,
      is_ceiling_light: false,
      home_assistant_name: "",
      openhab_item_color_temp: "",
      openhab_item_dimmer: "",
      openhab_item_rgb: "",
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

  // if (id && !hasFetchedConfig) {
  //   fetch(`/rest/entities/${id}`)
  //     .then((response) => response.json())
  //     .then((data) => {
  //       // setEntitySettings({ ...entitySettings, ...data.result.entity });
  //       reset(data);
  //       setHasFetchedConfig(true);
  //     });
  // }

  function onSave(values: LightFormData) {
    // PUT request using fetch with error handling
    console.log("Updating entity with data:", values);
    fetch("/rest/entities/lights", {
      credentials: "same-origin",
      method: "PUT",
      mode: "same-origin",
      headers: { "Content-Type": "application/json", "X-CSRFToken": getCookie("csrftoken") },
      body: JSON.stringify({
        values,
      }),
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
    <form onSubmit={handleSubmit(onSave)}>
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
          <li className="step step-primary">Light</li>
        </ul>
      </div>

      <div className="w-full">
        <div className="flex justify-center mb-4 duration-500 transition-transform ease-linear">
          {/* Name */}
          <div className="w-full">
            <label htmlFor="add_new_light_name" className="block mb-2 text-sm font-medium">
              Name
            </label>
            <div className="flex flex-row-reverse">
              <input
                className="outline-none bg-base-300 border-neutral rounded-md border focus:ring-0 focus:border-accent block flex-1 min-w-0 w-full text-sm p-2.5 peer/search_text"
                type="text"
                id="add_new_light_name"
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
                .home_assistant_options.filter((option) => option.value.startsWith("light.") || option.value.startsWith("switch."))}
              classNames={classNames}
              onChange={(newValue) => setValue("home_assistant_name", newValue ? newValue.value : "", { shouldValidate: true, shouldDirty: true })}
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

        <fieldset className="fieldset mt-4">
          <input type="hidden" {...register("controlled_by_nspanel_main_page")} />
          <label className={`label ${getValues("controlled_by_nspanel_main_page") ? "text-base-content" : "text-base-content/50"}`}>
            <input
              type="checkbox"
              className="toggle toggle-accent"
              checked={getValues("controlled_by_nspanel_main_page")}
              onClick={() => {
                setValue("controlled_by_nspanel_main_page", !getValues("controlled_by_nspanel_main_page"), { shouldValidate: true, shouldDirty: true });
              }}
            />
            {getValues("controlled_by_nspanel_main_page") ? "Controlled by NSPanel main page" : "Not controlled by NSPanel main page"}
          </label>
        </fieldset>

        {/*Select light type*/}
        <label className="block mb-2 mt-4 text-sm font-medium">Light type</label>
        <input type="hidden" {...register("is_ceiling_light")} />
        <div className="grid grid-cols-2 gap-4 w-full">
          {/*Table lamp*/}
          <div
            className="group bg-base-100 rounded-box border border-neutral border-dashed p-6 cursor-pointer hover:bg-yellow-500/10"
            onClick={() => setValue("is_ceiling_light", false, { shouldValidate: true, shouldDirty: true })}
          >
            <div className="flex justify-center items-center p-4">
              <span
                className={`mdi mdi-lamp text-5xl text-neutral-content/50 group-hover:text-yellow-500 ${getValues("is_ceiling_light") ? "" : "text-yellow-500"}`}
              ></span>
            </div>
            <div className="flex justify-center items-center">
              <span className="text-base-content/50 group-hover:text-base-content text-center">Table light</span>
            </div>
          </div>

          {/*Ceiling lamp*/}
          <div
            className="group bg-base-100 rounded-box border border-neutral border-dashed p-6 cursor-pointer hover:bg-yellow-500/10"
            onClick={() => setValue("is_ceiling_light", true, { shouldValidate: true, shouldDirty: true })}
          >
            <div className="flex justify-center items-center p-4">
              <span
                className={`mdi mdi-ceiling-light text-5xl text-neutral-content/50 group-hover:text-yellow-500 ${getValues("is_ceiling_light") ? "text-yellow-500" : ""}`}
              ></span>
            </div>
            <div className="flex justify-center items-center">
              <span className="text-base-content/50 group-hover:text-base-content text-center">Ceiling light</span>
            </div>
          </div>
        </div>

        {/*Select light features*/}
        <label className="block mb-2 mt-4 text-sm font-medium">Light capabilities</label>
        <input type="hidden" {...register("can_dim")} />
        <div className="grid grid-cols-3 gap-4 w-full">
          {/*Table lamp*/}
          <div
            className="group bg-base-100 rounded-box border border-neutral border-dashed p-6 cursor-pointer hover:bg-yellow-500/10"
            onClick={() => {
              setValue("can_dim", !getValues("can_dim"), { shouldValidate: true, shouldDirty: true });
            }}
          >
            <div className="flex justify-center items-center p-4">
              <span
                className={`mdi mdi-lightbulb-on-50 text-5xl text-neutral-content/50 group-hover:text-yellow-500 ${getValues("can_dim") ? "text-yellow-500" : ""}`}
              ></span>
            </div>
            <div className="flex justify-center items-center">
              <span className="text-base-content/50 group-hover:text-base-content text-center">Dimmable</span>
            </div>
          </div>

          {/*Ceiling lamp*/}
          <input type="hidden" {...register("can_color_temperature")} />
          <div
            className="group bg-base-100 rounded-box border border-neutral border-dashed p-6 cursor-pointer hover:bg-yellow-500/10"
            onClick={() => {
              setValue("can_color_temperature", !getValues("can_color_temperature"), { shouldValidate: true, shouldDirty: true });
            }}
          >
            <div className="flex justify-center items-center p-4">
              <span
                className={`mdi mdi-temperature-kelvin text-5xl text-neutral-content/50 group-hover:text-yellow-500 ${getValues("can_color_temperature") ? "text-yellow-500" : ""}`}
              ></span>
            </div>
            <div className="flex justify-center items-center">
              <span className="text-base-content/50 group-hover:text-base-content text-center">Color temp</span>
            </div>
          </div>

          {/*Can RGB*/}
          <input type="hidden" {...register("can_rgb")} />
          <div
            className="group bg-base-100 rounded-box border border-neutral border-dashed p-6 cursor-pointer hover:bg-yellow-500/10"
            onClick={() => {
              setValue("can_rgb", !getValues("can_rgb"), { shouldValidate: true, shouldDirty: true });
            }}
          >
            <div className="flex justify-center items-center p-4">
              <span
                className={`mdi mdi-palette text-5xl text-neutral-content/50 group-hover:text-yellow-500 ${getValues("can_rgb") ? "text-yellow-500" : ""}`}
              ></span>
            </div>
            <div className="flex justify-center items-center">
              <span className="text-base-content/50 group-hover:text-base-content text-center">Color</span>
            </div>
          </div>
        </div>

        {controller == "openhab" && (
          <>
            <label className="block mb-2 mt-4 text-sm font-medium">Brightness item</label>
            <Select<IOptionType>
              options={useAvailableEntitiesStore.getState().openhab_options}
              classNames={classNames}
              onChange={(newValue) => setValue("openhab_item_dimmer", newValue ? newValue.value : "", { shouldValidate: true, shouldDirty: true })}
              value={useAvailableEntitiesStore.getState().openhab_options.find((option) => option.value === getValues("openhab_item_dimmer"))}
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

        {controller == "openhab" && getValues("can_color_temperature") == true && (
          <>
            <label className="block mb-2 mt-4 text-sm font-medium">Color temperature item</label>
            <Select<IOptionType>
              options={useAvailableEntitiesStore.getState().openhab_options}
              classNames={classNames}
              onChange={(newValue) => {
                setValue("openhab_item_color_temp", newValue ? newValue.value : "", { shouldValidate: true, shouldDirty: true });
              }}
              value={useAvailableEntitiesStore.getState().openhab_options.find((option) => option.value === getValues("openhab_item_color_temp"))}
              components={select_components}
              unstyled
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

        {controller == "openhab" && getValues("can_rgb") == true && (
          <>
            <label className="block mb-2 mt-4 text-sm font-medium">RGB item</label>
            <Select<IOptionType>
              options={useAvailableEntitiesStore.getState().openhab_options}
              classNames={classNames}
              onChange={(newValue) => setValue("openhab_item_rgb", newValue ? newValue.value : "", { shouldValidate: true, shouldDirty: true })}
              value={useAvailableEntitiesStore.getState().openhab_options.find((option) => option.value === getValues("openhab_item_rgb"))}
              components={select_components}
              unstyled
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
        <button className="btn btn-info" disabled={!isValid}>
          Save
        </button>
      </div>
    </form>
  );
};

export default MultiStep_AddEditEntity_Step3_Light;
