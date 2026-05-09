from django.shortcuts import render
from django.http import HttpResponse
from models import SensorData

def data_by_sensor_id(request, sensor_id):
    records = SensorData.objects.filter(sensor_id=sensor_id).order_by('-date')

    return render(request, 'records/records_list.html', {'records': records, 'sensor_id': sensor_id})