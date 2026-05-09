import sys
import os

REL_PATH = os.path.dirname(os.path.realpath(__file__))
sys.path.insert(0, os.path.join(REL_PATH, "../../web"))

from web_ipc import IPC, SensFrame

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
