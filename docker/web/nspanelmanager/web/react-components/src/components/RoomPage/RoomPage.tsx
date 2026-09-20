import { type ClassNamesConfig, type GroupBase } from "react-select";
import { useEffect, useState } from "react";
import { useForm, useWatch } from "react-hook-form";
import { useStompStore } from "../../stores/StompStore";
import { useEntityStatesStore } from "../../stores/EntityStore";
import EntitiesPagesView from "../EntitiesPage/EntitiesPagesView";
import { Icon } from "@mdi/react";
import { mdiTextShort } from "@mdi/js";
import Select, { type OptionProps } from "react-select";
import { type IOptionType } from "../../stores/AvailableEntitiesStore";
import { useAvailableEntitiesStore } from "../../stores/AvailableEntitiesStore";
import { Notify } from "../NSPanelToastContainer";

export default function RoomPage({ room_id }: { room_id: number }) {
  const { status: stompStatus } = useStompStore();
  const { status: entityStatus, rooms } = useEntityStatesStore();
  const home_assistant_options = useAvailableEntitiesStore((s) => s.home_assistant_options);
  const openhab_options = useAvailableEntitiesStore((s) => s.openhab_options);
  const all_options = [...home_assistant_options.filter((option) => option.value.startsWith("sensor.")), ...openhab_options];
  const room = rooms[room_id];
  const [hasLoadedValues, setHasLoadedValues] = useState(false);

  const { control, register, getValues, setValue, reset, handleSubmit } = useForm({
    defaultValues: {
      id: room?.id,
      friendly_name: room?.friendly_name,
      room_temp_provider: room?.room_temp_provider,
      room_temp_sensor: room?.room_temp_sensor,
    },
  });
  const selected_room_temp_sensor = useWatch({ control, name: "room_temp_sensor" });
  const current_friendly_name = useWatch({ control, name: "friendly_name" });

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

  const onSubmit = (data: any) => {
    // PUT request using fetch with error handling
    fetch("/rest/rooms", {
      credentials: "same-origin",
      method: "PUT",
      mode: "same-origin",
      headers: { "Content-Type": "application/json", "X-CSRFToken": getCookie("csrftoken") },
      body: JSON.stringify(data),
    })
      .then(async (response) => {
        const data = await response.json();

        // check for error response
        if (!response.ok) {
          // get error message from body or default to response status
          const error = (data && data.message) || response.status;
          return Promise.reject(error);
        }

        Notify({ message: `Saved room setting successfully.`, level: "success", duration: 5000 });
      })
      .catch((error) => {
        // setErrorMessage(error);
        Notify({ message: `Error saving entity. Error: ${error}`, level: "error", duration: 10000 });
      });
  };

  useEffect(() => {
    if (stompStatus == "none") {
      useStompStore.getState().connect();
    }
  }, [stompStatus]);

  useEffect(() => {
    if (entityStatus == "none") {
      useEntityStatesStore.getState().fetchData();
    }
  }, [entityStatus]);

  useEffect(() => {
    if (useAvailableEntitiesStore.getState().status == "none") {
      useAvailableEntitiesStore.getState().fetchEntities();
    }
  }, []);

  useEffect(() => {
    if (room && !hasLoadedValues) {
      reset({
        id: room.id,
        friendly_name: room.friendly_name,
        room_temp_provider: room.room_temp_provider,
        room_temp_sensor: room.room_temp_sensor,
      });
      setHasLoadedValues(true);
    }
  }, [room, reset, hasLoadedValues, setHasLoadedValues]);

  if (entityStatus != "loaded") {
    return (
      <div className="flex w-full items-center justify-center text-2xl">
        <span className="loading loading-spinner loading-sm mr-2"></span>
        <span className="font-medium">Loading data ({entityStatus})...</span>
      </div>
    );
  }

  if (room_id && !rooms[room_id]) {
    return (
      <div className="flex w-full items-center justify-center text-2xl">
        <span className="font-medium text-error text-lg">Room with ID {room_id} not found.</span>
      </div>
    );
  }

  // Class names for the select component.
  const classNames: ClassNamesConfig<{ value: string; label: string }, false, GroupBase<{ value: string; label: string }>> = {
    control: (state) => `${state.isFocused ? "border" : "border-0"} border-accent p-2.5 text-sm rounded-box bg-base-300 text-base-content rounded-md`,
    menu: () => "bg-base-300 p-2.5 rounded-box text-base-content",
    option: (state) => `p-1 ${state.isSelected ? "bg-primary/20 rounded-sm" : ""} ${state.isFocused ? "bg-primary/20 rounded-sm" : ""}`,
  };
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

  return (
    <>
      <form onSubmit={handleSubmit(onSubmit)}>
        <div className="flex items-center justify-between" id="nspanel_settings_container">
          <div>
            <h2 className="font-medium text-2xl" id="nspanel_name">
              Settings for room '{current_friendly_name}'
            </h2>
          </div>
          <a href="https://nspanelmanager.com/docs/web-interface#room-page" target="_blank" className="btn btn-outline btn-info">
            Help
          </a>
        </div>
        <div className="md:flex md:justify-between md:items-start mt-2">
          <div className="p-4 bg-base-100 rounded-md shadow-lg w-full">
            <span className="block mb-2 text-xl">General</span>
            <div className="">
              <label htmlFor="friendly_name" className="block mb-2 text-sm font-medium">
                Name
              </label>
              <div className="flex flex-row-reverse">
                <input
                  className="outline-none rounded-none bg-base-300 border-neutral rounded-e-md border border-l-0 focus:ring-0 focus:border-accent block flex-1 min-w-0 w-full text-sm p-2.5 peer/name"
                  type="text"
                  name="friendly_name"
                  // value={room.friendly_name}
                  {...register("friendly_name")}
                />
                <span className="inline-flex items-center px-3 text-sm border border-neutral rounded-e-0 rounded-s-md peer-focus/name:border-accent">
                  <Icon path={mdiTextShort} className="w-4 h-4"></Icon>{" "}
                </span>
              </div>
            </div>

            <input type="hidden" name="room_temp_provider" id="room_temp_provider" readOnly={true} value="{{ room.room_temp_provider }}" />
            <div className="my-2 w-full md:ms-1">
              <label className="block mb-2 text-sm font-medium">Room temperature sensor</label>
              <Select<IOptionType>
                options={all_options}
                classNames={classNames}
                onChange={(newValue) => {
                  if (newValue) {
                    setValue("room_temp_provider", newValue.type, { shouldDirty: true });
                    setValue("room_temp_sensor", newValue.value, { shouldDirty: true });
                  } else {
                    setValue("room_temp_provider", "", { shouldDirty: true });
                    setValue("room_temp_sensor", "", { shouldDirty: true });
                  }
                }}
                unstyled
                components={select_components}
                value={all_options.find((option) => option.value === getValues("room_temp_sensor"))}
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
            {/*{% component "alert" level="info" text="A specific temperature sensor is not a requirement but it can be used to provide a more accurate value on the screensaver. If no sensor is selected the temperature will be read using the built-in NTC thermistor. If a sensor is selected the panel will still report its temperature over MQTT to Home Assistant and OpenHAB." %}{% endcomponent %}*/}

            {/*{% if total_num_rooms <= 1 %}*/}
            <div className="my-4">
              {/*{% component "alert" level="info" text="The last room cannot be deleted. Rename this or create another room before removing this one." %}{% endcomponent %}*/}
            </div>
            {/*{% endif %}*/}
            <div className="text-right mt-4">
              <button
                // href="{% if total_num_rooms != 1 %}{{ ingress_path }}{% url 'delete_room' room_id=room.id %}{% else %}#{% endif %}"
                className="btn btn-error"
                disabled={Object.keys(rooms).length === 1}
              >
                Delete
              </button>
              <button type="submit" className="btn btn-success">
                Save
              </button>
            </div>
          </div>
        </div>
      </form>

      <div className="md:flex md:justify-between md:items-start mt-2">
        <div className="p-4 bg-base-100 rounded-md shadow-lg w-full">
          <EntitiesPagesView room_id={room.id} type="scene" />
        </div>
      </div>

      <div className="md:flex md:justify-between md:items-start mt-2 mb-4">
        <div className="p-4 bg-base-100 rounded-md shadow-lg w-full">
          <EntitiesPagesView room_id={room.id} type="entity" />
        </div>
      </div>
    </>
  );
}
