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
        // Grayscale palette (main colors)
        gray: {
          50: '#fafafa',
          100: '#f5f5f5',
          200: '#e5e5e5',
          300: '#d4d4d4',
          400: '#a3a3a3',
          500: '#737373',
          600: '#525252',
          700: '#404040',
          800: '#262626',
          900: '#171717',
        },
        // Single accent hue (coral/salmon)
        accent: {
          DEFAULT: '#ff6b6b',
          50: '#fff5f5',
          100: '#ffe3e3',
          200: '#ffc9c9',
          300: '#ffa8a8',
          400: '#ff8787',
          500: '#ff6b6b',
          600: '#fa5252',
          700: '#f03e3e',
          800: '#e03131',
          900: '#c92a2a',
        },
        // Subtle success/warning for indicators only
        success: '#10b981',
        warning: '#f59e0b',
        // Background
        bg: {
          DEFAULT: '#fafafa',
        },
      },
      boxShadow: {
        // Light from above shadows (Apple-style)
        'soft': '0 1px 3px 0 rgba(0, 0, 0, 0.08), 0 1px 2px 0 rgba(0, 0, 0, 0.04)',
        'soft-lg': '0 4px 6px -1px rgba(0, 0, 0, 0.08), 0 2px 4px -1px rgba(0, 0, 0, 0.04)',
        'soft-xl': '0 10px 15px -3px rgba(0, 0, 0, 0.08), 0 4px 6px -2px rgba(0, 0, 0, 0.04)',
        'soft-2xl': '0 20px 25px -5px rgba(0, 0, 0, 0.08), 0 10px 10px -5px rgba(0, 0, 0, 0.02)',
        'inner-light': 'inset 0 2px 4px 0 rgba(0, 0, 0, 0.06)',
        'btn': '0 1px 2px 0 rgba(0, 0, 0, 0.08)',
        'btn-hover': '0 2px 4px 0 rgba(0, 0, 0, 0.12)',
        'card': '0 2px 8px 0 rgba(0, 0, 0, 0.06), 0 1px 2px 0 rgba(0, 0, 0, 0.04)',
      },
    },
  },
  plugins: [],
}
