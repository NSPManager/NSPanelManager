// import { defineConfig } from 'vite'
// import react from '@vitejs/plugin-react'

// // https://vite.dev/config/
// export default defineConfig({
//   plugins: [react()],
// })
import { defineConfig } from "vite";
import react from "@vitejs/plugin-react";
import generate_script_tag_html_file from "./build_tools/generate_django_html_tags";

export default defineConfig({
  plugins: [react(), generate_script_tag_html_file()],
  base: "/static/",
  build: {
    sourcemap: false,
    minify: "esbuild",
    outDir: "dist",
    assetsDir: "",
    assetsInlineLimit: 0,
    manifest: true,

    rolldownOptions: {
      treeshake: true,
      input: {
        main: "./src/main.tsx",
      },
      // output: {
      //   manualChunks: {
      //     "react-vendor": ["react", "react-dom", "react/jsx-runtime", "react-dom/client"],
      //   },
      // },
    },
  },
});
