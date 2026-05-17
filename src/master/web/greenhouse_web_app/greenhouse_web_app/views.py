from django.shortcuts import render
from django.http import HttpResponse
from .models import SensorData

import sys
import os

REL_PATH = os.path.dirname(os.path.realpath(__file__))
sys.path.insert(0, os.path.join(REL_PATH, "../../"))

from web import Web

def home(request):
    return render(request, 'base.html')

def records(request):
    if request.method == 'GET':
        records = SensorData.objects.order_by('-date')

        return render(request, 'records_base.html', {'records': records})
    elif request.method == 'POST':
        if request.POST.get("form_input_options") == "id":
            sensor_id = request.POST.get("search_input")
            records = SensorData.objects.filter(sensor_id=sensor_id).order_by('-date')
            info = f"ID: {sensor_id}"
            
        return render(request, 'records_base.html', {'filtered': True, 'records': records, 'info': info})

def config(request):
    parameters = [
        ["temp", 20, 5],
        ["hum", 50, 20],
        ["moist", 70, 10],
        ["co2", 30, 10],
    ]

    if request.method == 'POST':
        pass

        Web.get_instance().handle_config(
            request.POST.get("temp_perf"),
            request.POST.get("temp_error"),
            request.POST.get("hum_perf"),
            request.POST.get("hum_error"),
            request.POST.get("moist_perf"),
            request.POST.get("moist_error"),
            request.POST.get("co2_perf"),
            request.POST.get("co2_error")
        )

    return render(request, 'config.html', {'parameters': parameters})