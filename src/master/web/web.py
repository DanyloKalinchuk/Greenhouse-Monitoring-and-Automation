import threading
import time
from web_ipc import IPC, SensFrame
from greenhouse_web_app.greenhouse_web_app.models import SensorData

SOCKET_PATH = "/tmp/SCADA_SOCK"
REQ_DELAY_SEC = 300

class Web:
    _instance = None
    _lock = threading.Lock()
    _ipc_lock = threading.Lock()
    _ipc: IPC
    _ipc_req_thread: threading.Thread
    _ipc_thread_on = False

    def __init__(self):
        self._ipc = IPC(SOCKET_PATH)

        self._ipc_req_thread = threading.Thread(target=self.handle_request)
        self._ipc_thread_on = True
        self._ipc_req_thread.start()

    def __del__(self):
        self._ipc_thread_on = False
        self._ipc_req_thread.join()

    @staticmethod
    def get_instance():
        if Web._instance == None:
            with Web._lock:
                if Web._instance == None:
                    Web._instance = Web()

        return Web._instance
    
    def handle_request(self):
        from greenhouse_web_app.greenhouse_web_app.models import SensorData

        while self._ipc_thread_on:
            time.sleep(REQ_DELAY_SEC)

            with self._ipc_lock:
                sensors = self._ipc.request()

            for sensor in sensors:
                sensor_record = SensorData()
                sensor_record.sensor_id = sensor.id
                sensor_record.temperature = sensor.temp
                sensor_record.humidity = sensor.hum
                sensor_record.soild_moisture = sensor.moist
                sensor_record.co2 = sensor.co2

                sensor_record.save()

