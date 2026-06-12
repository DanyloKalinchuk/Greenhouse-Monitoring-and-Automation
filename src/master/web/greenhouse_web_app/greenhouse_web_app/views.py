from django.shortcuts import render
from django.http import HttpResponse
from .models import SensorData, Sensors
from django.db.models import Avg, Max, Min
from django.db.models.functions import TruncDay, TruncHour
from django.core.serializers.json import DjangoJSONEncoder
import json

import sys
import os

REL_PATH = os.path.dirname(os.path.realpath(__file__))
sys.path.insert(0, os.path.join(REL_PATH, "../../"))

from web import Web

def home(request):
    return render(request, 'base.html')

def help(request):
    return render(request,  'help.html')

def sensors(request):
    sensors = Sensors.objects.all()

    return render(request, 'sensors.html', {'sensors': sensors})

def records(request):
    records = SensorData.objects.all().order_by('-date')

    if request.method == 'GET':
        records_chart_avg = records.order_by().annotate(day=TruncDay("date")).values("day").annotate(
            temp_avg = Avg("temperature"),
            hum_avg = Avg("humidity"),
            moist_avg = Avg("soil_moisture"),
            co2_avg = Avg("co2")
        )

        records_chart_min_max = records.order_by().annotate(day=TruncDay("date")).values("day").annotate(
            temp_min = Min("temperature"),
            temp_max = Max("temperature"),

            hum_min = Min("humidity"),
            hum_max = Max("humidity"),

            moist_min = Min("soil_moisture"),
            moist_max = Max("soil_moisture"),

            co2_min = Min("co2"),
            co2_max = Max("co2"),
        )

        records_chart_avg = json.dumps(list(records_chart_avg), cls=DjangoJSONEncoder)
        records_chart_min_max = json.dumps(list(records_chart_min_max), cls=DjangoJSONEncoder)

        return render(request, 'records_base.html', {
            'records': records,
            'chart_avg': records_chart_avg, 
            'chart_min_max': records_chart_min_max,
        })
    
    elif request.method == 'POST':
        info = ""
        records_chart_avg = None
        records_chart_min_max = None

        if request.POST.get("form_input_options") == "id":
            sensor_id = request.POST.get("search_input")
            records = records.filter(sensor_id=sensor_id)
            info = f"ID: {sensor_id}"

            records_chart_avg = records.order_by().annotate(day=TruncDay("date")).values("day").annotate(
                temp_avg = Avg("temperature"),
                hum_avg = Avg("humidity"),
                moist_avg = Avg("soil_moisture"),
                co2_avg = Avg("co2")
            )

            records_chart_min_max = records.order_by().annotate(day=TruncDay("date")).values("day").annotate(
                temp_min = Min("temperature"),
                temp_max = Max("temperature"),

                hum_min = Min("humidity"),
                hum_max = Max("humidity"),

                moist_min = Min("soil_moisture"),
                moist_max = Max("soil_moisture"),

                co2_min = Min("co2"),
                co2_max = Max("co2"),
            )

        elif request.POST.get("form_input_options") == "date":
            date = request.POST.get("search_input")
            records = records.filter(date__date=date)
            info = f"Date: {date}"

            records_chart_avg = records.order_by().annotate(hour=TruncHour("date")).values("hour").annotate(
                temp_avg = Avg("temperature"),
                hum_avg = Avg("humidity"),
                moist_avg = Avg("soil_moisture"),
                co2_avg = Avg("co2")
            )

            records_chart_min_max = records.order_by().annotate(hour=TruncHour("date")).values("hour").annotate(
                temp_min = Min("temperature"),
                temp_max = Max("temperature"),

                hum_min = Min("humidity"),
                hum_max = Max("humidity"),

                moist_min = Min("soil_moisture"),
                moist_max = Max("soil_moisture"),

                co2_min = Min("co2"),
                co2_max = Max("co2"),
            )

        records_chart_avg = json.dumps(list(records_chart_avg), cls=DjangoJSONEncoder)
        records_chart_min_max = json.dumps(list(records_chart_min_max), cls=DjangoJSONEncoder)
            
        return render(request, 'records_base.html', {
            'filtered': True, 
            'records': records, 
            'info': info, 
            'chart_avg': records_chart_avg, 
            'chart_min_max': records_chart_min_max,
        })

def config(request):
    parameters = [
        ["temp", 20, 5],
        ["hum", 50, 20],
        ["moist", 70, 10],
        ["co2", 30, 10],
    ]

    if request.method == 'POST':
        web_instance = None

        try:
            web_instance = Web.get_instance()
            web_instance.handle_config(
                request.POST.get("temp_perf"),
                request.POST.get("temp_error"),
                request.POST.get("hum_perf"),
                request.POST.get("hum_error"),
                request.POST.get("moist_perf"),
                request.POST.get("moist_error"),
                request.POST.get("co2_perf"),
                request.POST.get("co2_error")
            )
        except:
            print("Failed to get the Web instance")

    return render(request, 'config.html', {'parameters': parameters})