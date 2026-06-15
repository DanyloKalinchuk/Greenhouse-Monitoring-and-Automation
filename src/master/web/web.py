import threading
from web_ipc import IPC, SensFrame

SOCKET_PATH = "/tmp/SCADA_SOCK"
REQ_DELAY_SEC = 300

class Web:
    _instance = None
    _lock = threading.Lock()
    _ipc_lock = threading.Lock()
    _ipc: IPC
    _ipc_req_thread: threading.Thread
    _ipc_request_event = threading.Event()
    _ipc_stop_event = threading.Event()

    def __init__(self):
        self._ipc = IPC(SOCKET_PATH)

        self._ipc_req_thread = threading.Thread(target=self._handle_request)
        self._ipc_req_thread.start()

    def __del__(self):
        self._ipc_stop_event.set()
        self._ipc_request_event.set()
        self._ipc_req_thread.join()

    @staticmethod
    def get_instance():
        if Web._instance == None:
            with Web._lock:
                if Web._instance == None:
                    Web._instance = Web()

        return Web._instance
    
    def _handle_request(self):
        from greenhouse_web_app.models import SensorData, Sensors

        while not self._ipc_stop_event:

            self._ipc_request_event.wait(REQ_DELAY_SEC)
            self._ipc_request_event.clear()

            with self._ipc_lock:
                sensors = self._ipc.request()

            for sensor in sensors:
                sensor_reg = Sensors.objects.filter(org_id=sensor.id)

                if (not sensor_reg.exists()):
                    sensor_reg = Sensors()
                    sensor_reg.org_id = sensor.id
                    sensor_reg.save()
                else:
                    sensor_reg = sensor_reg.first()

                sensor_record = SensorData()
                sensor_record.sensor = sensor_reg
                sensor_record.temperature = sensor.temp
                sensor_record.humidity = sensor.hum
                sensor_record.soil_moisture = sensor.moist
                sensor_record.co2 = sensor.co2

                sensor_record.save()

    def handle_config(self, temp_perf, temp_error, hum_perf, hum_error, moist_perf, moist_error, co2_perf, co2_error):
        with self._ipc_lock:
            self._ipc.configurate(temp_perf, temp_error, hum_perf, hum_error, 
                                  moist_perf, moist_error, co2_perf, co2_error)

