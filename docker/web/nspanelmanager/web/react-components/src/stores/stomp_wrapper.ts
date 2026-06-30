// Declare stomp wrapper functions that exists in global scope
declare global {
  interface Window {
    stomp_subscribe: typeof stomp_subscribe;
    stomp_unsubscribe: typeof stomp_unsubscribe;
    stomp_send: typeof stomp_send;
  }
}

export const stomp_subscribe = (topic: string, callback: (message: any) => void): void => {
  window.stomp_subscribe(topic, callback);
};

export const stomp_unsubscribe = (topic: string, callback: (message: any) => void): void => {
  window.stomp_unsubscribe(topic, callback);
};

export const stomp_send = (destination: string, message: any): void => {
  window.stomp_send(destination, message);
};
