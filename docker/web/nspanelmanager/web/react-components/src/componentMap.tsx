import { lazy } from "react";

const MultiStep_AddEditEntity = lazy(() => import("./components/MultiStep_AddEditEntity/MultiStep_AddEditEntity"));
const MultiStep_AddEditEntity_Step1 = lazy(() => import("./components/MultiStep_AddEditEntity/step1_select_type"));

const componentMap = {
  MultiStep_AddEditEntity,
  MultiStep_AddEditEntity_Step1,
};

export type ComponentKey = keyof typeof componentMap;

export default componentMap;
