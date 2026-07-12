import { lazy } from "react";

const MultiStep_AddEditEntity = lazy(() => import("./components/MultiStep_AddEditEntity/MultiStep_AddEditEntity"));
const EntitiesPagesView = lazy(() => import("./components/EntitiesPage/EntitiesPagesView"));
const NSPanelsIndexView = lazy(() => import("./components/IndexPage/NSPanelsView"));
const NSPanelToastContainer = lazy(() => import("./components/NSPanelToastContainer"));
const InitialSetup = lazy(() => import("./components/InitialSetup/InitialSetup"));
const NSPanelSideBar = lazy(() => import("./components/NSPanelPage/Sidebar"));
const NSPanelWarnings = lazy(() => import("./components/NSPanelPage/Warnings"));
const NSPanelLogs = lazy(() => import("./components/NSPanelPage/Logs"));

const componentMap = {
  MultiStep_AddEditEntity,
  EntitiesPagesView,
  NSPanelsIndexView,
  NSPanelToastContainer,
  InitialSetup,
  NSPanelSideBar,
  NSPanelWarnings,
  NSPanelLogs,
};

export type ComponentKey = keyof typeof componentMap;

export default componentMap;
