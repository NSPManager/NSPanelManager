from django.contrib import admin
from django.urls import path, include

from . import views, api

urlpatterns = [
    # Web routes
    path('', views.index, name='index'),
    path('rooms/<int:room_id>/', views.edit_room, name='edit_room'),
    path('rooms', views.rooms, name='rooms'),
    path('save_new_room', views.save_new_room, name='save_new_room'),
    # Below are API routes
    path('api/register_nspanel', api.register_nspanel, name='register_nspanel'),
]
