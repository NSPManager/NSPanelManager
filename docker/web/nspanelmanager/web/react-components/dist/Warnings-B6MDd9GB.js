import { c as __toESM, r as require_react, t as require_jsx_runtime } from "./main-Dhmdi5Xt.js";
import { t as useEntityStatesStore } from "./EntityStore-CTp1Ux3g.js";
import { y as require_Icon } from "./mdi-DbmAi7-6.js";
//#region src/components/NSPanelPage/Warnings.tsx
var import_react = /* @__PURE__ */ __toESM(require_react(), 1);
var import_Icon = require_Icon();
var import_jsx_runtime = require_jsx_runtime();
var NSPanelWarning = ({ warning }) => {
	return /* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
		className: "block p-3",
		children: /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
			className: "grid grid-cols-[auto_1fr] items-start justify-start gap-2 w-full",
			children: [/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
				className: "grid-cols-subgrid col-span-2 flex items-center justify-start",
				children: [
					warning.level == "error" && /* @__PURE__ */ (0, import_jsx_runtime.jsx)(import_Icon.Icon, {
						path: "M13,13H11V7H13M13,17H11V15H13M12,2A10,10 0 0,0 2,12A10,10 0 0,0 12,22A10,10 0 0,0 22,12A10,10 0 0,0 12,2Z",
						className: "w-4 h-4 text-error mr-2"
					}),
					warning.level == "warning" && /* @__PURE__ */ (0, import_jsx_runtime.jsx)(import_Icon.Icon, {
						path: "M13,13H11V7H13M13,17H11V15H13M12,2A10,10 0 0,0 2,12A10,10 0 0,0 12,22A10,10 0 0,0 22,12A10,10 0 0,0 12,2Z",
						className: "w-4 h-4 text-warning mr-2"
					}),
					warning.level == "info" && /* @__PURE__ */ (0, import_jsx_runtime.jsx)(import_Icon.Icon, {
						path: "M13,13H11V7H13M13,17H11V15H13M12,2A10,10 0 0,0 2,12A10,10 0 0,0 12,22A10,10 0 0,0 22,12A10,10 0 0,0 12,2Z",
						className: "w-4 h-4 text-info mr-2"
					}),
					warning.level == "debug" && /* @__PURE__ */ (0, import_jsx_runtime.jsx)(import_Icon.Icon, {
						path: "M13,13H11V7H13M13,17H11V15H13M12,2A10,10 0 0,0 2,12A10,10 0 0,0 12,22A10,10 0 0,0 22,12A10,10 0 0,0 12,2Z",
						className: "w-4 h-4 text-base-content mr-2"
					}),
					/* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
						className: "flex",
						children: warning.text
					})
				]
			}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
				className: "grid-cols-subgrid col-span-2 col-start-2",
				children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
					className: "text-xs pl-4 uppercase font-semibold opacity-60",
					children: warning.level.charAt(0).toUpperCase() + warning.level.slice(1)
				})
			})]
		})
	}, warning.text);
};
function NSPanelWarnings({ nspanel_mac }) {
	const { nspanels, status, fetchData } = useEntityStatesStore();
	(0, import_react.useEffect)(() => {
		if (status === "none") fetchData();
	}, [status, fetchData]);
	if (status == "loading" || status == "none") return null;
	else if (status == "loaded" && !nspanels[nspanel_mac]) return /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("span", {
		className: "text-error text-md",
		children: [
			"NSPanel state not found for panel with MAC: ",
			nspanel_mac,
			"!"
		]
	});
	if (nspanels[nspanel_mac].warnings.length == 0) return null;
	return /* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
		className: "divide-y divide-neutral w-full backdrop-blur-none backdrop-brightness-75 rounded-box border border-warning/50",
		children: nspanels[nspanel_mac].warnings.map((warning) => {
			return /* @__PURE__ */ (0, import_jsx_runtime.jsx)(NSPanelWarning, { warning });
		})
	});
}
//#endregion
export { NSPanelWarnings as default };
