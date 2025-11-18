import socket

ESP32_IP = "192.168.4.85"  # Replace with your ESP32 IP
ESP32_PORT = 4210

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

print("ESP32 UDP Control Terminal\n")
print("Enter 1 for win")
print("Enter 2 for lose")
# print("  beep <freq> <duration>       - Play beep (e.g., beep 1000 500)")
# print("  haptic <channel> <effect>    - Trigger haptic (e.g., haptic 0 47)")
# print("  combo <freq> <dur> <ch> <ef> <color> - Beep + haptic (e.g., combo 440 20 0 47 green)")
# print("  led <color>                  - Change LED color (e.g., led green)")
# print("  imu                          - Request IMU data\n")

while True:
    cmd = input("Enter command: ").strip().lower()
    if not cmd:
        continue  # skip empty input
    
    if cmd == "1":
        cmd = "combo 440 20 0 47 green"
    else:
        cmd = "combo 50 50 0 0 red"
        
    parts = cmd.split()

    # ---- BEEP ----
    if parts[0] == "beep" and len(parts) == 3:
        msg = f"beep:{parts[1]}:{parts[2]}"
        sock.sendto(msg.encode(), (ESP32_IP, ESP32_PORT))
        print(f"📡 Sent beep {parts[1]}Hz {parts[2]}ms")

    # ---- HAPTIC ----
    elif parts[0] == "haptic" and len(parts) == 3:
        msg = f"haptic:{parts[1]}:{parts[2]}"
        sock.sendto(msg.encode(), (ESP32_IP, ESP32_PORT))
        print(f"📡 Sent haptic ch={parts[1]} effect={parts[2]}")

    # ---- COMBO ----
    elif parts[0] == "combo" and len(parts) == 6:
        msg = f"combo:{parts[1]}:{parts[2]}:{parts[3]}:{parts[4]}:{parts[5]}"
        sock.sendto(msg.encode(), (ESP32_IP, ESP32_PORT))
        print(f"📡 Sent combo: {parts[1]}Hz {parts[2]}ms + haptic ch={parts[3]} effect={parts[4]} color = {parts[5]}")

    # ---- LED ----
    elif parts[0] == "led" and len(parts) == 2:
        if parts[1] in ["green", "red", "blue"]:
            msg = f"led:{parts[1]}"
            sock.sendto(msg.encode(), (ESP32_IP, ESP32_PORT))
            print(f"📡 Sent LED color: {parts[1]}")
        else:
            print("❌ Invalid color. Use: green, red, or blue")

    # ---- IMU ----
    elif parts[0] == "imu":
        msg = "imu"
        sock.sendto(msg.encode(), (ESP32_IP, ESP32_PORT))
        print("📡 Sent IMU request")

    else:
        print("❌ Invalid command")