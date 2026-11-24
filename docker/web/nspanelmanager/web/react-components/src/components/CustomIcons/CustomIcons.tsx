import { useEffect, useMemo, useRef, useState } from "react";
import { isCustomIcon, useCustomIconsStore } from "../../stores/CustomIconsStore";
import { Notify } from "../NSPanelToastContainer";

// The most icons the picker shows at once; searching narrows the list down.
const MAX_SHOWN = 120;

/**
 * A custom icon character, as stored for an entity, scene or thermostat mode, drawn as its
 * Material Design Icon. Nothing for anything else.
 */
export const CustomIcon = ({ character, className = "" }: { character: string | undefined | null; className?: string }) => {
  const icons = useCustomIconsStore((state) => state.icons);
  const fetchIcons = useCustomIconsStore((state) => state.fetchIcons);
  const icon = isCustomIcon(character) ? icons.find((icon) => icon.character === character) : undefined;

  useEffect(() => {
    if (isCustomIcon(character) && icon === undefined) {
      fetchIcons();
    }
  }, [character, icon, fetchIcons]);

  if (icon === undefined) return null;
  return <span className={`mdi mdi-${icon.name} ${className}`} title={icon.name}></span>;
};

// An icon name with line break opportunities after its hyphens, so long names wrap between words.
const IconName = ({ name }: { name: string }) => (
  <>
    {name.split("-").map((part, index, parts) => (
      <span key={index}>
        {part}
        {index < parts.length - 1 && (
          <>
            -<wbr />
          </>
        )}
      </span>
    ))}
  </>
);

/**
 * A dialog to search all Material Design Icons and choose one, laid out like pictogrammers.com: each
 * icon over its name. The chosen icon is registered as a custom icon, and onSelect gets the
 * character to store. selected: the character currently chosen, highlighted in the list.
 */
export const CustomIconDialog = ({
  open,
  onClose,
  onSelect,
  selected,
}: {
  open: boolean;
  onClose: () => void;
  onSelect: (character: string) => void;
  selected?: string;
}) => {
  const dialogRef = useRef<HTMLDialogElement>(null);
  const { available, fetchAvailable, register, icons } = useCustomIconsStore();
  const [search, setSearch] = useState("");
  const selectedName = isCustomIcon(selected) ? icons.find((icon) => icon.character === selected)?.name : undefined;

  useEffect(() => {
    const dialog = dialogRef.current;
    if (open) {
      fetchAvailable();
      dialog?.showModal();
    } else if (dialog?.open) {
      dialog.close();
    }
  }, [open, fetchAvailable]);

  const matches = useMemo(() => {
    const words = search.toLowerCase().split(/[\s-]+/).filter((word) => word.length > 0);
    return (available ?? []).filter((name) => words.every((word) => name.includes(word)));
  }, [available, search]);

  async function choose(name: string) {
    try {
      const icon = await register(name);
      onSelect(icon.character);
      onClose();
    } catch (error) {
      Notify({ message: `Error choosing icon. Error: ${error}`, level: "error", duration: 10000 });
    }
  }

  return (
    <dialog
      ref={dialogRef}
      className="modal"
      // React passes a dialog's close and cancel events on to the dialogs around it, unlike the
      // browser. This dialog is opened from the entity editor's dialog, which would close too.
      onClose={(e) => {
        e.stopPropagation();
        onClose();
      }}
      onCancel={(e) => e.stopPropagation()}
    >
      <div className="modal-box max-w-5xl">
        <h3 className="text-lg font-bold mb-2">Choose icon</h3>
        <input
          type="text"
          className="input input-bordered w-full mb-2"
          placeholder="Search icons, e.g. lamp, fan, tv"
          value={search}
          onChange={(e) => setSearch(e.target.value)}
          autoFocus
        />
        {available === null ? (
          <div className="flex justify-center p-4">
            <span className="loading loading-spinner"></span>
          </div>
        ) : (
          <>
            <div className="grid grid-cols-3 sm:grid-cols-5 md:grid-cols-7 gap-1 max-h-[60vh] overflow-y-auto">
              {matches.slice(0, MAX_SHOWN).map((name) => (
                <button
                  type="button"
                  key={name}
                  title={name}
                  className={`flex flex-col items-center justify-start gap-2 rounded-box p-3 cursor-pointer hover:bg-primary/20 ${
                    name === selectedName ? "bg-primary/20" : ""
                  }`}
                  onClick={() => choose(name)}
                >
                  <span className={`mdi mdi-${name} text-4xl leading-none`}></span>
                  <span className="text-xs text-center leading-tight break-normal">
                    <IconName name={name} />
                  </span>
                </button>
              ))}
            </div>
            <p className="text-sm opacity-70 mt-2">
              {matches.length > MAX_SHOWN ? `Showing ${MAX_SHOWN} of ${matches.length} icons. Search to find more.` : `${matches.length} icons.`}
            </p>
          </>
        )}
        <div className="modal-action">
          <button type="button" className="btn" onClick={onClose}>
            Cancel
          </button>
        </div>
      </div>
    </dialog>
  );
};

/**
 * Choose a custom icon for an entity or scene, or its default icon. value and onChange use the
 * stored form: a custom icon character, or "" for the default icon. defaultIcon: the MDI name of
 * the default icon, to show it while no custom icon is chosen.
 */
export const CustomIconPicker = ({
  value,
  onChange,
  id,
  defaultIcon,
}: {
  value: string | undefined;
  onChange: (character: string) => void;
  id?: string;
  defaultIcon?: string;
}) => {
  const [dialogOpen, setDialogOpen] = useState(false);
  const custom = isCustomIcon(value);

  return (
    <div className="flex items-center gap-2" id={id}>
      <button type="button" className="btn btn-outline" title="Choose icon" onClick={() => setDialogOpen(true)}>
        {custom ? (
          <CustomIcon character={value} className="text-2xl" />
        ) : defaultIcon ? (
          <>
            <span className={`mdi mdi-${defaultIcon} text-2xl`}></span>
            <span className="text-sm">Default</span>
          </>
        ) : (
          <span className="text-sm">Default icon</span>
        )}
      </button>
      {custom && (
        <button type="button" className="btn btn-ghost btn-sm" onClick={() => onChange("")}>
          Use default icon
        </button>
      )}
      <CustomIconDialog open={dialogOpen} onClose={() => setDialogOpen(false)} onSelect={onChange} selected={value} />
    </div>
  );
};
