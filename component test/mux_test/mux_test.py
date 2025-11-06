import socket

ESP32_IP = "192.168.0.158"   # Replace with your ESP32 IP
ESP32_PORT = 4210

# Create UDP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

print(f"🔗 Connected to ESP32 at {ESP32_IP}:{ESP32_PORT}")
print("Enter MUX channel number (0/1) to test each motor.")
print("Type 'q' to quit.\n")

while True:
    channel = input("Enter MUX channel (0/1): ").strip()
    if channel.lower() == 'q':
        break
    if not channel.isdigit() or not (0 <= int(channel) <= 7):
        print("⚠️ Invalid input. Must be an integer between 0–7.")
        continue

    message = f"channel:{channel}"
    print(f"📤 Sending: {message}")
    sock.sendto(message.encode(), (ESP32_IP, ESP32_PORT))

print("\n👋 Exiting...")
sock.close()