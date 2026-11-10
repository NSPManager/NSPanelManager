import { c as __toESM, r as require_react, t as require_jsx_runtime } from "./main-Ce1xTlTP.js";
import { c as useFormContext, i as get, l as useWatch, n as FormProvider, s as useForm } from "./index.esm-mYyDCATx.js";
import { n as Notify } from "./NSPanelToastContainer-KbJyV6eL.js";
import { a as RadioGroup$1, c as TextInput$1, i as NumberInput$1, l as api, n as Card, o as SecretInput$1, r as Row, s as Select$1, t as Alert, u as getCookie } from "./layout-DaQaV5-g.js";
//#region src/api/connectionTests.ts
var import_react = /* @__PURE__ */ __toESM(require_react(), 1);
async function errorMessage(response) {
	try {
		const data = await response.json();
		return data.message || data.error || `HTTP ${response.status}`;
	} catch {
		return `HTTP ${response.status}`;
	}
}
async function runTest(url, params) {
	try {
		const response = await fetch(`${url}?${new URLSearchParams(params).toString()}`, {
			method: "GET",
			credentials: "same-origin"
		});
		return response.ok ? null : await errorMessage(response);
	} catch (error) {
		return error instanceof Error ? error.message : String(error);
	}
}
/** The manager's IP address for a hostname, as the server resolves it. */
async function getIpByHostname(hostname) {
	const response = await fetch("/rest/get_ip_by_hostname", {
		method: "POST",
		credentials: "same-origin",
		headers: {
			"Content-Type": "application/json",
			"X-CSRFToken": getCookie("csrftoken")
		},
		body: JSON.stringify({ hostname })
	});
	if (!response.ok) throw new Error(await errorMessage(response));
	const data = await response.json();
	if (!data.ip) throw new Error("No \"ip\" in the response.");
	return data.ip;
}
function testManager(address, port) {
	return runTest("/rest/mqttmanager/test", {
		address,
		port: String(port)
	});
}
function testHomeAssistant(address, token) {
	return runTest("/rest/home_assistant/test", {
		address,
		token
	});
}
function testOpenhab(address, token) {
	return runTest("/rest/openhab/test", {
		address,
		token
	});
}
var MAX_TOTAL_DURATION_MS = 3e4;
var ARTICULATION_GAP_MS = 10;
var OCTAVE8_FREQUENCIES = [
	66976,
	70959,
	75178,
	79649,
	84385,
	89402,
	94719,
	100351,
	106318,
	112640,
	119338,
	126434
];
var SEMITONES = {
	c: 0,
	d: 2,
	e: 4,
	f: 5,
	g: 7,
	a: 9,
	b: 11,
	h: 11,
	p: -1
};
var VALID_DURATIONS = [
	1,
	2,
	4,
	8,
	16,
	32,
	64
];
var WHITESPACE = " 	\n\v\f\r";
var isDigit = (c) => c !== void 0 && c >= "0" && c <= "9";
var RtttlError = class extends Error {
	tooLong;
	constructor(message, tooLong = false) {
		super(message);
		this.tooLong = tooLong;
	}
};
/** Parse an RTTTL string, ie. "tick:d=64,o=7,b=625:b", the way the panel does. */
function parseRtttl(rtttl) {
	try {
		const steps = parse$1(rtttl);
		return {
			ok: true,
			steps,
			durationMs: steps.reduce((total, [, ms]) => total + ms, 0)
		};
	} catch (error) {
		if (error instanceof RtttlError) return {
			ok: false,
			error: error.message,
			tooLong: error.tooLong
		};
		throw error;
	}
}
function parse$1(rtttl) {
	let text = "";
	for (const c of rtttl) if (!WHITESPACE.includes(c)) text += c >= "A" && c <= "Z" ? c.toLowerCase() : c;
	const sections = text.split(":");
	if (sections.length !== 3) throw new RtttlError("Expected name:defaults:notes, for example tick:d=64,o=7,b=625:b.");
	const [, defaults, notes] = sections;
	const [duration, octave, bpm] = parseDefaults(defaults);
	const steps = [];
	let totalDurationMs = 0;
	const noteTexts = notes.split(",");
	for (let index = 0; index < noteTexts.length; index++) {
		const note = noteTexts[index];
		if (note === "" && index === noteTexts.length - 1 && steps.length > 0) break;
		const step = parseNote(note, duration, octave, bpm, index + 1);
		const last = steps[steps.length - 1];
		if (last && step[0] > 0 && last[0] === step[0] && last[1] > 20) {
			last[1] -= ARTICULATION_GAP_MS;
			steps.push([0, ARTICULATION_GAP_MS]);
		}
		steps.push(step);
		totalDurationMs += step[1];
		if (steps.length > 256) throw new RtttlError(`Too many notes: the panel plays at most 256 steps.`, true);
		if (totalDurationMs > 3e4) throw new RtttlError(`Too long: the panel plays at most ${MAX_TOTAL_DURATION_MS / 1e3} seconds.`, true);
	}
	return steps;
}
function parseDefaults(section) {
	let [duration, octave, bpm] = [
		4,
		6,
		63
	];
	const malformed = () => new RtttlError("Defaults must look like d=4,o=6,b=63.");
	let pos = 0;
	while (pos < section.length) {
		if (pos + 2 > section.length || section[pos + 1] !== "=") throw malformed();
		const key = section[pos];
		pos += 2;
		const [next, value] = parseNumber(section, pos, 999);
		if (value === null) throw malformed();
		pos = next;
		if (key === "d" && VALID_DURATIONS.includes(value)) duration = value;
		else if (key === "o" && value >= 1 && value <= 8) octave = value;
		else if (key === "b" && value >= 1 && value <= 900) bpm = value;
		else if (key === "d") throw new RtttlError("d must be 1, 2, 4, 8, 16, 32 or 64.");
		else if (key === "o") throw new RtttlError("o must be 1 to 8.");
		else if (key === "b") throw new RtttlError("b must be 1 to 900.");
		else throw new RtttlError(`Unknown default '${key}': only d, o and b are allowed.`);
		if (pos < section.length) {
			if (section[pos] !== ",") throw malformed();
			pos++;
		}
	}
	return [
		duration,
		octave,
		bpm
	];
}
function parseNote(note, defaultDuration, defaultOctave, bpm, position) {
	const invalid = () => new RtttlError(`Note ${position} ('${note}') is not valid.`);
	let pos = 0;
	let duration = defaultDuration;
	if (isDigit(note[pos])) {
		const [next, value] = parseNumber(note, pos, 64);
		if (value === null || !VALID_DURATIONS.includes(value)) throw invalid();
		[pos, duration] = [next, value];
	}
	if (pos >= note.length || !Object.hasOwn(SEMITONES, note[pos])) throw invalid();
	let semitone = SEMITONES[note[pos]];
	pos++;
	if (note[pos] === "#") {
		if (semitone < 0) throw invalid();
		semitone++;
		pos++;
	}
	let dotted = false;
	if (note[pos] === ".") {
		dotted = true;
		pos++;
	}
	let octave = defaultOctave;
	if (isDigit(note[pos])) {
		octave = Number(note[pos]);
		if (octave < 1 || octave > 8) throw invalid();
		pos++;
	}
	if (!dotted && note[pos] === ".") {
		dotted = true;
		pos++;
	}
	if (pos !== note.length) throw invalid();
	const numerator = 24e4 * (dotted ? 3 : 2);
	const denominator = bpm * duration * 2;
	const durationMs = Math.floor((numerator + Math.floor(denominator / 2)) / denominator);
	if (semitone < 0) return [0, durationMs];
	if (semitone === 12) {
		semitone = 0;
		octave++;
	}
	const shift = 12 - octave;
	return [OCTAVE8_FREQUENCIES[semitone] + (1 << shift - 1) >> shift, durationMs];
}
/** Read the digits at pos: [new pos, value], value null if there were none or it exceeds maxValue. */
function parseNumber(text, pos, maxValue) {
	const start = pos;
	let value = 0;
	while (isDigit(text[pos])) {
		value = value * 10 + Number(text[pos]);
		if (value > maxValue) return [pos, null];
		pos++;
	}
	return pos === start ? [pos, null] : [pos, value];
}
//#endregion
//#region src/lib/rtttlPlayer.ts
var VOLUME = .15;
var RAMP_S = .001;
var PIEZO_RESONANCE_HZ = 4e3;
var PIEZO_Q = 2.5;
var PIEZO_MAKEUP_GAIN = 2;
var START_DELAY_S = .02;
var sharedContext = null;
function isAudioSupported() {
	return typeof window !== "undefined" && typeof window.AudioContext === "function";
}
function audioContext() {
	sharedContext ??= new AudioContext();
	if (sharedContext.state === "suspended") sharedContext.resume();
	return sharedContext;
}
/** Play the steps; done resolves when they finish or stop() is called. */
function playSteps(steps, { piezo }) {
	const context = audioContext();
	const oscillator = context.createOscillator();
	oscillator.type = "square";
	const gain = context.createGain();
	gain.gain.value = 0;
	const nodes = [oscillator, gain];
	if (piezo) {
		const filter = context.createBiquadFilter();
		filter.type = "bandpass";
		filter.frequency.value = PIEZO_RESONANCE_HZ;
		filter.Q.value = PIEZO_Q;
		oscillator.connect(filter).connect(gain);
		nodes.push(filter);
	} else oscillator.connect(gain);
	gain.connect(context.destination);
	const volume = VOLUME * (piezo ? PIEZO_MAKEUP_GAIN : 1);
	const start = context.currentTime + START_DELAY_S;
	let time = start;
	for (const [frequency, ms] of steps) {
		const seconds = ms / 1e3;
		if (frequency > 0) {
			const ramp = Math.min(RAMP_S, seconds / 2);
			oscillator.frequency.setValueAtTime(frequency, time);
			gain.gain.setValueAtTime(0, time);
			gain.gain.linearRampToValueAtTime(volume, time + ramp);
			gain.gain.setValueAtTime(volume, time + seconds - ramp);
			gain.gain.linearRampToValueAtTime(0, time + seconds);
		}
		time += seconds;
	}
	const done = new Promise((resolve) => {
		oscillator.onended = () => {
			nodes.forEach((node) => node.disconnect());
			resolve();
		};
	});
	oscillator.start(start);
	oscillator.stop(time);
	return {
		stop: () => {
			try {
				oscillator.stop();
			} catch {}
		},
		done
	};
}
/**
* One preview at a time for a page: starting one stops the one playing. playing is the key of the
* preview playing, so its button can show a stop symbol.
*/
function useRtttlPlayer() {
	const [playing, setPlaying] = (0, import_react.useState)(null);
	const current = (0, import_react.useRef)(null);
	const stop = (0, import_react.useCallback)(() => {
		current.current?.playback.stop();
		current.current = null;
		setPlaying(null);
	}, []);
	const play = (0, import_react.useCallback)((key, steps, options) => {
		stop();
		const playback = playSteps(steps, options);
		const entry = {
			key,
			playback
		};
		current.current = entry;
		setPlaying(key);
		playback.done.then(() => {
			if (current.current === entry) {
				current.current = null;
				setPlaying(null);
			}
		});
	}, [stop]);
	(0, import_react.useEffect)(() => () => current.current?.playback.stop(), []);
	return {
		playing,
		play,
		stop
	};
}
//#endregion
//#region src/components/SettingsPage/buzzer.ts
var PRESETS = {
	tick: {
		id: "tick",
		name: "Tick",
		rtttl: "tick:d=64,o=7,b=625:b"
	},
	pip: {
		id: "pip",
		name: "Pip",
		rtttl: "pip:d=64,o=7,b=900:c"
	},
	midTick: {
		id: "midTick",
		name: "Mid tick",
		rtttl: "mtick:d=64,o=6,b=625:g"
	},
	tuk: {
		id: "tuk",
		name: "Tuk",
		rtttl: "tuk:d=64,o=6,b=900:e,c"
	},
	tock: {
		id: "tock",
		name: "Tock",
		rtttl: "tock:d=64,o=5,b=900:a"
	},
	chime: {
		id: "chime",
		name: "Chime",
		rtttl: "chime:d=32,o=6,b=190:g6,16p,32c7"
	},
	confirm: {
		id: "confirm",
		name: "Confirm",
		rtttl: "confirm:d=16,o=6,b=357:e,g"
	},
	rising: {
		id: "rising",
		name: "Rising",
		rtttl: "rising:d=16,o=6,b=180:c,e,g"
	},
	saved: {
		id: "saved",
		name: "Saved",
		rtttl: "saved:d=16,o=6,b=200:c,e,g,8c7"
	},
	silent: {
		id: "silent",
		name: "Silent",
		rtttl: "silent:d=64,o=6,b=900:p"
	}
};
var BUZZER_EVENTS = [
	{
		name: "buzzer_sound_touch",
		label: "Screen touch",
		blankLabel: "Off",
		hint: "Played on every press of the screen.",
		presets: [
			PRESETS.tick,
			PRESETS.pip,
			PRESETS.midTick,
			PRESETS.tuk,
			PRESETS.tock,
			PRESETS.chime,
			PRESETS.confirm,
			PRESETS.rising,
			PRESETS.saved
		]
	},
	{
		name: "buzzer_sound_scene_activated",
		label: "Scene activated",
		blankLabel: "Same as screen touch",
		hint: "Played instead of the touch sound when a scene is activated from the panel.",
		presets: [
			PRESETS.silent,
			PRESETS.confirm,
			PRESETS.rising,
			PRESETS.chime,
			PRESETS.tick,
			PRESETS.pip,
			PRESETS.midTick,
			PRESETS.tuk,
			PRESETS.tock,
			PRESETS.saved
		]
	},
	{
		name: "buzzer_sound_scene_saved",
		label: "Scene saved",
		blankLabel: "Off",
		hint: "Played when a scene is saved with a long press on the panel.",
		presets: [
			PRESETS.saved,
			PRESETS.rising,
			PRESETS.confirm,
			PRESETS.chime,
			PRESETS.tick,
			PRESETS.pip,
			PRESETS.midTick,
			PRESETS.tuk,
			PRESETS.tock
		]
	}
];
var BLANK = "blank";
var CUSTOM = "custom";
/** The dropdown choice for a stored value: BLANK, CUSTOM or a preset id. */
function choiceFor(event, rtttl) {
	if (rtttl === "") return BLANK;
	return event.presets.find((preset) => preset.rtttl === rtttl)?.id ?? "custom";
}
//#endregion
//#region src/components/SettingsPage/BuzzerCard.tsx
var import_jsx_runtime = require_jsx_runtime();
/** The buzzer sound for each panel event, with presets and a preview played in the browser. */
function BuzzerCard() {
	const player = useRtttlPlayer();
	const [piezo, setPiezo] = (0, import_react.useState)(true);
	const { control } = useFormContext();
	const touch = useWatch({
		control,
		name: "buzzer_sound_touch"
	}) ?? "";
	return /* @__PURE__ */ (0, import_jsx_runtime.jsxs)(Card, {
		title: "Buzzer sounds",
		children: [
			/* @__PURE__ */ (0, import_jsx_runtime.jsx)("p", {
				className: "mb-2 text-sm opacity-80",
				children: "Sonoff NSPanels only. Panels make no sound unless you choose one here. Each panel also has a buzzer mute switch in Home Assistant."
			}),
			BUZZER_EVENTS.map((event) => /* @__PURE__ */ (0, import_jsx_runtime.jsx)(BuzzerSoundField, {
				event,
				player,
				piezo,
				touch
			}, event.name)),
			isAudioSupported() ? /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("label", {
				className: "label text-sm",
				children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("input", {
					type: "checkbox",
					className: "checkbox checkbox-accent checkbox-sm",
					checked: piezo,
					onChange: (event) => setPiezo(event.target.checked)
				}), "Make the preview sound like the panel's buzzer (approximate)"]
			}) : /* @__PURE__ */ (0, import_jsx_runtime.jsx)("p", {
				className: "text-sm opacity-60",
				children: "This browser can't play the preview."
			})
		]
	});
}
var parse = (rtttl) => rtttl === "" ? null : parseRtttl(rtttl);
function BuzzerSoundField({ event, player, piezo, touch }) {
	const { name, label } = event;
	const { register, setValue, setFocus, control, formState: { errors } } = useFormContext();
	const value = useWatch({
		control,
		name
	}) ?? "";
	const [custom, setCustom] = (0, import_react.useState)(false);
	const choice = custom ? CUSTOM : choiceFor(event, value);
	const parsed = parse(value);
	const error = parsed && !parsed.ok ? parsed.error : get(errors, name)?.message;
	const preview = value === "" && name === "buzzer_sound_scene_activated" ? parse(touch) : parsed;
	const steps = preview?.ok ? preview.steps : null;
	const choose = (selected) => {
		setCustom(selected === CUSTOM);
		if (selected === "custom") {
			setFocus(name);
			return;
		}
		const rtttl = selected === "blank" ? "" : event.presets.find((preset) => preset.id === selected)?.rtttl ?? "";
		setValue(name, rtttl, {
			shouldDirty: true,
			shouldValidate: true
		});
		if (player.playing === name) player.stop();
	};
	const playing = player.playing === name;
	const previewLabel = playing ? `Stop the ${label.toLowerCase()} preview` : `Preview the ${label.toLowerCase()} sound`;
	return /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
		className: "mb-3 w-full",
		children: [
			/* @__PURE__ */ (0, import_jsx_runtime.jsx)("label", {
				htmlFor: `${name}-choice`,
				className: "block mb-1 text-sm font-medium",
				children: label
			}),
			/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
				className: "flex flex-wrap md:flex-nowrap gap-2",
				children: [/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("select", {
					id: `${name}-choice`,
					className: "select md:w-56 shrink-0",
					value: choice,
					onChange: (change) => choose(change.target.value),
					children: [
						/* @__PURE__ */ (0, import_jsx_runtime.jsx)("option", {
							value: BLANK,
							children: event.blankLabel
						}),
						event.presets.map((preset) => /* @__PURE__ */ (0, import_jsx_runtime.jsx)("option", {
							value: preset.id,
							children: preset.name
						}, preset.id)),
						/* @__PURE__ */ (0, import_jsx_runtime.jsx)("option", {
							value: CUSTOM,
							children: "Custom…"
						})
					]
				}), /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
					className: "join w-full",
					children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("input", {
						id: name,
						type: "text",
						"aria-label": `${label} RTTTL`,
						className: "input w-full join-item font-mono",
						maxLength: 255,
						spellCheck: false,
						autoComplete: "off",
						placeholder: choice === "custom" ? "name:d=4,o=6,b=63:c,e,g" : "",
						"aria-invalid": error !== void 0,
						"aria-describedby": `${name}-message`,
						...register(name, {
							setValueAs: (raw) => raw ?? "",
							validate: (raw) => {
								const result = parse(raw ?? "");
								return result === null || result.ok || result.error;
							}
						})
					}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)("button", {
						type: "button",
						className: "btn btn-accent join-item",
						"aria-label": previewLabel,
						title: previewLabel,
						disabled: !steps || !isAudioSupported(),
						onClick: () => playing ? player.stop() : steps && player.play(name, steps, { piezo }),
						children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", { className: `mdi ${playing ? "mdi-stop" : "mdi-play"}` })
					})]
				})]
			}),
			/* @__PURE__ */ (0, import_jsx_runtime.jsx)("p", {
				id: `${name}-message`,
				className: `mt-1 text-sm ${error ? "text-error" : "opacity-60"}`,
				children: error ?? `${event.hint}${parsed?.ok ? ` ${describe(parsed.steps, parsed.durationMs)}` : ""}`
			})
		]
	});
}
function describe(steps, durationMs) {
	return steps.every(([frequency]) => frequency === 0) ? "Silent." : `Plays for ${formatDuration(durationMs)}.`;
}
function formatDuration(ms) {
	return ms < 1e3 ? `${ms} ms` : `${(ms / 1e3).toFixed(ms < 1e4 ? 2 : 1)} s`;
}
//#endregion
//#region src/components/SettingsPage/choices.ts
var LOG_LEVELS = {
	error: "Error",
	warning: "Warning",
	info: "Info",
	debug: "Debug",
	trace: "Trace"
};
var NSPANEL_TYPES = {
	eu: "EU",
	us: "US"
};
var TURN_ON_BEHAVIORS = {
	color_temp: "Color temperature",
	restore: "Restore"
};
var SCREENSAVER_MODES = {
	with_background: "Show with background (clock, date and weather)",
	without_background: "Show without background (clock, date and weather)",
	datetime_with_background: "Show with background (no weather)",
	datetime_without_background: "Show without background (no weather)",
	no_screensaver: "No screensaver (turn off screen)"
};
var options = (choices) => Object.entries(choices);
//#endregion
//#region src/components/common/buttons.tsx
/** A button joined to the end of an input: pass it as the input's `button`. */
function JoinButton({ onClick, children }) {
	return /* @__PURE__ */ (0, import_jsx_runtime.jsx)("button", {
		type: "button",
		className: "btn btn-info join-item",
		onClick,
		children
	});
}
/** Runs a connection test and reports the result as a toast. With a disabledReason it is disabled, and shows why. */
function TestButton({ label, run, disabledReason }) {
	const [running, setRunning] = (0, import_react.useState)(false);
	const onClick = async () => {
		setRunning(true);
		const error = await run();
		setRunning(false);
		if (error) Notify({
			message: `${label} failed: ${error}`,
			level: "error",
			duration: 8e3
		});
		else Notify({
			message: `${label}: OK.`,
			level: "success",
			duration: 5e3
		});
	};
	return /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
		className: "mt-1",
		children: [/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("button", {
			type: "button",
			className: "btn btn-accent btn-sm",
			onClick,
			disabled: running || disabledReason !== void 0,
			children: [running && /* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", { className: "loading loading-spinner loading-sm" }), label]
		}), disabledReason && /* @__PURE__ */ (0, import_jsx_runtime.jsx)("p", {
			className: "mt-1 text-sm opacity-60",
			children: disabledReason
		})]
	});
}
//#endregion
//#region src/components/common/typedFields.ts
/** The inputs in ./fields with their field names checked against the form T. Call it once, at module level. */
function typedFields() {
	return {
		TextInput: TextInput$1,
		NumberInput: NumberInput$1,
		Select: Select$1,
		RadioGroup: RadioGroup$1,
		SecretInput: SecretInput$1
	};
}
//#endregion
//#region src/components/SettingsPage/form.ts
var PAGE_FIELDS = [
	"manager_address",
	"manager_port",
	"mqttmanager_log_level",
	"max_live_log_messages",
	"max_log_buffer_size",
	"default_nspanel_type",
	"color_temp_min",
	"color_temp_max",
	"reverse_color_temp",
	"turn_on_behavior",
	"raise_to_100_light_level",
	"min_button_push_time",
	"button_long_press_time",
	"special_mode_trigger_time",
	"special_mode_release_time",
	"screen_dim_level",
	"screensaver_dim_level",
	"screensaver_activation_timeout",
	"screensaver_mode",
	"show_screensaver_inside_temperature",
	"show_screensaver_outside_temperature",
	"optimistic_mode",
	"room_status_backoff_time",
	"all_rooms_status_backoff_time",
	"light_turn_on_brightness",
	"mqtt_server",
	"mqtt_port",
	"mqtt_username",
	"home_assistant_address",
	"openhab_address",
	"buzzer_sound_touch",
	"buzzer_sound_scene_activated",
	"buzzer_sound_scene_saved"
];
var SECRETS = [
	"mqtt_password",
	"home_assistant_token",
	"openhab_token"
];
var ADDON_MANAGED = ["home_assistant_address", "home_assistant_token"];
var MODES_WITH_OUTSIDE_TEMPERATURE = ["datetime_with_background", "datetime_without_background"];
function toFormValues(settings) {
	const values = {};
	for (const name of PAGE_FIELDS) values[name] = settings[name];
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
	for (const name of PAGE_FIELDS) if (dirty[name] && allowed(name)) body[name] = values[name];
	for (const secret of SECRETS) {
		if (!allowed(secret)) continue;
		if (values[`clear_${secret}`]) body[secret] = "";
		else if (values[secret]) body[secret] = values[secret];
	}
	return body;
}
//#endregion
//#region src/components/SettingsPage/SettingsPage.tsx
var { NumberInput, RadioGroup, SecretInput, Select, TextInput } = typedFields();
/** The global settings page (/settings), backed by /rest/v2/settings. */
function SettingsPage({ logDownloadUrl = "/download_mqttmanager_log" }) {
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
	return /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", { children: [
		/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
			className: "flex items-center justify-between mb-2",
			children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("h2", {
				className: "font-medium text-2xl",
				children: "Settings"
			}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)("a", {
				href: "https://nspanelmanager.com/docs/web-interface/#global-settings",
				target: "_blank",
				rel: "noreferrer",
				className: "btn btn-outline btn-info",
				children: "Help"
			})]
		}),
		state.status === "loading" && /* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", {
			className: "loading loading-spinner",
			"aria-label": "Loading settings"
		}),
		state.status === "error" && /* @__PURE__ */ (0, import_jsx_runtime.jsxs)(import_jsx_runtime.Fragment, { children: [/* @__PURE__ */ (0, import_jsx_runtime.jsxs)(Alert, {
			level: "error",
			children: ["Couldn't load the settings: ", state.message]
		}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)("button", {
			type: "button",
			className: "btn",
			onClick: retry,
			children: "Retry"
		})] }),
		state.status === "loaded" && /* @__PURE__ */ (0, import_jsx_runtime.jsx)(SettingsForm, {
			settings: state.settings,
			logDownloadUrl
		})
	] });
}
function SettingsForm({ settings: loaded, logDownloadUrl }) {
	const [settings, setSettings] = (0, import_react.useState)(loaded);
	const form = useForm({ defaultValues: toFormValues(loaded) });
	const { handleSubmit, reset, setError, setValue, getValues, control, formState: { isDirty, isSubmitting, dirtyFields } } = form;
	const addon = settings.is_home_assistant_addon;
	const onSubmit = async (values) => {
		const body = buildPatch(values, dirtyFields, addon);
		if (Object.keys(body).length === 0) {
			reset(values);
			return;
		}
		try {
			const { data, error, response } = await api.PATCH("/rest/v2/settings", { body });
			if (data) {
				setSettings(data);
				reset(toFormValues(data));
				Notify({
					message: "Settings saved. Reboot the panels for panel settings to take effect.",
					level: "success",
					duration: 5e3
				});
				return;
			}
			if (response.status === 400 && error && !("detail" in error && error.detail)) {
				const unplaced = applyFieldErrors(error, values, setError);
				Notify({
					message: `Nothing was saved. ${unplaced.length ? unplaced.join(" ") : "Fix the highlighted settings."}`,
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
	const loadAddressFromUrl = async () => {
		try {
			setValue("manager_address", await getIpByHostname(window.location.hostname), { shouldDirty: true });
		} catch (error) {
			Notify({
				message: `Couldn't get the manager's IP address from ${window.location.hostname}: ${error instanceof Error ? error.message : error}`,
				level: "error"
			});
		}
	};
	const loadPortFromUrl = () => {
		const port = window.location.port || (window.location.protocol === "https:" ? "443" : "80");
		setValue("manager_port", Number(port), { shouldDirty: true });
	};
	const [screensaverMode, typedHomeAssistantToken, typedOpenhabToken] = useWatch({
		control,
		name: [
			"screensaver_mode",
			"home_assistant_token",
			"openhab_token"
		]
	});
	return /* @__PURE__ */ (0, import_jsx_runtime.jsx)(FormProvider, {
		...form,
		children: /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("form", {
			onSubmit: handleSubmit(onSubmit),
			noValidate: true,
			children: [
				/* @__PURE__ */ (0, import_jsx_runtime.jsx)(Alert, {
					level: "info",
					children: "Changes to NSPanel settings on this page require a reboot of all NSPanels to take effect."
				}),
				/* @__PURE__ */ (0, import_jsx_runtime.jsxs)(Card, {
					title: "NSPanel Manager",
					children: [
						/* @__PURE__ */ (0, import_jsx_runtime.jsx)(Alert, {
							level: "warning",
							children: "The manager address must be an IP address only: not a hostname, and without http://."
						}),
						/* @__PURE__ */ (0, import_jsx_runtime.jsx)(TextInput, {
							name: "manager_address",
							label: "Manager address",
							button: /* @__PURE__ */ (0, import_jsx_runtime.jsx)(JoinButton, {
								onClick: loadAddressFromUrl,
								children: "Load from URL"
							})
						}),
						/* @__PURE__ */ (0, import_jsx_runtime.jsx)(NumberInput, {
							name: "manager_port",
							label: "Manager port",
							button: /* @__PURE__ */ (0, import_jsx_runtime.jsx)(JoinButton, {
								onClick: loadPortFromUrl,
								children: "Load from URL"
							})
						}),
						/* @__PURE__ */ (0, import_jsx_runtime.jsx)(TestButton, {
							label: "Test manager address",
							run: () => testManager(getValues("manager_address") ?? "", getValues("manager_port") ?? "")
						})
					]
				}),
				/* @__PURE__ */ (0, import_jsx_runtime.jsxs)(Card, {
					title: "Logging",
					actions: /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("a", {
						href: logDownloadUrl,
						title: "Download MQTTManager log",
						className: "btn btn-outline btn-info",
						children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", { className: "mdi mdi-download" }), " Download"]
					}),
					children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)(Select, {
						name: "mqttmanager_log_level",
						label: "MQTTManager log level",
						options: options(LOG_LEVELS)
					}), /* @__PURE__ */ (0, import_jsx_runtime.jsxs)(Row, { children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)(NumberInput, {
						name: "max_live_log_messages",
						label: "Messages shown in the live log"
					}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)(NumberInput, {
						name: "max_log_buffer_size",
						label: "Messages kept in the buffer"
					})] })]
				}),
				/* @__PURE__ */ (0, import_jsx_runtime.jsxs)(Card, {
					title: "Global NSPanel settings",
					children: [
						/* @__PURE__ */ (0, import_jsx_runtime.jsx)(Select, {
							name: "default_nspanel_type",
							label: "Default NSPanel type",
							options: options(NSPANEL_TYPES)
						}),
						/* @__PURE__ */ (0, import_jsx_runtime.jsxs)(Row, { children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)(NumberInput, {
							name: "color_temp_min",
							label: "Minimum color temperature (K)"
						}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)(NumberInput, {
							name: "color_temp_max",
							label: "Maximum color temperature (K)"
						})] }),
						/* @__PURE__ */ (0, import_jsx_runtime.jsx)(RadioGroup, {
							name: "reverse_color_temp",
							label: "Color temperature slider",
							options: [[false, "Keep default"], [true, "Reverse"]]
						}),
						/* @__PURE__ */ (0, import_jsx_runtime.jsx)(RadioGroup, {
							name: "turn_on_behavior",
							label: "Turn on behavior",
							options: options(TURN_ON_BEHAVIORS)
						}),
						/* @__PURE__ */ (0, import_jsx_runtime.jsxs)(Row, { children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)(NumberInput, {
							name: "raise_to_100_light_level",
							label: "Raise lights to 100% above (%)"
						}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)(NumberInput, {
							name: "light_turn_on_brightness",
							label: "Default light turn on brightness (%)"
						})] }),
						/* @__PURE__ */ (0, import_jsx_runtime.jsxs)(Row, { children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)(NumberInput, {
							name: "min_button_push_time",
							label: "Minimum button push time (ms)"
						}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)(NumberInput, {
							name: "button_long_press_time",
							label: "Minimum long press time (ms)"
						})] }),
						/* @__PURE__ */ (0, import_jsx_runtime.jsxs)(Row, { children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)(NumberInput, {
							name: "special_mode_trigger_time",
							label: "Special mode activation time (ms)"
						}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)(NumberInput, {
							name: "special_mode_release_time",
							label: "Special mode timeout (ms)"
						})] }),
						/* @__PURE__ */ (0, import_jsx_runtime.jsxs)(Row, { children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)(NumberInput, {
							name: "screen_dim_level",
							label: "Screen brightness (%)"
						}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)(NumberInput, {
							name: "screensaver_dim_level",
							label: "Screensaver brightness (%)"
						})] }),
						/* @__PURE__ */ (0, import_jsx_runtime.jsx)(NumberInput, {
							name: "screensaver_activation_timeout",
							label: "Screensaver activation timeout (ms)"
						}),
						/* @__PURE__ */ (0, import_jsx_runtime.jsx)(Select, {
							name: "screensaver_mode",
							label: "Screensaver mode",
							options: options(SCREENSAVER_MODES)
						}),
						/* @__PURE__ */ (0, import_jsx_runtime.jsx)(RadioGroup, {
							name: "show_screensaver_inside_temperature",
							label: "Screensaver inside temperature",
							options: [[true, "Show inside temperature"], [false, "Hide inside temperature"]]
						}),
						MODES_WITH_OUTSIDE_TEMPERATURE.includes(screensaverMode ?? "") && /* @__PURE__ */ (0, import_jsx_runtime.jsx)(RadioGroup, {
							name: "show_screensaver_outside_temperature",
							label: "Screensaver outside temperature",
							options: [[true, "Show outside temperature"], [false, "Hide outside temperature"]]
						}),
						/* @__PURE__ */ (0, import_jsx_runtime.jsx)(RadioGroup, {
							name: "optimistic_mode",
							label: "Entity update mode",
							options: [[true, "Optimistic"], [false, "Wait"]]
						}),
						/* @__PURE__ */ (0, import_jsx_runtime.jsxs)(Row, { children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)(NumberInput, {
							name: "room_status_backoff_time",
							label: "Room status backoff time (ms)"
						}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)(NumberInput, {
							name: "all_rooms_status_backoff_time",
							label: "\"All rooms\" status backoff time (ms)"
						})] })
					]
				}),
				/* @__PURE__ */ (0, import_jsx_runtime.jsx)(BuzzerCard, {}),
				/* @__PURE__ */ (0, import_jsx_runtime.jsxs)(Card, {
					title: "MQTT",
					children: [/* @__PURE__ */ (0, import_jsx_runtime.jsxs)(Row, { children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)(TextInput, {
						name: "mqtt_server",
						label: "MQTT server"
					}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)(NumberInput, {
						name: "mqtt_port",
						label: "MQTT port"
					})] }), /* @__PURE__ */ (0, import_jsx_runtime.jsxs)(Row, { children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)(TextInput, {
						name: "mqtt_username",
						label: "MQTT username",
						hint: "Blank for anonymous login."
					}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)(SecretInput, {
						name: "mqtt_password",
						clearName: "clear_mqtt_password",
						label: "MQTT password",
						isSet: settings.mqtt_password_set
					})] })]
				}),
				/* @__PURE__ */ (0, import_jsx_runtime.jsxs)(Card, {
					title: "Home Assistant API",
					children: [
						addon && /* @__PURE__ */ (0, import_jsx_runtime.jsx)(Alert, {
							level: "info",
							children: "You are running NSPanel Manager as a Home Assistant add-on. These settings are managed by Home Assistant and are locked."
						}),
						/* @__PURE__ */ (0, import_jsx_runtime.jsx)(TextInput, {
							name: "home_assistant_address",
							label: "Home Assistant address",
							hint: "For example http://homeassistant.local:8123",
							disabled: addon
						}),
						/* @__PURE__ */ (0, import_jsx_runtime.jsx)(SecretInput, {
							name: "home_assistant_token",
							clearName: "clear_home_assistant_token",
							label: "Home Assistant token",
							isSet: settings.home_assistant_token_set,
							disabled: addon
						}),
						!addon && /* @__PURE__ */ (0, import_jsx_runtime.jsx)(TestButton, {
							label: "Test Home Assistant",
							disabledReason: typedHomeAssistantToken ? void 0 : "Enter a token to test the connection. For security, the saved token is never sent back to the browser, so it can't be used here.",
							run: () => testHomeAssistant(getValues("home_assistant_address") ?? "", typedHomeAssistantToken)
						})
					]
				}),
				/* @__PURE__ */ (0, import_jsx_runtime.jsxs)(Card, {
					title: "OpenHAB API",
					children: [
						/* @__PURE__ */ (0, import_jsx_runtime.jsx)(TextInput, {
							name: "openhab_address",
							label: "OpenHAB address"
						}),
						/* @__PURE__ */ (0, import_jsx_runtime.jsx)(SecretInput, {
							name: "openhab_token",
							clearName: "clear_openhab_token",
							label: "OpenHAB token",
							isSet: settings.openhab_token_set
						}),
						/* @__PURE__ */ (0, import_jsx_runtime.jsx)(TestButton, {
							label: "Test OpenHAB",
							disabledReason: typedOpenhabToken ? void 0 : "Enter a token to test the connection. For security, the saved token is never sent back to the browser, so it can't be used here.",
							run: () => testOpenhab(getValues("openhab_address") ?? "", typedOpenhabToken)
						})
					]
				}),
				/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
					className: "flex justify-end gap-2 mt-4 mb-4",
					children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("button", {
						type: "button",
						className: "btn btn-ghost",
						disabled: !isDirty || isSubmitting,
						onClick: () => reset(),
						children: "Discard changes"
					}), /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("button", {
						type: "submit",
						className: "btn btn-success",
						disabled: !isDirty || isSubmitting,
						children: [isSubmitting && /* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", { className: "loading loading-spinner loading-sm" }), "Save"]
					})]
				})
			]
		})
	});
}
function applyFieldErrors(errors, values, setError) {
	const unplaced = [];
	for (const [name, messages] of Object.entries(errors)) {
		const message = Array.isArray(messages) ? messages.join(" ") : String(messages);
		if (name in values) setError(name, {
			type: "server",
			message
		});
		else unplaced.push(name === "non_field_errors" ? message : `${name}: ${message}`);
	}
	return unplaced;
}
//#endregion
export { SettingsPage as default };
