$(document).ready(function () {
    console.log('Document ready, attaching functions.');

    $('.modal-background').click(function () {
        $('.modal').removeClass('is-active');
    });

    $('.modal-card-head .delete').click(function () {
        $('.modal').removeClass('is-active');
    });

    $('.modal-cancel-button').click(function () {
        $('.modal').removeClass('is-active');
    });
});

function addNewRoom() {
    $('#modal-new-room').addClass('is-active');
    $('#new_room_name').select();
}