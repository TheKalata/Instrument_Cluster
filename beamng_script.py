import time
from beamngpy import BeamNGpy, Scenario, Vehicle
from beamngpy.sensors import Electrics

# Path to BeamNG
beamng_home = r"D:\BeamNG\BeamNG.drive.v0.21.3.0\BeamNG.drive.v0.21.3.0"

# Connect to BeamNG
bng = BeamNGpy('localhost', 64256, home=beamng_home)
bng.open()

# Create a scenario and vehicle
scenario = Scenario('west_coast_usa', 'test_scenario')
vehicle = Vehicle('ego_vehicle', model='etk800', license='PYTHON')

# Attach the electrics sensor correctly by instantiating it
electrics_sensor = Electrics()  # Create an Electrics sensor object
vehicle.attach_sensor('electrics', electrics_sensor)  # Attach the sensor to the vehicle

# Add the vehicle to the scenario
scenario.add_vehicle(vehicle, pos=(-717, 101, 118), rot_quat=(0, 0, 1, 0))

# Prepare scenario
scenario.make(bng)
bng.load_scenario(scenario)
bng.start_scenario()

time.sleep(2)  # Wait a bit for initialization

# Main loop to extract telemetry data
while True:
    # Poll vehicle sensors (this is the correct way to get telemetry data)
    sensors = bng.poll_sensors(vehicle)
    
    # Extract necessary telemetry data from sensors
    if 'electrics' in sensors:
        electrics = sensors['electrics']

        # Extract speed, RPM, and other data from the electrics sensor
        speed_mps = electrics['wheelspeed']  # Speed in m/s
        speed_kmh = speed_mps * 3.6  # Convert to km/h
        rpm = electrics['rpm']  # RPM value
        left_blinker = electrics['left_signal']  # Left blinker state (1 or 0)
        right_blinker = electrics['right_signal']  # Right blinker state (1 or 0)
        low_beam = electrics['lowbeam']  # Low beam state (1 or 0)
        high_beam = electrics['highbeam']  # High beam state (1 or 0)

        # Print out telemetry data
        print(f"Speed: {speed_kmh:.2f} km/h | RPM: {rpm} | L-Blinker: {left_blinker} | "
              f"R-Blinker: {right_blinker} | LowBeam: {low_beam} | HighBeam: {high_beam}")

    time.sleep(1)  # Wait for 1 second before next reading

# Close the connection to BeamNG
bng.close()
