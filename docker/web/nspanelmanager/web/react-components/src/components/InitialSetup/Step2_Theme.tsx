import { useState } from "react";
import { type formDataType } from "./InitialSetup";

const Step2_Theme = ({
  setFormData,
  formData,
  nextStep,
  prevStep,
}: {
  setFormData: (data: formDataType) => void;
  formData: formDataType;
  nextStep: () => void;
  prevStep: () => void;
}) => {
  const [selectedTheme, setSelectedTheme] = useState("default");
  const themes = {
    default: "Default",
    light: "Light",
    dark: "Dark",
    nspmwip: "NSPanel Manager",
    cupcake: "Cupcake",
    bumblebee: "Bumblebee",
    emerald: "Emerald",
    corporate: "Corporate",
    synthwave: "Synthwave",
    retro: "Retro",
    cyberpunk: "Cyberpunk",
    valentine: "Valentine",
    halloween: "Halloween",
    garden: "Garden",
    forest: "Forest",
    aqua: "Aqua",
    lofi: "Lo-Fi",
    pastel: "Pastel",
    fantasy: "Fantasy",
    wireframe: "Wireframe",
    black: "Black",
    luxury: "Luxury",
    dracula: "Dracula",
    cmyk: "CMYK",
    autumn: "Autumn",
    business: "Business",
    acid: "Acid",
    lemonade: "Lemonade",
    night: "Night",
    coffee: "Coffee",
    winter: "Winter",
    dim: "Dim",
    nord: "Nord",
    sunset: "Sunset",
  };

  return (
    <div>
      <div className="grid gap-4 grid-cols-3 mt-4 justify-end items-center">
        {Object.entries(themes).map(([theme_option, display_name]) => (
          <div
            className="bg-neutral text-neutral-content grid grid-cols-3 w-full h-20 rounded-box overflow-hidden cursor-pointer"
            data-theme={theme_option}
            onClick={() => {
              setSelectedTheme(theme_option);
              setFormData({ ...formData, theme: theme_option });
            }}
          >
            <div className="">
              <div className="bg-base-100 min-h-1/3"></div>
              <div className="bg-base-200 min-h-1/3"></div>
              <div className="bg-base-300 min-h-1/3"></div>
            </div>
            <div className="col-span-2 relative">
              <label className="label">
                <input
                  type="radio"
                  name="theme-dropdown"
                  className="theme-controller radio radio-xs radio-accent p-0 m-0 absolute top-1 right-1"
                  value={theme_option}
                  checked={theme_option === selectedTheme}
                />
                <span className="label-text justify-start w-full ml-2 text-wrap">{display_name}</span>
              </label>
              <div className="flex absolute bottom-2">
                <div className="badge badge-primary badge-xs ms-2">A</div>
                <div className="badge badge-accent badge-xs ms-1">B</div>
                <div className="badge badge-success badge-xs ms-1">C</div>
                <div className="badge badge-info badge-xs ms-1">D</div>
                <div className="badge badge-error badge-xs ms-1">E</div>
              </div>
            </div>
          </div>
        ))}
      </div>
      <div className="flex gap-2 justify-end mt-8">
        <button type="submit" className="btn btn-neutral" onClick={prevStep}>
          Back
        </button>
        <button type="submit" className="btn btn-primary" onClick={nextStep}>
          Next
        </button>
      </div>
    </div>
  );
};

export default Step2_Theme;
