import time
from beamngpy import BeamNGpy, Scenario, Vehicle

# Път до BeamNG
beamng_home = r"D:\BeamNG\BeamNG.drive.v0.21.3.0\BeamNG.drive.v0.21.3.0"

# Свързваме се с BeamNG
bng = BeamNGpy('localhost', 64256, home=beamng_home)
bng.open()

# Създаваме сценарий и автомобил
scenario = Scenario('west_coast_usa', 'test_scenario')
vehicle = Vehicle('ego_vehicle', model='etk800', license='TEST')

scenario.add_vehicle(vehicle, pos=(-717, 101, 118), rot_quat=(0, 0, 1, 0))

# Зареждаме и стартираме
scenario.make(bng)
bng.load_scenario(scenario)
bng.start_scenario()

# ✅ Нов метод за закачане на електрически сензор:
#vehicle.sensors['electrics'] = {'type': 'Electrics'}
#sensors = vehicle.poll_sensors()
#print(sensors)  # Ще видиш структурата
#for key, sensor in vehicle.sensors.items():
#    print(f"Sensor: {key}, Type: {type(sensor)}")


time.sleep(2)  # Изчакваме малко за инициализация

# Основен цикъл за четене на скоростта
while True:
    sensors = bng.poll_sensors(vehicle)  # Взима данните
    electrics = sensors['electrics']

    # Извличане на нужните стойности
    speed_kmh = electrics['wheelspeed'] * 3.6  # Конвертиране в km/h
    rpm = electrics['rpm']
    left_blinker = electrics['left_signal']
    right_blinker = electrics['right_signal']
    low_beam = electrics['lowbeam']
    high_beam = electrics['highbeam']

    # Печат на стойностите
    print(f"Speed: {speed_kmh:.2f} km/h | RPM: {rpm} | L-Blinker: {left_blinker} | "
          f"R-Blinker: {right_blinker} | LowBeam: {low_beam} | HighBeam: {high_beam}")

    #sensors = bng.poll_sensors(vehicle)
    #print(sensors)  # Дебъгване
#
    #if 'electrics' in sensors:
    #    electrics = sensors['electrics']
    #    if 'wheelspeed' in electrics:
    #        speed = electrics['wheelspeed'] * 3.6  # m/s -> km/h
    #        print(f"🚗 Скорост: {speed:.2f} km/h")
    #    else:
    #        print("⚠ Няма wheelspeed в electrics!")
    #else:
    #    print("⚠ Няма данни за electrics!")
#
    time.sleep(1)  # Изчакваме 1 секунда преди следващото четене

bng.close()