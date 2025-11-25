import socket
import threading

ESP32_IP = "192.168.4.85"  # Replace with your ESP32 IP
ESP32_PORT = 4210

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind(("", 0))  # Bind to any free port to receive messages

# =========================
# Function to listen for IMU/other UDP messages
# =========================
def listen_udp():
    while True:
        try:
            data, addr = sock.recvfrom(1024)
            print(f"📥 Received from {addr}: {data.decode().strip()}")
        except Exception as e:
            print("⚠️ UDP receive error:", e)

# Start listener in background thread
threading.Thread(target=listen_udp, daemon=True).start()

# =========================
# Main input loop
# =========================
print("ESP32 UDP Control Terminal\n")
print("Enter 1 for win")
print("Enter 2 for lose")
print("Enter 'imu' to request IMU data")

while True:
    cmd = input("Enter command: ").strip().lower()
    if not cmd:
        continue  # skip empty input
    
    if cmd == "1":
        msg = "combo:440:20:0:47:green"
    elif cmd == "2":
        msg = "combo:50:50:0:0:red"
    elif cmd == "imu":
        msg = "imu"
    else:
        parts = cmd.split()
        if parts[0] == "beep" and len(parts) == 3:
            msg = f"beep:{parts[1]}:{parts[2]}"
        elif parts[0] == "haptic" and len(parts) == 3:
            msg = f"haptic:{parts[1]}:{parts[2]}"
        elif parts[0] == "combo" and len(parts) == 6:
            msg = f"combo:{parts[1]}:{parts[2]}:{parts[3]}:{parts[4]}:{parts[5]}"
        elif parts[0] == "led" and len(parts) == 2 and parts[1] in ["green", "red", "blue"]:
            msg = f"led:{parts[1]}"
        else:
            print("❌ Invalid command")
            continue
    
    sock.sendto(msg.encode(), (ESP32_IP, ESP32_PORT))
    print(f"📡 Sent: {msg}")
