from django.http import HttpResponse
from django.shortcuts import render, redirect
from datetime import datetime

from .models import NSPanel, Room


def register_nspanel(request):
    """Update the already existing NSPanel OR create a new one"""
    new_panel = NSPanel.objects.filter(
        mac_address=request.POST['mac_address']).first()

    if not new_panel:
        new_panel = NSPanel()

    new_panel.friendly_name = request.POST['friendly_name']
    new_panel.last_seen = datetime.now()
    new_panel.save()
    return HttpResponse('OK', status=200)
