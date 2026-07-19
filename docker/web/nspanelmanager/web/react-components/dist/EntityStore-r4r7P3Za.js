import { t as create } from "./react-fWHvS6VU.js";
import { n as Notify } from "./NSPanelToastContainer-B5jbnFTb.js";
import { t as useStompStore } from "./StompStore-Bxp3E-AV.js";
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
		console.debug("Subscribing to entity_states...");
		useStompStore.getState().subscribe("entity_states", (message) => {
			console.debug("Got entity states update over STOMP.");
			set({
				_nspanels: JSON.parse(message.body).nspanels,
				status: "loaded"
			});
			useEntityStatesStore.getState()._attachNSPanelStatusDataSubscriptions();
		});
		console.debug("Loading rooms...");
		fetch("/rest/rooms").then((response) => response.json()).then((data) => {
			console.debug("Got rooms data.");
			for (const room of data.rooms) set({ rooms: {
				...useEntityStatesStore.getState().rooms,
				[room.id]: room
			} });
		}).catch((error) => {
			Notify({
				message: "Failed to fetch rooms. Error: " + error,
				level: "error"
			});
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
