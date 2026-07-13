import { t as create } from "./react-CC_gJ0mf.js";
import { t as useStompStore } from "./StompStore-p3Yel4rQ.js";
//#region src/stores/EntityStore.ts
var nspanel_stomp_status_update_callback = (message) => {
	const json_data = JSON.parse(message.body);
	useEntityStatesStore.getState()._update_nspanel_status(json_data.mac, json_data);
};
var useEntityStatesStore = create((set) => ({
	status: "none",
	_nspanels: [],
	nspanels: {},
	rooms: {},
	fetchData: async () => {
		set({ status: "loading" });
		if (useStompStore.getState().status == "none") useStompStore.getState().connect();
		useStompStore.getState().subscribe("entity_states", (message) => {
			set({
				_nspanels: JSON.parse(message.body).nspanels,
				status: "loaded"
			});
			useEntityStatesStore.getState()._attachNSPanelStatusDataSubscriptions();
		});
		fetch("/rest/rooms").then((response) => response.json()).then((data) => {
			for (const room of data.rooms) set({ rooms: {
				...useEntityStatesStore.getState().rooms,
				[room.id]: room
			} });
		});
	},
	_update_nspanel_status: (mac, status) => {
		set({ nspanels: {
			...useEntityStatesStore.getState().nspanels,
			[mac]: status
		} });
	},
	_attachNSPanelStatusDataSubscriptions: async () => {
		for (const panel of Object.entries(useEntityStatesStore.getState().nspanels)) {
			let nspanel_removed = true;
			for (const existingPanel of useEntityStatesStore.getState()._nspanels) if (existingPanel.mac === panel[0]) {
				nspanel_removed = false;
				break;
			}
			if (nspanel_removed) {
				console.log("Removing ", panel[0]);
				const current_nspanels = useEntityStatesStore.getState().nspanels;
				delete current_nspanels[panel[0]];
				useStompStore.getState().unsubscribe(`nspanel/${panel[0]}/status`, nspanel_stomp_status_update_callback);
				set({ nspanels: { ...current_nspanels } });
			}
		}
		for (const panel of useEntityStatesStore.getState()._nspanels) useStompStore.getState().subscribe(`nspanel/${panel.mac}/status`, nspanel_stomp_status_update_callback);
	}
}));
//#endregion
export { useEntityStatesStore as t };
