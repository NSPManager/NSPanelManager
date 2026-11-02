// Plays parsed RTTTL steps in the browser with Web Audio, as a preview of the panel's buzzer: a square
// wave, like the LEDC output that drives the panel's piezo. The pitches and timing are exactly the
// panel's; the sound is only an approximation, because a piezo isn't a speaker.

import { useCallback, useEffect, useRef, useState } from "react";
import type { ToneStep } from "./rtttl";

// Full-scale square waves are harsh, and the preview is usually played with the volume set for speech.
const VOLUME = 0.15;

// Ramps on every note edge, so the speaker doesn't click. The piezo clicks anyway, the browser needn't.
const RAMP_S = 0.001;

// The piezo is loudest around its resonance and much quieter away from it. A band-pass there (as in
// buzzer_sounds/gen_beeps.py's piezo()) gets closer to how the panel sounds than the raw square wave.
const PIEZO_RESONANCE_HZ = 4000;
const PIEZO_Q = 2.5;
const PIEZO_MAKEUP_GAIN = 2;

// Starting slightly in the future keeps the first note's attack from being cut off.
const START_DELAY_S = 0.02;

export type Playback = { stop: () => void; done: Promise<void> };

let sharedContext: AudioContext | null = null;

export function isAudioSupported(): boolean {
  return typeof window !== "undefined" && typeof window.AudioContext === "function";
}

// Browsers only let audio start from a user gesture, so the context is created on the first preview click.
function audioContext(): AudioContext {
  sharedContext ??= new AudioContext();
  if (sharedContext.state === "suspended") {
    void sharedContext.resume();
  }
  return sharedContext;
}

/** Play the steps; done resolves when they finish or stop() is called. */
export function playSteps(steps: readonly ToneStep[], { piezo }: { piezo: boolean }): Playback {
  const context = audioContext();
  const oscillator = context.createOscillator();
  oscillator.type = "square";
  const gain = context.createGain();
  gain.gain.value = 0;

  const nodes: AudioNode[] = [oscillator, gain];
  if (piezo) {
    const filter = context.createBiquadFilter();
    filter.type = "bandpass";
    filter.frequency.value = PIEZO_RESONANCE_HZ;
    filter.Q.value = PIEZO_Q;
    oscillator.connect(filter).connect(gain);
    nodes.push(filter);
  } else {
    oscillator.connect(gain);
  }
  gain.connect(context.destination);

  const volume = VOLUME * (piezo ? PIEZO_MAKEUP_GAIN : 1);
  const start = context.currentTime + START_DELAY_S;
  let time = start;
  for (const [frequency, ms] of steps) {
    const seconds = ms / 1000;
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

  const done = new Promise<void>((resolve) => {
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
      } catch {
        // Already stopped
      }
    },
    done,
  };
}

/**
 * One preview at a time for a page: starting one stops the one playing. playing is the key of the
 * preview playing, so its button can show a stop symbol.
 */
export function useRtttlPlayer() {
  const [playing, setPlaying] = useState<string | null>(null);
  const current = useRef<{ key: string; playback: Playback } | null>(null);

  const stop = useCallback(() => {
    current.current?.playback.stop();
    current.current = null;
    setPlaying(null);
  }, []);

  const play = useCallback(
    (key: string, steps: readonly ToneStep[], options: { piezo: boolean }) => {
      stop();
      const playback = playSteps(steps, options);
      const entry = { key, playback };
      current.current = entry;
      setPlaying(key);
      void playback.done.then(() => {
        if (current.current === entry) {
          current.current = null;
          setPlaying(null);
        }
      });
    },
    [stop],
  );

  // Don't keep playing after the page is left.
  useEffect(() => () => current.current?.playback.stop(), []);

  return { playing, play, stop };
}
