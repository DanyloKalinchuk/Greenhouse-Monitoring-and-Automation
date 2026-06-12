from django.db import models

class Sensors(models.Model):
    org_id = models.IntegerField(unique=True)

    def __str__(self):
        return f"Sensor ID: {self.org_id}; Inner ID: {self.id}"
    
    class Meta:
        app_label = "greenhouse_web_app"

class SensorData(models.Model):
    sensor = models.ForeignKey(Sensors, on_delete=models.CASCADE)
    date = models.DateTimeField(auto_now_add=True)
    temperature = models.IntegerField()
    humidity = models.IntegerField()
    soil_moisture = models.IntegerField()
    co2 = models.IntegerField()

    def __str__(self):
        return f"{self.sensor} | {self.date}"
    
    class Meta:
        app_label = 'greenhouse_web_app'
