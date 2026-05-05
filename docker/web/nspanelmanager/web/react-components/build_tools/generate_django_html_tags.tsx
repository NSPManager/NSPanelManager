import fs from "fs/promises";
import path from "path";
import { type Plugin } from "vite";

export default function generate_script_tag_html_file(): Plugin {
  return {
    name: "build-django-html-tags",
    async closeBundle() {
      try {
        // Path to the manifest file
        const manifestPath = path.resolve("dist/.vite/manifest.json");

        // Read and parse the manifest.json file
        const manifest = JSON.parse(await fs.readFile(manifestPath, "utf-8"));

        // Get the "src/main.tsx" entry point
        const entry = manifest["src/main.tsx"];

        if (!entry) {
          throw new Error('Entry point "src/main.tsx" not found in manifest.json.');
        }

        // Extract the main JS and CSS files
        const mainJsFile = entry.file;
        // const cssFiles = entry.css || [];

        // Generate the HTML content
        let htmlContent = "";

        htmlContent += `{% load static %}\n`;

        // Add CSS links
        // for (const cssFile of cssFiles) {
        //   htmlContent += `<link rel="stylesheet" href="{% static '${cssFile}' %}">\n`;
        // }

        // Add the JS script tag
        htmlContent += `<script type="module" src="{% static '${mainJsFile}' %}"></script>\n`;

        // Write the generated HTML to templates/scripts.html
        const outputDir = path.resolve("templates");
        await fs.mkdir(outputDir, { recursive: true });
        const outputFilePath = path.join(outputDir, "scripts.html");
        await fs.writeFile(outputFilePath, htmlContent, "utf-8");

        console.log("Generated templates/scripts.html successfully.");
      } catch (error) {
        console.error("Error generating templates/scripts.html:", error);
      }
    },
  };
}
