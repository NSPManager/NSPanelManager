from django.contrib import admin
from django.urls import path, include

from . import views, api

urlpatterns = [
    # Web routes
    path('', views.index, name='index'),
    path('rooms', views.rooms, name='rooms'),
    path('rooms/<int:room_id>/', views.edit_room, name='edit_room'),
    path('save_room/<int:room_id>',
         views.update_room_form, name='update_room_form'),
    path('save_new_room', views.save_new_room, name='save_new_room'),
    path('save_new_firmware', views.save_new_firmware, name='save_new_firmware'),
    path('download_firmware', views.download_firmware, name='download_firmware'),
    # Below are API routes
    path('api/register_nspanel', api.register_nspanel, name='register_nspanel'),
]
