import { type ReactNode } from "react";

export function Card({ title, actions, children }: { title: string; actions?: ReactNode; children: ReactNode }) {
  return (
    <section className="p-4 mt-2 bg-base-100 rounded-box shadow-lg w-full" aria-label={title}>
      <div className="flex items-center justify-between mb-2">
        <h3 className="text-xl">{title}</h3>
        {actions}
      </div>
      {children}
    </section>
  );
}

export function Row({ children }: { children: ReactNode }) {
  return <div className="flex flex-wrap md:flex-nowrap gap-2">{children}</div>;
}

export function Alert({ level, children }: { level: "info" | "warning" | "error"; children: ReactNode }) {
  const className = { info: "alert-info", warning: "alert-warning", error: "alert-error" }[level];
  return (
    <div role={level === "error" ? "alert" : "status"} className={`alert ${className} my-2`}>
      <span>{children}</span>
    </div>
  );
}
