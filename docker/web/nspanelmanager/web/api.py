from django.http import HttpResponse
from django.shortcuts import render, redirect
from datetime import datetime
from django.views.decorators.csrf import csrf_exempt

from .models import NSPanel, Room


@csrf_exempt
def register_nspanel(request):
    """Update the already existing NSPanel OR create a new one"""
    new_panel = NSPanel.objects.filter(
        mac_address=request.POST['mac_address']).first()

    if not new_panel:
        new_panel = NSPanel()

    new_panel.friendly_name = request.POST['friendly_name']
    new_panel.mac_address = request.POST['mac_address']
    new_panel.last_seen = datetime.now()

    # If no room is set, select the first one as default
    new_panel.room = Room.objects.first()

    # Save the update/Create new panel
    new_panel.save()
    return HttpResponse('OK', status=200)
