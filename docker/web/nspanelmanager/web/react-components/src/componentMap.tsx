import { lazy } from "react";

const MultiStep_AddEditEntity = lazy(() => import("./components/MultiStep_AddEditEntity/MultiStep_AddEditEntity"));
const EntitiesPagesView = lazy(() => import("./components/EntitiesPage/EntitiesPagesView"));
const NSPanelsIndexView = lazy(() => import("./components/IndexPage/NSPanelsView"));
const NSPanelToastContainer = lazy(() => import("./components/NSPanelToastContainer"));
const InitialSetup = lazy(() => import("./components/InitialSetup/InitialSetup"));

const componentMap = {
  MultiStep_AddEditEntity,
  EntitiesPagesView,
  NSPanelsIndexView,
  NSPanelToastContainer,
  InitialSetup,
};

export type ComponentKey = keyof typeof componentMap;

export default componentMap;
