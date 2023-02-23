function startNSPanelOtaUpdate(ip_address) {
    $.post("http://" + ip_address + "/start_ota_update", function (data) {
        $('#modal-command-sent').addClass('is-active');
    });
}