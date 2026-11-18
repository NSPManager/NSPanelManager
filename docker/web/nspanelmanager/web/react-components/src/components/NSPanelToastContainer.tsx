import { toast, ToastContainer, Bounce, type TypeOptions, type Id } from "react-toastify";

export const RemoveNotification = (toast_id: Id) => {
  toast.dismiss(toast_id);
};

export const Notify = ({
  message,
  level,
  duration,
  toast_id,
}: {
  message: string;
  level: "error" | "warning" | "info" | "debug" | "success";
  duration?: number;
  toast_id?: Id;
}) => {
  let toastType: TypeOptions = "default";
  if (level === "error") toastType = "error";
  if (level === "warning") toastType = "warning";
  if (level === "info") toastType = "info";
  if (level === "debug") toastType = "default";
  if (level === "success") toastType = "success";

  toast(CustomToast, {
    autoClose: duration ?? false,
    type: toastType,
    theme: "colored",
    icon: false,
    data: {
      message: message,
      level: level,
    },
    toastId: toast_id,
  });
};

export const CustomToast = ({ data }: { data: { message: string; level: "error" | "warning" | "info" | "debug" | "success" } }) => {
  return (
    <div className="pl-1 py-2 me-6">
      <p>{data.message}</p>
    </div>
  );
};

const contextClass = {
  success: "bg-success text-success-content",
  error: "bg-error text-error-content",
  info: "bg-info text-info-content",
  warning: "bg-warning text-warning-content",
  default: "bg-neutral text-neutral-content",
};

export default function NSPanelToastContainer() {
  return (
    <ToastContainer
      position="bottom-right"
      autoClose={5000}
      hideProgressBar={false}
      newestOnTop={false}
      closeOnClick={false}
      rtl={false}
      pauseOnFocusLoss
      draggable
      pauseOnHover
      theme="light"
      transition={Bounce}
      toastClassName={(context) =>
        contextClass[context?.type || "default"] + " relative flex min-w-84 p-1 min-h-10 rounded-md justify-between overflow-hidden cursor-pointer my-2"
      }
      progressClassName="progress"
      className=""
    />
  );
}
