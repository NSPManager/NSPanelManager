import Select, { type OptionProps } from "react-select";
import { type ClassNamesConfig, type GroupBase } from "react-select";
import { type formDataType, type handleChangeType } from "../MultiStep_AddEditEntity";
import { useState } from "react";

interface OptionType {
  value: string;
  label: string;
  icon?: string;
}

const CustomOption: React.FC<OptionProps<OptionType>> = ({ innerProps, isDisabled, isFocused, isSelected, children, data }) => {
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

const MultiStep_AddEditEntity_Step3_Light = ({ handleChange, values }: { handleChange: handleChangeType; values: formDataType }) => {
  const [options, setOptions] = useState<OptionType[]>([]);
  const [hasFetchedConfig, setHasFetchedConfig] = useState<boolean>(false);
  const [entitySettings, setEntitySettings] = useState<formDataType>({
    type: "light",
    can_color_temperature: "",
    can_dim: "",
    can_rgb: "",
    controlled_by_nspanel_main_page: true,
    home_assistant_name: "",
    is_ceiling_light: false,
    openhab_control_mode: "",
    openhab_item_color_temp: "",
    openhab_item_dimmer: "",
    openhab_item_rgb: "",
    openhab_item_switch: "",
    openhab_name: "",
    has_fetched_config: false,
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

  function fetch_entities() {
    const home_assistant_filter_params = new URLSearchParams({
      filter: JSON.stringify({ type: ["light", "switch"] }),
    });
    fetch(`/rest/home_assistant/entities?${home_assistant_filter_params.toString()}`, {
      credentials: "same-origin",
      method: "GET",
      mode: "same-origin",
      headers: { "Content-Type": "application/json", "X-CSRFToken": getCookie("csrftoken") },
    })
      .then(async (response) => {
        const data = await response.json();

        // check for error response
        if (!response.ok) {
          // get error message from body or default to response status
          const error = (data && data.message) || response.status;
          return Promise.reject(error);
        }

        const local_options: OptionType[] = [];
        for (const item of data.items) {
          let mdi_icon = "mdi-help"; // Default to a question mark when no icon is found for a given entity type.
          if (item.item_id.startsWith("light")) {
            mdi_icon = "mdi-lightbulb";
          } else if (item.item_id.startsWith("switch")) {
            mdi_icon = "mdi-toggle-switch-variant";
          }

          local_options.push({
            value: item.item_id,
            label: item.label,
            icon: mdi_icon,
          });
        }
        setOptions(local_options);
        console.log("Options length:", options.length);
      })
      .catch((error) => {
        // setErrorMessage(error);
        console.error("There was an error!", error);
      });
  }
  if (options.length == 0) {
    console.log("Options length is 0, fetching entities...");
    fetch_entities();
  }

  if (values.id != null && !hasFetchedConfig) {
    fetch(`/rest/entities/${values.id}`)
      .then((response) => response.json())
      .then((data) => {
        setEntitySettings({ ...entitySettings, ...data.result.entity });
        setHasFetchedConfig(true);
      });
  }

  function saveEntity() {
    // PUT request using fetch with error handling
    fetch("/rest/entities/lights", {
      credentials: "same-origin",
      method: "PUT",
      mode: "same-origin",
      headers: { "Content-Type": "application/json", "X-CSRFToken": getCookie("csrftoken") },
      body: JSON.stringify({
        base: values,
        entity: entitySettings,
      }),
    })
      .then(async (response) => {
        const data = await response.json();

        // check for error response
        if (!response.ok) {
          // get error message from body or default to response status
          const error = (data && data.message) || response.status;
          return Promise.reject(error);
        }

        for (let i = 0; i < document.getElementsByTagName("dialog").length; i++) {
          const dialog = document.getElementsByTagName("dialog")[i];
          if (dialog.id == "modal") {
            dialog.close();
          }
        }

        // setPostId(data.id);
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
    <div>
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
                onChange={handleChange("friendly_name")}
                value={values.friendly_name.toString()}
                required
              />
            </div>
          </div>
        </div>

        {values.controller == "home_assistant" && (
          <>
            <label className="block mb-2 text-sm font-medium">Home Assistant entity</label>
            <Select<OptionType>
              options={options}
              classNames={classNames}
              onChange={(newValue) => {
                setEntitySettings((prev) => ({ ...prev, home_assistant_name: newValue ? newValue.value : "" }));
              }}
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

        <fieldset className="fieldset mt-4">
          <label className={`label ${entitySettings.controlled_by_nspanel_main_page ? "text-base-content" : "text-base-content/50"}`}>
            <input
              type="checkbox"
              name="controlled_by_nspanel_main_page"
              className="toggle toggle-accent"
              checked={entitySettings.controlled_by_nspanel_main_page ? true : false}
              onChange={(newValue) => {
                setEntitySettings((prev) => ({ ...prev, controlled_by_nspanel_main_page: newValue.target.checked }));
              }}
            />
            {entitySettings.controlled_by_nspanel_main_page ? "Controlled by NSPanel main page" : "Not controlled by NSPanel main page"}
          </label>
        </fieldset>

        {/*Select light type*/}
        <label className="block mb-2 mt-4 text-sm font-medium">Light type</label>
        <div className="grid grid-cols-2 gap-4 w-full">
          {/*Table lamp*/}
          <div
            className="group bg-base-100 rounded-box border border-neutral border-dashed p-6 cursor-pointer hover:bg-yellow-500/10"
            onClick={() => setEntitySettings((prev) => ({ ...prev, is_ceiling_light: false }))}
          >
            <div className="flex justify-center items-center p-4">
              <span
                className={`mdi mdi-lamp text-5xl text-neutral-content/50 group-hover:text-yellow-500 ${!entitySettings.is_ceiling_light ? "text-yellow-500" : ""}`}
              ></span>
            </div>
            <div className="flex justify-center items-center">
              <span className="text-base-content/50 group-hover:text-base-content text-center">Table light</span>
            </div>
          </div>

          {/*Ceiling lamp*/}
          <div
            className="group bg-base-100 rounded-box border border-neutral border-dashed p-6 cursor-pointer hover:bg-yellow-500/10"
            onClick={() => setEntitySettings((prev) => ({ ...prev, is_ceiling_light: true }))}
          >
            <div className="flex justify-center items-center p-4">
              <span
                className={`mdi mdi-ceiling-light text-5xl text-neutral-content/50 group-hover:text-yellow-500 ${entitySettings.is_ceiling_light ? "text-yellow-500" : ""}`}
              ></span>
            </div>
            <div className="flex justify-center items-center">
              <span className="text-base-content/50 group-hover:text-base-content text-center">Ceiling light</span>
            </div>
          </div>
        </div>

        {/*Select light features*/}
        <label className="block mb-2 mt-4 text-sm font-medium">Light capabilities</label>
        <div className="grid grid-cols-3 gap-4 w-full">
          {/*Table lamp*/}
          <div
            className="group bg-base-100 rounded-box border border-neutral border-dashed p-6 cursor-pointer hover:bg-yellow-500/10"
            onClick={() => {
              setEntitySettings((prev) => ({ ...prev, can_dim: !entitySettings.can_dim }));
              setEntitySettings((prev) => ({ ...prev, openhab_control_mode: entitySettings.can_dim ? "dimmer" : "switch" }));
            }}
          >
            <div className="flex justify-center items-center p-4">
              <span
                className={`mdi mdi-lightbulb-on-50 text-5xl text-neutral-content/50 group-hover:text-yellow-500 ${entitySettings.can_dim ? "text-yellow-500" : ""}`}
              ></span>
            </div>
            <div className="flex justify-center items-center">
              <span className="text-base-content/50 group-hover:text-base-content text-center">Dimmable</span>
            </div>
          </div>

          {/*Ceiling lamp*/}
          <div
            className="group bg-base-100 rounded-box border border-neutral border-dashed p-6 cursor-pointer hover:bg-yellow-500/10"
            onClick={() => {
              setEntitySettings((prev) => ({ ...prev, can_color_temperature: !entitySettings.can_color_temperature }));
            }}
          >
            <div className="flex justify-center items-center p-4">
              <span
                className={`mdi mdi-temperature-kelvin text-5xl text-neutral-content/50 group-hover:text-yellow-500 ${entitySettings.can_color_temperature ? "text-yellow-500" : ""}`}
              ></span>
            </div>
            <div className="flex justify-center items-center">
              <span className="text-base-content/50 group-hover:text-base-content text-center">Color temp</span>
            </div>
          </div>

          {/*Can RGB*/}
          <div
            className="group bg-base-100 rounded-box border border-neutral border-dashed p-6 cursor-pointer hover:bg-yellow-500/10"
            onClick={() => {
              setEntitySettings((prev) => ({ ...prev, can_rgb: !entitySettings.can_rgb }));
            }}
          >
            <div className="flex justify-center items-center p-4">
              <span
                className={`mdi mdi-palette text-5xl text-neutral-content/50 group-hover:text-yellow-500 ${entitySettings.can_rgb ? "text-yellow-500" : ""}`}
              ></span>
            </div>
            <div className="flex justify-center items-center">
              <span className="text-base-content/50 group-hover:text-base-content text-center">Color</span>
            </div>
          </div>
        </div>

        {values.controller == "openhab" && (
          <>
            <label className="block mb-2 mt-4 text-sm font-medium">Brightness item</label>
            <Select<OptionType>
              options={options}
              classNames={classNames}
              onChange={(newValue) => setEntitySettings((prev) => ({ ...prev, openhab_item_dimmer: newValue ? newValue.value : "" }))}
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

        {values.controller == "openhab" && entitySettings.can_color_temperature == "true" && (
          <>
            <label className="block mb-2 mt-4 text-sm font-medium">Color temperature item</label>
            <Select<OptionType>
              options={options}
              classNames={classNames}
              onChange={(newValue) => {
                setEntitySettings((prev) => ({ ...prev, openhab_item_color_temp: newValue ? newValue.value : "" }));
              }}
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

        {values.controller == "openhab" && entitySettings.can_rgb == "true" && (
          <>
            <label className="block mb-2 mt-4 text-sm font-medium">RGB item</label>
            <Select<OptionType>
              options={options}
              classNames={classNames}
              onChange={(newValue) => setEntitySettings((prev) => ({ ...prev, openhab_item_rgb: newValue ? newValue.value : "" }))}
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
        <button type="button" className="btn btn-info" onClick={saveEntity}>
          Save
        </button>
      </div>
    </div>
  );
};

export default MultiStep_AddEditEntity_Step3_Light;
