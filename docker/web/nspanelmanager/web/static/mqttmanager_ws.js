class MQTTManager_WS {
  command_counter = 0;
  commands_sent_queue = {};
  on_message_function = null;
  on_connect_function = null;
  on_close_function = null;
  websocket = null;
  connected = false;

  process_message(message) {
    if ("cmd_id" in message && message["cmd_id"] in this.commands_sent_queue) {
      var callback = this.commands_sent_queue[message["cmd_id"]];
      callback(message);
    } else if (this.on_message_function != null) {
      this.on_message_function(message);
    } else {
      console.log(
        "ERROR! Got message but has not callback to handle said message."
      );
    }
  }

  is_connected() {
    return this.connected;
  }

  send_command(command, args, handler) {
    if (this.websocket == null) {
      return;
    }

    var cmd = {
      cmd_id: this.command_counter,
      command: command,
      args: args,
    };

    if (handler != null) {
      this.commands_sent_queue[this.command_counter] = handler;
    }

    this.command_counter += 1;
    this.websocket.send(JSON.stringify(cmd));
  }

  register_message_handler(func) {
    this.on_message_function = func;
  }

  register_on_connect_handler(func) {
    this.on_connect_function = func;
  }

  register_on_close_handler(func) {
    this.on_close_function = func;
  }

  connect() {
    var websocket_address = "";
    if(location.protocol === "https:") {
      websocket_address = "wss://";
    } else {
      websocket_address = "ws://";
    }
    
    websocket_address += location.hostname + ":" + location.port + "/websocket";
    console.log("Connecting to " + websocket_address);
    this.websocket = new WebSocket(websocket_address);

    this.websocket.onopen = (event) => {
      this.connected = true;
      if (this.on_connect_function != null) {
        this.on_connect_function();
      }
    };

    this.websocket.onmessage = (event) => {
      try {
        if(event.data.length > 0) {
          var json_message = JSON.parse(event.data);
          this.process_message(json_message);
        }
      }catch (error) {
        console.log("Received error while processing message " + event.data);
        console.log(error);
      }
    };

    this.websocket.onerror = () => {
      this.websocket.close();
    };

    this.websocket.onclose = (event) => {
      this.connected = false;
      if (event.code != 1001) { // Ignore when browser is leaving page.
        if (this.on_close_function != null) {
          this.on_close_function();
        }
        setTimeout(() => {
          this.connect();
        }, 1000);
      }
    };
  }
}
