// import { StrictMode } from 'react'
// import { createRoot } from 'react-dom/client'
// import './index.css'
// import App from './App.tsx'

// createRoot(document.getElementById('root')!).render(
//   <StrictMode>
//     <App />
//   </StrictMode>,
// )

import { StrictMode } from "react";
import { createRoot } from "react-dom/client";
import componentMap, { type ComponentKey } from "./componentMap";
import "./index.css";

if (import.meta.env.VITE_MODE === "development") {
  import("./App").then(({ default: App }) => {
    const rootElement = document.getElementById("root");
    if (rootElement) {
      createRoot(rootElement).render(
        <StrictMode>
          <App />
        </StrictMode>,
      );
    } else {
      console.error("Root element not found");
    }
  });
} else {
  // Find all divs with the `data-react-component` attribute
  const elements = document.querySelectorAll("[data-react-component]");
  for (let i = 0; i < elements.length; i++) {
    const el = elements[i];
    const componentName = el.getAttribute("data-react-component") as ComponentKey;
    const Component = componentMap[componentName];

    if (Component) {
      // Parse props from the `data-react-props` attribute
      const props = JSON.parse(el.getAttribute("data-react-props") || "{}");

      // Render the React component inside the div
      const root = createRoot(el);
      root.render(<Component {...props} />);
    } else {
      console.error(`Component ${componentName} not found`);
    }
  }
}
