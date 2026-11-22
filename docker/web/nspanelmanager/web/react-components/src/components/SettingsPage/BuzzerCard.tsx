import { useState } from "react";
import { get, useFormContext, useWatch } from "react-hook-form";
import { parseRtttl, type RtttlResult, type ToneStep } from "../../lib/rtttl";
import { isAudioSupported, useRtttlPlayer } from "../../lib/rtttlPlayer";
import { BLANK, BUZZER_EVENTS, choiceFor, CUSTOM, type BuzzerEvent } from "./buzzer";
import { Card } from "../common/layout";
import type { SettingsFormValues } from "./form";

type Player = ReturnType<typeof useRtttlPlayer>;

/** The buzzer sound for each panel event, with presets and a preview played in the browser. */
export function BuzzerCard() {
  const player = useRtttlPlayer();
  const [piezo, setPiezo] = useState(true);
  const { control } = useFormContext<SettingsFormValues>();
  const touch = useWatch({ control, name: "buzzer_sound_touch" }) ?? "";

  return (
    <Card title="Buzzer sounds">
      <p className="mb-2 text-sm opacity-80">
        Sonoff NSPanels only. Panels make no sound unless you choose one here. Each panel also has a buzzer mute switch in Home Assistant.
      </p>
      {BUZZER_EVENTS.map((event) => (
        <BuzzerSoundField key={event.name} event={event} player={player} piezo={piezo} touch={touch} />
      ))}
      {isAudioSupported() ? (
        <label className="label text-sm">
          <input type="checkbox" className="checkbox checkbox-accent checkbox-sm" checked={piezo} onChange={(event) => setPiezo(event.target.checked)} />
          Make the preview sound like the panel's buzzer (approximate)
        </label>
      ) : (
        <p className="text-sm opacity-60">This browser can't play the preview.</p>
      )}
    </Card>
  );
}

const parse = (rtttl: string): RtttlResult | null => (rtttl === "" ? null : parseRtttl(rtttl));

function BuzzerSoundField({ event, player, piezo, touch }: { event: BuzzerEvent; player: Player; piezo: boolean; touch: string }) {
  const { name, label } = event;
  const {
    register,
    setValue,
    setFocus,
    control,
    formState: { errors },
  } = useFormContext<SettingsFormValues>();
  const value = useWatch({ control, name }) ?? "";
  // "Custom" stays chosen while the text still matches a preset, so a preset can be edited.
  const [custom, setCustom] = useState(false);
  const choice = custom ? CUSTOM : choiceFor(event, value);

  const parsed = parse(value);
  const error = parsed && !parsed.ok ? parsed.error : (get(errors, name)?.message as string | undefined);

  // A blank "scene activated" plays the touch sound, so that's what its preview plays.
  const fallsBackToTouch = value === "" && name === "buzzer_sound_scene_activated";
  const preview = fallsBackToTouch ? parse(touch) : parsed;
  const steps = preview?.ok ? preview.steps : null;

  const choose = (selected: string) => {
    setCustom(selected === CUSTOM);
    if (selected === CUSTOM) {
      setFocus(name);
      return;
    }
    const rtttl = selected === BLANK ? "" : (event.presets.find((preset) => preset.id === selected)?.rtttl ?? "");
    setValue(name, rtttl, { shouldDirty: true, shouldValidate: true });
    if (player.playing === name) {
      player.stop();
    }
  };

  const playing = player.playing === name;
  const previewLabel = playing ? `Stop the ${label.toLowerCase()} preview` : `Preview the ${label.toLowerCase()} sound`;

  return (
    <div className="mb-3 w-full">
      <label htmlFor={`${name}-choice`} className="block mb-1 text-sm font-medium">
        {label}
      </label>
      <div className="flex flex-wrap md:flex-nowrap gap-2">
        <select id={`${name}-choice`} className="select md:w-56 shrink-0" value={choice} onChange={(change) => choose(change.target.value)}>
          <option value={BLANK}>{event.blankLabel}</option>
          {event.presets.map((preset) => (
            <option key={preset.id} value={preset.id}>
              {preset.name}
            </option>
          ))}
          <option value={CUSTOM}>Custom…</option>
        </select>
        <div className="join w-full">
          <input
            id={name}
            type="text"
            aria-label={`${label} RTTTL`}
            className="input w-full join-item font-mono"
            maxLength={255}
            spellCheck={false}
            autoComplete="off"
            placeholder={choice === CUSTOM ? "name:d=4,o=6,b=63:c,e,g" : ""}
            aria-invalid={error !== undefined}
            aria-describedby={`${name}-message`}
            {...register(name, {
              setValueAs: (raw) => raw ?? "",
              validate: (raw) => {
                const result = parse(raw ?? "");
                return result === null || result.ok || result.error;
              },
            })}
          />
          <button
            type="button"
            className="btn btn-accent join-item"
            aria-label={previewLabel}
            title={previewLabel}
            disabled={!steps || !isAudioSupported()}
            onClick={() => (playing ? player.stop() : steps && player.play(name, steps, { piezo }))}
          >
            <span className={`mdi ${playing ? "mdi-stop" : "mdi-play"}`} />
          </button>
        </div>
      </div>
      <p id={`${name}-message`} className={`mt-1 text-sm ${error ? "text-error" : "opacity-60"}`}>
        {error ?? `${event.hint}${parsed?.ok ? ` ${describe(parsed.steps, parsed.durationMs)}` : ""}`}
      </p>
    </div>
  );
}

function describe(steps: readonly ToneStep[], durationMs: number): string {
  return steps.every(([frequency]) => frequency === 0) ? "Silent." : `Plays for ${formatDuration(durationMs)}.`;
}

function formatDuration(ms: number): string {
  return ms < 1000 ? `${ms} ms` : `${(ms / 1000).toFixed(ms < 10000 ? 2 : 1)} s`;
}
