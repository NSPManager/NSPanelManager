import type { FieldValues } from "react-hook-form";
import { NumberInput, RadioGroup, SecretInput, Select, TextInput } from "./fields";

/** The inputs in ./fields with their field names checked against the form T. Call it once, at module level. */
export function typedFields<T extends FieldValues>() {
  return {
    TextInput: TextInput<T>,
    NumberInput: NumberInput<T>,
    Select: Select<T>,
    RadioGroup: RadioGroup<T>,
    SecretInput: SecretInput<T>,
  };
}
