import { c as __toESM, r as require_react, t as require_jsx_runtime } from "./main-BELnHkx2.js";
import { a as useWatch, i as useForm, r as useAvailableEntitiesStore } from "./MultiStep_AddEditEntity-D6sSc-s2.js";
import { t as StateManagedSelect$1 } from "./react-select.esm-doppudd5.js";
import { n as Notify } from "./NSPanelToastContainer-CSal8dJj.js";
import EntitiesPagesView from "./EntitiesPagesView-CFt3I69O.js";
import { t as useStompStore } from "./StompStore-CHy7gCAt.js";
import { t as useEntityStatesStore } from "./EntityStore-DZJg_-pl.js";
import { b as require_Icon, u as mdiTextShort } from "./mdi-CuGYvFfv.js";
//#region src/components/RoomPage/RoomPage.tsx
var import_react = /* @__PURE__ */ __toESM(require_react(), 1);
var import_Icon = require_Icon();
var import_jsx_runtime = require_jsx_runtime();
function RoomPage({ room_id }) {
	const { status: stompStatus } = useStompStore();
	const { status: entityStatus, rooms } = useEntityStatesStore();
	const home_assistant_options = useAvailableEntitiesStore((s) => s.home_assistant_options);
	const openhab_options = useAvailableEntitiesStore((s) => s.openhab_options);
	const all_options = [...home_assistant_options.filter((option) => option.value.startsWith("sensor.")), ...openhab_options];
	const room = rooms[room_id];
	const [hasLoadedValues, setHasLoadedValues] = (0, import_react.useState)(false);
	const { control, register, getValues, setValue, reset, handleSubmit } = useForm({ defaultValues: {
		id: room?.id,
		friendly_name: room?.friendly_name,
		room_temp_provider: room?.room_temp_provider,
		room_temp_sensor: room?.room_temp_sensor
	} });
	useWatch({
		control,
		name: "room_temp_sensor"
	});
	const current_friendly_name = useWatch({
		control,
		name: "friendly_name"
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
	const onSubmit = (data) => {
		fetch("/rest/rooms", {
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
			Notify({
				message: `Saved room setting successfully.`,
				level: "success",
				duration: 5e3
			});
		}).catch((error) => {
			Notify({
				message: `Error saving entity. Error: ${error}`,
				level: "error",
				duration: 1e4
			});
		});
	};
	(0, import_react.useEffect)(() => {
		if (stompStatus == "none") useStompStore.getState().connect();
	}, [stompStatus]);
	(0, import_react.useEffect)(() => {
		if (entityStatus == "none") useEntityStatesStore.getState().fetchData();
	}, [entityStatus]);
	(0, import_react.useEffect)(() => {
		if (useAvailableEntitiesStore.getState().status == "none") useAvailableEntitiesStore.getState().fetchEntities();
	}, []);
	(0, import_react.useEffect)(() => {
		if (room && !hasLoadedValues) {
			reset({
				id: room.id,
				friendly_name: room.friendly_name,
				room_temp_provider: room.room_temp_provider,
				room_temp_sensor: room.room_temp_sensor
			});
			setHasLoadedValues(true);
		}
	}, [
		room,
		reset,
		hasLoadedValues,
		setHasLoadedValues
	]);
	if (entityStatus != "loaded") return /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
		className: "flex w-full items-center justify-center text-2xl",
		children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", { className: "loading loading-spinner loading-sm mr-2" }), /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("span", {
			className: "font-medium",
			children: [
				"Loading data (",
				entityStatus,
				")..."
			]
		})]
	});
	if (room_id && !rooms[room_id]) return /* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
		className: "flex w-full items-center justify-center text-2xl",
		children: /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("span", {
			className: "font-medium text-error text-lg",
			children: [
				"Room with ID ",
				room_id,
				" not found."
			]
		})
	});
	const classNames = {
		control: (state) => `${state.isFocused ? "border" : "border-0"} border-accent p-2.5 text-sm rounded-box bg-base-300 text-base-content rounded-md`,
		menu: () => "bg-base-300 p-2.5 rounded-box text-base-content",
		option: (state) => `p-1 ${state.isSelected ? "bg-primary/20 rounded-sm" : ""} ${state.isFocused ? "bg-primary/20 rounded-sm" : ""}`
	};
	const CustomOption = ({ innerProps, isDisabled, isFocused, isSelected, children, data }) => {
		if (isDisabled) return null;
		return /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
			...innerProps,
			className: `p-1 ${isSelected ? "bg-primary/20 rounded-sm" : ""} ${isFocused ? "bg-primary/20 rounded-sm" : ""}`,
			children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", { className: `mdi ${data.icon} me-2` }), children]
		});
	};
	const select_components = { Option: CustomOption };
	return /* @__PURE__ */ (0, import_jsx_runtime.jsxs)(import_jsx_runtime.Fragment, { children: [
		/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("form", {
			onSubmit: handleSubmit(onSubmit),
			children: [/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
				className: "flex items-center justify-between",
				id: "nspanel_settings_container",
				children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", { children: /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("h2", {
					className: "font-medium text-2xl",
					id: "nspanel_name",
					children: [
						"Settings for room '",
						current_friendly_name,
						"'"
					]
				}) }), /* @__PURE__ */ (0, import_jsx_runtime.jsx)("a", {
					href: "https://nspanelmanager.com/docs/web-interface#room-page",
					target: "_blank",
					className: "btn btn-outline btn-info",
					children: "Help"
				})]
			}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
				className: "md:flex md:justify-between md:items-start mt-2",
				children: /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
					className: "p-4 bg-base-100 rounded-md shadow-lg w-full",
					children: [
						/* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", {
							className: "block mb-2 text-xl",
							children: "General"
						}),
						/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
							className: "",
							children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("label", {
								htmlFor: "friendly_name",
								className: "block mb-2 text-sm font-medium",
								children: "Name"
							}), /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
								className: "flex flex-row-reverse",
								children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("input", {
									className: "outline-none rounded-none bg-base-300 border-neutral rounded-e-md border border-l-0 focus:ring-0 focus:border-accent block flex-1 min-w-0 w-full text-sm p-2.5 peer/name",
									type: "text",
									...register("friendly_name")
								}), /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("span", {
									className: "inline-flex items-center px-3 text-sm border border-neutral rounded-e-0 rounded-s-md peer-focus/name:border-accent",
									children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)(import_Icon.Icon, {
										path: mdiTextShort,
										className: "w-4 h-4"
									}), " "]
								})]
							})]
						}),
						/* @__PURE__ */ (0, import_jsx_runtime.jsx)("input", {
							type: "hidden",
							name: "room_temp_provider",
							id: "room_temp_provider",
							readOnly: true,
							value: "{{ room.room_temp_provider }}"
						}),
						/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
							className: "my-2 w-full md:ms-1",
							children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("label", {
								className: "block mb-2 text-sm font-medium",
								children: "Room temperature sensor"
							}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)(StateManagedSelect$1, {
								options: all_options,
								classNames,
								onChange: (newValue) => {
									if (newValue) {
										setValue("room_temp_provider", newValue.type, { shouldDirty: true });
										setValue("room_temp_sensor", newValue.value, { shouldDirty: true });
									} else {
										setValue("room_temp_provider", "", { shouldDirty: true });
										setValue("room_temp_sensor", "", { shouldDirty: true });
									}
								},
								unstyled: true,
								components: select_components,
								value: all_options.find((option) => option.value === getValues("room_temp_sensor")),
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
						/* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", { className: "my-4" }),
						/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
							className: "text-right mt-4",
							children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("button", {
								className: "btn btn-error",
								disabled: Object.keys(rooms).length === 1,
								children: "Delete"
							}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)("button", {
								type: "submit",
								className: "btn btn-success",
								children: "Save"
							})]
						})
					]
				})
			})]
		}),
		/* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
			className: "md:flex md:justify-between md:items-start mt-2",
			children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
				className: "p-4 bg-base-100 rounded-md shadow-lg w-full",
				children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)(EntitiesPagesView, {
					room_id: room.id,
					type: "scene"
				})
			})
		}),
		/* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
			className: "md:flex md:justify-between md:items-start mt-2 mb-4",
			children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
				className: "p-4 bg-base-100 rounded-md shadow-lg w-full",
				children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)(EntitiesPagesView, {
					room_id: room.id,
					type: "entity"
				})
			})
		})
	] });
}
//#endregion
export { RoomPage as default };
