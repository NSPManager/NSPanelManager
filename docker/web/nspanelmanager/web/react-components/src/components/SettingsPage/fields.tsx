import { type ReactNode } from "react";
import { Controller, get, useFormContext, useWatch } from "react-hook-form";
import type { SettingsFormValues } from "./form";

type Name = keyof SettingsFormValues & string;

export function Card({ title, actions, children }: { title: string; actions?: ReactNode; children: ReactNode }) {
  return (
    <section className="p-4 mt-2 bg-base-100 rounded-box shadow-lg w-full" aria-label={title}>
      <div className="flex items-center justify-between mb-2">
        <h3 className="text-xl">{title}</h3>
        {actions}
      </div>
      {children}
    </section>
  );
}

export function Row({ children }: { children: ReactNode }) {
  return <div className="flex flex-wrap md:flex-nowrap gap-2">{children}</div>;
}

export function Alert({ level, children }: { level: "info" | "warning" | "error"; children: ReactNode }) {
  const className = { info: "alert-info", warning: "alert-warning", error: "alert-error" }[level];
  return (
    <div role={level === "error" ? "alert" : "status"} className={`alert ${className} my-2`}>
      <span>{children}</span>
    </div>
  );
}

// The label, the control, and below it the field's error, or a hint when nothing valid is stored.
function Field({ name, label, hint, children }: { name: Name; label: string; hint?: string; children: ReactNode }) {
  const {
    formState: { errors, defaultValues },
  } = useFormContext<SettingsFormValues>();
  const error = get(errors, name)?.message as string | undefined;
  const unreadable = defaultValues?.[name] === null;
  return (
    <div className="mb-2 w-full">
      <label htmlFor={name} className="block mb-1 text-sm font-medium">
        {label}
      </label>
      {children}
      {error ? (
        <p className="mt-1 text-sm text-error" id={`${name}-error`}>
          {error}
        </p>
      ) : unreadable ? (
        <p className="mt-1 text-sm text-warning">No valid value is stored. Enter one.</p>
      ) : hint ? (
        <p className="mt-1 text-sm opacity-60">{hint}</p>
      ) : null}
    </div>
  );
}

const inputClass = "input w-full";

function useInvalid(name: Name) {
  const {
    formState: { errors },
  } = useFormContext<SettingsFormValues>();
  const invalid = get(errors, name) !== undefined;
  return { "aria-invalid": invalid, "aria-describedby": invalid ? `${name}-error` : undefined };
}

export function TextInput({ name, label, hint, disabled, button }: { name: Name; label: string; hint?: string; disabled?: boolean; button?: ReactNode }) {
  const { register } = useFormContext<SettingsFormValues>();
  return (
    <Field name={name} label={label} hint={hint}>
      <div className="join w-full">
        <input id={name} type="text" className={`${inputClass} join-item`} disabled={disabled} {...useInvalid(name)} {...register(name)} />
        {button}
      </div>
    </Field>
  );
}

// Blank is sent as null, so the server reports it ("This field may not be null") rather than the UI guessing.
const toNumber = (value: unknown) => (value === "" || value === null || value === undefined ? null : Number(value));

export function NumberInput({ name, label, hint, button }: { name: Name; label: string; hint?: string; button?: ReactNode }) {
  const { register } = useFormContext<SettingsFormValues>();
  return (
    <Field name={name} label={label} hint={hint}>
      <div className="join w-full">
        <input id={name} type="number" step={1} className={`${inputClass} join-item`} {...useInvalid(name)} {...register(name, { setValueAs: toNumber })} />
        {button}
      </div>
    </Field>
  );
}

export function Select({ name, label, options }: { name: Name; label: string; options: [string, string][] }) {
  const { register } = useFormContext<SettingsFormValues>();
  return (
    <Field name={name} label={label}>
      <select id={name} className="select w-full" {...useInvalid(name)} {...register(name)}>
        <option value="" disabled>
          Choose…
        </option>
        {options.map(([value, text]) => (
          <option key={value} value={value}>
            {text}
          </option>
        ))}
      </select>
    </Field>
  );
}

export function RadioGroup<T extends string | boolean>({ name, label, options }: { name: Name; label: string; options: [T, string][] }) {
  const { control } = useFormContext<SettingsFormValues>();
  return (
    <Controller
      control={control}
      name={name}
      render={({ field }) => (
        <fieldset className="mb-2 w-full" aria-label={label}>
          <legend className="block mb-1 text-sm font-medium">{label}</legend>
          <div className="flex flex-wrap border border-neutral rounded-lg">
            {options.map(([value, text]) => (
              <label key={String(value)} className="flex items-center gap-2 flex-1 px-3 py-3 text-sm font-medium cursor-pointer">
                <input type="radio" className="radio radio-accent radio-sm" name={name} checked={field.value === value} onChange={() => field.onChange(value)} onBlur={field.onBlur} />
                {text}
              </label>
            ))}
          </div>
          <FieldMessage name={name} />
        </fieldset>
      )}
    />
  );
}

function FieldMessage({ name }: { name: Name }) {
  const {
    formState: { errors, defaultValues },
  } = useFormContext<SettingsFormValues>();
  const error = get(errors, name)?.message as string | undefined;
  if (error) {
    return <p className="mt-1 text-sm text-error">{error}</p>;
  }
  if (defaultValues?.[name] === null) {
    return <p className="mt-1 text-sm text-warning">No valid value is stored. Choose one.</p>;
  }
  return null;
}

// A secret is never sent to the browser. Leave it blank to keep the stored one, type a new one to
// replace it, or tick "Clear" to remove it.
export function SecretInput({ name, label, isSet, disabled, button }: { name: "mqtt_password" | "home_assistant_token" | "openhab_token"; label: string; isSet: boolean; disabled?: boolean; button?: ReactNode }) {
  const { register, control } = useFormContext<SettingsFormValues>();
  const clear = useWatch({ control, name: `clear_${name}` });
  return (
    <Field name={name} label={label} hint={isSet && !disabled ? "Stored, and not shown. Leave blank to keep it." : undefined}>
      <div className="join w-full">
        <input
          id={name}
          type="password"
          autoComplete="new-password"
          className={`${inputClass} join-item`}
          placeholder={isSet ? "••••••••" : ""}
          disabled={disabled || clear}
          {...useInvalid(name)}
          {...register(name)}
        />
        {button}
      </div>
      {isSet && !disabled && (
        <label className="label mt-1 text-sm">
          <input type="checkbox" className="checkbox checkbox-accent checkbox-sm" {...register(`clear_${name}`)} />
          Clear the stored {label.toLowerCase()}
        </label>
      )}
    </Field>
  );
}
