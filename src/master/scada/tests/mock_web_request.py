import socket

SOCKET_PATH = "/tmp/SCADA_SOCK"
REQ_MSG = 2

sfd = socket.socket(family=socket.AF_UNIX, type=socket.SOCK_STREAM)
sfd.connect(SOCKET_PATH)

print("Socket connected. Requesting data...\n")
sfd.sendall(REQ_MSG.to_bytes(2, byteorder='little'))

active_sensors = int.from_bytes(sfd.recv(2), byteorder='little')
print(f"Number of active sensors: {active_sensors}\n")

for _ in range(active_sensors):
    buff = int.from_bytes(sfd.recv(2), byteorder='little')
    print(f"Sensor ID: {buff}")

    buff = int.from_bytes(sfd.recv(2), byteorder='little')
    print(f"\tTemperature: {buff}")

    buff = int.from_bytes(sfd.recv(2), byteorder='little')
    print(f"\tHumidity: {buff}")

    buff = int.from_bytes(sfd.recv(2), byteorder='little')
    print(f"\tSoil Moisture: {buff}")

    buff = int.from_bytes(sfd.recv(2), byteorder='little')
    print(f"\tCO2: {buff}")

sfd.close()
