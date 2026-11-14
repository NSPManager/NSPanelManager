import { c as __toESM, r as require_react, t as require_jsx_runtime } from "./main-2GKhVwa3.js";
import { s as useForm } from "./index.esm-CyxbWRtN.js";
import { n as Notify } from "./NSPanelToastContainer-CdQdRrsL.js";
import { t as motion } from "./proxy-BJsAbzcZ.js";
import { t as AnimatePresence } from "./AnimatePresence-CuLSycWM.js";
import { i as mdiInformationOutline, x as require_Icon } from "./mdi-CeE3u6rn.js";
import { i as api, t as Alert } from "./layout-SvH9qeCO.js";
//#region src/components/InitialSetup/Step1_Welcome.tsx
var import_react = /* @__PURE__ */ __toESM(require_react(), 1);
var import_jsx_runtime = require_jsx_runtime();
var Step1_Welcome = ({ nextStep }) => {
	return /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", { children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
		className: "flex mt-4 justify-end items-center",
		children: /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", { children: [
			/* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
				className: "flex items-center justify-center p-8",
				children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)("img", {
					className: "h-28 bg-neutral/50 rounded-box p-1",
					src: "/static/logo.svg"
				})
			}),
			/* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
				className: "flex items-center justify-center",
				children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", {
					className: "text-2xl",
					children: "Welcome!"
				})
			}),
			/* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", {
				className: "flex items-center justify-center text-center",
				children: "NSPanel Manager is a software for easy management of multiple Sonoff NSPanels and intuitive use for all family members in the household. But you probably already knew that."
			}),
			/* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", {
				className: "flex items-center justify-center text-center mt-4 px-6",
				children: "Thank you for choosing NSPanel Manager! This initial setup guide will take you through the necessary steps to get a working setup, press the button below to continue."
			})
		] })
	}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
		className: "flex justify-end mt-8",
		children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)("button", {
			type: "submit",
			className: "btn btn-primary",
			onClick: () => nextStep(),
			children: "Get started"
		})
	})] });
};
//#endregion
//#region src/components/InitialSetup/form.ts
var INITIAL_SETUP_FIELDS = [
	"manager_address",
	"manager_port",
	"mqtt_server",
	"mqtt_port",
	"mqtt_username",
	"home_assistant_address",
	"openhab_address",
	"theme"
];
var SECRETS = [
	"mqtt_password",
	"home_assistant_token",
	"openhab_token"
];
var ADDON_MANAGED = ["home_assistant_address", "home_assistant_token"];
function toFormValues(settings) {
	const values = {};
	for (const name of INITIAL_SETUP_FIELDS) values[name] = settings[name];
	for (const secret of SECRETS) {
		values[secret] = "";
		values[`clear_${secret}`] = false;
	}
	return values;
}
/**
* The PATCH body for a submitted form: only the settings that changed. Secrets are sent when a new
* value is typed, or as "" when "clear" is ticked. Add-on managed settings are never sent in the add-on.
*/
function buildPatch(values, dirty, isHomeAssistantAddon) {
	const body = {};
	const allowed = (name) => !(isHomeAssistantAddon && ADDON_MANAGED.includes(name));
	for (const name of INITIAL_SETUP_FIELDS) if (dirty[name] && allowed(name)) body[name] = values[name];
	for (const secret of SECRETS) {
		if (!allowed(secret)) continue;
		if (values[`clear_${secret}`]) body[secret] = "";
		else if (values[secret]) body[secret] = values[secret];
	}
	return body;
}
//#endregion
//#region src/components/InitialSetup/Step2_Theme.tsx
var Step2_Theme = ({ setValue, nextStep, prevStep, watch }) => {
	const themes = {
		default: "Default",
		light: "Light",
		dark: "Dark",
		nspmwip: "NSPanel Manager",
		cupcake: "Cupcake",
		bumblebee: "Bumblebee",
		emerald: "Emerald",
		corporate: "Corporate",
		synthwave: "Synthwave",
		retro: "Retro",
		cyberpunk: "Cyberpunk",
		valentine: "Valentine",
		halloween: "Halloween",
		garden: "Garden",
		forest: "Forest",
		aqua: "Aqua",
		lofi: "Lo-Fi",
		pastel: "Pastel",
		fantasy: "Fantasy",
		wireframe: "Wireframe",
		black: "Black",
		luxury: "Luxury",
		dracula: "Dracula",
		cmyk: "CMYK",
		autumn: "Autumn",
		business: "Business",
		acid: "Acid",
		lemonade: "Lemonade",
		night: "Night",
		coffee: "Coffee",
		winter: "Winter",
		dim: "Dim",
		nord: "Nord",
		sunset: "Sunset"
	};
	const selectedTheme = watch("theme");
	return /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", { children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
		className: "grid gap-4 grid-cols-3 mt-4 justify-end items-center",
		children: Object.entries(themes).map(([theme_option, display_name]) => /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
			className: "bg-neutral text-neutral-content grid grid-cols-3 w-full h-20 rounded-box overflow-hidden cursor-pointer",
			"data-theme": theme_option,
			onClick: () => {
				setValue("theme", theme_option);
			},
			children: [/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
				className: "",
				children: [
					/* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", { className: "bg-base-100 min-h-1/3" }),
					/* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", { className: "bg-base-200 min-h-1/3" }),
					/* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", { className: "bg-base-300 min-h-1/3" })
				]
			}), /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
				className: "col-span-2 relative",
				children: [/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("label", {
					className: "label",
					children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("input", {
						type: "radio",
						name: "theme-dropdown",
						className: "theme-controller radio radio-xs radio-accent p-0 m-0 absolute top-1 right-1",
						value: theme_option,
						checked: theme_option == selectedTheme
					}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", {
						className: "label-text justify-start w-full ml-2 text-wrap",
						children: display_name
					})]
				}), /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
					className: "flex absolute bottom-2",
					children: [
						/* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
							className: "badge badge-primary badge-xs ms-2",
							children: "A"
						}),
						/* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
							className: "badge badge-accent badge-xs ms-1",
							children: "B"
						}),
						/* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
							className: "badge badge-success badge-xs ms-1",
							children: "C"
						}),
						/* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
							className: "badge badge-info badge-xs ms-1",
							children: "D"
						}),
						/* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
							className: "badge badge-error badge-xs ms-1",
							children: "E"
						})
					]
				})]
			})]
		}))
	}), /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
		className: "flex gap-2 justify-end mt-8",
		children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("button", {
			type: "button",
			className: "btn btn-neutral",
			onClick: prevStep,
			children: "Back"
		}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)("button", {
			type: "button",
			className: "btn btn-primary",
			onClick: nextStep,
			children: "Next"
		})]
	})] });
};
//#endregion
//#region src/components/InitialSetup/Step3_ManagerSettings.tsx
var Step3_ManagerSettings = ({ setValue, getValues, register, nextStep, prevStep }) => {
	const getIPFromURL = () => {
		fetch("/rest/get_ip_by_hostname", {
			method: "POST",
			headers: { "Content-Type": "application/json" },
			body: JSON.stringify({ hostname: window.location.hostname })
		}).then((response) => response.json()).then((data) => {
			if (data.ip) setValue("manager_address", data.ip);
			else Notify({
				message: "Failed to get IP from URL. No \"ip\" field in response.",
				level: "error"
			});
		}).catch(() => {
			Notify({
				message: "Failed to get IP from URL.",
				level: "error"
			});
		});
	};
	const testManagerAddressAndPort = () => {
		const params = {
			address: getValues("manager_address"),
			port: getValues("manager_port").toString()
		};
		const queryString = new URLSearchParams(params).toString();
		fetch("/rest/mqttmanager/test?" + queryString, {
			method: "GET",
			signal: AbortSignal.timeout(2e3)
		}).then((response) => {
			if (response.ok) Notify({
				message: "Manager address and port are valid.",
				level: "success",
				duration: 5e3
			});
			else response.json().then((data) => {
				Notify({
					message: "Failed to test Manager address and port. Error: " + data.message,
					level: "error",
					duration: 5e3
				});
			});
		}).catch((error) => {
			Notify({
				message: "Test failed. Error: " + error.message,
				level: "error",
				duration: 5e3
			});
		});
	};
	return /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", { children: [/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
		className: "w-full",
		children: [
			/* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
				className: "flex justify-center mb-4 duration-500 transition-transform ease-linear",
				children: /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
					className: "w-full",
					children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("label", {
						htmlFor: "add_new_button_name",
						className: "block mb-2 text-sm font-medium",
						children: "Manager IP"
					}), /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
						className: "flex",
						children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("input", {
							className: "outline-none bg-base-300 border-neutral rounded-md rounded-r-none border focus:ring-0 focus:border-accent block flex-1 min-w-0 w-full text-sm p-2.5 peer/search_text",
							type: "text",
							...register("manager_address"),
							required: true
						}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)("button", {
							type: "button",
							className: "btn btn-info rounded-l-none h-auto",
							onClick: () => getIPFromURL(),
							children: "Load from URL"
						})]
					})]
				})
			}),
			/* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
				className: "flex justify-center mb-4 duration-500 transition-transform ease-linear",
				children: /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
					className: "w-full",
					children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("label", {
						htmlFor: "manager_port",
						className: "block mb-2 text-sm font-medium",
						children: "Manager Port"
					}), /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
						className: "flex",
						children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("input", {
							className: "outline-none bg-base-300 border-neutral rounded-md rounded-r-none border focus:ring-0 focus:border-accent block flex-1 min-w-0 w-full text-sm p-2.5 peer/search_text",
							type: "number",
							...register("manager_port", { valueAsNumber: true }),
							required: true
						}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)("button", {
							type: "button",
							className: "btn btn-info rounded-l-none h-auto",
							onClick: () => {
								setValue("manager_port", parseInt(window.location.port));
							},
							children: "Load from URL"
						})]
					})]
				})
			}),
			/* @__PURE__ */ (0, import_jsx_runtime.jsx)("button", {
				className: "btn btn-accent w-full",
				onClick: testManagerAddressAndPort,
				children: "Test settings"
			})
		]
	}), /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
		className: "flex gap-2 justify-end mt-8",
		children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("button", {
			type: "button",
			className: "btn btn-neutral",
			onClick: prevStep,
			children: "Back"
		}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)("button", {
			type: "button",
			className: "btn btn-primary",
			onClick: nextStep,
			children: "Next"
		})]
	})] });
};
//#endregion
//#region src/components/InitialSetup/Step4_MqttSettings.tsx
var import_Icon = require_Icon();
var Step4_MqttSettings = ({ setValue, getValues, register, nextStep, prevStep }) => {
	const [noLogin, setNoLogin] = (0, import_react.useState)(getValues("mqtt_username") == "" && getValues("mqtt_password") == "");
	return /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", { children: [
		/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
			className: "alert alert-info my-4",
			children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)(import_Icon.Icon, {
				path: mdiInformationOutline,
				className: "w-6"
			}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", { children: "If you are running the MQTT broker (for example Mosquitto) as an add on in Home Assistant. Then enter the address for Home Assistant and the port for MQTT (1883 if you have not changed it)." })]
		}),
		/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
			className: "w-full",
			children: [
				/* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
					className: "flex justify-center mb-4 duration-500 transition-transform ease-linear",
					children: /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
						className: "w-full",
						children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("label", {
							htmlFor: "add_new_button_name",
							className: "block mb-2 text-sm font-medium",
							children: "MQTT Broker Address"
						}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
							className: "flex",
							children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)("input", {
								className: "outline-none bg-base-300 border-neutral rounded-md border focus:ring-0 focus:border-accent block flex-1 min-w-0 w-full text-sm p-2.5 peer/search_text",
								type: "text",
								...register("mqtt_server"),
								required: true
							})
						})]
					})
				}),
				/* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
					className: "flex justify-center mb-4 duration-500 transition-transform ease-linear",
					children: /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
						className: "w-full",
						children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("label", {
							htmlFor: "manager_port",
							className: "block mb-2 text-sm font-medium",
							children: "MQTT Broker Port"
						}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
							className: "flex",
							children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)("input", {
								className: "outline-none bg-base-300 border-neutral rounded-md border focus:ring-0 focus:border-accent block flex-1 min-w-0 w-full text-sm p-2.5 peer/search_text",
								type: "number",
								...register("mqtt_port", { valueAsNumber: true }),
								required: true
							})
						})]
					})
				}),
				/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("fieldset", {
					className: "fieldset bg-base-100 border-base-300 rounded-box w-full border p-4",
					children: [
						/* @__PURE__ */ (0, import_jsx_runtime.jsx)("legend", {
							className: "fieldset-legend",
							children: "Login options"
						}),
						/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("label", {
							className: "label",
							children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("input", {
								type: "checkbox",
								className: "checkbox checkbox-sm checkbox-primary",
								checked: noLogin,
								onChange: (e) => {
									if (e.currentTarget.checked) {
										setValue("mqtt_username", "");
										setValue("mqtt_password", "");
										setValue("clear_mqtt_password", true);
										setNoLogin(true);
									} else setNoLogin(false);
								}
							}), "Anonymous login"]
						}),
						/* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
							className: "flex justify-center my-4 duration-500 transition-transform ease-linear",
							hidden: noLogin,
							children: /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
								className: "w-full",
								children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("label", {
									htmlFor: "mqtt_username",
									className: "block mb-2 text-sm font-medium",
									children: "MQTT Broker Username"
								}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
									className: "flex",
									children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)("input", {
										className: "outline-none bg-base-300 border-neutral rounded-md border focus:ring-0 focus:border-accent block flex-1 min-w-0 w-full text-sm p-2.5 peer/search_text",
										type: "text",
										id: "mqtt_username",
										...register("mqtt_username"),
										required: true
									})
								})]
							})
						}),
						/* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
							className: "flex justify-center mb-4 duration-500 transition-transform ease-linear",
							hidden: noLogin == true,
							children: /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
								className: "w-full",
								children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("label", {
									htmlFor: "mqtt_password",
									className: "block mb-2 text-sm font-medium",
									children: "MQTT Broker Password"
								}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
									className: "flex",
									children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)("input", {
										className: "outline-none bg-base-300 border-neutral rounded-md border focus:ring-0 focus:border-accent block flex-1 min-w-0 w-full text-sm p-2.5 peer/search_text",
										type: "password",
										id: "mqtt_password",
										...register("mqtt_password"),
										required: true
									})
								})]
							})
						})
					]
				})
			]
		}),
		/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
			className: "flex gap-2 justify-end mt-8",
			children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("button", {
				type: "button",
				className: "btn btn-neutral",
				onClick: prevStep,
				children: "Back"
			}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)("button", {
				type: "button",
				className: "btn btn-primary",
				onClick: nextStep,
				disabled: getValues("mqtt_server") == "" || getValues("mqtt_port") < 1 || getValues("mqtt_port") > 65535,
				children: "Next"
			})]
		})
	] });
};
//#endregion
//#region src/components/InitialSetup/Step5_HomeAssistantSettings.tsx
var Step5_HomeAssistantSettings = ({ getValues, register, isHomeAssistantAddon, nextStep, prevStep }) => {
	const testHomeAssistant = () => {
		const params = {
			address: getValues("home_assistant_address"),
			token: getValues("home_assistant_token")
		};
		const queryString = new URLSearchParams(params).toString();
		fetch("/rest/home_assistant/test?" + queryString, { method: "GET" }).then((response) => {
			if (response.ok) Notify({
				message: "Home Assistant address and token are valid.",
				level: "success",
				duration: 5e3
			});
			else response.json().then((data) => {
				Notify({
					message: "Failed to test Home Assistant address and token. Error: " + data.message,
					level: "error",
					duration: 5e3
				});
			});
		}).catch((error) => {
			Notify({
				message: "Test failed. Error: " + error.message,
				level: "error",
				duration: 5e3
			});
		});
	};
	return /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", { children: [
		/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
			className: "alert alert-info my-4",
			hidden: isHomeAssistantAddon,
			children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)(import_Icon.Icon, {
				path: mdiInformationOutline,
				className: "w-6"
			}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", { children: "Home Assistant is optional. You may also run NSPanel Manager using OpenHAB." })]
		}),
		/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
			className: "alert alert-info my-4",
			hidden: !isHomeAssistantAddon,
			children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)(import_Icon.Icon, {
				path: mdiInformationOutline,
				className: "w-6"
			}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", { children: "You are running NSPanel Manager as a Home Assistant add on. These settings are managed for you by Home Assistant." })]
		}),
		/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
			className: "alert alert-info my-4",
			hidden: isHomeAssistantAddon,
			children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)(import_Icon.Icon, {
				path: mdiInformationOutline,
				className: "w-6"
			}), /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", { children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", { children: "Enter Home Assistant address in the form of " }), /* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", {
				className: "italic",
				children: "http(s)://address:port/"
			})] })]
		}),
		/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
			className: "w-full",
			hidden: isHomeAssistantAddon,
			children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
				className: "flex justify-center mb-4 duration-500 transition-transform ease-linear",
				children: /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
					className: "w-full",
					children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("label", {
						htmlFor: "add_new_button_name",
						className: "block mb-2 text-sm font-medium",
						children: "Home Assistant address"
					}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
						className: "flex",
						children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)("input", {
							className: "outline-none bg-base-300 border-neutral rounded-md border focus:ring-0 focus:border-accent block flex-1 min-w-0 w-full text-sm p-2.5",
							type: "text",
							...register("home_assistant_address"),
							disabled: isHomeAssistantAddon,
							required: true
						})
					})]
				})
			}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
				className: "flex justify-center mb-4 duration-500 transition-transform ease-linear",
				hidden: isHomeAssistantAddon,
				children: /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
					className: "w-full",
					children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("label", {
						htmlFor: "manager_port",
						className: "block mb-2 text-sm font-medium",
						children: "Home Assistant Token"
					}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
						className: "flex",
						children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)("input", {
							className: "outline-none bg-base-300 border-neutral rounded-md border focus:ring-0 focus:border-accent block flex-1 min-w-0 w-full text-sm p-2.5",
							type: "password",
							...register("home_assistant_token"),
							disabled: isHomeAssistantAddon,
							required: true
						})
					})]
				})
			})]
		}),
		/* @__PURE__ */ (0, import_jsx_runtime.jsx)("button", {
			className: "btn btn-accent w-full",
			onClick: testHomeAssistant,
			hidden: isHomeAssistantAddon,
			children: "Test settings"
		}),
		/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
			className: "flex gap-2 justify-end mt-8",
			children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("button", {
				type: "submit",
				className: "btn btn-neutral",
				onClick: prevStep,
				children: "Back"
			}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)("button", {
				type: "submit",
				className: "btn btn-primary",
				onClick: nextStep,
				children: "Next"
			})]
		})
	] });
};
//#endregion
//#region src/components/InitialSetup/Step6_OpenhabSettings.tsx
var Step6_OpenhabSettings = ({ getValues, register, nextStep, prevStep }) => {
	const testOpenhab = () => {
		const params = {
			address: getValues("openhab_address"),
			token: getValues("openhab_token")
		};
		const queryString = new URLSearchParams(params).toString();
		fetch("/rest/openhab/test?" + queryString, { method: "GET" }).then((response) => {
			if (response.ok) Notify({
				message: "OpenHAB address and token are valid.",
				level: "success",
				duration: 5e3
			});
			else response.json().then((data) => {
				Notify({
					message: "Failed to test OpenHAB address and token. Error: " + data.message,
					level: "error",
					duration: 5e3
				});
			});
		}).catch((error) => {
			Notify({
				message: "Test failed. Error: " + error.message,
				level: "error",
				duration: 5e3
			});
		});
	};
	return /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", { children: [
		/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
			className: "alert alert-info my-4",
			children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)(import_Icon.Icon, {
				path: mdiInformationOutline,
				className: "w-6"
			}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", { children: "OpenHAB is optional. You may also run NSPanel Manager using Home Assistant." })]
		}),
		/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
			className: "alert alert-info my-4",
			children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)(import_Icon.Icon, {
				path: mdiInformationOutline,
				className: "w-6"
			}), /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", { children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", { children: "Enter OpenHAB address in the form of " }), /* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", {
				className: "italic",
				children: "http(s)://address:port/"
			})] })]
		}),
		/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
			className: "w-full",
			children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
				className: "flex justify-center mb-4 duration-500 transition-transform ease-linear",
				children: /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
					className: "w-full",
					children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("label", {
						htmlFor: "add_new_button_name",
						className: "block mb-2 text-sm font-medium",
						children: "OpenHAB address"
					}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
						className: "flex",
						children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)("input", {
							className: "outline-none bg-base-300 border-neutral rounded-md border focus:ring-0 focus:border-accent block flex-1 min-w-0 w-full text-sm p-2.5",
							type: "text",
							...register("openhab_address"),
							required: true
						})
					})]
				})
			}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
				className: "flex justify-center mb-4 duration-500 transition-transform ease-linear",
				children: /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
					className: "w-full",
					children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("label", {
						htmlFor: "manager_port",
						className: "block mb-2 text-sm font-medium",
						children: "OpenHAB Token"
					}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
						className: "flex",
						children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)("input", {
							className: "outline-none bg-base-300 border-neutral rounded-md border focus:ring-0 focus:border-accent block flex-1 min-w-0 w-full text-sm p-2.5",
							type: "password",
							...register("openhab_token"),
							required: true
						})
					})]
				})
			})]
		}),
		/* @__PURE__ */ (0, import_jsx_runtime.jsx)("button", {
			className: "btn btn-accent w-full",
			onClick: testOpenhab,
			children: "Test settings"
		}),
		/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
			className: "flex gap-2 justify-end mt-8",
			children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("button", {
				type: "submit",
				className: "btn btn-neutral",
				onClick: prevStep,
				children: "Back"
			}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)("button", {
				type: "submit",
				className: "btn btn-primary",
				onClick: nextStep,
				children: "Next"
			})]
		})
	] });
};
//#endregion
//#region src/components/InitialSetup/Step7_Finished.tsx
var Step7_Finished = ({ form, prevStep, isHomeAssistantAddon, dialogRef }) => {
	const saveSettings = async () => {
		const { getValues, reset, formState: { dirtyFields } } = form;
		const body = buildPatch(getValues(), dirtyFields, isHomeAssistantAddon);
		try {
			const { data, error, response } = await api.PATCH("/rest/v2/settings", { body });
			if (data) {
				reset(toFormValues(data));
				Notify({
					message: "Settings saved.",
					level: "success",
					duration: 5e3
				});
				dialogRef.current?.close();
				return;
			}
			if (response.status === 400 && error && !("detail" in error && error.detail)) {
				console.error("Got error ", error);
				const fieldErrors = Object.entries(error);
				if (fieldErrors.length === 0) Notify({
					message: "Nothing was saved.",
					level: "error",
					duration: 8e3
				});
				for (const [field, messages] of fieldErrors) for (const message of Array.isArray(messages) ? messages : [messages]) Notify({
					message: `${field}: ${message}`,
					level: "error",
					duration: 8e3
				});
			} else {
				const detail = error && "detail" in error ? error.detail : `HTTP ${response.status}`;
				Notify({
					message: `Couldn't save the settings: ${detail}`,
					level: "error"
				});
			}
		} catch (failure) {
			Notify({
				message: `Couldn't save the settings: ${failure instanceof Error ? failure.message : String(failure)}`,
				level: "error"
			});
		}
	};
	return /* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", { children: /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
		className: "w-full",
		children: [
			/* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
				className: "flex items-center justify-center space-x-3 rtl:space-x-reverse",
				children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
					className: "bg-neutral/50 p-2 rounded-box",
					children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)("img", {
						src: "/static/logo.svg",
						alt: "Logo",
						className: "h-12"
					})
				})
			}),
			/* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
				className: "mt-1 text-center",
				children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", {
					className: "text-center",
					children: "Version: %version%"
				})
			}),
			/* @__PURE__ */ (0, import_jsx_runtime.jsx)("p", {
				className: "mt-2 text-center",
				children: "Initial setup is now finished. If you have any NSPanels already flashed and connected to the same MQTT broker they should show up automatically. In case they do not, there is the documentation and also a discord where you can get help. There is also the Github page where you can report issues and"
			}),
			/* @__PURE__ */ (0, import_jsx_runtime.jsx)("p", {
				className: "my-2 text-center font-bold",
				children: "Thank you for your support and trust. Have fun and enjoy!"
			}),
			/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
				id: "about_links",
				className: "join join-vertical w-full",
				children: [
					/* @__PURE__ */ (0, import_jsx_runtime.jsx)("a", {
						className: "join-item btn btn-link",
						href: "https://nspanelmanager.com",
						target: "_blank",
						children: "Documentation"
					}),
					/* @__PURE__ */ (0, import_jsx_runtime.jsx)("a", {
						className: "join-item btn btn-link",
						href: "https://github.com/NSPManager/NSPanelManager",
						target: "_blank",
						children: "GitHub"
					}),
					/* @__PURE__ */ (0, import_jsx_runtime.jsx)("a", {
						className: "join-item btn btn-link",
						href: "https://discord.gg/RwXvAH56fE",
						target: "_blank",
						children: "Discord"
					}),
					/* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
						className: "join-item flex items-center justify-center",
						children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)("a", {
							href: "https://www.buymeacoffee.com/nspanelmanager",
							className: "mt-4",
							target: "_blank",
							rel: "nofollow",
							children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)("img", {
								src: "/static/bmc-button.png",
								alt: "Buy Us A Coffee",
								height: "41",
								width: "174"
							})
						})
					})
				]
			}),
			/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
				className: "flex gap-2 justify-end mt-8",
				children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("button", {
					type: "button",
					className: "btn btn-neutral",
					onClick: prevStep,
					children: "Back"
				}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)("button", {
					type: "button",
					className: "btn btn-primary",
					onClick: saveSettings,
					children: "Save"
				})]
			})
		]
	}) });
};
//#endregion
//#region src/components/InitialSetup/InitialSetup.tsx
var InitialSetup = ({ opened, isHomeAssistantAddon }) => {
	const [state, setState] = (0, import_react.useState)({ status: "loading" });
	const fetchSettings = (0, import_react.useCallback)(() => {
		api.GET("/rest/v2/settings").then(({ data, response }) => setState(data ? {
			status: "loaded",
			settings: data
		} : {
			status: "error",
			message: `HTTP ${response.status}`
		})).catch((error) => setState({
			status: "error",
			message: error instanceof Error ? error.message : String(error)
		}));
	}, []);
	(0, import_react.useEffect)(fetchSettings, [fetchSettings]);
	const retry = () => {
		setState({ status: "loading" });
		fetchSettings();
	};
	const dialogRef = (0, import_react.useRef)(null);
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
		ref: dialogRef,
		open: opened ?? false,
		className: "modal duration-200",
		children: [
			state.status === "loading" && /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
				className: "flex items-center justify-center modal-box max-h-11/12 min-w-180 min-h-60 bg-base-100 text-base-content",
				children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", { className: "loading loading-spinner loading" }), /* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", {
					className: "ml-2 text-xl",
					children: "Initializing first setup"
				})]
			}),
			state.status === "error" && /* @__PURE__ */ (0, import_jsx_runtime.jsxs)(import_jsx_runtime.Fragment, { children: [/* @__PURE__ */ (0, import_jsx_runtime.jsxs)(Alert, {
				level: "error",
				children: ["Couldn't load settings: ", state.message]
			}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)("button", {
				type: "button",
				className: "btn",
				onClick: retry,
				children: "Retry"
			})] }),
			state.status === "loaded" && /* @__PURE__ */ (0, import_jsx_runtime.jsx)(InitialSetupForm, {
				settings: state.settings,
				dialogRef,
				isHomeAssistantAddon
			}),
			/* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", { className: "modal-backdrop" })
		]
	});
};
var InitialSetupForm = ({ dialogRef, isHomeAssistantAddon, settings }) => {
	const [step, setStep] = (0, import_react.useState)(1);
	const form = useForm({ defaultValues: toFormValues(settings) });
	const { register, setValue, getValues, watch } = form;
	const nextStep = () => setStep((prev) => prev + 1);
	const prevStep = () => setStep((prev) => prev - 1);
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
	return /* @__PURE__ */ (0, import_jsx_runtime.jsxs)(import_jsx_runtime.Fragment, { children: [/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
		ref: contentRef,
		className: `modal-box max-h-11/12 min-w-180 bg-base-100 text-base-content`,
		children: [(() => {
			return /* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
				className: "flex justify-center mb-4 w-full",
				children: /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("ul", {
					className: "steps w-full",
					children: [
						/* @__PURE__ */ (0, import_jsx_runtime.jsx)("li", {
							className: `step cursor-pointer ${step >= 1 ? "step-primary cursor-pointer" : ""}`,
							onClick: () => {
								setStep(1);
							},
							children: "Welcome"
						}),
						/* @__PURE__ */ (0, import_jsx_runtime.jsx)("li", {
							className: `step cursor-pointer ${step >= 2 ? "step-primary" : ""}`,
							onClick: () => {
								setStep(2);
							},
							children: "Theme"
						}),
						/* @__PURE__ */ (0, import_jsx_runtime.jsx)("li", {
							className: `step cursor-pointer ${step >= 3 ? "step-primary" : ""}`,
							onClick: () => {
								setStep(3);
							},
							children: "Manager settings"
						}),
						/* @__PURE__ */ (0, import_jsx_runtime.jsx)("li", {
							className: `step cursor-pointer ${step >= 4 ? "step-primary" : ""}`,
							onClick: () => {
								setStep(4);
							},
							children: "MQTT settings"
						}),
						/* @__PURE__ */ (0, import_jsx_runtime.jsx)("li", {
							className: `step cursor-pointer ${step >= 5 ? "step-primary" : ""}`,
							onClick: () => {
								setStep(5);
							},
							children: "Home Assistant settings"
						}),
						/* @__PURE__ */ (0, import_jsx_runtime.jsx)("li", {
							className: `step cursor-pointer ${step >= 6 ? "step-primary" : ""}`,
							onClick: () => {
								setStep(6);
							},
							children: "OpenHAB settings"
						}),
						/* @__PURE__ */ (0, import_jsx_runtime.jsx)("li", {
							className: `step cursor-pointer ${step >= 7 ? "step-primary" : ""}`,
							onClick: () => {
								setStep(7);
							},
							children: "Finished"
						})
					]
				})
			});
		})(), /* @__PURE__ */ (0, import_jsx_runtime.jsx)(AnimatePresence, {
			mode: "wait",
			children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)(motion.div, {
				layout: true,
				initial: {
					opacity: 0,
					x: 30
				},
				animate: {
					opacity: 1,
					x: 0
				},
				exit: {
					opacity: 0,
					x: -30
				},
				transition: { duration: .1 },
				children: (() => {
					if (step == 1) return /* @__PURE__ */ (0, import_jsx_runtime.jsx)(Step1_Welcome, { nextStep });
					else if (step == 2) return /* @__PURE__ */ (0, import_jsx_runtime.jsx)(Step2_Theme, {
						setValue,
						nextStep,
						prevStep,
						watch
					});
					else if (step == 3) return /* @__PURE__ */ (0, import_jsx_runtime.jsx)(Step3_ManagerSettings, {
						setValue,
						getValues,
						register,
						nextStep,
						prevStep
					});
					else if (step == 4) return /* @__PURE__ */ (0, import_jsx_runtime.jsx)(Step4_MqttSettings, {
						setValue,
						getValues,
						register,
						nextStep,
						prevStep
					});
					else if (step == 5) return /* @__PURE__ */ (0, import_jsx_runtime.jsx)(Step5_HomeAssistantSettings, {
						getValues,
						register,
						nextStep,
						prevStep,
						isHomeAssistantAddon
					});
					else if (step == 6) return /* @__PURE__ */ (0, import_jsx_runtime.jsx)(Step6_OpenhabSettings, {
						getValues,
						register,
						nextStep,
						prevStep
					});
					else if (step == 7) return /* @__PURE__ */ (0, import_jsx_runtime.jsx)(Step7_Finished, {
						form,
						isHomeAssistantAddon,
						prevStep,
						dialogRef
					});
				})()
			}, `step.${step}`)
		})]
	}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", { className: "modal-backdrop" })] });
};
//#endregion
export { InitialSetup as default };
