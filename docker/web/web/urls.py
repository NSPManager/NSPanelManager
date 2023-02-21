"""nspanel URL Configuration

The `urlpatterns` list routes URLs to views. For more information please see:
    https://docs.djangoproject.com/en/1.10/topics/http/urls/
Examples:
Function views
    1. Add an import:  from my_app import views
    2. Add a URL to urlpatterns:  url(r'^$', views.home, name='home')
Class-based views
    1. Add an import:  from other_app.views import Home
    2. Add a URL to urlpatterns:  url(r'^$', Home.as_view(), name='home')
Including another URLconf
    1. Import the include() function: from django.conf.urls import url, include
    2. Add a URL to urlpatterns:  url(r'^blog/', include('blog.urls'))
"""
from django.conf.urls import url
from django.contrib import admin
from django.views.decorators.csrf import csrf_exempt
from . import views, api

urlpatterns = [
    # Web routes
    url(r'^$', views.index, name='index'),
    url('rooms', views.rooms, name='rooms'),
    url('save_new_room', views.save_new_room, name='save_new_room'),
    # Below are API routes
    url('api/register_nspanel', csrf_exempt(api.register_nspanel),
        name='register_nspanel'),
    # ADMIN
    url(r'^admin/', admin.site.urls),
]
