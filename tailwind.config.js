/** @type {import('tailwindcss').Config} */
export default {
  darkMode: 'class',
  content: [
    "./index.html",
    "./src/**/*.{js,ts,jsx,tsx}",
  ],
  theme: {
    extend: {
      colors: {
        primary: {
          DEFAULT: '#1e1b4b',
          navy: '#1e1b4b',
        },
        accent: {
          DEFAULT: '#ff6b6b',
          light: '#ff8787',
          coral: '#ff6b6b',
        },
        success: {
          DEFAULT: '#4ade80',
          green: '#4ade80',
        },
        warning: {
          DEFAULT: '#fbbf24',
          yellow: '#fbbf24',
        },
        info: {
          DEFAULT: '#60a5fa',
          blue: '#60a5fa',
        },
        bg: {
          DEFAULT: '#f5f6fa',
        },
      },
    },
  },
  plugins: [],
}
