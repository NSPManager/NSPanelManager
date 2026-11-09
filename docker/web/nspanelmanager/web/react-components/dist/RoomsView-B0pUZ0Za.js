import { c as __toESM, r as require_react, t as require_jsx_runtime } from "./main-BYMetvRG.js";
import { n as Notify } from "./NSPanelToastContainer-IQT17CG7.js";
import { t as motion } from "./proxy-D5KUi63d.js";
import { n as useMotionTemplate, r as useMotionValue, t as useSpring } from "./use-spring-DJAd9Qe2.js";
import "./StompStore-BCdW0Jty.js";
import { t as useEntityStatesStore } from "./EntityStore-wGiBVZVW.js";
import { o as mdiMenu, x as require_Icon } from "./mdi-DocwAJIe.js";
import { t as api } from "./client-CRfCe5X7.js";
//#region src/components/RoomsPage/RoomBox.tsx
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
var delete_room = (nspanel_id) => {
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
(0, import_react.forwardRef)(({ status, delete_nspanel_dialog_ref }, ref) => {
	return /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("dialog", {
		ref,
		className: "modal",
		children: [/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
			className: "modal-box",
			children: [
				/* @__PURE__ */ (0, import_jsx_runtime.jsx)("h3", {
					className: "text-lg font-bold",
					children: "Delete Room"
				}),
				/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("p", {
					className: "py-4",
					children: [
						"Are you sure you want to delete Room '",
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
							delete_room(status.id);
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
var NSPanelBox = ({ nspanel }) => {
	const state = nspanel.state;
	return /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
		className: "badge badge-md bg-base-300",
		children: [(() => {
			if (state == "online") return /* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", { className: "indicator-item status status-success" });
			else if (state == "offline") return /* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", { className: "indicator-item status status-error" });
			else if (state == "rebooting") return /* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", { className: "indicator-item status status-info animate-pulse" });
			else if (state == "waiting") return /* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", { className: "indicator-item status status-info animate-pulse" });
			else if (state == "updating_fw" || state == "updating_fs" || state == "updating_tft") return /* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", { className: "indicator-item status status-info animate-pulse" });
			else return /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("span", {
				className: "indicator-item status status-error animate-ping",
				children: ["Unknown state: ", state]
			});
		})(), /* @__PURE__ */ (0, import_jsx_runtime.jsx)("a", {
			href: `/nspanel/${nspanel.id}`,
			children: nspanel.name
		})]
	});
};
var StatusHeader = ({ room }) => {
	const { nspanels, status, fetchData } = useEntityStatesStore();
	(0, import_react.useEffect)(() => {
		if (status === "none") fetchData();
	}, [status, fetchData]);
	const room_nspanels = [];
	let any_panel_online = false;
	Object.values(nspanels).forEach((nspanel) => {
		if (nspanel.default_room_id == room.id) {
			room_nspanels.push(nspanel);
			if (nspanel.state == "online" || nspanel.state == "rebooting" || nspanel.state == "updating_fs" || nspanel.state == "updating_fw" || nspanel.state == "updating_tft") any_panel_online = true;
		}
	});
	if (any_panel_online) return /* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
		className: "min-h-1 overflow-hidden w-full transition-all ease-linear duration-300 bg-success rounded-t-field",
		id: "panel_header-${id}"
	});
	else if (room_nspanels.length > 0 && !any_panel_online) return /* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
		className: "min-h-1 overflow-hidden w-full transition-all ease-linear duration-300 bg-error rounded-t-field",
		id: "panel_header-${id}"
	});
	else return /* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
		className: "min-h-1 overflow-hidden w-full transition-all ease-linear duration-300 bg-base-100 rounded-t-field",
		id: "panel_header-${id}"
	});
};
var RoomBoxContent = ({ room }) => {
	(0, import_react.useRef)(null);
	const boxRef = (0, import_react.useRef)(null);
	(0, import_react.useRef)(null);
	const mouseX = useMotionValue(0);
	const mouseY = useMotionValue(0);
	const springX = useSpring(mouseX, {
		stiffness: 300,
		damping: 30
	});
	const springY = useSpring(mouseY, {
		stiffness: 300,
		damping: 30
	});
	const clipPath = useMotionTemplate`radial-gradient(circle at ${springX}px ${springY}px, black 0%, black 15%, transparent 50%)`;
	(0, import_react.useEffect)(() => {
		const handleMouseMove = (e) => {
			const boundingRect = boxRef.current?.getBoundingClientRect();
			if (!boundingRect) return;
			mouseX.set(e.clientX - boundingRect.left);
			mouseY.set(e.clientY - boundingRect.top);
		};
		window.addEventListener("mousemove", handleMouseMove);
		return () => {
			window.removeEventListener("mousemove", handleMouseMove);
		};
	}, [mouseX, mouseY]);
	const { nspanels, status, fetchData } = useEntityStatesStore();
	(0, import_react.useEffect)(() => {
		if (status === "none") fetchData();
	}, [status, fetchData]);
	const room_nspanels = [];
	let number_panels_online = 0;
	let bg_class = "bg-base-100";
	Object.values(nspanels).forEach((nspanel) => {
		if (nspanel.default_room_id == room.id) {
			room_nspanels.push(nspanel);
			if (nspanel.state == "online" || nspanel.state == "rebooting" || nspanel.state == "updating_fs" || nspanel.state == "updating_fw" || nspanel.state == "updating_tft") number_panels_online++;
		}
	});
	if (room_nspanels.length > 0 && number_panels_online > 0) bg_class = "bg-success";
	else if (room_nspanels.length > 0 && number_panels_online <= 0) bg_class = "bg-error";
	return /* @__PURE__ */ (0, import_jsx_runtime.jsx)(import_jsx_runtime.Fragment, { children: /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
		className: "relative h-full group/box",
		ref: boxRef,
		children: [/* @__PURE__ */ (0, import_jsx_runtime.jsxs)(motion.div, {
			style: {
				WebkitMaskImage: clipPath,
				maskImage: clipPath,
				pointerEvents: "none"
			},
			className: "absolute tile-background w-full h-full group overflow-hidden mb-1 opacity-0 group-hover/box:opacity-5",
			children: [
				/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
					className: "grid grid-cols-8 w-full h-full inset-0",
					children: [
						/* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", { className: `w-full h-full ${bg_class} [animation-name:tile-background] [animation-duration:8s] [animation-iteration-count:infinite] opacity-0 [animation-delay:-2s] tile-1 group/box:opacity-50` }),
						/* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", { className: `w-full h-full ${bg_class} [animation-name:tile-background] [animation-duration:8s] [animation-iteration-count:infinite] opacity-0 [animation-delay:-6s] tile-2 group/box:opacity-50` }),
						/* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", { className: `w-full h-full ${bg_class} [animation-name:tile-background] [animation-duration:8s] [animation-iteration-count:infinite] opacity-0 [animation-delay:-4s] tile-3 group/box:opacity-50` }),
						/* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", { className: `w-full h-full ${bg_class} [animation-name:tile-background] [animation-duration:8s] [animation-iteration-count:infinite] opacity-0 [animation-delay:-8s] tile-4 group/box:opacity-50` }),
						/* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", { className: `w-full h-full ${bg_class} [animation-name:tile-background] [animation-duration:8s] [animation-iteration-count:infinite] opacity-0 [animation-delay:-4s] tile-1 group/box:opacity-50` }),
						/* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", { className: `w-full h-full ${bg_class} [animation-name:tile-background] [animation-duration:8s] [animation-iteration-count:infinite] opacity-0 [animation-delay:-2s] tile-2 group/box:opacity-50` }),
						/* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", { className: `w-full h-full ${bg_class} [animation-name:tile-background] [animation-duration:8s] [animation-iteration-count:infinite] opacity-0 [animation-delay:-8s] tile-3 group/box:opacity-50` }),
						/* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", { className: `w-full h-full ${bg_class} [animation-name:tile-background] [animation-duration:8s] [animation-iteration-count:infinite] opacity-0 [animation-delay:-2s] tile-4 group/box:opacity-50` }),
						/* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", { className: `w-full h-full ${bg_class} [animation-name:tile-background] [animation-duration:8s] [animation-iteration-count:infinite] opacity-0 [animation-delay:-4s] tile-1 group/box:opacity-50` }),
						/* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", { className: `w-full h-full ${bg_class} [animation-name:tile-background] [animation-duration:8s] [animation-iteration-count:infinite] opacity-0 [animation-delay:-8s] tile-2 group/box:opacity-50` }),
						/* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", { className: `w-full h-full ${bg_class} [animation-name:tile-background] [animation-duration:8s] [animation-iteration-count:infinite] opacity-0 [animation-delay:-6s] tile-3 group/box:opacity-50` }),
						/* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", { className: `w-full h-full ${bg_class} [animation-name:tile-background] [animation-duration:8s] [animation-iteration-count:infinite] opacity-0 [animation-delay:-4s] tile-4 group/box:opacity-50` }),
						/* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", { className: `w-full h-full ${bg_class} [animation-name:tile-background] [animation-duration:8s] [animation-iteration-count:infinite] opacity-0 [animation-delay:-8s] tile-1 group/box:opacity-50` }),
						/* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", { className: `w-full h-full ${bg_class} [animation-name:tile-background] [animation-duration:8s] [animation-iteration-count:infinite] opacity-0 [animation-delay:-6s] tile-2 group/box:opacity-50` }),
						/* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", { className: `w-full h-full ${bg_class} [animation-name:tile-background] [animation-duration:8s] [animation-iteration-count:infinite] opacity-0 [animation-delay:-2s] tile-3 group/box:opacity-50` }),
						/* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", { className: `w-full h-full ${bg_class} [animation-name:tile-background] [animation-duration:8s] [animation-iteration-count:infinite] opacity-0 [animation-delay:-4s] tile-4 group/box:opacity-50` }),
						/* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", { className: `w-full h-full col-span-4 ${bg_class} [animation-name:tile-background] [animation-duration:8s] [animation-iteration-count:infinite] opacity-0 [animation-delay:-6s] tile-1 group/box:opacity-50` }),
						/* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", { className: `w-full h-full col-span-4 ${bg_class} [animation-name:tile-background] [animation-duration:8s] [animation-iteration-count:infinite] opacity-0 [animation-delay:-2s] tile-2 group/box:opacity-50` })
					]
				}),
				/* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", { className: "absolute left-1/8 top-0 w-px h-full bg-base-content" }),
				/* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", { className: "absolute left-2/8 top-0 w-px h-full bg-base-content" }),
				/* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", { className: "absolute left-3/8 top-0 w-px h-full bg-base-content" }),
				/* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", { className: "absolute left-4/8 top-0 w-px h-full bg-base-content" }),
				/* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", { className: "absolute left-5/8 top-0 w-px h-full bg-base-content" }),
				/* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", { className: "absolute left-6/8 top-0 w-px h-full bg-base-content" }),
				/* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", { className: "absolute left-7/8 top-0 w-px h-full bg-base-content" }),
				/* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", { className: "absolute left-8/8 top-0 w-px h-full bg-base-content" }),
				/* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", { className: "absolute left-0 top-1/3 w-full h-px bg-base-content" }),
				/* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", { className: "absolute left-0 top-2/3 w-full h-px bg-base-content" }),
				/* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", { className: "absolute left-0 top-3/3 w-full h-px bg-base-content" })
			]
		}), /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
			className: "p-2 h-full",
			children: [/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
				className: "flex justify-between mx-auto",
				children: [/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("a", {
					href: `/rooms/${room.id}`,
					className: "font-medium text-lg flex items-center",
					children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)(import_Icon.Icon, {
						path: mdiMenu,
						className: "w-5 h-5 inline-block me-1"
					}), room.friendly_name]
				}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
					className: "flex items-center justify-end",
					children: (() => {
						if (room_nspanels.length <= 0) return /* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", {
							className: "flex items-end justify-end text-right italic font-light text-sm",
							children: "No nspanels"
						});
						else return /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("span", {
							className: "flex items-end justify-end text-right italic font-light text-xs",
							children: [
								number_panels_online,
								" of ",
								room_nspanels.length,
								" online"
							]
						});
					})()
				})]
			}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
				className: "flex flex-wrap gap-1 mt-2",
				children: room_nspanels.map((nspanel) => /* @__PURE__ */ (0, import_jsx_runtime.jsx)(NSPanelBox, { nspanel }))
			})]
		})]
	}) });
};
var RoomBox = ({ room }) => {
	return /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
		className: "text-base-content rounded-b-box rounded-t-field nspanel-box bg-base-200 hover:bg-base-100 transition-all duration-200 shadow-xl shadow-neutral/3 hover:shadow-neutral/10 overflow-visible h-34",
		children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
			className: "overflow-hidden",
			children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)(StatusHeader, { room })
		}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)(RoomBoxContent, { room })]
	});
};
//#endregion
//#region src/components/RoomsPage/RoomsView.tsx
var RoomsView = () => {
	(0, import_react.useRef)(null);
	const [state, setState] = (0, import_react.useState)({ status: "loading" });
	const fetchSettings = (0, import_react.useCallback)(() => {
		api.GET("/rest/v2/rooms").then(({ data, response }) => setState(data ? {
			status: "loaded",
			rooms: data
		} : {
			status: "error",
			message: `HTTP ${response.status}`
		})).catch((error) => setState({
			status: "error",
			message: error instanceof Error ? error.message : String(error)
		}));
	}, []);
	(0, import_react.useEffect)(fetchSettings, [fetchSettings]);
	return /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", { children: [/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
		className: "flex items-center justify-between",
		children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", {
			className: "text-2xl",
			children: "Rooms"
		}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", { children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)("button", {
			className: "btn btn-info font-medium",
			children: "New room"
		}) })]
	}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", { children: (() => {
		if (state.status != "loaded") return /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
			className: "flex w-full items-center justify-center text-2xl",
			children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", { className: "loading loading-spinner loading-sm mr-2" }), /* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", {
				className: "font-medium",
				children: "Loading rooms..."
			})]
		});
		if (state.status == "loaded" && state.rooms.length > 0) return /* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
			className: "grid md:grid-cols-3 gap-4 mt-4 items-stretch",
			id: "rooms_container",
			children: state.rooms.map((room) => {
				return /* @__PURE__ */ (0, import_jsx_runtime.jsx)(RoomBox, { room });
			})
		});
		else return /* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
			className: "grid md:grid-cols-3 gap-4 mt-4 items-start",
			id: "rooms_container",
			children: /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
				className: "col-span-3 w-full text-center text-xl text-base-content",
				children: [
					"No Rooms found. Getting started?",
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
export { RoomsView as default };
