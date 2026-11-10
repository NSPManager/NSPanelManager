import { c as __toESM, r as require_react, t as require_jsx_runtime } from "./main-2HtbJIdj.js";
import { a as useDroppable, i as useDraggable, n as isSortable, r as DragDropProvider, t as useSortable } from "./sortable-BQjqvTKl.js";
import { t as create } from "./react-DlvZzTUG.js";
import { i as useEntitiesPagesStore, n as MultiStep_AddOrEditEntity } from "./MultiStep_AddEditEntity-D9orgM2n.js";
//#region src/components/EntitiesPage/GenericEntityBox.tsx
var import_react = /* @__PURE__ */ __toESM(require_react(), 1);
var import_jsx_runtime = require_jsx_runtime();
var GenericEntityBox = ({ entity, draging_item }) => {
	const { ref } = useDraggable({
		id: `entity-${entity.id}`,
		data: {
			type: "entity",
			config: entity
		}
	});
	const { entities_pages, removeEntity, fetchData } = useEntitiesPagesStore();
	const entity_page = entities_pages.find((page) => page.id === entity.entities_page_id);
	const removeEntityDialogRef = (0, import_react.useRef)(null);
	const [editDialogOpened, setEditDialogOpened] = (0, import_react.useState)(false);
	if (entity_page == void 0) return null;
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
	async function deleteEntity(id) {
		console.log("Deleting entity", id);
		if (entities_pages.find((page) => page.id === entity.entities_page_id)) fetch(`/rest/entities/${id}`, {
			credentials: "same-origin",
			method: "DELETE",
			mode: "same-origin",
			headers: {
				"Content-Type": "application/json",
				"X-CSRFToken": getCookie("csrftoken")
			}
		}).then(async (response) => {
			if (response.ok) {
				console.log("Successfully deleted entity", id);
				removeEntity(id);
			}
		}).catch((error) => {
			console.error("Error deleting entity", error);
		});
	}
	return /* @__PURE__ */ (0, import_jsx_runtime.jsxs)(import_jsx_runtime.Fragment, { children: [
		/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("dialog", {
			ref: removeEntityDialogRef,
			className: "modal",
			children: [/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
				className: "modal-box",
				children: [
					/* @__PURE__ */ (0, import_jsx_runtime.jsx)("h3", {
						className: "text-lg font-bold",
						children: "Delete entities page"
					}),
					/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("p", {
						className: "py-4",
						children: [
							"Are you sure you want to delete entity \"",
							entity.friendly_name,
							"\"?"
						]
					}),
					/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
						className: "flex justify-end join",
						children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("button", {
							onClick: () => removeEntityDialogRef.current?.close(),
							className: "btn btn-neutral join-item",
							children: "Cancel"
						}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)("button", {
							onClick: () => {
								removeEntityDialogRef.current?.close();
								deleteEntity(entity.id);
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
		}),
		/* @__PURE__ */ (0, import_jsx_runtime.jsx)(MultiStep_AddOrEditEntity, {
			type: entity.entity_type,
			room_id: entity_page.room_id,
			entities_page_id: entity.entities_page_id,
			room_view_position: entity.room_view_position,
			id: entity.id,
			opened: editDialogOpened,
			setOpened: setEditDialogOpened,
			onComplete: () => {
				fetchData(entity_page.room_id);
				setEditDialogOpened(false);
			}
		}),
		/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
			ref,
			className: `draggable-entity-item bg-neutral/50 text-neutral-content rounded-box flex items-center justify-center indicator w-full h-full ${draging_item?.type != "entity_page" ? "hover:outline-1 hover:outline-accent" : ""} cursor-grab`,
			title: "Drag & drop to move this entity",
			children: [
				(() => {
					if (entity.controlled_by_nspanel_main_page) return /* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", {
						className: "indicator-item badge badge-secondary me-16 w-6 h-6 flex items-center justify-center cursor-default",
						title: "Controlled from main page",
						children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", { className: "mdi mdi-home" })
					});
					return null;
				})(),
				/* @__PURE__ */ (0, import_jsx_runtime.jsx)("button", {
					className: "indicator-item badge badge-info me-8 w-6 h-6 flex items-center justify-center cursor-pointer",
					title: "Edit entity",
					onClick: () => {
						setEditDialogOpened(true);
					},
					children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", { className: "mdi mdi-pencil" })
				}),
				/* @__PURE__ */ (0, import_jsx_runtime.jsx)("button", {
					className: "indicator-item badge badge-warning hover:badge-error w-6 h-6 flex items-center justify-center cursor-pointer",
					title: "Remove entity from page/room",
					onClick: () => {
						removeEntityDialogRef.current?.showModal();
					},
					children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", { className: "mdi mdi-close" })
				}),
				(() => {
					if (entity.controller == "home_assistant") return /* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
						className: "status status-info shadow-none absolute top-2 left-2 cursor-default",
						title: "Controlled by Home Assistant"
					});
					else if (entity.controller == "openhab") return /* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
						className: "status status-warning shadow-none absolute top-2 left-2 cursor-default",
						title: "Controlled by OpenHAB"
					});
					else if (entity.controller == "nspm_scene" || entity.controller == "nspm") return /* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
						className: "status status-accent shadow-none absolute top-2 left-2 cursor-default",
						title: "Controlled by NSPanel Manager"
					});
					else return /* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
						className: "status status-error animate-ping absolute top-2 left-2 cursor-default",
						title: "Unknown controller"
					});
				})(),
				/* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", {
					className: "text-sm m-2",
					children: entity.friendly_name
				})
			]
		})
	] });
};
//#endregion
//#region src/components/EntitiesPage/GenericSceneBox.tsx
var GenericSceneBox = ({ scene, draging_item }) => {
	const { ref } = useDraggable({
		id: `scene-${scene.id}`,
		data: {
			type: "scene",
			config: scene
		}
	});
	const { entities_pages, removeScene, fetchData } = useEntitiesPagesStore();
	const entity_page = entities_pages.find((page) => page.id === scene.entities_page_id);
	const removeSceneDialogRef = (0, import_react.useRef)(null);
	const [editDialogOpened, setEditDialogOpened] = (0, import_react.useState)(false);
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
	async function deleteScene(id) {
		console.log("Deleting scene", id);
		if (entity_page) fetch(`/rest/scenes/${id}`, {
			credentials: "same-origin",
			method: "DELETE",
			mode: "same-origin",
			headers: {
				"Content-Type": "application/json",
				"X-CSRFToken": getCookie("csrftoken")
			}
		}).then(async (response) => {
			if (response.ok) {
				console.log("Successfully deleted scene", id);
				removeScene(id);
			}
		}).catch((error) => {
			console.error("Error deleting scene", error);
		});
	}
	if (!entity_page) return null;
	return /* @__PURE__ */ (0, import_jsx_runtime.jsxs)(import_jsx_runtime.Fragment, { children: [
		/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("dialog", {
			ref: removeSceneDialogRef,
			className: "modal",
			children: [/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
				className: "modal-box",
				children: [
					/* @__PURE__ */ (0, import_jsx_runtime.jsx)("h3", {
						className: "text-lg font-bold",
						children: "Delete scene"
					}),
					/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("p", {
						className: "py-4",
						children: [
							"Are you sure you want to delete scene \"",
							scene.friendly_name,
							"\"?"
						]
					}),
					/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
						className: "flex justify-end join",
						children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("button", {
							onClick: () => removeSceneDialogRef.current?.close(),
							className: "btn btn-neutral join-item",
							children: "Cancel"
						}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)("button", {
							onClick: () => {
								removeSceneDialogRef.current?.close();
								deleteScene(scene.id);
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
		}),
		/* @__PURE__ */ (0, import_jsx_runtime.jsx)(MultiStep_AddOrEditEntity, {
			type: "scene",
			room_id: entity_page.room_id,
			entities_page_id: scene.entities_page_id,
			room_view_position: scene.room_view_position,
			id: scene.id,
			opened: editDialogOpened,
			setOpened: setEditDialogOpened,
			onComplete: () => {
				fetchData(entity_page.room_id);
				setEditDialogOpened(false);
			}
		}),
		/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
			ref,
			className: `draggable-entity-item bg-neutral/50 rounded-box text-neutral-content flex items-center justify-center indicator w-full h-full ${draging_item ? "hover:outline-1 hover:outline-accent" : ""} cursor-grab`,
			title: "Drag & drop to move this entity",
			children: [
				/* @__PURE__ */ (0, import_jsx_runtime.jsx)("button", {
					className: "indicator-item badge badge-info me-8 w-6 h-6 flex items-center justify-center cursor-pointer",
					title: "Edit scene",
					onClick: () => setEditDialogOpened(true),
					children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", { className: "mdi mdi-pencil" })
				}),
				/* @__PURE__ */ (0, import_jsx_runtime.jsx)("button", {
					className: "indicator-item badge badge-warning hover:badge-error w-6 h-6 flex items-center justify-center cursor-pointer",
					title: "Remove scene from page/room",
					onClick: () => {
						removeSceneDialogRef.current?.showModal();
					},
					children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", { className: "mdi mdi-close" })
				}),
				(() => {
					if (scene.controller == "home_assistant") return /* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
						className: "status status-info shadow-none absolute top-2 left-2 cursor-default",
						title: "Controlled by Home Assistant"
					});
					else if (scene.controller == "openhab") return /* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
						className: "status status-warning shadow-none absolute top-2 left-2 cursor-default",
						title: "Controlled by OpenHAB"
					});
					else if (scene.controller == "nspm_scene" || scene.controller == "nspm") return /* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
						className: "status status-accent shadow-none absolute top-2 left-2 cursor-default",
						title: "Controlled by NSPanel Manager"
					});
					else return /* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
						className: "status status-error animate-ping absolute top-2 left-2 cursor-default",
						title: "Unknown controller"
					});
				})(),
				/* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", {
					className: "text-sm m-2",
					children: scene.friendly_name
				})
			]
		})
	] });
};
//#endregion
//#region src/components/EntitiesPage/EntitiesPageDropTarget.tsx
var EntitiesDropTarget = (0, import_react.forwardRef)(({ entities_page_id, room_view_position, type, children, draging_item }) => {
	const [addEditEntityDialogOpened, setAddEditEntityDialogOpened] = (0, import_react.useState)(false);
	const { entities_pages, fetchData } = useEntitiesPagesStore();
	const entities_page = entities_pages.find((page) => page.id === entities_page_id);
	const { ref, isDropTarget } = useDroppable({
		id: `entities_page-${entities_page_id}-room_view_position-${room_view_position}`,
		data: {
			accepts: type,
			type: "entities_page_drop_target",
			config: {
				type,
				entities_page_id,
				room_view_position
			}
		}
	});
	if (entities_page === void 0) return null;
	const isChildDragging = draging_item && draging_item.config && draging_item.config.entities_page_id === entities_page_id && draging_item.config.room_view_position === room_view_position;
	return /* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
		ref,
		children: (() => {
			if (isChildDragging) return /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
				className: `rounded-box p-1 items-center justify-center w-full h-full bg-conic/[from_var(--border-angle)] from-base-200 via-accent/50 to-base-200 animate-rotate-border from-30% to-60%`,
				children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
					className: "flex items-center justify-center rounded-box w-full h-full bg-base-200",
					children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", {
						className: "italic font-thin",
						children: draging_item.config && draging_item.config.friendly_name
					})
				}), children]
			});
			else if (children) return /* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
				className: `rounded-box ${draging_item && draging_item.type != "entity_page" && isDropTarget ? "border-2 border-solid border-accent" : ""} items-center justify-center w-full h-full`,
				children
			});
			return /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
				className: `rounded-box ${draging_item && draging_item.type != "entity_page" && isDropTarget ? "border-2 border-solid border-accent" : "border-dashed border-2 border-neutral/50"} flex items-center justify-center indicator w-full h-full`,
				children: [
					/* @__PURE__ */ (0, import_jsx_runtime.jsx)(MultiStep_AddOrEditEntity, {
						type,
						room_id: entities_page.room_id,
						entities_page_id,
						room_view_position,
						onComplete: () => {
							fetchData(entities_page.room_id);
							setAddEditEntityDialogOpened(false);
						},
						opened: addEditEntityDialogOpened,
						setOpened: setAddEditEntityDialogOpened
					}),
					/* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", {
						className: "italic font-thin",
						children: draging_item?.type !== "entity_page" && isDropTarget && draging_item && draging_item.config ? draging_item.config.friendly_name : "No " + (type === "scene" ? "scene" : "entity") + " set"
					}),
					/* @__PURE__ */ (0, import_jsx_runtime.jsx)("button", {
						hidden: draging_item?.type !== "entity_page" && isDropTarget,
						className: "btn btn-neutral hover:btn-success btn-xs btn-circle ms-2 flex items-center justify-center",
						title: `Add ${type === "scene" ? "scene" : "entity"}`,
						onClick: () => setAddEditEntityDialogOpened(true),
						children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", { className: "mdi mdi-plus" })
					})
				]
			});
		})()
	});
});
//#endregion
//#region src/components/EntitiesPage/EntitiesPage.tsx
var EntitiesPage = ({ id, draging_item, deleteEntitiesPage }) => {
	const { entities_pages, entities, scenes } = useEntitiesPagesStore();
	const page = entities_pages.find((p) => p.id === id);
	const [pageData, setPageData] = (0, import_react.useState)({
		room_id: page ? page.room_id : -1,
		id,
		can_remove: page ? entities_pages.filter((p) => p.type == page.type).length > 1 : false,
		type: page ? page.type : "unknown",
		number_of_entities: page ? page.number_of_entities : 12
	});
	const [editPageTypeOpen, setEditPageTypeOpen] = (0, import_react.useState)(false);
	const gridRows = pageData.number_of_entities === 12 ? "grid-rows-6" : pageData.number_of_entities === 8 ? "grid-rows-4" : "grid-rows-2";
	const dialogRef = (0, import_react.useRef)(null);
	const entitiesPageNumberOfEntitiesDiv = (0, import_react.useRef)(null);
	const [handle, setHandle] = (0, import_react.useState)(null);
	const { ref } = useSortable({
		id,
		index: page ? page.display_order : 0,
		data: {
			type: "entity_page",
			entity_page_config: page
		},
		handle,
		disabled: !pageData.can_remove
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
	async function setPageNumberOfEntities(type) {
		setPageData({
			...pageData,
			number_of_entities: type
		});
		setEditPageTypeOpen(false);
		fetch(`/rest/entities_pages/${pageData.id}`, {
			credentials: "same-origin",
			method: "PUT",
			mode: "same-origin",
			headers: {
				"Content-Type": "application/json",
				"X-CSRFToken": getCookie("csrftoken")
			},
			body: JSON.stringify({
				...pageData,
				number_of_entities: type
			})
		}).then(async (response) => {
			const data = await response.json();
			if (!response.ok) {
				const error = data && data.message || response.status;
				return Promise.reject(error);
			}
		}).catch((error) => {
			console.error("There was an error!", error);
		});
	}
	(0, import_react.useEffect)(() => {
		const listener = (event) => {
			if (entitiesPageNumberOfEntitiesDiv.current && !entitiesPageNumberOfEntitiesDiv.current.contains(event.target)) setEditPageTypeOpen(false);
		};
		window.addEventListener("click", listener);
		return () => {
			window.removeEventListener("click", listener);
		};
	}, [entitiesPageNumberOfEntitiesDiv, editPageTypeOpen]);
	return /* @__PURE__ */ (0, import_jsx_runtime.jsxs)(import_jsx_runtime.Fragment, { children: [/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("dialog", {
		ref: dialogRef,
		className: "modal",
		children: [/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
			className: "modal-box",
			children: [
				/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("h3", {
					className: "text-lg font-bold",
					children: [
						"Delete ",
						pageData.type,
						" page"
					]
				}),
				/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("p", {
					className: "py-4",
					children: [
						"Are you sure you want to delete this ",
						pageData.type,
						" page and all its entities?"
					]
				}),
				/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
					className: "flex justify-end join",
					children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("button", {
						onClick: () => dialogRef.current?.close(),
						className: "btn btn-neutral join-item",
						children: "Cancel"
					}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)("button", {
						onClick: () => {
							dialogRef.current?.close();
							deleteEntitiesPage(pageData.id);
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
	}), /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("li", {
		ref,
		id: "nspanel_entities_page_{{ page.id }}",
		"hx-swap-oob": "true",
		className: `bg-base-200 float-left m-2 rounded-box p-3 grid grid-cols-2 ${gridRows} gap-4 w-full max-w-[24rem] aspect-square indicator nspanel-entities-page`,
		children: [
			(() => {
				const items = [];
				for (let i = 0; i < pageData.number_of_entities; i++) {
					let item = null;
					if (pageData.type == "entity") {
						for (let j = 0; j < entities.length; j++) if (entities[j].room_view_position === i && entities[j].type === "entity" && entities[j].entities_page_id === pageData.id) {
							item = entities[j];
							break;
						}
					} else if (pageData.type == "scene") {
						for (let j = 0; j < scenes.length; j++) if (scenes[j].room_view_position === i && scenes[j].type === "scene" && scenes[j].entities_page_id === pageData.id) {
							item = scenes[j];
							break;
						}
					} else console.error("Unknown entities page type: ", pageData.type);
					if (item != null) {
						if (item.type === "scene") items.push(/* @__PURE__ */ (0, import_jsx_runtime.jsx)(EntitiesDropTarget, {
							entities_page_id: pageData.id,
							room_view_position: i,
							type: pageData.type,
							draging_item,
							children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)(GenericSceneBox, {
								scene: item,
								draging_item
							}, item.id)
						}, i));
						else items.push(/* @__PURE__ */ (0, import_jsx_runtime.jsx)(EntitiesDropTarget, {
							entities_page_id: pageData.id,
							room_view_position: i,
							type: pageData.type,
							draging_item,
							children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)(GenericEntityBox, {
								entity: item,
								draging_item
							}, item.id)
						}, i));
					} else items.push(/* @__PURE__ */ (0, import_jsx_runtime.jsx)(EntitiesDropTarget, {
						entities_page_id: pageData.id,
						room_view_position: i,
						type: pageData.type,
						draging_item
					}, i));
				}
				return items;
			})(),
			/* @__PURE__ */ (0, import_jsx_runtime.jsx)("button", {
				disabled: !pageData.can_remove,
				className: "indicator-item indicator-bottom indicator-end badge not-disabled:badge-neutral not-disabled:hover:badge-error w-6 h-6 flex items-center justify-center not-disabled:cursor-pointer",
				title: `${pageData.can_remove ? "Delete page" : "Cannot delete last page"}`,
				onClick: () => dialogRef.current?.showModal(),
				children: "✕"
			}),
			/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
				ref: entitiesPageNumberOfEntitiesDiv,
				className: `dropdown dropdown-top dropdown-center ${editPageTypeOpen ? "dropdown-open" : ""} me-8 flex indicator-item indicator-bottom indicator-end`,
				children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
					onClick: () => setEditPageTypeOpen(!editPageTypeOpen),
					className: "badge badge-info w-6 h-6 flex items-center justify-center cursor-pointer",
					children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", { className: "mdi mdi-pencil" })
				}), /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
					className: "dropdown-content bg-base-100 join rounded-box z-1 p-1 shadow-sm",
					children: [
						/* @__PURE__ */ (0, import_jsx_runtime.jsx)("button", {
							onClick: () => setPageNumberOfEntities(4),
							className: `btn join-item btn-lg btn-square ${pageData.number_of_entities === 4 ? "btn-active btn-accent" : ""}`,
							children: "4"
						}),
						/* @__PURE__ */ (0, import_jsx_runtime.jsx)("button", {
							onClick: () => setPageNumberOfEntities(8),
							className: `btn join-item btn-lg btn-square ${pageData.number_of_entities === 8 ? "btn-active btn-accent" : ""}`,
							children: "8"
						}),
						/* @__PURE__ */ (0, import_jsx_runtime.jsx)("button", {
							onClick: () => setPageNumberOfEntities(12),
							className: `btn join-item btn-lg btn-square ${pageData.number_of_entities === 12 ? "btn-active btn-accent" : ""}`,
							children: "12"
						})
					]
				})]
			}),
			pageData.can_remove && /* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", {
				className: "indicator-item indicator-bottom indicator-center badge badge-neutral hover:badge-info w-6 h-6 flex items-center justify-center nspanel-entities-page-move-handle cursor-grab",
				title: "Drag & drop to move this page",
				ref: setHandle,
				children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", { className: "mdi mdi-drag" })
			})
		]
	})] });
};
//#endregion
//#region src/stores/RoomsStore.ts
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
var useRoomsStore = create((set) => ({
	status: "none",
	rooms: null,
	fetchData: async () => {
		set({ status: "loading" });
		fetch(`/rest/rooms`, {
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
				rooms: data.rooms
			}));
		});
	}
}));
//#endregion
//#region src/components/EntitiesPage/EntitiesPagesView.tsx
var EntitiesPagesView = ({ room_id, type }) => {
	const { entities_pages, fetchData, removeEntitiesPage, entities, scenes, setScenePosition, setEntityPosition } = useEntitiesPagesStore();
	const [dragingItem, setDragingItem] = (0, import_react.useState)(void 0);
	const [addNewPageTypeOpen, setAddNewPageTypeOpen] = (0, import_react.useState)(false);
	(0, import_react.useEffect)(() => {
		fetchData(room_id);
	}, [room_id, fetchData]);
	(0, import_react.useEffect)(() => {
		if (useRoomsStore.getState().status == "none") useRoomsStore.getState().fetchData();
	}, []);
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
	async function deleteEntitiesPage(id) {
		console.log("Deleting entities page", id);
		fetch(`/rest/entities_pages/${id}`, {
			credentials: "same-origin",
			method: "DELETE",
			mode: "same-origin",
			headers: {
				"Content-Type": "application/json",
				"X-CSRFToken": getCookie("csrftoken")
			}
		}).then(async (response) => {
			if (response.ok) {
				console.log("Successfully deleted entities page", id);
				removeEntitiesPage(id);
			}
		}).catch((error) => {
			console.error("Error deleting entities page", error);
		});
	}
	async function createEntitiesPage(number_of_entities) {
		console.log("Creating entities page", type);
		let url = `/rest/global/entities_pages`;
		if (room_id !== void 0) url = `/rest/rooms/${room_id}/entities_pages`;
		fetch(url, {
			credentials: "same-origin",
			method: "PUT",
			mode: "same-origin",
			headers: {
				"Content-Type": "application/json",
				"X-CSRFToken": getCookie("csrftoken")
			},
			body: JSON.stringify({
				type: number_of_entities,
				is_scenes_page: type == "scene"
			})
		}).then(async (response) => {
			if (response.ok) {
				console.log("Successfully created entities page", type, "number of entities", number_of_entities);
				fetchData(room_id);
			}
		}).catch((error) => {
			console.error("Error creating entities page", error);
		});
	}
	async function saveEntitiesOrder(entities, scenes) {
		fetch(`/rest/rooms/${room_id}/entities/order`, {
			credentials: "same-origin",
			method: "PUT",
			mode: "same-origin",
			headers: {
				"Content-Type": "application/json",
				"X-CSRFToken": getCookie("csrftoken")
			},
			body: JSON.stringify({
				entities,
				scenes
			})
		}).then(async (response) => {
			const data = await response.json();
			if (!response.ok) {
				const error = data && data.message || response.status;
				return Promise.reject(error);
			}
		}).catch((error) => {
			console.error("There was an error!", error);
		});
	}
	async function saveEntitiesPagesOrder(entities_pages) {
		fetch(`/rest/entities_pages/order`, {
			credentials: "same-origin",
			method: "PUT",
			mode: "same-origin",
			headers: {
				"Content-Type": "application/json",
				"X-CSRFToken": getCookie("csrftoken")
			},
			body: JSON.stringify({ order: entities_pages.map((page) => [page.id, page.display_order]) })
		}).then(async (response) => {
			const data = await response.json();
			if (!response.ok) {
				const error = data && data.message || response.status;
				return Promise.reject(error);
			}
		}).catch((error) => {
			console.error("There was an error!", error);
		});
	}
	return /* @__PURE__ */ (0, import_jsx_runtime.jsxs)(import_jsx_runtime.Fragment, { children: [/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
		className: "flex items-center justify-start mb-4",
		children: [/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("span", {
			className: "text-xl",
			children: [
				useRoomsStore.getState().rooms?.find((r) => r.id == room_id)?.friendly_name,
				" ",
				(() => {
					if (type == "entity") return "entities";
					if (type == "scene") return "scenes";
					return "UNKNOWN ENTITY PAGE TYPE";
				})()
			]
		}), /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
			className: `dropdown dropdown-top dropdown-center ${addNewPageTypeOpen ? "dropdown-open" : ""} me-8 flex indicator-item indicator-bottom indicator-end`,
			children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
				onClick: () => setAddNewPageTypeOpen(!addNewPageTypeOpen),
				className: "btn btn-xs btn-circle btn-success ml-2",
				children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", { className: "mdi mdi-plus" })
			}), /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
				className: "dropdown-content bg-base-100 join rounded-box z-1 p-1 shadow-sm",
				children: [
					/* @__PURE__ */ (0, import_jsx_runtime.jsx)("button", {
						onClick: () => {
							setAddNewPageTypeOpen(false);
							createEntitiesPage(4);
						},
						className: `btn join-item btn-lg btn-square`,
						children: "4"
					}),
					/* @__PURE__ */ (0, import_jsx_runtime.jsx)("button", {
						onClick: () => {
							setAddNewPageTypeOpen(false);
							createEntitiesPage(8);
						},
						className: `btn join-item btn-lg btn-square`,
						children: "8"
					}),
					/* @__PURE__ */ (0, import_jsx_runtime.jsx)("button", {
						onClick: () => {
							setAddNewPageTypeOpen(false);
							createEntitiesPage(12);
						},
						className: `btn join-item btn-lg btn-square`,
						children: "12"
					})
				]
			})]
		})]
	}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)(DragDropProvider, {
		onDragStart: (event) => {
			const { source } = event.operation;
			if (source == null) return;
			setDragingItem(source.data);
		},
		onDragEnd: (event) => {
			setDragingItem(void 0);
			if (event.canceled) return;
			const { source, target } = event.operation;
			if (source == null || target == null) return;
			const sourceData = source.data;
			const targetData = target.data;
			if (sourceData.config && targetData.config && sourceData.config.entities_page_id == targetData.config.entities_page_id && sourceData.config.room_view_position == targetData.config.room_view_position) return;
			if (sourceData.type == "entity" && sourceData.config) {
				const entity = entities.find((e) => sourceData.config && e.id == sourceData.config.id);
				if (!entity) return;
				let existingEntity = entities.find((e) => targetData.config && e.entities_page_id == targetData.config.entities_page_id && e.room_view_position == targetData.config.room_view_position);
				if (existingEntity != null) {
					existingEntity = JSON.parse(JSON.stringify(existingEntity));
					if (existingEntity) setEntityPosition(existingEntity.id, entity.entities_page_id, entity.room_view_position);
				}
				const updatedEntity = JSON.parse(JSON.stringify(entity));
				if (targetData.config) {
					setEntityPosition(updatedEntity.id, targetData.config.entities_page_id, targetData.config.room_view_position);
					saveEntitiesOrder(useEntitiesPagesStore.getState().entities, []);
				} else console.error("Failed to set new entity position. Target data has no config");
			} else if (sourceData.type == "scene" && sourceData.config) {
				const scene = scenes.find((e) => sourceData.config && e.id == sourceData.config.id);
				if (!scene) return;
				let existingScene = scenes.find((e) => targetData.config && e.entities_page_id == targetData.config.entities_page_id && e.room_view_position == targetData.config.room_view_position);
				if (existingScene != null) {
					existingScene = JSON.parse(JSON.stringify(existingScene));
					if (existingScene) setScenePosition(existingScene.id, scene.entities_page_id, scene.room_view_position);
				}
				const updatedEntity = JSON.parse(JSON.stringify(scene));
				if (targetData.config) {
					setScenePosition(updatedEntity.id, targetData.config.entities_page_id, targetData.config.room_view_position);
					saveEntitiesOrder([], useEntitiesPagesStore.getState().scenes);
				} else console.error("Failed to set new scene position as target as not config");
			} else if (sourceData.type == "entity_page" && sourceData.entity_page_config) {
				if (isSortable(source)) {
					const { initialIndex, index } = source;
					console.log("Initial index: ", initialIndex, " index: ", index);
					if (initialIndex == index) return;
					if (!useEntitiesPagesStore.getState().entities_pages.find((page) => page.display_order == initialIndex)) {
						console.error("Failed to find source page data. Cannot continue.");
						return;
					}
					const newEntitiesPagesOrder = [...useEntitiesPagesStore.getState().entities_pages.filter((page) => page.type == type)];
					const [removed] = newEntitiesPagesOrder.splice(initialIndex, 1);
					newEntitiesPagesOrder.splice(index, 0, removed);
					newEntitiesPagesOrder.forEach((page, i) => {
						page.display_order = i;
					});
					useEntitiesPagesStore.setState({ entities_pages: [...newEntitiesPagesOrder, ...useEntitiesPagesStore.getState().entities_pages.filter((page) => page.type != type)] });
					saveEntitiesPagesOrder(newEntitiesPagesOrder);
				}
			}
		},
		children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)("ul", {
			className: "list-none w-full nspanel-entities-pages",
			children: entities_pages.filter((entity_page) => entity_page.type === type).map((entity_page) => /* @__PURE__ */ (0, import_jsx_runtime.jsx)(EntitiesPage, {
				id: entity_page.id,
				draging_item: dragingItem,
				deleteEntitiesPage
			}, `entity_page-${entity_page.id}`))
		})
	})] });
};
//#endregion
export { EntitiesPagesView as default };
