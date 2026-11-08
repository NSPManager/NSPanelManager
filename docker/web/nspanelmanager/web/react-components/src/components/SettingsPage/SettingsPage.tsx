import { useCallback, useEffect, useState } from "react";
import { FormProvider, useForm, useWatch } from "react-hook-form";
import { api, type Settings, type SettingsErrors } from "../../api/client";
import { getIpByHostname, testHomeAssistant, testManager, testOpenhab } from "../../api/connectionTests";
import { Notify } from "../NSPanelToastContainer";
import { BuzzerCard } from "./BuzzerCard";
import { LOG_LEVELS, NSPANEL_TYPES, options, SCREENSAVER_MODES, TURN_ON_BEHAVIORS } from "./choices";
import { JoinButton, TestButton } from "../common/buttons";
import { Alert, Card, Row } from "../common/layout";
import { typedFields } from "../common/typedFields";
import { buildPatch, MODES_WITH_OUTSIDE_TEMPERATURE, toFormValues, type SettingsFormValues } from "./form";

const { NumberInput, RadioGroup, SecretInput, Select, TextInput } = typedFields<SettingsFormValues>();

type LoadState = { status: "loading" } | { status: "error"; message: string } | { status: "loaded"; settings: Settings };

/** The global settings page (/settings), backed by /rest/v2/settings. */
export default function SettingsPage({ logDownloadUrl = "/download_mqttmanager_log" }: { logDownloadUrl?: string }) {
  const [state, setState] = useState<LoadState>({ status: "loading" });

  // Sets the state only once the request finishes.
  const fetchSettings = useCallback(() => {
    api
      .GET("/rest/v2/settings")
      .then(({ data, response }) => setState(data ? { status: "loaded", settings: data } : { status: "error", message: `HTTP ${response.status}` }))
      .catch((error) => setState({ status: "error", message: error instanceof Error ? error.message : String(error) }));
  }, []);

  useEffect(fetchSettings, [fetchSettings]);

  const retry = () => {
    setState({ status: "loading" });
    fetchSettings();
  };

  return (
    <div>
      <div className="flex items-center justify-between mb-2">
        <h2 className="font-medium text-2xl">Settings</h2>
        <a href="https://nspanelmanager.com/docs/web-interface/#global-settings" target="_blank" rel="noreferrer" className="btn btn-outline btn-info">
          Help
        </a>
      </div>
      {state.status === "loading" && <span className="loading loading-spinner" aria-label="Loading settings" />}
      {state.status === "error" && (
        <>
          <Alert level="error">Couldn't load the settings: {state.message}</Alert>
          <button type="button" className="btn" onClick={retry}>
            Retry
          </button>
        </>
      )}
      {state.status === "loaded" && <SettingsForm settings={state.settings} logDownloadUrl={logDownloadUrl} />}
    </div>
  );
}

function SettingsForm({ settings: loaded, logDownloadUrl }: { settings: Settings; logDownloadUrl: string }) {
  const [settings, setSettings] = useState(loaded);
  const form = useForm<SettingsFormValues>({ defaultValues: toFormValues(loaded) });
  const {
    handleSubmit,
    reset,
    setError,
    setValue,
    getValues,
    control,
    formState: { isDirty, isSubmitting, dirtyFields },
  } = form;
  const addon = settings.is_home_assistant_addon;

  const onSubmit = async (values: SettingsFormValues) => {
    const body = buildPatch(values, dirtyFields, addon);
    if (Object.keys(body).length === 0) {
      reset(values);
      return;
    }
    try {
      const { data, error, response } = await api.PATCH("/rest/v2/settings", { body });
      if (data) {
        setSettings(data);
        reset(toFormValues(data));
        Notify({ message: "Settings saved. Reboot the panels for panel settings to take effect.", level: "success", duration: 5000 });
        return;
      }
      if (response.status === 400 && error && !("detail" in error && error.detail)) {
        const unplaced = applyFieldErrors(error as SettingsErrors, values, setError);
        Notify({ message: `Nothing was saved. ${unplaced.length ? unplaced.join(" ") : "Fix the highlighted settings."}`, level: "error", duration: 8000 });
      } else {
        const detail = error && "detail" in error ? error.detail : `HTTP ${response.status}`;
        Notify({ message: `Couldn't save the settings: ${detail}`, level: "error" });
      }
    } catch (failure) {
      Notify({ message: `Couldn't save the settings: ${failure instanceof Error ? failure.message : String(failure)}`, level: "error" });
    }
  };

  const loadAddressFromUrl = async () => {
    try {
      setValue("manager_address", await getIpByHostname(window.location.hostname), { shouldDirty: true });
    } catch (error) {
      Notify({
        message: `Couldn't get the manager's IP address from ${window.location.hostname}: ${error instanceof Error ? error.message : error}`,
        level: "error",
      });
    }
  };

  const loadPortFromUrl = () => {
    const port = window.location.port || (window.location.protocol === "https:" ? "443" : "80");
    setValue("manager_port", Number(port), { shouldDirty: true });
  };

  const [screensaverMode, typedHomeAssistantToken, typedOpenhabToken] = useWatch({
    control,
    name: ["screensaver_mode", "home_assistant_token", "openhab_token"],
  });

  return (
    <FormProvider {...form}>
      <form onSubmit={handleSubmit(onSubmit)} noValidate>
        <Alert level="info">Changes to NSPanel settings on this page require a reboot of all NSPanels to take effect.</Alert>

        <Card title="NSPanel Manager">
          <Alert level="warning">The manager address must be an IP address only: not a hostname, and without http://.</Alert>
          <TextInput name="manager_address" label="Manager address" button={<JoinButton onClick={loadAddressFromUrl}>Load from URL</JoinButton>} />
          <NumberInput name="manager_port" label="Manager port" button={<JoinButton onClick={loadPortFromUrl}>Load from URL</JoinButton>} />
          <TestButton label="Test manager address" run={() => testManager(getValues("manager_address") ?? "", getValues("manager_port") ?? "")} />
        </Card>

        <Card
          title="Logging"
          actions={
            <a href={logDownloadUrl} title="Download MQTTManager log" className="btn btn-outline btn-info">
              <span className="mdi mdi-download" /> Download
            </a>
          }
        >
          <Select name="mqttmanager_log_level" label="MQTTManager log level" options={options(LOG_LEVELS)} />
          <Row>
            <NumberInput name="max_live_log_messages" label="Messages shown in the live log" />
            <NumberInput name="max_log_buffer_size" label="Messages kept in the buffer" />
          </Row>
        </Card>

        <Card title="Global NSPanel settings">
          <Select name="default_nspanel_type" label="Default NSPanel type" options={options(NSPANEL_TYPES)} />
          <Row>
            <NumberInput name="color_temp_min" label="Minimum color temperature (K)" />
            <NumberInput name="color_temp_max" label="Maximum color temperature (K)" />
          </Row>
          <RadioGroup
            name="reverse_color_temp"
            label="Color temperature slider"
            options={[
              [false, "Keep default"],
              [true, "Reverse"],
            ]}
          />
          <RadioGroup name="turn_on_behavior" label="Turn on behavior" options={options(TURN_ON_BEHAVIORS)} />
          <Row>
            <NumberInput name="raise_to_100_light_level" label="Raise lights to 100% above (%)" />
            <NumberInput name="light_turn_on_brightness" label="Default light turn on brightness (%)" />
          </Row>
          <Row>
            <NumberInput name="min_button_push_time" label="Minimum button push time (ms)" />
            <NumberInput name="button_long_press_time" label="Minimum long press time (ms)" />
          </Row>
          <Row>
            <NumberInput name="special_mode_trigger_time" label="Special mode activation time (ms)" />
            <NumberInput name="special_mode_release_time" label="Special mode timeout (ms)" />
          </Row>
          <Row>
            <NumberInput name="screen_dim_level" label="Screen brightness (%)" />
            <NumberInput name="screensaver_dim_level" label="Screensaver brightness (%)" />
          </Row>
          <NumberInput name="screensaver_activation_timeout" label="Screensaver activation timeout (ms)" />
          <Select name="screensaver_mode" label="Screensaver mode" options={options(SCREENSAVER_MODES)} />
          <RadioGroup
            name="show_screensaver_inside_temperature"
            label="Screensaver inside temperature"
            options={[
              [true, "Show inside temperature"],
              [false, "Hide inside temperature"],
            ]}
          />
          {MODES_WITH_OUTSIDE_TEMPERATURE.includes(screensaverMode ?? "") && (
            <RadioGroup
              name="show_screensaver_outside_temperature"
              label="Screensaver outside temperature"
              options={[
                [true, "Show outside temperature"],
                [false, "Hide outside temperature"],
              ]}
            />
          )}
          <RadioGroup
            name="optimistic_mode"
            label="Entity update mode"
            options={[
              [true, "Optimistic"],
              [false, "Wait"],
            ]}
          />
          <Row>
            <NumberInput name="room_status_backoff_time" label="Room status backoff time (ms)" />
            <NumberInput name="all_rooms_status_backoff_time" label={'"All rooms" status backoff time (ms)'} />
          </Row>
        </Card>

        <BuzzerCard />

        <Card title="Telemetry">
          <RadioGroup
            name="telemetry_enabled"
            label="Telementry enabled"
            options={[
              [true, "Enabled"],
              [false, "Disabled"],
            ]}
          />

          <div className="flex flex-col divide-y divide-accent bg-base-200 rounded-box">
            <div className="p-2">
              <div className="text-sm w-full">A randomized installation ID.</div>
              <span className="text-xs w-full">
                This is collected to allow us to track number of unique installations and group other tracked items to this ID.
              </span>
            </div>

            <div className="p-2">
              <div className="text-sm w-full">Number of each type of entity</div>
              <span className="text-xs w-full">
                This is simply to know what to focus on. If there are to be improvments or there are issues it simply sets our priorities straight.
              </span>
            </div>

            <div className="p-2">
              <div className="text-sm w-full">Errors/Exceptions</div>
              <span className="text-xs w-full">
                We want to track errors and exceptions so that we can get feedback on issues that needs to be looked into. These will never include detailed
                information and will be more like "Failed to decode NSPanel log level".
              </span>
            </div>
          </div>
        </Card>

        <Card title="MQTT">
          <Row>
            <TextInput name="mqtt_server" label="MQTT server" />
            <NumberInput name="mqtt_port" label="MQTT port" />
          </Row>
          <Row>
            <TextInput name="mqtt_username" label="MQTT username" hint="Blank for anonymous login." />
            <SecretInput name="mqtt_password" clearName="clear_mqtt_password" label="MQTT password" isSet={settings.mqtt_password_set} />
          </Row>
        </Card>

        <Card title="Home Assistant API">
          {addon && (
            <Alert level="info">You are running NSPanel Manager as a Home Assistant add-on. These settings are managed by Home Assistant and are locked.</Alert>
          )}
          <TextInput name="home_assistant_address" label="Home Assistant address" hint="For example http://homeassistant.local:8123" disabled={addon} />
          <SecretInput
            name="home_assistant_token"
            clearName="clear_home_assistant_token"
            label="Home Assistant token"
            isSet={settings.home_assistant_token_set}
            disabled={addon}
          />
          {!addon && (
            <TestButton
              label="Test Home Assistant"
              disabledReason={
                typedHomeAssistantToken
                  ? undefined
                  : "Enter a token to test the connection. For security, the saved token is never sent back to the browser, so it can't be used here."
              }
              run={() => testHomeAssistant(getValues("home_assistant_address") ?? "", typedHomeAssistantToken)}
            />
          )}
        </Card>

        <Card title="OpenHAB API">
          <TextInput name="openhab_address" label="OpenHAB address" />
          <SecretInput name="openhab_token" clearName="clear_openhab_token" label="OpenHAB token" isSet={settings.openhab_token_set} />
          <TestButton
            label="Test OpenHAB"
            disabledReason={
              typedOpenhabToken
                ? undefined
                : "Enter a token to test the connection. For security, the saved token is never sent back to the browser, so it can't be used here."
            }
            run={() => testOpenhab(getValues("openhab_address") ?? "", typedOpenhabToken)}
          />
        </Card>

        <div className="flex justify-end gap-2 mt-4 mb-4">
          <button type="button" className="btn btn-ghost" disabled={!isDirty || isSubmitting} onClick={() => reset()}>
            Discard changes
          </button>
          <button type="submit" className="btn btn-success" disabled={!isDirty || isSubmitting}>
            {isSubmitting && <span className="loading loading-spinner loading-sm" />}
            Save
          </button>
        </div>
      </form>
    </FormProvider>
  );
}

// Field errors from a 400 go next to their field; returns the messages that have no field on this page.
function applyFieldErrors(
  errors: SettingsErrors,
  values: SettingsFormValues,
  setError: (name: keyof SettingsFormValues, error: { type: string; message: string }) => void,
): string[] {
  const unplaced: string[] = [];
  for (const [name, messages] of Object.entries(errors)) {
    const message = Array.isArray(messages) ? messages.join(" ") : String(messages);
    if (name in values) {
      setError(name as keyof SettingsFormValues, { type: "server", message });
    } else {
      unplaced.push(name === "non_field_errors" ? message : `${name}: ${message}`);
    }
  }
  return unplaced;
}
