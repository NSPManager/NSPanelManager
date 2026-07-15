import { c as __toESM, r as require_react, t as require_jsx_runtime } from "./main-GZAbzCI4.js";
import { t as StateManagedSelect$1 } from "./react-select.esm-C6euR-0J.js";
import { n as Notify } from "./NSPanelToastContainer--s3HWviP.js";
import { t as useStompStore } from "./StompStore-CDxaNzmd.js";
import { t as useEntityStatesStore } from "./EntityStore-9eGpH2oX.js";
import { _ as mdiWifiStrength4, a as mdiMemory, c as mdiRestart, d as mdiUpload, g as mdiWifiStrength3, h as mdiWifiStrength2, l as mdiTableArrowUp, m as mdiWifiStrength1Alert, n as mdiCog, o as mdiMenuDownOutline, p as mdiWeb, s as mdiMinusCircle, u as mdiThermometer, v as mdiWifiStrengthAlertOutline, y as require_Icon } from "./mdi-BbA6p6gZ.js";
//#region src/components/IndexPage/NSPanelBox.tsx
var import_react = /* @__PURE__ */ __toESM(require_react(), 1);
var import_Icon = require_Icon();
var import_jsx_runtime = require_jsx_runtime();
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
var delete_nspanel = (nspanel_id) => {
	fetch(`/rest/nspanel/${nspanel_id}`, {
		method: "DELETE",
		headers: { "X-CSRFToken": getCookie("csrftoken") }
	}).then((response) => {
		console.log(response);
	}).catch((error) => {
		Notify({
			message: `Failed to delete NSPanel: ${error}`,
			level: "error",
			duration: 1e4
		});
	});
};
var DeleteNSPanelConfirmDialog = (0, import_react.forwardRef)(({ status, delete_nspanel_dialog_ref }, ref) => {
	return /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("dialog", {
		ref,
		className: "modal",
		children: [/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
			className: "modal-box",
			children: [
				/* @__PURE__ */ (0, import_jsx_runtime.jsx)("h3", {
					className: "text-lg font-bold",
					children: "Delete NSPanel"
				}),
				/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("p", {
					className: "py-4",
					children: [
						"Are you sure you want to delete NSPanel '",
						status.name,
						"'?"
					]
				}),
				/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
					className: "flex justify-end join",
					children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("button", {
						onClick: () => delete_nspanel_dialog_ref.current?.close(),
						className: "btn btn-neutral join-item",
						children: "Cancel"
					}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)("button", {
						onClick: () => {
							delete_nspanel_dialog_ref.current?.close();
							delete_nspanel(status.id);
						},
						className: "btn btn-error join-item",
						children: "Delete"
					})]
				})
			]
		}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)("form", {
			method: "dialog",
			className: "modal-backdrop",
			children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)("button", { children: "close" })
		})]
	});
});
var NSPanelStatusHeader = ({ state, update_progress }) => {
	if (state == "online") return /* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
		className: "min-h-1 overflow-hidden w-full transition-all ease-linear duration-300 bg-success rounded-t-field",
		id: "panel_header-${id}"
	});
	else if (state == "offline") return /* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
		className: "min-h-1 overflow-hidden w-full transition-all ease-linear duration-300 bg-error rounded-t-field",
		id: "panel_header-${id}"
	});
	else if (state == "rebooting") return /* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
		className: "min-h-1 overflow-hidden w-full bg-info rounded-t-field animate-progress-indeterminate origin-left-right",
		id: "panel_header-${id}"
	});
	else if (state == "waiting") return /* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
		className: "min-h-1 overflow-hidden w-full transition-all ease-linear duration-300 bg-info animate-pulse rounded-t-field",
		id: "panel_header-${id}"
	});
	else if (state == "updating_fw" || state == "updating_fs" || state == "updating_tft") if (update_progress == 0) return /* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
		className: "min-h-1 overflow-hidden w-full transition-all ease-linear duration-300 bg-info animate-pulse rounded-t-field",
		id: "panel_header-${id}"
	});
	else return /* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
		className: "min-h-1 overflow-hidden w-full transition-all ease-linear duration-1000 bg-info rounded-t-field",
		style: { width: `${update_progress}%` },
		id: "panel_header-${id}"
	});
	else if (state == "awaiting_accept") return /* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
		className: "min-h-1 overflow-hidden w-full transition-all ease-linear duration-300 bg-accent animate-pulse rounded-t-field",
		id: "panel_header-${id}"
	});
	else return /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
		className: "min-h-1 overflow-hidden w-full transition-all ease-linear duration-300 bg-warning text-warning-content animate-pulse rounded-t-field",
		id: "panel_header-${id}",
		children: ["Unknown state: ", state]
	});
};
var NSPanelWarnings = ({ warnings }) => {
	if (warnings.length === 0) return null;
	let highest_warning_level = 5;
	let badge_color = "status-base-content";
	warnings.forEach((warning) => {
		switch (warning.level) {
			case "error":
				highest_warning_level = 1;
				badge_color = "status-error";
				break;
			case "warning":
				if (highest_warning_level > 2) highest_warning_level = 2;
				badge_color = "status-warning";
				break;
			case "info":
				if (highest_warning_level > 3) highest_warning_level = 3;
				badge_color = "status-info";
				break;
			case "debug":
				if (highest_warning_level > 4) highest_warning_level = 4;
				badge_color = "status-base-content";
				break;
		}
	});
	return /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
		className: "dropdown dropdown-hover dropdown-end p-0 m-0",
		children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
			className: "flex items-end justify-end px-1",
			children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
				className: `status ${badge_color} status-lg`,
				tabIndex: 1,
				role: "button"
			})
		}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)("ul", {
			tabIndex: -1,
			className: "dropdown-content list backdrop-blur-md backdrop-brightness-80 border border-warning/50 w-92 rounded-box shadow-md",
			children: warnings.map((warning, index) => /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("li", {
				className: "list-row",
				children: [/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", { children: [
					warning.level == "error" && /* @__PURE__ */ (0, import_jsx_runtime.jsx)(import_Icon.Icon, {
						path: "M13,13H11V7H13M13,17H11V15H13M12,2A10,10 0 0,0 2,12A10,10 0 0,0 12,22A10,10 0 0,0 22,12A10,10 0 0,0 12,2Z",
						className: "w-4 h-4 text-error"
					}),
					warning.level == "warning" && /* @__PURE__ */ (0, import_jsx_runtime.jsx)(import_Icon.Icon, {
						path: "M13,13H11V7H13M13,17H11V15H13M12,2A10,10 0 0,0 2,12A10,10 0 0,0 12,22A10,10 0 0,0 22,12A10,10 0 0,0 12,2Z",
						className: "w-4 h-4 text-warning"
					}),
					warning.level == "info" && /* @__PURE__ */ (0, import_jsx_runtime.jsx)(import_Icon.Icon, {
						path: "M13,13H11V7H13M13,17H11V15H13M12,2A10,10 0 0,0 2,12A10,10 0 0,0 12,22A10,10 0 0,0 22,12A10,10 0 0,0 12,2Z",
						className: "w-4 h-4 text-info"
					}),
					warning.level == "debug" && /* @__PURE__ */ (0, import_jsx_runtime.jsx)(import_Icon.Icon, {
						path: "M13,13H11V7H13M13,17H11V15H13M12,2A10,10 0 0,0 2,12A10,10 0 0,0 12,22A10,10 0 0,0 22,12A10,10 0 0,0 12,2Z",
						className: "w-4 h-4 text-base-content"
					})
				] }), /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", { children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", { children: warning.text }), /* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
					className: "text-xs uppercase font-semibold opacity-60",
					children: warning.level.charAt(0).toUpperCase() + warning.level.slice(1)
				})] })]
			}, index))
		})]
	});
};
var AcceptedNSPanelContent = ({ status }) => {
	const popoverRef = (0, import_react.useRef)(null);
	const delete_nspanel_dialog_ref = (0, import_react.useRef)(null);
	return /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
		className: "p-2 relative h-full",
		children: [
			/* @__PURE__ */ (0, import_jsx_runtime.jsx)(DeleteNSPanelConfirmDialog, {
				status,
				ref: delete_nspanel_dialog_ref,
				delete_nspanel_dialog_ref
			}),
			/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
				className: "flex justify-between mx-auto",
				children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("a", {
					href: `/nspanel/${status.id}`,
					className: "font-medium text-lg",
					children: status.name
				}), /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
					className: "flex items-center justify-end",
					children: [(() => {
						if (status.state === "online") return null;
						else if (status.state === "offline") return /* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", {
							className: "flex items-end justify-end text-right italic font-light text-sm",
							children: "Offline"
						});
						else if (status.state === "rebooting") return /* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", {
							className: "flex items-end justify-end text-right italic font-light text-sm",
							children: "Rebooting"
						});
						else if (status.state === "waiting") return /* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", {
							className: "flex items-end justify-end text-right italic font-light text-sm",
							children: "Waiting"
						});
						else if (status.state === "updating_fw") return /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("span", {
							className: "flex items-end justify-end text-right italic font-light text-sm",
							children: [
								"Updating firmware, ",
								status.update_progress,
								"%"
							]
						});
						else if (status.state === "updating_fs") return /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("span", {
							className: "flex items-end justify-end text-right italic font-light text-sm",
							children: [
								"Updating filesystem, ",
								status.update_progress,
								"%"
							]
						});
						else if (status.state === "updating_tft") return /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("span", {
							className: "flex items-end justify-end text-right italic font-light text-sm",
							children: [
								"Updating GUI, ",
								status.update_progress,
								"%"
							]
						});
						else return /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("span", {
							className: "flex items-end justify-end text-right italic font-light text-sm text-warning",
							children: ["Unknown status: ", status.state]
						});
						return null;
					})(), status.state != "offline" && /* @__PURE__ */ (0, import_jsx_runtime.jsx)(NSPanelWarnings, { warnings: status.warnings })]
				})]
			}),
			/* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
				className: "flex justify-between items-end",
				children: /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", { children: [
					/* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
						className: "my-1",
						children: (() => {
							if (status.model != "sonoff" && status.model != "custom") return null;
							if (status.state == "offline" || status.state == "unknown" || status.state == "waiting" || status.state == "rebooting") return /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("span", {
								className: "icon-text text-sm text-base-content/50",
								children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)(import_Icon.Icon, {
									path: mdiWifiStrengthAlertOutline,
									className: "w-4 h-4 inline-block me-1"
								}), "-"]
							});
							else if (status.rssi <= -80) return /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("span", {
								className: "icon-text text-sm",
								children: [
									/* @__PURE__ */ (0, import_jsx_runtime.jsx)(import_Icon.Icon, {
										path: mdiWifiStrength1Alert,
										className: "w-4 h-4 inline-block me-1"
									}),
									status.rssi,
									" dBm"
								]
							});
							else if (status.rssi <= -70) return /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("span", {
								className: "icon-text text-sm",
								children: [
									/* @__PURE__ */ (0, import_jsx_runtime.jsx)(import_Icon.Icon, {
										path: mdiWifiStrength2,
										className: "w-4 h-4 inline-block me-1"
									}),
									status.rssi,
									" dBm"
								]
							});
							else if (status.rssi <= -55) return /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("span", {
								className: "icon-text text-sm",
								children: [
									/* @__PURE__ */ (0, import_jsx_runtime.jsx)(import_Icon.Icon, {
										path: mdiWifiStrength3,
										className: "w-4 h-4 inline-block me-1"
									}),
									status.rssi,
									" dBm"
								]
							});
							else return /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("span", {
								className: "icon-text text-sm",
								children: [
									/* @__PURE__ */ (0, import_jsx_runtime.jsx)(import_Icon.Icon, {
										path: mdiWifiStrength4,
										className: "w-4 h-4 inline-block me-1"
									}),
									status.rssi,
									" dBm"
								]
							});
						})()
					}),
					/* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
						className: "my-1",
						children: (() => {
							if (status.model != "sonoff" && status.model != "custom") return null;
							if (status.state == "offline" || status.state == "unknown" || status.state == "waiting" || status.state == "rebooting") return /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
								className: "flex items-center justify-start text-base-content/50",
								children: [/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
									className: "icon-text",
									children: [
										/* @__PURE__ */ (0, import_jsx_runtime.jsx)(import_Icon.Icon, {
											path: mdiThermometer,
											className: "w-4 h-4 inline-block me-1"
										}),
										"-",
										/* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", {
											id: "temperature-${nspanel_id}",
											className: "text-sm",
											children: "-"
										})
									]
								}), status.model == "custom" && /* @__PURE__ */ (0, import_jsx_runtime.jsxs)(import_jsx_runtime.Fragment, { children: [
									/* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", {
										className: "mx-1",
										children: "|"
									}),
									/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
										className: "icon-text text-base-content/50",
										children: [
											/* @__PURE__ */ (0, import_jsx_runtime.jsx)(import_Icon.Icon, {
												path: "M12,3.25C12,3.25 6,10 6,14C6,17.32 8.69,20 12,20A6,6 0 0,0 18,14C18,10 12,3.25 12,3.25M14.47,9.97L15.53,11.03L9.53,17.03L8.47,15.97M9.75,10A1.25,1.25 0 0,1 11,11.25A1.25,1.25 0 0,1 9.75,12.5A1.25,1.25 0 0,1 8.5,11.25A1.25,1.25 0 0,1 9.75,10M14.25,14.5A1.25,1.25 0 0,1 15.5,15.75A1.25,1.25 0 0,1 14.25,17A1.25,1.25 0 0,1 13,15.75A1.25,1.25 0 0,1 14.25,14.5Z",
												className: "w-4 h-4 inline-block me-1"
											}),
											"-",
											/* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", {
												id: "humidity-${nspanel_id}",
												className: "text-sm",
												children: "-"
											})
										]
									}),
									/* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", {
										className: "mx-1",
										children: "|"
									}),
									/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
										className: "icon-text text-base-content/50",
										children: [
											/* @__PURE__ */ (0, import_jsx_runtime.jsx)(import_Icon.Icon, {
												path: "M12,2A10,10 0 0,0 2,12A10,10 0 0,0 12,22A10,10 0 0,0 22,12A10,10 0 0,0 12,2M12,4A8,8 0 0,1 20,12C20,14.4 19,16.5 17.3,18C15.9,16.7 14,16 12,16C10,16 8.2,16.7 6.7,18C5,16.5 4,14.4 4,12A8,8 0 0,1 12,4M14,5.89C13.62,5.9 13.26,6.15 13.1,6.54L11.81,9.77L11.71,10C11,10.13 10.41,10.6 10.14,11.26C9.73,12.29 10.23,13.45 11.26,13.86C12.29,14.27 13.45,13.77 13.86,12.74C14.12,12.08 14,11.32 13.57,10.76L13.67,10.5L14.96,7.29L14.97,7.26C15.17,6.75 14.92,6.17 14.41,5.96C14.28,5.91 14.15,5.89 14,5.89M10,6A1,1 0 0,0 9,7A1,1 0 0,0 10,8A1,1 0 0,0 11,7A1,1 0 0,0 10,6M7,9A1,1 0 0,0 6,10A1,1 0 0,0 7,11A1,1 0 0,0 8,10A1,1 0 0,0 7,9M17,9A1,1 0 0,0 16,10A1,1 0 0,0 17,11A1,1 0 0,0 18,10A1,1 0 0,0 17,9Z",
												className: "w-4 h-4 inline-block me-1"
											}),
											"-",
											/* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", {
												id: "pressure-${nspanel_id}",
												className: "text-sm",
												children: "-"
											})
										]
									})
								] })]
							});
							else return /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
								className: "flex items-center justify-start",
								children: [/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
									className: "icon-text",
									children: [
										/* @__PURE__ */ (0, import_jsx_runtime.jsx)(import_Icon.Icon, {
											path: mdiThermometer,
											className: "w-4 h-4 inline-block me-1"
										}),
										"-",
										/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("span", {
											id: "temperature-${nspanel_id}",
											className: "text-sm",
											children: [status.temperature.toFixed(1), status.temperature_unit]
										})
									]
								}), status.model == "custom" && /* @__PURE__ */ (0, import_jsx_runtime.jsxs)(import_jsx_runtime.Fragment, { children: [
									/* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", {
										className: "mx-1",
										children: "|"
									}),
									/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
										className: "icon-text",
										children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)(import_Icon.Icon, {
											path: "M12,3.25C12,3.25 6,10 6,14C6,17.32 8.69,20 12,20A6,6 0 0,0 18,14C18,10 12,3.25 12,3.25M14.47,9.97L15.53,11.03L9.53,17.03L8.47,15.97M9.75,10A1.25,1.25 0 0,1 11,11.25A1.25,1.25 0 0,1 9.75,12.5A1.25,1.25 0 0,1 8.5,11.25A1.25,1.25 0 0,1 9.75,10M14.25,14.5A1.25,1.25 0 0,1 15.5,15.75A1.25,1.25 0 0,1 14.25,17A1.25,1.25 0 0,1 13,15.75A1.25,1.25 0 0,1 14.25,14.5Z",
											className: "w-4 h-4 inline-block me-1"
										}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", {
											id: "humidity-${nspanel_id}",
											className: "text-sm",
											children: status.humidity && `${status.humidity.toFixed(1)}%` || "-"
										})]
									}),
									/* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", {
										className: "mx-1",
										children: "|"
									}),
									/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
										className: "icon-text",
										children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)(import_Icon.Icon, {
											path: "M12,2A10,10 0 0,0 2,12A10,10 0 0,0 12,22A10,10 0 0,0 22,12A10,10 0 0,0 12,2M12,4A8,8 0 0,1 20,12C20,14.4 19,16.5 17.3,18C15.9,16.7 14,16 12,16C10,16 8.2,16.7 6.7,18C5,16.5 4,14.4 4,12A8,8 0 0,1 12,4M14,5.89C13.62,5.9 13.26,6.15 13.1,6.54L11.81,9.77L11.71,10C11,10.13 10.41,10.6 10.14,11.26C9.73,12.29 10.23,13.45 11.26,13.86C12.29,14.27 13.45,13.77 13.86,12.74C14.12,12.08 14,11.32 13.57,10.76L13.67,10.5L14.96,7.29L14.97,7.26C15.17,6.75 14.92,6.17 14.41,5.96C14.28,5.91 14.15,5.89 14,5.89M10,6A1,1 0 0,0 9,7A1,1 0 0,0 10,8A1,1 0 0,0 11,7A1,1 0 0,0 10,6M7,9A1,1 0 0,0 6,10A1,1 0 0,0 7,11A1,1 0 0,0 8,10A1,1 0 0,0 7,9M17,9A1,1 0 0,0 16,10A1,1 0 0,0 17,11A1,1 0 0,0 18,10A1,1 0 0,0 17,9Z",
											className: "w-4 h-4 inline-block me-1"
										}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", {
											id: "pressure-${nspanel_id}",
											className: "text-sm",
											children: status.pressure && `${(status.pressure / 100).toFixed(1)} hPa` || "-"
										})]
									})
								] })]
							});
							return null;
						})()
					}),
					/* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
						className: "my-1",
						children: (() => {
							if (status.model != "sonoff" && status.model != "custom") return null;
							if (status.state == "offline" || status.state == "unknown" || status.state == "waiting" || status.state == "rebooting") return /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
								className: "icon-text text-base-content/50",
								children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)(import_Icon.Icon, {
									path: mdiMemory,
									className: "w-4 h-4 inline-block me-1"
								}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", {
									id: "ram-${nspanel_id}",
									className: "text-sm",
									children: "-"
								})]
							});
							else return /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
								className: "icon-text",
								children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)(import_Icon.Icon, {
									path: mdiMemory,
									className: "w-4 h-4 inline-block me-1"
								}), /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("span", {
									id: "ram-${nspanel_id}",
									className: "text-sm",
									children: [status.ram_usage.toFixed(0), "%"]
								})]
							});
							return null;
						})()
					})
				] })
			}),
			/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
				className: "absolute right-2 bottom-2",
				children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("button", {
					className: "font-medium text-sm px-1 py-2 text-center inline-flex items-center cursor-pointer",
					popoverTarget: `nspanel-actions-${status.id}`,
					style: { anchorName: `--anchor-nspanel-actions-${status.id}` },
					children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)(import_Icon.Icon, {
						path: mdiCog,
						className: "w-4 h-4"
					})
				}), /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("ul", {
					className: "dropdown menu w-52 rounded-box bg-base-100 shadow-sm",
					popover: "auto",
					id: `nspanel-actions-${status.id}`,
					style: { anchorName: `--anchor-nspanel-actions-${status.id}` },
					ref: popoverRef,
					children: [status.model === "sonoff" || status.model === "custom" ? /* @__PURE__ */ (0, import_jsx_runtime.jsxs)(import_jsx_runtime.Fragment, { children: [
						/* @__PURE__ */ (0, import_jsx_runtime.jsx)("li", { children: /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("button", {
							onClick: () => {
								useStompStore.getState().send(`nspanel/${status.mac}/command`, "reboot");
								Notify({
									message: `Sent reboot command to ${status.name}.`,
									level: "success",
									duration: 2e3
								});
								popoverRef.current?.hidePopover();
							},
							id: "reboot-{{ id }}",
							"hx-swap": "none",
							className: "block px-4 py-2 cursor-pointer nspanel-reboot-button group",
							children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)(import_Icon.Icon, {
								path: mdiRestart,
								className: "w-4 h-4 inline-block me-1 group-hover:text-warning"
							}), "Reboot"]
						}) }),
						/* @__PURE__ */ (0, import_jsx_runtime.jsx)("li", { children: /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("a", {
							href: status.state === "offline" ? "#" : `http://${status.ip_address}/`,
							onClick: () => status.state != "offline" ? popoverRef.current?.hidePopover() : null,
							id: "visit-{{ id }}",
							"hx-swap": "none",
							className: `block px-4 py-2 cursor-pointer nspanel-visit-button group ${status.state === "offline" ? "line-through" : ""}`,
							target: "_blank",
							"aria-disabled": status.state === "offline",
							children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)(import_Icon.Icon, {
								path: mdiWeb,
								className: "w-4 h-4 inline-block me-1 group-hover:text-info"
							}), "Visit"]
						}) }),
						/* @__PURE__ */ (0, import_jsx_runtime.jsx)("li", { className: "border-t border-neutral-content" }),
						/* @__PURE__ */ (0, import_jsx_runtime.jsx)("li", { children: /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("a", {
							onClick: () => {
								if (status.state !== "offline") {
									useStompStore.getState().send(`nspanel/${status.mac}/command`, "firmware_update");
									Notify({
										message: `Sent firmware update command to ${status.name}.`,
										level: "success",
										duration: 2e3
									});
									popoverRef.current?.hidePopover();
								}
							},
							id: "firmware-update-{{ nspanel.data.id }}",
							className: `block px-4 py-2 cursor-pointer group ${status.state === "offline" ? "line-through" : ""}`,
							"aria-disabled": status.state === "offline",
							children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)(import_Icon.Icon, {
								path: mdiUpload,
								className: "w-4 h-4 inline-block me-1 group-hover:text-success"
							}), "Update firmware"]
						}) }),
						/* @__PURE__ */ (0, import_jsx_runtime.jsx)("li", { children: /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("a", {
							onClick: () => {
								if (status.state !== "offline") {
									useStompStore.getState().send(`nspanel/${status.mac}/command`, "gui_update");
									Notify({
										message: `Sent GUI update command to ${status.name}.`,
										level: "success",
										duration: 2e3
									});
									popoverRef.current?.hidePopover();
								}
							},
							id: "screen-update-{{ nspanel.data.id }}",
							className: `block px-4 py-2 cursor-pointer group ${status.state === "offline" ? "line-through" : ""}`,
							"aria-disabled": status.state === "offline",
							children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)(import_Icon.Icon, {
								path: mdiTableArrowUp,
								className: "w-4 h-4 inline-block me-1 group-hover:text-success"
							}), "Update GUI"]
						}) }),
						/* @__PURE__ */ (0, import_jsx_runtime.jsx)("li", { className: "border-t border-neutral-content" })
					] }) : null, /* @__PURE__ */ (0, import_jsx_runtime.jsx)("li", { children: /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("button", {
						onClick: () => {
							delete_nspanel_dialog_ref.current?.showModal();
							popoverRef.current?.hidePopover();
						},
						id: "delete-{{ id }}",
						"hx-swap": "none",
						className: "block px-4 py-2 cursor-pointer group",
						children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)(import_Icon.Icon, {
							path: mdiMinusCircle,
							className: "w-4 h-4 inline-block me-1 group-hover:text-error"
						}), "Delete"]
					}) })]
				})]
			})
		]
	});
};
var deny_nspanel = (nspanel_id) => {
	fetch(`/rest/nspanel/${nspanel_id}/deny`, {
		method: "POST",
		headers: { "X-CSRFToken": getCookie("csrftoken") }
	}).then((response) => response.json()).then((data) => {
		console.log(data);
	});
};
var accept_nspanel = ({ nspanel_id, room_id }) => {
	fetch(`/rest/nspanel/${nspanel_id}/accept`, {
		method: "POST",
		headers: { "X-CSRFToken": getCookie("csrftoken") },
		body: JSON.stringify({ room_id })
	}).then((response) => response.json()).then((data) => {
		console.log(data);
	});
};
var AcceptNewNSPanelDialog = (0, import_react.forwardRef)(({ status, accept_new_nspanel_dialog_ref }, ref) => {
	const [selectedRoom, setSelectedRoom] = (0, import_react.useState)(null);
	const [availableRooms, setAvailableRooms] = (0, import_react.useState)([]);
	const classNames = {
		control: (state) => `${state.isFocused ? "border" : "border-0"} border-accent p-2.5 text-sm rounded-box bg-base-300 rounded-md`,
		menu: () => "bg-base-300 p-2.5 rounded-box",
		option: (state) => `p-1 ${state.isSelected ? "bg-primary/20 rounded-sm" : ""} ${state.isFocused ? "bg-primary/20 rounded-sm" : ""}`
	};
	const CustomOption = ({ innerProps, isDisabled, isFocused, isSelected, children }) => {
		if (isDisabled) return null;
		return /* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
			...innerProps,
			className: `p-1 ${isSelected ? "bg-primary/20 rounded-sm" : ""} ${isFocused ? "bg-primary/20 rounded-sm" : ""}`,
			children
		});
	};
	if (availableRooms.length === 0) {
		const rooms = useEntityStatesStore.getState().rooms;
		for (const room of Object.values(rooms)) setAvailableRooms((prev) => [...prev, {
			value: room.id,
			label: room.friendly_name
		}]);
	}
	const contentRef = (0, import_react.useRef)(null);
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
	return /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("dialog", {
		ref,
		className: "modal",
		children: [/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
			className: "modal-box",
			ref: contentRef,
			children: [
				/* @__PURE__ */ (0, import_jsx_runtime.jsx)("h3", {
					className: "text-lg font-bold",
					children: "Accept NSPanel"
				}),
				/* @__PURE__ */ (0, import_jsx_runtime.jsx)("label", {
					className: "block mb-2 text-sm font-medium",
					children: "Select room"
				}),
				/* @__PURE__ */ (0, import_jsx_runtime.jsx)(StateManagedSelect$1, {
					options: availableRooms,
					classNames,
					onChange: (newValue) => {
						setSelectedRoom(newValue);
					},
					value: selectedRoom,
					unstyled: true,
					components: { Option: CustomOption },
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
				}),
				/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
					className: "flex justify-end join pt-2",
					children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("button", {
						onClick: () => accept_new_nspanel_dialog_ref.current?.close(),
						className: "btn btn-neutral join-item",
						children: "Cancel"
					}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)("button", {
						onClick: () => {
							if (!selectedRoom) return;
							accept_new_nspanel_dialog_ref.current?.close();
							accept_nspanel({
								nspanel_id: status.id,
								room_id: selectedRoom.value
							});
						},
						className: "btn btn-success join-item",
						children: "Accept"
					})]
				})
			]
		}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)("form", {
			method: "dialog",
			className: "modal-backdrop",
			children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)("button", { children: "close" })
		})]
	});
});
var NewNSPanelContent = ({ status }) => {
	const accept_new_nspanel_dialog_ref = (0, import_react.useRef)(null);
	return /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
		className: "flex items-center justify-center h-full w-full",
		children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)(AcceptNewNSPanelDialog, {
			status,
			ref: accept_new_nspanel_dialog_ref,
			accept_new_nspanel_dialog_ref
		}), /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", { children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
			className: "flex w-full items-/center justify-center pt-2",
			children: /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("span", {
				className: "nspanel-name",
				children: [
					"NSPanel '",
					status.name,
					"' is awaiting accept."
				]
			})
		}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
			className: "flex w-full items-center justify-center pt-2 pb-4",
			children: /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", { children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("button", {
				className: "btn btn-soft btn-error m-1 rounded-btn",
				onClick: () => deny_nspanel(status.id),
				children: "Deny"
			}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)("button", {
				className: "btn btn-soft btn-info m-1 rounded-btn",
				onClick: () => accept_new_nspanel_dialog_ref.current?.showModal(),
				children: "Accept"
			})] })
		})] })]
	});
};
var NSPanelBox = ({ mac }) => {
	const nspanel_status = useEntityStatesStore.getState().nspanels[mac];
	return /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
		className: "text-base-content rounded-b-box rounded-t-field nspanel-box bg-base-200 hover:bg-base-100 transition-all duration-200 shadow-xl shadow-neutral/3 hover:shadow-neutral/10 overflow-visible h-34",
		children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
			className: "overflow-hidden",
			children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)(NSPanelStatusHeader, {
				state: nspanel_status?.state ?? "",
				update_progress: nspanel_status?.update_progress ?? 0
			})
		}), nspanel_status?.accepted && /* @__PURE__ */ (0, import_jsx_runtime.jsx)(AcceptedNSPanelContent, { status: nspanel_status }) || /* @__PURE__ */ (0, import_jsx_runtime.jsx)(NewNSPanelContent, { status: nspanel_status })]
	});
};
//#endregion
//#region src/components/IndexPage/NSPanelsView.tsx
var NSPanelsIndexView = () => {
	const { nspanels, status, fetchData } = useEntityStatesStore();
	const popoverRef = (0, import_react.useRef)(null);
	(0, import_react.useEffect)(() => {
		if (status === "none") fetchData();
	}, [status, fetchData]);
	return /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", { children: [/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
		className: "flex items-center justify-between",
		children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", {
			className: "text-2xl",
			children: "NSPanels"
		}), /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", { children: [/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("button", {
			className: "btn btn-soft font-medium",
			popoverTarget: `nspanels-actions`,
			style: { anchorName: `--anchor-nspanels-actions` },
			children: ["Actions", /* @__PURE__ */ (0, import_jsx_runtime.jsx)(import_Icon.Icon, {
				path: mdiMenuDownOutline,
				className: "w-4"
			})]
		}), /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("ul", {
			className: "dropdown menu w-52 rounded-box bg-base-100 shadow-sm",
			popover: "auto",
			id: `nspanels-actions`,
			style: { anchorName: `--anchor-nspanels-actions` },
			ref: popoverRef,
			children: [
				/* @__PURE__ */ (0, import_jsx_runtime.jsx)("li", { children: /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("button", {
					onClick: () => {
						const nspanels = useEntityStatesStore.getState().nspanels;
						for (const [mac, status] of Object.entries(nspanels)) if (status.accepted) useStompStore.getState().send(`nspanel/${mac}/command`, "reboot");
						Notify({
							message: `Sent reboot command to all NSPanels.`,
							level: "success",
							duration: 2e3
						});
						popoverRef.current?.hidePopover();
					},
					id: "reboot-{{ id }}",
					"hx-swap": "none",
					className: "block px-4 py-2 group",
					children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)(import_Icon.Icon, {
						path: mdiRestart,
						className: "w-6 h-6 inline-block pr-2 group-hover:text-warning"
					}), "Reboot all NSPanels"]
				}) }),
				/* @__PURE__ */ (0, import_jsx_runtime.jsx)("li", { className: "border-t border-neutral-content" }),
				/* @__PURE__ */ (0, import_jsx_runtime.jsx)("li", { children: /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("a", {
					onClick: () => {
						const nspanels = useEntityStatesStore.getState().nspanels;
						for (const [mac, status] of Object.entries(nspanels)) if (status.state == "online" && status.accepted) {
							useStompStore.getState().send(`nspanel/${mac}/command`, "firmware_update");
							Notify({
								message: `Sent FW update command to ${status.name}.`,
								level: "success",
								duration: 2e3
							});
						}
						popoverRef.current?.hidePopover();
					},
					id: "firmware-update-all",
					className: `block px-4 py-2 cursor-pointer group`,
					children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)(import_Icon.Icon, {
						path: mdiUpload,
						className: "w-6 h-6 inline-block pr-2 group-hover:text-success"
					}), "Update firmware on all NSPanels"]
				}) }),
				/* @__PURE__ */ (0, import_jsx_runtime.jsx)("li", { children: /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("a", {
					onClick: () => {
						const nspanels = useEntityStatesStore.getState().nspanels;
						for (const [mac, status] of Object.entries(nspanels)) if (status.state == "online" && status.accepted) {
							useStompStore.getState().send(`nspanel/${mac}/command`, "gui_update");
							Notify({
								message: `Sent GUI update command to ${status.name}.`,
								level: "success",
								duration: 2e3
							});
						}
						popoverRef.current?.hidePopover();
					},
					id: "screen-update-all",
					className: `block px-4 py-2 cursor-pointer group`,
					children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)(import_Icon.Icon, {
						path: mdiTableArrowUp,
						className: "w-6 h-6 inline-block pr-2 group-hover:text-success"
					}), "Update GUI on all NSPanels"]
				}) }),
				/* @__PURE__ */ (0, import_jsx_runtime.jsx)("li", { className: "border-t border-neutral-content" }),
				/* @__PURE__ */ (0, import_jsx_runtime.jsx)("li", { children: /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("button", {
					onClick: () => {
						fetch("/api/restart_mqtt_manager", { method: "POST" }).then((response) => {
							if (response.ok) Notify({
								message: `MQTTManager restarted successfully.`,
								level: "success",
								duration: 2e3
							});
							else Notify({
								message: `Failed to restart MQTTManager.`,
								level: "error",
								duration: 2e3
							});
						}).catch((error) => {
							Notify({
								message: `Failed to restart MQTTManager.`,
								level: "error",
								duration: 2e3
							});
							console.error("Got error while trying to restart MQTTManager process. Error:", error);
						});
						popoverRef.current?.hidePopover();
					},
					id: "restart-mqtt-manager",
					className: "block px-4 py-2 cursor-pointer group",
					children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)(import_Icon.Icon, {
						path: mdiRestart,
						className: "w-6 h-6 inline-block pr-2 group-hover:text-warning"
					}), "Restart MQTTManager"]
				}) })
			]
		})] })]
	}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", { children: (() => {
		if (status != "loaded") return /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
			className: "flex w-full items-center justify-center text-2xl",
			children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", { className: "loading loading-spinner loading-sm mr-2" }), /* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", {
				className: "font-medium",
				children: "Loading NSPanels..."
			})]
		});
		if (nspanels && Object.keys(nspanels).length > 0) return /* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
			className: "grid md:grid-cols-3 gap-4 mt-4 items-stretch",
			id: "nspanels_container",
			children: Object.entries(nspanels).filter(([_mac, status]) => !status.denied).map(([mac, status]) => /* @__PURE__ */ (0, import_jsx_runtime.jsx)(NSPanelBox, { mac: String(status.mac) }, mac))
		});
		else return /* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
			className: "grid md:grid-cols-3 gap-4 mt-4 items-start",
			id: "nspanels_container",
			children: /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
				className: "col-span-3 w-full text-center text-xl text-base-content",
				children: [
					"No NSPanels found. Getting started?",
					" ",
					/* @__PURE__ */ (0, import_jsx_runtime.jsx)("a", {
						href: "https://nspanelmanager.com/docs/getting-started/flash-nspanel",
						target: "_blank",
						className: "text-accent",
						children: "Read the documentation."
					})
				]
			})
		});
	})() })] });
};
//#endregion
export { NSPanelsIndexView as default };
