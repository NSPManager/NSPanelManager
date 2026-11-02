// RTTTL parsing, a port of the panel firmware's RtttlParser (NSPanelManager-firmware,
// firmware_espidf/lib/BuzzerManager/RtttlParser.cpp), so the settings page accepts and rejects exactly
// what the panel does and previews the same steps. It follows the C++ step by step, integer arithmetic
// included. web/tests/rtttl_vectors.json holds the firmware's output for a set of inputs; this module and
// the server's web/rest_v2/rtttl.py are both tested against it, and their messages match.

/** [frequency_hz, duration_ms]; 0 Hz is a pause. */
export type ToneStep = readonly [number, number];

export type RtttlResult = { ok: true; steps: ToneStep[]; durationMs: number } | { ok: false; error: string; tooLong: boolean };

export const MAX_STEPS = 256;
export const MAX_TOTAL_DURATION_MS = 30000;

const ARTICULATION_GAP_MS = 10;

// Equal temperament frequencies (A4 = 440 Hz) of C8 through B8, in fixed point with 4 fractional bits.
const OCTAVE8_FRACTION_BITS = 4;
const OCTAVE8_FREQUENCIES = [66976, 70959, 75178, 79649, 84385, 89402, 94719, 100351, 106318, 112640, 119338, 126434];

const SEMITONES: Record<string, number> = { c: 0, d: 2, e: 4, f: 5, g: 7, a: 9, b: 11, h: 11, p: -1 };
const VALID_DURATIONS = [1, 2, 4, 8, 16, 32, 64];

// The firmware uses C's isspace/tolower/isdigit, which only know ASCII.
const WHITESPACE = " \t\n\v\f\r";
const isDigit = (c: string | undefined) => c !== undefined && c >= "0" && c <= "9";

class RtttlError extends Error {
  readonly tooLong: boolean;
  constructor(message: string, tooLong = false) {
    super(message);
    this.tooLong = tooLong;
  }
}

/** Parse an RTTTL string, ie. "tick:d=64,o=7,b=625:b", the way the panel does. */
export function parseRtttl(rtttl: string): RtttlResult {
  try {
    const steps = parse(rtttl);
    return { ok: true, steps, durationMs: steps.reduce((total, [, ms]) => total + ms, 0) };
  } catch (error) {
    if (error instanceof RtttlError) {
      return { ok: false, error: error.message, tooLong: error.tooLong };
    }
    throw error;
  }
}

function parse(rtttl: string): ToneStep[] {
  let text = "";
  for (const c of rtttl) {
    if (!WHITESPACE.includes(c)) {
      text += c >= "A" && c <= "Z" ? c.toLowerCase() : c;
    }
  }

  const sections = text.split(":");
  if (sections.length !== 3) {
    throw new RtttlError("Expected name:defaults:notes, for example tick:d=64,o=7,b=625:b.");
  }
  const [, defaults, notes] = sections;
  const [duration, octave, bpm] = parseDefaults(defaults);

  const steps: [number, number][] = [];
  let totalDurationMs = 0;
  const noteTexts = notes.split(",");
  for (let index = 0; index < noteTexts.length; index++) {
    const note = noteTexts[index];
    if (note === "" && index === noteTexts.length - 1 && steps.length > 0) {
      break; // Trailing comma
    }

    const step = parseNote(note, duration, octave, bpm, index + 1);

    // Two notes of the same pitch back to back would sound like one long note, so the firmware cuts
    // a short gap from the end of the first.
    const last = steps[steps.length - 1];
    if (last && step[0] > 0 && last[0] === step[0] && last[1] > ARTICULATION_GAP_MS * 2) {
      last[1] -= ARTICULATION_GAP_MS;
      steps.push([0, ARTICULATION_GAP_MS]);
    }

    steps.push(step);
    totalDurationMs += step[1];
    if (steps.length > MAX_STEPS) {
      throw new RtttlError(`Too many notes: the panel plays at most ${MAX_STEPS} steps.`, true);
    }
    if (totalDurationMs > MAX_TOTAL_DURATION_MS) {
      throw new RtttlError(`Too long: the panel plays at most ${MAX_TOTAL_DURATION_MS / 1000} seconds.`, true);
    }
  }
  return steps;
}

function parseDefaults(section: string): [number, number, number] {
  let [duration, octave, bpm] = [4, 6, 63];
  const malformed = () => new RtttlError("Defaults must look like d=4,o=6,b=63.");
  let pos = 0;
  while (pos < section.length) {
    if (pos + 2 > section.length || section[pos + 1] !== "=") {
      throw malformed();
    }
    const key = section[pos];
    pos += 2;

    const [next, value] = parseNumber(section, pos, 999);
    if (value === null) {
      throw malformed();
    }
    pos = next;

    if (key === "d" && VALID_DURATIONS.includes(value)) {
      duration = value;
    } else if (key === "o" && value >= 1 && value <= 8) {
      octave = value;
    } else if (key === "b" && value >= 1 && value <= 900) {
      bpm = value;
    } else if (key === "d") {
      throw new RtttlError("d must be 1, 2, 4, 8, 16, 32 or 64.");
    } else if (key === "o") {
      throw new RtttlError("o must be 1 to 8.");
    } else if (key === "b") {
      throw new RtttlError("b must be 1 to 900.");
    } else {
      throw new RtttlError(`Unknown default '${key}': only d, o and b are allowed.`);
    }

    if (pos < section.length) {
      if (section[pos] !== ",") {
        throw malformed();
      }
      pos++;
    }
  }
  return [duration, octave, bpm];
}

function parseNote(note: string, defaultDuration: number, defaultOctave: number, bpm: number, position: number): [number, number] {
  const invalid = () => new RtttlError(`Note ${position} ('${note}') is not valid.`);

  let pos = 0;
  let duration = defaultDuration;
  if (isDigit(note[pos])) {
    const [next, value] = parseNumber(note, pos, 64);
    if (value === null || !VALID_DURATIONS.includes(value)) {
      throw invalid();
    }
    [pos, duration] = [next, value];
  }

  if (pos >= note.length || !Object.hasOwn(SEMITONES, note[pos])) {
    throw invalid();
  }
  let semitone = SEMITONES[note[pos]];
  pos++;

  if (note[pos] === "#") {
    if (semitone < 0) {
      throw invalid();
    }
    semitone++;
    pos++;
  }

  // The dot goes either before or after the octave, depending on who wrote the string.
  let dotted = false;
  if (note[pos] === ".") {
    dotted = true;
    pos++;
  }

  let octave = defaultOctave;
  if (isDigit(note[pos])) {
    octave = Number(note[pos]);
    if (octave < 1 || octave > 8) {
      throw invalid();
    }
    pos++;
  }

  if (!dotted && note[pos] === ".") {
    dotted = true;
    pos++;
  }

  if (pos !== note.length) {
    throw invalid();
  }

  // A whole note lasts 4 beats. Dotted notes last 1.5 times as long. Round to the nearest ms.
  const numerator = 240000 * (dotted ? 3 : 2);
  const denominator = bpm * duration * 2;
  const durationMs = Math.floor((numerator + Math.floor(denominator / 2)) / denominator);

  if (semitone < 0) {
    return [0, durationMs];
  }
  if (semitone === 12) {
    // B# is C in the next octave
    semitone = 0;
    octave++;
  }
  const shift = OCTAVE8_FRACTION_BITS + 8 - octave;
  return [(OCTAVE8_FREQUENCIES[semitone] + (1 << (shift - 1))) >> shift, durationMs];
}

/** Read the digits at pos: [new pos, value], value null if there were none or it exceeds maxValue. */
function parseNumber(text: string, pos: number, maxValue: number): [number, number | null] {
  const start = pos;
  let value = 0;
  while (isDigit(text[pos])) {
    value = value * 10 + Number(text[pos]);
    if (value > maxValue) {
      return [pos, null];
    }
    pos++;
  }
  return pos === start ? [pos, null] : [pos, value];
}
