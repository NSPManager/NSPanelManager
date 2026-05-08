import { useDroppable } from "@dnd-kit/react";
import { forwardRef } from "react";
// import Step2 from "./step2_select_controller";
// import Step3 from "./Step3";

const EntitiesDropTarget = forwardRef(({ id, type, children }: { id: string; type: string; children?: React.ReactNode }) => {
  const { ref, isDropTarget } = useDroppable({ id: id });

  return (
    <div ref={ref}>
      {(() => {
        if (children) {
          return children;
        }

        return (
          <div
            className={`rounded-box ${isDropTarget ? "border-2 border-solid border-accent" : "border-dashed border-2 border-neutral/50"} flex items-center justify-center indicator w-full h-full`}
          >
            <span className="italic font-thin">{isDropTarget ? "Drop here" : "No " + (type === "scene" ? "scene" : "entity") + " set"}</span>
            <button
              className="btn btn-neutral hover:btn-success btn-xs btn-circle ms-2 flex items-center justify-center"
              title={`Add ${type === "scene" ? "scene" : "entity"}`}
            >
              <span className="mdi mdi-plus"></span>
              {children}
            </button>
          </div>
        );
      })()}
    </div>
  );
});

export default EntitiesDropTarget;
