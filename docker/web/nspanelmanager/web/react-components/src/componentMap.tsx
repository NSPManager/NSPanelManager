import { lazy } from "react";

const MultiStep_AddEditEntity = lazy(() => import("./components/MultiStep_AddEditEntity/MultiStep_AddEditEntity"));
const EntitiesPagesView = lazy(() => import("./components/EntitiesPage/EntitiesPagesView"));

const componentMap = {
  MultiStep_AddEditEntity,
  EntitiesPagesView,
};

export type ComponentKey = keyof typeof componentMap;

export default componentMap;
