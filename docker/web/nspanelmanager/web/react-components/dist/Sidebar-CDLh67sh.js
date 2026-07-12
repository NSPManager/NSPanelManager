import { c as __toESM, r as require_react, t as require_jsx_runtime } from "./main-D4-XR9wG.js";
import { n as Notify } from "./NSPanelToastContainer-BCoYhxSl.js";
import { n as useStompStore, t as useEntityStatesStore } from "./EntityStore-BGWLtY3I.js";
import { _ as mdiWifiStrength4, a as mdiMemory, c as mdiRestart, d as mdiUpload, f as mdiWaterPercent, g as mdiWifiStrength3, h as mdiWifiStrength2, i as mdiInformationOutline, l as mdiTableArrowUp, m as mdiWifiStrength1Alert, p as mdiWeb, s as mdiMinusCircle, u as mdiThermometer, v as mdiWifiStrengthAlertOutline, y as require_Icon } from "./mdi-OVIfv5IR.js";
//#region src/components/NSPanelPage/Sidebar.tsx
var import_react = /* @__PURE__ */ __toESM(require_react(), 1);
var import_Icon = require_Icon();
var import_jsx_runtime = require_jsx_runtime();
var NSPanelStatus = ({ status }) => {
	if (status.state == "online") return /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
		className: "list-row flex items-center justify-start gap-1 py-3 px-2",
		children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)(import_Icon.Icon, {
			path: mdiInformationOutline,
			className: "ms-2 w-4 h-4 inline-block"
		}), /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("span", {
			className: "text-base",
			children: ["State: ", /* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", {
				className: "text-success",
				children: "Online"
			})]
		})]
	}, `nspanel-status-${status.id}`);
	else if (status.state == "offline") return /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
		className: "list-row flex items-center justify-start gap-1 py-3 px-2",
		children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)(import_Icon.Icon, {
			path: mdiInformationOutline,
			className: "ms-2 w-4 h-4 inline-block"
		}), /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("span", {
			className: "text-base",
			children: ["State: ", /* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", {
				className: "text-error",
				children: "Offline"
			})]
		})]
	}, `nspanel-status-${status.id}`);
	else if (status.state == "rebooting") return /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
		className: "list-row flex items-center justify-start gap-1 py-3 px-2",
		children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)(import_Icon.Icon, {
			path: mdiInformationOutline,
			className: "ms-2 w-4 h-4 inline-block"
		}), /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("span", {
			className: "text-base",
			children: ["State: ", /* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", {
				className: "text-info",
				children: "Rebooting"
			})]
		})]
	}, `nspanel-status-${status.id}`);
	else if (status.state == "updating_fw") return /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
		className: "list-row flex items-center justify-start gap-1 py-3 px-2 relative",
		children: [
			/* @__PURE__ */ (0, import_jsx_runtime.jsx)(import_Icon.Icon, {
				path: mdiInformationOutline,
				className: "ms-2 w-4 h-4 inline-block"
			}),
			/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("span", {
				className: "text-base",
				children: ["State: ", /* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", {
					className: "text-primary",
					children: "Updating firmware"
				})]
			}),
			/* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
				className: "absolute left-0 bottom-0 w-full",
				children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
					className: "min-h-1 bg-primary mx-2 transition-all ease-linear duration-1000 rounded-full",
					style: { width: `${status.update_progress}%` }
				})
			})
		]
	}, `nspanel-status-${status.id}`);
	else if (status.state == "updating_fs") return /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
		className: "list-row flex items-center justify-start gap-1 py-3 px-2 relative",
		children: [
			/* @__PURE__ */ (0, import_jsx_runtime.jsx)(import_Icon.Icon, {
				path: mdiInformationOutline,
				className: "ms-2 w-4 h-4 inline-block"
			}),
			/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("span", {
				className: "text-base",
				children: ["State: ", /* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", {
					className: "text-primary",
					children: "Updating filesystem"
				})]
			}),
			/* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
				className: "absolute left-0 bottom-0 w-full",
				children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
					className: "min-h-1 bg-primary mx-2 transition-all ease-linear duration-1000 rounded-full",
					style: { width: `${status.update_progress}%` }
				})
			})
		]
	}, `nspanel-status-${status.id}`);
	else if (status.state == "updating_tft") return /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
		className: "list-row flex items-center justify-start gap-1 py-3 px-2 relative",
		children: [
			/* @__PURE__ */ (0, import_jsx_runtime.jsx)(import_Icon.Icon, {
				path: mdiInformationOutline,
				className: "ms-2 w-4 h-4 inline-block"
			}),
			/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("span", {
				className: "text-base",
				children: ["State: ", /* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", {
					className: "text-primary",
					children: "Updating GUI"
				})]
			}),
			/* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
				className: "absolute left-0 bottom-0 w-full",
				children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
					className: "min-h-1 bg-primary mx-2 transition-all ease-linear duration-1000 rounded-full",
					style: { width: `${status.update_progress}%` }
				})
			})
		]
	}, `nspanel-status-${status.id}`);
	else return /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
		className: "list-row flex items-center justify-start gap-1 py-3 px-2",
		children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)(import_Icon.Icon, {
			path: mdiInformationOutline,
			className: "ms-2 w-4 h-4 inline-block"
		}), /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("span", {
			className: "text-base",
			children: ["UNKNOWN STATE! ", status.state]
		})]
	}, `nspanel-status-${status.id}`);
};
var NSPanelWiFiStatus = ({ status }) => {
	if (status.model != "sonoff" && status.model != "custom") return null;
	if (status.state == "offline" || status.state == "unknown" || status.state == "waiting" || status.state == "rebooting") return /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
		className: "list-row flex items-center justify-start gap-1 text-base py-3 px-2",
		children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)(import_Icon.Icon, {
			path: mdiWifiStrengthAlertOutline,
			className: "ms-2 w-4 h-4 inline-block me-1"
		}), "Wifi: -"]
	});
	else if (status.rssi <= -80) return /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
		className: "list-row flex items-center justify-start gap-1 text-base py-3 px-2",
		children: [
			/* @__PURE__ */ (0, import_jsx_runtime.jsx)(import_Icon.Icon, {
				path: mdiWifiStrength1Alert,
				className: "ms-2 w-4 h-4 inline-block"
			}),
			"Wifi: ",
			status.rssi,
			" dBm"
		]
	});
	else if (status.rssi <= -70) return /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
		className: "list-row flex items-center justify-start gap-1 text-base py-3 px-2",
		children: [
			/* @__PURE__ */ (0, import_jsx_runtime.jsx)(import_Icon.Icon, {
				path: mdiWifiStrength2,
				className: "ms-2 w-4 h-4 inline-block"
			}),
			"Wifi: ",
			status.rssi,
			" dBm"
		]
	});
	else if (status.rssi <= -55) return /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
		className: "list-row flex items-center justify-start gap-1 text-base py-3 px-2",
		children: [
			/* @__PURE__ */ (0, import_jsx_runtime.jsx)(import_Icon.Icon, {
				path: mdiWifiStrength3,
				className: "ms-2 w-4 h-4 inline-block"
			}),
			"Wifi: ",
			status.rssi,
			" dBm"
		]
	});
	else return /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
		className: "list-row flex items-center justify-start gap-1 text-base py-3 px-2",
		children: [
			/* @__PURE__ */ (0, import_jsx_runtime.jsx)(import_Icon.Icon, {
				path: mdiWifiStrength4,
				className: "ms-2 w-4 h-4 inline-block"
			}),
			"Wifi: ",
			status.rssi,
			" dBm"
		]
	});
};
var NSPanelRamStatus = ({ status }) => {
	if (status.model != "sonoff" && status.model != "custom") return null;
	if (status.state == "offline" || status.state == "unknown" || status.state == "waiting" || status.state == "rebooting") return /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
		className: "list-row flex items-center justify-start gap-1 text-base py-3 px-2",
		children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)(import_Icon.Icon, {
			path: mdiMemory,
			className: "ms-2 w-4 h-4 inline-block me-1"
		}), "RAM: -"]
	});
	else return /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
		className: "list-row flex items-center justify-start gap-1 text-base py-3 px-2",
		children: [
			/* @__PURE__ */ (0, import_jsx_runtime.jsx)(import_Icon.Icon, {
				path: mdiMemory,
				className: "ms-2 w-4 h-4 inline-block"
			}),
			"RAM: ",
			status.ram_usage,
			"%"
		]
	});
};
var NSPanelTemperatureStatus = ({ status }) => {
	if (status.model != "sonoff" && status.model != "custom") return null;
	if (status.state == "offline" || status.state == "unknown" || status.state == "waiting" || status.state == "rebooting") return /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
		className: "list-row flex items-center justify-start gap-1 text-base py-3 px-2",
		children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)(import_Icon.Icon, {
			path: mdiThermometer,
			className: "ms-2 w-4 h-4 inline-block me-1"
		}), "Temperature: -"]
	});
	else return /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
		className: "list-row flex items-center justify-start gap-1 text-base py-3 px-2",
		children: [
			/* @__PURE__ */ (0, import_jsx_runtime.jsx)(import_Icon.Icon, {
				path: mdiThermometer,
				className: "ms-2 w-4 h-4 inline-block"
			}),
			"Temp: ",
			status.temperature.toFixed(1),
			status.temperature_unit
		]
	});
};
var NSPanelHumidityStatus = ({ status }) => {
	if (status.model != "custom") return null;
	if (status.state == "offline" || status.state == "unknown" || status.state == "waiting" || status.state == "rebooting") return /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
		className: "list-row flex items-center justify-start gap-1 text-base py-3 px-2",
		children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)(import_Icon.Icon, {
			path: mdiWaterPercent,
			className: "ms-2 w-4 h-4 inline-block me-1"
		}), "Humidity: -"]
	});
	else return /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
		className: "list-row flex items-center justify-start gap-1 text-base py-3 px-2",
		children: [
			/* @__PURE__ */ (0, import_jsx_runtime.jsx)(import_Icon.Icon, {
				path: mdiWaterPercent,
				className: "ms-2 w-4 h-4 inline-block"
			}),
			"Humidity: ",
			status.humidity?.toFixed(1),
			"%"
		]
	});
};
var NSPanelPressureStatus = ({ status }) => {
	if (status.model != "custom") return null;
	if (status.state == "offline" || status.state == "unknown" || status.state == "waiting" || status.state == "rebooting") return /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
		className: "list-row flex items-center justify-start gap-1 text-base py-3 px-2",
		children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)(import_Icon.Icon, {
			path: mdiWaterPercent,
			className: "ms-2 w-4 h-4 inline-block me-1"
		}), "Pressure: -"]
	});
	else return /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
		className: "list-row flex items-center justify-start gap-1 text-base py-3 px-2",
		children: [
			/* @__PURE__ */ (0, import_jsx_runtime.jsx)(import_Icon.Icon, {
				path: mdiWaterPercent,
				className: "ms-2 w-4 h-4 inline-block"
			}),
			"Pressure: ",
			status.pressure && `${(status.pressure / 100).toFixed(1)} hPa` || "-"
		]
	});
};
var NSPanelRebootButton = ({ status }) => {
	if (status.model != "sonoff" && status.model != "custom") return null;
	return /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
		className: "list-row flex items-center justify-start gap-1 py-3 px-2 group cursor-pointer hover:bg-base-300",
		onClick: () => {
			useStompStore.getState().send(`nspanel/${status.mac}/command`, "reboot");
			Notify({
				message: `Sent reboot command to ${status.name}.`,
				level: "success",
				duration: 2e3
			});
		},
		children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)(import_Icon.Icon, {
			path: mdiRestart,
			className: "ms-2 w-4 h-4 inline-block group-hover:text-warning"
		}), "Reboot"]
	});
};
var NSPanelVisitButton = ({ status }) => {
	if (status.model != "sonoff" && status.model != "custom") return null;
	return /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("a", {
		className: `list-row flex items-center justify-start gap-1 py-3 px-2 group cursor-pointer hover:bg-base-300 ${status.state === "offline" ? "line-through" : ""}`,
		href: status.state === "offline" ? "#" : `http://${status.ip_address}/`,
		target: "_blank",
		children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)(import_Icon.Icon, {
			path: mdiWeb,
			className: "ms-2 w-4 h-4 inline-block group-hover:text-info"
		}), "Visit"]
	});
};
var NSPanelFirmwareUpdateButton = ({ status }) => {
	if (status.model != "sonoff" && status.model != "custom") return;
	return /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
		className: "list-row flex items-center justify-start gap-1 py-3 px-2 group cursor-pointer hover:bg-base-300",
		onClick: () => {
			if (status.state === "offline") return;
			useStompStore.getState().send(`nspanel/${status.mac}/command`, "firmware_update");
			Notify({
				message: `Sent firmware update command.`,
				level: "success",
				duration: 2e3
			});
		},
		children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)(import_Icon.Icon, {
			path: mdiUpload,
			className: `ms-2 w-4 h-4 inline-block group-hover:text-success ${status.state === "offline" ? "line-through" : ""}`
		}), "Update firmware"]
	});
};
var NSPanelForceFirmwareUpdateButton = ({ status }) => {
	if (status.model != "sonoff" && status.model != "custom") return;
	return /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
		className: "list-row flex items-center justify-start gap-1 py-3 px-2 group cursor-pointer hover:bg-base-300",
		onClick: () => {
			if (status.state === "offline") return;
			useStompStore.getState().send(`nspanel/${status.mac}/command`, "firmware_update_force");
			Notify({
				message: `Sent force firmware update command.`,
				level: "success",
				duration: 2e3
			});
		},
		children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)(import_Icon.Icon, {
			path: mdiUpload,
			className: `ms-2 w-4 h-4 inline-block group-hover:text-success ${status.state === "offline" ? "line-through" : ""}`
		}), "Force update firmware"]
	});
};
var NSPanelGuiUpdateButton = ({ status }) => {
	if (status.model != "sonoff" && status.model != "custom") return;
	return /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
		className: "list-row flex items-center justify-start gap-1 py-3 px-2 group cursor-pointer hover:bg-base-300",
		onClick: () => {
			if (status.state === "offline") return;
			useStompStore.getState().send(`nspanel/${status.mac}/command`, "gui_update");
			Notify({
				message: `Sent GUI update command.`,
				level: "success",
				duration: 2e3
			});
		},
		children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)(import_Icon.Icon, {
			path: mdiTableArrowUp,
			className: `ms-2 w-4 h-4 inline-block group-hover:text-success ${status.state === "offline" ? "line-through" : ""}`
		}), "Update GUI"]
	});
};
var NSPanelDeleteButton = ({ delete_confirm_dialog_ref }) => {
	return /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
		className: "list-row flex items-center justify-start gap-1 py-3 px-2 group cursor-pointer hover:bg-base-300",
		onClick: () => {
			if (delete_confirm_dialog_ref.current) delete_confirm_dialog_ref.current.showModal();
		},
		children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)(import_Icon.Icon, {
			path: mdiMinusCircle,
			className: `ms-2 w-4 h-4 inline-block group-hover:text-error`
		}), "Delete"]
	});
};
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
var NSPanelDeleteConfirmDialog = (0, import_react.forwardRef)(({ status, delete_nspanel_dialog_ref }, ref) => {
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
							fetch(`/rest/nspanel/${status.id}`, {
								method: "DELETE",
								headers: { "X-CSRFToken": getCookie("csrftoken") }
							}).then(() => {
								window.location.href = "/";
							}).catch((error) => {
								Notify({
									message: `Failed to delete NSPanel: ${error}`,
									level: "error",
									duration: 1e4
								});
							});
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
function Sidebar({ nspanel_mac }) {
	const { nspanels, status, fetchData } = useEntityStatesStore();
	(0, import_react.useEffect)(() => {
		if (status === "none") fetchData();
	}, [status, fetchData]);
	const delete_confirm_dialog_ref = (0, import_react.useRef)(null);
	if (status == "loading" || status == "none") return /* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
		className: "border-base-300 bg-base-100 p-2 rounded-box list",
		children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", {
			className: "text-lg",
			children: "Loading..."
		})
	});
	else if (status == "loaded" && !nspanels[nspanel_mac]) return /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("span", {
		className: "text-error text-md",
		children: [
			"NSPanel state not found for panel with MAC: ",
			nspanel_mac,
			"!"
		]
	});
	return /* @__PURE__ */ (0, import_jsx_runtime.jsxs)(import_jsx_runtime.Fragment, { children: [
		/* @__PURE__ */ (0, import_jsx_runtime.jsx)(NSPanelDeleteConfirmDialog, {
			ref: delete_confirm_dialog_ref,
			delete_nspanel_dialog_ref: delete_confirm_dialog_ref,
			status: nspanels[nspanel_mac]
		}),
		/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", { children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", {
			className: "font-medium text-lg",
			children: "Status"
		}), /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
			className: "border-base-300 bg-base-100 rounded-box list",
			children: [
				/* @__PURE__ */ (0, import_jsx_runtime.jsx)(NSPanelStatus, { status: nspanels[nspanel_mac] }),
				/* @__PURE__ */ (0, import_jsx_runtime.jsx)(NSPanelWiFiStatus, { status: nspanels[nspanel_mac] }),
				/* @__PURE__ */ (0, import_jsx_runtime.jsx)(NSPanelRamStatus, { status: nspanels[nspanel_mac] }),
				/* @__PURE__ */ (0, import_jsx_runtime.jsx)(NSPanelTemperatureStatus, { status: nspanels[nspanel_mac] }),
				/* @__PURE__ */ (0, import_jsx_runtime.jsx)(NSPanelHumidityStatus, { status: nspanels[nspanel_mac] }),
				/* @__PURE__ */ (0, import_jsx_runtime.jsx)(NSPanelPressureStatus, { status: nspanels[nspanel_mac] })
			]
		})] }),
		/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
			className: "mt-2",
			children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", {
				className: "font-medium text-lg",
				children: "Actions"
			}), /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("ul", {
				className: "border-base-300 bg-base-100 rounded-box list w-full",
				children: [
					/* @__PURE__ */ (0, import_jsx_runtime.jsx)(NSPanelRebootButton, { status: nspanels[nspanel_mac] }),
					/* @__PURE__ */ (0, import_jsx_runtime.jsx)(NSPanelVisitButton, { status: nspanels[nspanel_mac] }),
					/* @__PURE__ */ (0, import_jsx_runtime.jsx)(NSPanelFirmwareUpdateButton, { status: nspanels[nspanel_mac] }),
					/* @__PURE__ */ (0, import_jsx_runtime.jsx)(NSPanelForceFirmwareUpdateButton, { status: nspanels[nspanel_mac] }),
					/* @__PURE__ */ (0, import_jsx_runtime.jsx)(NSPanelGuiUpdateButton, { status: nspanels[nspanel_mac] }),
					/* @__PURE__ */ (0, import_jsx_runtime.jsx)(NSPanelDeleteButton, { delete_confirm_dialog_ref })
				]
			})]
		})
	] });
}
//#endregion
export { Sidebar as default };
