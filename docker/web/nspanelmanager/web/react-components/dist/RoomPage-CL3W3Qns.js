import { c as __toESM, r as require_react, t as require_jsx_runtime } from "./main-BWgDajaM.js";
import EntitiesPagesView from "./EntitiesPagesView-CFKeoTO2.js";
import { t as useStompStore } from "./StompStore-CvFAM8Xu.js";
import { t as useEntityStatesStore } from "./EntityStore-Ng4ecFrP.js";
import { b as require_Icon, u as mdiTextShort } from "./mdi-CkcpT4WR.js";
//#region src/components/RoomPage/RoomPage.tsx
var import_react = /* @__PURE__ */ __toESM(require_react(), 1);
var import_Icon = require_Icon();
var import_jsx_runtime = require_jsx_runtime();
function RoomPage({ room_id }) {
	const { status: stompStatus } = useStompStore();
	const { status: entityStatus, rooms } = useEntityStatesStore();
	(0, import_react.useEffect)(() => {
		if (stompStatus == "none") useStompStore.getState().connect();
	}, [stompStatus]);
	(0, import_react.useEffect)(() => {
		if (entityStatus == "none") useEntityStatesStore.getState().fetchData();
	}, [entityStatus]);
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
	const room = rooms[room_id];
	return /* @__PURE__ */ (0, import_jsx_runtime.jsxs)(import_jsx_runtime.Fragment, { children: [
		/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("form", {
			method: "POST",
			action: "{{ ingress_path }}{% url 'update_room_form' room_id=room.id %}",
			children: [/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
				className: "flex items-center justify-between",
				id: "nspanel_settings_container",
				children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", { children: /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("h2", {
					className: "font-medium text-2xl",
					id: "nspanel_name",
					children: [
						"Settings for room '",
						room.friendly_name,
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
									name: "friendly_name",
									value: room.friendly_name
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
								htmlFor: "room_temp_sensor",
								className: "block mb-1 text-sm font-medium",
								children: "Room temperature sensor"
							}), /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
								className: "flex flex-row-reverse",
								children: [
									/* @__PURE__ */ (0, import_jsx_runtime.jsx)("button", {
										type: "button",
										className: "btn btn-info rounded-l-none",
										"hx-get": "{{ ingress_path }}{% url 'htmx_partial_select_room_temperature_sensor_provider' room_id=room.id %}",
										"hx-swap": "none",
										children: "Select"
									}),
									/* @__PURE__ */ (0, import_jsx_runtime.jsx)("input", {
										className: "outline-none bg-base-300 border-neutral border border-l-0 focus:ring-0 focus:border-accent block flex-1 min-w-0 w-full text-sm p-2.5 peer/name",
										type: "text",
										id: "room_temp_sensor",
										name: "room_temp_sensor",
										value: "{{ room.room_temp_sensor }}",
										placeholder: "None selected",
										readOnly: true
									}),
									/* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", {
										className: "inline-flex items-center px-3 text-sm border border-neutral rounded-e-0 rounded-s-field peer-focus/name:border-accent",
										children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", { className: "mdi mdi-thermometer" })
									})
								]
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
