from django.contrib import admin
from django.urls import path, include
from .components.nspanel_status_wifi_signal_strength.nspanel_status_wifi_signal_strength import NSPanelStatusWifiSignalStrength
from .components.nspanel_status_text.nspanel_status_text import NSPanelStatusText
from .components.nspanel_status_header.nspanel_status_header import NSPanelHeader
from .components.nspanel_status_ram_usage.nspanel_status_ram_usage import NSPanelStatusRamUsage
from .components.nspanel_status_temperature.nspanel_status_temperature import NSPanelStatusTemperature
from .components.nspanel_visit_link.nspanel_visit_link import NSPanelVisitLink
from . import views, api, rest, htmx

urlpatterns = [
    # Web routes
    path('', views.index, name='index'),
    # Initial setup:
    path('initial_setup_manager_config', views.initial_setup_manager_config,
         name='initial_setup_manager_config'),
    path('initial_setup_mqtt_config', views.initial_setup_mqtt_config,
         name='initial_setup_mqtt_config'),
    path('initial_setup_home_assistant_config', views.initial_setup_home_assistant_config,
         name='initial_setup_home_assistant_config'),
    path('initial_setup_openhab_config', views.initial_setup_openhab_config,
         name='initial_setup_openhab_config'),
    path('rooms', views.rooms, name='rooms'),
    path('rooms/order', views.rooms_order, name='rooms_order'),
    path('moveRoomUp/<int:room_id>', views.move_room_up, name='move_room_up'),
    path('moveRoomDown/<int:room_id>',
         views.move_room_down, name='move_room_down'),
    path('rooms/<int:room_id>/', views.edit_room, name='edit_room'),
    path('save_room/<int:room_id>',
         views.update_room_form, name='update_room_form'),
    path('nspanel/<int:panel_id>', views.edit_nspanel, name='edit_nspanel'),
    path('save_nspanel_settings/<int:panel_id>',
         views.save_panel_settings, name='save_panel_settings'),
    path('remove_light/<int:room_id>/<int:light_id>',
         views.remove_light_from_room, name="remove_light_from_room"),
    path('add_light/<int:room_id>', views.add_light_to_room, name="add_new_light"),
    path('add_scene/<int:room_id>', views.add_scene_to_room,
         name="add_new_scene_to_room"),
    path('add_existing_scene/<int:room_id>', views.add_existing_scene_to_room,
         name="add_existing_scene_to_room"),
    path('add_scene/', views.add_scene_to_global,
         name="add_new_scene_to_global"),
    path('add_light_to_view/<int:room_id>', views.add_light_to_room_view,
         name="add_new_light_to_room_view"),
    path('remove_light_from_view/<int:room_id>',
         views.remove_light_from_room_view, name="remove_light_from_room_view"),
    path('delete_scene/<int:scene_id>', views.delete_scene, name="delete_scene"),
    path('delete_global_scene/<int:scene_id>',
         views.delete_global_scene, name="delete_global_scene"),
    path('settings', views.settings_page, name="settings"),
    path('save_settings', views.save_settings, name="save_settings"),
    path('save_new_room', views.save_new_room, name='save_new_room'),
    path('rooms/<int:room_id>/delete', views.delete_room, name='delete_room'),
    path('save_new_firmware', views.save_new_firmware, name='save_new_firmware'),
    path('save_new_data_file', views.save_new_data_file, name='save_new_data_file'),
    path('save_new_merged_flash', views.save_new_merged_flash,
         name='save_new_merged_flash'),
    path('save_new_tft_file', views.save_new_tft_file, name='save_new_tft_file'),
    path('download_firmware', views.download_firmware, name='download_firmware'),
    path('download_data_file', views.download_data_file, name='download_data_file'),
    path('download_tft', views.download_tft, name='download_tft'),
    path('download_tft_eu', views.download_tft_eu, name='download_tft_eu'),
    path('download_tft_us', views.download_tft_us, name='download_tft_us'),
    path('checksum_firmware', views.checksum_firmware, name='checksum_firmware'),
    path('checksum_data_file', views.checksum_data_file, name='checksum_data_file'),
    path('checksum_tft_file', views.checksum_tft_file, name='checksum_tft_file'),
    path('checksum_tft_file_eu', views.checksum_tft_file_eu,
         name='checksum_tft_file'),
    path('checksum_tft_file_us', views.checksum_tft_file_us,
         name='checksum_tft_file_us'),
    path('manual', views.get_manual, name="manual"),
    path('download_mqttmanager_log', views.download_mqttmanager_log,
         name="download_mqttmanager_log"),
    # Entities:
    path('entities/relay_groups', views.relay_groups, name="relay_groups"),
    path('entities/relay_groups/create', views.create_or_update_relay_group,
         name="create_or_update_relay_group"),
    path('entities/relay_groups/add_relay',
         views.add_nspanel_relay_to_group, name="add_nspanel_relay_to_group"),
    path('entities/relay_groups/delete_relay/<int:relay_binding_id>',
         views.delete_relay_group_binding, name="delete_relay_group_binding"),
    path('entities/relay_groups/delete/<int:relay_group_id>',
         views.delete_relay_group, name="delete_relay_group"),
    path('entities/global_scenes', views.global_scenes, name="global_scenes"),
    path('entities/weather_and_time',
         views.weather_and_time, name="weather_and_time"),
    path('denied_nspanels',
         views.denied_nspanels, name="denied_nspanels"),
    path('unblock_nspanel/<int:nspanel_id>',
         views.unblock_nspanel, name="unblock_nspanel"),
    # Below are API routes
    path('api/register_nspanel', api.register_nspanel, name='register_nspanel'),
    path('api/delete_nspanel/<int:panel_id>',
         api.delete_panel, name='delete_nspanel'),
    path('api/get_nspanel_config', api.get_nspanel_config,
         name='get_nspanel_config'),
    path('api/get_nspanel_config/room/<int:room_id>',
         api.get_room_config, name='get_nspanel_config'),
    path('api/get_nspanels_warnings', api.get_nspanels_warnings,
         name='get_nspanels_warnings'),
    path('api/get_all_available_entities', api.get_all_available_entities,
         name='get_all_available_entities'),
    path('api/get_light_config/<int:light_id>',
         api.get_light_config, name='get_all_available_entities'),
    path('api/get_mqtt_manager_config', api.get_mqtt_manager_config,
         name='get_mqtt_manager_config'),
    path('api/set_panel_status/<str:panel_mac>/',
         api.set_panel_status, name='set_panel_status'),
    path('api/set_panel_online_status/<str:panel_mac>/',
         api.set_panel_online_status, name='set_panel_online_status'),
    path('api/save_scene', api.save_scene, name='save_scene'),
    path('api/get_scenes', api.get_scenes, name='get_scenes'),
    path('api/restart_mqtt_manager', api.restart_mqtt_manager,
         name='restart_mqtt_manager'),
    path('api/save_theme', api.save_theme, name='save_theme'),

    #####################
    ### Rest API URLs ###
    #####################
    # MQTTManager endpoints
    path('rest/mqttmanager/settings/<str:setting_key>', rest.mqttmanager_get_setting, name="rest_mqttmanager_get_setting"),
    path('rest/mqttmanager/settings', rest.mqttmanager_settings_post, name="rest_mqttmanager_settings_post"),
    # NSPanel URLs
    path('rest/nspanels/warnings', rest.nspanel_warnings, name='rest_nspanel_warnings'),
    path('rest/nspanels/<int:panel_id>', rest.nspanel_delete, name='rest_nspanel_delete'),
    path('rest/nspanels', rest.nspanels, name='rest_nspanels'),
    # Relay groups
    path('rest/relay_groups', rest.relay_groups, name='rest_relay_groups'),
    # Room URLs
    path('rest/rooms/<int:room_id>', rest.room_delete, name='rest_room_delete'),
    path('rest/rooms', rest.rooms, name='rest_rooms_create'),
    # Light URLs
    path('rest/lights', rest.lights, name='rest_lights'),
    path('rest/lights/<int:light_id>', rest.light_delete, name='rest_light_delete'),
    # Scene URLs
    path('rest/scenes', rest.scenes, name='rest_lights'),



    #######################
    ### HTMX "API" URLs ###
    #######################
    path('htmx/partial/nspanel_index_view/<int:nspanel_id>', htmx.partial_nspanel_index_view, name="htmx_partial_nspanel_index_view"),
    path('htmx/nspanels/<int:nspanel_id>/reboot', htmx.nspanel_reboot, name='htmx_nspanel_reboot'),
    path('htmx/nspanels/<int:nspanel_id>/update_screen', htmx.nspanel_update_screen, name='htmx_nspanel_update_screen'),
    path('htmx/nspanels/<int:nspanel_id>/update_firmware', htmx.nspanel_update_firmware, name='htmx_nspanel_update_firmware'),

    path('htmx/partial/nspanel_status_header/<str:view>/<int:nspanel_id>', NSPanelHeader.as_view(), name="nspanel_status_header"),
    path('htmx/partial/nspanel_status_text/<str:view>/<int:nspanel_id>', NSPanelStatusText.as_view(), name="nspanel_status_text"),
    path('htmx/partial/nspanel_status_wifi_signal_strength/<str:view>/<int:nspanel_id>', NSPanelStatusWifiSignalStrength.as_view(), name="nspanel_status_wifi_signal_strength"),
    path('htmx/partial/nspanel_status_ram_usage/<str:view>/<int:nspanel_id>', NSPanelStatusRamUsage.as_view(), name="nspanel_status_ram_usage"),
    path('htmx/partial/nspanel_status_temperature/<str:view>/<int:nspanel_id>', NSPanelStatusTemperature.as_view(), name="nspanel_status_temperature"),
    path('htmx/partial/nspanel_visit_link/<str:view>/<int:nspanel_id>', NSPanelVisitLink.as_view(), name="nspanel_visit_link"),
]
