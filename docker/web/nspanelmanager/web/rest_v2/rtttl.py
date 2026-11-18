"""
RTTTL parsing, a port of the panel firmware's RtttlParser (NSPanelManager-firmware,
firmware_espidf/lib/BuzzerManager/RtttlParser.cpp). The buzzer_sound_* settings are sent to the panels as
they are, and a panel plays nothing for a string its parser rejects, logging the error only in the panel's
own log. Parsing here lets the API reject such a string with a field error instead.

It has to accept and reject exactly what the firmware does, and produce the same steps, so it follows the
C++ step by step, integer arithmetic included. web/tests/rtttl_vectors.json holds the firmware's output for
a set of inputs; both this module and the UI's src/lib/rtttl.ts are tested against it.
"""

MAX_STEPS = 256
MAX_TOTAL_DURATION_MS = 30000

_ARTICULATION_GAP_MS = 10

# Equal temperament frequencies (A4 = 440 Hz) of C8 through B8, in fixed point with 4 fractional bits.
_OCTAVE8_FRACTION_BITS = 4
_OCTAVE8_FREQUENCIES = (66976, 70959, 75178, 79649, 84385, 89402, 94719, 100351, 106318, 112640, 119338, 126434)

_SEMITONES = {"c": 0, "d": 2, "e": 4, "f": 5, "g": 7, "a": 9, "b": 11, "h": 11, "p": -1}
_VALID_DURATIONS = (1, 2, 4, 8, 16, 32, 64)

# The firmware uses C's isspace/tolower/isdigit, which only know ASCII. str.isspace(), str.lower() and
# str.isdigit() know Unicode, so they aren't used.
_WHITESPACE = " \t\n\v\f\r"
_DIGITS = "0123456789"


class RtttlError(ValueError):
    """The string can't be played. str() is a message for the user."""


class RtttlTooLong(RtttlError):
    """Valid RTTTL, but longer than MAX_STEPS or MAX_TOTAL_DURATION_MS."""


def parse(rtttl):
    """Parse an RTTTL string into a list of (frequency_hz, duration_ms), 0 Hz being a pause.

    Raises RtttlError (or its subclass RtttlTooLong) for any string the firmware would reject.
    """
    text = "".join(c.lower() if "A" <= c <= "Z" else c for c in rtttl if c not in _WHITESPACE)

    sections = text.split(":")
    if len(sections) != 3:
        raise RtttlError("Expected name:defaults:notes, for example tick:d=64,o=7,b=625:b.")
    _, defaults, notes = sections

    duration, octave, bpm = _parse_defaults(defaults)

    steps = []
    total_duration_ms = 0
    position = 1
    note_texts = notes.split(",")
    for index, note in enumerate(note_texts):
        if note == "" and index == len(note_texts) - 1 and steps:
            break  # Trailing comma

        step = _parse_note(note, duration, octave, bpm, position)

        # Two notes of the same pitch back to back would sound like one long note, so the firmware cuts
        # a short gap from the end of the first.
        if steps and step[0] > 0 and steps[-1][0] == step[0] and steps[-1][1] > _ARTICULATION_GAP_MS * 2:
            steps[-1] = (steps[-1][0], steps[-1][1] - _ARTICULATION_GAP_MS)
            steps.append((0, _ARTICULATION_GAP_MS))

        steps.append(step)
        total_duration_ms += step[1]
        if len(steps) > MAX_STEPS:
            raise RtttlTooLong(f"Too many notes: the panel plays at most {MAX_STEPS} steps.")
        if total_duration_ms > MAX_TOTAL_DURATION_MS:
            raise RtttlTooLong(f"Too long: the panel plays at most {MAX_TOTAL_DURATION_MS // 1000} seconds.")
        position += 1

    return steps


def _parse_defaults(section):
    duration, octave, bpm = 4, 6, 63
    pos = 0
    while pos < len(section):
        if pos + 2 > len(section) or section[pos + 1] != "=":
            raise RtttlError("Defaults must look like d=4,o=6,b=63.")
        key = section[pos]
        pos += 2

        pos, value = _parse_number(section, pos, 999)
        if value is None:
            raise RtttlError("Defaults must look like d=4,o=6,b=63.")

        if key == "d" and value in _VALID_DURATIONS:
            duration = value
        elif key == "o" and 1 <= value <= 8:
            octave = value
        elif key == "b" and 1 <= value <= 900:
            bpm = value
        elif key in "dob":
            raise RtttlError({"d": "d must be 1, 2, 4, 8, 16, 32 or 64.", "o": "o must be 1 to 8.", "b": "b must be 1 to 900."}[key])
        else:
            raise RtttlError(f"Unknown default '{key}': only d, o and b are allowed.")

        if pos < len(section):
            if section[pos] != ",":
                raise RtttlError("Defaults must look like d=4,o=6,b=63.")
            pos += 1
    return duration, octave, bpm


def _parse_note(note, default_duration, default_octave, bpm, position):
    def invalid():
        return RtttlError(f"Note {position} ('{note}') is not valid.")

    pos = 0
    duration = default_duration
    if pos < len(note) and note[pos] in _DIGITS:
        pos, duration = _parse_number(note, pos, 64)
        if duration not in _VALID_DURATIONS:
            raise invalid()

    if pos >= len(note) or note[pos] not in _SEMITONES:
        raise invalid()
    semitone = _SEMITONES[note[pos]]
    pos += 1

    if pos < len(note) and note[pos] == "#":
        if semitone < 0:
            raise invalid()
        semitone += 1
        pos += 1

    # The dot goes either before or after the octave, depending on who wrote the string.
    dotted = False
    if pos < len(note) and note[pos] == ".":
        dotted = True
        pos += 1

    octave = default_octave
    if pos < len(note) and note[pos] in _DIGITS:
        octave = int(note[pos])
        if not 1 <= octave <= 8:
            raise invalid()
        pos += 1

    if not dotted and pos < len(note) and note[pos] == ".":
        dotted = True
        pos += 1

    if pos != len(note):
        raise invalid()

    # A whole note lasts 4 beats. Dotted notes last 1.5 times as long. Round to the nearest ms.
    numerator = 240000 * (3 if dotted else 2)
    denominator = bpm * duration * 2
    duration_ms = (numerator + denominator // 2) // denominator

    if semitone < 0:
        return (0, duration_ms)

    if semitone == 12:  # B# is C in the next octave
        semitone = 0
        octave += 1
    shift = _OCTAVE8_FRACTION_BITS + 8 - octave
    return ((_OCTAVE8_FREQUENCIES[semitone] + (1 << (shift - 1))) >> shift, duration_ms)


def _parse_number(text, pos, max_value):
    """Read the digits at pos. Returns (new pos, value), value None if there were none or it exceeds max_value."""
    start = pos
    value = 0
    while pos < len(text) and text[pos] in _DIGITS:
        value = value * 10 + int(text[pos])
        if value > max_value:
            return pos, None
        pos += 1
    if pos == start:
        return pos, None
    return pos, value
