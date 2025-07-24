var stomp_subscription_callbacks = {};
var stomp_subscription_destinations = {};
function stomp_connected_callback() {
  console.log("Connected to STOMP. Resubscribing...");
  $("#ws_connection_in_progress_notification").addClass("hidden");

  // Resubscribe to all subscriptions
  for (const topic in stomp_subscription_destinations) {
    console.log("STOMP sub:", topic);
    var subscription = stomp_client.subscribe(topic, stomp_message_callback);
    stomp_subscription_destinations[topic] = subscription;
  }
}

function stomp_send(destination, message) {
  stomp_client.publish({
    destination: destination,
    headers: {
      "content-type": "text/plain",
    },
    body: message,
  });
}

function stomp_disconnected_callback() {
  console.log("Disconnected from STOMP.");
  $("#ws_connection_in_progress_notification").removeClass("hidden");
}

function stomp_websocket_close_callback() {
  console.log("Disconnected from STOMP (websocket closed).");
  // $("#ws_connection_in_progress_notification").removeClass("hidden");
}

function stomp_websocket_error_callback() {
  console.log("Disconnected from STOMP (websocket error).");
  $("#ws_connection_in_progress_notification").removeClass("hidden");
}

function stomp_error_callback(error) {
  console.error("STOMP error:", error);
}

function stomp_subscribe(topic, callback) {
  if (stomp_subscription_callbacks[topic]) {
    if (!stomp_subscription_callbacks[topic]["callbacks"].includes(callback)) {
      console.log("Attaching callback to existing subscription");
      stomp_subscription_callbacks[topic]["callbacks"].push(callback);
    }
  } else {
    stomp_subscription_callbacks[topic] = {
      subscription: subscription,
      callbacks: [callback],
    };
  }

  // Try subscribing if not already subscribed.
  if (!stomp_subscription_destinations[topic]) {
    stomp_subscription_destinations[topic] = null;

    // If the subscribe fails, log the error but it's still in the list of subscriptions though null.
    // It will most probaby fail because there is no STOMP connection. When a connection is established,
    // the subscription will be retried.
    try {
      var subscription = stomp_client.subscribe(topic, stomp_message_callback);
      stomp_subscription_destinations[topic] = subscription;
    } catch (error) {
      // console.error("STOMP subscription error:", error);
    }
  }
}

function stomp_unsubscribe(topic, callback) {
  stomp_subscription_callbacks[topic]["callbacks"].splice(
    stomp_subscription_callbacks[topic]["callbacks"].indexOf(callback),
    1,
  );

  if (stomp_subscription_callbacks[topic]["callbacks"].length === 0) {
    stomp_subscription_destinations[topic].unsubscribe();
    delete stomp_subscription_callbacks[topic];
    delete stomp_subscription_destinations[topic];
  }
}

function stomp_message_callback(message) {
  stomp_subscription_callbacks[message.headers.destination][
    "callbacks"
  ].forEach(function (callback) {
    callback(message);
  });
}

function stomp_reconnect_callback() {
  // No username or password required to connect to STOMP websocket. Set dummy values.
  stomp_client.connect(
    "user",
    "pass",
    stomp_connected_callback,
    stomp_disconnected_callback,
    stomp_error_callback,
  );
}

// Setup base Stomp connection so that all other components can it use
var stomp_websocket_address = "";
if (location.protocol === "https:") {
  stomp_websocket_address = "wss://";
} else {
  stomp_websocket_address = "ws://";
}

stomp_websocket_address +=
  location.hostname + ":" + location.port + "{{ingress_path}}/websocket/stomp";
const stomp_client = new StompJs.Client({
  brokerURL: stomp_websocket_address,
  connectHeaders: {
    login: "user",
    passcode: "pass",
  },
  reconnectDelay: 2500,
  heartbeatIncoming: 1000,
  heartbeatOutgoing: 1000,
  debug: function (str) {}, // Disable debug logging
});

stomp_client.onConnect = stomp_connected_callback;
stomp_client.onDisconnect = stomp_disconnected_callback;
stomp_client.onWebSocketClose = stomp_websocket_close_callback;
stomp_client.onWebSocketError = stomp_websocket_error_callback;
stomp_client.onStompError = stomp_error_callback;
stomp_client.stompVersions = new StompJs.Versions([]); // Set empty versions as to not set the "Sec-WebSocket-Protocol" header as that causes chrome to fail to connect.

$(document).ready(() => {
  // Only connect once the whole document has loaded and all callbacks has been registered
  stomp_client.activate();
});
