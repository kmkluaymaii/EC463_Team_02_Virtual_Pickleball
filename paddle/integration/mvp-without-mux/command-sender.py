import socket

ESP32_IP = "10.193.244.53"  # ESP32 IP
CONTROL_PORT = 4210          # ESP32 UDP input port

# Socket for sending commands
ctrl_sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

print("=== ESP32 UDP Control Terminal ===")
print("Commands:")
print("  1          -> Win combo (green)")
print("  2          -> Lose combo (red)")
print("  beep f d   -> Beep frequency f (Hz), duration d (ms)")
print("  haptic c e -> Haptic channel c, effect e")
print("  combo f d c e color -> Custom combo")
print("  led color  -> LED color (green/red/blue)")
print("  exit       -> Quit terminal")
print()

while True:
    cmd = input("Enter command: ").strip().lower()
    if not cmd:
        continue

    if cmd == "exit":
        break

    msg = None
    parts = cmd.split()

    if cmd == "1":
        msg = "combo:440:20:0:47:green"
    elif cmd == "2":
        msg = "combo:50:50:0:0:red"
    elif parts[0] == "beep" and len(parts) == 3:
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

    try:
        ctrl_sock.sendto(msg.encode(), (ESP32_IP, CONTROL_PORT))
        print(f"📡 Sent: {msg}")
    except Exception as e:
        print("⚠️ UDP send error:", e)
