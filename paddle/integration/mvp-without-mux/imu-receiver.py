import socket
import threading

ESP32_IP = "10.193.244.53"  # ESP32 IP
IMU_PORT = 5005             # ESP32 sending IMU data to this port

# Socket to listen for IMU data
imu_sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
imu_sock.bind(("", IMU_PORT))  # Listen on local port

def listen_imu():
    print(f"Listening for IMU data on UDP port {IMU_PORT}...")
    while True:
        try:
            data, addr = imu_sock.recvfrom(1024)
            print(f"📥 IMU from {addr}: {data.decode().strip()}")
        except Exception as e:
            print("⚠️ IMU receive error:", e)

# Run listener in background thread
threading.Thread(target=listen_imu, daemon=True).start()

# Keep script running
while True:
    pass
