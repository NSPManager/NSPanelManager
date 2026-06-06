import Select, { type OptionProps } from "react-select";
import { type ClassNamesConfig, type GroupBase } from "react-select";
import { type formDataType, type handleChangeType } from "../MultiStep_AddEditEntity";
import { useState } from "react";
import { useAvailableEntitiesStore } from "../../AvailableEntitiesStore";
import { type IOptionType } from "../../AvailableEntitiesStore";

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
  handleChange,
  values,
  onComplete,
}: {
  handleChange: handleChangeType;
  values: formDataType;
  onComplete?: () => void;
}) => {
  const [hasFetchedConfig, setHasFetchedConfig] = useState<boolean>(false);
  const [entitySettings, setEntitySettings] = useState<formDataType>({
    home_assistant_name: "",
    openhab_item_switch: "",
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
    console.log("Updating entity with data:", entitySettings);
    fetch("/rest/entities/switches", {
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
    <div>
      <div className="flex justify-center mb-4 duration-500 transition-transform ease-linear w-full">
        <ul className="steps">
          <li className="step step-primary">Type</li>
          <li className="step step-primary">Controller</li>
          <li className="step step-primary">Switch</li>
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
                onChange={handleChange("friendly_name")}
                value={values.friendly_name ? values.friendly_name.toString() : ""}
                required
              />
            </div>
          </div>
        </div>

        {values.controller == "home_assistant" && (
          <>
            <label className="block mb-2 text-sm font-medium">Home Assistant entity</label>
            <Select<IOptionType>
              options={useAvailableEntitiesStore
                .getState()
                .home_assistant_options.filter((option) => option.value.startsWith("switch.") || option.value.startsWith("input_boolean."))}
              classNames={classNames}
              onChange={(newValue) => {
                setEntitySettings((prev) => ({ ...prev, home_assistant_name: newValue ? newValue.value : "" }));
              }}
              unstyled
              components={select_components}
              value={useAvailableEntitiesStore.getState().home_assistant_options.find((option) => option.value === entitySettings.home_assistant_name)}
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

        {values.controller == "openhab" && (
          <>
            <label className="block mb-2 mt-4 text-sm font-medium">OpenHAB item</label>
            <Select<IOptionType>
              options={useAvailableEntitiesStore.getState().openhab_options}
              classNames={classNames}
              onChange={(newValue) => setEntitySettings((prev) => ({ ...prev, openhab_item_switch: newValue ? newValue.value : "" }))}
              value={useAvailableEntitiesStore.getState().openhab_options.find((option) => option.value === entitySettings.openhab_item_switch)}
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
        <button type="button" className="btn btn-info" onClick={saveEntity}>
          Save
        </button>
      </div>
    </div>
  );
};

export default MultiStep_AddEditEntity_Step3_Switch;
