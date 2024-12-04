/** @type {import('tailwindcss').Config} */
module.exports = {
  content: ["./data/**/*.{html,js}"],
  theme: {
    extend: {},
  },
  darkMode: 'selector',
  plugins: [require('@tailwindcss/forms')],
}

