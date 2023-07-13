from django.contrib import admin
from django.urls import path, include

from . import views, api

urlpatterns = [
    # Web routes
    path('', views.index, name='index'),
    path('rooms', views.rooms, name='rooms'),
    path('rooms/order', views.rooms_order, name='rooms_order'),
    path('moveRoomUp/<int:room_id>', views.move_room_up, name='move_room_up'),
    path('moveRoomDown/<int:room_id>', views.move_room_down, name='move_room_down'),
    path('rooms/<int:room_id>/', views.edit_room, name='edit_room'),
    path('save_room/<int:room_id>', views.update_room_form, name='update_room_form'),
    path('nspanel/<int:panel_id>', views.edit_nspanel, name='edit_nspanel'),
    path('save_nspanel_settings/<int:panel_id>', views.save_panel_settings, name='save_panel_settings'),
    path('remove_light/<int:room_id>/<int:light_id>', views.remove_light_from_room, name="remove_light_from_room"),
    path('add_light/<int:room_id>', views.add_light_to_room, name="add_new_light"),
    path('add_scene/<int:room_id>', views.add_scene_to_room, name="add_new_scene_to_room"),
    path('add_scene/', views.add_scene_to_global, name="add_new_scene_to_global"),
    path('add_light_to_view/<int:room_id>', views.add_light_to_room_view, name="add_new_light_to_room_view"),
    path('remove_light_from_view/<int:room_id>', views.remove_light_from_room_view, name="remove_light_from_room_view"),
    path('delete_scene/<int:scene_id>', views.delete_scene, name="delete_scene"),
    path('delete_global_scene/<int:scene_id>', views.delete_global_scene, name="delete_global_scene"),
    path('settings', views.settings_page, name="settings"),
    path('save_settings', views.save_settings, name="save_settings"),
    path('save_new_room', views.save_new_room, name='save_new_room'),
    path('rooms/<int:room_id>/delete', views.delete_room, name='delete_room'),
    path('save_new_firmware', views.save_new_firmware, name='save_new_firmware'),
    path('save_new_data_file', views.save_new_data_file, name='save_new_data_file'),
    path('save_new_merged_flash', views.save_new_merged_flash, name='save_new_merged_flash'),
    path('save_new_tft_file', views.save_new_tft_file, name='save_new_tft_file'),
    path('download_firmware', views.download_firmware, name='download_firmware'),
    path('download_data_file', views.download_data_file, name='download_data_file'),
    path('download_tft', views.download_tft, name='download_tft'),
    path('checksum_firmware', views.checksum_firmware, name='checksum_firmware'),
    path('checksum_data_file', views.checksum_data_file, name='checksum_data_file'),
    path('checksum_tft_file', views.checksum_tft_file, name='checksum_tft_file'),
    # Below are API routes
    path('api/register_nspanel', api.register_nspanel, name='register_nspanel'),
    path('api/delete_nspanel/<int:panel_id>', api.delete_panel, name='delete_nspanel'),
    path('api/reboot_panel', api.reboot_nspanel, name='reboot_panel'),
    path('api/get_nspanel_config', api.get_nspanel_config, name='get_nspanel_config'),
    path('api/get_nspanel_config/room/<int:room_id>', api.get_room_config, name='get_nspanel_config'),
    path('api/get_all_available_lights', api.get_all_available_light_entities, name='get_all_available_lights'),
    path('api/get_light_config/<int:light_id>', api.get_light_config, name='get_all_available_lights'),
    path('api/get_mqtt_manager_config', api.get_mqtt_manager_config, name='get_mqtt_manager_config'),
    path('api/set_panel_status/<str:panel_mac>/', api.set_panel_status, name='set_panel_status'),
    path('api/set_panel_online_status/<str:panel_mac>/', api.set_panel_online_status, name='set_panel_online_status'),
    path('api/save_scene', api.save_scene, name='save_scene'),
    path('api/get_scenes', api.get_scenes, name='get_scenes')
]
