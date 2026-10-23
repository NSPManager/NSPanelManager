// Adds DOM matchers such as toBeInTheDocument() and toBeDisabled() to expect().
import "@testing-library/jest-dom/vitest";
import { cleanup } from "@testing-library/react";
import { afterEach } from "vitest";

afterEach(() => {
  cleanup();
});

// jsdom has no layout, so it doesn't implement ResizeObserver. Components only use it to adjust
// overflow, which tests don't depend on.
if (!("ResizeObserver" in globalThis)) {
  globalThis.ResizeObserver = class {
    observe() {}
    unobserve() {}
    disconnect() {}
  };
}
