import { c as __toESM, r as require_react, t as require_jsx_runtime } from "./main-2HtbJIdj.js";
import { r as DragDropProvider, t as useSortable } from "./sortable-BQjqvTKl.js";
import { n as FormProvider, s as useForm } from "./index.esm-QFBIAhJI.js";
import { n as Notify } from "./NSPanelToastContainer-0TOVgnPE.js";
import { t as motion } from "./proxy-C3AeBDbu.js";
import { n as useMotionTemplate, r as useMotionValue, t as useSpring } from "./use-spring-DgFnloJ5.js";
import { t as useEntityStatesStore } from "./EntityStore-CC59Fqn-.js";
import { o as mdiMenu, x as require_Icon } from "./mdi-FwvGNUOi.js";
import { c as TextInput, l as api, t as Alert } from "./layout-CJwUXh-n.js";
//#region src/components/RoomsPage/RoomBox.tsx
var import_react = /* @__PURE__ */ __toESM(require_react(), 1);
var import_Icon = require_Icon();
var import_jsx_runtime = require_jsx_runtime();
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
var RoomBoxContent = ({ room, setHandle }) => {
	const boxRef = (0, import_react.useRef)(null);
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
				children: [/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
					className: "flex items-center",
					children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
						ref: setHandle,
						className: "flex",
						children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)(import_Icon.Icon, {
							path: mdiMenu,
							className: "w-5 h-5 inline-block me-1 cursor-grab"
						})
					}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)("a", {
						href: `/rooms/${room.id}`,
						className: "font-medium text-lg flex items-center",
						children: room.friendly_name
					})]
				}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
					className: "flex items-center justify-end",
					children: (() => {
						if (room_nspanels.length <= 0) return /* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", {
							className: "flex items-end justify-end text-right italic font-light text-sm",
							children: "No panels"
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
var RoomBox = ({ room, index }) => {
	const id = room.id;
	const [handle, setHandle] = (0, import_react.useState)(null);
	const { ref } = useSortable({
		id,
		index,
		data: {
			type: "room",
			room
		},
		handle,
		disabled: false
	});
	return /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("li", {
		ref,
		className: "text-base-content rounded-b-box rounded-t-field nspanel-box bg-base-200 hover:bg-base-100 transition-all duration-200 shadow-xl shadow-neutral/3 hover:shadow-neutral/10 overflow-visible h-34",
		children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
			className: "overflow-hidden",
			children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)(StatusHeader, { room })
		}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)(RoomBoxContent, {
			room,
			setHandle
		})]
	});
};
//#endregion
//#region node_modules/@dnd-kit/helpers/dist/index.js
var __defProp = Object.defineProperty;
var __defProps = Object.defineProperties;
var __getOwnPropDescs = Object.getOwnPropertyDescriptors;
var __getOwnPropSymbols = Object.getOwnPropertySymbols;
var __hasOwnProp = Object.prototype.hasOwnProperty;
var __propIsEnum = Object.prototype.propertyIsEnumerable;
var __defNormalProp = (obj, key, value) => key in obj ? __defProp(obj, key, {
	enumerable: true,
	configurable: true,
	writable: true,
	value
}) : obj[key] = value;
var __spreadValues = (a, b) => {
	for (var prop in b || (b = {})) if (__hasOwnProp.call(b, prop)) __defNormalProp(a, prop, b[prop]);
	if (__getOwnPropSymbols) {
		for (var prop of __getOwnPropSymbols(b)) if (__propIsEnum.call(b, prop)) __defNormalProp(a, prop, b[prop]);
	}
	return a;
};
var __spreadProps = (a, b) => __defProps(a, __getOwnPropDescs(b));
function arrayMove(array, from, to) {
	if (from === to) return array;
	const newArray = array.slice();
	newArray.splice(to, 0, newArray.splice(from, 1)[0]);
	return newArray;
}
function hasSortableIndices(source) {
	return "initialIndex" in source && typeof source.initialIndex === "number" && "index" in source && typeof source.index === "number";
}
function mutate(items, event, mutation) {
	var _a, _b, _c;
	const { source, target, canceled } = event.operation;
	if (!source || !target || canceled) {
		if ("preventDefault" in event) event.preventDefault();
		return items;
	}
	const findIndex = (item, id) => item === id || typeof item === "object" && "id" in item && item.id === id;
	if (Array.isArray(items)) {
		const sourceIndex2 = items.findIndex((item) => findIndex(item, source.id));
		const targetIndex2 = items.findIndex((item) => findIndex(item, target.id));
		if (sourceIndex2 === -1 || targetIndex2 === -1) {
			if (hasSortableIndices(source)) {
				const from = source.initialIndex;
				const to = source.index;
				if (from === to || from < 0 || from >= items.length) {
					if ("preventDefault" in event) event.preventDefault();
					return items;
				}
				return mutation(items, from, to);
			}
			return items;
		}
		if (!canceled && "index" in source && typeof source.index === "number") {
			const projectedSourceIndex = source.index;
			if (projectedSourceIndex !== sourceIndex2) return mutation(items, sourceIndex2, projectedSourceIndex);
		}
		return mutation(items, sourceIndex2, targetIndex2);
	}
	const entries = Object.entries(items);
	let sourceIndex = -1;
	let sourceParent;
	let targetIndex = -1;
	let targetParent;
	for (const [id, children] of entries) {
		if (sourceIndex === -1) {
			sourceIndex = children.findIndex((item) => findIndex(item, source.id));
			if (sourceIndex !== -1) sourceParent = id;
		}
		if (targetIndex === -1) {
			targetIndex = children.findIndex((item) => findIndex(item, target.id));
			if (targetIndex !== -1) targetParent = id;
		}
		if (sourceIndex !== -1 && targetIndex !== -1) break;
	}
	if (sourceIndex === -1 && hasSortableIndices(source)) {
		const srcParent = source.initialGroup;
		const srcIndex = source.initialIndex;
		const tgtParent = source.group;
		const tgtIndex = source.index;
		if (srcParent == null || tgtParent == null || !(srcParent in items) || !(tgtParent in items)) {
			if ("preventDefault" in event) event.preventDefault();
			return items;
		}
		if (srcParent === tgtParent && srcIndex === tgtIndex) {
			if ("preventDefault" in event) event.preventDefault();
			return items;
		}
		if (srcParent === tgtParent) return __spreadProps(__spreadValues({}, items), { [srcParent]: mutation(items[srcParent], srcIndex, tgtIndex) });
		const sourceItem2 = items[srcParent][srcIndex];
		return __spreadProps(__spreadValues({}, items), {
			[srcParent]: [...items[srcParent].slice(0, srcIndex), ...items[srcParent].slice(srcIndex + 1)],
			[tgtParent]: [
				...items[tgtParent].slice(0, tgtIndex),
				sourceItem2,
				...items[tgtParent].slice(tgtIndex)
			]
		});
	}
	if (!source.manager) return items;
	const { dragOperation } = source.manager;
	const position = (_b = (_a = dragOperation.shape) == null ? void 0 : _a.current.center) != null ? _b : dragOperation.position.current;
	if (targetParent == null) {
		if (target.id in items) {
			const insertionIndex = target.shape && position.y > target.shape.center.y ? items[target.id].length : 0;
			targetParent = target.id;
			targetIndex = insertionIndex;
		}
	}
	if (sourceParent == null || targetParent == null || sourceParent === targetParent && sourceIndex === targetIndex) {
		if (sourceParent != null && sourceParent === targetParent && sourceIndex === targetIndex && hasSortableIndices(source)) {
			const hasGroupChanged = source.group != null && source.group !== sourceParent;
			const hasIndexChanged = source.index !== sourceIndex;
			if (hasGroupChanged || hasIndexChanged) {
				const reconciledTargetParent = (_c = source.group) != null ? _c : sourceParent;
				if (reconciledTargetParent in items) {
					if (sourceParent === reconciledTargetParent) return __spreadProps(__spreadValues({}, items), { [sourceParent]: mutation(items[sourceParent], sourceIndex, source.index) });
					const sourceItem2 = items[sourceParent][sourceIndex];
					return __spreadProps(__spreadValues({}, items), {
						[sourceParent]: [...items[sourceParent].slice(0, sourceIndex), ...items[sourceParent].slice(sourceIndex + 1)],
						[reconciledTargetParent]: [
							...items[reconciledTargetParent].slice(0, source.index),
							sourceItem2,
							...items[reconciledTargetParent].slice(source.index)
						]
					});
				}
			}
		}
		if ("preventDefault" in event) event.preventDefault();
		return items;
	}
	if (sourceParent === targetParent) return __spreadProps(__spreadValues({}, items), { [sourceParent]: mutation(items[sourceParent], sourceIndex, targetIndex) });
	const modifier = target.shape && Math.round(position.y) > Math.round(target.shape.center.y) ? 1 : 0;
	const sourceItem = items[sourceParent][sourceIndex];
	return __spreadProps(__spreadValues({}, items), {
		[sourceParent]: [...items[sourceParent].slice(0, sourceIndex), ...items[sourceParent].slice(sourceIndex + 1)],
		[targetParent]: [
			...items[targetParent].slice(0, targetIndex + modifier),
			sourceItem,
			...items[targetParent].slice(targetIndex + modifier)
		]
	});
}
function move(items, event) {
	return mutate(items, event, arrayMove);
}
//#endregion
//#region src/components/RoomsPage/NewRoomDialog.tsx
var NewRoomDialog = ({ dialogRef, onCreated }) => {
	const [errorMessage, setErrorMessage] = (0, import_react.useState)(null);
	const form = useForm({ defaultValues: { friendly_name: "" } });
	const { handleSubmit, reset: formReset, formState: { isSubmitting } } = form;
	const reset = () => {
		setErrorMessage(null);
		formReset();
	};
	const onSubmit = async (values) => {
		setErrorMessage(null);
		try {
			const { data, error, response } = await api.POST("/rest/v2/rooms", { body: { friendly_name: values.friendly_name.trim() } });
			if (data) {
				dialogRef.current?.close();
				Notify({
					message: `Room "${data.friendly_name}" created`,
					level: "success",
					duration: 3e3
				});
				onCreated();
				return;
			}
			console.log("Failed to create room, error: ", error);
			setErrorMessage(`Failed to create room (HTTP ${response.status})`);
		} catch (failure) {
			setErrorMessage(failure instanceof Error ? failure.message : String(failure));
		}
	};
	return /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("dialog", {
		ref: dialogRef,
		className: "modal",
		onClose: reset,
		children: [/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
			className: "modal-box",
			children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("h3", {
				className: "text-lg font-bold",
				children: "New room"
			}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)(FormProvider, {
				...form,
				children: /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("form", {
					onSubmit: (event) => void handleSubmit(onSubmit)(event),
					noValidate: true,
					children: [
						/* @__PURE__ */ (0, import_jsx_runtime.jsx)(TextInput, {
							name: "friendly_name",
							label: "Name"
						}),
						errorMessage && /* @__PURE__ */ (0, import_jsx_runtime.jsx)(Alert, {
							level: "error",
							children: errorMessage
						}),
						/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
							className: "modal-action",
							children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("button", {
								type: "button",
								className: "btn",
								disabled: isSubmitting,
								onClick: () => dialogRef.current?.close(),
								children: "Cancel"
							}), /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("button", {
								type: "submit",
								className: "btn btn-info",
								disabled: isSubmitting,
								children: [isSubmitting && /* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", { className: "loading loading-spinner loading-xs" }), "Create"]
							})]
						})
					]
				})
			})]
		}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)("form", {
			method: "dialog",
			className: "modal-backdrop",
			children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)("button", { children: "close" })
		})]
	});
};
//#endregion
//#region src/components/RoomsPage/RoomsView.tsx
var RoomsView = () => {
	const [state, setState] = (0, import_react.useState)({ status: "loading" });
	const { status: entityStatesStatus, fetchData } = useEntityStatesStore();
	const newRoomDialogRef = (0, import_react.useRef)(null);
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
	(0, import_react.useEffect)(() => {
		if (entityStatesStatus === "none") fetchData();
	}, [entityStatesStatus, fetchData]);
	(0, import_react.useEffect)(fetchSettings, [fetchSettings]);
	return /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", { children: [
		/* @__PURE__ */ (0, import_jsx_runtime.jsx)(NewRoomDialog, {
			dialogRef: newRoomDialogRef,
			onCreated: fetchSettings
		}),
		/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
			className: "flex items-center justify-between",
			children: [/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
				className: "flex items-center justify-start gap-1",
				children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", {
					className: "text-2xl",
					children: "Rooms"
				}), (() => {
					if (entityStatesStatus != "loaded") return /* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", { className: "loading loading-spinner loading-sm mr-2" });
					return null;
				})()]
			}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", { children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)("button", {
				className: "btn btn-info font-medium",
				onClick: () => newRoomDialogRef.current?.showModal(),
				children: "New room"
			}) })]
		}),
		/* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", { children: (() => {
			if (state.status != "loaded") return /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
				className: "flex w-full items-center justify-center text-2xl",
				children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", { className: "loading loading-spinner loading-sm mr-2" }), /* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", {
					className: "font-medium",
					children: "Loading rooms..."
				})]
			});
			if (state.status == "loaded" && state.rooms.length > 0) return /* @__PURE__ */ (0, import_jsx_runtime.jsx)(DragDropProvider, {
				onDragStart: (event) => {
					console.log("Drag start: ", event);
				},
				onDragEnd: (event) => {
					if (event.canceled) return;
					const previousState = state;
					const reordered = move(state.rooms, event).map((room, index) => ({
						...room,
						displayOrder: index
					}));
					api.PATCH("/rest/v2/rooms", { body: { rooms: reordered.map((room, index) => ({
						id: room.id,
						displayOrder: index + 1
					})) } }).then(({ data, error, response }) => {
						if (data) {
							setState({
								status: "loaded",
								rooms: data
							});
							return;
						}
						setState(previousState);
						console.error("Failed to save room", error);
						Notify({
							message: `Failed to save room order (HTTP ${response.status})`,
							level: "error",
							duration: 5e3
						});
					}).catch((error) => {
						setState(previousState);
						Notify({
							message: error instanceof Error ? error.message : String(error),
							level: "error",
							duration: 5e3
						});
					});
				},
				children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)("ul", {
					className: "grid md:grid-cols-3 gap-4 mt-4 items-stretch",
					id: "rooms_container",
					children: state.rooms.map((room, index) => {
						return /* @__PURE__ */ (0, import_jsx_runtime.jsx)(RoomBox, {
							room,
							index
						}, `room-id-${room.id}`);
					})
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
		})() })
	] });
};
//#endregion
export { RoomsView as default };
