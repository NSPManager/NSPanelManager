import { render, screen, waitFor } from "@testing-library/react";
import userEvent from "@testing-library/user-event";
import { afterEach, beforeEach, describe, expect, it, vi } from "vitest";
import { CustomIcon, CustomIconPicker } from "./CustomIcons";
import { isCustomIcon, useCustomIconsStore } from "../../stores/CustomIconsStore";

vi.mock("../NSPanelToastContainer", () => ({ Notify: vi.fn() }));

const LAMP = String.fromCharCode(0xe000);

describe("custom icons", () => {
  beforeEach(() => {
    useCustomIconsStore.setState({ icons: [{ name: "lamp", code_point: 0xe000, character: LAMP }], available: ["lamp", "fan", "ceiling-light"] });
    // jsdom has no showModal()
    HTMLDialogElement.prototype.showModal = vi.fn(function (this: HTMLDialogElement) {
      this.setAttribute("open", "");
    });
    HTMLDialogElement.prototype.close = vi.fn(function (this: HTMLDialogElement) {
      this.removeAttribute("open");
      this.dispatchEvent(new Event("close")); // As browsers do
    });
  });

  afterEach(() => {
    vi.restoreAllMocks();
  });

  it("tells custom icon characters from built-in ones", () => {
    expect(isCustomIcon(LAMP)).toBe(true);
    expect(isCustomIcon("s")).toBe(false);
    expect(isCustomIcon("")).toBe(false);
    expect(isCustomIcon(undefined)).toBe(false);
  });

  it("draws a custom icon as its Material Design Icon", () => {
    const { container } = render(<CustomIcon character={LAMP} />);

    expect(container.querySelector(".mdi-lamp")).not.toBeNull();
  });

  it("draws nothing for a built-in icon", () => {
    const { container } = render(<CustomIcon character="s" />);

    expect(container.innerHTML).toBe("");
  });

  it("offers the default icon until one is chosen", () => {
    render(<CustomIconPicker value="" onChange={vi.fn()} />);

    expect(screen.getByText("Default icon")).toBeInTheDocument();
    expect(screen.queryByText("Use default icon")).not.toBeInTheDocument();
  });

  it("shows the default icon when it is given", () => {
    const { container } = render(<CustomIconPicker value="" onChange={vi.fn()} defaultIcon="toggle-switch-off" />);

    expect(container.querySelector(".mdi-toggle-switch-off")).not.toBeNull();
    expect(screen.getByText("Default")).toBeInTheDocument();
  });

  it("registers the chosen icon and stores its character", async () => {
    const fan = String.fromCharCode(0xe001);
    const fetchMock = vi.fn().mockResolvedValue({ ok: true, json: async () => ({ name: "fan", code_point: 0xe001, character: fan }) });
    vi.stubGlobal("fetch", fetchMock);
    const onChange = vi.fn();
    render(<CustomIconPicker value="" onChange={onChange} />);

    await userEvent.click(screen.getByTitle("Choose icon"));
    await userEvent.type(screen.getByPlaceholderText(/Search icons/), "fan");
    await userEvent.click(screen.getByTitle("fan"));

    await waitFor(() => expect(onChange).toHaveBeenCalledWith(fan));
    expect(JSON.parse(fetchMock.mock.calls[0][1].body)).toEqual({ name: "fan" });
  });

  it("does not close the dialog it is opened from when an icon is chosen", async () => {
    const fan = String.fromCharCode(0xe001);
    vi.stubGlobal("fetch", vi.fn().mockResolvedValue({ ok: true, json: async () => ({ name: "fan", code_point: 0xe001, character: fan }) }));
    const onChange = vi.fn();
    const editorClosed = vi.fn();
    render(
      <dialog open onClose={editorClosed}>
        <CustomIconPicker value="" onChange={onChange} />
      </dialog>,
    );

    await userEvent.click(screen.getByTitle("Choose icon"));
    await userEvent.click(screen.getByTitle("fan"));

    await waitFor(() => expect(onChange).toHaveBeenCalledWith(fan));
    expect(editorClosed).not.toHaveBeenCalled();
  });

  it("shows each icon's name", async () => {
    render(<CustomIconPicker value="" onChange={vi.fn()} />);

    await userEvent.click(screen.getByTitle("Choose icon"));

    expect(screen.getByTitle("ceiling-light")).toHaveTextContent("ceiling-light");
  });

  it("can go back to the default icon", async () => {
    const onChange = vi.fn();
    render(<CustomIconPicker value={LAMP} onChange={onChange} />);

    await userEvent.click(screen.getByText("Use default icon"));

    expect(onChange).toHaveBeenCalledWith("");
  });
});
