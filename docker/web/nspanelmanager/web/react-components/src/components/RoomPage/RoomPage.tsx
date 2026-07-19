import { useEffect } from "react";
import { useStompStore } from "../../stores/StompStore";
import { useEntityStatesStore } from "../../stores/EntityStore";
import EntitiesPagesView from "../EntitiesPage/EntitiesPagesView";
import { Icon } from "@mdi/react";
import { mdiTextShort } from "@mdi/js";

export default function RoomPage({ room_id }: { room_id: string }) {
  const { status: stompStatus } = useStompStore();
  const { status: entityStatus, rooms } = useEntityStatesStore();

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

  const room = rooms[room_id];
  return (
    <>
      <form method="POST" action="{{ ingress_path }}{% url 'update_room_form' room_id=room.id %}">
        <div className="flex items-center justify-between" id="nspanel_settings_container">
          <div>
            <h2 className="font-medium text-2xl" id="nspanel_name">
              Settings for room '{room.friendly_name}'
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
                  value={room.friendly_name}
                />
                <span className="inline-flex items-center px-3 text-sm border border-neutral rounded-e-0 rounded-s-md peer-focus/name:border-accent">
                  <Icon path={mdiTextShort} className="w-4 h-4"></Icon>{" "}
                </span>
              </div>
            </div>

            <input type="hidden" name="room_temp_provider" id="room_temp_provider" readOnly={true} value="{{ room.room_temp_provider }}" />
            <div className="my-2 w-full md:ms-1">
              <label htmlFor="room_temp_sensor" className="block mb-1 text-sm font-medium">
                Room temperature sensor
              </label>
              <div className="flex flex-row-reverse">
                <button
                  type="button"
                  className="btn btn-info rounded-l-none"
                  hx-get="{{ ingress_path }}{% url 'htmx_partial_select_room_temperature_sensor_provider' room_id=room.id %}"
                  hx-swap="none"
                >
                  Select
                </button>
                {/*<button type="button" className="btn btn-error rounded-none" onclick="clear_selected_room_temperatuer_sensor()">Clear</button>*/}
                <input
                  className="outline-none bg-base-300 border-neutral border border-l-0 focus:ring-0 focus:border-accent block flex-1 min-w-0 w-full text-sm p-2.5 peer/name"
                  type="text"
                  id="room_temp_sensor"
                  name="room_temp_sensor"
                  value="{{ room.room_temp_sensor }}"
                  placeholder="None selected"
                  readOnly={true}
                />
                <span className="inline-flex items-center px-3 text-sm border border-neutral rounded-e-0 rounded-s-field peer-focus/name:border-accent">
                  <span className="mdi mdi-thermometer"></span>
                </span>
              </div>
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
