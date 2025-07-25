interface Theme {
  "color-scheme": string
  "--color-base-100": string
  "--color-base-200": string
  "--color-base-300": string
  "--color-base-content": string
  "--color-primary": string
  "--color-primary-content": string
  "--color-secondary": string
  "--color-secondary-content": string
  "--color-accent": string
  "--color-accent-content": string
  "--color-neutral": string
  "--color-neutral-content": string
  "--color-info": string
  "--color-info-content": string
  "--color-success": string
  "--color-success-content": string
  "--color-warning": string
  "--color-warning-content": string
  "--color-error": string
  "--color-error-content": string
  "--radius-selector": string
  "--radius-field": string
  "--radius-box": string
  "--size-selector": string
  "--size-field": string
  "--border": string
  "--depth": string
  "--noise": string
}


interface Themes {
  black: Theme
  autumn: Theme
  bumblebee: Theme
  dim: Theme
  night: Theme
  coffee: Theme
  aqua: Theme
  sunset: Theme
  corporate: Theme
  dark: Theme
  synthwave: Theme
  wireframe: Theme
  silk: Theme
  light: Theme
  cmyk: Theme
  dracula: Theme
  caramellatte: Theme
  cyberpunk: Theme
  nord: Theme
  valentine: Theme
  fantasy: Theme
  lemonade: Theme
  acid: Theme
  lofi: Theme
  luxury: Theme
  forest: Theme
  garden: Theme
  abyss: Theme
  business: Theme
  pastel: Theme
  emerald: Theme
  retro: Theme
  halloween: Theme
  winter: Theme
  cupcake: Theme
  [key: string]: Theme
}

declare const themes: Themes
export default themes