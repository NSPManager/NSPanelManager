from django.contrib import admin
from django.urls import path, include

from . import views, api

urlpatterns = [
    # Web routes
    path('', views.index, name='index'),
    path('rooms', views.rooms, name='rooms'),
    path('rooms/order', views.rooms_order, name='rooms_order'),
    path('moveRoomUp/<int:room_id>', views.move_room_up, name='move_room_up'),
    path('moveRoomDown/<int:room_id>',
         views.move_room_down, name='move_room_down'),
    path('rooms/<int:room_id>/', views.edit_room, name='edit_room'),
    path('save_room/<int:room_id>',
         views.update_room_form, name='update_room_form'),
    path('save_new_room', views.save_new_room, name='save_new_room'),
    path('save_new_firmware', views.save_new_firmware, name='save_new_firmware'),
    path('save_new_data_file', views.save_new_data_file, name='save_new_data_file'),
    path('save_new_tft_file', views.save_new_tft_file, name='save_new_tft_file'),
    path('download_firmware', views.download_firmware, name='download_firmware'),
    path('download_data_file', views.download_data_file, name='download_data_file'),
    path('download_tft', views.download_tft, name='download_tft'),
    path('checksum_firmware', views.checksum_firmware, name='checksum_firmware'),
    path('checksum_data_file', views.checksum_data_file, name='checksum_data_file'),
    # Below are API routes
    path('api/register_nspanel', api.register_nspanel, name='register_nspanel'),
]
