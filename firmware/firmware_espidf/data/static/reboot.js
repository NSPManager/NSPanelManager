var hasRebootBeenCalled = false;
var hasRebooted = false;

// Websocket timeout handler
function connectionMonitor() { 
    // Do a simple HTTP GET request to check if the device is reachable
    $.ajax({
        url: "/connection_test",
        success: function(data){
            if(hasRebootBeenCalled && hasRebooted) {
                $("#reboot_status_text").html("Device is online. Redirecting.");
                window.location = "/";
            }
        },
        error: function() {
            hasRebooted = true;
        },
        timeout: 1000 //in milliseconds
     });
    setTimeout(connectionMonitor, 500);
}

function reboot() {
    $.get("/do_reboot", function() {
        hasRebootBeenCalled = true;
        $("#reboot_status_text").html("Device is rebooting. Please wait...");
        setTimeout(connectionMonitor, 1000);
    }).fail(function() {
        console.log("Failed to reboot, trying again in 1 second.");
        setTimeout(reboot, 1000);
    });
}


// A $( document ).ready() block.
$(document).ready(function() {
    reboot();
});