command_counter = 0;
commands_sent_queue = {};

function process_message(message) {
  if ("msg_id" in message) {
  }
}

function send_command(command) {
  command["cmd_id"] = command_counter;
}

function connect() {
  webSocket = new WebSocket("ws://" + location.hostname + ":8001");

  webSocket.onmessage = (event) => {
    process_message(message);
  };

  webSocket.onclose = (event) => {
    console.log("Websocket closed, trying in 1 second");
    setTimeout(function () {
      connect();
    }, 1000);
  };
}
