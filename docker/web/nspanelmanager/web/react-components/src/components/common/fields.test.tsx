import { render, screen } from "@testing-library/react";
import userEvent from "@testing-library/user-event";
import { useEffect, type ReactNode } from "react";
import { FormProvider, useForm } from "react-hook-form";
import { describe, expect, it, vi } from "vitest";
import { typedFields } from "./typedFields";

type Values = { name: string | null; count: number | null; mode: string | null; enabled: boolean | null; secret: string; clear_secret: boolean };

const { NumberInput, RadioGroup, SecretInput, Select, TextInput } = typedFields<Values>();

const defaults: Values = { name: "a", count: 1, mode: "x", enabled: true, secret: "", clear_secret: false };

// A form around the fields, which reports the values it submits and can start with a field error.
function Form({ defaultValues = defaults, error, onSubmit = () => {}, children }: { defaultValues?: Values; error?: [keyof Values, string]; onSubmit?: (values: Values) => void; children: ReactNode }) {
  const form = useForm<Values>({ defaultValues });
  const { setError } = form;
  useEffect(() => {
    if (error) {
      setError(error[0], { type: "server", message: error[1] });
    }
  }, [error, setError]);
  return (
    <FormProvider {...form}>
      <form onSubmit={form.handleSubmit(onSubmit)}>
        {children}
        <button type="submit">Submit</button>
      </form>
    </FormProvider>
  );
}

describe("field errors", () => {
  it("shows the error under the field and links it to the input", async () => {
    render(
      <Form error={["name", "Too long."]}>
        <TextInput name="name" label="Name" hint="A hint." />
      </Form>,
    );
    const input = await screen.findByLabelText("Name");
    expect(await screen.findByText("Too long.")).toBeInTheDocument();
    expect(input).toHaveAttribute("aria-invalid", "true");
    expect(input).toHaveAttribute("aria-describedby", "name-error");
    expect(screen.queryByText("A hint.")).not.toBeInTheDocument();
  });

  it("shows the hint when there is no error", () => {
    render(
      <Form>
        <TextInput name="name" label="Name" hint="A hint." />
      </Form>,
    );
    expect(screen.getByLabelText("Name")).toHaveAttribute("aria-invalid", "false");
    expect(screen.getByText("A hint.")).toBeInTheDocument();
  });
});

describe("a field with no valid stored value (a null default)", () => {
  it("asks for a value", () => {
    render(
      <Form defaultValues={{ ...defaults, name: null, count: null, mode: null, enabled: null }}>
        <TextInput name="name" label="Name" />
        <NumberInput name="count" label="Count" />
        <Select name="mode" label="Mode" options={[["x", "X"]]} />
        <RadioGroup name="enabled" label="Enabled" options={[[true, "On"], [false, "Off"]]} />
      </Form>,
    );
    expect(screen.getAllByText("No valid value is stored. Enter one.")).toHaveLength(3);
    expect(screen.getByText("No valid value is stored. Choose one.")).toBeInTheDocument();
  });
});

describe("NumberInput", () => {
  it("submits a number, or null when blank", async () => {
    const user = userEvent.setup();
    const onSubmit = vi.fn();
    render(
      <Form onSubmit={onSubmit}>
        <NumberInput name="count" label="Count" />
      </Form>,
    );
    const input = screen.getByLabelText("Count");
    await user.clear(input);
    await user.type(input, "42");
    await user.click(screen.getByRole("button", { name: "Submit" }));
    expect(onSubmit.mock.calls[0][0].count).toBe(42);

    await user.clear(input);
    await user.click(screen.getByRole("button", { name: "Submit" }));
    expect(onSubmit.mock.calls[1][0].count).toBeNull();
  });
});

describe("RadioGroup", () => {
  it("submits the chosen option's value", async () => {
    const user = userEvent.setup();
    const onSubmit = vi.fn();
    render(
      <Form onSubmit={onSubmit}>
        <RadioGroup name="enabled" label="Enabled" options={[[true, "On"], [false, "Off"]]} />
      </Form>,
    );
    expect(screen.getByLabelText("On")).toBeChecked();
    await user.click(screen.getByLabelText("Off"));
    await user.click(screen.getByRole("button", { name: "Submit" }));
    expect(onSubmit.mock.calls[0][0].enabled).toBe(false);
  });
});

describe("SecretInput", () => {
  it("offers no clear checkbox when nothing is stored", () => {
    render(
      <Form>
        <SecretInput name="secret" clearName="clear_secret" label="Token" isSet={false} />
      </Form>,
    );
    expect(screen.getByLabelText("Token")).toHaveAttribute("type", "password");
    expect(screen.queryByRole("checkbox")).not.toBeInTheDocument();
    expect(screen.queryByText(/Stored, and not shown/)).not.toBeInTheDocument();
  });

  it("says a stored secret is kept, and disables the input once Clear is ticked", async () => {
    const user = userEvent.setup();
    const onSubmit = vi.fn();
    render(
      <Form onSubmit={onSubmit}>
        <SecretInput name="secret" clearName="clear_secret" label="Token" isSet />
      </Form>,
    );
    expect(screen.getByText("Stored, and not shown. Leave blank to keep it.")).toBeInTheDocument();
    expect(screen.getByLabelText("Token")).toBeEnabled();
    await user.click(screen.getByRole("checkbox", { name: "Clear the stored token" }));
    expect(screen.getByLabelText("Token")).toBeDisabled();
    await user.click(screen.getByRole("button", { name: "Submit" }));
    expect(onSubmit.mock.calls[0][0].clear_secret).toBe(true);
  });

  it("offers no clear checkbox when disabled", () => {
    render(
      <Form>
        <SecretInput name="secret" clearName="clear_secret" label="Token" isSet disabled />
      </Form>,
    );
    expect(screen.getByLabelText("Token")).toBeDisabled();
    expect(screen.queryByRole("checkbox")).not.toBeInTheDocument();
  });
});
