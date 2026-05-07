from  web_ipc import *

SOCKET_PATH = "/tmp/SCADA_SOCK"
ipc = IPC(SOCKET_PATH)
sens_data = ipc.request()

print(f"Number of active sensors: {len(sens_data)}\n")

for sensor in sens_data:
    print(f"\tID: {sensor.id}")
    print(f"\tTemperatur: {sensor.temp}")
    print(f"\tHumidity: {sensor.hum}")
    print(f"\tSoil Moisture: {sensor.moist}")
    print(f"\tCO2: {sensor.co2}\n")
