from django.db import models

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