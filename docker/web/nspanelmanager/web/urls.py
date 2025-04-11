from django.contrib import admin
from django.urls import path, include
from .components.nspanel_status_wifi_signal_strength.nspanel_status_wifi_signal_strength import NSPanelStatusWifiSignalStrength
from .components.nspanel_status_text.nspanel_status_text import NSPanelStatusText
from .components.nspanel_status_badge.nspanel_status_badge import NSPanelStatusBadge
from .components.nspanel_status_header.nspanel_status_header import NSPanelHeader
from .components.nspanel_status_ram_usage.nspanel_status_ram_usage import NSPanelStatusRamUsage
from .components.nspanel_status_temperature.nspanel_status_temperature import NSPanelStatusTemperature
from .components.nspanel_update_progress.nspanel_update_progress import NSPanelUpdateProgress
from .components.nspanel_visit_link.nspanel_visit_link import NSPanelVisitLink
from .components.nspanel_reboot_button.nspanel_reboot_button import NSPanelRebootButton
from .components.nspanel_warnings.nspanel_warnings import NSPanelWarnings
from .components.nspanel_room_entities_pages.nspanel_room_entities_pages import NSPanelRoomEntitiesPages
from . import views, api, rest, htmx, protobuf_api

urlpatterns = [
    # Web routes
    path('', views.index, name='index'),
    path('rooms', views.rooms, name='rooms'),
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

    ####################
    ### PROTOBUF API ###
    ####################
    path('protobuf/mqttmanager/all_settings', protobuf_api.mqttmanager_get_all_settings, name="protobuf_mqttmanager_get_all_settings"),
    path('protobuf/mqttmanager/all_nspanels', protobuf_api.mqttmanager_get_all_nspanels, name="protobuf_mqttmanager_get_all_nspanels"),
    path('protobuf/mqttmanager/all_lights', protobuf_api.mqttmanager_get_all_lights, name="protobuf_mqttmanager_get_all_lights"),
    path('protobuf/mqttmanager/lights/<int:light_id>', protobuf_api.mqttmanager_get_light),
    path('protobuf/mqttmanager/all_rooms', protobuf_api.mqttmanager_get_all_rooms, name="protobuf_mqttmanager_get_all_rooms"),
    path('protobuf/mqttmanager/get_room_entities_page/<int:page_id>', protobuf_api.mqttmanager_get_room_entities_page),
    path('protobuf/mqttmanager/get_room_entities_page/<int:page_id>/entity_in_slot/<int:room_view_position>', protobuf_api.mqttmanager_get_entity_at_room_entities_page_position),


    #######################
    ### HTMX "API" URLs ###
    #######################
    path('htmx/partial/index_nspanels_section/', htmx.partial_index_nspanels_section, name="htmx_partial_index_nspanels_section"),
    path('htmx/partial/nspanel_index_view/<int:nspanel_id>', htmx.partial_nspanel_index_view, name="htmx_partial_nspanel_index_view"),
    path('htmx/unblock_nspanel/<int:nspanel_id>', htmx.unblock_nspanel, name="htmx_unblock_nspanel"),
    path('htmx/partial/handle_entity_modal_result', htmx.handle_entity_modal_result, name='htmx_handle_entity_modal_result'),
    path('htmx/partial/handle_entity_modal_entity_selected/<str:entity>', htmx.handle_entity_modal_entity_selected, name='htmx_handle_entity_modal_entity_selected'),
    path('htmx/partial/select_new_entity_type/<str:action>/<str:action_args>', htmx.partial_add_entity_to_entities_page_select_entity_type, name='htmx_partial_select_new_entity_type'),
    path('htmx/partial/select_new_entity_source/<str:action>/<str:action_args>', htmx.partial_add_entity_to_entities_page_select_entity_source, name='htmx_partial_select_new_entity_source'),
    path('htmx/partial/select_new_entity_config_modal/<str:entity_source>', htmx.partial_add_entity_to_entities_page_config_modal, name='htmx_partial_select_new_entity_config'),
    path('htmx/partial/select_new_entity_item_list/<str:action>/<str:action_args>', htmx.partial_select_new_entity_item_list, name='htmx_partial_select_new_entity_item_list'),
    path('htmx/partial/select_new_outside_temperature_sensor', htmx.partial_select_new_outside_temperature_sensor, name='htmx_partial_select_new_outside_temperature_sensor'),
    path('htmx/partial/add_light_entity_to_room/<int:room_id>', htmx.partial_entity_add_light_entity, name='htmx_partial_add_light_entity'),
    path('htmx/partial/edit_light_entity_in_room/<int:light_id>', htmx.partial_entity_edit_light_entity, name='htmx_partial_edit_light_entity'),
    path('htmx/partial/add_switch_entity_to_room/<int:room_id>', htmx.partial_entity_add_switch_entity, name='htmx_partial_add_switch_entity'),
    path('htmx/partial/edit_switch_entity_in_room/<int:switch_id>', htmx.partial_entity_edit_switch_entity, name='htmx_partial_edit_switch_entity'),
    path('htmx/partial/edit_scene_entity/<int:scene_id>', htmx.partial_entity_edit_scene_entity, name='htmx_partial_edit_scene_entity'),
    path('htmx/partial/remove_entity_from_page_slot/<int:page_id>/<int:slot_id>', htmx.partial_remove_entity_from_page_slot, name='htmx_partial_remove_entity_from_page_slot'),
    path('htmx/partial/add_entities_page_to_room/<int:room_id>/<str:is_scenes_page>/<str:is_global_scenes_page>', htmx.partial_add_entities_page_to_room, name='htmx_partial_add_entities_page_to_room'),
    path('htmx/partial/edit_entities_page/<int:page_id>', htmx.partial_edit_entities_page, name='htmx_partial_edit_entities_page'),
    path('htmx/partial/edit_entities_page/<int:page_id>/save/<int:page_type>', htmx.partial_save_edit_entities_page, name='htmx_partial_save_edit_entities_page'),
    path('htmx/partial/move_entity', htmx.partial_move_entity, name='htmx_partial_move_entity'),
    path('htmx/partial/reorder_entity_pages', htmx.partial_move_entities_pages, name='htmx_partial_move_entities_pages'),
    path('htmx/partial/delete_entities_page/<int:page_id>', htmx.partial_delete_entities_page, name='htmx_partial_delete_entities_page'),
    path('htmx/partial/create_entities_page_in_room/<int:room_id>/<int:page_type>/<str:is_scenes_page>/<str:is_global_scenes_page>', htmx.create_entities_page_in_room, name='htmx_create_entities_page_in_room'),
    path('htmx/partial/reorder_rooms', htmx.partial_reorder_rooms, name='htmx_partial_reorder_rooms'),
    path('htmx/nspanels/<int:nspanel_id>/reboot', htmx.nspanel_reboot, name='htmx_nspanel_reboot'),
    path('htmx/nspanels/<int:nspanel_id>/update_screen', htmx.nspanel_update_screen, name='htmx_nspanel_update_screen'),
    path('htmx/nspanels/<int:nspanel_id>/update_firmware', htmx.nspanel_update_firmware, name='htmx_nspanel_update_firmware'),
    path('htmx/nspanels/<int:nspanel_id>/accept_register_request', htmx.nspanel_accept_register_request, name='htmx_nspanel_accept_register_request'),
    path('htmx/nspanels/<int:nspanel_id>/deny_register_request', htmx.nspanel_deny_register_request, name='htmx_nspanel_deny_register_request'),
    path('htmx/nspanels/<int:nspanel_id>/delete', htmx.nspanel_delete, name='htmx_nspanel_delete'),
    path('htmx/partial/select_weather_location', htmx.select_weather_location, name='htmx_partial_select_weather_location'),
    path('htmx/partial/select_weather_outside_temperature_sensor', htmx.select_weather_outside_temperature_sensor, name='htmx_partial_select_weather_outside_temperature_sensor'),
    path('htmx/partial/select_weather_outside_temperature_sensor_provider', htmx.select_weather_outside_temperature_sensor_provider, name='htmx_partial_select_weather_outside_temperature_sensor_provider'),
    path('htmx/partial/select_weather_outside_temperature_sensor_from_list/<str:entity_source>', htmx.select_weather_outside_temperature_sensor_from_list, name='htmx_partial_select_weather_outside_temperature_sensor_from_list'),
    path('htmx/interface/theme', htmx.interface_theme, name='htmx_interface_theme'),

    # HTMX initial setup URLs
    path('htmx/initial_setup/welcome', htmx.initial_setup_welcome, name='htmx_initial_setup_welcome'),
    path('htmx/initial_setup/manager_settings', htmx.initial_setup_manager_settings, name='htmx_initial_setup_manager_settings'),
    path('htmx/initial_setup/mqtt_settings', htmx.initial_setup_mqtt_settings, name='htmx_initial_setup_mqtt_settings'),
    path('htmx/initial_setup/home_assistant_settings', htmx.initial_setup_home_assistant_settings, name='htmx_initial_setup_home_assistant_settings'),
    path('htmx/initial_setup/openhab_settings', htmx.initial_setup_openhab_settings, name='htmx_initial_setup_openhab_settings'),
    path('htmx/initial_setup/finished', htmx.initial_setup_finished, name='htmx_initial_setup_finished'),

    # HTMX relay group URLs
    path('htmx/relay_groups/create_new_modal', htmx.relay_group_create_new_modal, name='htmx_modal_create_new_relay_group'),
    path('htmx/relay_groups/edit_modal/<int:relay_group_id>', htmx.relay_group_edit_modal, name='htmx_modal_edit_relay_group'),
    path('htmx/relay_groups/save', htmx.relay_group_save, name='htmx_save_relay_group'),
    path('htmx/relay_groups/delete/<int:relay_group_id>', htmx.relay_group_delete, name='htmx_delete_relay_group'),
    path('htmx/relay_groups/add_relay_modal/<int:relay_group_id>', htmx.relay_group_add_relay_modal, name='htmx_modal_add_relay_to_relay_group'),
    path('htmx/relay_groups/add_relay/<int:relay_group_id>', htmx.relay_group_add_relay, name='htmx_add_relay_to_relay_group'),
    path('htmx/relay_groups/remove_relay_binding/<int:relay_binding_id>', htmx.relay_group_remove_relay, name='htmx_remove_relay_from_relay_group'),

    # Components
    path('htmx/component/nspanel_status_header/<str:view>/<int:nspanel_id>', NSPanelHeader.as_view(), name="nspanel_status_header"),
    path('htmx/component/nspanel_status_text/<str:view>/<int:nspanel_id>', NSPanelStatusText.as_view(), name="nspanel_status_text"),
    path('htmx/component/nspanel_status_update_progress/<str:view>/<int:nspanel_id>', NSPanelUpdateProgress.as_view(), name="nspanel_status_update_progress"),
    path('htmx/component/nspanel_status_wifi_signal_strength/<str:view>/<int:nspanel_id>', NSPanelStatusWifiSignalStrength.as_view(), name="nspanel_status_wifi_signal_strength"),
    path('htmx/component/nspanel_status_ram_usage/<str:view>/<int:nspanel_id>', NSPanelStatusRamUsage.as_view(), name="nspanel_status_ram_usage"),
    path('htmx/component/nspanel_status_temperature/<str:view>/<int:nspanel_id>', NSPanelStatusTemperature.as_view(), name="nspanel_status_temperature"),
    path('htmx/component/nspanel_visit_link/<str:view>/<int:nspanel_id>', NSPanelVisitLink.as_view(), name="nspanel_visit_link"),
    path('htmx/component/nspanel_reboot_button/<str:view>/<int:nspanel_id>', NSPanelRebootButton.as_view(), name="nspanel_reboot_button"),
    path('htmx/component/nspanel_warnings/<str:view>/<int:nspanel_id>', NSPanelWarnings.as_view(), name="nspanel_warnings"),
    path('htmx/component/nspanel_status_badge/<str:view>/<int:nspanel_id>', NSPanelStatusBadge.as_view(), name="nspanel_status_badge"),
    path('htmx/component/room_entities_pages_view/<str:view>/<int:room_id>', NSPanelRoomEntitiesPages.as_view(), name="nspanel_room_entities_pages"),
]
