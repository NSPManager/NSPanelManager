import { lazy } from "react";

const MultiStep_AddEditEntity = lazy(() => import("./components/MultiStep_AddEditEntity/MultiStep_AddEditEntity"));
const EntitiesPagesView = lazy(() => import("./components/EntitiesPage/EntitiesPagesView"));
const NSPanelsIndexView = lazy(() => import("./components/IndexPage/NSPanelsView"));
const NSPanelToastContainer = lazy(() => import("./components/NSPanelToastContainer"));
const StompWrapper = lazy(() => import("./stores/stomp"));

const componentMap = {
  MultiStep_AddEditEntity,
  EntitiesPagesView,
  NSPanelsIndexView,
  NSPanelToastContainer,
  StompWrapper,
};

export type ComponentKey = keyof typeof componentMap;

export default componentMap;
