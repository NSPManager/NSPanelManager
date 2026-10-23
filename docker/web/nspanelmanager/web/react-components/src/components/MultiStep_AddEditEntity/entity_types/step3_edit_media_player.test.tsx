import { render, screen, waitFor } from "@testing-library/react";
import userEvent from "@testing-library/user-event";
import { afterEach, beforeEach, describe, expect, it, vi } from "vitest";
import MultiStep_AddEditEntity_Step3_MediaPlayer from "./step3_edit_media_player";
import { useAvailableEntitiesStore } from "../../../stores/AvailableEntitiesStore";
import { useEntitiesPagesStore, type IEntityOrSceneData } from "../../../stores/EntitiesPagesStore";
import { Notify } from "../../NSPanelToastContainer";

vi.mock("../../NSPanelToastContainer", () => ({ Notify: vi.fn() }));

// A room player in a multi-room setup and an entity of another type, as /rest/home_assistant/entities returns them.
const home_assistant_entities = [
  {
    type: "home_assistant",
    label: "Kitchen zone",
    item_id: "media_player.kitchen_zone",
    item: {
      entity_id: "media_player.kitchen_zone",
      attributes: { friendly_name: "Kitchen zone", volume_level: 0.2, source_entity_id: "media_player.living_room", source_volume: 0.4 },
    },
  },
  {
    type: "home_assistant",
    label: "Kitchen light",
    item_id: "light.kitchen",
    item: { entity_id: "light.kitchen", attributes: { friendly_name: "Kitchen light" } },
  },
];

const fetchMock = vi.fn();

function respondWith(status: number, body: object) {
  fetchMock.mockResolvedValue({ ok: status >= 200 && status < 300, status: status, json: async () => body });
}

function renderForm(props: { id?: number; onComplete?: () => void } = {}) {
  return render(
    <MultiStep_AddEditEntity_Step3_MediaPlayer controller="home_assistant" room_id={1} entities_page_id={2} room_view_position={3} id={props.id} onComplete={props.onComplete} />,
  );
}

function sentBody() {
  expect(fetchMock).toHaveBeenCalledTimes(1);
  const [url, options] = fetchMock.mock.calls[0];
  expect(url).toBe("/rest/entities/media_players");
  expect(options.method).toBe("PUT");
  return JSON.parse(options.body);
}

async function fillNameAndEntity(user: ReturnType<typeof userEvent.setup>) {
  await user.type(screen.getByLabelText("Name"), "Kitchen speakers");
  await user.click(screen.getByRole("combobox"));
  await user.click(await screen.findByText("Kitchen zone"));
}

const saveButton = () => screen.getByRole("button", { name: "Save" });

beforeEach(() => {
  vi.stubGlobal("fetch", fetchMock);
  respondWith(200, { status: "ok" });
  useAvailableEntitiesStore.setState({
    status: "loaded",
    home_assistant_entities: home_assistant_entities,
    home_assistant_options: home_assistant_entities.map((entity) => ({ value: entity.item_id, label: entity.label, icon: "mdi-speaker", type: entity.type })),
    openhab_items: [],
    openhab_options: [],
  });
  useEntitiesPagesStore.setState({ entities: [], entities_pages: [], scenes: [] });
});

afterEach(() => {
  fetchMock.mockReset();
  vi.mocked(Notify).mockClear();
  vi.unstubAllGlobals();
});

describe("media player form", () => {
  it("shows the beta banner", () => {
    renderForm();
    expect(screen.getByText(/Media players are a beta feature/)).toBeInTheDocument();
  });

  it("only offers Home Assistant media players", async () => {
    const user = userEvent.setup();
    renderForm();
    await user.click(screen.getByRole("combobox"));
    expect(await screen.findByText("Kitchen zone")).toBeInTheDocument();
    expect(screen.queryByText("Kitchen light")).not.toBeInTheDocument();
  });

  it("can't be saved without a name and an entity", async () => {
    const user = userEvent.setup();
    renderForm();
    expect(saveButton()).toBeDisabled();

    await user.type(screen.getByLabelText("Name"), "Kitchen speakers");
    expect(saveButton()).toBeDisabled();

    await user.click(screen.getByRole("combobox"));
    await user.click(await screen.findByText("Kitchen zone"));
    await waitFor(() => expect(saveButton()).toBeEnabled());
  });

  it("saves a standalone media player without source player settings", async () => {
    const user = userEvent.setup();
    const onComplete = vi.fn();
    renderForm({ onComplete });

    expect(screen.getByLabelText(/Standalone device/)).toBeChecked();
    expect(screen.queryByLabelText("Source player attribute")).not.toBeInTheDocument();

    await fillNameAndEntity(user);
    await waitFor(() => expect(saveButton()).toBeEnabled());
    await user.click(saveButton());

    await waitFor(() => expect(onComplete).toHaveBeenCalled());
    expect(sentBody()).toEqual({
      id: null,
      type: "entity",
      entity_type: "media_player",
      controller: "home_assistant",
      room_id: 1,
      entities_page_id: 2,
      room_view_position: 3,
      friendly_name: "Kitchen speakers",
      home_assistant_name: "media_player.kitchen_zone",
      volume_step: 5,
      source_volume_strategy: "none",
      source_entity_attribute: "",
      source_volume_attribute: "",
    });
  });

  it("only asks for the source player details in a multi-room setup", async () => {
    const user = userEvent.setup();
    renderForm();
    expect(screen.queryByText(/The room player says which player feeds it/)).not.toBeInTheDocument();

    await user.click(screen.getByLabelText(/Part of a multi-room audio setup/));

    expect(screen.getByLabelText(/The room player says which player feeds it/)).toBeChecked();
    expect(screen.getByLabelText(/The room player reports the source player and its volume/)).not.toBeChecked();
    expect(screen.getByLabelText("Source player attribute")).toBeInTheDocument();
    expect(screen.queryByLabelText("Source volume attribute")).not.toBeInTheDocument();
  });

  it("saves a room player that says which player feeds it", async () => {
    const user = userEvent.setup();
    const onComplete = vi.fn();
    renderForm({ onComplete });
    await fillNameAndEntity(user);
    await user.click(screen.getByLabelText(/Part of a multi-room audio setup/));

    // The source player attribute is required for a multi-room setup.
    await waitFor(() => expect(saveButton()).toBeDisabled());
    await user.type(screen.getByLabelText("Source player attribute"), "source_entity_id");
    await waitFor(() => expect(saveButton()).toBeEnabled());
    await user.click(saveButton());

    await waitFor(() => expect(onComplete).toHaveBeenCalled());
    expect(sentBody()).toMatchObject({
      source_volume_strategy: "source_entity",
      source_entity_attribute: "source_entity_id",
      source_volume_attribute: "",
    });
  });

  it("saves a room player that reports the source player and its volume", async () => {
    const user = userEvent.setup();
    const onComplete = vi.fn();
    renderForm({ onComplete });
    await fillNameAndEntity(user);
    await user.click(screen.getByLabelText(/Part of a multi-room audio setup/));
    await user.click(screen.getByLabelText(/The room player reports the source player and its volume/));

    await user.type(screen.getByLabelText("Source player attribute"), "source_entity_id");
    // Both attributes are required for this setup.
    await waitFor(() => expect(saveButton()).toBeDisabled());
    await user.type(screen.getByLabelText("Source volume attribute"), "source_volume");
    await waitFor(() => expect(saveButton()).toBeEnabled());
    await user.click(saveButton());

    await waitFor(() => expect(onComplete).toHaveBeenCalled());
    expect(sentBody()).toMatchObject({
      source_volume_strategy: "player_attributes",
      source_entity_attribute: "source_entity_id",
      source_volume_attribute: "source_volume",
    });
  });

  it("doesn't save source player settings after switching back to standalone", async () => {
    const user = userEvent.setup();
    const onComplete = vi.fn();
    renderForm({ onComplete });
    await fillNameAndEntity(user);
    await user.click(screen.getByLabelText(/Part of a multi-room audio setup/));
    await user.click(screen.getByLabelText(/The room player reports the source player and its volume/));
    await user.type(screen.getByLabelText("Source player attribute"), "source_entity_id");
    await user.type(screen.getByLabelText("Source volume attribute"), "source_volume");

    await user.click(screen.getByLabelText(/Standalone device/));
    expect(screen.queryByLabelText("Source player attribute")).not.toBeInTheDocument();
    await waitFor(() => expect(saveButton()).toBeEnabled());
    await user.click(saveButton());

    await waitFor(() => expect(onComplete).toHaveBeenCalled());
    expect(sentBody()).toMatchObject({ source_volume_strategy: "none", source_entity_attribute: "", source_volume_attribute: "" });
  });

  it("suggests attribute names from the selected media player", async () => {
    const user = userEvent.setup();
    renderForm();
    await fillNameAndEntity(user);
    await user.click(screen.getByLabelText(/Part of a multi-room audio setup/));

    const suggestions = Array.from(document.querySelectorAll("#media_player_attribute_names option")).map((option) => option.getAttribute("value"));
    expect(suggestions).toEqual(["friendly_name", "source_entity_id", "source_volume", "volume_level"]);
    expect(screen.getByLabelText("Source player attribute")).toHaveAttribute("list", "media_player_attribute_names");
  });

  it.each([["0"], ["101"], ["2.5"], [""]])("rejects a volume step of %j", async (volume_step) => {
    const user = userEvent.setup();
    renderForm();
    await fillNameAndEntity(user);
    await waitFor(() => expect(saveButton()).toBeEnabled());

    await user.clear(screen.getByLabelText("Volume step (%)"));
    if (volume_step != "") {
      await user.type(screen.getByLabelText("Volume step (%)"), volume_step);
    }

    await waitFor(() => expect(saveButton()).toBeDisabled());
    expect(screen.getByText("Must be a whole number between 1 and 100.")).toBeInTheDocument();
  });

  it("saves a custom volume step", async () => {
    const user = userEvent.setup();
    const onComplete = vi.fn();
    renderForm({ onComplete });
    await fillNameAndEntity(user);
    await user.clear(screen.getByLabelText("Volume step (%)"));
    await user.type(screen.getByLabelText("Volume step (%)"), "10");
    await waitFor(() => expect(saveButton()).toBeEnabled());
    await user.click(saveButton());

    await waitFor(() => expect(onComplete).toHaveBeenCalled());
    expect(sentBody()).toMatchObject({ volume_step: 10 });
  });

  it("loads an existing multi-room media player for editing", async () => {
    const user = userEvent.setup();
    const onComplete = vi.fn();
    useEntitiesPagesStore.setState({
      entities: [
        {
          id: 7,
          friendly_name: "Kitchen speakers",
          type: "entity",
          entity_type: "media_player",
          room_id: 1,
          entities_page_id: 2,
          room_view_position: 3,
          controller: "home_assistant",
          controlled_by_nspanel_main_page: false,
          home_assistant_name: "media_player.kitchen_zone",
          volume_step: 10,
          source_volume_strategy: "player_attributes",
          source_entity_attribute: "source_entity_id",
          source_volume_attribute: "source_volume",
        } as IEntityOrSceneData,
      ],
    });
    renderForm({ id: 7, onComplete });

    await waitFor(() => expect(screen.getByLabelText("Name")).toHaveValue("Kitchen speakers"));
    expect(screen.getByText("Kitchen zone")).toBeInTheDocument();
    expect(screen.getByLabelText("Volume step (%)")).toHaveValue(10);
    expect(screen.getByLabelText(/Part of a multi-room audio setup/)).toBeChecked();
    expect(screen.getByLabelText(/The room player reports the source player and its volume/)).toBeChecked();
    expect(screen.getByLabelText("Source player attribute")).toHaveValue("source_entity_id");
    expect(screen.getByLabelText("Source volume attribute")).toHaveValue("source_volume");

    await waitFor(() => expect(saveButton()).toBeEnabled());
    await user.click(saveButton());
    await waitFor(() => expect(onComplete).toHaveBeenCalled());
    expect(sentBody()).toMatchObject({ id: 7, friendly_name: "Kitchen speakers", volume_step: 10, source_volume_strategy: "player_attributes" });
  });

  it("uses defaults when editing a media player saved before the source settings existed", async () => {
    useEntitiesPagesStore.setState({
      entities: [
        {
          id: 7,
          friendly_name: "Kitchen speakers",
          type: "entity",
          entity_type: "media_player",
          room_id: 1,
          entities_page_id: 2,
          room_view_position: 3,
          controller: "home_assistant",
          controlled_by_nspanel_main_page: false,
          home_assistant_name: "media_player.kitchen_zone",
        } as IEntityOrSceneData,
      ],
    });
    renderForm({ id: 7 });

    await waitFor(() => expect(screen.getByLabelText("Name")).toHaveValue("Kitchen speakers"));
    expect(screen.getByLabelText("Volume step (%)")).toHaveValue(5);
    expect(screen.getByLabelText(/Standalone device/)).toBeChecked();
    await waitFor(() => expect(saveButton()).toBeEnabled());
  });

  it("reports the server's error and stays open when saving fails", async () => {
    const user = userEvent.setup();
    const onComplete = vi.fn();
    respondWith(409, { status: "error", message: "Slot 3 on entities page 2 is already in use." });
    renderForm({ onComplete });
    await fillNameAndEntity(user);
    await waitFor(() => expect(saveButton()).toBeEnabled());
    await user.click(saveButton());

    await waitFor(() => expect(Notify).toHaveBeenCalled());
    expect(vi.mocked(Notify).mock.calls[0][0]).toMatchObject({ level: "error", message: expect.stringContaining("Slot 3 on entities page 2 is already in use.") });
    expect(onComplete).not.toHaveBeenCalled();
  });
});
