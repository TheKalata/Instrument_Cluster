import time
import socket
from beamngpy import BeamNGpy, Scenario, Vehicle
from beamngpy.sensors import Electrics

beamng_home = r"D:\BeamNG\BeamNG.drive.v0.21.3.0\BeamNG.drive.v0.21.3.0"

RASPBERRY_IP = "192.168.62.203"
PORT = 5005

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

bng = BeamNGpy('localhost', 64256, home=beamng_home)
bng.open()

scenario = Scenario('west_coast_usa', 'test_scenario')
vehicle = Vehicle('ego_vehicle', model='etk800', license='PYTHON')

electrics_sensor = Electrics()  
vehicle.attach_sensor('electrics', electrics_sensor)

scenario.add_vehicle(vehicle, pos=(-717, 101, 118), rot_quat=(0, 0, 1, 0))

scenario.make(bng)
bng.load_scenario(scenario)
bng.start_scenario()

time.sleep(2)  

while True:
    sensors = bng.poll_sensors(vehicle)
    
    if 'electrics' in sensors:
        electrics = sensors['electrics']

        speed_kmh = electrics.get('wheelspeed', 0) * 3.6
        rpm = electrics.get('rpm', 0)
        left_blinker = electrics.get('left_signal', 0)
        right_blinker = electrics.get('right_signal', 0)
        low_beam = electrics.get('lowbeam', 0)
        high_beam = electrics.get('highbeam', 0)

        data_str = f"{speed_kmh},{rpm},{left_blinker},{right_blinker},{low_beam},{high_beam}"

        sock.sendto(data_str.encode(), (RASPBERRY_IP, PORT))

        print(f"Sent: {data_str}") 

    time.sleep(0.01)  

bng.close()
