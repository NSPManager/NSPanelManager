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
  dim: Theme
  bumblebee: Theme
  aqua: Theme
  night: Theme
  coffee: Theme
  corporate: Theme
  sunset: Theme
  dark: Theme
  wireframe: Theme
  synthwave: Theme
  silk: Theme
  garden: Theme
  light: Theme
  acid: Theme
  lemonade: Theme
  forest: Theme
  nord: Theme
  valentine: Theme
  cyberpunk: Theme
  dracula: Theme
  cmyk: Theme
  caramellatte: Theme
  abyss: Theme
  luxury: Theme
  lofi: Theme
  fantasy: Theme
  cupcake: Theme
  halloween: Theme
  retro: Theme
  business: Theme
  emerald: Theme
  winter: Theme
  pastel: Theme
  [key: string]: Theme
}

declare const themes: Themes
export default themes