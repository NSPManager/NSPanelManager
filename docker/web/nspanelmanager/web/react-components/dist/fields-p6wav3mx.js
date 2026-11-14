import { r as require_react, t as require_jsx_runtime } from "./main-2GKhVwa3.js";
import { c as useFormContext, i as get, l as useWatch, t as Controller } from "./index.esm-CyxbWRtN.js";
require_react();
var import_jsx_runtime = require_jsx_runtime();
function Field({ name, label, hint, children }) {
	const { formState: { errors, defaultValues } } = useFormContext();
	const error = get(errors, name)?.message;
	const unreadable = get(defaultValues, name) === null;
	return /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
		className: "mb-2 w-full",
		children: [
			/* @__PURE__ */ (0, import_jsx_runtime.jsx)("label", {
				htmlFor: name,
				className: "block mb-1 text-sm font-medium",
				children: label
			}),
			children,
			error ? /* @__PURE__ */ (0, import_jsx_runtime.jsx)("p", {
				className: "mt-1 text-sm text-error",
				id: `${name}-error`,
				children: error
			}) : unreadable ? /* @__PURE__ */ (0, import_jsx_runtime.jsx)("p", {
				className: "mt-1 text-sm text-warning",
				children: "No valid value is stored. Enter one."
			}) : hint ? /* @__PURE__ */ (0, import_jsx_runtime.jsx)("p", {
				className: "mt-1 text-sm opacity-60",
				children: hint
			}) : null
		]
	});
}
var inputClass = "input w-full";
function useInvalid(name) {
	const { formState: { errors } } = useFormContext();
	const invalid = get(errors, name) !== void 0;
	return {
		"aria-invalid": invalid,
		"aria-describedby": invalid ? `${name}-error` : void 0
	};
}
function TextInput({ name, label, hint, disabled, button }) {
	const { register } = useFormContext();
	return /* @__PURE__ */ (0, import_jsx_runtime.jsx)(Field, {
		name,
		label,
		hint,
		children: /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
			className: "join w-full",
			children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("input", {
				id: name,
				type: "text",
				className: `${inputClass} join-item`,
				disabled,
				...useInvalid(name),
				...register(name)
			}), button]
		})
	});
}
var toNumber = (value) => value === "" || value === null || value === void 0 ? null : Number(value);
function NumberInput({ name, label, hint, button }) {
	const { register } = useFormContext();
	return /* @__PURE__ */ (0, import_jsx_runtime.jsx)(Field, {
		name,
		label,
		hint,
		children: /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
			className: "join w-full",
			children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("input", {
				id: name,
				type: "number",
				step: 1,
				className: `${inputClass} join-item`,
				...useInvalid(name),
				...register(name, { setValueAs: toNumber })
			}), button]
		})
	});
}
function Select({ name, label, options }) {
	const { register } = useFormContext();
	return /* @__PURE__ */ (0, import_jsx_runtime.jsx)(Field, {
		name,
		label,
		children: /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("select", {
			id: name,
			className: "select w-full",
			...useInvalid(name),
			...register(name),
			children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("option", {
				value: "",
				disabled: true,
				children: "Choose…"
			}), options.map(([value, text]) => /* @__PURE__ */ (0, import_jsx_runtime.jsx)("option", {
				value,
				children: text
			}, value))]
		})
	});
}
function RadioGroup({ name, label, options }) {
	const { control } = useFormContext();
	return /* @__PURE__ */ (0, import_jsx_runtime.jsx)(Controller, {
		control,
		name,
		render: ({ field }) => /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("fieldset", {
			className: "mb-2 w-full",
			"aria-label": label,
			children: [
				/* @__PURE__ */ (0, import_jsx_runtime.jsx)("legend", {
					className: "block mb-1 text-sm font-medium",
					children: label
				}),
				/* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
					className: "flex flex-wrap border border-neutral rounded-lg",
					children: options.map(([value, text]) => /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("label", {
						className: "flex items-center gap-2 flex-1 px-3 py-3 text-sm font-medium cursor-pointer",
						children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("input", {
							type: "radio",
							className: "radio radio-accent radio-sm",
							name,
							checked: field.value === value,
							onChange: () => field.onChange(value),
							onBlur: field.onBlur
						}), text]
					}, String(value)))
				}),
				/* @__PURE__ */ (0, import_jsx_runtime.jsx)(FieldMessage, { name })
			]
		})
	});
}
function FieldMessage({ name }) {
	const { formState: { errors, defaultValues } } = useFormContext();
	const error = get(errors, name)?.message;
	if (error) return /* @__PURE__ */ (0, import_jsx_runtime.jsx)("p", {
		className: "mt-1 text-sm text-error",
		children: error
	});
	if (get(defaultValues, name) === null) return /* @__PURE__ */ (0, import_jsx_runtime.jsx)("p", {
		className: "mt-1 text-sm text-warning",
		children: "No valid value is stored. Choose one."
	});
	return null;
}
function SecretInput({ name, clearName, label, isSet, disabled, button }) {
	const { register, control } = useFormContext();
	const clear = useWatch({
		control,
		name: clearName
	});
	return /* @__PURE__ */ (0, import_jsx_runtime.jsxs)(Field, {
		name,
		label,
		hint: isSet && !disabled ? "Stored, and not shown. Leave blank to keep it." : void 0,
		children: [/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
			className: "join w-full",
			children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("input", {
				id: name,
				type: "password",
				autoComplete: "new-password",
				className: `${inputClass} join-item`,
				placeholder: isSet ? "••••••••" : "",
				disabled: disabled || clear,
				...useInvalid(name),
				...register(name)
			}), button]
		}), isSet && !disabled && /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("label", {
			className: "label mt-1 text-sm",
			children: [
				/* @__PURE__ */ (0, import_jsx_runtime.jsx)("input", {
					type: "checkbox",
					className: "checkbox checkbox-accent checkbox-sm",
					...register(clearName)
				}),
				"Clear the stored ",
				label.toLowerCase()
			]
		})]
	});
}
//#endregion
export { TextInput as a, Select as i, RadioGroup as n, SecretInput as r, NumberInput as t };
