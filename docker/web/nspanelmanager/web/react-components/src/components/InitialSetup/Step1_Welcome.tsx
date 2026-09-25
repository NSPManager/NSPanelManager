const Step1_Welcome = ({ nextStep }: { nextStep: () => void }) => {
  return (
    <div>
      <div className="flex mt-4 justify-end items-center">
        <div>
          <div className="flex items-center justify-center p-8">
            <img className="h-28 bg-neutral/50 rounded-box p-1" src="/static/logo.svg" />
          </div>
          <div className="flex items-center justify-center">
            <span className="text-2xl">Welcome!</span>
          </div>
          <span className="flex items-center justify-center text-center">
            NSPanel Manager is a software for easy management of multiple Sonoff NSPanels and intuitive use for all family members in the household. But you
            probably already knew that.
          </span>
          <span className="flex items-center justify-center text-center mt-4 px-6">
            Thank you for choosing NSPanel Manager! This initial setup guide will take you through the necessary steps to get a working setup, press the button
            below to continue.
          </span>
        </div>
      </div>
      <div className="flex justify-end mt-8">
        <button type="submit" className="btn btn-primary" onClick={() => nextStep()}>
          Get started
        </button>
      </div>
    </div>
  );
};

export default Step1_Welcome;
