from django.db import models
from rest_framework import serializers

class SensorData(models.Model):
    sensor_id = models.IntegerField()
    date = models.DateTimeField(auto_now_add=True)
    temperature = models.IntegerField()
    humidity = models.IntegerField()
    soil_moisture = models.IntegerField()
    co2 = models.IntegerField()

    def __str__(self):
        return f"{self.sensor_id} | {self.date}"
    
    class Meta:
        app_label = 'greenhouse_web_app'

class SensorDataSerializer(serializers.ModelSerializer):
    class Meta:
        model = SensorData
        fields = ['sensor_id', "date", "temperature", "humidity", "soil_moisture", "co2"]