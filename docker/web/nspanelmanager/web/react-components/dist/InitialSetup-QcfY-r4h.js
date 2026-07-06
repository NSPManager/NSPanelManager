import { c as __toESM, r as require_react, t as require_jsx_runtime } from "./main-ZL8y8bxT.js";
import { n as AnimatePresence, r as useSettingsStore, t as motion } from "./proxy-VnFMqKQL.js";
import { n as Notify } from "./NSPanelToastContainer-CoQ7ri-y.js";
import { i as mdiInformationOutline, y as require_Icon } from "./mdi-BoWm9XqF.js";
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
//#region src/components/InitialSetup/Step2_Theme.tsx
var Step2_Theme = ({ setFormData, formData, nextStep, prevStep }) => {
	const [selectedTheme, setSelectedTheme] = (0, import_react.useState)("default");
	return /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", { children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
		className: "grid gap-4 grid-cols-3 mt-4 justify-end items-center",
		children: Object.entries({
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
		}).map(([theme_option, display_name]) => /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
			className: "bg-neutral text-neutral-content grid grid-cols-3 w-full h-20 rounded-box overflow-hidden cursor-pointer",
			"data-theme": theme_option,
			onClick: () => {
				setSelectedTheme(theme_option);
				setFormData({
					...formData,
					theme: theme_option
				});
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
						checked: theme_option === selectedTheme
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
	})] });
};
//#endregion
//#region src/components/InitialSetup/Step3_ManagerSettings.tsx
var Step3_ManagerSettings = ({ formData, setFormData, nextStep, prevStep }) => {
	const managerAddressRef = (0, import_react.useRef)(null);
	const managerPortRef = (0, import_react.useRef)(null);
	const getIPFromURL = () => {
		fetch("/rest/get_ip_by_hostname", {
			method: "POST",
			headers: { "Content-Type": "application/json" },
			body: JSON.stringify({ hostname: window.location.hostname })
		}).then((response) => response.json()).then((data) => {
			if (data.ip) {
				if (managerAddressRef.current) {
					managerAddressRef.current.value = data.ip;
					setFormData({
						...formData,
						manager_address: data.ip
					});
				}
			} else Notify({
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
			address: formData.manager_address,
			port: formData.manager_port.toString()
		};
		const queryString = new URLSearchParams(params).toString();
		fetch("/rest/mqttmanager/test?" + queryString, { method: "GET" }).then((response) => {
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
							onChange: (e) => setFormData({
								...formData,
								manager_address: e.target.value
							}),
							value: formData.manager_address,
							required: true,
							ref: managerAddressRef
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
							onChange: (e) => setFormData({
								...formData,
								manager_port: parseInt(e.target.value)
							}),
							value: formData.manager_port,
							required: true,
							ref: managerPortRef
						}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)("button", {
							type: "button",
							className: "btn btn-info rounded-l-none h-auto",
							onClick: () => managerPortRef.current && (managerPortRef.current.value = window.location.port) && setFormData({
								...formData,
								manager_port: parseInt(window.location.port)
							}),
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
			type: "submit",
			className: "btn btn-neutral",
			onClick: prevStep,
			children: "Back"
		}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)("button", {
			type: "submit",
			className: "btn btn-primary",
			onClick: nextStep,
			disabled: !formData.manager_address || !formData.manager_port,
			children: "Next"
		})]
	})] });
};
//#endregion
//#region src/components/InitialSetup/Step4_MqttSettings.tsx
var import_Icon = require_Icon();
var Step4_MqttSettings = ({ formData, setFormData, nextStep, prevStep }) => {
	const [anonymousLogin, setAnonymousLogin] = (0, import_react.useState)(formData.mqtt_username === "");
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
								onChange: (e) => setFormData({
									...formData,
									mqtt_server: e.target.value
								}),
								value: formData.mqtt_server,
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
								onChange: (e) => setFormData({
									...formData,
									mqtt_port: parseInt(e.target.value)
								}),
								value: formData.mqtt_port,
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
								checked: anonymousLogin,
								onChange: () => {
									setAnonymousLogin(!anonymousLogin);
									setFormData({
										...formData,
										mqtt_username: "",
										mqtt_password: ""
									});
								}
							}), "Anonymous login"]
						}),
						/* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
							className: "flex justify-center my-4 duration-500 transition-transform ease-linear",
							hidden: anonymousLogin,
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
										onChange: (e) => setFormData({
											...formData,
											mqtt_username: e.target.value
										}),
										value: formData.mqtt_username,
										disabled: anonymousLogin,
										required: true
									})
								})]
							})
						}),
						/* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
							className: "flex justify-center mb-4 duration-500 transition-transform ease-linear",
							hidden: anonymousLogin,
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
										onChange: (e) => setFormData({
											...formData,
											mqtt_password: e.target.value
										}),
										value: formData.mqtt_password,
										disabled: anonymousLogin,
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
				type: "submit",
				className: "btn btn-neutral",
				onClick: prevStep,
				children: "Back"
			}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)("button", {
				type: "submit",
				className: "btn btn-primary",
				onClick: nextStep,
				disabled: !formData.mqtt_server || !formData.mqtt_port,
				children: "Next"
			})]
		})
	] });
};
//#endregion
//#region src/components/InitialSetup/Step5_HomeAssistantSettings.tsx
var Step5_HomeAssistantSettings = ({ formData, setFormData, nextStep, prevStep, isHomeAssistantAddon }) => {
	const testHomeAssistant = () => {
		const params = {
			address: formData.home_assistant_address,
			token: formData.home_assistant_token
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
			}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", { children: "You are running NSPanel Manager as a Home Assistant add on. These settings are managed for you by Home Assistant. Simply press next to continue." })]
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
							onChange: (e) => setFormData({
								...formData,
								home_assistant_address: e.target.value
							}),
							value: formData.home_assistant_address,
							disabled: isHomeAssistantAddon,
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
						children: "Home Assistant Token"
					}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
						className: "flex",
						children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)("input", {
							className: "outline-none bg-base-300 border-neutral rounded-md border focus:ring-0 focus:border-accent block flex-1 min-w-0 w-full text-sm p-2.5",
							type: "password",
							onChange: (e) => setFormData({
								...formData,
								home_assistant_token: e.target.value
							}),
							value: formData.home_assistant_token,
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
var Step6_OpenhabSettings = ({ formData, setFormData, nextStep, prevStep }) => {
	const testOpenhab = () => {
		const params = {
			address: formData.openhab_address,
			token: formData.openhab_token
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
							onChange: (e) => setFormData({
								...formData,
								openhab_address: e.target.value
							}),
							value: formData.openhab_address,
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
							onChange: (e) => setFormData({
								...formData,
								openhab_token: e.target.value
							}),
							value: formData.openhab_token,
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
var Step7_Finished = ({ formData, prevStep }) => {
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
	const saveSettings = () => {
		fetch("/rest/settings", {
			method: "POST",
			headers: {
				"Content-Type": "application/json",
				"X-CSRFToken": getCookie("csrftoken")
			},
			body: JSON.stringify({ settings: formData })
		}).then((response) => {
			if (response.ok) {
				Notify({
					message: "Settings saved successfully.",
					level: "success",
					duration: 5e3
				});
				window.location.reload();
			} else response.json().then((data) => {
				Notify({
					message: "Failed to save settings. Error: " + data.message,
					level: "error",
					duration: 5e3
				});
			});
		}).catch((error) => {
			Notify({
				message: "Failed to save settings. Error: " + error.message,
				level: "error",
				duration: 5e3
			});
		});
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
				children: "Initial setup is now finished. If you have any NSPanels already flashed and connected to the same MQTT broker they should show up automatically. In case they do not, there is the documentation and also a discord where you can get help. There is also the Github page where you can report issues and contribute."
			}),
			/* @__PURE__ */ (0, import_jsx_runtime.jsx)("p", {
				className: "my-2 text-center font-bold",
				children: "Thank you for your support and trust and have fun!"
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
					type: "submit",
					className: "btn btn-neutral",
					onClick: prevStep,
					children: "Back"
				}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)("button", {
					type: "submit",
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
	const [step, setStep] = (0, import_react.useState)(1);
	const [hasPopulatedFromExistingSettings, setHasPopulatedFromExistingSettings] = (0, import_react.useState)(false);
	const [formData, setFormData] = (0, import_react.useState)({
		theme: "default",
		manager_address: "",
		manager_port: 8e3,
		mqtt_server: "",
		mqtt_port: 1883,
		mqtt_username: "",
		mqtt_password: "",
		home_assistant_address: "",
		home_assistant_token: "",
		openhab_address: "",
		openhab_token: ""
	});
	const settingsStoreStatus = useSettingsStore((state) => state.status);
	if (settingsStoreStatus == "none") useSettingsStore.getState().fetchData();
	else if (settingsStoreStatus == "loaded" && !hasPopulatedFromExistingSettings) {
		console.log("Settings loaded, updating formData for initial setup.");
		setFormData((prev) => ({
			...prev,
			theme: useSettingsStore.getState().settings?.theme ?? "default",
			manager_address: useSettingsStore.getState().settings?.manager_address ?? "",
			manager_port: useSettingsStore.getState().settings?.manager_port ?? 8e3,
			mqtt_server: useSettingsStore.getState().settings?.mqtt_server ?? "",
			mqtt_port: useSettingsStore.getState().settings?.mqtt_port ?? 1883,
			home_assistant_address: useSettingsStore.getState().settings?.home_assistant_address ?? "",
			openhab_address: useSettingsStore.getState().settings?.openhab_address ?? ""
		}));
		setHasPopulatedFromExistingSettings(true);
	}
	const nextStep = () => setStep((prev) => prev + 1);
	const prevStep = () => setStep((prev) => prev - 1);
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
		children: [/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
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
							formData,
							setFormData,
							nextStep,
							prevStep
						});
						else if (step == 3) return /* @__PURE__ */ (0, import_jsx_runtime.jsx)(Step3_ManagerSettings, {
							formData,
							setFormData,
							nextStep,
							prevStep
						});
						else if (step == 4) return /* @__PURE__ */ (0, import_jsx_runtime.jsx)(Step4_MqttSettings, {
							formData,
							setFormData,
							nextStep,
							prevStep
						});
						else if (step == 5) return /* @__PURE__ */ (0, import_jsx_runtime.jsx)(Step5_HomeAssistantSettings, {
							formData,
							setFormData,
							nextStep,
							prevStep,
							isHomeAssistantAddon
						});
						else if (step == 6) return /* @__PURE__ */ (0, import_jsx_runtime.jsx)(Step6_OpenhabSettings, {
							formData,
							setFormData,
							nextStep,
							prevStep
						});
						else if (step == 7) return /* @__PURE__ */ (0, import_jsx_runtime.jsx)(Step7_Finished, {
							formData,
							prevStep
						});
					})()
				}, `step.${step}`)
			})]
		}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", { className: "modal-backdrop" })]
	});
};
//#endregion
export { InitialSetup as default };
