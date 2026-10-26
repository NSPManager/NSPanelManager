import { render, screen, waitFor } from "@testing-library/react";
import userEvent from "@testing-library/user-event";
import { afterEach, beforeEach, describe, expect, it, vi } from "vitest";
import MultiStep_AddEditEntity_Step3_Cover from "./step3_edit_cover";
import { useAvailableEntitiesStore } from "../../../stores/AvailableEntitiesStore";
import { useEntitiesPagesStore, type IEntityOrSceneData } from "../../../stores/EntitiesPagesStore";
import { Notify } from "../../NSPanelToastContainer";

vi.mock("../../NSPanelToastContainer", () => ({ Notify: vi.fn() }));

const home_assistant_entities = [
  {
    type: "home_assistant",
    label: "Living room blind",
    item_id: "cover.living_room_blind",
    item: {
      entity_id: "cover.living_room_blind",
      attributes: { friendly_name: "Living room blind", supported_features: 15, device_class: "blind" },
    },
  },
  {
    type: "home_assistant",
    label: "Kitchen light",
    item_id: "light.kitchen",
    item: { entity_id: "light.kitchen", attributes: { friendly_name: "Kitchen light" } },
  },
];

const openhab_items = [
  {
    type: "openhab",
    openhab_type: "item",
    label: "Living room shutter",
    item_id: "LivingRoom_Shutter",
    item: { name: "LivingRoom_Shutter", type: "Rollershutter", state: "0" },
  },
  {
    type: "openhab",
    openhab_type: "item",
    label: "Kitchen light",
    item_id: "Kitchen_Light",
    item: { name: "Kitchen_Light", type: "Switch", state: "OFF" },
  },
];

const fetchMock = vi.fn();

function respondWith(status: number, body: object) {
  fetchMock.mockResolvedValue({ ok: status >= 200 && status < 300, status: status, json: async () => body });
}

function renderForm(props: { controller?: string; id?: number; onComplete?: () => void } = {}) {
  return render(
    <MultiStep_AddEditEntity_Step3_Cover
      controller={props.controller ?? "home_assistant"}
      room_id={1}
      entities_page_id={2}
      room_view_position={3}
      id={props.id}
      onComplete={props.onComplete}
    />,
  );
}

function sentBody() {
  expect(fetchMock).toHaveBeenCalledTimes(1);
  const [url, options] = fetchMock.mock.calls[0];
  expect(url).toBe("/rest/entities/covers");
  expect(options.method).toBe("PUT");
  return JSON.parse(options.body);
}

const saveButton = () => screen.getByRole("button", { name: "Save" });

beforeEach(() => {
  vi.stubGlobal("fetch", fetchMock);
  respondWith(200, { status: "ok" });
  useAvailableEntitiesStore.setState({
    status: "loaded",
    home_assistant_entities: home_assistant_entities,
    home_assistant_options: home_assistant_entities.map((entity) => ({ value: entity.item_id, label: entity.label, icon: "mdi-window-shutter", type: entity.type })),
    openhab_items: openhab_items,
    openhab_options: openhab_items.map((item) => ({ value: item.item_id, label: item.label, icon: "mdi-window-shutter", type: item.type })),
  });
  useEntitiesPagesStore.setState({ entities: [], entities_pages: [], scenes: [] });
});

afterEach(() => {
  fetchMock.mockReset();
  vi.mocked(Notify).mockClear();
  vi.unstubAllGlobals();
});

describe("cover form", () => {
  it("shows the beta banner", () => {
    renderForm();
    expect(screen.getByText(/Covers are a beta feature/)).toBeInTheDocument();
  });

  it("only offers Home Assistant cover entities", async () => {
    const user = userEvent.setup();
    renderForm();
    await user.click(screen.getByRole("combobox"));

    expect(await screen.findByText("Living room blind")).toBeInTheDocument();
    expect(screen.queryByText("Kitchen light")).not.toBeInTheDocument();
  });

  it("can't be saved without a name and an entity", async () => {
    const user = userEvent.setup();
    renderForm();
    expect(saveButton()).toBeDisabled();

    await user.type(screen.getByLabelText("Name"), "Living room blind");
    expect(saveButton()).toBeDisabled();

    await user.click(screen.getByRole("combobox"));
    await user.click(await screen.findByText("Living room blind"));
    await waitFor(() => expect(saveButton()).toBeEnabled());
  });

  it("shows the detected capabilities", async () => {
    const user = userEvent.setup();
    renderForm();

    await user.click(screen.getByRole("combobox"));
    await user.click(await screen.findByText("Living room blind"));

    expect(screen.getByText(/Detected: position/)).toBeInTheDocument();
  });

  it("saves a Home Assistant cover", async () => {
    const user = userEvent.setup();
    const onComplete = vi.fn();
    renderForm({ onComplete });

    await user.type(screen.getByLabelText("Name"), "Living room blind");
    await user.click(screen.getByRole("combobox"));
    await user.click(await screen.findByText("Living room blind"));
    await waitFor(() => expect(saveButton()).toBeEnabled());
    await user.click(saveButton());

    await waitFor(() => expect(onComplete).toHaveBeenCalled());
    expect(sentBody()).toEqual({
      id: null,
      type: "entity",
      entity_type: "cover",
      controller: "home_assistant",
      room_id: 1,
      entities_page_id: 2,
      room_view_position: 3,
      friendly_name: "Living room blind",
      home_assistant_name: "cover.living_room_blind",
      openhab_item: "",
    });
  });

  it("only offers Rollershutter items for OpenHAB and saves the item", async () => {
    const user = userEvent.setup();
    const onComplete = vi.fn();
    renderForm({ controller: "openhab", onComplete });

    await user.type(screen.getByLabelText("Name"), "Living room shutter");
    await user.click(screen.getByRole("combobox"));
    expect(await screen.findByText("Living room shutter")).toBeInTheDocument();
    expect(screen.queryByText("Kitchen light")).not.toBeInTheDocument();
    await user.click(screen.getByText("Living room shutter"));
    await waitFor(() => expect(saveButton()).toBeEnabled());
    await user.click(saveButton());

    await waitFor(() => expect(onComplete).toHaveBeenCalled());
    expect(sentBody()).toMatchObject({
      controller: "openhab",
      home_assistant_name: "",
      openhab_item: "LivingRoom_Shutter",
    });
  });

  it("opens an existing cover on its form", async () => {
    useEntitiesPagesStore.setState({
      entities: [
        {
          id: 7,
          friendly_name: "Living room blind",
          type: "entity",
          entity_type: "cover",
          room_id: 1,
          entities_page_id: 2,
          room_view_position: 3,
          controller: "home_assistant",
          home_assistant_name: "cover.living_room_blind",
          openhab_item: "",
          controlled_by_nspanel_main_page: false,
        } as IEntityOrSceneData,
      ],
    });
    renderForm({ id: 7 });

    expect(await screen.findByDisplayValue("Living room blind")).toBeInTheDocument();
    expect(screen.queryByText("Cover / blind")).not.toBeInTheDocument();
  });

  it("shows a notification when saving fails", async () => {
    respondWith(400, { status: "error", message: "Slot is taken" });
    const user = userEvent.setup();
    renderForm();

    await user.type(screen.getByLabelText("Name"), "Living room blind");
    await user.click(screen.getByRole("combobox"));
    await user.click(await screen.findByText("Living room blind"));
    await waitFor(() => expect(saveButton()).toBeEnabled());
    await user.click(saveButton());

    await waitFor(() => expect(Notify).toHaveBeenCalledWith(expect.objectContaining({ message: expect.stringContaining("Slot is taken") })));
  });
});
