import { useState, type ReactNode } from "react";
import type { TestResult } from "../../api/connectionTests";
import { Notify } from "../NSPanelToastContainer";

/** A button joined to the end of an input: pass it as the input's `button`. */
export function JoinButton({ onClick, children }: { onClick: () => void; children: ReactNode }) {
  return (
    <button type="button" className="btn btn-info join-item" onClick={onClick}>
      {children}
    </button>
  );
}

/** Runs a connection test and reports the result as a toast. With a disabledReason it is disabled, and shows why. */
export function TestButton({ label, run, disabledReason }: { label: string; run: () => Promise<TestResult>; disabledReason?: string }) {
  const [running, setRunning] = useState(false);
  const onClick = async () => {
    setRunning(true);
    const error = await run();
    setRunning(false);
    if (error) {
      Notify({ message: `${label} failed: ${error}`, level: "error", duration: 8000 });
    } else {
      Notify({ message: `${label}: OK.`, level: "success", duration: 5000 });
    }
  };
  return (
    <div className="mt-1">
      <button type="button" className="btn btn-accent btn-sm" onClick={onClick} disabled={running || disabledReason !== undefined}>
        {running && <span className="loading loading-spinner loading-sm" />}
        {label}
      </button>
      {disabledReason && <p className="mt-1 text-sm opacity-60">{disabledReason}</p>}
    </div>
  );
}
