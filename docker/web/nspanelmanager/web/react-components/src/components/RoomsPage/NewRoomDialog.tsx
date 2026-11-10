import { useState, type RefObject } from "react";
import { api } from "../../api/client";
import { Notify } from "../NSPanelToastContainer.tsx";
import { TextInput } from "../common/fields.tsx";
import { Alert } from "../common/layout.tsx";
import { useForm, FormProvider } from "react-hook-form";
type NewRoomFormValues = { friendly_name: string };

const NewRoomDialog = ({ dialogRef, onCreated }: { dialogRef: RefObject<HTMLDialogElement | null>; onCreated: () => void }) => {
  const [errorMessage, setErrorMessage] = useState<string | null>(null);
  const form = useForm<NewRoomFormValues>({ defaultValues: { friendly_name: "" } });
  const {
    handleSubmit,
    reset: formReset,
    formState: { isSubmitting },
  } = form;

  const reset = () => {
    setErrorMessage(null);
    formReset();
  };

  const onSubmit = async (values: NewRoomFormValues) => {
    setErrorMessage(null);
    try {
      const { data, error, response } = await api.POST("/rest/v2/rooms", {
        body: { friendly_name: values.friendly_name.trim() },
      });
      if (data) {
        dialogRef.current?.close(); // onClose below resets the form
        Notify({ message: `Room "${data.friendly_name}" created`, level: "success", duration: 3000 });
        // api
        //   .POST("/rest/v2/rooms", {
        //     body: { friendly_name: values.friendly_name.trim() },
        //   })
        //   .then(({ data, error, response }) => {
        //     if (data) {
        //       dialogRef.current?.close(); // onClose below resets the form
        //       Notify({ message: `Room "${data.friendly_name}" created`, level: "success", duration: 3000 });
        //       return;
        //     }
        //     console.error("Failed to create room", error);
        //     // HTTP error (400, 403, 415, ...): keep the dialog open so the user can fix the input
        //     setErrorMessage(`Failed to create room (HTTP ${response.status})`);
        //   })
        //   .catch((err) => setErrorMessage(err instanceof Error ? err.message : String(err)))
        //   .finally(() => setSubmitting(false));

        onCreated();
        return;
      }

      console.log("Failed to create room, error: ", error);
      setErrorMessage(`Failed to create room (HTTP ${response.status})`);
    } catch (failure) {
      setErrorMessage(failure instanceof Error ? failure.message : String(failure));
    }
  };

  //   const handleSubmit = (event: SubmitEvent<HTMLFormElement>) => {
  //     event.preventDefault();
  //     setSubmitting(true);
  //     setErrorMessage(null);
  //
  //     api
  //       .POST("/rest/v2/rooms", {
  //         body: { friendly_name: friendlyName.trim() },
  //       })
  //       .then(({ data, error, response }) => {
  //         if (data) {
  //           dialogRef.current?.close(); // onClose below resets the form
  //           Notify({ message: `Room "${data.friendly_name}" created`, level: "success", duration: 3000 });
  //           onCreated();
  //           return;
  //         }
  //         console.error("Failed to create room", error);
  //         // HTTP error (400, 403, 415, ...): keep the dialog open so the user can fix the input
  //         setErrorMessage(`Failed to create room (HTTP ${response.status})`);
  //       })
  //       .catch((err) => setErrorMessage(err instanceof Error ? err.message : String(err)))
  //       .finally(() => setSubmitting(false));
  //   };

  return (
    <dialog ref={dialogRef} className="modal" onClose={reset}>
      <div className="modal-box">
        <h3 className="text-lg font-bold">New room</h3>
        <FormProvider {...form}>
          <form onSubmit={(event) => void handleSubmit(onSubmit)(event)} noValidate>
            <TextInput name="friendly_name" label="Name" />

            {errorMessage && <Alert level="error">{errorMessage}</Alert>}

            <div className="modal-action">
              <button type="button" className="btn" disabled={isSubmitting} onClick={() => dialogRef.current?.close()}>
                Cancel
              </button>
              <button type="submit" className="btn btn-info" disabled={isSubmitting}>
                {isSubmitting && <span className="loading loading-spinner loading-xs" />}
                Create
              </button>
            </div>
          </form>
        </FormProvider>
      </div>

      {/* Clicking outside the box closes the dialog */}
      <form method="dialog" className="modal-backdrop">
        <button>close</button>
      </form>
    </dialog>
  );
};

export default NewRoomDialog;
