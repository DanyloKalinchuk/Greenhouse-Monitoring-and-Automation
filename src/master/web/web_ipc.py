import socket

CONF_MSG = 1
REQ_MSG = 2

class SensFrame:
    id: int
    temp: int
    hum: int
    moist: int
    co2: int

class IPC:
    sfd = None

    def __init__(self, socket_path):
        self.sfd = socket.socket(family=socket.AF_UNIX, type=socket.SOCK_STREAM)
        self.sfd.connect(socket_path)

    def __del__(self):
        self.sfd.close()

    def configurate(self, temp_perf, temp_error, hum_perf, hum_error, moist_perf, 
                    moist_error, co2_perf, co2_error):
        self.sfd.sendall(CONF_MSG.to_bytes(2, byteorder='little'))

        self.sfd.sendall(int(temp_perf).to_bytes(2, byteorder='little'))
        self.sfd.sendall(int(temp_error).to_bytes(2, byteorder='little'))

        self.sfd.sendall(int(hum_perf).to_bytes(2, byteorder='little'))
        self.sfd.sendall(int(hum_error).to_bytes(2, byteorder='little'))

        self.sfd.sendall(int(moist_perf).to_bytes(2, byteorder='little'))
        self.sfd.sendall(int(moist_error).to_bytes(2, byteorder='little'))

        self.sfd.sendall(int(co2_perf).to_bytes(2, byteorder='little'))
        self.sfd.sendall(int(co2_error).to_bytes(2, byteorder='little'))

    def request(self):
        self.sfd.sendall(REQ_MSG.to_bytes(2, byteorder='little'))
        active_sensors = int.from_bytes(self.sfd.recv(2), byteorder='little')

        sens_data = []
        for _ in range(active_sensors):
            frame = SensFrame()

            frame.id = int.from_bytes(self.sfd.recv(2), byteorder='little')
            frame.temp = int.from_bytes(self.sfd.recv(2), byteorder='little')
            frame.hum = int.from_bytes(self.sfd.recv(2), byteorder='little')
            frame.moist = int.from_bytes(self.sfd.recv(2), byteorder='little')
            frame.co2 = int.from_bytes(self.sfd.recv(2), byteorder='little')

            sens_data.append(frame)

        return sens_data