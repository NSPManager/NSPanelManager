import { render, screen } from "@testing-library/react";
import userEvent from "@testing-library/user-event";
import { beforeEach, describe, expect, it, vi } from "vitest";
import MultiStep_AddOrEditEntity from "./MultiStep_AddEditEntity";
import { useAvailableEntitiesStore } from "../../stores/AvailableEntitiesStore";
import { useEntitiesPagesStore, type IEntityOrSceneData } from "../../stores/EntitiesPagesStore";
import { useSettingsStore } from "../../stores/SettingsStore";

vi.mock("../NSPanelToastContainer", () => ({ Notify: vi.fn() }));

type Settings = NonNullable<ReturnType<typeof useSettingsStore.getState>["settings"]>;

function renderWizard(id?: number) {
  return render(
    <MultiStep_AddOrEditEntity room_id={1} entities_page_id={2} room_view_position={3} type="entity" id={id} opened={true} setOpened={() => {}} />,
  );
}

beforeEach(() => {
  // Both Home Assistant and OpenHAB are configured, so the controller step is limited only by the entity type.
  useSettingsStore.setState({ status: "loaded", settings: { home_assistant_token_set: true, openhab_token_set: true } as Settings });
  useAvailableEntitiesStore.setState({ status: "loaded", home_assistant_entities: [], home_assistant_options: [], openhab_items: [], openhab_options: [] });
  useEntitiesPagesStore.setState({ entities_pages: [{ id: 2, display_order: 0, number_of_entities: 8, type: "entity", room_id: 1 }], entities: [], scenes: [] });
});

describe("add or edit entity wizard", () => {
  it("only offers Home Assistant as the controller for a media player", async () => {
    const user = userEvent.setup();
    renderWizard();

    await user.click(screen.getByText("Media player"));

    expect(await screen.findByText("Home Assistant")).toBeInTheDocument();
    expect(screen.queryByText("OpenHAB")).not.toBeInTheDocument();
    expect(screen.queryByText("NSPanel Manager")).not.toBeInTheDocument();
  });

  it("offers OpenHAB for other entity types when it is configured", async () => {
    // Guards the test above: OpenHAB missing for media players must come from the entity type, not the settings.
    const user = userEvent.setup();
    renderWizard();

    await user.click(screen.getByText("Light"));

    expect(await screen.findByText("OpenHAB")).toBeInTheDocument();
  });

  it("goes from the media player type to the media player form", async () => {
    const user = userEvent.setup();
    renderWizard();

    await user.click(screen.getByText("Media player"));
    await user.click(await screen.findByText("Home Assistant"));

    expect(await screen.findByText(/Media players are a beta feature/)).toBeInTheDocument();
    expect(screen.getByLabelText(/Standalone device/)).toBeChecked();
  });

  it("opens an existing media player straight on its form", async () => {
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
          volume_step: 5,
          source_volume_strategy: "none",
          source_entity_attribute: "",
          source_volume_attribute: "",
        } as IEntityOrSceneData,
      ],
    });
    renderWizard(7);

    expect(await screen.findByText(/Media players are a beta feature/)).toBeInTheDocument();
    expect(await screen.findByDisplayValue("Kitchen speakers")).toBeInTheDocument();
    expect(screen.queryByText("Media player")).not.toBeInTheDocument();
  });
});
