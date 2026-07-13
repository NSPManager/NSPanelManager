import { t as create } from "./react-B85aptD-.js";
//#region src/stores/SettingsStore.ts
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
var useSettingsStore = create((set) => ({
	status: "none",
	settings: null,
	fetchData: async () => {
		set({ status: "loading" });
		fetch(`/rest/settings`, {
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
				settings: data.settings
			}));
			set((state) => ({
				...state,
				status: "loaded"
			}));
		});
	}
}));
//#endregion
export { useSettingsStore as t };
