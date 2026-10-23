import Select, { type OptionProps } from "react-select";
import { type ClassNamesConfig, type GroupBase } from "react-select";
import { useAvailableEntitiesStore } from "../../../stores/AvailableEntitiesStore";
import { type IOptionType } from "../../../stores/AvailableEntitiesStore";
import { useForm } from "react-hook-form";
import { zodResolver } from "@hookform/resolvers/zod";
import z from "zod";
import { useEntitiesPagesStore } from "../../../stores/EntitiesPagesStore";
import { useEffect, useState } from "react";
import { Notify } from "../../NSPanelToastContainer";

// Same rules as put_media_player_entity in rest.py, so an invalid combination can't be saved.
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
    home_assistant_name: z.string().min(1),
    volume_step: z.number().int().min(1).max(100),
    source_volume_strategy: z.enum(["none", "player_attributes", "source_entity"]),
    source_entity_attribute: z.string(),
    source_volume_attribute: z.string(),
  })
  .superRefine((data, ctx) => {
    if (data.source_volume_strategy != "none" && data.source_entity_attribute.trim() == "") {
      ctx.addIssue({ code: "custom", path: ["source_entity_attribute"], message: "Required for this multi-room setup" });
    }
    if (data.source_volume_strategy == "player_attributes" && data.source_volume_attribute.trim() == "") {
      ctx.addIssue({ code: "custom", path: ["source_volume_attribute"], message: "Required for this multi-room setup" });
    }
  });
export type MediaPlayerFormData = z.infer<typeof schema>;

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

// How a media player in a multi-room setup tells which source player is feeding it.
// A standalone media player uses the "none" strategy.
const multi_room_strategies = [
  {
    value: "source_entity",
    label: "The room player says which player feeds it",
    description:
      "An attribute on the room player holds the entity ID of the source player currently feeding it. The panel follows that player and uses its own volume and sources, even when the source changes.",
  },
  {
    value: "player_attributes",
    label: "The room player reports the source player and its volume",
    description:
      "Attributes on the room player hold both the entity ID of the source player and the source volume. Use this when the integration reports the source volume on the room player itself.",
  },
];

const MultiStep_AddEditEntity_Step3_MediaPlayer = ({
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
  const defaultValues: MediaPlayerFormData = {
    id: id ?? null,
    controller: controller,
    type: "entity",
    entity_type: "media_player",
    friendly_name: "",
    room_id: room_id,
    entities_page_id: entities_page_id,
    room_view_position: room_view_position,
    home_assistant_name: "",
    volume_step: 5,
    source_volume_strategy: "none",
    source_entity_attribute: "",
    source_volume_attribute: "",
  };

  const {
    handleSubmit,
    register,
    reset,
    setValue,
    watch,
    formState: { isValid, errors },
  } = useForm<MediaPlayerFormData>({
    resolver: zodResolver(schema),
    mode: "onChange",
    defaultValues: defaultValues,
  });

  const home_assistant_name = watch("home_assistant_name");
  const source_volume_strategy = watch("source_volume_strategy");
  // A standalone media player has no source player, so it is saved with the "none" strategy.
  const [multiRoom, setMultiRoom] = useState(false);

  const selectMultiRoom = (multi_room: boolean) => {
    setMultiRoom(multi_room);
    setValue("source_volume_strategy", multi_room ? "source_entity" : "none", { shouldValidate: true });
  };

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
      // Media players saved before a field existed won't have it, keep the default for those.
      const values = { ...defaultValues, ...entityData };
      reset(values);
      setMultiRoom(values.source_volume_strategy != "none");
    }
  }, [id, useEntitiesPagesStore.getState().entities]);

  function saveEntity(data: MediaPlayerFormData) {
    // Don't keep attributes the selected setup doesn't use, so switching setups never leaves stale config behind.
    if (data.source_volume_strategy == "none") {
      data = { ...data, source_entity_attribute: "", source_volume_attribute: "" };
    } else if (data.source_volume_strategy == "source_entity") {
      data = { ...data, source_volume_attribute: "" };
    }

    // PUT request using fetch with error handling
    fetch("/rest/entities/media_players", {
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
        onComplete?.();
      })
      .catch((error) => {
        Notify({ message: `Error saving entity. Error: ${error}`, level: "error", duration: 10000 });
      });
  }

  // Suggest attribute names from the selected media player's current state in Home Assistant.
  const selected_entity = useAvailableEntitiesStore.getState().home_assistant_entities.find((entity) => entity.item_id == home_assistant_name);
  const attribute_names: string[] = Object.keys(selected_entity?.item?.attributes ?? {}).sort();

  const classNames: ClassNamesConfig<IOptionType, false, GroupBase<IOptionType>> = {
    control: (state) => `${state.isFocused ? "border" : "border-0"} border-accent p-2.5 text-sm rounded-box bg-base-300 rounded-md`,
    menu: () => "bg-base-300 p-2.5 rounded-box",
    option: (state) => `p-1 ${state.isSelected ? "bg-primary/20 rounded-sm" : ""} ${state.isFocused ? "bg-primary/20 rounded-sm" : ""}`,
  };

  const text_input_class =
    "outline-none bg-base-300 border-neutral rounded-md border focus:ring-0 focus:border-accent block flex-1 min-w-0 w-full text-sm p-2.5";

  return (
    <form onSubmit={handleSubmit(saveEntity)}>
      <input type="hidden" {...register("id")} />
      <input type="hidden" {...register("controller")} />
      <input type="hidden" {...register("type")} />
      <input type="hidden" {...register("entity_type")} />
      <input type="hidden" {...register("room_id")} />
      <input type="hidden" {...register("entities_page_id")} />
      <input type="hidden" {...register("room_view_position")} />

      <div className="alert alert-warning mb-4">
        <span className="mdi mdi-alert text-xl"></span>
        <span>
          <span className="font-bold">BETA:</span> Media players are a beta feature. Not all of the changes they require have been merged into the NSPanel firmware yet, so
          they may not show up or work correctly on your panels.
        </span>
      </div>

      <div className="w-full">
        <div className="flex justify-center mb-4 duration-500 transition-transform ease-linear">
          {/* Name */}
          <div className="w-full">
            <label htmlFor="add_new_media_player_name" className="block mb-2 text-sm font-medium">
              Name
            </label>
            <div className="flex flex-row-reverse">
              <input className={text_input_class} type="text" id="add_new_media_player_name" {...register("friendly_name")} required />
            </div>
          </div>
        </div>

        {controller == "home_assistant" && (
          <div className="mb-4">
            <label className="block mb-2 text-sm font-medium">Home Assistant entity</label>
            <Select<IOptionType>
              options={useAvailableEntitiesStore.getState().home_assistant_options.filter((option) => option.value.startsWith("media_player."))}
              classNames={classNames}
              onChange={(newValue) => {
                setValue("home_assistant_name", newValue ? newValue.value : "", { shouldValidate: true });
              }}
              unstyled
              components={select_components}
              value={useAvailableEntitiesStore.getState().home_assistant_options.find((option) => option.value === home_assistant_name) ?? null}
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

        {/* Volume step */}
        <div className="mb-4">
          <label htmlFor="add_new_media_player_volume_step" className="block mb-2 text-sm font-medium">
            Volume step (%)
          </label>
          <input
            type="number"
            id="add_new_media_player_volume_step"
            className={text_input_class}
            step="1"
            min="1"
            max="100"
            {...register("volume_step", { valueAsNumber: true })}
          />
          <span className="block mt-1 text-xs text-base-content/50">How much one volume up/down press on the panel changes the volume.</span>
          {errors.volume_step && <span className="block mt-1 text-xs text-error">Must be a whole number between 1 and 100.</span>}
        </div>

        {/* Standalone or multi-room */}
        <div className="mt-4 border border-primary rounded-box p-4 pt-2">
          <span className="text-lg">Setup</span>
          <div className="mt-2 w-full text-sm font-medium border border-neutral rounded-lg bg-base-300">
            <div className="flex items-center ps-3 w-full">
              <input
                type="radio"
                id="media_player_setup_standalone"
                name="media_player_setup"
                checked={!multiRoom}
                onChange={() => selectMultiRoom(false)}
                className="w-4 h-4 radio radio-accent"
              />
              <label htmlFor="media_player_setup_standalone" className="w-full py-3 ms-2 text-sm font-medium cursor-pointer">
                <span className="block text-sm font-medium">Standalone device</span>
                <span className="block text-xs text-base-content/50">The panel controls this media player's own volume and sources.</span>
              </label>
            </div>
            <div className="flex items-center ps-3 w-full">
              <input
                type="radio"
                id="media_player_setup_multi_room"
                name="media_player_setup"
                checked={multiRoom}
                onChange={() => selectMultiRoom(true)}
                className="w-4 h-4 radio radio-accent"
              />
              <label htmlFor="media_player_setup_multi_room" className="w-full py-3 ms-2 text-sm font-medium cursor-pointer">
                <span className="block text-sm font-medium">Part of a multi-room audio setup</span>
                <span className="block text-xs text-base-content/50">
                  Another media player, the source player, feeds this room, for example through an audio matrix. The panel can also control the source player's volume
                  and select between its sources.
                </span>
              </label>
            </div>
          </div>

          {multiRoom && (
            <>
              <label className="block mt-4 mb-2 text-sm font-medium">How does Home Assistant tell which source player feeds this room?</label>
              <div className="w-full text-sm font-medium border border-neutral rounded-lg bg-base-300">
                {multi_room_strategies.map((strategy) => (
                  <div key={strategy.value} className="flex items-center ps-3 w-full">
                    <input
                      type="radio"
                      id={`media_player_source_volume_${strategy.value}`}
                      value={strategy.value}
                      {...register("source_volume_strategy")}
                      className="w-4 h-4 radio radio-accent"
                    />
                    <label htmlFor={`media_player_source_volume_${strategy.value}`} className="w-full py-3 ms-2 text-sm font-medium cursor-pointer">
                      <span className="block text-sm font-medium">{strategy.label}</span>
                      <span className="block text-xs text-base-content/50">{strategy.description}</span>
                    </label>
                  </div>
                ))}
              </div>

              <datalist id="media_player_attribute_names">
                {attribute_names.map((attribute) => (
                  <option key={attribute} value={attribute} />
                ))}
              </datalist>

              <div className="mt-4">
                <label htmlFor="add_new_media_player_source_entity_attribute" className="block mb-2 text-sm font-medium">
                  Source player attribute
                </label>
                <input
                  type="text"
                  id="add_new_media_player_source_entity_attribute"
                  className={text_input_class}
                  list="media_player_attribute_names"
                  {...register("source_entity_attribute")}
                />
                <span className="block mt-1 text-xs text-base-content/50">
                  Name of the attribute on the room player holding the entity ID of the source player, for example <code>source_entity_id</code>.
                </span>
                {errors.source_entity_attribute && <span className="block mt-1 text-xs text-error">{errors.source_entity_attribute.message}</span>}
              </div>

              {source_volume_strategy == "player_attributes" && (
                <div className="mt-4">
                  <label htmlFor="add_new_media_player_source_volume_attribute" className="block mb-2 text-sm font-medium">
                    Source volume attribute
                  </label>
                  <input
                    type="text"
                    id="add_new_media_player_source_volume_attribute"
                    className={text_input_class}
                    list="media_player_attribute_names"
                    {...register("source_volume_attribute")}
                  />
                  <span className="block mt-1 text-xs text-base-content/50">
                    Name of the attribute on the room player holding the source volume, from 0.0 to 1.0. Volume changes are sent to the source player.
                  </span>
                  {errors.source_volume_attribute && <span className="block mt-1 text-xs text-error">{errors.source_volume_attribute.message}</span>}
                </div>
              )}
            </>
          )}
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

export default MultiStep_AddEditEntity_Step3_MediaPlayer;
