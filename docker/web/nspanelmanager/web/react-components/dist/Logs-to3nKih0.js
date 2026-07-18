import { c as __toESM, r as require_react, t as require_jsx_runtime } from "./main-ttieu6Zu.js";
import { t as useSettingsStore } from "./SettingsStore-zkoXIVaG.js";
import { t as useStompStore } from "./StompStore-BKyW2wrw.js";
//#region src/components/NSPanelPage/Logs.tsx
var import_react = /* @__PURE__ */ __toESM(require_react(), 1);
var import_jsx_runtime = require_jsx_runtime();
function NSPanelLogs({ nspanel_mac }) {
	const { status: settingsStatus, fetchData: fetchSettingsData } = useSettingsStore();
	const { status: stompStatus } = useStompStore();
	const [logs, setLogs] = (0, import_react.useState)([]);
	const [hasSubscribed, setHasSubscribed] = (0, import_react.useState)(false);
	const [hasDownloadedLogs, setHasDownloadedLogs] = (0, import_react.useState)(false);
	const maxLogs = useSettingsStore.getState().settings?.max_live_log_messages ?? 250;
	const nspanel_backlog_callback = (message) => {
		useStompStore.getState().unsubscribe(`nspanel/${nspanel_mac}/log_backlog`, nspanel_backlog_callback);
		if (logs.length === 0) setLogs(JSON.parse(message.body).logs);
		if (logs.length > maxLogs) setLogs(logs.slice(0, maxLogs));
	};
	(0, import_react.useEffect)(() => {
		if (settingsStatus === "none") fetchSettingsData();
		if (stompStatus == "none") {
			setHasSubscribed(false);
			useStompStore.getState().connect();
		}
		if (stompStatus == "connected" && !hasSubscribed) {
			useStompStore.getState().subscribe(`nspanel/${nspanel_mac}/log_backlog`, nspanel_backlog_callback);
			useStompStore.getState().subscribe(`nspanel/${nspanel_mac}/log`, (message) => {
				const log = JSON.parse(message.body);
				setLogs((prevLogs) => [log, ...prevLogs]);
				if (logs.length > maxLogs) setLogs(logs.slice(0, maxLogs));
			});
			setHasSubscribed(true);
		}
	}, [
		settingsStatus,
		fetchSettingsData,
		stompStatus,
		nspanel_mac
	]);
	const download_nspanel_logs_callback = (message) => {
		if (!hasDownloadedLogs) {
			const data = JSON.parse(message.body);
			const logs = [];
			data.logs.map((log) => logs.push(`${log.time} ${log.level} ${log.message}`));
			const blob = new Blob([logs.join("\n")], { type: "text/plain" });
			const url = URL.createObjectURL(blob);
			const a = document.createElement("a");
			a.href = url;
			a.download = `nspanel_${nspanel_mac}_logs.txt`;
			document.body.appendChild(a);
			a.click();
			document.body.removeChild(a);
			URL.revokeObjectURL(url);
			useStompStore.getState().unsubscribe(`nspanel/${nspanel_mac}/log_backlog`, download_nspanel_logs_callback);
			setHasDownloadedLogs(true);
		}
	};
	return /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
		className: "w-full bg-base-100 rounded-box p-3",
		children: [/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
			className: "flex items-center justify-between",
			children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", {
				className: "text-lg font-semibold mb-1 block",
				children: "Logs"
			}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)("button", {
				className: "btn btn-outline btn-sm btn-info",
				onClick: () => {
					setHasDownloadedLogs(false);
					useStompStore.getState().subscribe(`nspanel/${nspanel_mac}/log_backlog`, download_nspanel_logs_callback);
				},
				children: "Download"
			})]
		}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
			className: "overflow-x-auto",
			children: /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("table", {
				className: "table table-xs",
				children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("thead", { children: /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("tr", { children: [
					/* @__PURE__ */ (0, import_jsx_runtime.jsx)("th", {
						className: "px-3 py-1",
						children: "Time"
					}),
					/* @__PURE__ */ (0, import_jsx_runtime.jsx)("th", {
						className: "px-3 py-1",
						children: "Level"
					}),
					/* @__PURE__ */ (0, import_jsx_runtime.jsx)("th", {
						className: "px-3 py-1",
						children: "Message"
					})
				] }) }), /* @__PURE__ */ (0, import_jsx_runtime.jsx)("tbody", {
					className: "",
					children: logs.map((log) => /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("tr", {
						className: "hover:bg-base-300",
						children: [
							/* @__PURE__ */ (0, import_jsx_runtime.jsx)("td", {
								className: "px-3 py-1",
								children: log.time
							}),
							(() => {
								switch (log.level) {
									case "TRACE": return /* @__PURE__ */ (0, import_jsx_runtime.jsx)("td", {
										className: "px-3 py-1 text-center bg-white text-black",
										children: log.level
									});
									case "DEBUG": return /* @__PURE__ */ (0, import_jsx_runtime.jsx)("td", {
										className: "px-3 py-1 text-center",
										children: log.level
									});
									case "INFO": return /* @__PURE__ */ (0, import_jsx_runtime.jsx)("td", {
										className: "px-3 py-1 bg-info text-info-content text-center",
										children: log.level
									});
									case "WARNING": return /* @__PURE__ */ (0, import_jsx_runtime.jsx)("td", {
										className: "px-3 py-1 bg-warning text-warning-content text-center",
										children: log.level
									});
									case "ERROR": return /* @__PURE__ */ (0, import_jsx_runtime.jsx)("td", {
										className: "px-3 py-1 bg-error text-error-content text-center",
										children: log.level
									});
									default: return /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("td", {
										className: "px-3 py-1",
										children: ["Unknown log level! ", log.level]
									});
								}
							})(),
							/* @__PURE__ */ (0, import_jsx_runtime.jsx)("td", {
								className: "px-3 py-1",
								children: log.message
							})
						]
					}, `${log.time}-${log.level}-${log.message}`))
				})]
			})
		})]
	});
}
//#endregion
export { NSPanelLogs as default };
