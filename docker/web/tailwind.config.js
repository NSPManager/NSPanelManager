/** @type {import('tailwindcss').Config} */
module.exports = {
  content: ["./nspanelmanager/web/static/**/*.{html,js}", "./nspanelmanager/web/templates/**/*.{html,js}"],
  theme: {
    extend: {},
  },
  darkMode: 'selector',
  plugins: [require('@tailwindcss/forms')]
}

