import socket
import time

ESP32_IP = "10.192.27.84"  # Replace with your ESP32 IP
ESP32_PORT = 4210

# === Create UDP socket ===
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

print("🔗 Connected to ESP32 at", ESP32_IP)
print("Enter 'q' anytime to quit.\n")

while True:
    try:
        # Ask user for channel
        channel = input("Enter vibrator number (1 or 2): ").strip()
        if channel.lower() == 'q':
            break
        if channel not in ['0', '1', '2', '3', '4', '5']:
            print("⚠️ Please enter 1 or 2 for channel.")
            continue

        # Ask user for pattern number
        pattern = input("Enter vibration pattern (0–116): ").strip()
        if pattern.lower() == 'q':
            break
        if not pattern.isdigit() or not (0 <= int(pattern) <= 116):
            print("⚠️ Pattern must be a number between 0–116.")
            continue

        # Construct message
        message = f"vibrate:{channel}:{pattern}"
        print(f"📤 Sending: {message}")

        # Send UDP packet
        sock.sendto(message.encode(), (ESP32_IP, ESP32_PORT))

    except KeyboardInterrupt:
        print("\n👋 Exiting...")
        break

sock.close()