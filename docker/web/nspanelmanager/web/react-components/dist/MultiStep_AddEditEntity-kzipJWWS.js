import { c as __toESM, o as __exportAll, r as require_react, t as require_jsx_runtime } from "./main-BWgDajaM.js";
import { t as create } from "./react-BrBB-CGW.js";
import { t as useSettingsStore } from "./SettingsStore-D3WfdsO1.js";
import { t as StateManagedSelect$1 } from "./react-select.esm-BBLHA-jB.js";
import { n as Notify } from "./NSPanelToastContainer-Abzvd6TK.js";
import { t as motion } from "./proxy-BZ7OE7_K.js";
import { t as AnimatePresence } from "./AnimatePresence-BSe8g7GZ.js";
//#region src/stores/EntitiesPagesStore.ts
function getCookie$1(name) {
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
var useEntitiesPagesStore = create((set) => ({
	status: "loading",
	room_id: null,
	entities_pages: [],
	entities: [],
	scenes: [],
	fetchData: async (room_id) => {
		set({
			status: "loading",
			room_id
		});
		let url = `/rest/global/entities_pages`;
		if (room_id !== void 0) url = `/rest/rooms/${room_id}/entities_pages`;
		fetch(url, {
			credentials: "same-origin",
			method: "GET",
			mode: "same-origin",
			headers: {
				"Content-Type": "application/json",
				"X-CSRFToken": getCookie$1("csrftoken")
			}
		}).then((response) => response.json()).then((data) => {
			set((state) => ({
				...state,
				room_id,
				entities_pages: data.entities_pages,
				entities: [],
				scenes: []
			}));
			for (const page of data.entities_pages) set((state) => ({
				...state,
				entities: [...state.entities, ...page.entities],
				scenes: [...state.scenes, ...page.scenes]
			}));
			set({ status: "loaded" });
		});
	},
	removeEntitiesPage: (id) => {
		set((state) => ({ entities_pages: state.entities_pages.filter((page) => page.id !== id) }));
	},
	createEntitiesPage: (room_id, is_scenes_page) => {
		set((state) => ({ entities_pages: [...state.entities_pages, {
			id: state.entities_pages.length + 1,
			display_order: 0,
			number_of_entities: 0,
			type: is_scenes_page ? "scene" : "entity",
			room_id
		}] }));
	},
	setScenePosition: (id, entities_page_id, room_view_position) => {
		set((state) => ({
			...state,
			scenes: state.scenes.map((scene) => scene.id === id ? {
				...scene,
				entities_page_id,
				room_view_position
			} : scene)
		}));
	},
	setEntityPosition: (id, entities_page_id, room_view_position) => {
		set((state) => ({
			...state,
			entities: state.entities.map((entity) => entity.id === id ? {
				...entity,
				entities_page_id,
				room_view_position
			} : entity)
		}));
	},
	removeEntity: (id) => {
		set((state) => ({ entities: state.entities.filter((entity) => entity.id !== id) }));
	},
	removeScene: (id) => {
		set((state) => ({ scenes: state.scenes.filter((scene) => scene.id !== id) }));
	}
}));
//#endregion
//#region src/components/MultiStep_AddEditEntity/step1_select_type.tsx
var import_react = /* @__PURE__ */ __toESM(require_react(), 1);
var import_jsx_runtime = require_jsx_runtime();
var MultiStep_AddEditEntity_Step1 = ({ handleButtonSelectEvent, next_step }) => {
	const [lightHovered, setLightHovered] = (0, import_react.useState)(false);
	const [switchHovered, setSwitchHovered] = (0, import_react.useState)(false);
	const [buttonHovered, setButtonHovered] = (0, import_react.useState)(false);
	const [thermostatHovered, setThermostatHovered] = (0, import_react.useState)(false);
	return /* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", { children: /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
		className: "grid grid-cols-3 gap-4 w-full",
		children: [
			/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
				onMouseEnter: () => setLightHovered(true),
				onMouseLeave: () => setLightHovered(false),
				className: "group bg-base-100 rounded-box border border-neutral border-dashed p-6 cursor-pointer hover:bg-yellow-500/10",
				onClick: () => {
					handleButtonSelectEvent("entity_type", "light")();
					next_step();
				},
				children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
					className: "flex justify-center items-center",
					children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", { className: `mdi ${lightHovered ? "mdi-lightbulb-on" : "mdi-lightbulb"} text-3xl text-neutral-content/50 group-hover:text-yellow-500` })
				}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
					className: "flex justify-center items-center",
					children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", {
						className: "text-base-content/50 group-hover:text-base-content",
						children: "Light"
					})
				})]
			}),
			/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
				onMouseEnter: () => setSwitchHovered(true),
				onMouseLeave: () => setSwitchHovered(false),
				className: "group bg-base-100 rounded-box border border-neutral border-dashed p-6 cursor-pointer hover:bg-yellow-500/10",
				onClick: () => {
					handleButtonSelectEvent("entity_type", "switch")();
					next_step();
				},
				children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
					className: "flex justify-center items-center",
					children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", { className: `mdi ${switchHovered ? "mdi-toggle-switch-variant" : "mdi-toggle-switch-variant-off"} text-3xl text-neutral-content/50 group-hover:text-yellow-500` })
				}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
					className: "flex justify-center items-center",
					children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", {
						className: "text-base-content/50 group-hover:text-base-content",
						children: "Switch"
					})
				})]
			}),
			/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
				onMouseEnter: () => setButtonHovered(true),
				onMouseLeave: () => setButtonHovered(false),
				className: "group bg-base-100 rounded-box border border-neutral border-dashed p-6 cursor-pointer hover:bg-yellow-500/10",
				onClick: () => {
					handleButtonSelectEvent("entity_type", "button")();
					next_step();
				},
				children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
					className: "flex justify-center items-center",
					children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", { className: `mdi ${buttonHovered ? "mdi-gesture-tap-button" : "mdi-button-pointer"} text-3xl text-neutral-content/50 group-hover:text-yellow-500` })
				}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
					className: "flex justify-center items-center",
					children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", {
						className: "text-base-content/50 group-hover:text-base-content",
						children: "Button"
					})
				})]
			}),
			/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
				onMouseEnter: () => setThermostatHovered(true),
				onMouseLeave: () => setThermostatHovered(false),
				className: "group bg-base-100 rounded-box border border-neutral border-dashed p-6 cursor-pointer hover:bg-yellow-500/10",
				onClick: () => {
					handleButtonSelectEvent("entity_type", "thermostat")();
					next_step();
				},
				children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
					className: "flex justify-center items-center",
					children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", { className: `mdi ${thermostatHovered ? "mdi-thermostat-cog" : "mdi-thermostat"} text-3xl text-neutral-content/50 group-hover:text-yellow-500` })
				}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
					className: "flex justify-center items-center",
					children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", {
						className: "text-base-content/50 group-hover:text-base-content text-center",
						children: "Thermostat / climate"
					})
				})]
			})
		]
	}) });
};
//#endregion
//#region src/components/MultiStep_AddEditEntity/step2_select_source.tsx
var MultiStep_AddEditEntity_Step2 = ({ handleButtonSelectEvent, entity_type, next_step }) => {
	const available_controllers = {
		light: ["home_assistant", "openhab"],
		switch: ["home_assistant", "openhab"],
		button: ["home_assistant", "nspm"],
		thermostat: ["home_assistant", "openhab"],
		scene: [
			"home_assistant",
			"openhab",
			"nspm"
		]
	};
	return /* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", { children: /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
		className: "grid grid-cols-3 gap-4 w-full",
		children: [
			available_controllers[entity_type]?.includes("home_assistant") && useSettingsStore.getState().settings?.home_assistant_token_set && /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
				className: "group bg-base-100 rounded-box border border-neutral border-dashed p-6 cursor-pointer hover:bg-home-assistant/10",
				onClick: () => {
					handleButtonSelectEvent("controller", "home_assistant")();
					next_step();
				},
				children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
					className: "flex justify-center items-center p-4",
					children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)("img", {
						src: "/static/home-assistant-logo/home-assistant-logomark-color-on-light.svg",
						alt: "Home Assistant logo",
						className: "h-12 w-12"
					})
				}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
					className: "flex justify-center items-center",
					children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", {
						className: "text-base-content/50 group-hover:text-base-content text-center",
						children: "Home Assistant"
					})
				})]
			}),
			available_controllers[entity_type]?.includes("openhab") && useSettingsStore.getState().settings?.openhab_token_set && /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
				className: "group bg-base-100 rounded-box border border-neutral border-dashed p-6 cursor-pointer hover:bg-openhab/10",
				onClick: () => {
					handleButtonSelectEvent("controller", "openhab")();
					next_step();
				},
				children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
					className: "flex justify-center items-center p-4",
					children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)("img", {
						src: "/static/openhab-logo/openHAB_darkBG_appicon.svg",
						alt: "OpenHAB logo",
						className: "h-12 w-12"
					})
				}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
					className: "flex justify-center items-center",
					children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", {
						className: "text-base-content/50 group-hover:text-base-content text-center",
						children: "OpenHAB"
					})
				})]
			}),
			available_controllers[entity_type]?.includes("nspm") && /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
				className: "group bg-base-100 rounded-box border border-neutral border-dashed p-6 cursor-pointer hover:bg-accent/10",
				onClick: () => {
					handleButtonSelectEvent("controller", "nspm")();
					next_step();
				},
				children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
					className: "flex justify-center items-center p-4",
					children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)("img", {
						src: "/static/icon.svg",
						alt: "NSPanel Manager logo",
						className: "h-12 w-12"
					})
				}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
					className: "flex justify-center items-center",
					children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", {
						className: "text-base-content/50 group-hover:text-base-content text-center",
						children: "NSPanel Manager"
					})
				})]
			})
		]
	}) });
};
//#endregion
//#region node_modules/react-hook-form/dist/index.esm.mjs
var isCheckBoxInput = (element) => element.type === "checkbox";
var isFileInput = (element) => element.type === "file";
var isDateObject = (value) => value instanceof Date;
var isNullOrUndefined = (value) => value == null;
var isObjectType = (value) => typeof value === "object";
var isObject$1 = (value) => !isNullOrUndefined(value) && !Array.isArray(value) && isObjectType(value) && !isDateObject(value);
var getEventValue = (event) => isObject$1(event) && event.target ? isCheckBoxInput(event.target) ? event.target.checked : isFileInput(event.target) ? event.target.files : event.target.value : event;
var FIELD_PATH_RE = /[.[\]'"]/;
var stringToPath = (input) => input.split(FIELD_PATH_RE).filter(Boolean);
var isNameInFieldArray = (names, name) => name.split(".").some((part, index, arr) => !isNaN(Number(part)) && names.has(arr.slice(0, index).join(".")));
var isWeb = typeof window !== "undefined" && typeof window.HTMLElement !== "undefined" && typeof document !== "undefined";
function cloneObject(data) {
	if (data === null || typeof data !== "object") return data;
	if (data instanceof Date) return new Date(data);
	const isFileListInstance = typeof FileList !== "undefined" && data instanceof FileList;
	if (isWeb && (data instanceof Blob || isFileListInstance)) return data;
	const isArray = Array.isArray(data);
	if (!isArray && data.constructor !== Object) return data;
	const copy = isArray ? [] : Object.create(Object.getPrototypeOf(data));
	for (const key in data) if (Object.prototype.hasOwnProperty.call(data, key)) copy[key] = cloneObject(data[key]);
	return copy;
}
var EVENTS = {
	BLUR: "blur",
	FOCUS_OUT: "focusout",
	CHANGE: "change",
	SUBMIT: "submit",
	TRIGGER: "trigger",
	VALID: "valid"
};
var VALIDATION_MODE = {
	onBlur: "onBlur",
	onChange: "onChange",
	onSubmit: "onSubmit",
	onTouched: "onTouched",
	all: "all"
};
var INPUT_VALIDATION_RULES = {
	max: "max",
	min: "min",
	maxLength: "maxLength",
	minLength: "minLength",
	pattern: "pattern",
	required: "required",
	validate: "validate"
};
var ROOT_ERROR_TYPE = "root";
var PROTOTYPE_KEYWORDS = [
	"__proto__",
	"constructor",
	"prototype"
];
var IS_KEY_RE = /^\w*$/;
var isKey = (value) => IS_KEY_RE.test(value);
var isUndefined = (val) => val === void 0;
var get = (object, path, defaultValue) => {
	if (!path || !isObject$1(object)) return defaultValue;
	const paths = isKey(path) ? [path] : stringToPath(path);
	if (paths.some((key) => PROTOTYPE_KEYWORDS.includes(key))) return defaultValue;
	const result = paths.reduce((result, key) => {
		return isNullOrUndefined(result) ? void 0 : result[key];
	}, object);
	return isUndefined(result) || result === object ? isUndefined(object[path]) ? defaultValue : object[path] : result;
};
var isBoolean = (value) => typeof value === "boolean";
var isFunction = (value) => typeof value === "function";
var set = (object, path, value) => {
	let index = -1;
	const tempPath = isKey(path) ? [path] : stringToPath(path);
	const length = tempPath.length;
	const lastIndex = length - 1;
	while (++index < length) {
		const key = tempPath[index];
		let newValue = value;
		if (index !== lastIndex) {
			const objValue = object[key];
			newValue = isObject$1(objValue) || Array.isArray(objValue) ? objValue : !isNaN(+tempPath[index + 1]) ? [] : {};
		}
		if (PROTOTYPE_KEYWORDS.includes(key)) return;
		object[key] = newValue;
		object = object[key];
	}
};
/**
* Separate context for `control` to prevent unnecessary rerenders.
* Internal hooks that only need control use this instead of full form context.
*/
var HookFormControlContext = import_react.createContext(null);
HookFormControlContext.displayName = "HookFormControlContext";
/**
* @internal Internal hook to access only control from context.
*/
var useFormControlContext = () => import_react.useContext(HookFormControlContext);
var getProxyFormState = (formState, control, localProxyFormState, isRoot = true) => {
	const result = {};
	for (const key in formState) Object.defineProperty(result, key, { get: () => {
		const _key = key;
		if (control._proxyFormState[_key] !== VALIDATION_MODE.all) control._proxyFormState[_key] = !isRoot || VALIDATION_MODE.all;
		localProxyFormState && (localProxyFormState[_key] = true);
		return formState[_key];
	} });
	return result;
};
var useIsomorphicLayoutEffect = isWeb ? import_react.useLayoutEffect : import_react.useEffect;
var isPlainObject$1 = (tempObject) => {
	const prototypeCopy = tempObject.constructor && tempObject.constructor.prototype;
	return isObject$1(prototypeCopy) && prototypeCopy.hasOwnProperty("isPrototypeOf");
};
var isPrimitive = (value) => isNullOrUndefined(value) || !isObjectType(value);
var isEmptyObjectWithCustomPrototype = (object, keys) => keys.length === 0 && !Array.isArray(object) && !isPlainObject$1(object);
function deepEqual(object1, object2, visited = /* @__PURE__ */ new WeakMap()) {
	if (object1 === object2) return true;
	if (isPrimitive(object1) || isPrimitive(object2)) return Object.is(object1, object2);
	if (isDateObject(object1) && isDateObject(object2)) return Object.is(object1.getTime(), object2.getTime());
	const keys1 = Object.keys(object1);
	const keys2 = Object.keys(object2);
	if (keys1.length !== keys2.length) return false;
	if (isEmptyObjectWithCustomPrototype(object1, keys1) || isEmptyObjectWithCustomPrototype(object2, keys2)) return Object.is(object1, object2);
	if (!keys1.length && Array.isArray(object1) !== Array.isArray(object2)) return false;
	const visitedPairs = visited.get(object1);
	if (visitedPairs && visitedPairs.has(object2)) return true;
	if (visitedPairs) visitedPairs.add(object2);
	else {
		const ws = /* @__PURE__ */ new WeakSet();
		ws.add(object2);
		visited.set(object1, ws);
	}
	for (const key of keys1) {
		const val1 = object1[key];
		if (!(key in object2)) return false;
		if (key !== "ref") {
			const val2 = object2[key];
			if (isDateObject(val1) && isDateObject(val2) || (isObject$1(val1) || Array.isArray(val1)) && (isObject$1(val2) || Array.isArray(val2)) ? !deepEqual(val1, val2, visited) : !Object.is(val1, val2)) return false;
		}
	}
	return true;
}
function useResyncOnReconnect(getInitialValue) {
	const _connected = import_react.useRef(false);
	const _initialized = import_react.useRef(false);
	const _prevValue = import_react.useRef(void 0);
	const _renderCount = import_react.useRef(0);
	_renderCount.current++;
	if (!_initialized.current && getInitialValue) {
		_initialized.current = true;
		_prevValue.current = cloneObject(getInitialValue());
	}
	return {
		resyncIfNeeded: import_react.useCallback((enabled, getCurrentValue, setValue) => {
			if (enabled && (_connected.current || _initialized.current && _renderCount.current > 1)) {
				const currentValue = getCurrentValue();
				if (!deepEqual(_prevValue.current, currentValue)) setValue(currentValue);
			}
			_connected.current = true;
		}, []),
		snapshot: import_react.useCallback((enabled, getCurrentValue) => {
			if (enabled) _prevValue.current = cloneObject(getCurrentValue());
		}, [])
	};
}
var isString = (value) => typeof value === "string";
var generateWatchOutput = (names, _names, formValues, isGlobal, defaultValue) => {
	if (isString(names)) {
		isGlobal && _names.watch.add(names);
		return get(formValues, names, defaultValue);
	}
	if (Array.isArray(names)) return names.map((fieldName) => (isGlobal && _names.watch.add(fieldName), get(formValues, fieldName, get(defaultValue, fieldName))));
	isGlobal && (_names.watchAll = true);
	return formValues;
};
var generateId = () => typeof crypto !== "undefined" && crypto.randomUUID ? crypto.randomUUID() : "xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx".replace(/[xy]/g, (c) => {
	const r = Math.random() * 16 | 0;
	return (c === "x" ? r : r & 3 | 8).toString(16);
});
var getFocusFieldName = (name, index, options = {}) => options.shouldFocus || isUndefined(options.shouldFocus) ? options.focusName || `${name}.${isUndefined(options.focusIndex) ? index : options.focusIndex}.` : "";
var getValidationModes = (mode) => ({
	isOnSubmit: !mode || mode === VALIDATION_MODE.onSubmit,
	isOnBlur: mode === VALIDATION_MODE.onBlur,
	isOnChange: mode === VALIDATION_MODE.onChange,
	isOnAll: mode === VALIDATION_MODE.all,
	isOnTouch: mode === VALIDATION_MODE.onTouched
});
var isWatched = (name, _names, isBlurEvent) => {
	if (isBlurEvent) return false;
	if (_names.watchAll || _names.watch.has(name)) return true;
	for (const watchName of _names.watch) if (name.startsWith(watchName) && name.charAt(watchName.length) === ".") return true;
	return false;
};
var iterateFieldsByAction = (fields, action, fieldsNames, abortEarly) => {
	for (const key of fieldsNames || Object.keys(fields)) {
		if (key === "_f") continue;
		const field = fieldsNames ? get(fields, key) : fields[key];
		if (field) {
			const { _f } = field;
			if (_f) {
				if (_f.refs && _f.refs[0] && action(_f.refs[0], key) && !abortEarly) return true;
				else if (_f.ref && action(_f.ref, _f.name) && !abortEarly) return true;
				else if (iterateFieldsByAction(field, action)) break;
			} else if (isObject$1(field) || Array.isArray(field)) {
				if (iterateFieldsByAction(field, action)) break;
			}
		}
	}
};
var updateFieldArrayRootError = (errors, error, name) => {
	const existingErrors = get(errors, name);
	const fieldArrayErrors = Array.isArray(existingErrors) ? existingErrors : [];
	set(fieldArrayErrors, ROOT_ERROR_TYPE, error[name]);
	set(errors, name, fieldArrayErrors);
	return errors;
};
var isEmptyObject = (value) => isObject$1(value) && !Object.keys(value).length;
var isHTMLElement = (value) => {
	if (!isWeb) return false;
	const owner = value ? value.ownerDocument : 0;
	return value instanceof (owner && owner.defaultView ? owner.defaultView.HTMLElement : HTMLElement);
};
var isRadioInput = (element) => element.type === "radio";
var isRegex = (value) => value instanceof RegExp;
var appendErrors = (name, validateAllFieldCriteria, errors, type, message) => validateAllFieldCriteria ? {
	...errors[name],
	types: {
		...errors[name] && errors[name].types ? errors[name].types : {},
		[type]: message || true
	}
} : {};
var defaultResult = {
	value: false,
	isValid: false
};
var validResult = {
	value: true,
	isValid: true
};
var getCheckboxValue = (options) => {
	if (!Array.isArray(options)) return defaultResult;
	if (options.length > 1) {
		const values = options.filter((option) => option && option.checked && !option.disabled).map((option) => option.value);
		return {
			value: values,
			isValid: !!values.length
		};
	}
	const option = options[0];
	if (!option || !option.checked || option.disabled) return defaultResult;
	if (!option.attributes || !("value" in option.attributes)) return validResult;
	return isUndefined(option.value) || option.value === "" ? validResult : {
		value: option.value,
		isValid: true
	};
};
var defaultReturn = {
	isValid: false,
	value: null
};
var getRadioValue = (options) => Array.isArray(options) ? options.reduce((previous, option) => option && option.checked && !option.disabled ? {
	isValid: true,
	value: option.value
} : previous, defaultReturn) : defaultReturn;
function getValidateError(result, ref, type = "validate") {
	if (isString(result) || Array.isArray(result) && result.every(isString) || isBoolean(result) && !result) return {
		type,
		message: isString(result) ? result : "",
		ref
	};
}
var getValueAndMessage = (validationData) => isObject$1(validationData) && !isRegex(validationData) ? validationData : {
	value: validationData,
	message: ""
};
var validateField = async (field, disabledFieldNames, formValues, validateAllFieldCriteria, shouldUseNativeValidation, isFieldArray) => {
	const { ref, refs, required, maxLength, minLength, min, max, pattern, validate, name, valueAsNumber, mount } = field._f;
	const inputValue = get(formValues, name);
	if (!mount || disabledFieldNames.has(name)) return {};
	const inputRef = refs ? refs[0] : ref;
	const setCustomValidity = (message) => {
		if (shouldUseNativeValidation && inputRef.reportValidity) {
			const validityMessage = isBoolean(message) ? "" : message || "";
			if (refs) refs.forEach((ref) => ref.setCustomValidity(validityMessage));
			else inputRef.setCustomValidity(validityMessage);
			inputRef.reportValidity();
		}
	};
	const error = {};
	const isRadio = isRadioInput(ref);
	const isCheckBox = isCheckBoxInput(ref);
	const isRadioOrCheckbox = isRadio || isCheckBox;
	const isEmpty = (valueAsNumber || isFileInput(ref)) && isUndefined(ref.value) && isUndefined(inputValue) || isHTMLElement(ref) && ref.value === "" || inputValue === "" || Array.isArray(inputValue) && !inputValue.length;
	const appendErrorsCurry = appendErrors.bind(null, name, validateAllFieldCriteria, error);
	const getMinMaxMessage = (exceedMax, maxLengthMessage, minLengthMessage, maxType = INPUT_VALIDATION_RULES.maxLength, minType = INPUT_VALIDATION_RULES.minLength) => {
		const message = exceedMax ? maxLengthMessage : minLengthMessage;
		error[name] = {
			type: exceedMax ? maxType : minType,
			message,
			ref,
			...appendErrorsCurry(exceedMax ? maxType : minType, message)
		};
	};
	if (isFieldArray ? !Array.isArray(inputValue) || !inputValue.length : required && (!isRadioOrCheckbox && (isEmpty || isNullOrUndefined(inputValue)) || isBoolean(inputValue) && !inputValue || isCheckBox && !getCheckboxValue(refs).isValid || isRadio && !getRadioValue(refs).isValid)) {
		const { value, message } = isString(required) ? {
			value: !!required,
			message: required
		} : getValueAndMessage(required);
		if (value) {
			error[name] = {
				type: INPUT_VALIDATION_RULES.required,
				message,
				ref: inputRef,
				...appendErrorsCurry(INPUT_VALIDATION_RULES.required, message)
			};
			if (!validateAllFieldCriteria) {
				setCustomValidity(message);
				return error;
			}
		}
	}
	if (!isEmpty && (!isNullOrUndefined(min) || !isNullOrUndefined(max))) {
		let exceedMax;
		let exceedMin;
		const maxOutput = getValueAndMessage(max);
		const minOutput = getValueAndMessage(min);
		if (!isNullOrUndefined(inputValue) && !isDateObject(inputValue) && !isNaN(inputValue)) {
			const valueNumber = ref.valueAsNumber || (inputValue ? +inputValue : inputValue);
			if (!isNullOrUndefined(maxOutput.value)) exceedMax = valueNumber > maxOutput.value;
			if (!isNullOrUndefined(minOutput.value)) exceedMin = valueNumber < minOutput.value;
		} else {
			const valueDate = ref.valueAsDate || new Date(inputValue);
			const convertTimeToDate = (time) => /* @__PURE__ */ new Date((/* @__PURE__ */ new Date()).toDateString() + " " + time);
			const isTime = ref.type == "time";
			const isWeek = ref.type == "week";
			if (isString(maxOutput.value) && inputValue) exceedMax = isTime ? convertTimeToDate(inputValue) > convertTimeToDate(maxOutput.value) : isWeek ? inputValue > maxOutput.value : valueDate > new Date(maxOutput.value);
			if (isString(minOutput.value) && inputValue) exceedMin = isTime ? convertTimeToDate(inputValue) < convertTimeToDate(minOutput.value) : isWeek ? inputValue < minOutput.value : valueDate < new Date(minOutput.value);
		}
		if (exceedMax || exceedMin) {
			getMinMaxMessage(!!exceedMax, maxOutput.message, minOutput.message, INPUT_VALIDATION_RULES.max, INPUT_VALIDATION_RULES.min);
			if (!validateAllFieldCriteria) {
				setCustomValidity(error[name].message);
				return error;
			}
		}
	}
	if ((maxLength || minLength) && !isEmpty && (isString(inputValue) || isFieldArray && Array.isArray(inputValue))) {
		const maxLengthOutput = getValueAndMessage(maxLength);
		const minLengthOutput = getValueAndMessage(minLength);
		const exceedMax = !isNullOrUndefined(maxLengthOutput.value) && inputValue.length > +maxLengthOutput.value;
		const exceedMin = !isNullOrUndefined(minLengthOutput.value) && inputValue.length < +minLengthOutput.value;
		if (exceedMax || exceedMin) {
			getMinMaxMessage(exceedMax, maxLengthOutput.message, minLengthOutput.message);
			if (!validateAllFieldCriteria) {
				setCustomValidity(error[name].message);
				return error;
			}
		}
	}
	if (pattern && !isEmpty && isString(inputValue)) {
		const { value: patternValue, message } = getValueAndMessage(pattern);
		if (isRegex(patternValue) && !inputValue.match(patternValue)) {
			error[name] = {
				type: INPUT_VALIDATION_RULES.pattern,
				message,
				ref,
				...appendErrorsCurry(INPUT_VALIDATION_RULES.pattern, message)
			};
			if (!validateAllFieldCriteria) {
				setCustomValidity(message);
				return error;
			}
		}
	}
	if (validate) {
		if (isFunction(validate)) {
			const validateError = getValidateError(await validate(inputValue, formValues), inputRef);
			if (validateError) {
				error[name] = {
					...validateError,
					...appendErrorsCurry(INPUT_VALIDATION_RULES.validate, validateError.message)
				};
				if (!validateAllFieldCriteria) {
					setCustomValidity(validateError.message);
					return error;
				}
			}
		} else if (isObject$1(validate)) {
			let validationResult = {};
			for (const key in validate) {
				if (!isEmptyObject(validationResult) && !validateAllFieldCriteria) break;
				const validateError = getValidateError(await validate[key](inputValue, formValues), inputRef, key);
				if (validateError) {
					validationResult = {
						...validateError,
						...appendErrorsCurry(key, validateError.message)
					};
					setCustomValidity(validateError.message);
					if (validateAllFieldCriteria) error[name] = validationResult;
				}
			}
			if (!isEmptyObject(validationResult)) {
				error[name] = {
					ref: inputRef,
					...validationResult
				};
				if (!validateAllFieldCriteria) return error;
			}
		}
	}
	const fieldError = error[name];
	setCustomValidity(fieldError ? fieldError.message : true);
	return error;
};
var convertToArrayPayload = (value) => Array.isArray(value) ? value : [value];
var appendAt = (data, value) => [...data, ...convertToArrayPayload(value)];
var fillEmptyArray = (value) => Array.isArray(value) ? value.map(() => void 0) : void 0;
function insert(data, index, value) {
	return [
		...data.slice(0, index),
		...convertToArrayPayload(value),
		...data.slice(index)
	];
}
var moveArrayAt = (data, from, to) => {
	if (!Array.isArray(data)) return [];
	if (isUndefined(data[to])) data[to] = void 0;
	data.splice(to, 0, data.splice(from, 1)[0]);
	return data;
};
var prependAt = (data, value) => [...convertToArrayPayload(value), ...convertToArrayPayload(data)];
var compact = (value) => Array.isArray(value) ? value.filter(Boolean) : [];
function removeAtIndexes(data, indexes) {
	let i = 0;
	const temp = [...data];
	for (const index of indexes) {
		temp.splice(index - i, 1);
		i++;
	}
	return compact(temp).length ? temp : [];
}
var removeArrayAt = (data, index) => isUndefined(index) ? [] : removeAtIndexes(data, convertToArrayPayload(index).sort((a, b) => a - b));
var swapArrayAt = (data, indexA, indexB) => {
	[data[indexA], data[indexB]] = [data[indexB], data[indexA]];
};
function baseGet(object, updatePath) {
	const length = updatePath.length - 1;
	let index = 0;
	while (index < length) {
		if (isNullOrUndefined(object)) {
			object = void 0;
			break;
		}
		object = object[updatePath[index]];
		index++;
	}
	return object;
}
function isEmptyArray(obj) {
	for (const key in obj) if (obj.hasOwnProperty(key) && !isUndefined(obj[key])) return false;
	return true;
}
function unset(object, path) {
	if (isString(path) && Object.prototype.hasOwnProperty.call(object, path)) {
		delete object[path];
		return object;
	}
	const paths = Array.isArray(path) ? path : isKey(path) ? [path] : stringToPath(path);
	if (paths.some((segment) => PROTOTYPE_KEYWORDS.includes(String(segment)))) return object;
	const childObject = paths.length === 1 ? object : baseGet(object, paths);
	const index = paths.length - 1;
	const key = paths[index];
	if (childObject) delete childObject[key];
	if (index !== 0 && (isObject$1(childObject) && isEmptyObject(childObject) || Array.isArray(childObject) && isEmptyArray(childObject))) unset(object, paths.slice(0, -1));
	return object;
}
var updateAt = (fieldValues, index, value) => {
	fieldValues[index] = value;
	return fieldValues;
};
/**
* A custom hook that exposes convenient methods to perform operations with a list of dynamic inputs that need to be appended, updated, removed etc. • [Demo](https://codesandbox.io/s/react-hook-form-usefieldarray-ssugn) • [Video](https://youtu.be/4MrbfGSFY2A)
*
* @remarks
* [API](https://react-hook-form.com/docs/usefieldarray) • [Demo](https://codesandbox.io/s/react-hook-form-usefieldarray-ssugn)
*
* @param props - useFieldArray props
*
* @returns methods - functions to manipulate with the Field Arrays (dynamic inputs) {@link UseFieldArrayReturn}
*
* @example
* ```tsx
* function App() {
*   const { register, control, handleSubmit, reset, trigger, setError } = useForm({
*     defaultValues: {
*       test: []
*     }
*   });
*   const { fields, append } = useFieldArray({
*     control,
*     name: "test"
*   });
*
*   return (
*     <form onSubmit={handleSubmit(data => console.log(data))}>
*       {fields.map((item, index) => (
*          <input key={item.id} {...register(`test.${index}.firstName`)}  />
*       ))}
*       <button type="button" onClick={() => append({ firstName: "bill" })}>
*         append
*       </button>
*       <input type="submit" />
*     </form>
*   );
* }
* ```
*/
function useFieldArray(props) {
	const formControl = useFormControlContext();
	const { control = formControl, name, keyName = "id", disabled, shouldUnregister, rules } = props;
	const [fields, setFields] = import_react.useState(control._getFieldArray(name));
	const ids = import_react.useRef(control._getFieldArray(name).map(generateId));
	const _actioned = import_react.useRef(false);
	if (!disabled) control._names.array.add(name);
	import_react.useMemo(() => !disabled && rules && fields.length >= 0 && control.register(name, rules), [
		control,
		name,
		fields.length,
		rules,
		disabled
	]);
	useIsomorphicLayoutEffect(() => {
		if (disabled) return;
		return control._subjects.array.subscribe({ next: ({ values, name: fieldArrayName }) => {
			if (fieldArrayName === name || !fieldArrayName) {
				const fieldValues = get(values, name);
				if (Array.isArray(fieldValues)) {
					setFields(fieldValues);
					ids.current = fieldValues.map(generateId);
				} else if (!fieldArrayName) {
					setFields([]);
					ids.current = [];
				}
			}
		} }).unsubscribe;
	}, [
		control,
		name,
		disabled
	]);
	const updateValues = import_react.useCallback((updatedFieldArrayValues) => {
		_actioned.current = true;
		control._setFieldArray(name, updatedFieldArrayValues);
	}, [control, name]);
	const append = (value, options) => {
		if (disabled) return;
		const appendValue = convertToArrayPayload(cloneObject(value));
		const updatedFieldArrayValues = appendAt(control._getFieldArray(name), appendValue);
		control._names.focus = getFocusFieldName(name, updatedFieldArrayValues.length - 1, options);
		ids.current = appendAt(ids.current, appendValue.map(generateId));
		updateValues(updatedFieldArrayValues);
		setFields(updatedFieldArrayValues);
		control._setFieldArray(name, updatedFieldArrayValues, appendAt, { argA: fillEmptyArray(value) });
	};
	const prepend = (value, options) => {
		if (disabled) return;
		const prependValue = convertToArrayPayload(cloneObject(value));
		const updatedFieldArrayValues = prependAt(control._getFieldArray(name), prependValue);
		control._names.focus = getFocusFieldName(name, 0, options);
		ids.current = prependAt(ids.current, prependValue.map(generateId));
		updateValues(updatedFieldArrayValues);
		setFields(updatedFieldArrayValues);
		control._setFieldArray(name, updatedFieldArrayValues, prependAt, { argA: fillEmptyArray(value) });
	};
	const remove = (index) => {
		if (disabled) return;
		const updatedFieldArrayValues = removeArrayAt(control._getFieldArray(name), index);
		ids.current = removeArrayAt(ids.current, index);
		updateValues(updatedFieldArrayValues);
		setFields(updatedFieldArrayValues);
		!Array.isArray(get(control._fields, name)) && set(control._fields, name, void 0);
		control._setFieldArray(name, updatedFieldArrayValues, removeArrayAt, { argA: index });
	};
	const insert$1 = (index, value, options) => {
		if (disabled) return;
		const insertValue = convertToArrayPayload(cloneObject(value));
		const updatedFieldArrayValues = insert(control._getFieldArray(name), index, insertValue);
		control._names.focus = getFocusFieldName(name, index, options);
		ids.current = insert(ids.current, index, insertValue.map(generateId));
		updateValues(updatedFieldArrayValues);
		setFields(updatedFieldArrayValues);
		control._setFieldArray(name, updatedFieldArrayValues, insert, {
			argA: index,
			argB: fillEmptyArray(value)
		});
	};
	const swap = (indexA, indexB) => {
		if (disabled) return;
		const updatedFieldArrayValues = control._getFieldArray(name);
		swapArrayAt(updatedFieldArrayValues, indexA, indexB);
		swapArrayAt(ids.current, indexA, indexB);
		updateValues(updatedFieldArrayValues);
		setFields(updatedFieldArrayValues);
		control._setFieldArray(name, updatedFieldArrayValues, swapArrayAt, {
			argA: indexA,
			argB: indexB
		}, false);
	};
	const move = (from, to) => {
		if (disabled) return;
		const updatedFieldArrayValues = control._getFieldArray(name);
		moveArrayAt(updatedFieldArrayValues, from, to);
		moveArrayAt(ids.current, from, to);
		updateValues(updatedFieldArrayValues);
		setFields(updatedFieldArrayValues);
		control._setFieldArray(name, updatedFieldArrayValues, moveArrayAt, {
			argA: from,
			argB: to
		}, false);
	};
	const update = (index, value) => {
		if (disabled) return;
		const updateValue = cloneObject(value);
		const updatedFieldArrayValues = updateAt(control._getFieldArray(name), index, updateValue);
		ids.current = [...updatedFieldArrayValues].map((item, i) => !item || i === index ? generateId() : ids.current[i]);
		updateValues(updatedFieldArrayValues);
		setFields([...updatedFieldArrayValues]);
		control._setFieldArray(name, updatedFieldArrayValues, updateAt, {
			argA: index,
			argB: fillEmptyArray(value)
		});
	};
	const replace = (value) => {
		if (disabled) return;
		const updatedFieldArrayValues = convertToArrayPayload(cloneObject(value));
		ids.current = updatedFieldArrayValues.map(generateId);
		updateValues([...updatedFieldArrayValues]);
		setFields([...updatedFieldArrayValues]);
		control._setFieldArray(name, [...updatedFieldArrayValues], (data) => data, {}, true, false);
	};
	import_react.useEffect(() => {
		if (disabled) {
			control._state.actionArrayLengths.delete(name);
			return;
		}
		control._state.action = false;
		control._state.actionArrayLengths.delete(name);
		isWatched(name, control._names) && control._subjects.state.next({ ...control._formState });
		const validationModes = getValidationModes(control._options.mode);
		if (_actioned.current && (!validationModes.isOnSubmit || control._formState.isSubmitted) && !getValidationModes(control._options.reValidateMode).isOnSubmit && !validationModes.isOnBlur) {
			if (control._options.resolver) control._runSchema([name]).then((result) => {
				var _a, _b;
				control._updateIsValidating([name]);
				const error = get(result.errors, name);
				const existingError = get(control._formState.errors, name);
				const existingErrorType = existingError && (existingError.type || ((_a = existingError.root) === null || _a === void 0 ? void 0 : _a.type));
				const existingErrorMessage = existingError && (existingError.message || ((_b = existingError.root) === null || _b === void 0 ? void 0 : _b.message));
				if (existingError ? !error && existingErrorType || error && (existingErrorType !== error.type || existingErrorMessage !== error.message) : error && error.type) {
					if (error) isObject$1(error) && !Object.keys(error).some((key) => !Number.isNaN(+key)) ? updateFieldArrayRootError(control._formState.errors, { [name]: error }, name) : set(control._formState.errors, name, error);
					else unset(control._formState.errors, name);
					control._subjects.state.next({ errors: control._formState.errors });
				}
			});
			else {
				const field = get(control._fields, name);
				if (field && field._f) validateField(field, control._names.disabled, control._formValues, control._options.criteriaMode === VALIDATION_MODE.all, control._options.shouldUseNativeValidation, true).then((error) => !isEmptyObject(error) && control._subjects.state.next({ errors: updateFieldArrayRootError(control._formState.errors, error, name) }));
			}
		}
		_actioned.current && control._subjects.state.next({
			name,
			values: cloneObject(control._formValues)
		});
		control._names.focus && iterateFieldsByAction(control._fields, (ref, key) => {
			if (control._names.focus && key.startsWith(control._names.focus) && ref.focus) {
				ref.focus();
				return 1;
			}
		});
		control._names.focus = "";
		control._setValid();
		_actioned.current = false;
	}, [
		fields,
		name,
		control,
		disabled
	]);
	import_react.useEffect(() => {
		if (!disabled) !get(control._formValues, name) && control._setFieldArray(name);
		return () => {
			control._state.actionArrayLengths.delete(name);
			if (disabled) return;
			const shouldKeepFieldArrayValues = !(control._options.shouldUnregister || shouldUnregister);
			const updateMounted = (name, value) => {
				const field = get(control._fields, name);
				if (field && field._f) field._f.mount = value;
			};
			if (_actioned.current && shouldKeepFieldArrayValues) control._subjects.state.next({
				name,
				values: cloneObject(control._formValues)
			});
			shouldKeepFieldArrayValues ? updateMounted(name, false) : control.unregister(name);
		};
	}, [
		name,
		control,
		keyName,
		shouldUnregister,
		disabled
	]);
	return {
		swap: import_react.useCallback(swap, [
			updateValues,
			name,
			control,
			disabled
		]),
		move: import_react.useCallback(move, [
			updateValues,
			name,
			control,
			disabled
		]),
		prepend: import_react.useCallback(prepend, [
			updateValues,
			name,
			control,
			disabled
		]),
		append: import_react.useCallback(append, [
			updateValues,
			name,
			control,
			disabled
		]),
		remove: import_react.useCallback(remove, [
			updateValues,
			name,
			control,
			disabled
		]),
		insert: import_react.useCallback(insert$1, [
			updateValues,
			name,
			control,
			disabled
		]),
		update: import_react.useCallback(update, [
			updateValues,
			name,
			control,
			disabled
		]),
		replace: import_react.useCallback(replace, [
			updateValues,
			name,
			control,
			disabled
		]),
		fields: import_react.useMemo(() => fields.map((field, index) => ({
			...field,
			...isBoolean(disabled) ? { disabled } : {},
			[keyName]: ids.current[index] || generateId()
		})), [
			fields,
			keyName,
			disabled
		])
	};
}
var HookFormContext = import_react.createContext(null);
HookFormContext.displayName = "HookFormContext";
var createSubject = () => {
	let _observers = [];
	const next = (value) => {
		for (const observer of _observers) observer.next && observer.next(value);
	};
	const subscribe = (observer) => {
		_observers.push(observer);
		return { unsubscribe: () => {
			_observers = _observers.filter((o) => o !== observer);
		} };
	};
	const unsubscribe = () => {
		_observers = [];
	};
	return {
		get observers() {
			return _observers;
		},
		next,
		subscribe,
		unsubscribe
	};
};
function extractFormValues(fieldsState, formValues) {
	const values = {};
	for (const key in fieldsState) if (fieldsState.hasOwnProperty(key)) {
		const fieldState = fieldsState[key];
		const fieldValue = formValues[key];
		if (fieldState && isObject$1(fieldState) && fieldValue) {
			const nestedFieldsState = extractFormValues(fieldState, fieldValue);
			if (isObject$1(nestedFieldsState)) values[key] = nestedFieldsState;
		} else if (fieldsState[key]) values[key] = fieldValue;
	}
	return values;
}
var hasOwn = (value, key) => value !== null && isObjectType(value) && Object.prototype.hasOwnProperty.call(value, key);
var has = (object, path) => {
	if (!path) return false;
	let result = object;
	for (const key of isKey(path) ? [path] : stringToPath(path)) {
		if (!hasOwn(result, key)) return hasOwn(object, path);
		result = result[key];
	}
	return true;
};
var isMultipleSelect = (element) => element.type === `select-multiple`;
var isRadioOrCheckbox = (ref) => isRadioInput(ref) || isCheckBoxInput(ref);
var live = (ref) => isHTMLElement(ref) && ref.isConnected;
function isDirtyContainer(value) {
	return Array.isArray(value) || isObject$1(value);
}
function collectDirtyFieldNames(dirtyTree, cachedDirtyFields, prefix = "", names = []) {
	for (const key in dirtyTree) {
		const path = prefix ? `${prefix}.${key}` : key;
		const value = dirtyTree[key];
		if (isDirtyContainer(value) && isDirtyContainer(get(cachedDirtyFields, path))) collectDirtyFieldNames(value, cachedDirtyFields, path, names);
		else names.push(path);
	}
	return names;
}
var objectHasFunction = (data) => {
	for (const key in data) if (isFunction(data[key])) return true;
	return false;
};
function isTraversable(value) {
	return Array.isArray(value) || isObject$1(value) && !objectHasFunction(value);
}
function isRegisteredLeaf(fieldRef) {
	return !!(fieldRef && "_f" in fieldRef);
}
function isEmptyDirtyContainer(value) {
	return Array.isArray(value) ? !value.some((item) => !isUndefined(item)) : !Object.keys(value).length;
}
function clearDirtyField(container, key) {
	if (Array.isArray(container)) container[key] = void 0;
	else delete container[key];
}
function markFieldsDirty(data, fields = {}, fieldRefs) {
	for (const key in data) {
		const value = data[key];
		const fieldRef = fieldRefs && fieldRefs[key];
		if (isTraversable(value) && (!Array.isArray(value) || !isRegisteredLeaf(fieldRef))) {
			fields[key] = Array.isArray(value) ? [] : {};
			markFieldsDirty(value, fields[key], fieldRef);
			if (isEmptyDirtyContainer(fields[key])) clearDirtyField(fields, key);
		} else if (!isUndefined(value)) fields[key] = true;
	}
	return fields;
}
function getDirtyFields(data, formValues, dirtyFieldsFromValues, fieldRefs) {
	if (!dirtyFieldsFromValues) dirtyFieldsFromValues = markFieldsDirty(formValues, {}, fieldRefs);
	for (const key in data) {
		const value = data[key];
		const fieldRef = fieldRefs && fieldRefs[key];
		if (isTraversable(value) && (!Array.isArray(value) || !isRegisteredLeaf(fieldRef))) {
			if (isUndefined(formValues) || isPrimitive(dirtyFieldsFromValues[key])) dirtyFieldsFromValues[key] = markFieldsDirty(value, Array.isArray(value) ? [] : {}, fieldRef);
			else getDirtyFields(value, isNullOrUndefined(formValues) ? {} : formValues[key], dirtyFieldsFromValues[key], fieldRef);
			if (isEmptyDirtyContainer(dirtyFieldsFromValues[key])) clearDirtyField(dirtyFieldsFromValues, key);
		} else if (deepEqual(value, formValues[key])) clearDirtyField(dirtyFieldsFromValues, key);
		else dirtyFieldsFromValues[key] = true;
	}
	return dirtyFieldsFromValues;
}
var getFieldArrayItemNames = (names, name) => {
	const segments = name.split(".");
	const matches = [];
	let prefix = segments[0];
	for (let i = 1; i < segments.length; prefix += "." + segments[i++]) if (!isNaN(+segments[i]) && names.has(prefix)) matches.push(`${prefix}.${segments[i]}`);
	return matches;
};
var getFieldValueAs = (value, { valueAsNumber, valueAsDate, setValueAs }) => isUndefined(value) ? value : valueAsNumber ? value === "" ? NaN : value ? +value : value : valueAsDate && isString(value) ? new Date(value) : setValueAs ? setValueAs(value) : value;
function getFieldValue(_f) {
	const ref = _f.ref;
	if (isFileInput(ref)) return ref.files;
	if (isRadioInput(ref)) return getRadioValue(_f.refs).value;
	if (isMultipleSelect(ref)) return [...ref.selectedOptions].map(({ value }) => value);
	if (isCheckBoxInput(ref)) return getCheckboxValue(_f.refs).value;
	return getFieldValueAs(ref.value, _f);
}
var getResolverOptions = (fieldsNames, _fields, criteriaMode, shouldUseNativeValidation) => {
	const fields = {};
	for (const name of fieldsNames) {
		const field = get(_fields, name);
		field && set(fields, name, field._f);
	}
	return {
		criteriaMode,
		names: [...fieldsNames],
		fields,
		shouldUseNativeValidation
	};
};
var getRuleValue = (rule) => isUndefined(rule) ? rule : isRegex(rule) ? rule.source : isObject$1(rule) ? isRegex(rule.value) ? rule.value.source : rule.value : rule;
var ASYNC_FUNCTION = "AsyncFunction";
var hasPromiseValidation = (fieldReference) => {
	if (!fieldReference || !fieldReference.validate) return false;
	if (isFunction(fieldReference.validate)) return fieldReference.validate.constructor.name === ASYNC_FUNCTION;
	if (isObject$1(fieldReference.validate)) {
		for (const key in fieldReference.validate) if (fieldReference.validate[key].constructor.name === ASYNC_FUNCTION) return true;
	}
	return false;
};
var hasValidation = (options) => options.mount && (options.required || !isUndefined(options.required) && options.required !== false || !isUndefined(options.min) || !isUndefined(options.max) || !isUndefined(options.maxLength) || !isUndefined(options.minLength) || options.pattern || options.validate);
function schemaErrorLookup(errors, _fields, name) {
	const error = get(errors, name);
	if (error || isKey(name)) return {
		error,
		name
	};
	const names = name.split(".");
	while (names.length) {
		const fieldName = names.join(".");
		const field = get(_fields, fieldName);
		const foundError = get(errors, fieldName);
		if (field && !Array.isArray(field) && name !== fieldName) return { name };
		if (foundError && foundError.type) return {
			name: fieldName,
			error: foundError
		};
		if (foundError && foundError.root && foundError.root.type) return {
			name: `${fieldName}.root`,
			error: foundError.root
		};
		names.pop();
	}
	return { name };
}
var shouldRenderFormState = (formStateData, _proxyFormState, updateFormState, isRoot) => {
	updateFormState(formStateData);
	const keys = Object.keys(formStateData).filter((key) => key !== "name");
	return !keys.length || isRoot && keys.length >= Object.keys(_proxyFormState).length || keys.find((key) => _proxyFormState[key] === (!isRoot || VALIDATION_MODE.all));
};
var shouldSubscribeByName = (name, signalName, exact) => !name || !signalName || name === signalName || convertToArrayPayload(name).some((currentName) => currentName && (exact ? currentName === signalName || currentName.startsWith(signalName + ".") : currentName.startsWith(signalName) || signalName.startsWith(currentName)));
var skipValidation = (isBlurEvent, isTouched, isSubmitted, reValidateMode, mode) => {
	if (mode.isOnAll) return false;
	else if (!isSubmitted && mode.isOnTouch) return !(isTouched || isBlurEvent);
	else if (isSubmitted ? reValidateMode.isOnBlur : mode.isOnBlur) return !isBlurEvent;
	else if (isSubmitted ? reValidateMode.isOnChange : mode.isOnChange) return isBlurEvent;
	return true;
};
var unsetEmptyArray = (ref, name) => {
	const array = get(ref, name);
	!compact(array).length && !(array !== null && array !== void 0 && array.root) && unset(ref, name);
};
var defaultOptions = {
	mode: VALIDATION_MODE.onSubmit,
	reValidateMode: VALIDATION_MODE.onChange,
	shouldFocusError: true
};
var FORM_ERROR_TYPE = "form";
var updateDirtyFields = (dirtyFields, nextDirtyFields) => {
	for (const key in dirtyFields) if (!(key in nextDirtyFields)) delete dirtyFields[key];
	Object.assign(dirtyFields, nextDirtyFields);
};
var DEFAULT_FORM_STATE = {
	submitCount: 0,
	isDirty: false,
	isReady: false,
	isValidating: false,
	isSubmitted: false,
	isSubmitting: false,
	isSubmitSuccessful: false,
	isValid: false,
	touchedFields: {},
	dirtyFields: {},
	validatingFields: {}
};
function createFormControl(props = {}) {
	let _options = {
		...defaultOptions,
		...props
	};
	let _formState = {
		...cloneObject(DEFAULT_FORM_STATE),
		isLoading: isFunction(_options.defaultValues),
		errors: _options.errors || {},
		disabled: _options.disabled || false
	};
	let _fields = {};
	let _defaultValues = isObject$1(_options.defaultValues) || isObject$1(_options.values) ? cloneObject(_options.defaultValues || _options.values) || {} : {};
	let _formValues = _options.shouldUnregister ? {} : cloneObject(_defaultValues);
	let _state = {
		action: false,
		actionArrayLengths: /* @__PURE__ */ new Map(),
		mount: false,
		watch: false,
		keepIsValid: false
	};
	let _names = {
		mount: /* @__PURE__ */ new Set(),
		disabled: /* @__PURE__ */ new Set(),
		unMount: /* @__PURE__ */ new Set(),
		array: /* @__PURE__ */ new Set(),
		watch: /* @__PURE__ */ new Set(),
		registerName: /* @__PURE__ */ new Set()
	};
	const delayErrorCallbacks = {};
	const timers = {};
	let _valuesSubscriberCount = 0;
	let _validationModeBeforeSubmit = getValidationModes(_options.mode);
	let _validationModeAfterSubmit = getValidationModes(_options.reValidateMode);
	const defaultProxyFormState = {
		isDirty: false,
		dirtyFields: false,
		validatingFields: false,
		touchedFields: false,
		isValidating: false,
		isValid: false,
		errors: false
	};
	const _proxyFormState = { ...defaultProxyFormState };
	let _proxySubscribeFormState = { ..._proxyFormState };
	const _subjects = {
		array: createSubject(),
		state: createSubject()
	};
	let _setValidCallId = 0;
	const shouldDisplayAllAssociatedErrors = _options.criteriaMode === VALIDATION_MODE.all;
	const debounce = (name, callback) => (wait) => {
		clearTimeout(timers[name]);
		timers[name] = setTimeout(callback, wait);
	};
	const _setValid = async (shouldUpdateValid) => {
		if (_state.keepIsValid) return;
		if (!_options.disabled && (_proxyFormState.isValid || _proxySubscribeFormState.isValid || shouldUpdateValid)) {
			const callId = ++_setValidCallId;
			let isValid;
			if (_options.resolver) {
				isValid = isEmptyObject((await _runSchema()).errors);
				callId === _setValidCallId && _updateIsValidating();
			} else isValid = await executeBuiltInValidation({
				fields: _fields,
				onlyCheckValid: true,
				eventType: EVENTS.VALID
			});
			if (callId === _setValidCallId && isValid !== _formState.isValid) _subjects.state.next({ isValid });
		}
	};
	const _updateIsValidating = (names, isValidating) => {
		if (!_options.disabled && (_proxyFormState.isValidating || _proxyFormState.validatingFields || _proxySubscribeFormState.isValidating || _proxySubscribeFormState.validatingFields)) {
			(names || _names.mount).forEach((name) => {
				if (name) isValidating ? set(_formState.validatingFields, name, isValidating) : unset(_formState.validatingFields, name);
			});
			_subjects.state.next({
				validatingFields: _formState.validatingFields,
				isValidating: !isEmptyObject(_formState.validatingFields)
			});
		}
	};
	const _updateDirtyFields = () => {
		_formState.dirtyFields = getDirtyFields(_defaultValues, _formValues, void 0, _fields);
	};
	const _setFieldArray = (name, values = [], method, args, shouldSetValues = true, shouldUpdateFieldsAndState = true) => {
		if (args && method && !_options.disabled) {
			_state.action = true;
			const fields = get(_fields, name);
			if (!_state.actionArrayLengths.has(name)) _state.actionArrayLengths.set(name, Array.isArray(fields) ? fields.length : 0);
			if (shouldUpdateFieldsAndState && Array.isArray(fields)) {
				const fieldValues = method(fields, args.argA, args.argB);
				shouldSetValues && set(_fields, name, fieldValues);
			}
			const fieldArrayErrors = get(_formState.errors, name);
			if (shouldUpdateFieldsAndState && Array.isArray(fieldArrayErrors)) {
				const rootError = fieldArrayErrors.root;
				const errors = method(fieldArrayErrors, args.argA, args.argB) || fieldArrayErrors;
				if (rootError) errors.root = rootError;
				shouldSetValues && set(_formState.errors, name, errors);
				unsetEmptyArray(_formState.errors, name);
			}
			const touchedFieldsArray = get(_formState.touchedFields, name);
			if ((_proxyFormState.touchedFields || _proxySubscribeFormState.touchedFields) && shouldUpdateFieldsAndState && Array.isArray(touchedFieldsArray)) {
				const touchedFields = method(touchedFieldsArray, args.argA, args.argB);
				shouldSetValues && set(_formState.touchedFields, name, touchedFields);
			}
			if (_proxyFormState.dirtyFields || _proxySubscribeFormState.dirtyFields) _updateDirtyFields();
			_subjects.state.next({
				name,
				isDirty: _getDirty(name, values),
				dirtyFields: _formState.dirtyFields,
				errors: _formState.errors,
				isValid: _formState.isValid
			});
		} else set(_formValues, name, values);
	};
	const updateErrors = (name, error) => {
		set(_formState.errors, name, error);
		_formState.errors = { ..._formState.errors };
		_subjects.state.next({ errors: _formState.errors });
	};
	const _setErrors = (errors) => {
		_formState.errors = errors;
		_subjects.state.next({
			errors: _formState.errors,
			isValid: false
		});
	};
	const hasExplicitNullIntermediate = (name) => {
		const segments = isKey(name) ? [name] : stringToPath(name);
		let formValues = _formValues;
		let defaultValues = _defaultValues;
		for (let i = 0; i < segments.length - 1; i++) {
			const key = segments[i];
			formValues = isNullOrUndefined(formValues) ? formValues : formValues[key];
			defaultValues = isNullOrUndefined(defaultValues) ? defaultValues : defaultValues[key];
			if (formValues === null && defaultValues !== null) return true;
		}
		return false;
	};
	const isStaleArrayIndex = (name) => {
		if (!_state.actionArrayLengths.size) return false;
		const segments = isKey(name) ? [name] : stringToPath(name);
		let node = _formValues;
		let path = "";
		let ownerDepth = -1;
		let ownerPreActionLength = 0;
		for (let i = 0; i < segments.length; i++) {
			if (isNullOrUndefined(node)) return false;
			const key = segments[i];
			path = path ? `${path}.${key}` : key;
			if (Array.isArray(node) && +key >= node.length) return ownerDepth === -1 ? false : i === ownerDepth ? +key < ownerPreActionLength : true;
			if (_state.actionArrayLengths.has(path)) {
				ownerDepth = i + 1;
				ownerPreActionLength = _state.actionArrayLengths.get(path);
			}
			node = node[key];
		}
		return false;
	};
	const updateValidAndValue = (name, shouldSkipSetValueAs, value, ref) => {
		const field = get(_fields, name);
		if (field) {
			if (hasExplicitNullIntermediate(name) || isStaleArrayIndex(name)) return;
			const wasUnsetInFormValues = isUndefined(get(_formValues, name));
			const defaultValue = get(_formValues, name, isUndefined(value) ? get(_defaultValues, name) : value);
			isUndefined(defaultValue) || ref && ref.defaultChecked || shouldSkipSetValueAs ? set(_formValues, name, shouldSkipSetValueAs ? defaultValue : getFieldValue(field._f)) : setFieldValue(name, defaultValue);
			if (_state.mount && !_state.action) {
				_setValid();
				if (wasUnsetInFormValues && _formState.isDirty && (_proxyFormState.isDirty || _proxySubscribeFormState.isDirty)) {
					if (!_getDirty()) {
						_formState.isDirty = false;
						_subjects.state.next({ ..._formState });
					}
				}
				if (props.shouldUnregister && wasUnsetInFormValues && !isUndefined(get(_formValues, name)) && isWatched(name, _names)) _state.watch = true;
			}
		}
	};
	const updateTouchAndDirty = (name, fieldValue, isBlurEvent, shouldDirty, shouldRender) => {
		let shouldUpdateField = false;
		let isPreviousDirty = false;
		const output = { name };
		if (!_options.disabled || shouldDirty === true) {
			if (!isBlurEvent || shouldDirty) {
				const isCurrentFieldPristine = deepEqual(get(_defaultValues, name), fieldValue);
				if (_proxyFormState.isDirty || _proxySubscribeFormState.isDirty) {
					isPreviousDirty = _formState.isDirty;
					_formState.isDirty = output.isDirty = !isCurrentFieldPristine || _getDirty();
					shouldUpdateField = isPreviousDirty !== output.isDirty;
				}
				isPreviousDirty = !!get(_formState.dirtyFields, name);
				if (isCurrentFieldPristine !== _formState.isDirty) updateDirtyFields(_formState.dirtyFields, getDirtyFields(_defaultValues, _formValues, void 0, _fields));
				else isCurrentFieldPristine ? unset(_formState.dirtyFields, name) : set(_formState.dirtyFields, name, true);
				output.dirtyFields = _formState.dirtyFields;
				shouldUpdateField = shouldUpdateField || (_proxyFormState.dirtyFields || _proxySubscribeFormState.dirtyFields) && isPreviousDirty !== !isCurrentFieldPristine;
			}
			if (isBlurEvent) {
				const isPreviousFieldTouched = get(_formState.touchedFields, name);
				if (!isPreviousFieldTouched) {
					set(_formState.touchedFields, name, isBlurEvent);
					output.touchedFields = _formState.touchedFields;
					shouldUpdateField = shouldUpdateField || (_proxyFormState.touchedFields || _proxySubscribeFormState.touchedFields) && isPreviousFieldTouched !== isBlurEvent;
				}
			}
			shouldUpdateField && shouldRender && _subjects.state.next(output);
		}
		return shouldUpdateField ? output : {};
	};
	const shouldRenderByError = (name, isValid, error, fieldState) => {
		const previousFieldError = get(_formState.errors, name);
		const shouldUpdateValid = (_proxyFormState.isValid || _proxySubscribeFormState.isValid) && isBoolean(isValid) && _formState.isValid !== isValid;
		if (_options.delayError && error) {
			delayErrorCallbacks[name] = debounce(name, () => updateErrors(name, error));
			delayErrorCallbacks[name](_options.delayError);
		} else {
			clearTimeout(timers[name]);
			delete delayErrorCallbacks[name];
			error ? set(_formState.errors, name, error) : unset(_formState.errors, name);
			_formState.errors = { ..._formState.errors };
		}
		if ((error ? !deepEqual(previousFieldError, error) : previousFieldError) || !isEmptyObject(fieldState) || shouldUpdateValid) {
			const updatedFormState = {
				...fieldState,
				...shouldUpdateValid && isBoolean(isValid) ? { isValid } : {},
				errors: _formState.errors,
				name
			};
			_subjects.state.next(updatedFormState);
		}
	};
	const _runSchema = async (name) => {
		_updateIsValidating(name, true);
		return await _options.resolver(_formValues, _options.context, getResolverOptions(name || _names.mount, _fields, _options.criteriaMode, _options.shouldUseNativeValidation));
	};
	const executeSchemaAndUpdateState = async (names) => {
		const { errors } = await _runSchema(names);
		_updateIsValidating(names);
		if (names) {
			for (const name of names) {
				const error = get(errors, name);
				error ? _names.array.has(name) && isObject$1(error) && !Object.keys(error).some((key) => !Number.isNaN(Number(key))) ? updateFieldArrayRootError(_formState.errors, { [name]: error }, name) : set(_formState.errors, name, error) : unset(_formState.errors, name);
			}
			_formState.errors = { ..._formState.errors };
		} else _formState.errors = errors;
		return errors;
	};
	const validateForm = async ({ name, eventType }) => {
		if (props.validate) {
			const result = await props.validate({
				formValues: _formValues,
				formState: _formState,
				name,
				eventType
			});
			if (isObject$1(result)) for (const key in result) {
				const error = result[key];
				if (error) setError(`${FORM_ERROR_TYPE}.${key}`, {
					message: isString(error.message) ? error.message : "",
					type: error.type || INPUT_VALIDATION_RULES.validate
				});
			}
			else if (isString(result) || !result) setError(FORM_ERROR_TYPE, {
				message: result || "",
				type: INPUT_VALIDATION_RULES.validate
			});
			else clearErrors(FORM_ERROR_TYPE);
			return result;
		}
		return true;
	};
	const executeBuiltInValidation = async ({ fields, onlyCheckValid, name, eventType, context = {
		valid: true,
		runRootValidation: false
	} }) => {
		if (props.validate) {
			context.runRootValidation = true;
			if (!await validateForm({
				name,
				eventType
			})) {
				context.valid = false;
				if (onlyCheckValid) return context.valid;
			}
		}
		for (const name in fields) {
			const field = fields[name];
			if (field) {
				const { _f, ...fieldValue } = field;
				if (_f) {
					const isFieldArrayRoot = _names.array.has(_f.name);
					const isPromiseFunction = field._f && hasPromiseValidation(field._f);
					const shouldTrackIsValidatingState = _proxyFormState.validatingFields || _proxyFormState.isValidating || _proxySubscribeFormState.validatingFields || _proxySubscribeFormState.isValidating;
					if (isPromiseFunction && shouldTrackIsValidatingState) _updateIsValidating([_f.name], true);
					const fieldError = await validateField(field, _names.disabled, _formValues, shouldDisplayAllAssociatedErrors, _options.shouldUseNativeValidation && !onlyCheckValid, isFieldArrayRoot);
					if (isPromiseFunction && shouldTrackIsValidatingState) _updateIsValidating([_f.name]);
					if (fieldError[_f.name]) {
						context.valid = false;
						if (onlyCheckValid) break;
					}
					!onlyCheckValid && (get(fieldError, _f.name) ? isFieldArrayRoot ? updateFieldArrayRootError(_formState.errors, fieldError, _f.name) : set(_formState.errors, _f.name, fieldError[_f.name]) : unset(_formState.errors, _f.name));
					if (props.shouldUseNativeValidation && fieldError[_f.name]) break;
				}
				!isEmptyObject(fieldValue) && await executeBuiltInValidation({
					context,
					onlyCheckValid,
					fields: fieldValue,
					name,
					eventType
				});
			}
		}
		return context.valid;
	};
	const _removeUnmounted = () => {
		for (const name of _names.unMount) {
			const field = get(_fields, name);
			field && (field._f.refs ? field._f.refs.every((ref) => !live(ref)) : !live(field._f.ref)) && unregister(name);
		}
		_names.unMount = /* @__PURE__ */ new Set();
	};
	const _getDirty = (name, data) => (name && data && set(_formValues, name, data), !deepEqual(_state.mount ? _formValues : _defaultValues, _defaultValues));
	const _getWatch = (names, defaultValue, isGlobal) => generateWatchOutput(names, _names, { ..._state.mount ? _formValues : isUndefined(defaultValue) || isString(names) ? _defaultValues : defaultValue }, isGlobal, defaultValue);
	const _getFieldArray = (name) => compact(get(_state.mount ? _formValues : _defaultValues, name, _options.shouldUnregister ? get(_defaultValues, name, []) : []));
	const setFieldValue = (name, value, options = {}, skipClone = false, skipRender = false, skipValueRender = false) => {
		const field = get(_fields, name);
		let fieldValue = value;
		if (field) {
			const fieldReference = field._f;
			if (fieldReference) {
				!fieldReference.disabled && set(_formValues, name, getFieldValueAs(value, fieldReference));
				fieldValue = isHTMLElement(fieldReference.ref) && isNullOrUndefined(value) ? "" : value;
				if (isMultipleSelect(fieldReference.ref)) [...fieldReference.ref.options].forEach((optionRef) => optionRef.selected = fieldValue.includes(optionRef.value));
				else if (fieldReference.refs) {
					if (isCheckBoxInput(fieldReference.ref)) fieldReference.refs.forEach((checkboxRef) => {
						if (!checkboxRef.defaultChecked || !checkboxRef.disabled) {
							if (Array.isArray(fieldValue)) checkboxRef.checked = !!fieldValue.find((data) => data === checkboxRef.value);
							else checkboxRef.checked = fieldValue === checkboxRef.value || !!fieldValue;
						}
					});
					else fieldReference.refs.forEach((radioRef) => radioRef.checked = radioRef.value === fieldValue);
				} else if (isFileInput(fieldReference.ref)) fieldReference.ref.value = "";
				else {
					fieldReference.ref.value = fieldValue;
					if (!fieldReference.ref.type && !skipRender && !skipValueRender) _subjects.state.next({
						name,
						values: skipClone ? _formValues : cloneObject(_formValues)
					});
				}
			}
		}
		(options.shouldDirty || options.shouldTouch) && updateTouchAndDirty(name, fieldValue, options.shouldTouch, options.shouldDirty, !skipRender);
		options.shouldValidate && trigger(name, { delayError: options.delayError });
	};
	const setFieldValues = (name, value, options, skipClone = false, skipRender = false, skipValueRender = false) => {
		if (_names.array.has(name)) _subjects.array.next({
			name,
			values: skipClone ? _formValues : cloneObject(_formValues)
		});
		for (const fieldKey in value) {
			if (!value.hasOwnProperty(fieldKey)) continue;
			const fieldValue = value[fieldKey];
			const fieldName = name + "." + fieldKey;
			const field = get(_fields, fieldName);
			(_names.array.has(name) || isObject$1(fieldValue) || field && !field._f) && !isDateObject(fieldValue) ? setFieldValues(fieldName, fieldValue, options, skipClone, skipRender, skipValueRender) : setFieldValue(fieldName, fieldValue, options, skipClone, skipRender, skipValueRender);
		}
	};
	const _setValue = (name, value, options, skipClone, skipStateEmit = false) => {
		const field = get(_fields, name);
		const isFieldArray = _names.array.has(name);
		const cloneValue = skipClone ? value : cloneObject(value);
		const isValueUnchanged = deepEqual(get(_formValues, name), cloneValue);
		if (!isValueUnchanged) set(_formValues, name, cloneValue);
		if (isFieldArray) {
			_subjects.array.next({
				name,
				values: skipClone ? _formValues : cloneObject(_formValues)
			});
			if ((_proxyFormState.isDirty || _proxyFormState.dirtyFields || _proxySubscribeFormState.isDirty || _proxySubscribeFormState.dirtyFields) && options.shouldDirty) {
				_updateDirtyFields();
				if (!skipStateEmit) _subjects.state.next({
					name,
					dirtyFields: _formState.dirtyFields,
					isDirty: _getDirty(name, cloneValue)
				});
			}
		} else {
			const isEmpty = Array.isArray(cloneValue) && !cloneValue.length || isEmptyObject(cloneValue);
			const skipValueRender = !isValueUnchanged && !skipStateEmit;
			if (!field || field._f || isNullOrUndefined(cloneValue) || isEmpty) setFieldValue(name, cloneValue, options, skipClone, skipStateEmit, skipValueRender);
			else setFieldValues(name, cloneValue, options, skipClone, skipStateEmit, skipValueRender);
		}
		if (!isValueUnchanged && !skipStateEmit) {
			const watched = isWatched(name, _names);
			const values = skipClone ? _formValues : cloneObject(_formValues);
			_subjects.state.next({
				...watched && _formState,
				name: _state.mount || watched ? name : void 0,
				values
			});
			if (!isFieldArray) for (const itemName of getFieldArrayItemNames(_names.array, name)) _subjects.state.next({
				name: itemName,
				values
			});
		}
	};
	const setValue = (name, value, options = {}) => _setValue(name, value, options, false);
	const setValues = (formValues, options = {}) => {
		const updatedFormValues = isFunction(formValues) ? formValues(_formValues) : formValues;
		if (!deepEqual(_formValues, updatedFormValues)) {
			_formValues = {
				..._formValues,
				...updatedFormValues
			};
			for (const fieldName of _names.mount) if (has(updatedFormValues, fieldName)) _setValue(fieldName, get(updatedFormValues, fieldName), options, true, true);
			_subjects.state.next({
				..._formState,
				name: void 0,
				type: void 0,
				..._valuesSubscriberCount ? { values: _formValues } : {}
			});
			if (options.shouldValidate) _setValid();
		}
	};
	const onChange = async (event) => {
		_state.mount = true;
		const target = event.target;
		let name = target.name;
		let isFieldValueUpdated = true;
		const field = get(_fields, name);
		const _updateIsFieldValueUpdated = (fieldValue) => {
			isFieldValueUpdated = Number.isNaN(fieldValue) || isDateObject(fieldValue) && isNaN(fieldValue.getTime()) || deepEqual(fieldValue, get(_formValues, name, fieldValue));
		};
		if (field) {
			let error;
			let isValid;
			const fieldValue = target.type ? getFieldValue(field._f) : getEventValue(event);
			const isBlurEvent = event.type === EVENTS.BLUR || event.type === EVENTS.FOCUS_OUT;
			const hasNoValidationEffect = !hasValidation(field._f) && !props.validate && !_options.resolver && !get(_formState.errors, name) && !field._f.deps;
			const shouldSkipValidation = hasNoValidationEffect || skipValidation(isBlurEvent, get(_formState.touchedFields, name), _formState.isSubmitted, _validationModeAfterSubmit, _validationModeBeforeSubmit);
			const watched = isWatched(name, _names, isBlurEvent);
			set(_formValues, name, cloneObject(fieldValue));
			if (isBlurEvent) {
				if (!target || !target.readOnly) {
					field._f.onBlur && field._f.onBlur(event);
					const pendingDelayError = delayErrorCallbacks[name];
					pendingDelayError && pendingDelayError(0);
				}
			} else if (field._f.onChange) field._f.onChange(event);
			const fieldState = updateTouchAndDirty(name, fieldValue, isBlurEvent);
			const shouldRender = !isEmptyObject(fieldState) || watched;
			!isBlurEvent && _subjects.state.next({
				name,
				type: event.type,
				..._valuesSubscriberCount ? { values: cloneObject(_formValues) } : {}
			});
			if (shouldSkipValidation) {
				if ((!hasNoValidationEffect || !_formState.isValid) && (_proxyFormState.isValid || _proxySubscribeFormState.isValid)) {
					if (_options.mode === "onBlur") {
						if (isBlurEvent) _setValid();
					} else if (!isBlurEvent) _setValid();
				}
				return shouldRender && _subjects.state.next({
					name,
					...watched ? {} : fieldState
				});
			}
			if (!_options.resolver && props.validate) await validateForm({
				name,
				eventType: event.type
			});
			!isBlurEvent && watched && _subjects.state.next({ ..._formState });
			if (_options.resolver) {
				const { errors } = await _runSchema([name]);
				_updateIsValidating([name]);
				_updateIsFieldValueUpdated(fieldValue);
				if (!isFieldValueUpdated) {
					!isEmptyObject(fieldState) && _subjects.state.next(fieldState);
					return;
				}
				const previousErrorLookupResult = schemaErrorLookup(_formState.errors, _fields, name);
				const errorLookupResult = schemaErrorLookup(errors, _fields, previousErrorLookupResult.name || name);
				error = errorLookupResult.error;
				name = errorLookupResult.name;
				isValid = isEmptyObject(errors);
			} else {
				_updateIsValidating([name], true);
				error = (await validateField(field, _names.disabled, _formValues, shouldDisplayAllAssociatedErrors, _options.shouldUseNativeValidation))[name];
				_updateIsValidating([name]);
				_updateIsFieldValueUpdated(fieldValue);
				if (isFieldValueUpdated) {
					if (error) isValid = false;
					else if (_proxyFormState.isValid || _proxySubscribeFormState.isValid) isValid = await executeBuiltInValidation({
						fields: _fields,
						onlyCheckValid: true,
						name,
						eventType: event.type
					});
				}
			}
			if (isFieldValueUpdated) {
				field._f.deps && (!Array.isArray(field._f.deps) || field._f.deps.length > 0) && trigger(field._f.deps);
				shouldRenderByError(name, isValid, error, fieldState);
			}
		}
	};
	const _focusInput = (ref, key) => {
		if (get(_formState.errors, key) && ref.focus) {
			ref.focus();
			return 1;
		}
	};
	const trigger = async (name, options = {}) => {
		let isValid;
		let validationResult;
		const fieldNames = convertToArrayPayload(name);
		if (_options.resolver) {
			const errors = await executeSchemaAndUpdateState(isUndefined(name) ? name : fieldNames);
			isValid = isEmptyObject(errors);
			validationResult = name ? !fieldNames.some((name) => get(errors, name)) : isValid;
		} else if (name) {
			validationResult = (await Promise.all(fieldNames.map(async (fieldName) => {
				const field = get(_fields, fieldName);
				return await executeBuiltInValidation({
					fields: field && field._f ? { [fieldName]: field } : field,
					eventType: EVENTS.TRIGGER
				});
			}))).every(Boolean);
			!(!validationResult && !_formState.isValid) && _setValid();
		} else validationResult = isValid = await executeBuiltInValidation({
			fields: _fields,
			name,
			eventType: EVENTS.TRIGGER
		});
		if (options.delayError && _options.delayError && isString(name)) {
			const error = get(_formState.errors, name);
			if (error) {
				unset(_formState.errors, name);
				delayErrorCallbacks[name] = debounce(name, () => updateErrors(name, error));
				delayErrorCallbacks[name](_options.delayError);
			} else {
				clearTimeout(timers[name]);
				delete delayErrorCallbacks[name];
			}
		}
		if (options.shouldTouch) for (const fieldName of name ? fieldNames : _names.mount) !_names.array.has(fieldName) && set(_formState.touchedFields, fieldName, true);
		_subjects.state.next({
			...!isString(name) || (_proxyFormState.isValid || _proxySubscribeFormState.isValid) && isValid !== _formState.isValid ? {} : { name },
			..._options.resolver || !name ? { isValid } : {},
			...options.shouldTouch && (_proxyFormState.touchedFields || _proxySubscribeFormState.touchedFields) ? { touchedFields: _formState.touchedFields } : {},
			errors: _formState.errors
		});
		options.shouldFocus && !validationResult && iterateFieldsByAction(_fields, _focusInput, name ? fieldNames : _names.mount);
		return validationResult;
	};
	const getValues = (fieldNames, config) => {
		let values = { ..._state.mount ? _formValues : _defaultValues };
		if (config) values = extractFormValues(config.dirtyFields ? _formState.dirtyFields : _formState.touchedFields, values);
		return isUndefined(fieldNames) ? values : isString(fieldNames) ? get(values, fieldNames) : fieldNames.map((name) => get(values, name));
	};
	const getErrors = (fieldNames) => isUndefined(fieldNames) ? { ..._formState.errors } : isString(fieldNames) ? get(_formState.errors, fieldNames) : fieldNames.map((name) => get(_formState.errors, name));
	const getFieldState = (name, formState) => {
		const targetFormState = formState || _formState;
		const error = get(targetFormState.errors, name);
		return {
			invalid: !!error,
			isDirty: !!get(targetFormState.dirtyFields, name),
			error,
			isValidating: !!get(_formState.validatingFields, name),
			isTouched: !!get(targetFormState.touchedFields, name)
		};
	};
	const clearErrors = (name) => {
		const names = name ? convertToArrayPayload(name) : void 0;
		names === null || names === void 0 || names.forEach((inputName) => unset(_formState.errors, inputName));
		if (names) names.forEach((inputName) => {
			_subjects.state.next({
				name: inputName,
				errors: _formState.errors
			});
		});
		else {
			_formState.errors = {};
			_subjects.state.next({ errors: _formState.errors });
		}
	};
	const setError = (name, error, options) => {
		const ref = (get(_fields, name, { _f: {} })._f || {}).ref;
		const { ref: currentRef, message, type, ...restOfErrorTree } = get(_formState.errors, name) || {};
		set(_formState.errors, name, {
			...restOfErrorTree,
			...error,
			ref
		});
		_subjects.state.next({
			name,
			errors: _formState.errors,
			isValid: false
		});
		options && options.shouldFocus && ref && ref.focus && ref.focus();
	};
	const watch = (name, defaultValue) => {
		if (isFunction(name)) {
			_valuesSubscriberCount++;
			const { unsubscribe } = _subjects.state.subscribe({ next: (payload) => "values" in payload && name(payload.values || _getWatch(void 0, defaultValue), payload) });
			let called = false;
			return { unsubscribe: () => {
				if (called) return;
				called = true;
				_valuesSubscriberCount--;
				unsubscribe();
			} };
		}
		return _getWatch(name, defaultValue, true);
	};
	const _subscribe = (props) => {
		var _a;
		const needsValues = !!((_a = props.formState) === null || _a === void 0 ? void 0 : _a.values);
		if (needsValues) _valuesSubscriberCount++;
		const { unsubscribe } = _subjects.state.subscribe({ next: (formState) => {
			if (shouldSubscribeByName(props.name, formState.name, props.exact) && shouldRenderFormState(formState, props.formState || _proxyFormState, _setFormState, props.reRenderRoot)) {
				const snapshot = { ..._formValues };
				props.callback({
					values: snapshot,
					..._formState,
					...formState,
					defaultValues: _defaultValues
				});
			}
		} });
		if (!needsValues) return unsubscribe;
		let called = false;
		return () => {
			if (called) return;
			called = true;
			_valuesSubscriberCount--;
			unsubscribe();
		};
	};
	const subscribe = (props) => {
		_state.mount = true;
		_proxySubscribeFormState = {
			..._proxySubscribeFormState,
			...props.formState
		};
		return _subscribe({
			...props,
			formState: {
				...defaultProxyFormState,
				...props.formState
			}
		});
	};
	const unregister = (name, options = {}) => {
		for (const fieldName of name ? convertToArrayPayload(name) : _names.mount) {
			_names.mount.delete(fieldName);
			_names.array.delete(fieldName);
			if (!options.keepValue) {
				unset(_fields, fieldName);
				unset(_formValues, fieldName);
			}
			!options.keepError && unset(_formState.errors, fieldName);
			!options.keepDirty && unset(_formState.dirtyFields, fieldName);
			!options.keepTouched && unset(_formState.touchedFields, fieldName);
			!options.keepIsValidating && unset(_formState.validatingFields, fieldName);
			!_options.shouldUnregister && !options.keepDefaultValue && unset(_defaultValues, fieldName);
		}
		_valuesSubscriberCount && _subjects.state.next({ values: cloneObject(_formValues) });
		_subjects.state.next({
			..._formState,
			...options.keepDirty ? {} : { isDirty: _getDirty() }
		});
		!options.keepIsValid && _setValid();
	};
	const _setDisabledField = ({ disabled, name }) => {
		if (isBoolean(disabled) && _state.mount || !!disabled || _names.disabled.has(name)) {
			const disabledStateChanged = _names.disabled.has(name) !== !!disabled;
			disabled ? _names.disabled.add(name) : _names.disabled.delete(name);
			disabledStateChanged && _state.mount && !_state.action && _setValid();
		}
	};
	const register = (name, options = {}) => {
		let field = get(_fields, name);
		const disabledIsDefined = isBoolean(options.disabled) || isBoolean(_options.disabled);
		const shouldRevalidateRemount = !_names.registerName.has(name) && field && field._f && !field._f.mount;
		set(_fields, name, {
			...field || {},
			_f: {
				...field && field._f ? field._f : { ref: { name } },
				name,
				mount: true,
				...options
			}
		});
		_names.mount.add(name);
		if (field && !shouldRevalidateRemount) _setDisabledField({
			disabled: isBoolean(options.disabled) ? options.disabled : _options.disabled,
			name
		});
		else updateValidAndValue(name, true, options.value);
		return {
			...disabledIsDefined ? { disabled: options.disabled || _options.disabled } : {},
			..._options.progressive ? {
				required: !!options.required,
				min: getRuleValue(options.min),
				max: getRuleValue(options.max),
				minLength: getRuleValue(options.minLength),
				maxLength: getRuleValue(options.maxLength),
				pattern: getRuleValue(options.pattern)
			} : {},
			name,
			onChange,
			onBlur: onChange,
			ref: (ref) => {
				if (ref) {
					_names.registerName.add(name);
					register(name, options);
					_names.registerName.delete(name);
					field = get(_fields, name);
					const fieldRef = isUndefined(ref.value) ? ref.querySelectorAll ? ref.querySelectorAll("input,select,textarea")[0] || ref : ref : ref;
					const radioOrCheckbox = isRadioOrCheckbox(fieldRef);
					const refs = field._f.refs || [];
					if (radioOrCheckbox ? refs.find((option) => option === fieldRef) : fieldRef === field._f.ref) return;
					const newField = { ...field._f };
					if (radioOrCheckbox) {
						newField.refs = [
							...refs.filter(live),
							fieldRef,
							...Array.isArray(get(_defaultValues, name)) ? [{}] : []
						];
						newField.ref = {
							type: fieldRef.type,
							name
						};
					} else {
						newField.ref = fieldRef;
						delete newField.refs;
					}
					set(_fields, name, { _f: newField });
					updateValidAndValue(name, false, void 0, fieldRef);
				} else {
					field = get(_fields, name, {});
					if (field._f) field._f.mount = false;
					(_options.shouldUnregister || options.shouldUnregister) && !(isNameInFieldArray(_names.array, name) && _state.action) && _names.unMount.add(name);
				}
			}
		};
	};
	const _focusError = () => _options.shouldFocusError && !_options.shouldUseNativeValidation && iterateFieldsByAction(_fields, _focusInput, _names.mount);
	const _disableForm = (disabled) => {
		if (isBoolean(disabled)) {
			_subjects.state.next({ disabled });
			iterateFieldsByAction(_fields, (ref, name) => {
				const currentField = get(_fields, name);
				if (currentField) {
					ref.disabled = currentField._f.disabled || disabled;
					if (Array.isArray(currentField._f.refs)) currentField._f.refs.forEach((inputRef) => {
						inputRef.disabled = currentField._f.disabled || disabled;
					});
				}
			}, 0, false);
		}
	};
	const handleSubmit = (onValid, onInvalid) => async (e) => {
		let result = void 0;
		let onValidError = void 0;
		if (e) {
			e.preventDefault && e.preventDefault();
			e.persist && e.persist();
		}
		let fieldValues = cloneObject(_formValues);
		_subjects.state.next({ isSubmitting: true });
		if (_options.resolver) {
			const { errors, values } = await _runSchema();
			_updateIsValidating();
			_formState.errors = errors;
			fieldValues = cloneObject(values);
		} else await executeBuiltInValidation({
			fields: _fields,
			eventType: EVENTS.SUBMIT
		});
		if (_names.disabled.size) for (const name of _names.disabled) unset(fieldValues, name);
		unset(_formState.errors, ROOT_ERROR_TYPE);
		if (isEmptyObject(_formState.errors)) {
			_subjects.state.next({ errors: {} });
			try {
				result = await onValid(fieldValues, e);
			} catch (error) {
				onValidError = error;
			}
		} else {
			if (onInvalid) await onInvalid({ ..._formState.errors }, e);
			_focusError();
			setTimeout(_focusError);
		}
		_subjects.state.next({
			isSubmitted: true,
			isSubmitting: false,
			isSubmitSuccessful: isEmptyObject(_formState.errors) && !onValidError,
			submitCount: _formState.submitCount + 1,
			errors: _formState.errors
		});
		if (onValidError) throw onValidError;
		return result;
	};
	const resetField = (name, options = {}) => {
		if (get(_fields, name)) {
			if (isUndefined(options.defaultValue)) setValue(name, cloneObject(get(_defaultValues, name)));
			else {
				setValue(name, options.defaultValue);
				set(_defaultValues, name, cloneObject(options.defaultValue));
			}
			if (!options.keepTouched) unset(_formState.touchedFields, name);
			if (!options.keepDirty) {
				unset(_formState.dirtyFields, name);
				_formState.isDirty = options.defaultValue ? _getDirty(name, cloneObject(get(_defaultValues, name))) : _getDirty();
			}
			if (!options.keepError) {
				unset(_formState.errors, name);
				_setValid();
			}
			_subjects.state.next({ ..._formState });
		}
	};
	const _reset = (formValues, keepStateOptions = {}) => {
		const updatedValues = formValues ? cloneObject(formValues) : _defaultValues;
		const cloneUpdatedValues = cloneObject(updatedValues);
		const isEmptyResetValues = isEmptyObject(formValues);
		const values = cloneUpdatedValues;
		const fieldRefs = _fields;
		if (!keepStateOptions.keepDefaultValues) _defaultValues = updatedValues;
		if (!keepStateOptions.keepValues) {
			if (keepStateOptions.keepDirtyValues) {
				const fieldsToCheck = /* @__PURE__ */ new Set([..._names.mount, ...collectDirtyFieldNames(getDirtyFields(_defaultValues, _formValues, void 0, fieldRefs), _formState.dirtyFields)]);
				for (const fieldName of fieldsToCheck) {
					const isDirty = get(_formState.dirtyFields, fieldName);
					const existingValue = get(_formValues, fieldName);
					const newValue = get(values, fieldName);
					if (isDirty && !isUndefined(existingValue)) set(values, fieldName, existingValue);
					else if (!isDirty && !isUndefined(newValue)) setValue(fieldName, newValue);
				}
			} else {
				if (isWeb && isUndefined(formValues)) for (const name of _names.mount) {
					const field = get(_fields, name);
					if (field && field._f) {
						const fieldReference = Array.isArray(field._f.refs) ? field._f.refs[0] : field._f.ref;
						if (isHTMLElement(fieldReference)) {
							const form = fieldReference.closest("form");
							if (form) {
								form.reset();
								break;
							}
						}
					}
				}
				if (keepStateOptions.keepFieldsRef) for (const fieldName of _names.mount) setValue(fieldName, get(values, fieldName));
				else _fields = {};
			}
			if (_options.shouldUnregister) {
				_formValues = keepStateOptions.keepDefaultValues ? cloneObject(_defaultValues) : {};
				if (keepStateOptions.keepFieldsRef) for (const fieldName of _names.mount) set(_formValues, fieldName, get(values, fieldName));
			} else _formValues = cloneObject(values);
			_subjects.array.next({ values: { ...values } });
			_subjects.state.next({
				name: void 0,
				type: void 0,
				values: { ...values }
			});
		}
		_names = {
			mount: keepStateOptions.keepDirtyValues ? _names.mount : /* @__PURE__ */ new Set(),
			unMount: /* @__PURE__ */ new Set(),
			array: /* @__PURE__ */ new Set(),
			registerName: /* @__PURE__ */ new Set(),
			disabled: /* @__PURE__ */ new Set(),
			watch: /* @__PURE__ */ new Set(),
			watchAll: false,
			focus: ""
		};
		_state.mount = !_proxyFormState.isValid || !!keepStateOptions.keepIsValid || !!keepStateOptions.keepDirtyValues || !_options.shouldUnregister && !isEmptyObject(values);
		_state.watch = !!_options.shouldUnregister;
		_state.keepIsValid = !!keepStateOptions.keepIsValid;
		_state.action = false;
		_state.actionArrayLengths.clear();
		if (!keepStateOptions.keepErrors) _formState.errors = {};
		_subjects.state.next({
			submitCount: keepStateOptions.keepSubmitCount ? _formState.submitCount : 0,
			isDirty: isEmptyResetValues ? false : keepStateOptions.keepDirty ? _formState.isDirty : keepStateOptions.keepValues ? _getDirty() : !!(keepStateOptions.keepDefaultValues && !deepEqual(formValues, _defaultValues)),
			isSubmitted: keepStateOptions.keepIsSubmitted ? _formState.isSubmitted : false,
			dirtyFields: isEmptyResetValues ? {} : keepStateOptions.keepDirtyValues ? keepStateOptions.keepDefaultValues && _formValues ? getDirtyFields(_defaultValues, _formValues, void 0, fieldRefs) : _formState.dirtyFields : keepStateOptions.keepDefaultValues && formValues ? getDirtyFields(_defaultValues, formValues, void 0, fieldRefs) : keepStateOptions.keepDirty ? _formState.dirtyFields : {},
			touchedFields: keepStateOptions.keepTouched ? _formState.touchedFields : {},
			errors: keepStateOptions.keepErrors ? _formState.errors : {},
			isSubmitSuccessful: keepStateOptions.keepIsSubmitSuccessful ? _formState.isSubmitSuccessful : false,
			isSubmitting: false,
			defaultValues: _defaultValues
		});
	};
	const reset = (formValues, keepStateOptions) => _reset(isFunction(formValues) ? formValues(_formValues) : formValues, {
		..._options.resetOptions,
		...keepStateOptions
	});
	const setFocus = (name, options = {}) => {
		const field = get(_fields, name);
		const fieldReference = field && field._f;
		if (fieldReference) {
			const fieldRef = fieldReference.refs ? fieldReference.refs[0] : fieldReference.ref;
			if (fieldRef.focus) setTimeout(() => {
				fieldRef.focus();
				options.shouldSelect && isFunction(fieldRef.select) && fieldRef.select();
			});
		}
	};
	const _setFormState = (updatedFormState) => {
		const { name, type, values, ...formState } = updatedFormState;
		_formState = {
			..._formState,
			...formState
		};
	};
	_subjects.state.subscribe({ next: _setFormState });
	const _resetDefaultValues = () => isFunction(_options.defaultValues) && _options.defaultValues().then((values) => {
		reset(values, _options.resetOptions);
		_subjects.state.next({ isLoading: false });
	});
	const resetDefaultValues = (values, options = {}) => {
		_defaultValues = cloneObject(values);
		if (!options.keepDirty) {
			const newDirtyFields = getDirtyFields(_defaultValues, _formValues, void 0, _fields);
			_formState.dirtyFields = newDirtyFields;
			_formState.isDirty = !isEmptyObject(newDirtyFields);
		}
		if (!options.keepIsValid) _setValid();
		_subjects.state.next({
			..._formState,
			defaultValues: _defaultValues
		});
	};
	const methods = {
		control: {
			register,
			unregister,
			getFieldState,
			handleSubmit,
			setError,
			_subscribe,
			_runSchema,
			_updateIsValidating,
			_focusError,
			_getWatch,
			_getDirty,
			_setValid,
			_setFieldArray,
			_setDisabledField,
			_setErrors,
			_getFieldArray,
			_reset,
			_resetDefaultValues,
			_removeUnmounted,
			_disableForm,
			_subjects,
			_proxyFormState,
			get _fields() {
				return _fields;
			},
			get _formValues() {
				return _formValues;
			},
			get _state() {
				return _state;
			},
			set _state(value) {
				_state = value;
			},
			get _defaultValues() {
				return _defaultValues;
			},
			get _names() {
				return _names;
			},
			set _names(value) {
				_names = value;
			},
			get _formState() {
				return _formState;
			},
			get _options() {
				return _options;
			},
			set _options(value) {
				_options = {
					..._options,
					...value
				};
				_validationModeBeforeSubmit = getValidationModes(_options.mode);
				_validationModeAfterSubmit = getValidationModes(_options.reValidateMode);
			}
		},
		subscribe,
		trigger,
		register,
		handleSubmit,
		watch,
		setValue,
		setValues,
		getValues,
		getErrors,
		reset,
		resetField,
		resetDefaultValues,
		clearErrors,
		unregister,
		setError,
		setFocus,
		getFieldState
	};
	return {
		...methods,
		formControl: methods
	};
}
/**
* Custom hook to manage the entire form.
*
* @remarks
* [API](https://react-hook-form.com/docs/useform) • [Demo](https://codesandbox.io/s/react-hook-form-get-started-ts-5ksmm) • [Video](https://www.youtube.com/watch?v=RkXv4AXXC_4)
*
* @param props - form configuration and validation parameters.
*
* @returns methods - individual functions to manage the form state. {@link UseFormReturn}
*
* @example
* ```tsx
* function App() {
*   const { register, handleSubmit, watch, formState: { errors } } = useForm();
*   const onSubmit = data => console.log(data);
*
*   console.log(watch("example"));
*
*   return (
*     <form onSubmit={handleSubmit(onSubmit)}>
*       <input defaultValue="test" {...register("example")} />
*       <input {...register("exampleRequired", { required: true })} />
*       {errors.exampleRequired && <span>This field is required</span>}
*       <button>Submit</button>
*     </form>
*   );
* }
* ```
*/
function useForm(props = {}) {
	const _formControl = import_react.useRef(void 0);
	const _values = import_react.useRef(void 0);
	const _formControlProp = import_react.useRef(props.formControl);
	const [formState, updateFormState] = import_react.useState(() => ({
		...cloneObject(DEFAULT_FORM_STATE),
		isLoading: isFunction(props.defaultValues),
		errors: props.errors || {},
		disabled: props.disabled || false,
		defaultValues: isFunction(props.defaultValues) ? void 0 : props.defaultValues
	}));
	if (!_formControl.current || props.formControl && _formControlProp.current !== props.formControl) {
		_formControlProp.current = props.formControl;
		if (props.formControl) {
			_formControl.current = {
				...props.formControl,
				formState
			};
			if (props.defaultValues && !isFunction(props.defaultValues)) props.formControl.reset(props.defaultValues, props.resetOptions);
		} else {
			const { formControl, ...rest } = createFormControl(props);
			_formControl.current = {
				...rest,
				formState
			};
		}
	}
	const control = _formControl.current.control;
	control._options = props;
	const { resyncIfNeeded, snapshot } = useResyncOnReconnect();
	useIsomorphicLayoutEffect(() => {
		const getCurrentFormState = () => ({
			...control._formState,
			defaultValues: control._defaultValues
		});
		resyncIfNeeded(true, getCurrentFormState, updateFormState);
		const unsubscribe = control._subscribe({
			formState: control._proxyFormState,
			callback: () => updateFormState({
				...control._formState,
				defaultValues: control._defaultValues
			}),
			reRenderRoot: true
		});
		updateFormState((data) => ({
			...data,
			isReady: true
		}));
		control._formState.isReady = true;
		return () => {
			unsubscribe();
			snapshot(true, getCurrentFormState);
		};
	}, [
		control,
		resyncIfNeeded,
		snapshot
	]);
	import_react.useEffect(() => control._disableForm(props.disabled), [control, props.disabled]);
	import_react.useEffect(() => {
		if (props.mode) control._options.mode = props.mode;
		if (props.reValidateMode) control._options.reValidateMode = props.reValidateMode;
	}, [
		control,
		props.mode,
		props.reValidateMode
	]);
	import_react.useEffect(() => {
		if (props.errors) {
			control._setErrors(props.errors);
			control._focusError();
		}
	}, [control, props.errors]);
	import_react.useEffect(() => {
		props.shouldUnregister && control._subjects.state.next({ values: control._getWatch() });
	}, [control, props.shouldUnregister]);
	import_react.useEffect(() => {
		if (control._proxyFormState.isDirty) {
			const isDirty = control._getDirty();
			if (isDirty !== formState.isDirty) control._subjects.state.next({ isDirty });
		}
	}, [control, formState.isDirty]);
	import_react.useEffect(() => {
		var _a;
		if (props.values && !deepEqual(props.values, _values.current)) {
			control._reset(props.values, {
				keepFieldsRef: true,
				...control._options.resetOptions
			});
			if (!((_a = control._options.resetOptions) === null || _a === void 0 ? void 0 : _a.keepIsValid)) control._setValid();
			_values.current = props.values;
			updateFormState((state) => ({ ...state }));
		} else control._resetDefaultValues();
	}, [control, props.values]);
	import_react.useEffect(() => {
		if (!control._state.mount) {
			control._setValid();
			control._state.mount = true;
		}
		if (control._state.watch) {
			control._state.watch = false;
			control._subjects.state.next({ ...control._formState });
		}
		control._removeUnmounted();
	});
	_formControl.current.formState = import_react.useMemo(() => getProxyFormState(formState, control), [control, formState]);
	return _formControl.current;
}
//#endregion
//#region node_modules/zod/v4/core/util.js
function getEnumValues(entries) {
	const numericValues = Object.values(entries).filter((v) => typeof v === "number");
	return Object.entries(entries).filter(([k, _]) => numericValues.indexOf(+k) === -1).map(([_, v]) => v);
}
function joinValues(array, separator = "|") {
	return array.map((val) => stringifyPrimitive(val)).join(separator);
}
function jsonStringifyReplacer(_, value) {
	if (typeof value === "bigint") return value.toString();
	return value;
}
function cached(getter) {
	return { get value() {
		{
			const value = getter();
			Object.defineProperty(this, "value", { value });
			return value;
		}
	} };
}
function nullish(input) {
	return input === null || input === void 0;
}
function cleanRegex(source) {
	const start = source.startsWith("^") ? 1 : 0;
	const end = source.endsWith("$") ? source.length - 1 : source.length;
	return source.slice(start, end);
}
function floatSafeRemainder(val, step) {
	const ratio = val / step;
	const roundedRatio = Math.round(ratio);
	const tolerance = 4 * Number.EPSILON * Math.max(Math.abs(ratio), 1);
	if (Math.abs(ratio - roundedRatio) < tolerance) return 0;
	return ratio - roundedRatio;
}
function assignProp(target, prop, value) {
	Object.defineProperty(target, prop, {
		value,
		writable: true,
		enumerable: true,
		configurable: true
	});
}
function mergeDefs(...defs) {
	const mergedDescriptors = {};
	for (const def of defs) {
		const descriptors = Object.getOwnPropertyDescriptors(def);
		Object.assign(mergedDescriptors, descriptors);
	}
	return Object.defineProperties({}, mergedDescriptors);
}
function esc(str) {
	return JSON.stringify(str);
}
function slugify(input) {
	return input.toLowerCase().trim().replace(/[^\w\s-]/g, "").replace(/[\s_-]+/g, "-").replace(/^-+|-+$/g, "");
}
var captureStackTrace = "captureStackTrace" in Error ? Error.captureStackTrace : (..._args) => {};
function isObject(data) {
	return typeof data === "object" && data !== null && !Array.isArray(data);
}
var allowsEval = /* @__PURE__*/ cached(() => {
	if (globalConfig.jitless) return false;
	if (typeof navigator !== "undefined" && navigator?.userAgent?.includes("Cloudflare")) return false;
	try {
		new Function("");
		return true;
	} catch (_) {
		return false;
	}
});
function isPlainObject(o) {
	if (isObject(o) === false) return false;
	const ctor = o.constructor;
	if (ctor === void 0) return true;
	if (typeof ctor !== "function") return true;
	const prot = ctor.prototype;
	if (isObject(prot) === false) return false;
	if (Object.prototype.hasOwnProperty.call(prot, "isPrototypeOf") === false) return false;
	return true;
}
function shallowClone(o) {
	if (isPlainObject(o)) return { ...o };
	if (Array.isArray(o)) return [...o];
	if (o instanceof Map) return new Map(o);
	if (o instanceof Set) return new Set(o);
	return o;
}
var propertyKeyTypes = /* @__PURE__*/ new Set([
	"string",
	"number",
	"symbol"
]);
function escapeRegex(str) {
	return str.replace(/[.*+?^${}()|[\]\\]/g, "\\$&");
}
function clone(inst, def, params) {
	const cl = new inst._zod.constr(def ?? inst._zod.def);
	if (!def || params?.parent) cl._zod.parent = inst;
	return cl;
}
function normalizeParams(_params) {
	const params = _params;
	if (!params) return {};
	if (typeof params === "string") return { error: () => params };
	if (params?.message !== void 0) {
		if (params?.error !== void 0) throw new Error("Cannot specify both `message` and `error` params");
		params.error = params.message;
	}
	delete params.message;
	if (typeof params.error === "string") return {
		...params,
		error: () => params.error
	};
	return params;
}
function stringifyPrimitive(value) {
	if (typeof value === "bigint") return value.toString() + "n";
	if (typeof value === "string") return `"${value}"`;
	return `${value}`;
}
function optionalKeys(shape) {
	return Object.keys(shape).filter((k) => {
		return shape[k]._zod.optin !== void 0 && shape[k]._zod.optout === "optional";
	});
}
var NUMBER_FORMAT_RANGES = /*@__PURE__*/ (() => ({
	safeint: [Number.MIN_SAFE_INTEGER, Number.MAX_SAFE_INTEGER],
	int32: [-2147483648, 2147483647],
	uint32: [0, 4294967295],
	float32: [-34028234663852886e22, 34028234663852886e22],
	float64: [-Number.MAX_VALUE, Number.MAX_VALUE]
}))();
function pick(schema, mask) {
	const currDef = schema._zod.def;
	const checks = currDef.checks;
	if (checks && checks.length > 0) throw new Error(".pick() cannot be used on object schemas containing refinements");
	return clone(schema, mergeDefs(schema._zod.def, {
		get shape() {
			const newShape = {};
			for (const key of Reflect.ownKeys(mask)) {
				if (!Object.prototype.hasOwnProperty.call(currDef.shape, key)) throw new Error(`Unrecognized key: "${String(key)}"`);
				if (!mask[key]) continue;
				assignProp(newShape, key, currDef.shape[key]);
			}
			assignProp(this, "shape", newShape);
			return newShape;
		},
		checks: []
	}));
}
function omit(schema, mask) {
	const currDef = schema._zod.def;
	const checks = currDef.checks;
	if (checks && checks.length > 0) throw new Error(".omit() cannot be used on object schemas containing refinements");
	return clone(schema, mergeDefs(schema._zod.def, {
		get shape() {
			const newShape = { ...schema._zod.def.shape };
			for (const key of Reflect.ownKeys(mask)) {
				if (!Object.prototype.hasOwnProperty.call(currDef.shape, key)) throw new Error(`Unrecognized key: "${String(key)}"`);
				if (!mask[key]) continue;
				delete newShape[key];
			}
			assignProp(this, "shape", newShape);
			return newShape;
		},
		checks: []
	}));
}
function extend(schema, shape) {
	if (!isPlainObject(shape)) throw new Error("Invalid input to extend: expected a plain object");
	const checks = schema._zod.def.checks;
	if (checks && checks.length > 0) {
		const existingShape = schema._zod.def.shape;
		for (const key of Reflect.ownKeys(shape)) if (Object.getOwnPropertyDescriptor(existingShape, key) !== void 0) throw new Error("Cannot overwrite keys on object schemas containing refinements. Use `.safeExtend()` instead.");
	}
	return clone(schema, mergeDefs(schema._zod.def, { get shape() {
		const _shape = {
			...schema._zod.def.shape,
			...shape
		};
		assignProp(this, "shape", _shape);
		return _shape;
	} }));
}
function safeExtend(schema, shape) {
	if (!isPlainObject(shape)) throw new Error("Invalid input to safeExtend: expected a plain object");
	return clone(schema, mergeDefs(schema._zod.def, { get shape() {
		const _shape = {
			...schema._zod.def.shape,
			...shape
		};
		assignProp(this, "shape", _shape);
		return _shape;
	} }));
}
function merge(a, b) {
	if (!b?._zod?.def) throw new Error("Invalid input to merge: expected an object schema. To merge a plain shape, use `.extend()`.");
	if (a._zod.def.checks?.length) throw new Error(".merge() cannot be used on object schemas containing refinements. Use .safeExtend() instead.");
	return clone(a, mergeDefs(a._zod.def, {
		get shape() {
			const _shape = {
				...a._zod.def.shape,
				...b._zod.def.shape
			};
			assignProp(this, "shape", _shape);
			return _shape;
		},
		get catchall() {
			return b._zod.def.catchall;
		},
		checks: b._zod.def.checks ?? []
	}));
}
function partial(Class, schema, mask, name = "partial") {
	const checks = schema._zod.def.checks;
	if (checks && checks.length > 0) throw new Error(`.${name}() cannot be used on object schemas containing refinements`);
	return clone(schema, mergeDefs(schema._zod.def, {
		get shape() {
			const oldShape = schema._zod.def.shape;
			const shape = { ...oldShape };
			if (mask) for (const key of Reflect.ownKeys(mask)) {
				if (!Object.prototype.hasOwnProperty.call(oldShape, key)) throw new Error(`Unrecognized key: "${String(key)}"`);
				if (!mask[key]) continue;
				shape[key] = Class ? new Class({
					type: "optional",
					innerType: oldShape[key]
				}) : oldShape[key];
			}
			else for (const key of Reflect.ownKeys(oldShape)) shape[key] = Class ? new Class({
				type: "optional",
				innerType: oldShape[key]
			}) : oldShape[key];
			assignProp(this, "shape", shape);
			return shape;
		},
		checks: []
	}));
}
function required(Class, schema, mask) {
	return clone(schema, mergeDefs(schema._zod.def, { get shape() {
		const oldShape = schema._zod.def.shape;
		const shape = { ...oldShape };
		if (mask) for (const key of Reflect.ownKeys(mask)) {
			if (!Object.prototype.hasOwnProperty.call(shape, key)) throw new Error(`Unrecognized key: "${String(key)}"`);
			if (!mask[key]) continue;
			shape[key] = new Class({
				type: "nonoptional",
				innerType: oldShape[key]
			});
		}
		else for (const key of Reflect.ownKeys(oldShape)) shape[key] = new Class({
			type: "nonoptional",
			innerType: oldShape[key]
		});
		assignProp(this, "shape", shape);
		return shape;
	} }));
}
function aborted(x, startIndex = 0) {
	if (x.aborted === true) return true;
	for (let i = startIndex; i < x.issues.length; i++) if (x.issues[i]?.continue !== true) return true;
	return false;
}
function explicitlyAborted(x, startIndex = 0) {
	if (x.aborted === true) return true;
	for (let i = startIndex; i < x.issues.length; i++) if (x.issues[i]?.continue === false) return true;
	return false;
}
function prefixIssues(path, issues) {
	return issues.map((iss) => {
		var _a;
		(_a = iss).path ?? (_a.path = []);
		iss.path.unshift(path);
		return iss;
	});
}
function unwrapMessage(message) {
	return typeof message === "string" ? message : message?.message;
}
function attachSchema(issues, start, inst) {
	var _a;
	for (let i = start; i < issues.length; i++) (_a = issues[i]).schema ?? (_a.schema = inst);
}
function finalizeIssue(iss, ctx, config) {
	var _a;
	const traits = iss.inst?._zod?.traits;
	if (traits?.has("$ZodType")) {
		if (traits.has("$ZodCheck")) (_a = iss).schema ?? (_a.schema = iss.inst);
		else iss.schema = iss.inst;
	}
	const schemaError = iss.schema !== iss.inst ? iss.schema?._zod.def?.error : void 0;
	const message = iss.message ? iss.message : unwrapMessage(iss.inst?._zod.def?.error?.(iss)) ?? unwrapMessage(schemaError?.(iss)) ?? unwrapMessage(ctx?.error?.(iss)) ?? unwrapMessage(config.customError?.(iss)) ?? unwrapMessage(config.localeError?.(iss)) ?? "Invalid input";
	const { inst: _inst, schema: _schema, continue: _continue, input: _input, ...rest } = iss;
	rest.path ?? (rest.path = []);
	rest.message = message;
	if (ctx?.reportInput) rest.input = _input;
	return rest;
}
var highSurrogate = /[\uD800-\uDBFF]/;
function codePointLength(str) {
	const units = str.length;
	if (!highSurrogate.test(str)) return units;
	let count = units;
	for (let i = 0; i < units - 1; i++) if ((str.charCodeAt(i) & 64512) === 55296 && (str.charCodeAt(i + 1) & 64512) === 56320) {
		count--;
		i++;
	}
	return count;
}
function getLengthableOrigin(input) {
	if (Array.isArray(input)) return "array";
	if (typeof input === "string") return "string";
	return "unknown";
}
function parsedType(data) {
	const t = typeof data;
	switch (t) {
		case "number": return Number.isNaN(data) ? "nan" : "number";
		case "object": {
			if (data === null) return "null";
			if (Array.isArray(data)) return "array";
			const obj = data;
			if (obj && Object.getPrototypeOf(obj) !== Object.prototype && "constructor" in obj && obj.constructor) return obj.constructor.name;
		}
	}
	return t;
}
function issue(...args) {
	const [iss, input, inst] = args;
	if (typeof iss === "string") return {
		message: iss,
		code: "custom",
		input,
		inst
	};
	return { ...iss };
}
/**
* Installs a trait's members on its prototype. Each value builds that member for the instance on first read; the built value shadows the accessor as an own property, so a detached `const { parse } = schema` keeps working.
*
* Call this from a `proto` initializer, which runs once per prototype — never per instance.
*/
function members(proto, table) {
	for (const key in table) {
		const desc = Object.getOwnPropertyDescriptor(table, key);
		if (desc.get) Object.defineProperty(proto, key, {
			...desc,
			enumerable: false
		});
		else defineBound(proto, key, desc.value);
	}
}
/** Shadows a prototype member with an own value, so a getter that builds from the instance runs once. */
function own(inst, key, value, enumerable = true) {
	Object.defineProperty(inst, key, {
		configurable: true,
		writable: true,
		enumerable,
		value
	});
	return value;
}
/** Like {@link own}, for a member that was never an own data property and has to stay out of `Object.keys`. */
function hide(inst, key, value) {
	return own(inst, key, value, false);
}
function defineBound(proto, key, fn) {
	Object.defineProperty(proto, key, {
		configurable: true,
		get() {
			return this == null ? fn : own(this, key, fn.bind(this));
		},
		set(value) {
			own(this, key, value);
		}
	});
}
/** Returns the prototype to install on, or `undefined` if this group is already installed on it. */
function claim(inst, sentinel) {
	const proto = Object.getPrototypeOf(inst);
	return sentinel in proto ? void 0 : proto;
}
var installing;
var broke = false;
var breaker = {
	configurable: true,
	get() {
		broke = true;
	}
};
/**
* Installs a lazily-derived internal on the `_zod` prototype of `inst`'s
* constructor, computed from the internals object itself and cached there on
* first read. One accessor per constructor rather than one per instance.
*/
function defineLazyInternal(inst, key, compute) {
	const proto = Object.getPrototypeOf(inst._zod);
	if (key in proto && installing !== inst._zod) {
		installing = void 0;
		return;
	}
	installing = inst._zod;
	Object.defineProperty(proto, key, {
		configurable: true,
		get() {
			Object.defineProperty(this, key, breaker);
			const outer = broke;
			broke = false;
			try {
				const value = compute(this);
				if (broke) delete this[key];
				else Object.defineProperty(this, key, {
					configurable: true,
					writable: true,
					value
				});
				broke = broke || outer;
				return value;
			} catch (err) {
				delete this[key];
				broke = broke || outer;
				throw err;
			}
		},
		set(value) {
			Object.defineProperty(this, key, {
				configurable: true,
				writable: true,
				value
			});
		}
	});
}
/**
* Installs `key` on `inst`'s prototype, computed by `make` on first read and cached there as an own
* data property. One accessor per constructor rather than one per instance, because an own accessor
* puts every instance after the first into v8 dictionary mode. The key doubles as the sentinel.
*/
function installLazyProp(inst, key, make, enumerable) {
	const proto = claim(inst, key);
	if (!proto) return;
	Object.defineProperty(proto, key, {
		configurable: true,
		get() {
			const desc = {
				configurable: true,
				writable: true,
				enumerable,
				value: void 0
			};
			Object.defineProperty(this, key, desc);
			desc.value = make(this);
			Object.defineProperty(this, key, desc);
			return desc.value;
		},
		set(value) {
			Object.defineProperty(this, key, {
				configurable: true,
				writable: true,
				enumerable,
				value
			});
		}
	});
}
/** Marks the thunk `_catch` synthesises for a constant catch value. `Function.length` cannot tell that thunk from a user callback — rest and defaulted parameters both report arity 0 — and a user callback reads `ctx.error`, whose issues only finalize correctly against the caller's per-parse error map. Provenance can say what arity cannot. A plain string key rather than `Symbol.for`, whose call at module scope no bundler can prove pure — the same shape that anchored `urlCanParse` into every build. */
var CONSTANT_CATCH = "~constantCatch";
/** Wraps a constant catch value in a thunk tagged with {@link CONSTANT_CATCH}. */
function constantCatch(value) {
	const fn = () => value;
	fn[CONSTANT_CATCH] = true;
	return fn;
}
//#endregion
//#region node_modules/zod/v4/core/core.js
var _a$1;
var _zodDesc$1 = {
	value: void 0,
	enumerable: false
};
var _E = "captureStackTrace" in Error ? Error : null;
function newError(Definition) {
	const E = _E;
	if (E) {
		const saved = E.stackTraceLimit;
		if (typeof saved === "number") {
			try {
				E.stackTraceLimit = 0;
			} catch {
				_E = null;
				return new Definition();
			}
			try {
				return new Definition();
			} finally {
				E.stackTraceLimit = saved;
			}
		}
	}
	return new Definition();
}
function $constructor(name, initializer, proto, params) {
	const zodProto = {};
	function Internals(def) {
		this.def = def;
		this.constr = _;
		this.traits = /* @__PURE__ */ new Set();
	}
	Internals.prototype = zodProto;
	const protoMembers = proto;
	const initialized = protoMembers && /* @__PURE__ */ new WeakSet();
	function init(inst, def) {
		if (!inst._zod) {
			_zodDesc$1.value = new Internals(def);
			try {
				Object.defineProperty(inst, "_zod", _zodDesc$1);
			} finally {
				_zodDesc$1.value = void 0;
			}
		}
		if (inst._zod.traits.has(name)) return;
		inst._zod.traits.add(name);
		initializer(inst, def);
		if (initialized) {
			const own = Object.getPrototypeOf(inst);
			const ctorProto = inst._zod.constr.prototype;
			let up = own;
			while (up && up !== ctorProto) up = Object.getPrototypeOf(up);
			const target = up ?? own;
			if (!initialized.has(target)) {
				initialized.add(target);
				members(target, protoMembers);
			}
		}
		const proto = _.prototype;
		for (const k in proto) {
			if (!Object.prototype.hasOwnProperty.call(proto, k)) continue;
			if (!(k in inst)) inst[k] = proto[k].bind(inst);
		}
	}
	const Parent = params?.Parent ?? Object;
	class Definition extends Parent {}
	Object.defineProperty(Definition, "name", { value: name });
	function _(def) {
		const inst = params?.Parent ? newError(Definition) : this;
		init(inst, def);
		const deferred = inst._zod.deferred;
		if (deferred) {
			for (const fn of deferred) fn();
			inst._zod.deferred = void 0;
		}
		const pp = globalThis.__zod_globalConfig?.postProcessor;
		if (pp) pp(inst);
		return inst;
	}
	Object.defineProperty(_, "init", { value: init });
	Object.defineProperty(_, Symbol.hasInstance, { value: (inst) => {
		if (params?.Parent && inst instanceof params.Parent) return true;
		return inst?._zod?.traits?.has(name);
	} });
	Object.defineProperty(_, "name", { value: name });
	return _;
}
var $ZodAsyncError = class extends Error {
	constructor() {
		super(`Encountered Promise during synchronous parse. Use .parseAsync() instead.`);
	}
};
var $ZodEncodeError = class extends Error {
	constructor(name) {
		super(`Encountered unidirectional transform during encode: ${name}`);
		this.name = "ZodEncodeError";
	}
};
(_a$1 = globalThis).__zod_globalConfig ?? (_a$1.__zod_globalConfig = {});
var globalConfig = globalThis.__zod_globalConfig;
function config(newConfig) {
	if (newConfig) Object.assign(globalConfig, newConfig);
	return globalConfig;
}
//#endregion
//#region node_modules/zod/v4/core/errors.js
function _getMessage() {
	const internals = this._zod;
	internals.message ?? (internals.message = JSON.stringify(internals.def, jsonStringifyReplacer, 2));
	return internals.message;
}
function _setMessage(value) {
	this._zod.message = value;
}
var _messageDesc = {
	get: _getMessage,
	set: _setMessage,
	enumerable: true,
	configurable: true
};
var _zodDesc = {
	value: void 0,
	enumerable: false
};
var _issuesDesc = {
	value: void 0,
	enumerable: false
};
var _installedToString = /* @__PURE__ */ new WeakSet([Object.prototype, Error.prototype]);
var initializer$1 = (inst, def) => {
	inst.name = "$ZodError";
	_zodDesc.value = inst._zod;
	Object.defineProperty(inst, "_zod", _zodDesc);
	_issuesDesc.value = def;
	Object.defineProperty(inst, "issues", _issuesDesc);
	_zodDesc.value = void 0;
	_issuesDesc.value = void 0;
	Object.defineProperty(inst, "message", _messageDesc);
	const proto = Object.getPrototypeOf(inst);
	if (!_installedToString.has(proto)) {
		_installedToString.add(proto);
		Object.defineProperty(proto, "toString", {
			configurable: true,
			enumerable: false,
			get() {
				const value = () => this.message;
				Object.defineProperty(this, "toString", {
					value,
					configurable: true,
					writable: true
				});
				return value;
			},
			set(value) {
				Object.defineProperty(this, "toString", {
					value,
					configurable: true,
					writable: true
				});
			}
		});
	}
};
var $ZodError = $constructor("$ZodError", initializer$1);
var $ZodRealError = $constructor("$ZodError", initializer$1, void 0, { Parent: Error });
/** Get-or-create `obj[key]` as an own data property. A path segment naming an inherited member
* ("toString", "constructor") would otherwise read through to the prototype, and assigning
* "__proto__" would hit the setter instead of creating a key. */
function node(obj, key, make) {
	if (!Object.prototype.hasOwnProperty.call(obj, key)) {
		if (key === "__proto__") Object.defineProperty(obj, key, {
			value: make(),
			writable: true,
			enumerable: true,
			configurable: true
		});
		else obj[key] = make();
	}
	return obj[key];
}
function flattenError(error, mapper = (issue) => issue.message) {
	const fieldErrors = {};
	const formErrors = [];
	for (const sub of error.issues) if (sub.path.length > 0) node(fieldErrors, sub.path[0], () => []).push(mapper(sub));
	else formErrors.push(mapper(sub));
	return {
		formErrors,
		fieldErrors
	};
}
function formatError(error, mapper = (issue) => issue.message) {
	const fieldErrors = { _errors: [] };
	const processError = (error, path = []) => {
		for (const issue of error.issues) if (issue.code === "invalid_union" && issue.errors.length) issue.errors.map((issues) => processError({ issues }, [...path, ...issue.path]));
		else if (issue.code === "invalid_key") processError({ issues: issue.issues }, [...path, ...issue.path]);
		else if (issue.code === "invalid_element") processError({ issues: issue.issues }, [...path, ...issue.path]);
		else {
			const fullpath = [...path, ...issue.path];
			if (fullpath.length === 0) fieldErrors._errors.push(mapper(issue));
			else {
				let curr = fieldErrors;
				let i = 0;
				while (i < fullpath.length) {
					const el = fullpath[i];
					const terminal = i === fullpath.length - 1;
					if (el === "_errors") {
						if (terminal) curr._errors.push(mapper(issue));
						i++;
						continue;
					}
					if (!Object.prototype.hasOwnProperty.call(curr, el)) Object.defineProperty(curr, el, {
						value: { _errors: [] },
						enumerable: true,
						writable: true,
						configurable: true
					});
					const node = curr[el];
					if (terminal) node._errors.push(mapper(issue));
					curr = node;
					i++;
				}
			}
		}
	};
	processError(error);
	return fieldErrors;
}
//#endregion
//#region node_modules/zod/v4/core/parse.js
function finalizeParams(callee, params) {
	return {
		callee: params?.callee ?? callee,
		Err: params?.Err
	};
}
var _parse = (_Err) => {
	const fn = (schema, value, _ctx, _params) => {
		const ctx = _ctx ? {
			..._ctx,
			async: false
		} : { async: false };
		const result = schema._zod.run({
			value,
			issues: []
		}, ctx);
		if (result instanceof Promise) throw new $ZodAsyncError();
		if (result.issues.length) {
			const e = new ((_params?.Err) ?? _Err)(result.issues.map((iss) => finalizeIssue(iss, ctx, config())));
			captureStackTrace(e, _params?.callee ?? fn);
			throw e;
		}
		return result.value;
	};
	return fn;
};
var _parseAsync = (_Err) => {
	const fn = async (schema, value, _ctx, params) => {
		const ctx = _ctx ? {
			..._ctx,
			async: true
		} : { async: true };
		let result = schema._zod.run({
			value,
			issues: []
		}, ctx);
		if (result instanceof Promise) result = await result;
		if (result.issues.length) {
			const e = new ((params?.Err) ?? _Err)(result.issues.map((iss) => finalizeIssue(iss, ctx, config())));
			captureStackTrace(e, params?.callee ?? fn);
			throw e;
		}
		return result.value;
	};
	return fn;
};
var _safeParse = (_Err) => (schema, value, _ctx) => {
	const ctx = _ctx ? {
		..._ctx,
		async: false
	} : { async: false };
	const result = schema._zod.run({
		value,
		issues: []
	}, ctx);
	if (result instanceof Promise) throw new $ZodAsyncError();
	return result.issues.length ? {
		success: false,
		error: new (_Err ?? $ZodError)(result.issues.map((iss) => finalizeIssue(iss, ctx, config())))
	} : {
		success: true,
		data: result.value
	};
};
var safeParse$1 = /* @__PURE__*/ _safeParse($ZodRealError);
var _safeParseAsync = (_Err) => async (schema, value, _ctx) => {
	const ctx = _ctx ? {
		..._ctx,
		async: true
	} : { async: true };
	let result = schema._zod.run({
		value,
		issues: []
	}, ctx);
	if (result instanceof Promise) result = await result;
	return result.issues.length ? {
		success: false,
		error: new _Err(result.issues.map((iss) => finalizeIssue(iss, ctx, config())))
	} : {
		success: true,
		data: result.value
	};
};
var safeParseAsync$1 = /* @__PURE__*/ _safeParseAsync($ZodRealError);
var _encode = (_Err) => {
	const parse = _parse(_Err);
	const fn = (schema, value, _ctx, _params) => {
		const ctx = _ctx ? {
			..._ctx,
			direction: "backward"
		} : { direction: "backward" };
		return parse(schema, value, ctx, finalizeParams(fn, _params));
	};
	return fn;
};
var _decode = (_Err) => {
	const parse = _parse(_Err);
	const fn = (schema, value, _ctx, _params) => {
		return parse(schema, value, _ctx, finalizeParams(fn, _params));
	};
	return fn;
};
var _encodeAsync = (_Err) => {
	const parseAsync = _parseAsync(_Err);
	const fn = async (schema, value, _ctx, _params) => {
		const ctx = _ctx ? {
			..._ctx,
			direction: "backward"
		} : { direction: "backward" };
		return await parseAsync(schema, value, ctx, finalizeParams(fn, _params));
	};
	return fn;
};
var _decodeAsync = (_Err) => {
	const parseAsync = _parseAsync(_Err);
	const fn = async (schema, value, _ctx, _params) => {
		return await parseAsync(schema, value, _ctx, finalizeParams(fn, _params));
	};
	return fn;
};
var _safeEncode = (_Err) => (schema, value, _ctx) => {
	const ctx = _ctx ? {
		..._ctx,
		direction: "backward"
	} : { direction: "backward" };
	return _safeParse(_Err)(schema, value, ctx);
};
var _safeDecode = (_Err) => (schema, value, _ctx) => {
	return _safeParse(_Err)(schema, value, _ctx);
};
var _safeEncodeAsync = (_Err) => async (schema, value, _ctx) => {
	const ctx = _ctx ? {
		..._ctx,
		direction: "backward"
	} : { direction: "backward" };
	return _safeParseAsync(_Err)(schema, value, ctx);
};
var _safeDecodeAsync = (_Err) => async (schema, value, _ctx) => {
	return _safeParseAsync(_Err)(schema, value, _ctx);
};
//#endregion
//#region node_modules/zod/v4/core/regexes.js
/**
* @deprecated CUID v1 is deprecated by its authors due to information leakage
* (timestamps embedded in the id). Use {@link cuid2} instead.
* See https://github.com/paralleldrive/cuid.
*/
var cuid = /^[cC][0-9a-z]{6,}$/;
var cuid2 = /^[0-9a-z]+$/;
var ulid = /^[0-7][0-9A-HJKMNP-TV-Za-hjkmnp-tv-z]{25}$/;
var xid = /^[0-9a-vA-V]{20}$/;
var ksuid = /^[A-Za-z0-9]{27}$/;
var nanoid = /^[a-zA-Z0-9_-]{21}$/;
function nanoidOfLength(length) {
	return new RegExp(`^[a-zA-Z0-9_-]{${length}}$`);
}
/** ISO 8601-1 duration regex. Does not support the 8601-2 extensions like negative durations or fractional/negative components. */
var duration = /^P(?:(\d+W)|(?!.*W)(?=\d|T\d)(\d+Y)?(\d+M)?(\d+D)?(T(?=\d)(\d+H)?(\d+M)?(\d+([.,]\d+)?S)?)?)$/;
/** A regex for any UUID-like identifier: 8-4-4-4-12 hex pattern */
var guid = /^([0-9a-fA-F]{8}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{12})$/;
/** Returns a regex for validating an RFC 9562/4122 UUID.
*
* @param version Optionally specify a version 1-8. If no version is specified, all versions are supported. */
var uuid = (version) => {
	if (!version) return /^([0-9a-fA-F]{8}-[0-9a-fA-F]{4}-[1-8][0-9a-fA-F]{3}-[89abAB][0-9a-fA-F]{3}-[0-9a-fA-F]{12}|00000000-0000-0000-0000-000000000000|ffffffff-ffff-ffff-ffff-ffffffffffff)$/;
	return new RegExp(`^([0-9a-fA-F]{8}-[0-9a-fA-F]{4}-${version}[0-9a-fA-F]{3}-[89abAB][0-9a-fA-F]{3}-[0-9a-fA-F]{12})$`);
};
/** Practical email validation */
var email = /^(?!\.)(?!.*\.\.)([A-Za-z0-9_'+\-\.]*)[A-Za-z0-9_+-]@([A-Za-z0-9][A-Za-z0-9\-]*\.)+[A-Za-z]{2,}$/;
var _emoji$1 = `^[\\p{Extended_Pictographic}\\p{Emoji_Component}]+$`;
function emoji() {
	return new RegExp(_emoji$1, "u");
}
var ipv4 = /^(?:(?:25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])\.){3}(?:25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])$/;
var ipv6 = /^(([0-9a-fA-F]{1,4}:){7}[0-9a-fA-F]{1,4}|([0-9a-fA-F]{1,4}:){1,7}:|([0-9a-fA-F]{1,4}:){1,6}:[0-9a-fA-F]{1,4}|([0-9a-fA-F]{1,4}:){1,5}(:[0-9a-fA-F]{1,4}){1,2}|([0-9a-fA-F]{1,4}:){1,4}(:[0-9a-fA-F]{1,4}){1,3}|([0-9a-fA-F]{1,4}:){1,3}(:[0-9a-fA-F]{1,4}){1,4}|([0-9a-fA-F]{1,4}:){1,2}(:[0-9a-fA-F]{1,4}){1,5}|[0-9a-fA-F]{1,4}:((:[0-9a-fA-F]{1,4}){1,6})|:((:[0-9a-fA-F]{1,4}){1,7}|:))$/;
var cidrv4 = /^((25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])\.){3}(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])\/([0-9]|[1-2][0-9]|3[0-2])$/;
var cidrv6 = /^(([0-9a-fA-F]{1,4}:){7}[0-9a-fA-F]{1,4}|([0-9a-fA-F]{1,4}:){1,7}:|([0-9a-fA-F]{1,4}:){1,6}:[0-9a-fA-F]{1,4}|([0-9a-fA-F]{1,4}:){1,5}(:[0-9a-fA-F]{1,4}){1,2}|([0-9a-fA-F]{1,4}:){1,4}(:[0-9a-fA-F]{1,4}){1,3}|([0-9a-fA-F]{1,4}:){1,3}(:[0-9a-fA-F]{1,4}){1,4}|([0-9a-fA-F]{1,4}:){1,2}(:[0-9a-fA-F]{1,4}){1,5}|[0-9a-fA-F]{1,4}:((:[0-9a-fA-F]{1,4}){1,6})|:((:[0-9a-fA-F]{1,4}){1,7}|:))\/(12[0-8]|1[01][0-9]|[1-9]?[0-9])$/;
var base64 = /^$|^(?:[0-9a-zA-Z+/]{4})*(?:(?:[0-9a-zA-Z+/]{2}==)|(?:[0-9a-zA-Z+/]{3}=))?$/;
var base64url = /^[A-Za-z0-9_-]*$/;
var httpProtocol = /^https?$/;
var e164 = /^\+[1-9]\d{6,14}$/;
var dateSource = `(?:(?:\\d\\d[2468][048]|\\d\\d[13579][26]|\\d\\d0[48]|[02468][048]00|[13579][26]00)-02-29|\\d{4}-(?:(?:0[13578]|1[02])-(?:0[1-9]|[12]\\d|3[01])|(?:0[469]|11)-(?:0[1-9]|[12]\\d|30)|(?:02)-(?:0[1-9]|1\\d|2[0-8])))`;
/** Anchors a pattern source. The interpolation lives here rather than at the call site because
* esbuild will not drop a `@__PURE__` call whose own argument interpolates a variable, but it
* will drop `anchor(dateSource)`. Keeping it inline pinned `date` into every bundle. */
function anchor(source) {
	return new RegExp(`^${source}$`);
}
var date = /*@__PURE__*/ anchor(dateSource);
function timeSource(args) {
	const hhmm = `(?:[01]\\d|2[0-3]):[0-5]\\d`;
	return typeof args.precision === "number" ? args.precision === -1 ? `${hhmm}` : args.precision === 0 ? `${hhmm}:[0-5]\\d` : `${hhmm}:[0-5]\\d\\.\\d{${args.precision}}` : args.seconds ? `${hhmm}:[0-5]\\d(?:\\.\\d+)?` : `${hhmm}(?::[0-5]\\d(?:\\.\\d+)?)?`;
}
function time(args) {
	return new RegExp(`^${timeSource(args)}$`);
}
function datetime(args) {
	const opts = ["Z"];
	if (args.offset) opts.push(`([+-](?:[01]\\d|2[0-3]):[0-5]\\d)`);
	const qualified = `${timeSource({
		precision: args.precision,
		seconds: true
	})}(?:${opts.join("|")})`;
	const timeRegex = args.local ? `${qualified}|${timeSource({ precision: args.precision })}` : qualified;
	return new RegExp(`^${dateSource}T(?:${timeRegex})$`);
}
var string$1 = (params) => {
	const regex = params ? `[\\s\\S]{${params?.minimum ?? 0},${params?.maximum ?? ""}}` : `[\\s\\S]*`;
	return new RegExp(`^${regex}$`);
};
var integer = /^-?\d+$/;
var number$1 = /^-?\d+(?:\.\d+)?$/;
var boolean$1 = /^(?:true|false)$/i;
var lowercase = /^[^A-Z]*$/;
var uppercase = /^[^a-z]*$/;
//#endregion
//#region node_modules/zod/v4/core/checks.js
var $ZodCheck = /*@__PURE__*/ $constructor("$ZodCheck", (inst, def) => {
	var _a;
	inst._zod ?? (inst._zod = {});
	inst._zod.def = def;
	(_a = inst._zod).onattach ?? (_a.onattach = []);
});
/** Default `when` for length-based checks: run only on non-nullish values with a `length`. */
var _whenHasLength = (payload) => {
	const val = payload.value;
	return !nullish(val) && val.length !== void 0;
};
var numericOriginMap = {
	number: "number",
	bigint: "bigint",
	object: "date"
};
var $ZodCheckLessThan = /*@__PURE__*/ $constructor("$ZodCheckLessThan", (inst, def) => {
	$ZodCheck.init(inst, def);
	const origin = numericOriginMap[typeof def.value];
	inst._zod.onattach.push((inst) => {
		const bag = inst._zod.bag;
		const curr = (def.inclusive ? bag.maximum : bag.exclusiveMaximum) ?? Number.POSITIVE_INFINITY;
		if (def.value < curr) {
			if (def.inclusive) bag.maximum = def.value;
			else bag.exclusiveMaximum = def.value;
		}
	});
	inst._zod.check = (payload) => {
		if (def.inclusive ? payload.value <= def.value : payload.value < def.value) return;
		payload.issues.push({
			origin: numericOriginMap[typeof payload.value] ?? origin,
			code: "too_big",
			maximum: typeof def.value === "object" ? def.value.getTime() : def.value,
			input: payload.value,
			inclusive: def.inclusive,
			inst,
			continue: !def.abort
		});
	};
});
var $ZodCheckGreaterThan = /*@__PURE__*/ $constructor("$ZodCheckGreaterThan", (inst, def) => {
	$ZodCheck.init(inst, def);
	const origin = numericOriginMap[typeof def.value];
	inst._zod.onattach.push((inst) => {
		const bag = inst._zod.bag;
		const curr = (def.inclusive ? bag.minimum : bag.exclusiveMinimum) ?? Number.NEGATIVE_INFINITY;
		if (def.value > curr) {
			if (def.inclusive) bag.minimum = def.value;
			else bag.exclusiveMinimum = def.value;
		}
	});
	inst._zod.check = (payload) => {
		if (def.inclusive ? payload.value >= def.value : payload.value > def.value) return;
		payload.issues.push({
			origin: numericOriginMap[typeof payload.value] ?? origin,
			code: "too_small",
			minimum: typeof def.value === "object" ? def.value.getTime() : def.value,
			input: payload.value,
			inclusive: def.inclusive,
			inst,
			continue: !def.abort
		});
	};
});
var $ZodCheckMultipleOf = /*@__PURE__*/ $constructor("$ZodCheckMultipleOf", (inst, def) => {
	$ZodCheck.init(inst, def);
	inst._zod.onattach.push((inst) => {
		var _a;
		(_a = inst._zod.bag).multipleOf ?? (_a.multipleOf = def.value);
	});
	inst._zod.check = (payload) => {
		if (typeof payload.value !== typeof def.value) throw new Error("Cannot mix number and bigint in multiple_of check.");
		if (typeof payload.value === "bigint" ? def.value !== BigInt(0) && payload.value % def.value === BigInt(0) : floatSafeRemainder(payload.value, def.value) === 0) return;
		payload.issues.push({
			origin: typeof payload.value,
			code: "not_multiple_of",
			divisor: def.value,
			input: payload.value,
			inst,
			continue: !def.abort
		});
	};
});
var $ZodCheckNumberFormat = /*@__PURE__*/ $constructor("$ZodCheckNumberFormat", (inst, def) => {
	$ZodCheck.init(inst, def);
	def.format = def.format || "float64";
	const isInt = def.format?.includes("int");
	const origin = isInt ? "int" : "number";
	const [minimum, maximum] = NUMBER_FORMAT_RANGES[def.format];
	inst._zod.onattach.push((inst) => {
		const bag = inst._zod.bag;
		bag.format = def.format;
		bag.minimum = minimum;
		bag.maximum = maximum;
		if (isInt) bag.pattern = integer;
	});
	inst._zod.check = (payload) => {
		const input = payload.value;
		if (isInt) {
			if (!Number.isInteger(input)) {
				payload.issues.push({
					expected: origin,
					format: def.format,
					code: "invalid_type",
					continue: false,
					input,
					inst
				});
				return;
			}
			if (!Number.isSafeInteger(input)) {
				if (input > 0) payload.issues.push({
					input,
					code: "too_big",
					maximum: Number.MAX_SAFE_INTEGER,
					note: "Integers must be within the safe integer range.",
					inst,
					origin,
					inclusive: true,
					continue: !def.abort
				});
				else payload.issues.push({
					input,
					code: "too_small",
					minimum: Number.MIN_SAFE_INTEGER,
					note: "Integers must be within the safe integer range.",
					inst,
					origin,
					inclusive: true,
					continue: !def.abort
				});
				return;
			}
		}
		if (input < minimum) payload.issues.push({
			origin: "number",
			input,
			code: "too_small",
			minimum,
			inclusive: true,
			inst,
			continue: !def.abort
		});
		if (input > maximum) payload.issues.push({
			origin: "number",
			input,
			code: "too_big",
			maximum,
			inclusive: true,
			inst,
			continue: !def.abort
		});
	};
});
var $ZodCheckMaxLength = /*@__PURE__*/ $constructor("$ZodCheckMaxLength", (inst, def) => {
	var _a;
	$ZodCheck.init(inst, def);
	(_a = inst._zod.def).when ?? (_a.when = _whenHasLength);
	inst._zod.onattach.push((inst) => {
		const curr = inst._zod.bag.maximum ?? Number.POSITIVE_INFINITY;
		if (def.maximum < curr) inst._zod.bag.maximum = def.maximum;
	});
	inst._zod.check = (payload) => {
		const input = payload.value;
		const units = input.length;
		if ((typeof input === "string" && units > def.maximum ? codePointLength(input) : units) <= def.maximum) return;
		const origin = getLengthableOrigin(input);
		payload.issues.push({
			origin,
			code: "too_big",
			maximum: def.maximum,
			inclusive: true,
			input,
			inst,
			continue: !def.abort
		});
	};
});
var $ZodCheckMinLength = /*@__PURE__*/ $constructor("$ZodCheckMinLength", (inst, def) => {
	var _a;
	$ZodCheck.init(inst, def);
	(_a = inst._zod.def).when ?? (_a.when = _whenHasLength);
	inst._zod.onattach.push((inst) => {
		const curr = inst._zod.bag.minimum ?? Number.NEGATIVE_INFINITY;
		if (def.minimum > curr) inst._zod.bag.minimum = def.minimum;
	});
	inst._zod.check = (payload) => {
		const input = payload.value;
		const units = input.length;
		if ((typeof input === "string" && units >= def.minimum && units < def.minimum * 2 ? codePointLength(input) : units) >= def.minimum) return;
		const origin = getLengthableOrigin(input);
		payload.issues.push({
			origin,
			code: "too_small",
			minimum: def.minimum,
			inclusive: true,
			input,
			inst,
			continue: !def.abort
		});
	};
});
var $ZodCheckLengthEquals = /*@__PURE__*/ $constructor("$ZodCheckLengthEquals", (inst, def) => {
	var _a;
	$ZodCheck.init(inst, def);
	(_a = inst._zod.def).when ?? (_a.when = _whenHasLength);
	inst._zod.onattach.push((inst) => {
		const bag = inst._zod.bag;
		bag.minimum = def.length;
		bag.maximum = def.length;
		bag.length = def.length;
	});
	inst._zod.check = (payload) => {
		const input = payload.value;
		const units = input.length;
		const length = typeof input === "string" && units >= def.length && units <= def.length * 2 ? codePointLength(input) : units;
		if (length === def.length) return;
		const origin = getLengthableOrigin(input);
		const tooBig = length > def.length;
		payload.issues.push({
			origin,
			...tooBig ? {
				code: "too_big",
				maximum: def.length
			} : {
				code: "too_small",
				minimum: def.length
			},
			inclusive: true,
			exact: true,
			input: payload.value,
			inst,
			continue: !def.abort
		});
	};
});
var $ZodCheckStringFormat = /*@__PURE__*/ $constructor("$ZodCheckStringFormat", (inst, def) => {
	var _a, _b;
	$ZodCheck.init(inst, def);
	inst._zod.onattach.push((inst) => {
		const bag = inst._zod.bag;
		bag.format = def.format;
		if (def.pattern) {
			bag.patterns ?? (bag.patterns = /* @__PURE__ */ new Set());
			bag.patterns.add(def.pattern);
		}
	});
	if (def.pattern) (_a = inst._zod).check ?? (_a.check = (payload) => {
		def.pattern.lastIndex = 0;
		if (def.pattern.test(payload.value)) return;
		payload.issues.push({
			origin: "string",
			code: "invalid_format",
			format: def.format,
			input: payload.value,
			...def.pattern ? { pattern: def.pattern.toString() } : {},
			inst,
			continue: !def.abort
		});
	});
	else (_b = inst._zod).check ?? (_b.check = () => {});
});
var $ZodCheckRegex = /*@__PURE__*/ $constructor("$ZodCheckRegex", (inst, def) => {
	$ZodCheckStringFormat.init(inst, def);
	inst._zod.check = (payload) => {
		def.pattern.lastIndex = 0;
		if (def.pattern.test(payload.value)) return;
		payload.issues.push({
			origin: "string",
			code: "invalid_format",
			format: "regex",
			input: payload.value,
			pattern: def.pattern.toString(),
			inst,
			continue: !def.abort
		});
	};
});
var $ZodCheckLowerCase = /*@__PURE__*/ $constructor("$ZodCheckLowerCase", (inst, def) => {
	def.pattern ?? (def.pattern = lowercase);
	$ZodCheckStringFormat.init(inst, def);
});
var $ZodCheckUpperCase = /*@__PURE__*/ $constructor("$ZodCheckUpperCase", (inst, def) => {
	def.pattern ?? (def.pattern = uppercase);
	$ZodCheckStringFormat.init(inst, def);
});
var $ZodCheckIncludes = /*@__PURE__*/ $constructor("$ZodCheckIncludes", (inst, def) => {
	$ZodCheck.init(inst, def);
	const escapedRegex = escapeRegex(def.includes);
	const pattern = new RegExp(typeof def.position === "number" ? `^.{${def.position},}${escapedRegex}` : escapedRegex);
	def.pattern = pattern;
	inst._zod.onattach.push((inst) => {
		const bag = inst._zod.bag;
		bag.patterns ?? (bag.patterns = /* @__PURE__ */ new Set());
		bag.patterns.add(pattern);
	});
	inst._zod.check = (payload) => {
		if (payload.value.includes(def.includes, def.position)) return;
		payload.issues.push({
			origin: "string",
			code: "invalid_format",
			format: "includes",
			includes: def.includes,
			input: payload.value,
			inst,
			continue: !def.abort
		});
	};
});
var $ZodCheckStartsWith = /*@__PURE__*/ $constructor("$ZodCheckStartsWith", (inst, def) => {
	$ZodCheck.init(inst, def);
	const pattern = new RegExp(`^${escapeRegex(def.prefix)}.*`);
	def.pattern ?? (def.pattern = pattern);
	inst._zod.onattach.push((inst) => {
		const bag = inst._zod.bag;
		bag.patterns ?? (bag.patterns = /* @__PURE__ */ new Set());
		bag.patterns.add(pattern);
	});
	inst._zod.check = (payload) => {
		if (payload.value.startsWith(def.prefix)) return;
		payload.issues.push({
			origin: "string",
			code: "invalid_format",
			format: "starts_with",
			prefix: def.prefix,
			input: payload.value,
			inst,
			continue: !def.abort
		});
	};
});
var $ZodCheckEndsWith = /*@__PURE__*/ $constructor("$ZodCheckEndsWith", (inst, def) => {
	$ZodCheck.init(inst, def);
	const pattern = new RegExp(`.*${escapeRegex(def.suffix)}$`);
	def.pattern ?? (def.pattern = pattern);
	inst._zod.onattach.push((inst) => {
		const bag = inst._zod.bag;
		bag.patterns ?? (bag.patterns = /* @__PURE__ */ new Set());
		bag.patterns.add(pattern);
	});
	inst._zod.check = (payload) => {
		if (payload.value.endsWith(def.suffix)) return;
		payload.issues.push({
			origin: "string",
			code: "invalid_format",
			format: "ends_with",
			suffix: def.suffix,
			input: payload.value,
			inst,
			continue: !def.abort
		});
	};
});
var $ZodCheckOverwrite = /*@__PURE__*/ $constructor("$ZodCheckOverwrite", (inst, def) => {
	$ZodCheck.init(inst, def);
	inst._zod.check = (payload) => {
		payload.value = def.tx(payload.value);
	};
});
//#endregion
//#region node_modules/zod/v4/core/doc.js
var Doc = class {
	constructor(args = [], closed = {}) {
		this.content = [];
		this.indent = 0;
		this.args = args;
		this.closed = closed;
	}
	indented(fn) {
		this.indent += 1;
		fn(this);
		this.indent -= 1;
	}
	write(arg) {
		if (typeof arg === "function") {
			arg(this, { execution: "sync" });
			arg(this, { execution: "async" });
			return;
		}
		const lines = arg.split("\n").filter((x) => x);
		const minIndent = Math.min(...lines.map((x) => x.length - x.trimStart().length));
		const dedented = lines.map((x) => x.slice(minIndent)).map((x) => " ".repeat(this.indent * 2) + x);
		for (const line of dedented) this.content.push(line);
	}
	compile() {
		const F = Function;
		const content = this?.content ?? [``];
		return new F(...Object.keys(this.closed), `return function (${this.args.join(", ")}) {\n${content.join("\n")}\n};`)(...Object.values(this.closed));
	}
};
//#endregion
//#region node_modules/zod/v4/core/versions.js
var version = {
	major: 4,
	minor: 5,
	patch: 4
};
//#endregion
//#region node_modules/zod/v4/core/schemas.js
var $ZodType = /*@__PURE__*/ $constructor("$ZodType", (inst, def) => {
	var _a;
	inst ?? (inst = {});
	inst._zod.def = def;
	inst._zod.bag = inst._zod.bag || {};
	inst._zod.version = version;
	const defChecks = inst._zod.def.checks;
	const checks = inst._zod.traits.has("$ZodCheck") ? [inst, ...defChecks ?? []] : defChecks?.length ? [...defChecks] : [];
	for (const ch of checks) for (const fn of ch._zod.onattach) fn(inst);
	if (checks.length === 0) {
		(_a = inst._zod).deferred ?? (_a.deferred = []);
		inst._zod.deferred?.push(() => {
			inst._zod.run = inst._zod.parse;
		});
	} else {
		const runChecks = (payload, checks, ctx) => {
			if (payload.memo) return payload;
			let isAborted = aborted(payload);
			let asyncResult;
			for (const ch of checks) {
				if (ch._zod.def.when) {
					if (explicitlyAborted(payload)) continue;
					if (!ch._zod.def.when(payload)) continue;
				} else if (isAborted) continue;
				const currLen = payload.issues.length;
				const _ = ch._zod.check(payload);
				if (_ instanceof Promise && ctx?.async === false) throw new $ZodAsyncError();
				if (asyncResult || _ instanceof Promise) asyncResult = (asyncResult ?? Promise.resolve()).then(async () => {
					await _;
					if (payload.issues.length === currLen) return;
					attachSchema(payload.issues, currLen, inst);
					if (!isAborted) isAborted = aborted(payload, currLen);
				});
				else {
					if (payload.issues.length === currLen) continue;
					attachSchema(payload.issues, currLen, inst);
					if (!isAborted) isAborted = aborted(payload, currLen);
				}
			}
			if (asyncResult) return asyncResult.then(() => {
				return payload;
			});
			return payload;
		};
		const handleCanaryResult = (canary, payload, ctx) => {
			if (aborted(canary)) {
				canary.aborted = true;
				return canary;
			}
			const checkResult = runChecks(payload, checks, ctx);
			if (checkResult instanceof Promise) {
				if (ctx.async === false) throw new $ZodAsyncError();
				return checkResult.then((checkResult) => inst._zod.parse(checkResult, ctx));
			}
			return inst._zod.parse(checkResult, ctx);
		};
		inst._zod.run = (payload, ctx) => {
			if (ctx.skipChecks) return inst._zod.parse(payload, ctx);
			if (ctx.direction === "backward") {
				const canary = inst._zod.parse({
					value: payload.value,
					issues: []
				}, {
					...ctx,
					skipChecks: true
				});
				if (canary instanceof Promise) return canary.then((canary) => {
					return handleCanaryResult(canary, payload, ctx);
				});
				return handleCanaryResult(canary, payload, ctx);
			}
			const result = inst._zod.parse(payload, ctx);
			if (result instanceof Promise) {
				if (ctx.async === false) throw new $ZodAsyncError();
				return result.then((result) => runChecks(result, checks, ctx));
			}
			return runChecks(result, checks, ctx);
		};
	}
}, {
	get "~standard"() {
		return hide(this, "~standard", standardProps(this));
	},
	set "~standard"(value) {
		own(this, "~standard", value);
	}
});
/** The Standard Schema surface for `inst`. Shared so wrappers can extend it without forcing it. */
var toStandardResult = (r) => r.success ? { value: r.data } : { issues: r.error?.issues };
function standardProps(inst) {
	return {
		validate: (value) => {
			try {
				return toStandardResult(safeParse$1(inst, value));
			} catch (_) {
				return safeParseAsync$1(inst, value).then(toStandardResult);
			}
		},
		vendor: "zod",
		version: 1
	};
}
var $ZodString = /*@__PURE__*/ $constructor("$ZodString", (inst, def) => {
	$ZodType.init(inst, def);
	inst._zod.pattern = [...inst?._zod.bag?.patterns ?? []].pop() ?? string$1(inst._zod.bag);
	inst._zod.parse = (payload, _) => {
		if (def.coerce) try {
			payload.value = String(payload.value);
		} catch (_) {}
		if (typeof payload.value === "string") return payload;
		payload.issues.push({
			expected: "string",
			code: "invalid_type",
			input: payload.value,
			inst
		});
		return payload;
	};
});
var $ZodStringFormat = /*@__PURE__*/ $constructor("$ZodStringFormat", (inst, def) => {
	$ZodCheckStringFormat.init(inst, def);
	$ZodString.init(inst, def);
});
var $ZodGUID = /*@__PURE__*/ $constructor("$ZodGUID", (inst, def) => {
	def.pattern ?? (def.pattern = guid);
	$ZodStringFormat.init(inst, def);
});
var $ZodUUID = /*@__PURE__*/ $constructor("$ZodUUID", (inst, def) => {
	if (def.version) {
		const v = {
			v1: 1,
			v2: 2,
			v3: 3,
			v4: 4,
			v5: 5,
			v6: 6,
			v7: 7,
			v8: 8
		}[def.version];
		if (v === void 0) throw new Error(`Invalid UUID version: "${def.version}"`);
		def.pattern ?? (def.pattern = uuid(v));
	} else def.pattern ?? (def.pattern = uuid());
	$ZodStringFormat.init(inst, def);
});
var $ZodEmail = /*@__PURE__*/ $constructor("$ZodEmail", (inst, def) => {
	def.pattern ?? (def.pattern = email);
	$ZodStringFormat.init(inst, def);
});
/** Parses a URL for `$ZodURL`, applying the one guard the URL constructor cannot express. Returns the parsed URL, or a code naming the stage that rejected it — the runtime needs that distinction to pick an issue note, and compiled code only needs to know it is not a URL. */
function parseURLObject(trimmed, def) {
	if (!def.normalize && def.protocol?.source === httpProtocol.source && !/^https?:\/\//i.test(trimmed)) return 1;
	try {
		return new URL(trimmed);
	} catch {
		return 2;
	}
}
var asciiTabOrNewline = /[\t\n\r]/g;
/** The URL parser deletes every ASCII tab, LF and CR from its input before it parses, so `new URL("https://exa\nmple.com")` reports on `example.com`. Applying the same deletion to the returned value closes the half of that divergence which can move the host; the parser's other rewrite, stripping C0 controls at the edges, cannot. */
function stripTabAndNewline(value) {
	return value.replace(asciiTabOrNewline, "");
}
function urlHostnameOk(url, hostname) {
	hostname.lastIndex = 0;
	return hostname.test(url.hostname);
}
function urlProtocolOk(url, protocol) {
	protocol.lastIndex = 0;
	return protocol.test(url.protocol.endsWith(":") ? url.protocol.slice(0, -1) : url.protocol);
}
var $ZodURL = /*@__PURE__*/ $constructor("$ZodURL", (inst, def) => {
	$ZodStringFormat.init(inst, def);
	inst._zod.check = (payload) => {
		try {
			const trimmed = payload.value.trim();
			const url = parseURLObject(trimmed, def);
			if (url === 1) {
				payload.issues.push({
					code: "invalid_format",
					format: "url",
					note: "Invalid URL format",
					input: payload.value,
					inst,
					continue: !def.abort
				});
				return;
			}
			if (url === 2) {
				payload.issues.push({
					code: "invalid_format",
					format: "url",
					input: payload.value,
					inst,
					continue: !def.abort
				});
				return;
			}
			if (def.hostname && !urlHostnameOk(url, def.hostname)) payload.issues.push({
				code: "invalid_format",
				format: "url",
				note: "Invalid hostname",
				pattern: def.hostname.source,
				input: payload.value,
				inst,
				continue: !def.abort
			});
			if (def.protocol && !urlProtocolOk(url, def.protocol)) payload.issues.push({
				code: "invalid_format",
				format: "url",
				note: "Invalid protocol",
				pattern: def.protocol.source,
				input: payload.value,
				inst,
				continue: !def.abort
			});
			payload.value = def.normalize ? url.href : stripTabAndNewline(trimmed);
			return;
		} catch (_) {
			payload.issues.push({
				code: "invalid_format",
				format: "url",
				input: payload.value,
				inst,
				continue: !def.abort
			});
		}
	};
});
var $ZodEmoji = /*@__PURE__*/ $constructor("$ZodEmoji", (inst, def) => {
	def.pattern ?? (def.pattern = emoji());
	$ZodStringFormat.init(inst, def);
});
var $ZodNanoID = /*@__PURE__*/ $constructor("$ZodNanoID", (inst, def) => {
	if (def.length !== void 0 && (!Number.isInteger(def.length) || def.length < 1)) throw new Error(`Invalid nanoid length: ${def.length}`);
	def.pattern ?? (def.pattern = def.length === void 0 ? nanoid : nanoidOfLength(def.length));
	$ZodStringFormat.init(inst, def);
});
/**
* @deprecated CUID v1 is deprecated by its authors due to information leakage
* (timestamps embedded in the id). Use {@link $ZodCUID2} instead.
* See https://github.com/paralleldrive/cuid.
*/
var $ZodCUID = /*@__PURE__*/ $constructor("$ZodCUID", (inst, def) => {
	def.pattern ?? (def.pattern = cuid);
	$ZodStringFormat.init(inst, def);
});
var $ZodCUID2 = /*@__PURE__*/ $constructor("$ZodCUID2", (inst, def) => {
	def.pattern ?? (def.pattern = cuid2);
	$ZodStringFormat.init(inst, def);
});
var $ZodULID = /*@__PURE__*/ $constructor("$ZodULID", (inst, def) => {
	def.pattern ?? (def.pattern = ulid);
	$ZodStringFormat.init(inst, def);
});
var $ZodXID = /*@__PURE__*/ $constructor("$ZodXID", (inst, def) => {
	def.pattern ?? (def.pattern = xid);
	$ZodStringFormat.init(inst, def);
});
var $ZodKSUID = /*@__PURE__*/ $constructor("$ZodKSUID", (inst, def) => {
	def.pattern ?? (def.pattern = ksuid);
	$ZodStringFormat.init(inst, def);
});
var $ZodISODateTime = /*@__PURE__*/ $constructor("$ZodISODateTime", (inst, def) => {
	def.pattern ?? (def.pattern = datetime(def));
	$ZodStringFormat.init(inst, def);
	if (def.local || def.precision === -1) {
		inst._zod.bag.laxFormat = true;
		inst._zod.onattach.push((s) => {
			s._zod.bag.laxFormat = true;
		});
	}
});
var $ZodISODate = /*@__PURE__*/ $constructor("$ZodISODate", (inst, def) => {
	def.pattern ?? (def.pattern = date);
	$ZodStringFormat.init(inst, def);
});
var $ZodISOTime = /*@__PURE__*/ $constructor("$ZodISOTime", (inst, def) => {
	def.pattern ?? (def.pattern = time(def));
	$ZodStringFormat.init(inst, def);
});
var $ZodISODuration = /*@__PURE__*/ $constructor("$ZodISODuration", (inst, def) => {
	def.pattern ?? (def.pattern = duration);
	$ZodStringFormat.init(inst, def);
});
var $ZodIPv4 = /*@__PURE__*/ $constructor("$ZodIPv4", (inst, def) => {
	def.pattern ?? (def.pattern = ipv4);
	$ZodStringFormat.init(inst, def);
	inst._zod.bag.format = `ipv4`;
});
/** An IPv6 address is written with hex digits, colons and dots, and nothing else. The guard is what makes the check below an IPv6 check: `new URL("http://[...]")` parses an authority, not an address, so `@` and `\` re-delimit it and `"::@1\\"` validates against the host `0.0.0.1`. The URL parser also deletes ASCII tab, LF and CR rather than failing, which is how `"::1\n"` validated as `::1`. */
var ipv6Alphabet = /^[0-9a-fA-F:.]+$/;
function isValidIPv6(value) {
	if (!ipv6Alphabet.test(value)) return false;
	try {
		new URL(`http://[${value}]`);
		return true;
	} catch {
		return false;
	}
}
var $ZodIPv6 = /*@__PURE__*/ $constructor("$ZodIPv6", (inst, def) => {
	def.pattern ?? (def.pattern = ipv6);
	$ZodStringFormat.init(inst, def);
	inst._zod.bag.format = `ipv6`;
	inst._zod.check = (payload) => {
		if (!isValidIPv6(payload.value)) payload.issues.push({
			code: "invalid_format",
			format: "ipv6",
			input: payload.value,
			inst,
			continue: !def.abort
		});
	};
});
var $ZodCIDRv4 = /*@__PURE__*/ $constructor("$ZodCIDRv4", (inst, def) => {
	def.pattern ?? (def.pattern = cidrv4);
	$ZodStringFormat.init(inst, def);
});
function isValidCIDRv6(value) {
	const parts = value.split("/");
	if (parts.length !== 2) return false;
	const [address, prefix] = parts;
	if (!prefix) return false;
	const prefixNum = Number(prefix);
	if (`${prefixNum}` !== prefix) return false;
	if (prefixNum < 0 || prefixNum > 128) return false;
	return isValidIPv6(address);
}
var $ZodCIDRv6 = /*@__PURE__*/ $constructor("$ZodCIDRv6", (inst, def) => {
	def.pattern ?? (def.pattern = cidrv6);
	$ZodStringFormat.init(inst, def);
	inst._zod.check = (payload) => {
		if (!isValidCIDRv6(payload.value)) payload.issues.push({
			code: "invalid_format",
			format: "cidrv6",
			input: payload.value,
			inst,
			continue: !def.abort
		});
	};
});
function isValidBase64(data) {
	if (data === "") return true;
	if (/\s/.test(data)) return false;
	if (data.length % 4 !== 0) return false;
	try {
		atob(data);
		return true;
	} catch {
		return false;
	}
}
var $ZodBase64 = /*@__PURE__*/ $constructor("$ZodBase64", (inst, def) => {
	def.pattern ?? (def.pattern = base64);
	$ZodStringFormat.init(inst, def);
	inst._zod.bag.contentEncoding = "base64";
	inst._zod.check = (payload) => {
		if (isValidBase64(payload.value)) return;
		payload.issues.push({
			code: "invalid_format",
			format: "base64",
			input: payload.value,
			inst,
			continue: !def.abort
		});
	};
});
function isValidBase64URL(data) {
	if (!base64url.test(data)) return false;
	const base64 = data.replace(/[-_]/g, (c) => c === "-" ? "+" : "/");
	return isValidBase64(base64.padEnd(Math.ceil(base64.length / 4) * 4, "="));
}
var $ZodBase64URL = /*@__PURE__*/ $constructor("$ZodBase64URL", (inst, def) => {
	def.pattern ?? (def.pattern = base64url);
	$ZodStringFormat.init(inst, def);
	inst._zod.bag.contentEncoding = "base64url";
	inst._zod.check = (payload) => {
		if (isValidBase64URL(payload.value)) return;
		payload.issues.push({
			code: "invalid_format",
			format: "base64url",
			input: payload.value,
			inst,
			continue: !def.abort
		});
	};
});
var $ZodE164 = /*@__PURE__*/ $constructor("$ZodE164", (inst, def) => {
	def.pattern ?? (def.pattern = e164);
	$ZodStringFormat.init(inst, def);
});
function isValidJWT(token, algorithm = null) {
	try {
		const tokensParts = token.split(".");
		if (tokensParts.length !== 3) return false;
		const [header] = tokensParts;
		if (!header) return false;
		const parsedHeader = JSON.parse(atob(header));
		if ("typ" in parsedHeader && parsedHeader?.typ !== "JWT") return false;
		if (!parsedHeader.alg) return false;
		if (algorithm && (!("alg" in parsedHeader) || parsedHeader.alg !== algorithm)) return false;
		return true;
	} catch {
		return false;
	}
}
var $ZodJWT = /*@__PURE__*/ $constructor("$ZodJWT", (inst, def) => {
	$ZodStringFormat.init(inst, def);
	inst._zod.check = (payload) => {
		if (isValidJWT(payload.value, def.alg)) return;
		payload.issues.push({
			code: "invalid_format",
			format: "jwt",
			input: payload.value,
			inst,
			continue: !def.abort
		});
	};
});
var $ZodNumber = /*@__PURE__*/ $constructor("$ZodNumber", (inst, def) => {
	$ZodType.init(inst, def);
	inst._zod.pattern = inst._zod.bag.pattern ?? number$1;
	inst._zod.parse = (payload, _ctx) => {
		if (def.coerce) try {
			payload.value = Number(payload.value);
		} catch (_) {}
		const input = payload.value;
		if (typeof input === "number" && !Number.isNaN(input) && Number.isFinite(input)) return payload;
		const received = typeof input === "number" ? Number.isNaN(input) ? "NaN" : !Number.isFinite(input) ? String(input) : void 0 : void 0;
		payload.issues.push({
			expected: "number",
			code: "invalid_type",
			input,
			inst,
			...received ? { received } : {}
		});
		return payload;
	};
});
var $ZodNumberFormat = /*@__PURE__*/ $constructor("$ZodNumberFormat", (inst, def) => {
	$ZodCheckNumberFormat.init(inst, def);
	$ZodNumber.init(inst, def);
});
var $ZodBoolean = /*@__PURE__*/ $constructor("$ZodBoolean", (inst, def) => {
	$ZodType.init(inst, def);
	inst._zod.pattern = boolean$1;
	inst._zod.parse = (payload, _ctx) => {
		if (def.coerce) try {
			payload.value = Boolean(payload.value);
		} catch (_) {}
		const input = payload.value;
		if (typeof input === "boolean") return payload;
		payload.issues.push({
			expected: "boolean",
			code: "invalid_type",
			input,
			inst
		});
		return payload;
	};
});
var $ZodUnknown = /*@__PURE__*/ $constructor("$ZodUnknown", (inst, def) => {
	$ZodType.init(inst, def);
	inst._zod.parse = (payload) => payload;
});
var $ZodNever = /*@__PURE__*/ $constructor("$ZodNever", (inst, def) => {
	$ZodType.init(inst, def);
	inst._zod.parse = (payload, _ctx) => {
		payload.issues.push({
			expected: "never",
			code: "invalid_type",
			input: payload.value,
			inst
		});
		return payload;
	};
});
function handleArrayResult(result, final, index) {
	if (result.issues.length) final.issues.push(...prefixIssues(index, result.issues));
	final.value[index] = result.value;
}
var $ZodArray = /*@__PURE__*/ $constructor("$ZodArray", (inst, def) => {
	$ZodType.init(inst, def);
	const memo = globalConfig.memoizer;
	memo?.attach(inst);
	inst._zod.parse = (payload, ctx) => {
		const input = payload.value;
		if (!Array.isArray(input)) {
			payload.issues.push({
				expected: "array",
				code: "invalid_type",
				input,
				inst
			});
			return payload;
		}
		payload.value = memo ? memo.alloc(inst, payload, Array(input.length), ctx) : Array(input.length);
		const proms = [];
		for (let i = 0; i < input.length; i++) {
			const item = input[i];
			const result = def.element._zod.run({
				value: item,
				issues: []
			}, ctx);
			if (result instanceof Promise) proms.push(result.then((result) => handleArrayResult(result, payload, i)));
			else handleArrayResult(result, payload, i);
		}
		if (proms.length) return Promise.all(proms).then(() => payload);
		return payload;
	};
});
function handlePropertyResult(result, final, key, input, optin, optout) {
	const isPresent = key in input;
	const isOptionalOut = optout === "optional";
	if (!isPresent && isOptionalOut && optin === "optional") return;
	if (result.issues.length) {
		if (optin !== void 0 && isOptionalOut && !isPresent) return;
		final.issues.push(...prefixIssues(key, result.issues));
	}
	if (!isPresent && optin === void 0) {
		if (!result.issues.length) final.issues.push({
			code: "invalid_type",
			expected: "nonoptional",
			input: void 0,
			path: [key]
		});
		return;
	}
	if (result.value === void 0) {
		if (isPresent) final.value[key] = void 0;
	} else final.value[key] = result.value;
}
var NO_SYMBOL_KEYS = [];
function normalizeDef(def) {
	const keys = Object.keys(def.shape);
	const ownSymbols = Object.getOwnPropertySymbols(def.shape);
	const symbolKeys = ownSymbols.length ? ownSymbols : NO_SYMBOL_KEYS;
	const allKeys = symbolKeys.length ? [...keys, ...symbolKeys] : keys;
	for (const k of allKeys) if (!def.shape?.[k]?._zod?.traits?.has("$ZodType")) throw new Error(`Invalid element at key "${String(k)}": expected a Zod schema`);
	const okeys = optionalKeys(def.shape);
	return {
		...def,
		allKeys,
		symbolKeys,
		keySet: new Set(keys),
		numKeys: keys.length,
		optionalKeys: new Set(okeys)
	};
}
function handleCatchall(proms, input, payload, ctx, def, inst) {
	const unrecognized = [];
	const keySet = def.keySet;
	const _catchall = def.catchall._zod;
	const t = _catchall.def.type;
	const optin = _catchall.optin;
	const optout = _catchall.optout;
	for (const key in input) {
		if (keySet.has(key)) continue;
		if (key === "__proto__") {
			if (t === "never") unrecognized.push(key);
			continue;
		}
		if (t === "never") {
			unrecognized.push(key);
			continue;
		}
		const r = _catchall.run({
			value: input[key],
			issues: []
		}, ctx);
		if (r instanceof Promise) proms.push(r.then((r) => handlePropertyResult(r, payload, key, input, optin, optout)));
		else handlePropertyResult(r, payload, key, input, optin, optout);
	}
	if (unrecognized.length) payload.issues.push({
		code: "unrecognized_keys",
		keys: unrecognized,
		input,
		inst,
		continue: true
	});
	if (!proms.length) return payload;
	return Promise.all(proms).then(() => {
		return payload;
	});
}
var propShapes = /* @__PURE__ */ new WeakMap();
var $ZodObject = /*@__PURE__*/ $constructor("$ZodObject", (inst, def) => {
	$ZodType.init(inst, def);
	if (!Object.getOwnPropertyDescriptor(def, "shape")?.get) {
		const sh = def.shape;
		propShapes.set(def, sh);
		Object.defineProperty(def, "shape", { get: () => {
			const newSh = { ...sh };
			Object.defineProperty(def, "shape", { value: newSh });
			propShapes.set(def, newSh);
			return newSh;
		} });
	}
	const _normalized = cached(() => normalizeDef(def));
	defineLazyInternal(inst, "propValues", (zod) => {
		const shape = zod.def.shape;
		const propValues = {};
		for (const key in shape) {
			const field = shape[key]._zod;
			if (field.values) {
				if (!Object.prototype.hasOwnProperty.call(propValues, key)) assignProp(propValues, key, /* @__PURE__ */ new Set());
				for (const v of field.values) propValues[key].add(v);
				if (field.optin !== void 0) propValues[key].add(void 0);
			}
		}
		return propValues;
	});
	const isObject$2 = isObject;
	const catchall = def.catchall;
	let value;
	const memo = globalConfig.memoizer;
	memo?.attach(inst);
	inst._zod.parse = (payload, ctx) => {
		value ?? (value = _normalized.value);
		const input = payload.value;
		if (!isObject$2(input)) {
			payload.issues.push({
				expected: "object",
				code: "invalid_type",
				input,
				inst
			});
			return payload;
		}
		payload.value = memo ? memo.alloc(inst, payload, {}, ctx) : {};
		const proms = [];
		const shape = value.shape;
		for (const key of value.allKeys) {
			if (key === "__proto__") continue;
			const el = shape[key];
			const optin = el._zod.optin;
			const optout = el._zod.optout;
			const r = el._zod.run({
				value: input[key],
				issues: []
			}, ctx);
			if (r instanceof Promise) proms.push(r.then((r) => handlePropertyResult(r, payload, key, input, optin, optout)));
			else handlePropertyResult(r, payload, key, input, optin, optout);
		}
		if (!catchall) return proms.length ? Promise.all(proms).then(() => payload) : payload;
		return handleCatchall(proms, input, payload, ctx, _normalized.value, inst);
	};
});
var $ZodObjectJIT = /*@__PURE__*/ $constructor("$ZodObjectJIT", (inst, def) => {
	$ZodObject.init(inst, def);
	const superParse = inst._zod.parse;
	const _normalized = cached(() => normalizeDef(def));
	const memo = globalConfig.memoizer;
	const generateFastpass = (shape) => {
		const normalized = _normalized.value;
		const syms = normalized.symbolKeys;
		const doc = new Doc(["payload", "ctx"], {
			shape,
			inst,
			memo,
			syms
		});
		const parseStr = (k) => `shape[${k}]._zod.run({ value: input[${k}], issues: [] }, ctx)`;
		const prefixStr = (id, k) => `
          for (let i = 0; i < ${id}.issues.length; i++) {
            const iss = ${id}.issues[i];
            iss.path = iss.path ? [${k}, ...iss.path] : [${k}];
            payload.issues.push(iss);
          }`;
		doc.write(`const input = payload.value;`);
		const ids = Object.create(null);
		let counter = 0;
		for (const key of normalized.allKeys) ids[key] = `key_${counter++}`;
		doc.write(memo ? `const newResult = memo.alloc(inst, payload, {}, ctx);` : `const newResult = {};`);
		for (const key of normalized.allKeys) {
			if (key === "__proto__") continue;
			const id = ids[key];
			const k = typeof key === "symbol" ? `syms[${syms.indexOf(key)}]` : esc(key);
			const isPresent = `${k} in input`;
			const schema = shape[key];
			const optin = schema?._zod?.optin;
			const isOptionalIn = optin !== void 0;
			const isOptionalOut = schema?._zod?.optout === "optional";
			doc.write(`const ${id} = ${parseStr(k)};`);
			if (isOptionalIn && isOptionalOut) {
				const assign = optin === "optional" ? `${id}_present` : `${id}.value !== undefined || ${id}_present`;
				doc.write(`
        const ${id}_present = ${isPresent};
        if (!${id}.issues.length || ${id}_present) {
          if (${id}.issues.length) {${prefixStr(id, k)}
          }

          if (${assign}) {
            newResult[${k}] = ${id}.value;
          }
        }

      `);
			} else if (!isOptionalIn) doc.write(`
        const ${id}_present = ${isPresent};
        if (${id}.issues.length) {${prefixStr(id, k)}
        }
        if (!${id}_present && !${id}.issues.length) {
          payload.issues.push({
            code: "invalid_type",
            expected: "nonoptional",
            input: undefined,
            path: [${k}]
          });
        }

        if (${id}_present) {
          newResult[${k}] = ${id}.value;
        }

      `);
			else doc.write(`
        if (${id}.issues.length) {${prefixStr(id, k)}
        }
        
        if (${id}.value === undefined) {
          if (${isPresent}) {
            newResult[${k}] = undefined;
          }
        } else {
          newResult[${k}] = ${id}.value;
        }

      `);
		}
		doc.write(`payload.value = newResult;`);
		doc.write(`return payload;`);
		return doc.compile();
	};
	let fastpass;
	const isObject$3 = isObject;
	const jit = !globalConfig.jitless;
	const fastEnabled = jit && allowsEval.value;
	const catchall = def.catchall;
	let value;
	inst._zod.parse = (payload, ctx) => {
		value ?? (value = _normalized.value);
		const input = payload.value;
		if (!isObject$3(input)) {
			payload.issues.push({
				expected: "object",
				code: "invalid_type",
				input,
				inst
			});
			return payload;
		}
		if (jit && fastEnabled && ctx?.async === false && ctx.jitless !== true) {
			if (!fastpass) fastpass = generateFastpass(def.shape);
			payload = fastpass(payload, ctx);
			if (!catchall) return payload;
			return handleCatchall([], input, payload, ctx, value, inst);
		}
		return superParse(payload, ctx);
	};
});
function handleUnionResults(results, final, inst, ctx) {
	for (const result of results) if (result.issues.length === 0) {
		final.value = result.value;
		return final;
	}
	const nonaborted = results.filter((r) => !aborted(r));
	if (nonaborted.length === 1) {
		final.value = nonaborted[0].value;
		return nonaborted[0];
	}
	final.issues.push({
		code: "invalid_union",
		input: final.value,
		inst,
		errors: results.map((result) => result.issues.map((iss) => finalizeIssue(iss, ctx, config())))
	});
	return final;
}
var $ZodUnion = /*@__PURE__*/ $constructor("$ZodUnion", (inst, def) => {
	$ZodType.init(inst, def);
	defineLazyInternal(inst, "optin", (zod) => zod.def.options.some((o) => o._zod.optin === "defaulted") ? "defaulted" : zod.def.options.some((o) => o._zod.optin !== void 0) ? "optional" : void 0);
	defineLazyInternal(inst, "optout", (zod) => zod.def.options.some((o) => o._zod.optout === "optional") ? "optional" : void 0);
	defineLazyInternal(inst, "values", (zod) => {
		if (zod.def.options.every((o) => o._zod.values)) return new Set(zod.def.options.flatMap((option) => Array.from(option._zod.values)));
	});
	defineLazyInternal(inst, "pattern", (zod) => {
		if (zod.def.options.every((o) => o._zod.pattern)) {
			const patterns = zod.def.options.map((o) => o._zod.pattern);
			return new RegExp(`^(${patterns.map((p) => cleanRegex(p.source)).join("|")})$`);
		}
	});
	const first = def.options.length === 1 ? def.options[0]._zod.run : null;
	inst._zod.parse = (payload, ctx) => {
		if (first) return first(payload, ctx);
		let async = false;
		const results = [];
		for (const option of def.options) {
			const result = option._zod.run({
				value: payload.value,
				issues: []
			}, ctx);
			if (result instanceof Promise) {
				results.push(result);
				async = true;
			} else {
				if (result.issues.length === 0) return result;
				results.push(result);
			}
		}
		if (!async) return handleUnionResults(results, payload, inst, ctx);
		return Promise.all(results).then((results) => {
			return handleUnionResults(results, payload, inst, ctx);
		});
	};
});
var $ZodIntersection = /*@__PURE__*/ $constructor("$ZodIntersection", (inst, def) => {
	$ZodType.init(inst, def);
	inst._zod.parse = (payload, ctx) => {
		const input = payload.value;
		const left = def.left._zod.run({
			value: input,
			issues: []
		}, ctx);
		const right = def.right._zod.run({
			value: input,
			issues: []
		}, ctx);
		if (left instanceof Promise || right instanceof Promise) return Promise.all([left, right]).then(([left, right]) => {
			return handleIntersectionResults(payload, left, right);
		});
		return handleIntersectionResults(payload, left, right);
	};
});
function mergeValues(a, b) {
	if (a === b) return {
		valid: true,
		data: a
	};
	if (a instanceof Date && b instanceof Date && +a === +b) return {
		valid: true,
		data: a
	};
	if (isPlainObject(a) && isPlainObject(b)) {
		const bKeys = Object.keys(b);
		const sharedKeys = Object.keys(a).filter((key) => bKeys.indexOf(key) !== -1);
		const newObj = {
			...a,
			...b
		};
		if (Object.prototype.hasOwnProperty.call(newObj, "__proto__")) delete newObj.__proto__;
		for (const key of sharedKeys) {
			if (key === "__proto__") continue;
			const sharedValue = mergeValues(a[key], b[key]);
			if (!sharedValue.valid) return {
				valid: false,
				mergeErrorPath: [key, ...sharedValue.mergeErrorPath]
			};
			newObj[key] = sharedValue.data;
		}
		return {
			valid: true,
			data: newObj
		};
	}
	if (Array.isArray(a) && Array.isArray(b)) {
		if (a.length !== b.length) return {
			valid: false,
			mergeErrorPath: []
		};
		const newArray = [];
		for (let index = 0; index < a.length; index++) {
			const itemA = a[index];
			const itemB = b[index];
			const sharedValue = mergeValues(itemA, itemB);
			if (!sharedValue.valid) return {
				valid: false,
				mergeErrorPath: [index, ...sharedValue.mergeErrorPath]
			};
			newArray.push(sharedValue.data);
		}
		return {
			valid: true,
			data: newArray
		};
	}
	return {
		valid: false,
		mergeErrorPath: []
	};
}
function handleIntersectionResults(result, left, right) {
	const unrecKeys = /* @__PURE__ */ new Map();
	let unrecIssue;
	const keyIssues = /* @__PURE__ */ new Map();
	const collect = (iss, side) => {
		let keys;
		if (iss.code === "unrecognized_keys" && !iss.path?.length) {
			unrecIssue ?? (unrecIssue = iss);
			keys = iss.keys;
		} else if (iss.code === "invalid_key" && iss.origin === "record" && iss.path?.length === 1) {
			const k = String(iss.path[0]);
			if (!keyIssues.has(k)) keyIssues.set(k, iss);
			keys = [k];
		} else return false;
		for (const k of keys) {
			if (!unrecKeys.has(k)) unrecKeys.set(k, {});
			unrecKeys.get(k)[side] = true;
		}
		return true;
	};
	for (const iss of left.issues) if (!collect(iss, "l")) result.issues.push(iss);
	for (const iss of right.issues) if (!collect(iss, "r")) result.issues.push(iss);
	const bothKeys = [...unrecKeys].filter(([, f]) => f.l && f.r).map(([k]) => k);
	if (bothKeys.length) {
		const aggregated = unrecIssue ? bothKeys.filter((k) => unrecIssue.keys.includes(k)) : [];
		if (aggregated.length) result.issues.push({
			...unrecIssue,
			keys: aggregated
		});
		for (const k of bothKeys) if (!aggregated.includes(k) && keyIssues.has(k)) result.issues.push(keyIssues.get(k));
	}
	const merged = mergeValues(left.value, right.value);
	if (!merged.valid) {
		if (aborted(result)) return result;
		throw new Error(`Unmergable intersection. Error path: ${JSON.stringify(merged.mergeErrorPath)}`);
	}
	result.value = merged.data;
	return result;
}
var $ZodEnum = /*@__PURE__*/ $constructor("$ZodEnum", (inst, def) => {
	$ZodType.init(inst, def);
	const values = getEnumValues(def.entries);
	const valuesSet = new Set(values);
	inst._zod.values = valuesSet;
	const patternValues = values.filter((k) => propertyKeyTypes.has(typeof k));
	inst._zod.pattern = new RegExp(patternValues.length ? `^(${patternValues.map((o) => escapeRegex(o.toString())).join("|")})$` : "^[^\\s\\S]$");
	inst._zod.parse = (payload, _ctx) => {
		const input = payload.value;
		if (valuesSet.has(input)) return payload;
		payload.issues.push({
			code: "invalid_value",
			values,
			input,
			inst
		});
		return payload;
	};
});
var $ZodTransform = /*@__PURE__*/ $constructor("$ZodTransform", (inst, def) => {
	$ZodType.init(inst, def);
	inst._zod.optin = "optional";
	globalConfig.memoizer?.guard(inst);
	inst._zod.parse = (payload, ctx) => {
		if (ctx.direction === "backward") throw new $ZodEncodeError(inst.constructor.name);
		const _out = def.transform(payload.value, payload);
		if (ctx.async) return (_out instanceof Promise ? _out : Promise.resolve(_out)).then((output) => {
			payload.value = output;
			return payload;
		});
		if (_out instanceof Promise) throw new $ZodAsyncError();
		payload.value = _out;
		return payload;
	};
});
function handleOptionalResult(payload, result) {
	payload.value = result.issues.length ? void 0 : result.value;
	return payload;
}
var $ZodOptional = /*@__PURE__*/ $constructor("$ZodOptional", (inst, def) => {
	$ZodType.init(inst, def);
	defineLazyInternal(inst, "optin", (zod) => zod.def.innerType._zod.optin === "defaulted" ? "defaulted" : "optional");
	inst._zod.optout = "optional";
	defineLazyInternal(inst, "values", (zod) => {
		const values = zod.def.innerType._zod.values;
		return values ? /* @__PURE__ */ new Set([...values, void 0]) : void 0;
	});
	defineLazyInternal(inst, "pattern", (zod) => {
		const pattern = zod.def.innerType._zod.pattern;
		return pattern ? new RegExp(`^(${cleanRegex(pattern.source)})?$`) : void 0;
	});
	inst._zod.parse = (payload, ctx) => {
		if (payload.value === void 0) {
			if (def.innerType._zod.optin !== "defaulted") return payload;
			const result = def.innerType._zod.run({
				value: payload.value,
				issues: []
			}, ctx);
			if (result instanceof Promise) return result.then((result) => handleOptionalResult(payload, result));
			return handleOptionalResult(payload, result);
		}
		return def.innerType._zod.run(payload, ctx);
	};
});
var $ZodExactOptional = /*@__PURE__*/ $constructor("$ZodExactOptional", (inst, def) => {
	$ZodOptional.init(inst, def);
	defineLazyInternal(inst, "values", (zod) => zod.def.innerType._zod.values);
	defineLazyInternal(inst, "pattern", (zod) => zod.def.innerType._zod.pattern);
	inst._zod.parse = (payload, ctx) => {
		return def.innerType._zod.run(payload, ctx);
	};
});
var $ZodNullable = /*@__PURE__*/ $constructor("$ZodNullable", (inst, def) => {
	$ZodType.init(inst, def);
	defineLazyInternal(inst, "optin", (zod) => zod.def.innerType._zod.optin);
	defineLazyInternal(inst, "optout", (zod) => zod.def.innerType._zod.optout);
	defineLazyInternal(inst, "pattern", (zod) => {
		const pattern = zod.def.innerType._zod.pattern;
		return pattern ? new RegExp(`^(${cleanRegex(pattern.source)}|null)$`) : void 0;
	});
	defineLazyInternal(inst, "values", (zod) => {
		return zod.def.innerType._zod.values ? /* @__PURE__ */ new Set([...zod.def.innerType._zod.values, null]) : void 0;
	});
	inst._zod.parse = (payload, ctx) => {
		if (payload.value === null) return payload;
		return def.innerType._zod.run(payload, ctx);
	};
});
var $ZodDefault = /*@__PURE__*/ $constructor("$ZodDefault", (inst, def) => {
	$ZodType.init(inst, def);
	inst._zod.optin = "defaulted";
	defineLazyInternal(inst, "values", (zod) => zod.def.innerType._zod.values);
	inst._zod.parse = (payload, ctx) => {
		if (ctx.direction === "backward") return def.innerType._zod.run(payload, ctx);
		if (payload.value === void 0) {
			payload.value = def.defaultValue;
			/**
			* $ZodDefault returns the default value immediately in forward direction.
			* It doesn't pass the default value into the validator ("prefault"). There's no reason to pass the default value through validation. The validity of the default is enforced by TypeScript statically. Otherwise, it's the responsibility of the user to ensure the default is valid. In the case of pipes with divergent in/out types, you can specify the default on the `in` schema of your ZodPipe to set a "prefault" for the pipe.   */
			return payload;
		}
		const result = def.innerType._zod.run(payload, ctx);
		if (result instanceof Promise) return result.then((result) => handleDefaultResult(result, def));
		return handleDefaultResult(result, def);
	};
});
function handleDefaultResult(payload, def) {
	if (payload.value === void 0) payload.value = def.defaultValue;
	return payload;
}
var $ZodPrefault = /*@__PURE__*/ $constructor("$ZodPrefault", (inst, def) => {
	$ZodType.init(inst, def);
	inst._zod.optin = "defaulted";
	defineLazyInternal(inst, "values", (zod) => zod.def.innerType._zod.values);
	inst._zod.parse = (payload, ctx) => {
		if (ctx.direction === "backward") return def.innerType._zod.run(payload, ctx);
		if (payload.value === void 0) payload.value = def.defaultValue;
		return def.innerType._zod.run(payload, ctx);
	};
});
var $ZodNonOptional = /*@__PURE__*/ $constructor("$ZodNonOptional", (inst, def) => {
	$ZodType.init(inst, def);
	defineLazyInternal(inst, "values", (zod) => {
		const v = zod.def.innerType._zod.values;
		return v ? new Set([...v].filter((x) => x !== void 0)) : void 0;
	});
	inst._zod.parse = (payload, ctx) => {
		const result = def.innerType._zod.run(payload, ctx);
		if (result instanceof Promise) return result.then((result) => handleNonOptionalResult(result, inst));
		return handleNonOptionalResult(result, inst);
	};
});
function handleNonOptionalResult(payload, inst) {
	if (!payload.issues.length && payload.value === void 0) payload.issues.push({
		code: "invalid_type",
		expected: "nonoptional",
		input: payload.value,
		inst
	});
	return payload;
}
function handleCatchResult(payload, result, def, ctx) {
	if (!result.issues.length) {
		payload.value = result.value;
		if (result.memo) payload.memo = true;
		return payload;
	}
	payload.value = def.catchValue({
		...result,
		value: payload.value,
		error: { issues: result.issues.map((iss) => finalizeIssue(iss, ctx, config())) },
		input: payload.value
	});
	return payload;
}
var $ZodCatch = /*@__PURE__*/ $constructor("$ZodCatch", (inst, def) => {
	$ZodType.init(inst, def);
	defineLazyInternal(inst, "optin", (zod) => zod.def.innerType._zod.optin === "defaulted" ? "defaulted" : "optional");
	defineLazyInternal(inst, "optout", (zod) => zod.def.innerType._zod.optout);
	defineLazyInternal(inst, "values", (zod) => zod.def.innerType._zod.values);
	inst._zod.parse = (payload, ctx) => {
		if (ctx.direction === "backward") return def.innerType._zod.run(payload, ctx);
		const result = def.innerType._zod.run({
			value: payload.value,
			issues: []
		}, ctx);
		if (result instanceof Promise) return result.then((result) => handleCatchResult(payload, result, def, ctx));
		return handleCatchResult(payload, result, def, ctx);
	};
});
var $ZodPipe = /*@__PURE__*/ $constructor("$ZodPipe", (inst, def) => {
	$ZodType.init(inst, def);
	defineLazyInternal(inst, "values", (zod) => zod.def.in._zod.values);
	defineLazyInternal(inst, "optin", (zod) => zod.def.in._zod.optin);
	defineLazyInternal(inst, "optout", (zod) => zod.def.out._zod.optout);
	defineLazyInternal(inst, "propValues", (zod) => zod.def.in._zod.propValues);
	inst._zod.parse = (payload, ctx) => {
		if (ctx.direction === "backward") {
			const right = def.out._zod.run(payload, ctx);
			if (right instanceof Promise) return right.then((right) => handlePipeResult(right, def.in, ctx));
			return handlePipeResult(right, def.in, ctx);
		}
		const left = def.in._zod.run(payload, ctx);
		if (left instanceof Promise) return left.then((left) => handlePipeResult(left, def.out, ctx));
		return handlePipeResult(left, def.out, ctx);
	};
});
function handlePipeResult(left, next, ctx) {
	if (left.issues.some((iss) => iss.code !== "unrecognized_keys")) {
		left.aborted = true;
		return left;
	}
	return next._zod.run({
		value: left.value,
		issues: left.issues
	}, ctx);
}
var $ZodReadonly = /*@__PURE__*/ $constructor("$ZodReadonly", (inst, def) => {
	$ZodType.init(inst, def);
	defineLazyInternal(inst, "propValues", (zod) => zod.def.innerType._zod.propValues);
	defineLazyInternal(inst, "values", (zod) => zod.def.innerType._zod.values);
	defineLazyInternal(inst, "optin", (zod) => zod.def.innerType?._zod?.optin);
	defineLazyInternal(inst, "optout", (zod) => zod.def.innerType?._zod?.optout);
	inst._zod.parse = (payload, ctx) => {
		if (ctx.direction === "backward") return def.innerType._zod.run(payload, ctx);
		const result = def.innerType._zod.run(payload, ctx);
		if (result instanceof Promise) return result.then(handleReadonlyResult);
		return handleReadonlyResult(result);
	};
});
function handleReadonlyResult(payload) {
	if (!payload.memo) payload.value = Object.freeze(payload.value);
	return payload;
}
var $ZodCustom = /*@__PURE__*/ $constructor("$ZodCustom", (inst, def) => {
	$ZodCheck.init(inst, def);
	$ZodType.init(inst, def);
	inst._zod.parse = (payload, _) => {
		return payload;
	};
	inst._zod.check = (payload) => {
		const input = payload.value;
		const r = def.fn(input);
		if (r instanceof Promise) return r.then((r) => handleRefineResult(r, payload, input, inst));
		handleRefineResult(r, payload, input, inst);
	};
});
function handleRefineResult(result, payload, input, inst) {
	if (!result) {
		const _iss = {
			code: "custom",
			input,
			inst,
			path: [...inst._zod.def.path ?? []],
			continue: !inst._zod.def.abort
		};
		if (inst._zod.def.params) _iss.params = inst._zod.def.params;
		payload.issues.push(issue(_iss));
	}
}
//#endregion
//#region node_modules/zod/v4/core/memoizer.js
var $ZodCyclicError = class extends Error {
	constructor() {
		super(`Cannot parse a reference cycle that closes through a transform`);
		this.name = "ZodCyclicError";
	}
};
/** Keyed off the context object every schema in one parse call already shares. */
var STATE = "~memo";
var NO_ISSUES = [];
function cloneIssues(issues) {
	return issues.map((iss) => iss.path ? {
		...iss,
		path: iss.path.slice()
	} : { ...iss });
}
var recursive = /*@__PURE__*/ new WeakMap();
/** Whether this schema's subtree contains a cycle, so one parse can re-enter it. */
function isRecursive(inst, stack) {
	const cached = recursive.get(inst);
	if (cached !== void 0) return cached;
	if (stack.has(inst)) return true;
	stack.add(inst);
	let result = false;
	const check = (child) => {
		if (!result && child?._zod && isRecursive(child, stack)) result = true;
	};
	const def = inst._zod.def;
	switch (def.type) {
		case "object":
			for (const key of Reflect.ownKeys(def.shape)) check(def.shape[key]);
			check(def.catchall);
			break;
		case "array":
			check(def.element);
			break;
		case "tuple":
			for (const el of def.items) check(el);
			check(def.rest);
			break;
		case "record":
		case "map":
			check(def.keyType);
			check(def.valueType);
			break;
		case "set":
			check(def.valueType);
			break;
		case "union":
			for (const el of def.options) check(el);
			break;
		case "intersection":
			check(def.left);
			check(def.right);
			break;
		case "optional":
		case "nullable":
		case "default":
		case "prefault":
		case "catch":
		case "readonly":
		case "nonoptional":
		case "promise":
		case "success":
			check(def.innerType);
			break;
		case "pipe":
			check(def.in);
			check(def.out);
			break;
		case "function":
			check(def.input);
			check(def.output);
			break;
		case "lazy":
			check(inst._zod.innerType);
			break;
		case "template_literal":
		case "string":
		case "number":
		case "int":
		case "boolean":
		case "bigint":
		case "symbol":
		case "undefined":
		case "null":
		case "void":
		case "never":
		case "any":
		case "unknown":
		case "date":
		case "nan":
		case "enum":
		case "literal":
		case "file":
		case "transform":
		case "custom": break;
		default: for (const key in def) {
			const desc = Object.getOwnPropertyDescriptor(def, key);
			if (!desc || desc.get) continue;
			const value = desc.value;
			if (!value || typeof value !== "object") continue;
			if (value._zod) check(value);
			else if (Array.isArray(value)) for (const el of value) check(el);
		}
	}
	stack.delete(inst);
	recursive.set(inst, result);
	return result;
}
function bucketFor(state, inst) {
	let bucket = state.buckets.get(inst);
	if (!bucket) {
		bucket = /* @__PURE__ */ new Map();
		state.buckets.set(inst, bucket);
	}
	return bucket;
}
var handoff;
var open = [];
var memo = {
	alloc(_inst, payload, empty) {
		const bucket = handoff;
		if (!bucket) return empty;
		handoff = void 0;
		const entry = {
			value: empty,
			issues: null
		};
		bucket.set(payload.value, entry);
		open.push(entry);
		return empty;
	},
	guard(inst) {
		var _a;
		(_a = inst._zod).deferred ?? (_a.deferred = []);
		inst._zod.deferred.push(() => {
			const base = inst._zod.parse;
			const wrapped = (payload, ctx) => {
				if (ctx.direction !== "backward" && isBackEdge(ctx, payload.value)) throw new $ZodCyclicError();
				return base(payload, ctx);
			};
			inst._zod.parse = wrapped;
			if (inst._zod.run === base) inst._zod.run = wrapped;
		});
	},
	attach(inst) {
		var _a;
		let isRecursiveInst;
		let lastCtx;
		let lastBucket;
		(_a = inst._zod).deferred ?? (_a.deferred = []);
		inst._zod.deferred.push(() => {
			const base = inst._zod.parse;
			const wrapped = (payload, ctx) => {
				if (isRecursiveInst === void 0) {
					isRecursiveInst = isRecursive(inst, /* @__PURE__ */ new Set());
					if (!isRecursiveInst) {
						inst._zod.parse = base;
						if (inst._zod.run === wrapped) inst._zod.run = base;
						return base(payload, ctx);
					}
				}
				const input = payload.value;
				if (input === null || typeof input !== "object") return base(payload, ctx);
				let state = ctx[STATE];
				if (!state) {
					state = {
						buckets: /* @__PURE__ */ new Map(),
						backEdges: void 0
					};
					ctx[STATE] = state;
				}
				let bucket;
				if (lastCtx === ctx) bucket = lastBucket;
				else {
					bucket = bucketFor(state, inst);
					lastCtx = ctx;
					lastBucket = bucket;
				}
				const hit = bucket.get(input);
				if (hit) {
					payload.value = hit.value;
					if (hit.issues) {
						if (hit.issues.length) payload.issues.push(...cloneIssues(hit.issues));
					} else {
						payload.memo = true;
						state.backEdges ?? (state.backEdges = /* @__PURE__ */ new Set());
						state.backEdges.add(hit.value);
					}
					return payload;
				}
				handoff = bucket;
				const depth = open.length;
				const result = base(payload, ctx);
				handoff = void 0;
				const entry = open.length > depth ? open.pop() : void 0;
				if (result instanceof Promise) return result.then((r) => {
					if (entry) entry.issues = r.issues.length ? cloneIssues(r.issues) : NO_ISSUES;
					return r;
				});
				if (entry) entry.issues = result.issues.length ? cloneIssues(result.issues) : NO_ISSUES;
				return result;
			};
			inst._zod.parse = wrapped;
			if (inst._zod.run === base) inst._zod.run = wrapped;
		});
	}
};
/** The memoizer that gives containers cycle support. `zod` installs it by default; `zod/mini` opts in with `config({ memoizer: memoizer() })`. */
function memoizer() {
	return memo;
}
/** Whether this value is a node a back-edge resolved to before it finished. */
function isBackEdge(ctx, value) {
	const backEdges = ctx[STATE]?.backEdges;
	return backEdges !== void 0 && value !== null && typeof value === "object" && backEdges.has(value);
}
//#endregion
//#region node_modules/zod/v4/locales/en.js
var error = () => {
	const Sizable = {
		string: {
			unit: "characters",
			verb: "to have"
		},
		file: {
			unit: "bytes",
			verb: "to have"
		},
		array: {
			unit: "items",
			verb: "to have"
		},
		set: {
			unit: "items",
			verb: "to have"
		},
		map: {
			unit: "entries",
			verb: "to have"
		}
	};
	function getSizing(origin) {
		return Sizable[origin] ?? null;
	}
	const FormatDictionary = {
		regex: "input",
		email: "email address",
		url: "URL",
		emoji: "emoji",
		uuid: "UUID",
		uuidv4: "UUIDv4",
		uuidv6: "UUIDv6",
		nanoid: "nanoid",
		guid: "GUID",
		cuid: "cuid",
		cuid2: "cuid2",
		ulid: "ULID",
		xid: "XID",
		ksuid: "KSUID",
		datetime: "ISO datetime",
		date: "ISO date",
		time: "ISO time",
		duration: "ISO duration",
		ipv4: "IPv4 address",
		ipv6: "IPv6 address",
		mac: "MAC address",
		cidrv4: "IPv4 range",
		cidrv6: "IPv6 range",
		base64: "base64-encoded string",
		base64url: "base64url-encoded string",
		json_string: "JSON string",
		e164: "E.164 number",
		credit_card: "credit card number",
		jwt: "JWT",
		template_literal: "input"
	};
	const TypeDictionary = { nan: "NaN" };
	function getTypeName(type, input) {
		if (type === "number" && typeof input === "number" && !Number.isFinite(input)) return String(input);
		return TypeDictionary[type] ?? type;
	}
	return (issue) => {
		switch (issue.code) {
			case "invalid_type": return `Invalid input: expected ${getTypeName(issue.expected)}, received ${getTypeName(parsedType(issue.input), issue.input)}`;
			case "invalid_value":
				if (issue.values.length === 1) return `Invalid input: expected ${stringifyPrimitive(issue.values[0])}`;
				return `Invalid option: expected one of ${joinValues(issue.values, "|")}`;
			case "too_big": {
				const adj = issue.exact ? "exactly " : issue.inclusive ? "<=" : "<";
				const sizing = getSizing(issue.origin);
				if (sizing) return `Too big: expected ${issue.origin ?? "value"} to have ${adj}${issue.maximum.toString()} ${sizing.unit ?? "elements"}`;
				return `Too big: expected ${issue.origin ?? "value"} to be ${adj}${issue.maximum.toString()}`;
			}
			case "too_small": {
				const adj = issue.exact ? "exactly " : issue.inclusive ? ">=" : ">";
				const sizing = getSizing(issue.origin);
				if (sizing) return `Too small: expected ${issue.origin} to have ${adj}${issue.minimum.toString()} ${sizing.unit}`;
				return `Too small: expected ${issue.origin} to be ${adj}${issue.minimum.toString()}`;
			}
			case "invalid_format": {
				const _issue = issue;
				if (_issue.format === "starts_with") return `Invalid string: must start with "${_issue.prefix}"`;
				if (_issue.format === "ends_with") return `Invalid string: must end with "${_issue.suffix}"`;
				if (_issue.format === "includes") return `Invalid string: must include "${_issue.includes}"`;
				if (_issue.format === "regex") return `Invalid string: must match pattern ${_issue.pattern}`;
				return `Invalid ${FormatDictionary[_issue.format] ?? issue.format}`;
			}
			case "not_multiple_of": return `Invalid number: must be a multiple of ${issue.divisor}`;
			case "unrecognized_keys": return `Unrecognized key${issue.keys.length > 1 ? "s" : ""}: ${joinValues(issue.keys, ", ")}`;
			case "invalid_key": return `Invalid key in ${issue.origin}`;
			case "invalid_union":
				if (issue.options && Array.isArray(issue.options) && issue.options.length > 0) return `Invalid discriminator value. Expected ${issue.options.map((o) => `'${o}'`).join(" | ")}`;
				if (issue.inclusive === false) return "Invalid input: more than one option matched";
				return "Invalid input";
			case "invalid_element": return `Invalid value in ${issue.origin}`;
			default: return `Invalid input`;
		}
	};
};
function en_default() {
	return { localeError: error() };
}
//#endregion
//#region node_modules/zod/v4/core/registries.js
var _a;
var $ZodRegistry = class {
	constructor() {
		this._map = /* @__PURE__ */ new WeakMap();
		this._idmap = /* @__PURE__ */ new Map();
	}
	add(schema, ..._meta) {
		const meta = _meta[0];
		this._map.set(schema, meta);
		if (meta && typeof meta === "object" && "id" in meta) this._idmap.set(meta.id, schema);
		return this;
	}
	clear() {
		this._map = /* @__PURE__ */ new WeakMap();
		this._idmap = /* @__PURE__ */ new Map();
		return this;
	}
	remove(schema) {
		const meta = this._map.get(schema);
		if (meta && typeof meta === "object" && "id" in meta) this._idmap.delete(meta.id);
		this._map.delete(schema);
		return this;
	}
	get(schema) {
		const p = schema._zod.parent;
		if (p) {
			const pm = { ...this.get(p) ?? {} };
			delete pm.id;
			const f = {
				...pm,
				...this._map.get(schema)
			};
			return Object.keys(f).length ? f : void 0;
		}
		return this._map.get(schema);
	}
	has(schema) {
		return this._map.has(schema);
	}
};
function registry() {
	return new $ZodRegistry();
}
(_a = globalThis).__zod_globalRegistry ?? (_a.__zod_globalRegistry = registry());
var globalRegistry = globalThis.__zod_globalRegistry;
//#endregion
//#region node_modules/zod/v4/core/api.js
// @__NO_SIDE_EFFECTS__
function _string(Class, params) {
	return new Class({
		type: "string",
		...normalizeParams(params)
	});
}
// @__NO_SIDE_EFFECTS__
function _email(Class, params) {
	return new Class({
		type: "string",
		format: "email",
		check: "string_format",
		abort: false,
		...normalizeParams(params)
	});
}
// @__NO_SIDE_EFFECTS__
function _guid(Class, params) {
	return new Class({
		type: "string",
		format: "guid",
		check: "string_format",
		abort: false,
		...normalizeParams(params)
	});
}
// @__NO_SIDE_EFFECTS__
function _uuid(Class, params) {
	return new Class({
		type: "string",
		format: "uuid",
		check: "string_format",
		abort: false,
		...normalizeParams(params)
	});
}
// @__NO_SIDE_EFFECTS__
function _uuidv4(Class, params) {
	return new Class({
		type: "string",
		format: "uuid",
		check: "string_format",
		abort: false,
		version: "v4",
		...normalizeParams(params)
	});
}
// @__NO_SIDE_EFFECTS__
function _uuidv6(Class, params) {
	return new Class({
		type: "string",
		format: "uuid",
		check: "string_format",
		abort: false,
		version: "v6",
		...normalizeParams(params)
	});
}
// @__NO_SIDE_EFFECTS__
function _uuidv7(Class, params) {
	return new Class({
		type: "string",
		format: "uuid",
		check: "string_format",
		abort: false,
		version: "v7",
		...normalizeParams(params)
	});
}
// @__NO_SIDE_EFFECTS__
function _url(Class, params) {
	return new Class({
		type: "string",
		format: "url",
		check: "string_format",
		abort: false,
		...normalizeParams(params)
	});
}
// @__NO_SIDE_EFFECTS__
function _emoji(Class, params) {
	return new Class({
		type: "string",
		format: "emoji",
		check: "string_format",
		abort: false,
		...normalizeParams(params)
	});
}
// @__NO_SIDE_EFFECTS__
function _nanoid(Class, params) {
	return new Class({
		type: "string",
		format: "nanoid",
		check: "string_format",
		abort: false,
		...normalizeParams(params)
	});
}
/**
* @deprecated CUID v1 is deprecated by its authors due to information leakage
* (timestamps embedded in the id). Use {@link _cuid2} instead.
* See https://github.com/paralleldrive/cuid.
*/
// @__NO_SIDE_EFFECTS__
function _cuid(Class, params) {
	return new Class({
		type: "string",
		format: "cuid",
		check: "string_format",
		abort: false,
		...normalizeParams(params)
	});
}
// @__NO_SIDE_EFFECTS__
function _cuid2(Class, params) {
	return new Class({
		type: "string",
		format: "cuid2",
		check: "string_format",
		abort: false,
		...normalizeParams(params)
	});
}
// @__NO_SIDE_EFFECTS__
function _ulid(Class, params) {
	return new Class({
		type: "string",
		format: "ulid",
		check: "string_format",
		abort: false,
		...normalizeParams(params)
	});
}
// @__NO_SIDE_EFFECTS__
function _xid(Class, params) {
	return new Class({
		type: "string",
		format: "xid",
		check: "string_format",
		abort: false,
		...normalizeParams(params)
	});
}
// @__NO_SIDE_EFFECTS__
function _ksuid(Class, params) {
	return new Class({
		type: "string",
		format: "ksuid",
		check: "string_format",
		abort: false,
		...normalizeParams(params)
	});
}
// @__NO_SIDE_EFFECTS__
function _ipv4(Class, params) {
	return new Class({
		type: "string",
		format: "ipv4",
		check: "string_format",
		abort: false,
		...normalizeParams(params)
	});
}
// @__NO_SIDE_EFFECTS__
function _ipv6(Class, params) {
	return new Class({
		type: "string",
		format: "ipv6",
		check: "string_format",
		abort: false,
		...normalizeParams(params)
	});
}
// @__NO_SIDE_EFFECTS__
function _cidrv4(Class, params) {
	return new Class({
		type: "string",
		format: "cidrv4",
		check: "string_format",
		abort: false,
		...normalizeParams(params)
	});
}
// @__NO_SIDE_EFFECTS__
function _cidrv6(Class, params) {
	return new Class({
		type: "string",
		format: "cidrv6",
		check: "string_format",
		abort: false,
		...normalizeParams(params)
	});
}
// @__NO_SIDE_EFFECTS__
function _base64(Class, params) {
	return new Class({
		type: "string",
		format: "base64",
		check: "string_format",
		abort: false,
		...normalizeParams(params)
	});
}
// @__NO_SIDE_EFFECTS__
function _base64url(Class, params) {
	return new Class({
		type: "string",
		format: "base64url",
		check: "string_format",
		abort: false,
		...normalizeParams(params)
	});
}
// @__NO_SIDE_EFFECTS__
function _e164(Class, params) {
	return new Class({
		type: "string",
		format: "e164",
		check: "string_format",
		abort: false,
		...normalizeParams(params)
	});
}
// @__NO_SIDE_EFFECTS__
function _jwt(Class, params) {
	return new Class({
		type: "string",
		format: "jwt",
		check: "string_format",
		abort: false,
		...normalizeParams(params)
	});
}
// @__NO_SIDE_EFFECTS__
function _isoDateTime(Class, params) {
	return new Class({
		type: "string",
		format: "datetime",
		check: "string_format",
		offset: false,
		local: false,
		precision: null,
		...normalizeParams(params)
	});
}
// @__NO_SIDE_EFFECTS__
function _isoDate(Class, params) {
	return new Class({
		type: "string",
		format: "date",
		check: "string_format",
		...normalizeParams(params)
	});
}
// @__NO_SIDE_EFFECTS__
function _isoTime(Class, params) {
	return new Class({
		type: "string",
		format: "time",
		check: "string_format",
		precision: null,
		...normalizeParams(params)
	});
}
// @__NO_SIDE_EFFECTS__
function _isoDuration(Class, params) {
	return new Class({
		type: "string",
		format: "duration",
		check: "string_format",
		...normalizeParams(params)
	});
}
// @__NO_SIDE_EFFECTS__
function _number(Class, params) {
	return new Class({
		type: "number",
		checks: [],
		...normalizeParams(params)
	});
}
// @__NO_SIDE_EFFECTS__
function _int(Class, params) {
	return new Class({
		type: "number",
		check: "number_format",
		abort: false,
		format: "safeint",
		...normalizeParams(params)
	});
}
// @__NO_SIDE_EFFECTS__
function _float32(Class, params) {
	return new Class({
		type: "number",
		check: "number_format",
		abort: false,
		format: "float32",
		...normalizeParams(params)
	});
}
// @__NO_SIDE_EFFECTS__
function _boolean(Class, params) {
	return new Class({
		type: "boolean",
		...normalizeParams(params)
	});
}
// @__NO_SIDE_EFFECTS__
function _unknown(Class) {
	return new Class({ type: "unknown" });
}
// @__NO_SIDE_EFFECTS__
function _never(Class, params) {
	return new Class({
		type: "never",
		...normalizeParams(params)
	});
}
// @__NO_SIDE_EFFECTS__
function _lt(value, params) {
	return new $ZodCheckLessThan({
		check: "less_than",
		...normalizeParams(params),
		value,
		inclusive: false
	});
}
// @__NO_SIDE_EFFECTS__
function _lte(value, params) {
	return new $ZodCheckLessThan({
		check: "less_than",
		...normalizeParams(params),
		value,
		inclusive: true
	});
}
// @__NO_SIDE_EFFECTS__
function _gt(value, params) {
	return new $ZodCheckGreaterThan({
		check: "greater_than",
		...normalizeParams(params),
		value,
		inclusive: false
	});
}
// @__NO_SIDE_EFFECTS__
function _gte(value, params) {
	return new $ZodCheckGreaterThan({
		check: "greater_than",
		...normalizeParams(params),
		value,
		inclusive: true
	});
}
// @__NO_SIDE_EFFECTS__
function _multipleOf(value, params) {
	return new $ZodCheckMultipleOf({
		check: "multiple_of",
		...normalizeParams(params),
		value
	});
}
// @__NO_SIDE_EFFECTS__
function _maxLength(maximum, params) {
	return new $ZodCheckMaxLength({
		check: "max_length",
		...normalizeParams(params),
		maximum
	});
}
// @__NO_SIDE_EFFECTS__
function _minLength(minimum, params) {
	return new $ZodCheckMinLength({
		check: "min_length",
		...normalizeParams(params),
		minimum
	});
}
// @__NO_SIDE_EFFECTS__
function _length(length, params) {
	return new $ZodCheckLengthEquals({
		check: "length_equals",
		...normalizeParams(params),
		length
	});
}
// @__NO_SIDE_EFFECTS__
function _regex(pattern, params) {
	return new $ZodCheckRegex({
		check: "string_format",
		format: "regex",
		...normalizeParams(params),
		pattern
	});
}
// @__NO_SIDE_EFFECTS__
function _lowercase(params) {
	return new $ZodCheckLowerCase({
		check: "string_format",
		format: "lowercase",
		...normalizeParams(params)
	});
}
// @__NO_SIDE_EFFECTS__
function _uppercase(params) {
	return new $ZodCheckUpperCase({
		check: "string_format",
		format: "uppercase",
		...normalizeParams(params)
	});
}
// @__NO_SIDE_EFFECTS__
function _includes(includes, params) {
	return new $ZodCheckIncludes({
		check: "string_format",
		format: "includes",
		...normalizeParams(params),
		includes
	});
}
// @__NO_SIDE_EFFECTS__
function _startsWith(prefix, params) {
	return new $ZodCheckStartsWith({
		check: "string_format",
		format: "starts_with",
		...normalizeParams(params),
		prefix
	});
}
// @__NO_SIDE_EFFECTS__
function _endsWith(suffix, params) {
	return new $ZodCheckEndsWith({
		check: "string_format",
		format: "ends_with",
		...normalizeParams(params),
		suffix
	});
}
// @__NO_SIDE_EFFECTS__
function _overwrite(tx) {
	return new $ZodCheckOverwrite({
		check: "overwrite",
		tx
	});
}
// @__NO_SIDE_EFFECTS__
function _normalize(form) {
	return /* @__PURE__ */ _overwrite((input) => input.normalize(form));
}
// @__NO_SIDE_EFFECTS__
function _trim() {
	return /* @__PURE__ */ _overwrite((input) => input.trim());
}
// @__NO_SIDE_EFFECTS__
function _toLowerCase() {
	return /* @__PURE__ */ _overwrite((input) => input.toLowerCase());
}
// @__NO_SIDE_EFFECTS__
function _toUpperCase() {
	return /* @__PURE__ */ _overwrite((input) => input.toUpperCase());
}
// @__NO_SIDE_EFFECTS__
function _slugify() {
	return /* @__PURE__ */ _overwrite((input) => slugify(input));
}
// @__NO_SIDE_EFFECTS__
function _array(Class, element, params) {
	return new Class({
		type: "array",
		element,
		...normalizeParams(params)
	});
}
// @__NO_SIDE_EFFECTS__
function _refine(Class, fn, _params) {
	return new Class({
		type: "custom",
		check: "custom",
		fn,
		...normalizeParams(_params)
	});
}
// @__NO_SIDE_EFFECTS__
function _superRefine(fn, params) {
	const ch = /* @__PURE__ */ _check((payload) => {
		payload.addIssue = (issue$2) => {
			if (typeof issue$2 === "string") payload.issues.push(issue(issue$2, payload.value, ch._zod.def));
			else {
				const _issue = issue$2;
				if (_issue.fatal) _issue.continue = false;
				_issue.code ?? (_issue.code = "custom");
				if (!("input" in _issue)) _issue.input = payload.value;
				_issue.inst ?? (_issue.inst = ch);
				_issue.continue ?? (_issue.continue = !ch._zod.def.abort);
				payload.issues.push(issue(_issue));
			}
		};
		return fn(payload.value, payload);
	}, params);
	return ch;
}
// @__NO_SIDE_EFFECTS__
function _check(fn, params) {
	const ch = new $ZodCheck({
		check: "custom",
		...normalizeParams(params)
	});
	ch._zod.check = fn;
	return ch;
}
//#endregion
//#region node_modules/zod/v4/core/to-json-schema.js
function assignProps(target, ...sources) {
	for (const source of sources) for (const key of Reflect.ownKeys(source)) if (Object.prototype.propertyIsEnumerable.call(source, key)) assignProp(target, key, source[key]);
	return target;
}
function initializeContext(params) {
	let target = params?.target ?? "draft-2020-12";
	if (target === "draft-4") target = "draft-04";
	if (target === "draft-7") target = "draft-07";
	return {
		processors: params.processors ?? {},
		metadataRegistry: params?.metadata ?? globalRegistry,
		target,
		unrepresentable: params?.unrepresentable ?? "throw",
		override: params?.override ?? (() => {}),
		io: params?.io ?? "output",
		counter: 0,
		seen: /* @__PURE__ */ new Map(),
		sharedDefsExtractedFor: void 0,
		sharedEmitDoneFor: void 0,
		cycles: params?.cycles ?? "ref",
		reused: params?.reused ?? "inline",
		intersections: [],
		deferred: [],
		external: params?.external ?? void 0
	};
}
/**
* Applies the `unrepresentable` setting at a site that has no JSON Schema equivalent. Throws
* `message` unless the setting (or the handler's return value) says otherwise. Returns `true` if a
* custom JSON Schema was written into `json`, in which case the caller must not write its own.
*/
function handleUnrepresentable(schema, ctx, json, params, message) {
	const result = typeof ctx.unrepresentable === "function" ? ctx.unrepresentable({
		zodSchema: schema,
		path: params.path,
		message
	}) : ctx.unrepresentable;
	if (result === "any") return false;
	if (result === void 0 || result === "throw") throw new Error(message);
	Object.assign(json, result);
	return true;
}
function process(schema, ctx, _params = {
	path: [],
	schemaPath: []
}) {
	var _a;
	const def = schema._zod.def;
	const seen = ctx.seen.get(schema);
	if (seen) {
		seen.count++;
		if (_params.schemaPath.includes(schema)) seen.cycle = _params.path;
		return seen.schema;
	}
	const result = {
		schema: {},
		count: 1,
		cycle: void 0,
		path: _params.path
	};
	ctx.seen.set(schema, result);
	ctx.sharedDefsExtractedFor = void 0;
	ctx.sharedEmitDoneFor = void 0;
	const overrideSchema = schema._zod.toJSONSchema?.();
	if (overrideSchema) result.schema = overrideSchema;
	else {
		const params = {
			..._params,
			schemaPath: [..._params.schemaPath, schema],
			path: _params.path
		};
		if (schema._zod.processJSONSchema) schema._zod.processJSONSchema(ctx, result.schema, params);
		else {
			const _json = result.schema;
			const processor = ctx.processors[def.type];
			if (!processor) throw new Error(`[toJSONSchema]: Non-representable type encountered: ${def.type}`);
			processor(schema, ctx, _json, params);
		}
		const parent = schema._zod.parent;
		if (parent) {
			if (!result.ref) result.ref = parent;
			process(parent, ctx, params);
			ctx.seen.get(parent).isParent = true;
		}
	}
	const meta = ctx.metadataRegistry.get(schema);
	if (meta) assignProps(result.schema, meta);
	if (ctx.io === "input" && isTransforming(schema)) {
		delete result.schema.examples;
		delete result.schema.default;
	}
	if (ctx.io === "input" && "_prefault" in result.schema) (_a = result.schema).default ?? (_a.default = result.schema._prefault);
	delete result.schema._prefault;
	return ctx.seen.get(schema).schema;
}
function encodeJSONPointerSegment(segment) {
	return segment.replace(/~/g, "~0").replace(/\//g, "~1");
}
function extractDefs(ctx, schema) {
	const root = ctx.seen.get(schema);
	if (!root) throw new Error("Unprocessed schema. This is a bug in Zod.");
	if (ctx.external && ctx.sharedDefsExtractedFor === ctx.external) return;
	const idToSchema = /* @__PURE__ */ new Map();
	for (const entry of ctx.seen.entries()) {
		const id = ctx.metadataRegistry.get(entry[0])?.id;
		if (id) {
			const existing = idToSchema.get(id);
			if (existing && existing !== entry[0]) throw new Error(`Duplicate schema id "${id}" detected during JSON Schema conversion. Two different schemas cannot share the same id when converted together.`);
			idToSchema.set(id, entry[0]);
		}
	}
	const makeURI = (entry) => {
		const defsSegment = ctx.target === "draft-2020-12" ? "$defs" : "definitions";
		if (ctx.external) {
			const externalId = ctx.external.registry.get(entry[0])?.id;
			const uriGenerator = ctx.external.uri ?? ((id) => id);
			if (externalId) return { ref: uriGenerator(externalId) };
			const id = entry[1].defId ?? entry[1].schema.id ?? `schema${ctx.counter++}`;
			entry[1].defId = id;
			return {
				defId: id,
				ref: `${uriGenerator("__shared")}#/${defsSegment}/${encodeJSONPointerSegment(id)}`
			};
		}
		const uriPrefix = `#`;
		const defUriPrefix = `${uriPrefix}/${defsSegment}/`;
		if (entry[1] === root && !entry[1].schema.id) return { ref: uriPrefix };
		const defId = entry[1].schema.id ?? `__schema${ctx.counter++}`;
		return {
			defId,
			ref: defUriPrefix + encodeJSONPointerSegment(defId)
		};
	};
	const extractToDef = (entry) => {
		if (entry[1].schema.$ref) return;
		const seen = entry[1];
		const { ref, defId } = makeURI(entry);
		seen.def = { ...seen.schema };
		if (defId) seen.defId = defId;
		const schema = seen.schema;
		for (const key in schema) delete schema[key];
		schema.$ref = ref;
	};
	if (ctx.cycles === "throw") for (const entry of ctx.seen.entries()) {
		const seen = entry[1];
		if (seen.cycle) throw new Error(`Cycle detected: #/${seen.cycle?.join("/")}/<root>

Set the \`cycles\` parameter to \`"ref"\` to resolve cyclical schemas with defs.`);
	}
	for (const entry of ctx.seen.entries()) {
		const seen = entry[1];
		if (schema === entry[0]) {
			extractToDef(entry);
			continue;
		}
		if (ctx.external) {
			const ext = ctx.external.registry.get(entry[0])?.id;
			if (schema !== entry[0] && ext) {
				extractToDef(entry);
				continue;
			}
		}
		if (ctx.metadataRegistry.get(entry[0])?.id) {
			extractToDef(entry);
			continue;
		}
		if (seen.cycle) {
			extractToDef(entry);
			continue;
		}
		if (seen.count > 1) {
			if (ctx.reused === "ref") {
				extractToDef(entry);
				continue;
			}
		}
	}
	if (ctx.external) ctx.sharedDefsExtractedFor = ctx.external;
}
/** Rewrites `anyOf: [{type: "a"}, {type: "b"}]` to `type: ["a", "b"]`, which every JSON Schema draft treats as equivalent and most consumers render far better for the nullable case. Only branches that are a bare type assertion qualify — anything carrying a constraint, `$ref`, `const` or metadata is left alone. Runs after `flattenRef`, so a branch an override decorated or `$defs` extraction turned into a `$ref` is no longer bare and correctly stays in `anyOf`. `oneOf` is excluded: `integer` and `number` overlap, so "exactly one" and "at least one" are not the same there. OpenAPI 3.0 is excluded: its `type` must be a single string. */
function compactTypeUnion(schema) {
	const options = schema.anyOf;
	if (!Array.isArray(options) || options.length === 0 || schema.type !== void 0) return;
	const types = [];
	for (const option of options) {
		if (!option || typeof option !== "object") return;
		compactTypeUnion(option);
		const keys = Object.keys(option);
		if (keys.length !== 1 || keys[0] !== "type") return;
		const type = option.type;
		for (const member of Array.isArray(type) ? type : [type]) {
			if (typeof member !== "string") return;
			if (!types.includes(member)) types.push(member);
		}
	}
	delete schema.anyOf;
	schema.type = types.length === 1 ? types[0] : types;
}
/** Keywords `foldIntersection` knows how to combine. Anything else — `$ref`, `patternProperties`,
* an annotation like `description` — makes a member unfoldable, so a constraint this does not
* understand leaves the `allOf` alone instead of being silently dropped or misattributed. */
var FOLDABLE_KEYS = /* @__PURE__ */ new Set([
	"type",
	"properties",
	"required",
	"additionalProperties"
]);
var UNION_KEYS = ["oneOf", "anyOf"];
/** A member's constraint on a key it does not declare itself. A `catchall` states one; `false`, an absent `additionalProperties`, and the empty schema a loose object emits state nothing. */
function undeclaredConstraint(member) {
	const extra = member.additionalProperties;
	if (extra === void 0 || extra === false || typeof extra !== "object" || extra === null) return null;
	return Object.keys(extra).length ? extra : null;
}
/** Combines object members into the single object they describe together, or returns `null` if any of them carries a keyword outside {@link FOLDABLE_KEYS}. */
function foldObjects(members) {
	const objects = [];
	for (const member of members) {
		if (typeof member !== "object" || member.type !== "object") return null;
		for (const key in member) if (!FOLDABLE_KEYS.has(key)) return null;
		objects.push(member);
	}
	const properties = {};
	const required = /* @__PURE__ */ new Set();
	for (const object of objects) {
		for (const key in object.properties) {
			if (Object.prototype.hasOwnProperty.call(properties, key)) continue;
			const parts = [];
			for (const other of objects) {
				const part = other.properties?.[key] ?? undeclaredConstraint(other);
				if (part === null || part === void 0) continue;
				if (!parts.some((seen) => JSON.stringify(seen) === JSON.stringify(part))) parts.push(part);
			}
			assignProp(properties, key, parts.length === 1 ? parts[0] : foldObjects(parts) ?? { allOf: parts });
		}
		for (const key of object.required ?? []) required.add(key);
	}
	const folded = {
		type: "object",
		properties
	};
	if (required.size) folded.required = [...required];
	if (objects.every((object) => object.additionalProperties === false)) folded.additionalProperties = false;
	else {
		const constraints = [];
		for (const object of objects) {
			const constraint = undeclaredConstraint(object);
			if (constraint && !constraints.some((seen) => JSON.stringify(seen) === JSON.stringify(constraint))) constraints.push(constraint);
		}
		if (constraints.length === 1) folded.additionalProperties = constraints[0];
		else if (constraints.length > 1) folded.additionalProperties = { allOf: constraints };
	}
	return folded;
}
/** `additionalProperties` in an `allOf` member sees only that member's own `properties`, so two
* closed object members reject each other's keys and the schema validates nothing. Zod's parser
* pools the key sets instead — `handleIntersectionResults` reports a key as unrecognized only when
* *every* side rejects it — so the emitted schema has to pool them too, and folding the members
* into one object is the encoding that says so on every target.
*
* This runs from `finalize`, after `extractDefs`, which is what keeps it clear of the `$ref`
* machinery: a member extracted into `$defs` is already a `$ref` by now and declines to fold, so it
* keeps its reference and its own closedness rather than being inlined as a stale copy. */
function foldIntersection(json) {
	const allOf = json.allOf;
	if (!Array.isArray(allOf) || allOf.length < 2) return;
	for (const key of FOLDABLE_KEYS) if (key in json) return;
	const unions = allOf.filter((m) => UNION_KEYS.some((k) => Array.isArray(m[k])));
	let folded = null;
	if (!unions.length) folded = foldObjects(allOf);
	else {
		const union = unions[0];
		const keyword = UNION_KEYS.find((k) => Array.isArray(union[k]));
		if (Object.keys(union).length !== 1) return;
		const rest = allOf.filter((m) => m !== union);
		const branches = union[keyword].map((branch) => foldObjects([...rest, branch]));
		if (branches.some((b) => !b)) return;
		folded = { [keyword]: branches };
	}
	if (!folded) return;
	delete json.allOf;
	assignProps(json, folded);
}
function finalize(ctx, schema) {
	const root = ctx.seen.get(schema);
	if (!root) throw new Error("Unprocessed schema. This is a bug in Zod.");
	const flattenRef = (zodSchema) => {
		const seen = ctx.seen.get(zodSchema);
		if (seen.ref === null) return;
		const schema = seen.def ?? seen.schema;
		const _cached = { ...schema };
		const ref = seen.ref;
		seen.ref = null;
		if (ref) {
			flattenRef(ref);
			const refSeen = ctx.seen.get(ref);
			const refSchema = refSeen.schema;
			if (refSchema.$ref && (ctx.target === "draft-07" || ctx.target === "draft-04" || ctx.target === "openapi-3.0")) {
				schema.allOf = schema.allOf ?? [];
				schema.allOf.push(refSchema);
			} else assignProps(schema, refSchema);
			assignProps(schema, _cached);
			if (zodSchema._zod.parent === ref) for (const key in schema) {
				if (key === "$ref" || key === "allOf") continue;
				if (!(key in _cached)) delete schema[key];
			}
			if (refSchema.$ref && refSeen.def) for (const key in schema) {
				if (key === "$ref" || key === "allOf") continue;
				if (key in refSeen.def && JSON.stringify(schema[key]) === JSON.stringify(refSeen.def[key])) delete schema[key];
			}
		}
		const parent = zodSchema._zod.parent;
		if (parent && parent !== ref) {
			flattenRef(parent);
			const parentSeen = ctx.seen.get(parent);
			if (parentSeen?.schema.$ref) {
				schema.$ref = parentSeen.schema.$ref;
				if (parentSeen.def) for (const key in schema) {
					if (key === "$ref" || key === "allOf") continue;
					if (key in parentSeen.def && JSON.stringify(schema[key]) === JSON.stringify(parentSeen.def[key])) delete schema[key];
				}
			}
		}
		ctx.override({
			zodSchema,
			jsonSchema: schema,
			path: seen.path ?? []
		});
	};
	if (!ctx.external || ctx.sharedEmitDoneFor !== ctx.external) {
		for (const entry of [...ctx.seen.entries()].reverse()) flattenRef(entry[0]);
		if (ctx.target !== "openapi-3.0") for (const entry of ctx.seen.entries()) compactTypeUnion(entry[1].def ?? entry[1].schema);
		for (const rewrite of ctx.deferred) rewrite();
		if (ctx.intersections.length) {
			const carriers = /* @__PURE__ */ new Map();
			for (const seen of ctx.seen.values()) for (const json of [seen.schema, seen.def]) {
				const allOf = json?.allOf;
				if (!Array.isArray(allOf)) continue;
				const existing = carriers.get(allOf);
				if (existing) existing.push(json);
				else carriers.set(allOf, [json]);
			}
			for (const allOf of ctx.intersections) for (const json of carriers.get(allOf) ?? []) foldIntersection(json);
		}
	}
	const result = {};
	if (ctx.target === "draft-2020-12") result.$schema = "https://json-schema.org/draft/2020-12/schema";
	else if (ctx.target === "draft-07") result.$schema = "http://json-schema.org/draft-07/schema#";
	else if (ctx.target === "draft-04") result.$schema = "http://json-schema.org/draft-04/schema#";
	else if (ctx.target === "openapi-3.0") {}
	if (ctx.external?.uri) {
		const id = ctx.external.registry.get(schema)?.id;
		if (!id) throw new Error("Schema is missing an `id` property");
		result.$id = ctx.external.uri(id);
	}
	assignProps(result, root.defId ? root.schema : root.def ?? root.schema);
	const rootMetaId = ctx.metadataRegistry.get(schema)?.id;
	if (rootMetaId !== void 0 && result.id === rootMetaId) delete result.id;
	const defs = ctx.external?.defs ?? {};
	if (!ctx.external || ctx.sharedEmitDoneFor !== ctx.external) for (const entry of ctx.seen.entries()) {
		const seen = entry[1];
		if (seen.def && seen.defId) {
			if (seen.def.id === seen.defId) delete seen.def.id;
			assignProp(defs, seen.defId, seen.def);
		}
	}
	if (ctx.external) ctx.sharedEmitDoneFor = ctx.external;
	if (ctx.external) {} else if (Object.keys(defs).length > 0) {
		if (ctx.target === "draft-2020-12") result.$defs = defs;
		else result.definitions = defs;
	}
	try {
		const finalized = JSON.parse(JSON.stringify(result));
		Object.defineProperty(finalized, "~standard", {
			value: {
				...schema["~standard"],
				jsonSchema: {
					input: createStandardJSONSchemaMethod(schema, "input", ctx.processors),
					output: createStandardJSONSchemaMethod(schema, "output", ctx.processors)
				}
			},
			enumerable: false,
			writable: false
		});
		return finalized;
	} catch (_err) {
		throw new Error("Error converting schema to JSON.");
	}
}
function isTransforming(_schema, _ctx) {
	const ctx = _ctx ?? { seen: /* @__PURE__ */ new Set() };
	if (ctx.seen.has(_schema)) return false;
	ctx.seen.add(_schema);
	const def = _schema._zod.def;
	if (def.type === "transform") return true;
	if (def.type === "array") return isTransforming(def.element, ctx);
	if (def.type === "set") return isTransforming(def.valueType, ctx);
	if (def.type === "lazy") return isTransforming(def.getter(), ctx);
	if (def.type === "promise" || def.type === "optional" || def.type === "nonoptional" || def.type === "nullable" || def.type === "readonly" || def.type === "default" || def.type === "prefault" || def.type === "catch") return isTransforming(def.innerType, ctx);
	if (def.type === "intersection") return isTransforming(def.left, ctx) || isTransforming(def.right, ctx);
	if (def.type === "record" || def.type === "map") return isTransforming(def.keyType, ctx) || isTransforming(def.valueType, ctx);
	if (def.type === "pipe") {
		if (_schema._zod.traits.has("$ZodCodec")) return true;
		return isTransforming(def.in, ctx) || isTransforming(def.out, ctx);
	}
	if (def.type === "object") {
		for (const key in def.shape) if (isTransforming(def.shape[key], ctx)) return true;
		return false;
	}
	if (def.type === "union") {
		for (const option of def.options) if (isTransforming(option, ctx)) return true;
		return false;
	}
	if (def.type === "tuple") {
		for (const item of def.items) if (isTransforming(item, ctx)) return true;
		if (def.rest && isTransforming(def.rest, ctx)) return true;
		return false;
	}
	return false;
}
/**
* Creates a toJSONSchema method for a schema instance.
* This encapsulates the logic of initializing context, processing, extracting defs, and finalizing.
*/
var createToJSONSchemaMethod = (schema, processors = {}) => (params) => {
	const ctx = initializeContext({
		...params,
		processors
	});
	process(schema, ctx);
	extractDefs(ctx, schema);
	return finalize(ctx, schema);
};
var createStandardJSONSchemaMethod = (schema, io, processors = {}) => (params) => {
	const { libraryOptions, target } = params ?? {};
	const ctx = initializeContext({
		...libraryOptions ?? {},
		target,
		io,
		processors
	});
	process(schema, ctx);
	extractDefs(ctx, schema);
	return finalize(ctx, schema);
};
//#endregion
//#region node_modules/zod/v4/core/json-schema-processors.js
var formatMap = {
	guid: "uuid",
	url: "uri",
	datetime: "date-time",
	json_string: "json-string",
	regex: ""
};
var stringProcessor = (schema, ctx, _json, _params) => {
	const json = _json;
	json.type = "string";
	const { minimum, maximum, format, patterns, contentEncoding, laxFormat } = schema._zod.bag;
	if (typeof minimum === "number") json.minLength = minimum;
	if (typeof maximum === "number") json.maxLength = maximum;
	if (format) {
		json.format = formatMap[format] ?? format;
		if (json.format === "") delete json.format;
		if (format === "time" || laxFormat) delete json.format;
	}
	if (contentEncoding) json.contentEncoding = contentEncoding;
	if (patterns && patterns.size > 0) {
		const patternList = [...patterns];
		if (patternList.length === 1) json.pattern = patternList[0].source;
		else if (patternList.length > 1) json.allOf = [...patternList.map((regex) => ({
			...ctx.target === "draft-07" || ctx.target === "draft-04" || ctx.target === "openapi-3.0" ? { type: "string" } : {},
			pattern: regex.source
		}))];
	}
};
var numberProcessor = (schema, ctx, _json, params) => {
	const json = _json;
	const { minimum, maximum, format, multipleOf, exclusiveMaximum, exclusiveMinimum } = schema._zod.bag;
	if (typeof format === "string" && format.includes("int")) json.type = "integer";
	else json.type = "number";
	const exMin = typeof exclusiveMinimum === "number" && exclusiveMinimum >= (minimum ?? Number.NEGATIVE_INFINITY);
	const exMax = typeof exclusiveMaximum === "number" && exclusiveMaximum <= (maximum ?? Number.POSITIVE_INFINITY);
	const legacy = ctx.target === "draft-04" || ctx.target === "openapi-3.0";
	if (exMin) {
		if (legacy) {
			json.minimum = exclusiveMinimum;
			json.exclusiveMinimum = true;
		} else json.exclusiveMinimum = exclusiveMinimum;
	} else if (typeof minimum === "number") json.minimum = minimum;
	if (exMax) {
		if (legacy) {
			json.maximum = exclusiveMaximum;
			json.exclusiveMaximum = true;
		} else json.exclusiveMaximum = exclusiveMaximum;
	} else if (typeof maximum === "number") json.maximum = maximum;
	if (typeof multipleOf === "number") {
		if (Number.isFinite(multipleOf) && multipleOf !== 0) json.multipleOf = Math.abs(multipleOf);
		else handleUnrepresentable(schema, ctx, json, params, `A multipleOf divisor of ${multipleOf} cannot be represented in JSON Schema`);
	}
};
var booleanProcessor = (_schema, _ctx, json, _params) => {
	json.type = "boolean";
};
var neverProcessor = (_schema, _ctx, json, _params) => {
	json.not = {};
};
var enumProcessor = (schema, _ctx, json, _params) => {
	const def = schema._zod.def;
	const values = getEnumValues(def.entries);
	if (values.length === 0) {
		json.not = {};
		return;
	}
	if (values.every((v) => typeof v === "number")) json.type = "number";
	if (values.every((v) => typeof v === "string")) json.type = "string";
	json.enum = values;
};
var customProcessor = (schema, ctx, json, params) => {
	handleUnrepresentable(schema, ctx, json, params, "Custom types cannot be represented in JSON Schema");
};
var transformProcessor = (schema, ctx, json, params) => {
	handleUnrepresentable(schema, ctx, json, params, "Transforms cannot be represented in JSON Schema");
};
var arrayProcessor = (schema, ctx, _json, params) => {
	const json = _json;
	const def = schema._zod.def;
	const { minimum, maximum } = schema._zod.bag;
	if (typeof minimum === "number") json.minItems = minimum;
	if (typeof maximum === "number") json.maxItems = maximum;
	json.type = "array";
	json.items = process(def.element, ctx, {
		...params,
		path: [...params.path, "items"]
	});
};
function inputOptin(schema) {
	const def = schema._zod.def;
	if (def.type === "pipe" && def.in._zod.traits.has("$ZodTransform")) return inputOptin(def.out);
	if (def.type === "catch") return inputOptin(def.innerType);
	return schema._zod.optin;
}
var objectProcessor = (schema, ctx, _json, params) => {
	const json = _json;
	const def = schema._zod.def;
	const shape = def.shape;
	if (Object.getOwnPropertySymbols(shape).length && handleUnrepresentable(schema, ctx, json, params, "Symbol keys cannot be represented in JSON Schema")) return;
	json.type = "object";
	json.properties = {};
	for (const key in shape) assignProp(json.properties, key, process(shape[key], ctx, {
		...params,
		path: [
			...params.path,
			"properties",
			key
		]
	}));
	const allKeys = new Set(Object.keys(shape));
	const requiredKeys = new Set([...allKeys].filter((key) => {
		const field = def.shape[key];
		if (ctx.io === "input") return inputOptin(field) === void 0;
		else return field._zod.optout === void 0;
	}));
	if (requiredKeys.size > 0) json.required = Array.from(requiredKeys);
	if (def.catchall?._zod.def.type === "never") json.additionalProperties = false;
	else if (!def.catchall) {
		if (ctx.io === "output") json.additionalProperties = false;
	} else if (def.catchall) json.additionalProperties = process(def.catchall, ctx, {
		...params,
		path: [...params.path, "additionalProperties"]
	});
};
var unionProcessor = (schema, ctx, json, params) => {
	const def = schema._zod.def;
	const isExclusive = def.inclusive === false;
	const options = def.options.map((x, i) => process(x, ctx, {
		...params,
		path: [
			...params.path,
			isExclusive ? "oneOf" : "anyOf",
			i
		]
	}));
	if (isExclusive) json.oneOf = options;
	else json.anyOf = options;
};
var intersectionProcessor = (schema, ctx, json, params) => {
	const def = schema._zod.def;
	const a = process(def.left, ctx, {
		...params,
		path: [
			...params.path,
			"allOf",
			0
		]
	});
	const b = process(def.right, ctx, {
		...params,
		path: [
			...params.path,
			"allOf",
			1
		]
	});
	const isSimpleIntersection = (val) => "allOf" in val && Object.keys(val).length === 1;
	const allOf = [...isSimpleIntersection(a) ? a.allOf : [a], ...isSimpleIntersection(b) ? b.allOf : [b]];
	json.allOf = allOf;
	ctx.intersections.push(allOf);
};
var nullableProcessor = (schema, ctx, json, params) => {
	const def = schema._zod.def;
	const inner = process(def.innerType, ctx, params);
	const seen = ctx.seen.get(schema);
	if (ctx.target === "openapi-3.0") {
		seen.ref = def.innerType;
		json.nullable = true;
	} else json.anyOf = [inner, { type: "null" }];
};
var nonoptionalProcessor = (schema, ctx, _json, params) => {
	const def = schema._zod.def;
	process(def.innerType, ctx, params);
	const seen = ctx.seen.get(schema);
	seen.ref = def.innerType;
};
/** Round-trips a default value through JSON so the emitted schema is guaranteed to be valid JSON.
* A BigInt has no reliable encoding, so it goes through `unrepresentable` like any other
* unrepresentable value. Returns a sentinel when the caller must not write a default of its own. */
var UNREPRESENTABLE_DEFAULT = Symbol();
function serializeDefaultValue(value, schema, ctx, json, params) {
	let unrepresentable = false;
	const serialized = JSON.stringify(value, (_, val) => {
		if (typeof val !== "bigint") return val;
		unrepresentable = true;
		return null;
	});
	if (!unrepresentable) return JSON.parse(serialized);
	handleUnrepresentable(schema, ctx, json, params, "BigInt defaults cannot be represented in JSON Schema");
	return UNREPRESENTABLE_DEFAULT;
}
var defaultProcessor = (schema, ctx, json, params) => {
	const def = schema._zod.def;
	process(def.innerType, ctx, params);
	const seen = ctx.seen.get(schema);
	seen.ref = def.innerType;
	const value = serializeDefaultValue(def.defaultValue, schema, ctx, json, params);
	if (value !== UNREPRESENTABLE_DEFAULT) json.default = value;
};
var prefaultProcessor = (schema, ctx, json, params) => {
	const def = schema._zod.def;
	process(def.innerType, ctx, params);
	const seen = ctx.seen.get(schema);
	seen.ref = def.innerType;
	if (ctx.io !== "input") return;
	const value = serializeDefaultValue(def.defaultValue, schema, ctx, json, params);
	if (value !== UNREPRESENTABLE_DEFAULT) json._prefault = value;
};
var catchProcessor = (schema, ctx, json, params) => {
	const def = schema._zod.def;
	process(def.innerType, ctx, params);
	const seen = ctx.seen.get(schema);
	seen.ref = def.innerType;
	let catchValue;
	try {
		catchValue = def.catchValue(void 0);
	} catch {
		handleUnrepresentable(schema, ctx, json, params, "Dynamic catch values are not supported in JSON Schema");
		return;
	}
	json.default = catchValue;
};
var pipeProcessor = (schema, ctx, _json, params) => {
	const def = schema._zod.def;
	const inIsTransform = def.in._zod.traits.has("$ZodTransform");
	const innerType = ctx.io === "input" ? inIsTransform ? def.out : def.in : def.out;
	process(innerType, ctx, params);
	const seen = ctx.seen.get(schema);
	seen.ref = innerType;
};
var readonlyProcessor = (schema, ctx, json, params) => {
	const def = schema._zod.def;
	process(def.innerType, ctx, params);
	const seen = ctx.seen.get(schema);
	seen.ref = def.innerType;
	json.readOnly = true;
};
var optionalProcessor = (schema, ctx, _json, params) => {
	const def = schema._zod.def;
	process(def.innerType, ctx, params);
	const seen = ctx.seen.get(schema);
	seen.ref = def.innerType;
};
//#endregion
//#region node_modules/zod/v4/classic/errors.js
var _installedErrorProtos = /* @__PURE__ */ new WeakSet([Object.prototype, Error.prototype]);
function _lazyMethod(proto, key, make) {
	Object.defineProperty(proto, key, {
		configurable: true,
		enumerable: false,
		get() {
			const value = make(this);
			Object.defineProperty(this, key, {
				value,
				configurable: true,
				writable: true
			});
			return value;
		},
		set(value) {
			Object.defineProperty(this, key, {
				value,
				configurable: true,
				writable: true
			});
		}
	});
}
var initializer = (inst, issues) => {
	$ZodError.init(inst, issues);
	inst.name = "ZodError";
	const proto = Object.getPrototypeOf(inst);
	if (_installedErrorProtos.has(proto)) return;
	_installedErrorProtos.add(proto);
	_lazyMethod(proto, "format", (self) => (mapper) => formatError(self, mapper));
	_lazyMethod(proto, "flatten", (self) => (mapper) => flattenError(self, mapper));
	_lazyMethod(proto, "addIssue", (self) => (issue) => {
		self.issues.push(issue);
		self.message = JSON.stringify(self.issues, jsonStringifyReplacer, 2);
	});
	_lazyMethod(proto, "addIssues", (self) => (issues) => {
		self.issues.push(...issues);
		self.message = JSON.stringify(self.issues, jsonStringifyReplacer, 2);
	});
	Object.defineProperty(proto, "isEmpty", {
		configurable: true,
		enumerable: false,
		get() {
			return this.issues.length === 0;
		}
	});
};
var ZodRealError = /*@__PURE__*/ $constructor("ZodError", initializer, void 0, { Parent: Error });
//#endregion
//#region node_modules/zod/v4/classic/parse.js
var parse = /* @__PURE__ */ _parse(ZodRealError);
var parseAsync = /* @__PURE__ */ _parseAsync(ZodRealError);
var safeParse = /* @__PURE__ */ _safeParse(ZodRealError);
var safeParseAsync = /* @__PURE__ */ _safeParseAsync(ZodRealError);
var encode = /* @__PURE__ */ _encode(ZodRealError);
var decode = /* @__PURE__ */ _decode(ZodRealError);
var encodeAsync = /* @__PURE__ */ _encodeAsync(ZodRealError);
var decodeAsync = /* @__PURE__ */ _decodeAsync(ZodRealError);
var safeEncode = /* @__PURE__ */ _safeEncode(ZodRealError);
var safeDecode = /* @__PURE__ */ _safeDecode(ZodRealError);
var safeEncodeAsync = /* @__PURE__ */ _safeEncodeAsync(ZodRealError);
var safeDecodeAsync = /* @__PURE__ */ _safeDecodeAsync(ZodRealError);
//#endregion
//#region node_modules/zod/v4/classic/schemas.js
function _ensureDefaultLocale() {
	if (!globalConfig.localeError) config(en_default());
}
function _ensureDefaultMemoizer() {
	if (!globalConfig.memoizer) config({ memoizer: memoizer() });
}
var ZodType = /*@__PURE__*/ $constructor("ZodType", (inst, def) => {
	_ensureDefaultLocale();
	$ZodType.init(inst, def);
	inst.def = def;
	inst.type = def.type;
	return inst;
}, {
	check(...chks) {
		const def = this.def;
		return this.clone(mergeDefs(def, { checks: [...def.checks ?? [], ...chks.map((ch) => typeof ch === "function" ? { _zod: {
			check: ch,
			def: { check: "custom" },
			onattach: []
		} } : ch)] }), { parent: true });
	},
	with(...chks) {
		return this.check(...chks);
	},
	clone(def, params) {
		return clone(this, def, params);
	},
	brand() {
		return this;
	},
	register(reg, meta) {
		reg.add(this, meta);
		return this;
	},
	refine(check, params) {
		return this.check(refine(check, params));
	},
	superRefine(refinement, params) {
		return this.check(superRefine(refinement, params));
	},
	overwrite(fn) {
		return this.check(/* @__PURE__ */ _overwrite(fn));
	},
	optional() {
		return optional(this);
	},
	exactOptional() {
		return exactOptional(this);
	},
	nullable() {
		return nullable(this);
	},
	nullish() {
		return optional(nullable(this));
	},
	nonoptional(params) {
		return nonoptional(this, params);
	},
	array() {
		return array(this);
	},
	or(arg) {
		return union([this, arg]);
	},
	and(arg) {
		return intersection(this, arg);
	},
	transform(tx) {
		return pipe(this, transform(tx));
	},
	default(d) {
		return _default(this, d);
	},
	prefault(d) {
		return prefault(this, d);
	},
	catch(params) {
		return _catch(this, params);
	},
	pipe(target) {
		return pipe(this, target);
	},
	readonly() {
		return readonly(this);
	},
	describe(description) {
		const cl = this.clone();
		globalRegistry.add(cl, { description });
		return cl;
	},
	meta(...args) {
		if (args.length === 0) return globalRegistry.get(this);
		const cl = this.clone();
		globalRegistry.add(cl, args[0]);
		return cl;
	},
	isOptional() {
		return this.safeParse(void 0).success;
	},
	isNullable() {
		return this.safeParse(null).success;
	},
	apply(fn, ...args) {
		return args.length === 0 ? fn(this) : fn(this, ...args);
	},
	get "~standard"() {
		return hide(this, "~standard", {
			...standardProps(this),
			jsonSchema: {
				input: createStandardJSONSchemaMethod(this, "input"),
				output: createStandardJSONSchemaMethod(this, "output")
			}
		});
	},
	set "~standard"(value) {
		own(this, "~standard", value);
	},
	parse: function _parse(data, params) {
		return parse(this, data, params, { callee: _parse });
	},
	parseAsync: async function _parseAsync(data, params) {
		return await parseAsync(this, data, params, { callee: _parseAsync });
	},
	safeParse(data, params) {
		return safeParse(this, data, params);
	},
	async safeParseAsync(data, params) {
		return safeParseAsync(this, data, params);
	},
	get spa() {
		return this?.safeParseAsync;
	},
	set spa(value) {
		own(this, "spa", value);
	},
	encode: function _encode(data, params) {
		return encode(this, data, params, { callee: _encode });
	},
	decode: function _decode(data, params) {
		return decode(this, data, params, { callee: _decode });
	},
	encodeAsync: async function _encodeAsync(data, params) {
		return await encodeAsync(this, data, params, { callee: _encodeAsync });
	},
	decodeAsync: async function _decodeAsync(data, params) {
		return await decodeAsync(this, data, params, { callee: _decodeAsync });
	},
	safeEncode(data, params) {
		return safeEncode(this, data, params);
	},
	safeDecode(data, params) {
		return safeDecode(this, data, params);
	},
	async safeEncodeAsync(data, params) {
		return safeEncodeAsync(this, data, params);
	},
	async safeDecodeAsync(data, params) {
		return safeDecodeAsync(this, data, params);
	},
	toJSONSchema(params) {
		return createToJSONSchemaMethod(this, {})(params);
	},
	get description() {
		return globalRegistry.get(this)?.description;
	},
	get _def() {
		return this._zod.def;
	}
});
/** @internal */
var _ZodString = /*@__PURE__*/ $constructor("_ZodString", (inst, def) => {
	$ZodString.init(inst, def);
	ZodType.init(inst, def);
	inst._zod.processJSONSchema = (ctx, json, params) => stringProcessor(inst, ctx, json, params);
	const bag = inst._zod.bag;
	inst.format = bag.format ?? null;
	inst.minLength = bag.minimum ?? null;
	inst.maxLength = bag.maximum ?? null;
}, {
	regex(...args) {
		return this.check(/* @__PURE__ */ _regex(...args));
	},
	includes(...args) {
		return this.check(/* @__PURE__ */ _includes(...args));
	},
	startsWith(...args) {
		return this.check(/* @__PURE__ */ _startsWith(...args));
	},
	endsWith(...args) {
		return this.check(/* @__PURE__ */ _endsWith(...args));
	},
	min(...args) {
		return this.check(/* @__PURE__ */ _minLength(...args));
	},
	max(...args) {
		return this.check(/* @__PURE__ */ _maxLength(...args));
	},
	length(...args) {
		return this.check(/* @__PURE__ */ _length(...args));
	},
	nonempty(...args) {
		return this.check(/* @__PURE__ */ _minLength(1, ...args));
	},
	lowercase(params) {
		return this.check(/* @__PURE__ */ _lowercase(params));
	},
	uppercase(params) {
		return this.check(/* @__PURE__ */ _uppercase(params));
	},
	trim() {
		return this.check(/* @__PURE__ */ _trim());
	},
	normalize(...args) {
		return this.check(/* @__PURE__ */ _normalize(...args));
	},
	toLowerCase() {
		return this.check(/* @__PURE__ */ _toLowerCase());
	},
	toUpperCase() {
		return this.check(/* @__PURE__ */ _toUpperCase());
	},
	slugify() {
		return this.check(/* @__PURE__ */ _slugify());
	}
});
var ZodString = /*@__PURE__*/ $constructor("ZodString", (inst, def) => {
	$ZodString.init(inst, def);
	_ZodString.init(inst, def);
}, {
	email(params) {
		return this.check(/* @__PURE__ */ _email(ZodEmail, params));
	},
	url(params) {
		return this.check(/* @__PURE__ */ _url(ZodURL, params));
	},
	jwt(params) {
		return this.check(/* @__PURE__ */ _jwt(ZodJWT, params));
	},
	emoji(params) {
		return this.check(/* @__PURE__ */ _emoji(ZodEmoji, params));
	},
	guid(params) {
		return this.check(/* @__PURE__ */ _guid(ZodGUID, params));
	},
	uuid(params) {
		return this.check(/* @__PURE__ */ _uuid(ZodUUID, params));
	},
	uuidv4(params) {
		return this.check(/* @__PURE__ */ _uuidv4(ZodUUID, params));
	},
	uuidv6(params) {
		return this.check(/* @__PURE__ */ _uuidv6(ZodUUID, params));
	},
	uuidv7(params) {
		return this.check(/* @__PURE__ */ _uuidv7(ZodUUID, params));
	},
	nanoid(params) {
		return this.check(/* @__PURE__ */ _nanoid(ZodNanoID, params));
	},
	cuid(params) {
		return this.check(/* @__PURE__ */ _cuid(ZodCUID, params));
	},
	cuid2(params) {
		return this.check(/* @__PURE__ */ _cuid2(ZodCUID2, params));
	},
	ulid(params) {
		return this.check(/* @__PURE__ */ _ulid(ZodULID, params));
	},
	base64(params) {
		return this.check(/* @__PURE__ */ _base64(ZodBase64, params));
	},
	base64url(params) {
		return this.check(/* @__PURE__ */ _base64url(ZodBase64URL, params));
	},
	xid(params) {
		return this.check(/* @__PURE__ */ _xid(ZodXID, params));
	},
	ksuid(params) {
		return this.check(/* @__PURE__ */ _ksuid(ZodKSUID, params));
	},
	ipv4(params) {
		return this.check(/* @__PURE__ */ _ipv4(ZodIPv4, params));
	},
	ipv6(params) {
		return this.check(/* @__PURE__ */ _ipv6(ZodIPv6, params));
	},
	cidrv4(params) {
		return this.check(/* @__PURE__ */ _cidrv4(ZodCIDRv4, params));
	},
	cidrv6(params) {
		return this.check(/* @__PURE__ */ _cidrv6(ZodCIDRv6, params));
	},
	e164(params) {
		return this.check(/* @__PURE__ */ _e164(ZodE164, params));
	},
	datetime(params) {
		return this.check(/* @__PURE__ */ _isoDateTime(ZodISODateTime, params));
	},
	date(params) {
		return this.check(/* @__PURE__ */ _isoDate(ZodISODate, params));
	},
	time(params) {
		return this.check(/* @__PURE__ */ _isoTime(ZodISOTime, params));
	},
	duration(params) {
		return this.check(/* @__PURE__ */ _isoDuration(ZodISODuration, params));
	}
});
function string(params) {
	return /* @__PURE__ */ _string(ZodString, params);
}
var ZodStringFormat = /*@__PURE__*/ $constructor("ZodStringFormat", (inst, def) => {
	$ZodStringFormat.init(inst, def);
	_ZodString.init(inst, def);
});
var ZodISODateTime = /*@__PURE__*/ $constructor("ZodISODateTime", (inst, def) => {
	$ZodISODateTime.init(inst, def);
	ZodStringFormat.init(inst, def);
});
var ZodISODate = /*@__PURE__*/ $constructor("ZodISODate", (inst, def) => {
	$ZodISODate.init(inst, def);
	ZodStringFormat.init(inst, def);
});
var ZodISOTime = /*@__PURE__*/ $constructor("ZodISOTime", (inst, def) => {
	$ZodISOTime.init(inst, def);
	ZodStringFormat.init(inst, def);
});
var ZodISODuration = /*@__PURE__*/ $constructor("ZodISODuration", (inst, def) => {
	$ZodISODuration.init(inst, def);
	ZodStringFormat.init(inst, def);
});
var ZodEmail = /*@__PURE__*/ $constructor("ZodEmail", (inst, def) => {
	$ZodEmail.init(inst, def);
	ZodStringFormat.init(inst, def);
});
var ZodGUID = /*@__PURE__*/ $constructor("ZodGUID", (inst, def) => {
	$ZodGUID.init(inst, def);
	ZodStringFormat.init(inst, def);
});
var ZodUUID = /*@__PURE__*/ $constructor("ZodUUID", (inst, def) => {
	$ZodUUID.init(inst, def);
	ZodStringFormat.init(inst, def);
});
var ZodURL = /*@__PURE__*/ $constructor("ZodURL", (inst, def) => {
	$ZodURL.init(inst, def);
	ZodStringFormat.init(inst, def);
});
var ZodEmoji = /*@__PURE__*/ $constructor("ZodEmoji", (inst, def) => {
	$ZodEmoji.init(inst, def);
	ZodStringFormat.init(inst, def);
});
var ZodNanoID = /*@__PURE__*/ $constructor("ZodNanoID", (inst, def) => {
	$ZodNanoID.init(inst, def);
	ZodStringFormat.init(inst, def);
});
/**
* @deprecated CUID v1 is deprecated by its authors due to information leakage
* (timestamps embedded in the id). Use {@link ZodCUID2} instead.
* See https://github.com/paralleldrive/cuid.
*/
var ZodCUID = /*@__PURE__*/ $constructor("ZodCUID", (inst, def) => {
	$ZodCUID.init(inst, def);
	ZodStringFormat.init(inst, def);
});
var ZodCUID2 = /*@__PURE__*/ $constructor("ZodCUID2", (inst, def) => {
	$ZodCUID2.init(inst, def);
	ZodStringFormat.init(inst, def);
});
var ZodULID = /*@__PURE__*/ $constructor("ZodULID", (inst, def) => {
	$ZodULID.init(inst, def);
	ZodStringFormat.init(inst, def);
});
var ZodXID = /*@__PURE__*/ $constructor("ZodXID", (inst, def) => {
	$ZodXID.init(inst, def);
	ZodStringFormat.init(inst, def);
});
var ZodKSUID = /*@__PURE__*/ $constructor("ZodKSUID", (inst, def) => {
	$ZodKSUID.init(inst, def);
	ZodStringFormat.init(inst, def);
});
var ZodIPv4 = /*@__PURE__*/ $constructor("ZodIPv4", (inst, def) => {
	$ZodIPv4.init(inst, def);
	ZodStringFormat.init(inst, def);
});
var ZodIPv6 = /*@__PURE__*/ $constructor("ZodIPv6", (inst, def) => {
	$ZodIPv6.init(inst, def);
	ZodStringFormat.init(inst, def);
});
var ZodCIDRv4 = /*@__PURE__*/ $constructor("ZodCIDRv4", (inst, def) => {
	$ZodCIDRv4.init(inst, def);
	ZodStringFormat.init(inst, def);
});
var ZodCIDRv6 = /*@__PURE__*/ $constructor("ZodCIDRv6", (inst, def) => {
	$ZodCIDRv6.init(inst, def);
	ZodStringFormat.init(inst, def);
});
var ZodBase64 = /*@__PURE__*/ $constructor("ZodBase64", (inst, def) => {
	$ZodBase64.init(inst, def);
	ZodStringFormat.init(inst, def);
});
var ZodBase64URL = /*@__PURE__*/ $constructor("ZodBase64URL", (inst, def) => {
	$ZodBase64URL.init(inst, def);
	ZodStringFormat.init(inst, def);
});
var ZodE164 = /*@__PURE__*/ $constructor("ZodE164", (inst, def) => {
	$ZodE164.init(inst, def);
	ZodStringFormat.init(inst, def);
});
var ZodJWT = /*@__PURE__*/ $constructor("ZodJWT", (inst, def) => {
	$ZodJWT.init(inst, def);
	ZodStringFormat.init(inst, def);
});
var ZodNumber = /*@__PURE__*/ $constructor("ZodNumber", (inst, def) => {
	$ZodNumber.init(inst, def);
	ZodType.init(inst, def);
	inst._zod.processJSONSchema = (ctx, json, params) => numberProcessor(inst, ctx, json, params);
	const bag = inst._zod.bag;
	inst.minValue = Math.max(bag.minimum ?? Number.NEGATIVE_INFINITY, bag.exclusiveMinimum ?? Number.NEGATIVE_INFINITY) ?? null;
	inst.maxValue = Math.min(bag.maximum ?? Number.POSITIVE_INFINITY, bag.exclusiveMaximum ?? Number.POSITIVE_INFINITY) ?? null;
	inst.isInt = (bag.format ?? "").includes("int") || Number.isSafeInteger(bag.multipleOf ?? .5);
	inst.isFinite = true;
	inst.format = bag.format ?? null;
}, {
	gt(value, params) {
		return this.check(/* @__PURE__ */ _gt(value, params));
	},
	gte(value, params) {
		return this.check(/* @__PURE__ */ _gte(value, params));
	},
	min(value, params) {
		return this.check(/* @__PURE__ */ _gte(value, params));
	},
	lt(value, params) {
		return this.check(/* @__PURE__ */ _lt(value, params));
	},
	lte(value, params) {
		return this.check(/* @__PURE__ */ _lte(value, params));
	},
	max(value, params) {
		return this.check(/* @__PURE__ */ _lte(value, params));
	},
	int(params) {
		return this.check(int(params));
	},
	safe(params) {
		return this.check(int(params));
	},
	positive(params) {
		return this.check(/* @__PURE__ */ _gt(0, params));
	},
	nonnegative(params) {
		return this.check(/* @__PURE__ */ _gte(0, params));
	},
	negative(params) {
		return this.check(/* @__PURE__ */ _lt(0, params));
	},
	nonpositive(params) {
		return this.check(/* @__PURE__ */ _lte(0, params));
	},
	multipleOf(value, params) {
		return this.check(/* @__PURE__ */ _multipleOf(value, params));
	},
	step(value, params) {
		return this.check(/* @__PURE__ */ _multipleOf(value, params));
	},
	finite() {
		return this;
	}
});
function number(params) {
	return /* @__PURE__ */ _number(ZodNumber, params);
}
var ZodNumberFormat = /*@__PURE__*/ $constructor("ZodNumberFormat", (inst, def) => {
	$ZodNumberFormat.init(inst, def);
	ZodNumber.init(inst, def);
});
function int(params) {
	return /* @__PURE__ */ _int(ZodNumberFormat, params);
}
function float32(params) {
	return /* @__PURE__ */ _float32(ZodNumberFormat, params);
}
var ZodBoolean = /*@__PURE__*/ $constructor("ZodBoolean", (inst, def) => {
	$ZodBoolean.init(inst, def);
	ZodType.init(inst, def);
	inst._zod.processJSONSchema = (ctx, json, params) => booleanProcessor(inst, ctx, json, params);
});
function boolean(params) {
	return /* @__PURE__ */ _boolean(ZodBoolean, params);
}
var ZodUnknown = /*@__PURE__*/ $constructor("ZodUnknown", (inst, def) => {
	$ZodUnknown.init(inst, def);
	ZodType.init(inst, def);
	inst._zod.processJSONSchema = (ctx, json, params) => void 0;
});
function unknown() {
	return /* @__PURE__ */ _unknown(ZodUnknown);
}
var ZodNever = /*@__PURE__*/ $constructor("ZodNever", (inst, def) => {
	$ZodNever.init(inst, def);
	ZodType.init(inst, def);
	inst._zod.processJSONSchema = (ctx, json, params) => neverProcessor(inst, ctx, json, params);
});
function never(params) {
	return /* @__PURE__ */ _never(ZodNever, params);
}
var ZodArray = /*@__PURE__*/ $constructor("ZodArray", (inst, def) => {
	_ensureDefaultMemoizer();
	$ZodArray.init(inst, def);
	ZodType.init(inst, def);
	inst._zod.processJSONSchema = (ctx, json, params) => arrayProcessor(inst, ctx, json, params);
	inst.element = def.element;
}, {
	min(n, params) {
		return this.check(/* @__PURE__ */ _minLength(n, params));
	},
	nonempty(params) {
		return this.check(/* @__PURE__ */ _minLength(1, params));
	},
	max(n, params) {
		return this.check(/* @__PURE__ */ _maxLength(n, params));
	},
	length(n, params) {
		return this.check(/* @__PURE__ */ _length(n, params));
	},
	unwrap() {
		return this.element;
	}
});
function array(element, params) {
	return /* @__PURE__ */ _array(ZodArray, element, params);
}
var ZodObject = /*@__PURE__*/ $constructor("ZodObject", (inst, def) => {
	_ensureDefaultMemoizer();
	$ZodObjectJIT.init(inst, def);
	ZodType.init(inst, def);
	inst._zod.processJSONSchema = (ctx, json, params) => objectProcessor(inst, ctx, json, params);
	installLazyProp(inst, "shape", (self) => self._zod.def.shape, false);
}, {
	keyof() {
		return _enum(Object.keys(this._zod.def.shape));
	},
	catchall(catchall) {
		return this.clone({
			...this._zod.def,
			catchall
		});
	},
	passthrough() {
		return this.clone({
			...this._zod.def,
			catchall: unknown()
		});
	},
	loose() {
		return this.clone({
			...this._zod.def,
			catchall: unknown()
		});
	},
	strict() {
		return this.clone({
			...this._zod.def,
			catchall: never()
		});
	},
	strip() {
		return this.clone({
			...this._zod.def,
			catchall: void 0
		});
	},
	extend(incoming) {
		return extend(this, incoming);
	},
	safeExtend(incoming) {
		return safeExtend(this, incoming);
	},
	merge(other) {
		return merge(this, other);
	},
	pick(mask) {
		return pick(this, mask);
	},
	omit(mask) {
		return omit(this, mask);
	},
	partial(...args) {
		return partial(ZodOptional, this, args[0]);
	},
	exactPartial(...args) {
		return partial(ZodExactOptional, this, args[0], "exactPartial");
	},
	required(...args) {
		return required(ZodNonOptional, this, args[0]);
	}
});
function object(shape, params) {
	return new ZodObject({
		type: "object",
		shape: shape ?? {},
		...normalizeParams(params)
	});
}
var ZodUnion = /*@__PURE__*/ $constructor("ZodUnion", (inst, def) => {
	$ZodUnion.init(inst, def);
	ZodType.init(inst, def);
	inst._zod.processJSONSchema = (ctx, json, params) => unionProcessor(inst, ctx, json, params);
	inst.options = def.options;
});
function union(options, params) {
	return new ZodUnion({
		type: "union",
		options,
		...normalizeParams(params)
	});
}
var ZodIntersection = /*@__PURE__*/ $constructor("ZodIntersection", (inst, def) => {
	$ZodIntersection.init(inst, def);
	ZodType.init(inst, def);
	inst._zod.processJSONSchema = (ctx, json, params) => intersectionProcessor(inst, ctx, json, params);
});
function intersection(left, right) {
	return new ZodIntersection({
		type: "intersection",
		left,
		right
	});
}
var ZodEnum = /*@__PURE__*/ $constructor("ZodEnum", (inst, def) => {
	$ZodEnum.init(inst, def);
	ZodType.init(inst, def);
	inst._zod.processJSONSchema = (ctx, json, params) => enumProcessor(inst, ctx, json, params);
	inst.enum = def.entries;
	inst.options = Object.values(def.entries);
	const keys = new Set(Object.keys(def.entries));
	inst.extract = (values, params) => {
		const newEntries = {};
		for (const value of values) if (keys.has(value)) newEntries[value] = def.entries[value];
		else throw new Error(`Key ${value} not found in enum`);
		return new ZodEnum({
			...def,
			checks: [],
			...normalizeParams(params),
			entries: newEntries
		});
	};
	inst.exclude = (values, params) => {
		const newEntries = { ...def.entries };
		for (const value of values) if (keys.has(value)) delete newEntries[value];
		else throw new Error(`Key ${value} not found in enum`);
		return new ZodEnum({
			...def,
			checks: [],
			...normalizeParams(params),
			entries: newEntries
		});
	};
});
function _enum(values, params) {
	return new ZodEnum({
		type: "enum",
		entries: Array.isArray(values) ? Object.fromEntries(values.map((v) => [v, v])) : values,
		...normalizeParams(params)
	});
}
var ZodTransform = /*@__PURE__*/ $constructor("ZodTransform", (inst, def) => {
	_ensureDefaultMemoizer();
	$ZodTransform.init(inst, def);
	ZodType.init(inst, def);
	inst._zod.processJSONSchema = (ctx, json, params) => transformProcessor(inst, ctx, json, params);
	inst._zod.parse = (payload, _ctx) => {
		if (_ctx.direction === "backward") throw new $ZodEncodeError(inst.constructor.name);
		payload.addIssue = (issue$1) => {
			if (typeof issue$1 === "string") payload.issues.push(issue(issue$1, payload.value, def));
			else {
				const _issue = issue$1;
				if (_issue.fatal) _issue.continue = false;
				_issue.code ?? (_issue.code = "custom");
				if (!("input" in _issue)) _issue.input = payload.value;
				_issue.inst ?? (_issue.inst = inst);
				payload.issues.push(issue(_issue));
			}
		};
		const output = def.transform(payload.value, payload);
		if (output instanceof Promise) return output.then((output) => {
			payload.value = output;
			return payload;
		});
		payload.value = output;
		return payload;
	};
});
function transform(fn) {
	return new ZodTransform({
		type: "transform",
		transform: fn
	});
}
var ZodOptional = /*@__PURE__*/ $constructor("ZodOptional", (inst, def) => {
	$ZodOptional.init(inst, def);
	ZodType.init(inst, def);
	inst._zod.processJSONSchema = (ctx, json, params) => optionalProcessor(inst, ctx, json, params);
	inst.unwrap = () => inst._zod.def.innerType;
});
function optional(innerType) {
	return new ZodOptional({
		type: "optional",
		innerType
	});
}
var ZodExactOptional = /*@__PURE__*/ $constructor("ZodExactOptional", (inst, def) => {
	$ZodExactOptional.init(inst, def);
	ZodType.init(inst, def);
	inst._zod.processJSONSchema = (ctx, json, params) => optionalProcessor(inst, ctx, json, params);
	inst.unwrap = () => inst._zod.def.innerType;
});
function exactOptional(innerType) {
	return new ZodExactOptional({
		type: "optional",
		innerType
	});
}
var ZodNullable = /*@__PURE__*/ $constructor("ZodNullable", (inst, def) => {
	$ZodNullable.init(inst, def);
	ZodType.init(inst, def);
	inst._zod.processJSONSchema = (ctx, json, params) => nullableProcessor(inst, ctx, json, params);
	inst.unwrap = () => inst._zod.def.innerType;
});
function nullable(innerType) {
	return new ZodNullable({
		type: "nullable",
		innerType
	});
}
var ZodDefault = /*@__PURE__*/ $constructor("ZodDefault", (inst, def) => {
	$ZodDefault.init(inst, def);
	ZodType.init(inst, def);
	inst._zod.processJSONSchema = (ctx, json, params) => defaultProcessor(inst, ctx, json, params);
	inst.unwrap = () => inst._zod.def.innerType;
	inst.removeDefault = inst.unwrap;
});
function _default(innerType, defaultValue) {
	return new ZodDefault({
		type: "default",
		innerType,
		get defaultValue() {
			return typeof defaultValue === "function" ? defaultValue() : shallowClone(defaultValue);
		}
	});
}
var ZodPrefault = /*@__PURE__*/ $constructor("ZodPrefault", (inst, def) => {
	$ZodPrefault.init(inst, def);
	ZodType.init(inst, def);
	inst._zod.processJSONSchema = (ctx, json, params) => prefaultProcessor(inst, ctx, json, params);
	inst.unwrap = () => inst._zod.def.innerType;
});
function prefault(innerType, defaultValue) {
	return new ZodPrefault({
		type: "prefault",
		innerType,
		get defaultValue() {
			return typeof defaultValue === "function" ? defaultValue() : shallowClone(defaultValue);
		}
	});
}
var ZodNonOptional = /*@__PURE__*/ $constructor("ZodNonOptional", (inst, def) => {
	$ZodNonOptional.init(inst, def);
	ZodType.init(inst, def);
	inst._zod.processJSONSchema = (ctx, json, params) => nonoptionalProcessor(inst, ctx, json, params);
	inst.unwrap = () => inst._zod.def.innerType;
});
function nonoptional(innerType, params) {
	return new ZodNonOptional({
		type: "nonoptional",
		innerType,
		...normalizeParams(params)
	});
}
var ZodCatch = /*@__PURE__*/ $constructor("ZodCatch", (inst, def) => {
	$ZodCatch.init(inst, def);
	ZodType.init(inst, def);
	inst._zod.processJSONSchema = (ctx, json, params) => catchProcessor(inst, ctx, json, params);
	inst.unwrap = () => inst._zod.def.innerType;
	inst.removeCatch = inst.unwrap;
});
function _catch(innerType, catchValue) {
	return new ZodCatch({
		type: "catch",
		innerType,
		catchValue: typeof catchValue === "function" ? catchValue : constantCatch(catchValue)
	});
}
var ZodPipe = /*@__PURE__*/ $constructor("ZodPipe", (inst, def) => {
	$ZodPipe.init(inst, def);
	ZodType.init(inst, def);
	inst._zod.processJSONSchema = (ctx, json, params) => pipeProcessor(inst, ctx, json, params);
	inst.in = def.in;
	inst.out = def.out;
});
function pipe(in_, out) {
	return new ZodPipe({
		type: "pipe",
		in: in_,
		out
	});
}
var ZodReadonly = /*@__PURE__*/ $constructor("ZodReadonly", (inst, def) => {
	$ZodReadonly.init(inst, def);
	ZodType.init(inst, def);
	inst._zod.processJSONSchema = (ctx, json, params) => readonlyProcessor(inst, ctx, json, params);
	inst.unwrap = () => inst._zod.def.innerType;
});
function readonly(innerType) {
	return new ZodReadonly({
		type: "readonly",
		innerType
	});
}
var ZodCustom = /*@__PURE__*/ $constructor("ZodCustom", (inst, def) => {
	$ZodCustom.init(inst, def);
	ZodType.init(inst, def);
	inst._zod.processJSONSchema = (ctx, json, params) => customProcessor(inst, ctx, json, params);
});
function refine(fn, _params = {}) {
	return /* @__PURE__ */ _refine(ZodCustom, fn, _params);
}
function superRefine(fn, params) {
	return /* @__PURE__ */ _superRefine(fn, params);
}
//#endregion
//#region node_modules/@hookform/resolvers/dist/resolvers.mjs
var t$1 = (r, t, s) => {
	if (r && "reportValidity" in r) {
		const o = get(s, t);
		r.setCustomValidity(o && o.message || ""), r.reportValidity();
	}
};
var s$1 = (e, r) => {
	for (const s in r.fields) {
		const o = r.fields[s];
		o && o.ref && "reportValidity" in o.ref ? t$1(o.ref, s, e) : o && o.refs && o.refs.forEach((r) => t$1(r, s, e));
	}
};
var o$1 = (t, o) => {
	o.shouldUseNativeValidation && s$1(t, o);
	const n = {};
	for (const s in t) {
		const f = get(o.fields, s), c = Object.assign(t[s] || {}, { ref: f && f.refs ? f.refs[0] : f && f.ref });
		if (i$1(o.names || Object.keys(t), s)) {
			const t = Object.assign({}, get(n, s));
			set(t, "root", c), set(n, s, t);
		} else set(n, s, c);
	}
	return n;
};
var i$1 = (e, r) => {
	const t = n(r).replace(/[.*+?^${}()|\\]/g, "\\$&");
	return e.some((e) => n(e).match(`^${t}\\.\\d+`));
};
function n(e) {
	return e.replace(/\[(\d+)]/g, ".$1").replace(/[[\]]/g, "");
}
//#endregion
//#region node_modules/@hookform/resolvers/zod/dist/zod.mjs
function o() {
	return o = Object.assign ? Object.assign.bind() : function(r) {
		for (var e = 1; e < arguments.length; e++) {
			var n = arguments[e];
			for (var o in n) ({}).hasOwnProperty.call(n, o) && (r[o] = n[o]);
		}
		return r;
	}, o.apply(null, arguments);
}
function t(r, e) {
	try {
		var n = r();
	} catch (r) {
		return e(r);
	}
	return n && n.then ? n.then(void 0, e) : n;
}
function s(r, e) {
	for (var o = Object.create(null); r.length;) {
		var t = r[0], s = t.code, i = t.message, u = t.path.join(".");
		if (!o[u]) if ("unionErrors" in t) {
			var a, c, l = t.unionErrors.reduce(function(r, e) {
				return e.errors.length < r.errors.length ? e : r;
			}).errors[0];
			o[u] = {
				message: null != (a = null == l ? void 0 : l.message) ? a : i,
				type: null != (c = null == l ? void 0 : l.code) ? c : s
			};
		} else o[u] = {
			message: i,
			type: s
		};
		if ("unionErrors" in t && t.unionErrors.forEach(function(e) {
			return e.errors.forEach(function(e) {
				return r.push(e);
			});
		}), e) {
			var f = o[u].types, d = f && f[t.code];
			o[u] = appendErrors(u, e, o, s, d ? [].concat(d, t.message) : t.message);
		}
		r.shift();
	}
	return o;
}
function i(r, e) {
	for (var t = Object.create(null), s = function() {
		var s = r[0], i = s.code, u = s.message, a = s.path.join(".");
		if (!t[a]) if ("invalid_union" === s.code && s.errors.length > 0) {
			var c, l, f = s.errors.reduce(function(r, e) {
				return e.length < r.length ? e : r;
			})[0];
			t[a] = {
				message: null != (c = null == f ? void 0 : f.message) ? c : u,
				type: null != (l = null == f ? void 0 : f.code) ? l : i
			};
		} else t[a] = {
			message: u,
			type: i
		};
		if ("invalid_union" === s.code && s.errors.forEach(function(e) {
			return e.forEach(function(e) {
				return r.push(o({}, e, { path: [].concat(s.path, e.path) }));
			});
		}), e) {
			var d = t[a].types, h = d && d[s.code];
			t[a] = appendErrors(a, e, t, i, h ? [].concat(h, s.message) : s.message);
		}
		r.shift();
	}; r.length;) s();
	return t;
}
function u(n, o, u) {
	if (void 0 === u && (u = {}), function(r) {
		return "_zod" in r && "object" == typeof r._zod;
	}(n)) return function(s, a, c) {
		try {
			return Promise.resolve(t(function() {
				function e(e) {
					return c.shouldUseNativeValidation && s$1({}, c), {
						errors: {},
						values: u.raw ? Object.assign({}, s) : e
					};
				}
				var t = n;
				return "sync" === u.mode ? e(t.parse(s, o)) : Promise.resolve(t.parseAsync(s, o)).then(e);
			}, function(r) {
				if (function(r) {
					var e;
					return !(null == r || null == (e = r._zod) || null == (e = e.traits) || !e.has("$ZodError"));
				}(r)) return {
					values: {},
					errors: o$1(i(r.issues, !c.shouldUseNativeValidation && "all" === c.criteriaMode), c)
				};
				throw r;
			}));
		} catch (r) {
			return Promise.reject(r);
		}
	};
	if (function(r) {
		return "_def" in r && "object" == typeof r._def;
	}(n)) return function(i, a, c) {
		try {
			return Promise.resolve(t(function() {
				return Promise.resolve(n["sync" === u.mode ? "parse" : "parseAsync"](i, o)).then(function(e) {
					return c.shouldUseNativeValidation && s$1({}, c), {
						errors: {},
						values: u.raw ? Object.assign({}, i) : e
					};
				});
			}, function(r) {
				if (function(r) {
					return Array.isArray(null == r ? void 0 : r.issues);
				}(r)) return {
					values: {},
					errors: o$1(s(r.errors, !c.shouldUseNativeValidation && "all" === c.criteriaMode), c)
				};
				throw r;
			}));
		} catch (r) {
			return Promise.reject(r);
		}
	};
	throw new Error("Invalid input: not a Zod schema");
}
//#endregion
//#region src/stores/AvailableEntitiesStore.ts
function getCookie(name) {
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
function getOptionsFromItems(items) {
	const options = [];
	for (const item of items) {
		let mdi_icon = "mdi-help";
		if (item.item_id.startsWith("light")) mdi_icon = "mdi-lightbulb";
		else if (item.item_id.startsWith("input_boolean")) mdi_icon = "mdi-toggle-switch-variant";
		else if (item.item_id.startsWith("switch")) mdi_icon = "mdi-toggle-switch-variant";
		else if (item.item_id.startsWith("button") || item.item_id.startsWith("input_button")) mdi_icon = "mdi-gesture-tap-button";
		else if (item.item_id.startsWith("climate")) mdi_icon = "mdi-thermostat";
		else if (item.item_id.startsWith("scene")) mdi_icon = "mdi-palette";
		else if (item.item_id.startsWith("script")) mdi_icon = "mdi-code-json";
		options.push({
			value: item.item_id,
			label: item.label,
			icon: mdi_icon
		});
	}
	return options;
}
var useAvailableEntitiesStore = create((set) => ({
	status: "none",
	home_assistant_entities: [],
	home_assistant_options: [],
	openhab_items: [],
	openhab_options: [],
	fetchEntities: async () => {
		set({ status: "loading" });
		fetch(`/rest/home_assistant/entities`, {
			credentials: "same-origin",
			method: "GET",
			mode: "same-origin",
			headers: {
				"Content-Type": "application/json",
				"X-CSRFToken": getCookie("csrftoken")
			}
		}).then((response) => response.json()).then((data) => {
			set((state) => ({
				...state,
				status: "loading",
				home_assistant_entities: data.items
			}));
			set((state) => ({
				...state,
				home_assistant_options: getOptionsFromItems(data.items)
			}));
			fetch(`/rest/openhab/items`, {
				credentials: "same-origin",
				method: "GET",
				mode: "same-origin",
				headers: {
					"Content-Type": "application/json",
					"X-CSRFToken": getCookie("csrftoken")
				}
			}).then((response) => response.json()).then((data) => {
				set((state) => ({
					...state,
					status: "loaded",
					openhab_items: data.items,
					openhab_options: getOptionsFromItems(data.items)
				}));
			});
		});
	}
}));
//#endregion
//#region src/components/MultiStep_AddEditEntity/entity_types/step3_edit_light.tsx
var schema$4 = object({
	id: number().nullable(),
	type: string(),
	entity_type: string(),
	room_id: number(),
	entities_page_id: number(),
	room_view_position: number(),
	friendly_name: string().min(1),
	controller: string(),
	can_color_temperature: boolean().optional(),
	can_dim: boolean().optional(),
	can_rgb: boolean().optional(),
	controlled_by_nspanel_main_page: boolean().optional(),
	home_assistant_name: string().optional(),
	is_ceiling_light: boolean().optional(),
	openhab_item_color_temp: string().optional(),
	openhab_item_dimmer: string().optional(),
	openhab_item_rgb: string().optional()
});
var CustomOption$4 = ({ innerProps, isDisabled, isFocused, isSelected, children, data }) => {
	if (isDisabled) return null;
	return /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
		...innerProps,
		className: `p-1 ${isSelected ? "bg-primary/20 rounded-sm" : ""} ${isFocused ? "bg-primary/20 rounded-sm" : ""}`,
		children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", { className: `mdi ${data.icon} me-2` }), children]
	});
};
var select_components$4 = { Option: CustomOption$4 };
var MultiStep_AddEditEntity_Step3_Light = ({ controller, room_id, entities_page_id, room_view_position, id, onComplete }) => {
	const { handleSubmit, register, reset, getValues, setValue, formState: { isValid } } = useForm({
		resolver: u(schema$4),
		defaultValues: {
			id: id ?? null,
			controller,
			type: "entity",
			entity_type: "light",
			room_id: room_id ?? -1,
			entities_page_id,
			room_view_position,
			can_color_temperature: false,
			can_dim: false,
			can_rgb: false,
			controlled_by_nspanel_main_page: true,
			is_ceiling_light: false,
			home_assistant_name: "",
			openhab_item_color_temp: "",
			openhab_item_dimmer: "",
			openhab_item_rgb: ""
		}
	});
	function getCookie(name) {
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
	(0, import_react.useEffect)(() => {
		if (id != null) {
			const entityData = useEntitiesPagesStore.getState().entities.find((entity) => entity.id == id);
			reset(entityData);
		}
	}, [id, useEntitiesPagesStore.getState().entities]);
	function onSave(values) {
		fetch("/rest/entities/lights", {
			credentials: "same-origin",
			method: "PUT",
			mode: "same-origin",
			headers: {
				"Content-Type": "application/json",
				"X-CSRFToken": getCookie("csrftoken")
			},
			body: JSON.stringify({ values })
		}).then(async (response) => {
			const data = await response.json();
			if (!response.ok) {
				const error = data && data.message || response.status;
				return Promise.reject(error);
			}
			onComplete?.();
		}).catch((error) => {
			Notify({
				message: `Error saving entity. Error: ${error}`,
				level: "error",
				duration: 1e4
			});
		});
	}
	const classNames = {
		control: (state) => `${state.isFocused ? "border" : "border-0"} border-accent p-2.5 text-sm rounded-box bg-base-300 rounded-md`,
		menu: () => "bg-base-300 p-2.5 rounded-box",
		option: (state) => `p-1 ${state.isSelected ? "bg-primary/20 rounded-sm" : ""} ${state.isFocused ? "bg-primary/20 rounded-sm" : ""}`
	};
	return /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("form", {
		onSubmit: handleSubmit(onSave),
		children: [
			/* @__PURE__ */ (0, import_jsx_runtime.jsx)("input", {
				type: "hidden",
				...register("id")
			}),
			/* @__PURE__ */ (0, import_jsx_runtime.jsx)("input", {
				type: "hidden",
				...register("controller")
			}),
			/* @__PURE__ */ (0, import_jsx_runtime.jsx)("input", {
				type: "hidden",
				...register("type")
			}),
			/* @__PURE__ */ (0, import_jsx_runtime.jsx)("input", {
				type: "hidden",
				...register("entity_type")
			}),
			/* @__PURE__ */ (0, import_jsx_runtime.jsx)("input", {
				type: "hidden",
				...register("room_id")
			}),
			/* @__PURE__ */ (0, import_jsx_runtime.jsx)("input", {
				type: "hidden",
				...register("entities_page_id")
			}),
			/* @__PURE__ */ (0, import_jsx_runtime.jsx)("input", {
				type: "hidden",
				...register("room_view_position")
			}),
			/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
				className: "w-full",
				children: [
					/* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
						className: "flex justify-center mb-4 duration-500 transition-transform ease-linear",
						children: /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
							className: "w-full",
							children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("label", {
								htmlFor: "add_new_light_name",
								className: "block mb-2 text-sm font-medium",
								children: "Name"
							}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
								className: "flex flex-row-reverse",
								children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)("input", {
									className: "outline-none bg-base-300 border-neutral rounded-md border focus:ring-0 focus:border-accent block flex-1 min-w-0 w-full text-sm p-2.5 peer/search_text",
									type: "text",
									id: "add_new_light_name",
									...register("friendly_name"),
									required: true
								})
							})]
						})
					}),
					controller == "home_assistant" && /* @__PURE__ */ (0, import_jsx_runtime.jsxs)(import_jsx_runtime.Fragment, { children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("label", {
						className: "block mb-2 text-sm font-medium",
						children: "Home Assistant entity"
					}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)(StateManagedSelect$1, {
						options: useAvailableEntitiesStore.getState().home_assistant_options.filter((option) => option.value.startsWith("light.") || option.value.startsWith("switch.")),
						classNames,
						onChange: (newValue) => setValue("home_assistant_name", newValue ? newValue.value : "", {
							shouldValidate: true,
							shouldDirty: true
						}),
						unstyled: true,
						components: select_components$4,
						value: useAvailableEntitiesStore.getState().home_assistant_options.find((option) => option.value === getValues("home_assistant_name")),
						styles: {
							input: (base) => ({
								...base,
								"input:focus": { boxShadow: "none" }
							}),
							multiValueLabel: (base) => ({
								...base,
								whiteSpace: "normal",
								overflow: "visible"
							}),
							control: (base) => ({
								...base,
								transition: "none"
							})
						}
					})] }),
					/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("fieldset", {
						className: "fieldset mt-4",
						children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("input", {
							type: "hidden",
							...register("controlled_by_nspanel_main_page")
						}), /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("label", {
							className: `label ${getValues("controlled_by_nspanel_main_page") ? "text-base-content" : "text-base-content/50"}`,
							children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("input", {
								type: "checkbox",
								className: "toggle toggle-accent",
								checked: getValues("controlled_by_nspanel_main_page"),
								onClick: () => {
									setValue("controlled_by_nspanel_main_page", !getValues("controlled_by_nspanel_main_page"), {
										shouldValidate: true,
										shouldDirty: true
									});
								}
							}), getValues("controlled_by_nspanel_main_page") ? "Controlled by NSPanel main page" : "Not controlled by NSPanel main page"]
						})]
					}),
					/* @__PURE__ */ (0, import_jsx_runtime.jsx)("label", {
						className: "block mb-2 mt-4 text-sm font-medium",
						children: "Light type"
					}),
					/* @__PURE__ */ (0, import_jsx_runtime.jsx)("input", {
						type: "hidden",
						...register("is_ceiling_light")
					}),
					/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
						className: "grid grid-cols-2 gap-4 w-full",
						children: [/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
							className: "group bg-base-100 rounded-box border border-neutral border-dashed p-6 cursor-pointer hover:bg-yellow-500/10",
							onClick: () => setValue("is_ceiling_light", false, {
								shouldValidate: true,
								shouldDirty: true
							}),
							children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
								className: "flex justify-center items-center p-4",
								children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", { className: `mdi mdi-lamp text-5xl text-neutral-content/50 group-hover:text-yellow-500 ${getValues("is_ceiling_light") ? "" : "text-yellow-500"}` })
							}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
								className: "flex justify-center items-center",
								children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", {
									className: "text-base-content/50 group-hover:text-base-content text-center",
									children: "Table light"
								})
							})]
						}), /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
							className: "group bg-base-100 rounded-box border border-neutral border-dashed p-6 cursor-pointer hover:bg-yellow-500/10",
							onClick: () => setValue("is_ceiling_light", true, {
								shouldValidate: true,
								shouldDirty: true
							}),
							children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
								className: "flex justify-center items-center p-4",
								children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", { className: `mdi mdi-ceiling-light text-5xl text-neutral-content/50 group-hover:text-yellow-500 ${getValues("is_ceiling_light") ? "text-yellow-500" : ""}` })
							}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
								className: "flex justify-center items-center",
								children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", {
									className: "text-base-content/50 group-hover:text-base-content text-center",
									children: "Ceiling light"
								})
							})]
						})]
					}),
					/* @__PURE__ */ (0, import_jsx_runtime.jsx)("label", {
						className: "block mb-2 mt-4 text-sm font-medium",
						children: "Light capabilities"
					}),
					/* @__PURE__ */ (0, import_jsx_runtime.jsx)("input", {
						type: "hidden",
						...register("can_dim")
					}),
					/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
						className: "grid grid-cols-3 gap-4 w-full",
						children: [
							/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
								className: "group bg-base-100 rounded-box border border-neutral border-dashed p-6 cursor-pointer hover:bg-yellow-500/10",
								onClick: () => {
									setValue("can_dim", !getValues("can_dim"), {
										shouldValidate: true,
										shouldDirty: true
									});
								},
								children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
									className: "flex justify-center items-center p-4",
									children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", { className: `mdi mdi-lightbulb-on-50 text-5xl text-neutral-content/50 group-hover:text-yellow-500 ${getValues("can_dim") ? "text-yellow-500" : ""}` })
								}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
									className: "flex justify-center items-center",
									children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", {
										className: "text-base-content/50 group-hover:text-base-content text-center",
										children: "Dimmable"
									})
								})]
							}),
							/* @__PURE__ */ (0, import_jsx_runtime.jsx)("input", {
								type: "hidden",
								...register("can_color_temperature")
							}),
							/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
								className: "group bg-base-100 rounded-box border border-neutral border-dashed p-6 cursor-pointer hover:bg-yellow-500/10",
								onClick: () => {
									setValue("can_color_temperature", !getValues("can_color_temperature"), {
										shouldValidate: true,
										shouldDirty: true
									});
								},
								children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
									className: "flex justify-center items-center p-4",
									children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", { className: `mdi mdi-temperature-kelvin text-5xl text-neutral-content/50 group-hover:text-yellow-500 ${getValues("can_color_temperature") ? "text-yellow-500" : ""}` })
								}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
									className: "flex justify-center items-center",
									children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", {
										className: "text-base-content/50 group-hover:text-base-content text-center",
										children: "Color temp"
									})
								})]
							}),
							/* @__PURE__ */ (0, import_jsx_runtime.jsx)("input", {
								type: "hidden",
								...register("can_rgb")
							}),
							/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
								className: "group bg-base-100 rounded-box border border-neutral border-dashed p-6 cursor-pointer hover:bg-yellow-500/10",
								onClick: () => {
									setValue("can_rgb", !getValues("can_rgb"), {
										shouldValidate: true,
										shouldDirty: true
									});
								},
								children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
									className: "flex justify-center items-center p-4",
									children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", { className: `mdi mdi-palette text-5xl text-neutral-content/50 group-hover:text-yellow-500 ${getValues("can_rgb") ? "text-yellow-500" : ""}` })
								}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
									className: "flex justify-center items-center",
									children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", {
										className: "text-base-content/50 group-hover:text-base-content text-center",
										children: "Color"
									})
								})]
							})
						]
					}),
					controller == "openhab" && /* @__PURE__ */ (0, import_jsx_runtime.jsxs)(import_jsx_runtime.Fragment, { children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("label", {
						className: "block mb-2 mt-4 text-sm font-medium",
						children: "Brightness item"
					}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)(StateManagedSelect$1, {
						options: useAvailableEntitiesStore.getState().openhab_options,
						classNames,
						onChange: (newValue) => setValue("openhab_item_dimmer", newValue ? newValue.value : "", {
							shouldValidate: true,
							shouldDirty: true
						}),
						value: useAvailableEntitiesStore.getState().openhab_options.find((option) => option.value === getValues("openhab_item_dimmer")),
						unstyled: true,
						components: select_components$4,
						styles: {
							input: (base) => ({
								...base,
								"input:focus": { boxShadow: "none" }
							}),
							multiValueLabel: (base) => ({
								...base,
								whiteSpace: "normal",
								overflow: "visible"
							}),
							control: (base) => ({
								...base,
								transition: "none"
							})
						}
					})] }),
					controller == "openhab" && getValues("can_color_temperature") == true && /* @__PURE__ */ (0, import_jsx_runtime.jsxs)(import_jsx_runtime.Fragment, { children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("label", {
						className: "block mb-2 mt-4 text-sm font-medium",
						children: "Color temperature item"
					}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)(StateManagedSelect$1, {
						options: useAvailableEntitiesStore.getState().openhab_options,
						classNames,
						onChange: (newValue) => {
							setValue("openhab_item_color_temp", newValue ? newValue.value : "", {
								shouldValidate: true,
								shouldDirty: true
							});
						},
						value: useAvailableEntitiesStore.getState().openhab_options.find((option) => option.value === getValues("openhab_item_color_temp")),
						components: select_components$4,
						unstyled: true,
						styles: {
							input: (base) => ({
								...base,
								"input:focus": { boxShadow: "none" }
							}),
							multiValueLabel: (base) => ({
								...base,
								whiteSpace: "normal",
								overflow: "visible"
							}),
							control: (base) => ({
								...base,
								transition: "none"
							})
						}
					})] }),
					controller == "openhab" && getValues("can_rgb") == true && /* @__PURE__ */ (0, import_jsx_runtime.jsxs)(import_jsx_runtime.Fragment, { children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("label", {
						className: "block mb-2 mt-4 text-sm font-medium",
						children: "RGB item"
					}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)(StateManagedSelect$1, {
						options: useAvailableEntitiesStore.getState().openhab_options,
						classNames,
						onChange: (newValue) => setValue("openhab_item_rgb", newValue ? newValue.value : "", {
							shouldValidate: true,
							shouldDirty: true
						}),
						value: useAvailableEntitiesStore.getState().openhab_options.find((option) => option.value === getValues("openhab_item_rgb")),
						components: select_components$4,
						unstyled: true,
						styles: {
							input: (base) => ({
								...base,
								"input:focus": { boxShadow: "none" }
							}),
							multiValueLabel: (base) => ({
								...base,
								whiteSpace: "normal",
								overflow: "visible"
							}),
							control: (base) => ({
								...base,
								transition: "none"
							})
						}
					})] })
				]
			}),
			/* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
				className: "flex mt-4 justify-end items-center",
				children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)("button", {
					className: "btn btn-info",
					disabled: !isValid,
					children: "Save"
				})
			})
		]
	});
};
//#endregion
//#region src/components/MultiStep_AddEditEntity/entity_types/step3_edit_switch.tsx
var schema$3 = object({
	id: number().nullable(),
	type: string(),
	entity_type: string(),
	room_id: number(),
	entities_page_id: number(),
	room_view_position: number(),
	friendly_name: string().min(1),
	controller: string(),
	home_assistant_name: string().optional(),
	openhab_item_switch: string().optional()
});
var CustomOption$3 = ({ innerProps, isDisabled, isFocused, isSelected, children, data }) => {
	if (isDisabled) return null;
	return /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
		...innerProps,
		className: `p-1 ${isSelected ? "bg-primary/20 rounded-sm" : ""} ${isFocused ? "bg-primary/20 rounded-sm" : ""}`,
		children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", { className: `mdi ${data.icon} me-2` }), children]
	});
};
var select_components$3 = { Option: CustomOption$3 };
var MultiStep_AddEditEntity_Step3_Switch = ({ controller, room_id, entities_page_id, room_view_position, id, onComplete }) => {
	const { handleSubmit, register, reset, getValues, setValue, formState: { isValid } } = useForm({
		resolver: u(schema$3),
		defaultValues: {
			id: id ?? null,
			controller,
			type: "entity",
			entity_type: "light",
			room_id,
			entities_page_id,
			room_view_position,
			openhab_item_switch: "",
			home_assistant_name: ""
		}
	});
	function getCookie(name) {
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
	(0, import_react.useEffect)(() => {
		if (id != null) {
			const entityData = useEntitiesPagesStore.getState().entities.find((entity) => entity.id == id);
			reset(entityData);
		}
	}, [id, useEntitiesPagesStore.getState().entities]);
	function saveEntity(data) {
		fetch("/rest/entities/switches", {
			credentials: "same-origin",
			method: "PUT",
			mode: "same-origin",
			headers: {
				"Content-Type": "application/json",
				"X-CSRFToken": getCookie("csrftoken")
			},
			body: JSON.stringify(data)
		}).then(async (response) => {
			const data = await response.json();
			if (!response.ok) {
				const error = data && data.message || response.status;
				return Promise.reject(error);
			}
			onComplete?.();
		}).catch((error) => {
			Notify({
				message: `Error saving entity. Error: ${error}`,
				level: "error",
				duration: 1e4
			});
		});
	}
	const classNames = {
		control: (state) => `${state.isFocused ? "border" : "border-0"} border-accent p-2.5 text-sm rounded-box bg-base-300 rounded-md`,
		menu: () => "bg-base-300 p-2.5 rounded-box",
		option: (state) => `p-1 ${state.isSelected ? "bg-primary/20 rounded-sm" : ""} ${state.isFocused ? "bg-primary/20 rounded-sm" : ""}`
	};
	return /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("form", {
		onSubmit: handleSubmit(saveEntity),
		children: [
			/* @__PURE__ */ (0, import_jsx_runtime.jsx)("input", {
				type: "hidden",
				...register("id")
			}),
			/* @__PURE__ */ (0, import_jsx_runtime.jsx)("input", {
				type: "hidden",
				...register("controller")
			}),
			/* @__PURE__ */ (0, import_jsx_runtime.jsx)("input", {
				type: "hidden",
				...register("type")
			}),
			/* @__PURE__ */ (0, import_jsx_runtime.jsx)("input", {
				type: "hidden",
				...register("entity_type")
			}),
			/* @__PURE__ */ (0, import_jsx_runtime.jsx)("input", {
				type: "hidden",
				...register("room_id")
			}),
			/* @__PURE__ */ (0, import_jsx_runtime.jsx)("input", {
				type: "hidden",
				...register("entities_page_id")
			}),
			/* @__PURE__ */ (0, import_jsx_runtime.jsx)("input", {
				type: "hidden",
				...register("room_view_position")
			}),
			/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
				className: "w-full",
				children: [
					/* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
						className: "flex justify-center mb-4 duration-500 transition-transform ease-linear",
						children: /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
							className: "w-full",
							children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("label", {
								htmlFor: "add_new_switch_name",
								className: "block mb-2 text-sm font-medium",
								children: "Name"
							}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
								className: "flex flex-row-reverse",
								children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)("input", {
									className: "outline-none bg-base-300 border-neutral rounded-md border focus:ring-0 focus:border-accent block flex-1 min-w-0 w-full text-sm p-2.5 peer/search_text",
									type: "text",
									id: "add_new_switch_name",
									...register("friendly_name"),
									required: true
								})
							})]
						})
					}),
					controller == "home_assistant" && /* @__PURE__ */ (0, import_jsx_runtime.jsxs)(import_jsx_runtime.Fragment, { children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("label", {
						className: "block mb-2 text-sm font-medium",
						children: "Home Assistant entity"
					}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)(StateManagedSelect$1, {
						options: useAvailableEntitiesStore.getState().home_assistant_options.filter((option) => option.value.startsWith("switch.") || option.value.startsWith("input_boolean.")),
						classNames,
						onChange: (newValue) => {
							setValue("home_assistant_name", newValue ? newValue.value : "");
						},
						unstyled: true,
						components: select_components$3,
						value: useAvailableEntitiesStore.getState().home_assistant_options.find((option) => option.value === getValues("home_assistant_name")),
						styles: {
							input: (base) => ({
								...base,
								"input:focus": { boxShadow: "none" }
							}),
							multiValueLabel: (base) => ({
								...base,
								whiteSpace: "normal",
								overflow: "visible"
							}),
							control: (base) => ({
								...base,
								transition: "none"
							})
						}
					})] }),
					controller == "openhab" && /* @__PURE__ */ (0, import_jsx_runtime.jsxs)(import_jsx_runtime.Fragment, { children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("label", {
						className: "block mb-2 mt-4 text-sm font-medium",
						children: "OpenHAB item"
					}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)(StateManagedSelect$1, {
						options: useAvailableEntitiesStore.getState().openhab_options,
						classNames,
						onChange: (newValue) => setValue("openhab_item_switch", newValue ? newValue.value : ""),
						value: useAvailableEntitiesStore.getState().openhab_options.find((option) => option.value === getValues("openhab_item_switch")),
						unstyled: true,
						components: select_components$3,
						styles: {
							input: (base) => ({
								...base,
								"input:focus": { boxShadow: "none" }
							}),
							multiValueLabel: (base) => ({
								...base,
								whiteSpace: "normal",
								overflow: "visible"
							}),
							control: (base) => ({
								...base,
								transition: "none"
							})
						}
					})] })
				]
			}),
			/* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
				className: "flex mt-4 justify-end items-center",
				children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)("button", {
					type: "submit",
					className: "btn btn-info",
					disabled: !isValid,
					children: "Save"
				})
			})
		]
	});
};
//#endregion
//#region src/components/MultiStep_AddEditEntity/entity_types/step3_edit_button.tsx
var schema$2 = object({
	id: number().nullable(),
	type: string(),
	entity_type: string(),
	room_id: number(),
	entities_page_id: number(),
	room_view_position: number(),
	friendly_name: string().min(1),
	controller: string(),
	home_assistant_name: string().optional(),
	mqtt_topic: string().optional(),
	mqtt_payload: string().optional()
});
var CustomOption$2 = ({ innerProps, isDisabled, isFocused, isSelected, children, data }) => {
	if (isDisabled) return null;
	return /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
		...innerProps,
		className: `p-1 ${isSelected ? "bg-primary/20 rounded-sm" : ""} ${isFocused ? "bg-primary/20 rounded-sm" : ""}`,
		children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", { className: `mdi ${data.icon} me-2` }), children]
	});
};
var select_components$2 = { Option: CustomOption$2 };
var MultiStep_AddEditEntity_Step3_Button = ({ controller, room_id, entities_page_id, room_view_position, id, onComplete }) => {
	const { handleSubmit, register, reset, getValues, setValue, formState: { isValid } } = useForm({
		resolver: u(schema$2),
		defaultValues: {
			id: null,
			controller,
			type: "entity",
			entity_type: "light",
			room_id: room_id ?? -1,
			entities_page_id,
			room_view_position,
			home_assistant_name: "",
			mqtt_topic: "",
			mqtt_payload: ""
		}
	});
	function getCookie(name) {
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
	(0, import_react.useEffect)(() => {
		if (id != null) {
			const entityData = useEntitiesPagesStore.getState().entities.find((entity) => entity.id == id);
			reset(entityData);
		}
	}, [id, useEntitiesPagesStore.getState().entities]);
	function saveEntity(data) {
		fetch("/rest/entities/buttons", {
			credentials: "same-origin",
			method: "PUT",
			mode: "same-origin",
			headers: {
				"Content-Type": "application/json",
				"X-CSRFToken": getCookie("csrftoken")
			},
			body: JSON.stringify(data)
		}).then(async (response) => {
			const data = await response.json();
			if (!response.ok) {
				const error = data && data.message || response.status;
				return Promise.reject(error);
			}
			onComplete?.();
		}).catch((error) => {
			Notify({
				message: `Error saving entity. Error: ${error}`,
				level: "error",
				duration: 1e4
			});
		});
	}
	const classNames = {
		control: (state) => `${state.isFocused ? "border" : "border-0"} border-accent p-2.5 text-sm rounded-box bg-base-300 rounded-md`,
		menu: () => "bg-base-300 p-2.5 rounded-box",
		option: (state) => `p-1 ${state.isSelected ? "bg-primary/20 rounded-sm" : ""} ${state.isFocused ? "bg-primary/20 rounded-sm" : ""}`
	};
	return /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("form", {
		onSubmit: handleSubmit(saveEntity),
		children: [
			/* @__PURE__ */ (0, import_jsx_runtime.jsx)("input", {
				type: "hidden",
				...register("id")
			}),
			/* @__PURE__ */ (0, import_jsx_runtime.jsx)("input", {
				type: "hidden",
				...register("controller")
			}),
			/* @__PURE__ */ (0, import_jsx_runtime.jsx)("input", {
				type: "hidden",
				...register("type")
			}),
			/* @__PURE__ */ (0, import_jsx_runtime.jsx)("input", {
				type: "hidden",
				...register("entity_type")
			}),
			/* @__PURE__ */ (0, import_jsx_runtime.jsx)("input", {
				type: "hidden",
				...register("room_id")
			}),
			/* @__PURE__ */ (0, import_jsx_runtime.jsx)("input", {
				type: "hidden",
				...register("entities_page_id")
			}),
			/* @__PURE__ */ (0, import_jsx_runtime.jsx)("input", {
				type: "hidden",
				...register("room_view_position")
			}),
			/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
				className: "w-full",
				children: [
					/* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
						className: "flex justify-center mb-4 duration-500 transition-transform ease-linear",
						children: /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
							className: "w-full",
							children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("label", {
								htmlFor: "add_new_button_name",
								className: "block mb-2 text-sm font-medium",
								children: "Name"
							}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
								className: "flex flex-row-reverse",
								children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)("input", {
									className: "outline-none bg-base-300 border-neutral rounded-md border focus:ring-0 focus:border-accent block flex-1 min-w-0 w-full text-sm p-2.5 peer/search_text",
									type: "text",
									id: "add_new_button_name",
									...register("friendly_name"),
									required: true
								})
							})]
						})
					}),
					controller == "home_assistant" && /* @__PURE__ */ (0, import_jsx_runtime.jsxs)(import_jsx_runtime.Fragment, { children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("label", {
						className: "block mb-2 text-sm font-medium",
						children: "Home Assistant entity"
					}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)(StateManagedSelect$1, {
						options: useAvailableEntitiesStore.getState().home_assistant_options.filter((option) => option.value.startsWith("button.") || option.value.startsWith("input_button.")),
						classNames,
						onChange: (newValue) => {
							setValue("home_assistant_name", newValue ? newValue.value : "");
						},
						unstyled: true,
						components: select_components$2,
						value: useAvailableEntitiesStore.getState().home_assistant_options.find((option) => option.value === getValues("home_assistant_name")),
						styles: {
							input: (base) => ({
								...base,
								"input:focus": { boxShadow: "none" }
							}),
							multiValueLabel: (base) => ({
								...base,
								whiteSpace: "normal",
								overflow: "visible"
							}),
							control: (base) => ({
								...base,
								transition: "none"
							})
						}
					})] }),
					controller == "nspm" && /* @__PURE__ */ (0, import_jsx_runtime.jsxs)(import_jsx_runtime.Fragment, { children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
						className: "flex justify-center mb-4 duration-500 transition-transform ease-linear",
						children: /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
							className: "w-full",
							children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("label", {
								className: "block mb-2 text-sm font-medium",
								children: "MQTT Topic"
							}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
								className: "flex flex-row-reverse",
								children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)("input", {
									className: "outline-none bg-base-300 border-neutral rounded-md border focus:ring-0 focus:border-accent block flex-1 min-w-0 w-full text-sm p-2.5 peer/search_text",
									type: "text",
									...register("mqtt_topic"),
									required: true
								})
							})]
						})
					}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
						className: "flex justify-center mb-4 duration-500 transition-transform ease-linear",
						children: /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
							className: "w-full",
							children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("label", {
								className: "block mb-2 text-sm font-medium",
								children: "MQTT Payload"
							}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
								className: "flex flex-row-reverse",
								children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)("input", {
									className: "outline-none bg-base-300 border-neutral rounded-md border focus:ring-0 focus:border-accent block flex-1 min-w-0 w-full text-sm p-2.5 peer/search_text",
									type: "text",
									...register("mqtt_payload"),
									required: true
								})
							})]
						})
					})] })
				]
			}),
			/* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
				className: "flex mt-4 justify-end items-center",
				children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)("button", {
					type: "submit",
					className: "btn btn-info",
					disabled: !isValid,
					children: "Save"
				})
			})
		]
	});
};
//#endregion
//#region src/components/MultiStep_AddEditEntity/entity_types/step3_edit_thermostat.tsx
var schema$1 = object({
	id: number().nullable(),
	type: string(),
	entity_type: string(),
	room_id: number(),
	entities_page_id: number(),
	room_view_position: number(),
	friendly_name: string().min(1),
	step_size: float32(),
	controller: string(),
	home_assistant_name: string().optional(),
	openhab_fan_mode_item: string().optional(),
	openhab_hvac_mode_item: string().optional(),
	openhab_preset_mode_item: string().optional(),
	openhab_swing_mode_item: string().optional(),
	openhab_swingh_mode_item: string().optional(),
	openhab_temperature_item: string().optional(),
	fan_modes: array(object({
		icon: string(),
		label: string().min(1),
		value: string().min(1)
	}).optional()),
	hvac_modes: array(object({
		icon: string(),
		label: string().min(1),
		value: string().min(1)
	}).optional()),
	preset_modes: array(object({
		icon: string(),
		label: string().min(1),
		value: string().min(1)
	})).optional(),
	swing_modes: array(object({
		icon: string(),
		label: string().min(1),
		value: string().min(1)
	})).optional(),
	swingh_modes: array(object({
		icon: string(),
		label: string().min(1),
		value: string().min(1)
	})).optional()
});
var CustomOption$1 = ({ innerProps, isDisabled, isFocused, isSelected, children, data }) => {
	if (isDisabled) return null;
	return /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
		...innerProps,
		className: `p-1 ${isSelected ? "bg-primary/20 rounded-sm" : ""} ${isFocused ? "bg-primary/20 rounded-sm" : ""}`,
		children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", { className: `mdi ${data.icon} me-2` }), children]
	});
};
var select_components$1 = { Option: CustomOption$1 };
var IconSelector = ({ value, onChange }) => {
	const climateIcons = [
		{
			name: "Off",
			icon: "h"
		},
		{
			name: "Heating",
			icon: "!"
		},
		{
			name: "Cooling",
			icon: "8"
		},
		{
			name: "Hot/Cold",
			icon: "#"
		},
		{
			name: "Thermostat",
			icon: "7"
		},
		{
			name: "Thermostat Auto",
			icon: "$"
		},
		{
			name: "Dry",
			icon: "%"
		},
		{
			name: "Eco",
			icon: "&"
		},
		{
			name: "Away",
			icon: "'"
		},
		{
			name: "Home",
			icon: "("
		},
		{
			name: "Sleep",
			icon: ")"
		},
		{
			name: "Boost",
			icon: "*"
		},
		{
			name: "Comfort",
			icon: "+"
		},
		{
			name: "Activity",
			icon: ","
		},
		{
			name: "Defrosting",
			icon: "-"
		},
		{
			name: "Swing Both",
			icon: "."
		},
		{
			name: "Swing Horizontal",
			icon: "/"
		},
		{
			name: "Swing Vertical",
			icon: "0"
		},
		{
			name: "Fan Off",
			icon: "1"
		},
		{
			name: "Fan Auto",
			icon: "2"
		},
		{
			name: "Fan",
			icon: "3"
		},
		{
			name: "Fan 1",
			icon: "4"
		},
		{
			name: "Fan 2",
			icon: "5"
		},
		{
			name: "Fan 3",
			icon: "6"
		}
	];
	const [selectedIcon, setSelectedIcon] = (0, import_react.useState)(value);
	const [detailsOpen, setDetailsOpen] = (0, import_react.useState)(false);
	const ref = (0, import_react.useRef)(null);
	const select_icon = (icon) => {
		setSelectedIcon(icon);
		onChange(icon);
		setDetailsOpen(false);
	};
	(0, import_react.useEffect)(() => {
		const listener = (event) => {
			if (ref.current && !ref.current.contains(event.target)) setDetailsOpen(false);
		};
		window.addEventListener("click", listener);
		return () => {
			window.removeEventListener("click", listener);
		};
	}, [ref, detailsOpen]);
	return /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("details", {
		open: detailsOpen,
		onToggle: (e) => setDetailsOpen(e.currentTarget.open),
		className: "dropdown dropdown-end h-auto",
		id: "dropdown_icon_selector",
		ref,
		children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("summary", {
			id: "icon_selector_button",
			title: "Select icon",
			className: "btn rounded-box h-full border-neutral border-y border-x-0 font-nspm-mdi w-full",
			children: selectedIcon
		}), /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
			className: "dropdown-content bg-base-100 text-base-content rounded-box rounded-tr-none w-56 border border-primary",
			children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
				className: "flex justify-center pb-1 mb-2 mt-1 border-b border-primary",
				children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", {
					className: "text-sm",
					children: "Select Icon"
				})
			}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
				className: "grid grid-cols-4 w-full",
				children: climateIcons.map((icon) => /* @__PURE__ */ (0, import_jsx_runtime.jsx)("button", {
					type: "button",
					title: icon.name,
					className: "btn btn-ghost rounded-none hover:btn-info icon-select-button",
					"data-icon_value": icon.icon,
					onClick: () => {
						select_icon(icon.icon);
					},
					children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", {
						className: "font-nspm-mdi",
						children: icon.icon
					})
				}))
			})]
		})]
	});
};
var MultiStep_AddEditEntity_Step3_Thermostat = ({ controller, room_id, entities_page_id, room_view_position, id, onComplete }) => {
	const { control, handleSubmit, register, reset, getValues, setValue, formState: { isValid } } = useForm({
		resolver: u(schema$1),
		defaultValues: {
			id: id ?? null,
			controller,
			type: "entity",
			entity_type: "thermostat",
			room_id,
			entities_page_id,
			room_view_position,
			home_assistant_name: "",
			openhab_fan_mode_item: "",
			openhab_hvac_mode_item: "",
			openhab_preset_mode_item: "",
			openhab_swing_mode_item: "",
			openhab_swingh_mode_item: "",
			openhab_temperature_item: "",
			step_size: .5,
			fan_modes: [],
			hvac_modes: [],
			preset_modes: [],
			swing_modes: [],
			swingh_modes: []
		}
	});
	const { entities } = useEntitiesPagesStore.getState();
	const { fields: fanModeFields, append: appendFanModeField, remove: removeFanModeField } = useFieldArray({
		control,
		name: "fan_modes"
	});
	const { fields: hvacModeFields, append: appendHvacModeField, remove: removeHvacModeField } = useFieldArray({
		control,
		name: "hvac_modes"
	});
	const { fields: presetModeFields, append: appendPresetModeField, remove: removePresetModeField } = useFieldArray({
		control,
		name: "preset_modes"
	});
	const { fields: swingModeFields, append: appendSwingModeField, remove: removeSwingModeField } = useFieldArray({
		control,
		name: "swing_modes"
	});
	const { fields: swinghModeFields, append: appendSwinghModeField, remove: removeSwinghModeField } = useFieldArray({
		control,
		name: "swingh_modes"
	});
	function getCookie(name) {
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
	(0, import_react.useEffect)(() => {
		if (id != null) {
			const entityData = entities.find((entity) => entity.id == id);
			console.log("Found entity data:", entityData);
			reset(entityData);
			if (entityData?.step_size) setValue("step_size", parseFloat(String(entityData.step_size)));
		}
	}, [
		id,
		entities,
		reset,
		setValue
	]);
	function saveEntity(data) {
		fetch("/rest/entities/thermostats", {
			credentials: "same-origin",
			method: "PUT",
			mode: "same-origin",
			headers: {
				"Content-Type": "application/json",
				"X-CSRFToken": getCookie("csrftoken")
			},
			body: JSON.stringify(data)
		}).then(async (response) => {
			const data = await response.json();
			if (!response.ok) {
				const error = data && data.message || response.status;
				return Promise.reject(error);
			}
			onComplete?.();
		}).catch((error) => {
			Notify({
				message: `Error saving entity. Error: ${error}`,
				level: "error",
				duration: 1e4
			});
		});
	}
	function loadHaEntitySettings() {
		const selected_entity = useAvailableEntitiesStore.getState().home_assistant_entities.find((entity) => entity.item_id == getValues("home_assistant_name"));
		if (!selected_entity) return;
		for (let i = fanModeFields.length - 1; i >= 0; i--) removeFanModeField(i);
		for (let i = hvacModeFields.length - 1; i >= 0; i--) removeHvacModeField(i);
		for (let i = presetModeFields.length - 1; i >= 0; i--) removePresetModeField(i);
		for (let i = swingModeFields.length - 1; i >= 0; i--) removeSwingModeField(i);
		for (let i = swinghModeFields.length - 1; i >= 0; i--) removeSwinghModeField(i);
		const attributes = selected_entity.item.attributes;
		if ("fan_modes" in attributes) attributes["fan_modes"].forEach((mode) => {
			appendFanModeField({
				icon: "",
				label: mode,
				value: mode
			});
		});
		if ("hvac_modes" in attributes) attributes["hvac_modes"].forEach((mode) => {
			appendHvacModeField({
				icon: "",
				label: mode,
				value: mode
			});
		});
		if ("preset_modes" in attributes) attributes["preset_modes"].forEach((mode) => {
			appendPresetModeField({
				icon: "",
				label: mode,
				value: mode
			});
		});
		if ("swing_modes" in attributes) attributes["swing_modes"].forEach((mode) => {
			appendSwingModeField({
				icon: "",
				label: mode,
				value: mode
			});
		});
		if ("swing_horizontal_modes" in attributes) attributes["swing_horizontal_modes"].forEach((mode) => {
			appendSwinghModeField({
				icon: "",
				label: mode,
				value: mode
			});
		});
	}
	const classNames = {
		control: (state) => `${state.isFocused ? "border" : "border-0"} border-accent p-2.5 text-sm rounded-box bg-base-300 text-base-content rounded-md`,
		menu: () => "bg-base-300 p-2.5 rounded-box text-base-content",
		option: (state) => `p-1 ${state.isSelected ? "bg-primary/20 rounded-sm" : ""} ${state.isFocused ? "bg-primary/20 rounded-sm" : ""}`
	};
	return /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("form", {
		onSubmit: handleSubmit(saveEntity),
		children: [
			/* @__PURE__ */ (0, import_jsx_runtime.jsx)("input", {
				type: "hidden",
				...register("id")
			}),
			/* @__PURE__ */ (0, import_jsx_runtime.jsx)("input", {
				type: "hidden",
				...register("controller")
			}),
			/* @__PURE__ */ (0, import_jsx_runtime.jsx)("input", {
				type: "hidden",
				...register("type")
			}),
			/* @__PURE__ */ (0, import_jsx_runtime.jsx)("input", {
				type: "hidden",
				...register("entity_type")
			}),
			/* @__PURE__ */ (0, import_jsx_runtime.jsx)("input", {
				type: "hidden",
				...register("room_id")
			}),
			/* @__PURE__ */ (0, import_jsx_runtime.jsx)("input", {
				type: "hidden",
				...register("entities_page_id")
			}),
			/* @__PURE__ */ (0, import_jsx_runtime.jsx)("input", {
				type: "hidden",
				...register("room_view_position")
			}),
			/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
				className: "w-full",
				children: [
					/* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
						className: "flex justify-center mb-4 duration-500 transition-transform ease-linear",
						children: /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
							className: "w-full",
							children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("label", {
								htmlFor: "add_new_switch_name",
								className: "block mb-2 text-sm font-medium",
								children: "Name"
							}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
								className: "flex flex-row-reverse",
								children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)("input", {
									className: "outline-none bg-base-300 text-base-content border-neutral rounded-md border focus:ring-0 focus:border-accent block flex-1 min-w-0 w-full text-sm p-2.5 peer/search_text",
									type: "text",
									id: "add_new_switch_name",
									...register("friendly_name"),
									required: true
								})
							})]
						})
					}),
					/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
						className: "mb-4",
						children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("label", {
							className: "block mb-2 text-sm font-medium",
							children: "Step size"
						}), /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
							className: "join w-full",
							children: [
								/* @__PURE__ */ (0, import_jsx_runtime.jsx)("button", {
									type: "button",
									onClick: () => {
										if (getValues("step_size") > 10) setValue("step_size", 10);
										else if (getValues("step_size") > .1) setValue("step_size", Math.round(getValues("step_size") * 10 - 1) / 10);
										else setValue("step_size", .1);
									},
									className: "btn h-auto rounded-l-md border-neutral border border-r-0 ",
									children: "-"
								}),
								/* @__PURE__ */ (0, import_jsx_runtime.jsx)("input", {
									type: "number",
									className: "join-item number-apperance-none outline-none bg-base-300 text-base-content border-neutral border focus:ring-0 focus:border-accent block min-w-0 w-full text-sm p-2.5",
									step: "0.1",
									min: "0.1",
									max: "10",
									...register("step_size")
								}),
								/* @__PURE__ */ (0, import_jsx_runtime.jsx)("button", {
									type: "button",
									onClick: () => {
										if (getValues("step_size") < .1) setValue("step_size", .1);
										else if (getValues("step_size") < 10) setValue("step_size", Math.round(getValues("step_size") * 10 + 1) / 10);
										else setValue("step_size", 10);
									},
									className: "btn h-auto rounded-r-md border-neutral border border-l-0",
									children: "+"
								})
							]
						})]
					}),
					controller == "home_assistant" && /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
						className: "mb-4",
						children: [/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", { children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("label", {
							className: "block mb-2 text-sm font-medium",
							children: "Home Assistant entity"
						}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)(StateManagedSelect$1, {
							options: useAvailableEntitiesStore.getState().home_assistant_options.filter((option) => option.value.startsWith("climate.")),
							classNames,
							onChange: (newValue) => {
								setValue("home_assistant_name", newValue ? newValue.value : "", {
									shouldValidate: true,
									shouldDirty: true
								});
							},
							unstyled: true,
							components: select_components$1,
							value: useAvailableEntitiesStore.getState().home_assistant_options.find((option) => option.value === getValues("home_assistant_name")),
							styles: {
								input: (base) => ({
									...base,
									"input:focus": { boxShadow: "none" }
								}),
								multiValueLabel: (base) => ({
									...base,
									whiteSpace: "normal",
									overflow: "visible"
								}),
								control: (base) => ({
									...base,
									transition: "none"
								})
							}
						})] }), /* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
							className: "mt-1",
							children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)("button", {
								type: "button",
								onClick: loadHaEntitySettings,
								className: "btn btn-xs w-full btn-info",
								children: "Load settings from HA entity"
							})
						})]
					}),
					controller == "openhab" && /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
						className: "mt-4 border border-primary rounded-box p-4 pt-2",
						children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", {
							className: "text-lg mb-4",
							children: "Temperature"
						}), /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
							className: "",
							children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("label", {
								className: "block mb-2 text-sm font-medium",
								children: "OpenHAB target temperature item"
							}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)(StateManagedSelect$1, {
								options: useAvailableEntitiesStore.getState().openhab_options,
								classNames,
								onChange: (newValue) => setValue("openhab_temperature_item", newValue ? newValue.value : ""),
								value: useAvailableEntitiesStore.getState().openhab_options.find((option) => option.value === getValues("openhab_temperature_item")),
								unstyled: true,
								components: select_components$1,
								styles: {
									input: (base) => ({
										...base,
										"input:focus": { boxShadow: "none" }
									}),
									multiValueLabel: (base) => ({
										...base,
										whiteSpace: "normal",
										overflow: "visible"
									}),
									control: (base) => ({
										...base,
										transition: "none"
									})
								}
							})]
						})]
					}),
					/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
						className: "mt-4 border border-primary rounded-box p-4 pt-2",
						children: [
							/* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", {
								className: "text-lg mb-4",
								children: "Fan modes"
							}),
							controller == "openhab" && /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
								className: "mb-4",
								children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("label", {
									className: "block mb-2 text-sm font-medium",
									children: "OpenHAB fan mode item"
								}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)(StateManagedSelect$1, {
									options: useAvailableEntitiesStore.getState().openhab_options,
									classNames,
									onChange: (newValue) => setValue("openhab_fan_mode_item", newValue ? newValue.value : ""),
									value: useAvailableEntitiesStore.getState().openhab_options.find((option) => option.value === getValues("openhab_fan_mode_item")),
									unstyled: true,
									components: select_components$1,
									styles: {
										input: (base) => ({
											...base,
											"input:focus": { boxShadow: "none" }
										}),
										multiValueLabel: (base) => ({
											...base,
											whiteSpace: "normal",
											overflow: "visible"
										}),
										control: (base) => ({
											...base,
											transition: "none"
										})
									}
								})]
							}),
							/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
								id: `fan_mode_options`,
								children: [/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
									className: "grid grid-cols-[1fr_1fr_3rem_3rem] items-stretch justify-center w-full mb-1 gap-1 mt-2",
									children: [
										/* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", {
											className: "text-sm ml-2",
											children: "Label"
										}),
										/* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", {
											className: "text-sm ml-2",
											children: "Value"
										}),
										/* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", {
											className: "text-sm ml-2",
											children: "Icon"
										}),
										/* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", { className: "text-sm ml-2" }),
										(() => {
											const elements = [];
											fanModeFields.forEach((field, index) => {
												elements.push(/* @__PURE__ */ (0, import_jsx_runtime.jsx)("input", {
													type: "text",
													...register(`fan_modes.${index}.label`),
													className: "rounded-box outline-none bg-base-300 text-base-content border-neutral border focus:ring-0 focus:border-accent block flex-1 min-w-0 w-full text-sm p-2.5",
													title: "Label"
												}));
												elements.push(/* @__PURE__ */ (0, import_jsx_runtime.jsx)("input", {
													type: "text",
													...register(`fan_modes.${index}.value`),
													className: "rounded-box outline-none bg-base-300 text-base-content border-neutral border focus:ring-0 focus:border-accent block flex-1 min-w-0 w-full text-sm p-2.5",
													title: "Value"
												}));
												elements.push(/* @__PURE__ */ (0, import_jsx_runtime.jsx)(IconSelector, {
													value: field.icon,
													onChange: (icon) => setValue(`fan_modes.${index}.icon`, icon)
												}));
												elements.push(/* @__PURE__ */ (0, import_jsx_runtime.jsx)("button", {
													type: "button",
													className: "btn h-auto rounded-box border border-neutral border-l-0 text-base-content bg-error/20 hover:bg-error/70 join-item ring-0 inset-ring-0 focus:ring-0",
													title: "Remove fan mode",
													onClick: () => removeFanModeField(index),
													children: "✕"
												}));
											});
											return elements;
										})()
									]
								}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)("button", {
									type: "button",
									className: "btn btn-xs w-full btn-success",
									title: "Add fan mode",
									onClick: () => appendFanModeField({
										icon: "",
										label: "",
										value: ""
									}),
									children: "Add fan mode"
								})]
							})
						]
					}),
					/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
						className: "mt-4 border border-primary rounded-box p-4 pt-2",
						children: [
							/* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", {
								className: "input_field_label block mb-2 text-sm font-medium",
								children: "HVAC modes"
							}),
							controller == "openhab" && /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
								className: "mb-4",
								children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("label", {
									className: "block mb-2 text-sm font-medium",
									children: "OpenHAB HVAC mode item"
								}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)(StateManagedSelect$1, {
									options: useAvailableEntitiesStore.getState().openhab_options,
									classNames,
									onChange: (newValue) => setValue("openhab_hvac_mode_item", newValue ? newValue.value : ""),
									value: useAvailableEntitiesStore.getState().openhab_options.find((option) => option.value === getValues("openhab_hvac_mode_item")),
									unstyled: true,
									components: select_components$1,
									styles: {
										input: (base) => ({
											...base,
											"input:focus": { boxShadow: "none" }
										}),
										multiValueLabel: (base) => ({
											...base,
											whiteSpace: "normal",
											overflow: "visible"
										}),
										control: (base) => ({
											...base,
											transition: "none"
										})
									}
								})]
							}),
							/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
								id: `hvac_mode_options`,
								children: [/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
									className: "grid grid-cols-[1fr_1fr_3rem_3rem] items-stretch justify-center w-full mb-1 gap-1 mt-2",
									children: [
										/* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", {
											className: "text-sm ml-2",
											children: "Label"
										}),
										/* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", {
											className: "text-sm ml-2",
											children: "Value"
										}),
										/* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", {
											className: "text-sm ml-2",
											children: "Icon"
										}),
										/* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", { className: "text-sm ml-2" }),
										(() => {
											const elements = [];
											hvacModeFields.forEach((field, index) => {
												elements.push(/* @__PURE__ */ (0, import_jsx_runtime.jsx)("input", {
													type: "text",
													...register(`hvac_modes.${index}.label`),
													className: "rounded-box outline-none bg-base-300 text-base-content border-neutral border focus:ring-0 focus:border-accent block flex-1 min-w-0 w-full text-sm p-2.5",
													title: "Label"
												}));
												elements.push(/* @__PURE__ */ (0, import_jsx_runtime.jsx)("input", {
													type: "text",
													...register(`hvac_modes.${index}.value`),
													className: "rounded-box outline-none bg-base-300 text-base-content border-neutral border focus:ring-0 focus:border-accent block flex-1 min-w-0 w-full text-sm p-2.5",
													title: "Value"
												}));
												elements.push(/* @__PURE__ */ (0, import_jsx_runtime.jsx)(IconSelector, {
													value: field.icon,
													onChange: (icon) => setValue(`hvac_modes.${index}.icon`, icon)
												}));
												elements.push(/* @__PURE__ */ (0, import_jsx_runtime.jsx)("button", {
													type: "button",
													className: "btn h-auto rounded-box border border-neutral border-l-0 text-base-content bg-error/20 hover:bg-error/70 join-item ring-0 inset-ring-0 focus:ring-0",
													title: "Remove HVAC mode",
													onClick: () => removeHvacModeField(index),
													children: "✕"
												}));
											});
											return elements;
										})()
									]
								}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)("button", {
									type: "button",
									className: "btn btn-xs w-full btn-success",
									title: "Add HVAC mode",
									onClick: () => appendHvacModeField({
										icon: "",
										label: "",
										value: ""
									}),
									children: "Add HVAC mode"
								})]
							})
						]
					}),
					/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
						className: "mt-4 border border-primary rounded-box p-4 pt-2",
						children: [
							/* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", {
								className: "input_field_label block mb-2 text-sm font-medium",
								children: "Presets"
							}),
							controller == "openhab" && /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
								className: "mb-4",
								children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("label", {
									className: "block mb-2 text-sm font-medium",
									children: "OpenHAB preset item"
								}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)(StateManagedSelect$1, {
									options: useAvailableEntitiesStore.getState().openhab_options,
									classNames,
									onChange: (newValue) => setValue("openhab_preset_mode_item", newValue ? newValue.value : ""),
									value: useAvailableEntitiesStore.getState().openhab_options.find((option) => option.value === getValues("openhab_preset_mode_item")),
									unstyled: true,
									components: select_components$1,
									styles: {
										input: (base) => ({
											...base,
											"input:focus": { boxShadow: "none" }
										}),
										multiValueLabel: (base) => ({
											...base,
											whiteSpace: "normal",
											overflow: "visible"
										}),
										control: (base) => ({
											...base,
											transition: "none"
										})
									}
								})]
							}),
							/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
								id: `preset_mode_options`,
								children: [/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
									className: "grid grid-cols-[1fr_1fr_3rem_3rem] items-stretch justify-center w-full mb-1 gap-1 mt-2",
									children: [
										/* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", {
											className: "text-sm ml-2",
											children: "Label"
										}),
										/* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", {
											className: "text-sm ml-2",
											children: "Value"
										}),
										/* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", {
											className: "text-sm ml-2",
											children: "Icon"
										}),
										/* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", { className: "text-sm ml-2" }),
										(() => {
											const elements = [];
											presetModeFields.forEach((field, index) => {
												elements.push(/* @__PURE__ */ (0, import_jsx_runtime.jsx)("input", {
													type: "text",
													...register(`preset_modes.${index}.label`),
													className: "rounded-box outline-none bg-base-300 text-base-content border-neutral border focus:ring-0 focus:border-accent block flex-1 min-w-0 w-full text-sm p-2.5",
													title: "Label"
												}));
												elements.push(/* @__PURE__ */ (0, import_jsx_runtime.jsx)("input", {
													type: "text",
													...register(`preset_modes.${index}.value`),
													className: "rounded-box outline-none bg-base-300 text-base-content border-neutral border focus:ring-0 focus:border-accent block flex-1 min-w-0 w-full text-sm p-2.5",
													title: "Value"
												}));
												elements.push(/* @__PURE__ */ (0, import_jsx_runtime.jsx)(IconSelector, {
													value: field.icon,
													onChange: (icon) => setValue(`preset_modes.${index}.icon`, icon)
												}));
												elements.push(/* @__PURE__ */ (0, import_jsx_runtime.jsx)("button", {
													type: "button",
													className: "btn h-auto rounded-box border border-neutral border-l-0 text-base-content bg-error/20 hover:bg-error/70 join-item ring-0 inset-ring-0 focus:ring-0",
													title: "Remove preset mode",
													onClick: () => removePresetModeField(index),
													children: "✕"
												}));
											});
											return elements;
										})()
									]
								}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)("button", {
									type: "button",
									className: "btn btn-xs w-full btn-success",
									title: "Add preset mode",
									onClick: () => appendPresetModeField({
										icon: "",
										label: "",
										value: ""
									}),
									children: "Add preset"
								})]
							})
						]
					}),
					/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
						className: "mt-4 border border-primary rounded-box p-4 pt-2",
						children: [
							/* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", {
								className: "input_field_label block mb-2 text-sm font-medium",
								children: "Swing modes"
							}),
							controller == "openhab" && /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
								className: "mb-4",
								children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("label", {
									className: "block mb-2 text-sm font-medium",
									children: "OpenHAB swing item"
								}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)(StateManagedSelect$1, {
									options: useAvailableEntitiesStore.getState().openhab_options,
									classNames,
									onChange: (newValue) => setValue("openhab_swing_mode_item", newValue ? newValue.value : ""),
									value: useAvailableEntitiesStore.getState().openhab_options.find((option) => option.value === getValues("openhab_swing_mode_item")),
									unstyled: true,
									components: select_components$1,
									styles: {
										input: (base) => ({
											...base,
											"input:focus": { boxShadow: "none" }
										}),
										multiValueLabel: (base) => ({
											...base,
											whiteSpace: "normal",
											overflow: "visible"
										}),
										control: (base) => ({
											...base,
											transition: "none"
										})
									}
								})]
							}),
							/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
								id: `swing_mode_options`,
								children: [
									/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
										className: "grid grid-cols-[1fr_1fr_3rem_3rem] items-stretch justify-center w-full mb-1 gap-1 mt-2",
										children: [
											/* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", {
												className: "text-sm ml-2",
												children: "Label"
											}),
											/* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", {
												className: "text-sm ml-2",
												children: "Value"
											}),
											/* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", {
												className: "text-sm ml-2",
												children: "Icon"
											}),
											/* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", { className: "text-sm ml-2" }),
											(() => {
												const elements = [];
												swingModeFields.forEach((field, index) => {
													elements.push(/* @__PURE__ */ (0, import_jsx_runtime.jsx)("input", {
														type: "text",
														...register(`swing_modes.${index}.label`),
														className: "rounded-box outline-none bg-base-300 text-base-content border-neutral border focus:ring-0 focus:border-accent block flex-1 min-w-0 w-full text-sm p-2.5",
														title: "Label"
													}));
													elements.push(/* @__PURE__ */ (0, import_jsx_runtime.jsx)("input", {
														type: "text",
														...register(`swing_modes.${index}.value`),
														className: "rounded-box outline-none bg-base-300 text-base-content border-neutral border focus:ring-0 focus:border-accent block flex-1 min-w-0 w-full text-sm p-2.5",
														title: "Value"
													}));
													elements.push(/* @__PURE__ */ (0, import_jsx_runtime.jsx)(IconSelector, {
														value: field.icon,
														onChange: (icon) => setValue(`swing_modes.${index}.icon`, icon)
													}));
													elements.push(/* @__PURE__ */ (0, import_jsx_runtime.jsx)("button", {
														type: "button",
														className: "btn h-auto rounded-box border border-neutral border-l-0 text-base-content bg-error/20 hover:bg-error/70 join-item ring-0 inset-ring-0 focus:ring-0",
														title: "Remove swing mode",
														onClick: () => removeSwingModeField(index),
														children: "✕"
													}));
												});
												return elements;
											})()
										]
									}),
									"min-content",
									/* @__PURE__ */ (0, import_jsx_runtime.jsx)("button", {
										type: "button",
										className: "btn btn-xs w-full btn-success",
										title: "Add swing mode",
										onClick: () => appendSwingModeField({
											icon: "",
											label: "",
											value: ""
										}),
										children: "Add swing mode"
									})
								]
							})
						]
					}),
					/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
						className: "mt-4 border border-primary rounded-box p-4 pt-2",
						children: [
							/* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", {
								className: "input_field_label block mb-2 text-sm font-medium",
								children: "Horizontal swing modes"
							}),
							controller == "openhab" && /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
								className: "mb-4",
								children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("label", {
									className: "block mb-2 text-sm font-medium",
									children: "OpenHAB horizontal swing item"
								}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)(StateManagedSelect$1, {
									options: useAvailableEntitiesStore.getState().openhab_options,
									classNames,
									onChange: (newValue) => setValue("openhab_swingh_mode_item", newValue ? newValue.value : ""),
									value: useAvailableEntitiesStore.getState().openhab_options.find((option) => option.value === getValues("openhab_swingh_mode_item")),
									unstyled: true,
									components: select_components$1,
									styles: {
										input: (base) => ({
											...base,
											"input:focus": { boxShadow: "none" }
										}),
										multiValueLabel: (base) => ({
											...base,
											whiteSpace: "normal",
											overflow: "visible"
										}),
										control: (base) => ({
											...base,
											transition: "none"
										})
									}
								})]
							}),
							/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
								id: `swing_mode_options`,
								children: [/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
									className: "grid grid-cols-[1fr_1fr_3rem_3rem] items-stretch justify-center w-full mb-1 gap-1 mt-2",
									children: [
										/* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", {
											className: "text-sm ml-2",
											children: "Label"
										}),
										/* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", {
											className: "text-sm ml-2",
											children: "Value"
										}),
										/* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", {
											className: "text-sm ml-2",
											children: "Icon"
										}),
										/* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", { className: "text-sm ml-2" }),
										(() => {
											const elements = [];
											swinghModeFields.forEach((field, index) => {
												elements.push(/* @__PURE__ */ (0, import_jsx_runtime.jsx)("input", {
													type: "text",
													...register(`swingh_modes.${index}.label`),
													className: "rounded-box outline-none bg-base-300 text-base-content border-neutral border focus:ring-0 focus:border-accent block flex-1 min-w-0 w-full text-sm p-2.5",
													title: "Label"
												}));
												elements.push(/* @__PURE__ */ (0, import_jsx_runtime.jsx)("input", {
													type: "text",
													...register(`swingh_modes.${index}.value`),
													className: "rounded-box outline-none bg-base-300 text-base-content border-neutral border focus:ring-0 focus:border-accent block flex-1 min-w-0 w-full text-sm p-2.5",
													title: "Value"
												}));
												elements.push(/* @__PURE__ */ (0, import_jsx_runtime.jsx)(IconSelector, {
													value: field.icon,
													onChange: (icon) => setValue(`swingh_modes.${index}.icon`, icon)
												}));
												elements.push(/* @__PURE__ */ (0, import_jsx_runtime.jsx)("button", {
													type: "button",
													className: "btn h-auto rounded-box border border-neutral border-l-0 text-base-content bg-error/20 hover:bg-error/70 join-item ring-0 inset-ring-0 focus:ring-0",
													title: "Remove horizontal swing mode",
													onClick: () => removeSwinghModeField(index),
													children: "✕"
												}));
											});
											return elements;
										})()
									]
								}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)("button", {
									type: "button",
									className: "btn btn-xs w-full btn-success",
									title: "Add horizontal swing mode",
									onClick: () => appendSwinghModeField({
										icon: "",
										label: "",
										value: ""
									}),
									children: "Add horizontal swing mode"
								})]
							})
						]
					})
				]
			}),
			/* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
				className: "flex mt-4 justify-end items-center",
				children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)("button", {
					type: "submit",
					className: "btn btn-info",
					disabled: !isValid,
					children: "Save"
				})
			})
		]
	});
};
//#endregion
//#region src/components/MultiStep_AddEditEntity/entity_types/step3_edit_scene.tsx
var schema = object({
	id: number().nullable(),
	type: string(),
	controller: string(),
	scene_type: string(),
	room_id: number().nullable(),
	entities_page_id: number(),
	room_view_position: number(),
	friendly_name: string().min(1),
	backend_name: string().optional()
});
var CustomOption = ({ innerProps, isDisabled, isFocused, isSelected, children, data }) => {
	if (isDisabled) return null;
	return /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
		...innerProps,
		className: `p-1 ${isSelected ? "bg-primary/20 rounded-sm" : ""} ${isFocused ? "bg-primary/20 rounded-sm" : ""}`,
		children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", { className: `mdi ${data.icon} me-2` }), children]
	});
};
var select_components = { Option: CustomOption };
var MultiStep_AddEditEntity_Step3_Scene = ({ controller, room_id, entities_page_id, room_view_position, id, onComplete }) => {
	const { handleSubmit, register, reset, getValues, setValue, formState: { isValid } } = useForm({
		resolver: u(schema),
		defaultValues: {
			id: id ?? null,
			type: "scene",
			scene_type: "",
			controller,
			room_id,
			entities_page_id,
			room_view_position,
			friendly_name: "",
			backend_name: ""
		}
	});
	if (controller == "home_assistant") setValue("scene_type", "home_assistant");
	else if (controller == "openhab") setValue("scene_type", "openhab");
	else if (controller == "nspm") setValue("scene_type", "nspm_scene");
	function getCookie(name) {
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
	(0, import_react.useEffect)(() => {
		if (id != null) {
			const sceneData = useEntitiesPagesStore.getState().scenes.find((scene) => scene.id == id);
			reset(sceneData);
		}
	}, [id, useEntitiesPagesStore.getState().scenes]);
	function saveEntity(data) {
		fetch("/rest/entities/scenes", {
			credentials: "same-origin",
			method: "PUT",
			mode: "same-origin",
			headers: {
				"Content-Type": "application/json",
				"X-CSRFToken": getCookie("csrftoken")
			},
			body: JSON.stringify(data)
		}).then(async (response) => {
			const data = await response.json();
			if (!response.ok) {
				const error = data && data.message || response.status;
				return Promise.reject(error);
			}
			onComplete?.();
		}).catch((error) => {
			Notify({
				message: `Error saving entity. Error: ${error}`,
				level: "error",
				duration: 1e4
			});
		});
	}
	const classNames = {
		control: (state) => `${state.isFocused ? "border" : "border-0"} border-accent p-2.5 text-sm rounded-box bg-base-300 rounded-md`,
		menu: () => "bg-base-300 p-2.5 rounded-box",
		option: (state) => `p-1 ${state.isSelected ? "bg-primary/20 rounded-sm" : ""} ${state.isFocused ? "bg-primary/20 rounded-sm" : ""}`
	};
	return /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("form", {
		onSubmit: handleSubmit(saveEntity),
		children: [
			/* @__PURE__ */ (0, import_jsx_runtime.jsx)("input", {
				type: "hidden",
				...register("id")
			}),
			/* @__PURE__ */ (0, import_jsx_runtime.jsx)("input", {
				type: "hidden",
				...register("type")
			}),
			/* @__PURE__ */ (0, import_jsx_runtime.jsx)("input", {
				type: "hidden",
				...register("scene_type")
			}),
			/* @__PURE__ */ (0, import_jsx_runtime.jsx)("input", {
				type: "hidden",
				...register("room_id")
			}),
			/* @__PURE__ */ (0, import_jsx_runtime.jsx)("input", {
				type: "hidden",
				...register("entities_page_id")
			}),
			/* @__PURE__ */ (0, import_jsx_runtime.jsx)("input", {
				type: "hidden",
				...register("room_view_position")
			}),
			/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
				className: "w-full",
				children: [
					/* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
						className: "flex justify-center mb-4 duration-500 transition-transform ease-linear",
						children: /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
							className: "w-full",
							children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("label", {
								htmlFor: "add_new_switch_name",
								className: "block mb-2 text-sm font-medium",
								children: "Name"
							}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
								className: "flex flex-row-reverse",
								children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)("input", {
									className: "outline-none bg-base-300 border-neutral rounded-md border focus:ring-0 focus:border-accent block flex-1 min-w-0 w-full text-sm p-2.5 peer/search_text",
									type: "text",
									...register("friendly_name"),
									required: true
								})
							})]
						})
					}),
					controller == "home_assistant" && /* @__PURE__ */ (0, import_jsx_runtime.jsxs)(import_jsx_runtime.Fragment, { children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("label", {
						className: "block mb-2 text-sm font-medium",
						children: "Home Assistant scene"
					}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)(StateManagedSelect$1, {
						options: useAvailableEntitiesStore.getState().home_assistant_options.filter((option) => option.value.startsWith("scene.") || option.value.startsWith("script.")),
						classNames,
						onChange: (newValue) => {
							setValue("backend_name", newValue ? newValue.value : "");
						},
						unstyled: true,
						components: select_components,
						value: useAvailableEntitiesStore.getState().home_assistant_options.find((option) => option.value === getValues("backend_name")),
						styles: {
							input: (base) => ({
								...base,
								"input:focus": { boxShadow: "none" }
							}),
							multiValueLabel: (base) => ({
								...base,
								whiteSpace: "normal",
								overflow: "visible"
							}),
							control: (base) => ({
								...base,
								transition: "none"
							})
						}
					})] }),
					controller == "openhab" && /* @__PURE__ */ (0, import_jsx_runtime.jsxs)(import_jsx_runtime.Fragment, { children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("label", {
						className: "block mb-2 mt-4 text-sm font-medium",
						children: "OpenHAB item"
					}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)(StateManagedSelect$1, {
						options: useAvailableEntitiesStore.getState().openhab_options,
						classNames,
						onChange: (newValue) => setValue("backend_name", newValue ? newValue.value : ""),
						value: useAvailableEntitiesStore.getState().openhab_options.find((option) => option.value === getValues("backend_name")),
						unstyled: true,
						components: select_components,
						styles: {
							input: (base) => ({
								...base,
								"input:focus": { boxShadow: "none" }
							}),
							multiValueLabel: (base) => ({
								...base,
								whiteSpace: "normal",
								overflow: "visible"
							}),
							control: (base) => ({
								...base,
								transition: "none"
							})
						}
					})] })
				]
			}),
			/* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
				className: "flex mt-4 justify-end items-center",
				children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)("button", {
					type: "submit",
					className: "btn btn-info",
					disabled: !isValid,
					children: "Save"
				})
			})
		]
	});
};
//#endregion
//#region src/components/MultiStep_AddEditEntity/MultiStep_AddEditEntity.tsx
var MultiStep_AddEditEntity_exports = /* @__PURE__ */ __exportAll({ default: () => MultiStep_AddOrEditEntity });
var MultiStep_AddOrEditEntity = ({ room_id, entities_page_id, room_view_position, type, id, onComplete, onClose, opened, setOpened }) => {
	const [step, setStep] = (0, import_react.useState)(1);
	const [formData, setFormData] = (0, import_react.useState)({
		room_id,
		entities_page_id,
		room_view_position,
		controller: "",
		type,
		entity_type: "",
		friendly_name: "",
		id: null
	});
	const [entitiesPageType, setEntitiesPageType] = (0, import_react.useState)("");
	(0, import_react.useEffect)(() => {
		if (useAvailableEntitiesStore.getState().status == "none") useAvailableEntitiesStore.getState().fetchEntities();
		if (useSettingsStore.getState().status == "none") useSettingsStore.getState().fetchData();
	}, []);
	const nextStep = () => setStep((prev) => prev + 1);
	const dialogRef = (0, import_react.useRef)(null);
	const contentRef = (0, import_react.useRef)(null);
	const handleButtonSelectEvent = (input, value) => () => {
		setFormData({
			...formData,
			[input]: value
		});
	};
	(0, import_react.useEffect)(() => {
		if (id != null) {
			const entityData = useEntitiesPagesStore.getState().entities.find((entity) => entity.id == id);
			setFormData({
				...formData,
				...entityData
			});
			setStep(3);
		}
		const entityPage = useEntitiesPagesStore.getState().entities_pages.find((page) => page.id == entities_page_id);
		setEntitiesPageType(entityPage?.type ?? "");
	}, [
		id,
		useEntitiesPagesStore.getState().entities,
		useEntitiesPagesStore.getState().entities_pages
	]);
	const onDialogClose = () => {
		dialogRef.current?.close();
		setTimeout(() => {
			setOpened(false);
			if (onClose) onClose();
			if (id != null) {
				const entityData = useEntitiesPagesStore.getState().entities.find((entity) => entity.id == id);
				setFormData({
					...formData,
					...entityData
				});
			}
			setStep(1);
		}, 220);
	};
	const checkOverflow = () => {
		if (contentRef.current) {
			const maxHeightString = window.getComputedStyle(contentRef.current).maxHeight;
			let maxHeight = parseFloat(maxHeightString);
			if (maxHeightString.endsWith("%")) maxHeight = Math.trunc(window.innerHeight * maxHeight / 100);
			contentRef.current.classList.toggle("overflow-y-auto", contentRef.current.scrollHeight > maxHeight);
			contentRef.current.classList.toggle("overflow-y-visible", contentRef.current.scrollHeight <= maxHeight);
		}
	};
	(0, import_react.useEffect)(() => {
		const observer = new ResizeObserver((entries) => {
			entries.forEach(() => {
				checkOverflow();
			});
		});
		if (contentRef.current) observer.observe(contentRef.current);
	}, [contentRef]);
	(0, import_react.useEffect)(() => {
		if (id == null && step == 1 && entitiesPageType == "scene") {
			setFormData({
				...formData,
				entity_type: "scene"
			});
			setStep(2);
		} else if (id != null && entitiesPageType == "scene") {
			setFormData({
				...formData,
				entity_type: "scene"
			});
			setStep(3);
		}
	}, [
		id,
		step,
		entitiesPageType
	]);
	return /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("dialog", {
		ref: dialogRef,
		open: opened ?? false,
		onClose: onDialogClose,
		className: "modal duration-200",
		children: [/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
			ref: contentRef,
			className: `modal-box max-h-11/12 bg-base-100 text-base-content`,
			children: [
				/* @__PURE__ */ (0, import_jsx_runtime.jsx)("button", {
					className: "btn btn-sm btn-circle btn-ghost absolute right-2 top-2",
					onClick: () => {
						setOpened(false);
						onDialogClose();
					},
					children: "✕"
				}),
				(() => {
					return /* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
						className: "flex justify-center mb-4 w-full",
						children: /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("ul", {
							className: "steps w-full",
							children: [
								id == null && /* @__PURE__ */ (0, import_jsx_runtime.jsx)("li", {
									className: `step ${step >= 1 ? "step-primary cursor-pointer" : ""}`,
									onClick: () => {
										setStep(1);
									},
									children: "Type"
								}),
								id == null && /* @__PURE__ */ (0, import_jsx_runtime.jsx)("li", {
									className: `step ${step >= 2 ? "step-primary" : ""} ${formData.entity_type == "" ? "cursor-not-allowed" : "cursor-pointer"}`,
									onClick: () => {
										if (formData.entity_type != "") setStep(2);
									},
									children: "Controller"
								}),
								/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("li", {
									className: `step ${step >= 3 ? "step-primary" : ""} ${formData.entity_type == "" ? "cursor-not-allowed" : "cursor-pointer"}`,
									onClick: () => {
										if (formData.entity_type != "") setStep(3);
									},
									children: ["Configure ", formData.entity_type]
								})
							]
						})
					});
				})(),
				/* @__PURE__ */ (0, import_jsx_runtime.jsx)(AnimatePresence, {
					mode: "wait",
					children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)(motion.div, {
						layout: true,
						initial: {
							opacity: 0,
							x: 30
						},
						animate: {
							opacity: 1,
							x: 0
						},
						exit: {
							opacity: 0,
							x: -30
						},
						transition: { duration: .1 },
						children: (() => {
							if (id == null && step == 1 && entitiesPageType == "entity") return /* @__PURE__ */ (0, import_jsx_runtime.jsx)(MultiStep_AddEditEntity_Step1, {
								handleButtonSelectEvent,
								next_step: nextStep
							});
							if (id == null && step == 2) return /* @__PURE__ */ (0, import_jsx_runtime.jsx)(MultiStep_AddEditEntity_Step2, {
								handleButtonSelectEvent,
								entity_type: String(formData.entity_type),
								next_step: nextStep
							});
							else if (formData.entity_type != "") switch (formData.entity_type) {
								case "light": return /* @__PURE__ */ (0, import_jsx_runtime.jsx)(MultiStep_AddEditEntity_Step3_Light, {
									controller: String(formData.controller),
									room_id,
									entities_page_id,
									room_view_position,
									id,
									onComplete
								});
								case "switch": return /* @__PURE__ */ (0, import_jsx_runtime.jsx)(MultiStep_AddEditEntity_Step3_Switch, {
									controller: String(formData.controller),
									room_id,
									id,
									entities_page_id,
									room_view_position,
									onComplete
								});
								case "button": return /* @__PURE__ */ (0, import_jsx_runtime.jsx)(MultiStep_AddEditEntity_Step3_Button, {
									controller: String(formData.controller),
									room_id,
									id,
									entities_page_id,
									room_view_position,
									onComplete
								});
								case "thermostat": return /* @__PURE__ */ (0, import_jsx_runtime.jsx)(MultiStep_AddEditEntity_Step3_Thermostat, {
									controller: String(formData.controller),
									room_id,
									id,
									entities_page_id,
									room_view_position,
									onComplete
								});
								case "scene": return /* @__PURE__ */ (0, import_jsx_runtime.jsx)(MultiStep_AddEditEntity_Step3_Scene, {
									controller: String(formData.controller),
									room_id,
									id,
									entities_page_id,
									room_view_position,
									onComplete
								});
								default:
									Notify({
										message: `Unknown entity type while trying to edit entity. Type: ${formData.entity_type}`,
										level: "error",
										duration: 1e4
									});
									return /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("span", {
										className: "text-lg text-error",
										children: ["Unknown entity type: ", formData.entity_type]
									});
							}
						})()
					}, `step.${step}`)
				})
			]
		}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)("form", {
			method: "dialog",
			className: "modal-backdrop",
			children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)("button", {
				onClick: () => {
					setOpened(false);
					onDialogClose();
				},
				children: "close"
			})
		})]
	});
};
//#endregion
export { MultiStep_AddOrEditEntity as n, useEntitiesPagesStore as r, MultiStep_AddEditEntity_exports as t };
