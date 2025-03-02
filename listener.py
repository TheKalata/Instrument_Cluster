import socket

HOST = "0.0.0.0"
PORT = 5005

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind((HOST, PORT))

QT_IP = "127.0.0.1"
QT_PORT = 5006

print("Listening for data from BeamNG...")

while True:
    data, addr = sock.recvfrom(1024)
    data = data.decode().split(",")

    speed_kmh = float(data[0])
    rpm = int(float(data[1]))
    left_blinker = 1 if data[2].lower() == "true" else 0
    right_blinker = 1 if data[3].lower() == "true" else 0
    low_beam = int(data[4])
    high_beam = int(data[5])

    print(f"Speed: {speed_kmh:.2f} km/h, RPM: {rpm}, "
          f"L: {left_blinker}, R: {right_blinker}, Low: {low_beam}, High: {high_beam}")
          
    telemetry_data = f"{speed_kmh},{rpm},{left_blinker},{right_blinker},{low_beam},{high_beam}"
    
    send_sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    send_sock.sendto(telemetry_data.encode(), (QT_IP, QT_PORT))
    send_sock.close()
