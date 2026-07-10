import { create } from "zustand";
import { RxStomp, RxStompConfig, Versions, type IMessage } from "@stomp/rx-stomp";
import { Notify, RemoveNotification } from "../components/NSPanelToastContainer";
import type { Subscription } from "rxjs";

export interface IMqttManagerWarningData {
  level: "error" | "warning" | "info" | "debug";
  text: string;
}

const rxStompConfig: RxStompConfig = {
  brokerURL: "/websocket/stomp",
  connectHeaders: {
    login: "",
    passcode: "",
  },
  // debug: (msg) => {
  //   console.log("Got STOMP message:", msg);
  // },
  heartbeatIncoming: 1000,
  heartbeatOutgoing: 1000,
  reconnectDelay: 1000,
  stompVersions: new Versions(["1.2"]),
};

interface ISubscriptionData {
  topic: string;
  last_message?: IMessage;
  callbacks: ((message: IMessage) => void)[];
  subscription?: Subscription;
}

export interface IStompStore {
  status: "none" | "connecting" | "connected" | "disconnected";
  stompClient: RxStomp | null;
  mqttmanager_warnings: IMqttManagerWarningData[];
  connect: () => void;
  subscribe: (topic: string, callback: (message: IMessage) => void) => void;
  unsubscribe: (topic: string, callback: (message: IMessage) => void) => void;
  send: (topic: string, body: string) => void;
  _subscriptions: Map<string, ISubscriptionData>;
}

export const useStompStore = create<IStompStore>((set, get) => ({
  status: "none",
  stompClient: null as RxStomp | null,
  mqttmanager_warnings: [],
  connect: () => {
    set({ status: "connecting" });
    if (!get().stompClient) {
      set({ stompClient: new RxStomp() });
    }

    get().stompClient?.configure(rxStompConfig);
    get().stompClient?.activate();
    get().stompClient?.connected$.subscribe(() => {
      set({ status: "connected" });
      RemoveNotification("stomp_disconnected");
    });
    get().stompClient?.webSocketErrors$.subscribe(() => {
      set({ status: "disconnected" });
      Notify({ message: "Disconnected from MQTTManager.", level: "error", toast_id: "stomp_disconnected" });
    });

    get().subscribe("mqttmanager/warnings", (message) => {
      const data = JSON.parse(message.body);
      const warnings = data.warnings as IMqttManagerWarningData[];

      // Remove any set warnings that have been resolved
      const resolvedWarnings = get().mqttmanager_warnings.filter((warning: IMqttManagerWarningData) => {
        return !warnings.some((new_warning) => new_warning.text == warning.text);
      });
      for (const warning of resolvedWarnings) {
        RemoveNotification(warning.text);
      }

      // Show any warnings currently active
      for (const warning of warnings) {
        Notify({ message: warning.text, level: warning.level, toast_id: warning.text });
      }

      set({ mqttmanager_warnings: data.warnings });
    });
  },
  subscribe: (topic: string, callback: (message: IMessage) => void) => {
    if (!get()._subscriptions.has(topic)) {
      const stompClient = get().stompClient;
      if (stompClient) {
        const subscription = stompClient.watch(topic).subscribe((message) => {
          if (!get()._subscriptions.has(topic)) {
            get()._subscriptions.set(topic, { topic, callbacks: [], subscription });
          }
          const topicData = get()._subscriptions.get(topic);
          if (topicData) {
            topicData.last_message = message;
            for (const cb of topicData.callbacks) {
              cb(message);
            }
          }
        });

        get()._subscriptions.set(topic, { topic, callbacks: [callback], subscription });
      } else {
        console.error("Tried to subscribe to ", topic, " but stompClient is null");
      }
    } else {
      const subscription = get()._subscriptions.get(topic);
      if (subscription) {
        subscription.callbacks.push(callback);
        if (subscription.last_message) {
          callback(subscription.last_message);
        }
      }
    }
  },
  unsubscribe: (topic: string, callback: (message: IMessage) => void) => {
    if (get()._subscriptions.has(topic)) {
      const subscription = get()._subscriptions.get(topic);
      if (subscription) {
        const index = subscription.callbacks.indexOf(callback);
        if (index !== -1) {
          subscription.callbacks.splice(index, 1);

          if (subscription.callbacks.length === 0) {
            subscription.subscription?.unsubscribe();
            get()._subscriptions.delete(topic);
          }
        }
      }
    }
  },
  send: (topic: string, body: string) => {
    get().stompClient?.publish({ destination: topic, body });
  },
  _subscriptions: new Map<string, ISubscriptionData>(),
}));
