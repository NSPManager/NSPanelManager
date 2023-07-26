function add_new_scene() {
    $("#edit_scene_id").val("");
    $("#add_new_scene_to_room_modal").addClass("is-active");
}

function edit_scene(id, name) {
    $("#edit_scene_id").val(id);
    $("#scene_name").val(name);
    $("#add_new_scene_to_room_modal").addClass("is-active");
}
