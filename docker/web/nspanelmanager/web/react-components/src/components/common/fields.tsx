import { type ReactNode } from "react";
import { Controller, get, useFormContext, useWatch, type FieldValues, type Path } from "react-hook-form";

// Inputs for a react-hook-form form, used inside its <FormProvider>. Get them through typedFields<YourFormValues>()
// so that field names are checked against the form.
//
// A field whose default value is null is one the server has no valid value for (the /rest/v2 APIs report a
// stored value they can't read as null): it is marked so the user enters one.

// The label, the control, and below it the field's error, or a hint when nothing valid is stored.
function Field<T extends FieldValues>({ name, label, hint, children }: { name: Path<T>; label: string; hint?: string; children: ReactNode }) {
  const {
    formState: { errors, defaultValues },
  } = useFormContext<T>();
  const error = get(errors, name)?.message as string | undefined;
  const unreadable = get(defaultValues, name) === null;
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

function useInvalid<T extends FieldValues>(name: Path<T>) {
  const {
    formState: { errors },
  } = useFormContext<T>();
  const invalid = get(errors, name) !== undefined;
  return { "aria-invalid": invalid, "aria-describedby": invalid ? `${name}-error` : undefined };
}

export function TextInput<T extends FieldValues>({ name, label, hint, disabled, button }: { name: Path<T>; label: string; hint?: string; disabled?: boolean; button?: ReactNode }) {
  const { register } = useFormContext<T>();
  return (
    <Field<T> name={name} label={label} hint={hint}>
      <div className="join w-full">
        <input id={name} type="text" className={`${inputClass} join-item`} disabled={disabled} {...useInvalid<T>(name)} {...register(name)} />
        {button}
      </div>
    </Field>
  );
}

// Blank is sent as null, so the server reports it ("This field may not be null") rather than the UI guessing.
const toNumber = (value: unknown) => (value === "" || value === null || value === undefined ? null : Number(value));

export function NumberInput<T extends FieldValues>({ name, label, hint, button }: { name: Path<T>; label: string; hint?: string; button?: ReactNode }) {
  const { register } = useFormContext<T>();
  return (
    <Field<T> name={name} label={label} hint={hint}>
      <div className="join w-full">
        <input id={name} type="number" step={1} className={`${inputClass} join-item`} {...useInvalid<T>(name)} {...register(name, { setValueAs: toNumber })} />
        {button}
      </div>
    </Field>
  );
}

export function Select<T extends FieldValues>({ name, label, options }: { name: Path<T>; label: string; options: [string, string][] }) {
  const { register } = useFormContext<T>();
  return (
    <Field<T> name={name} label={label}>
      <select id={name} className="select w-full" {...useInvalid<T>(name)} {...register(name)}>
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

export function RadioGroup<T extends FieldValues, V extends string | boolean = string | boolean>({ name, label, options }: { name: Path<T>; label: string; options: [V, string][] }) {
  const { control } = useFormContext<T>();
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
          <FieldMessage<T> name={name} />
        </fieldset>
      )}
    />
  );
}

function FieldMessage<T extends FieldValues>({ name }: { name: Path<T> }) {
  const {
    formState: { errors, defaultValues },
  } = useFormContext<T>();
  const error = get(errors, name)?.message as string | undefined;
  if (error) {
    return <p className="mt-1 text-sm text-error">{error}</p>;
  }
  if (get(defaultValues, name) === null) {
    return <p className="mt-1 text-sm text-warning">No valid value is stored. Choose one.</p>;
  }
  return null;
}

// A secret is never sent to the browser. Leave it blank to keep the stored one, type a new one to
// replace it, or tick "Clear" (the boolean field clearName) to remove it.
export function SecretInput<T extends FieldValues>({ name, clearName, label, isSet, disabled, button }: { name: Path<T>; clearName: Path<T>; label: string; isSet: boolean; disabled?: boolean; button?: ReactNode }) {
  const { register, control } = useFormContext<T>();
  const clear = useWatch({ control, name: clearName });
  return (
    <Field<T> name={name} label={label} hint={isSet && !disabled ? "Stored, and not shown. Leave blank to keep it." : undefined}>
      <div className="join w-full">
        <input
          id={name}
          type="password"
          autoComplete="new-password"
          className={`${inputClass} join-item`}
          placeholder={isSet ? "••••••••" : ""}
          disabled={disabled || clear}
          {...useInvalid<T>(name)}
          {...register(name)}
        />
        {button}
      </div>
      {isSet && !disabled && (
        <label className="label mt-1 text-sm">
          <input type="checkbox" className="checkbox checkbox-accent checkbox-sm" {...register(clearName)} />
          Clear the stored {label.toLowerCase()}
        </label>
      )}
    </Field>
  );
}
