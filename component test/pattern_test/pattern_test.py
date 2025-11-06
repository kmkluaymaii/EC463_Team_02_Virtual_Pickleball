import socket

ESP32_IP = "10.192.27.84"   # Replace with your ESP32 IP
ESP32_PORT = 4210

# === Map user input (1,2,3) to DRV2605 pattern numbers ===
pattern_map = {
    "1": 6,  
    "2": 20, 
    "3": 17,   
}

# === Setup UDP ===
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

print("🔗 Connected to ESP32 at", ESP32_IP)
print("Type a vibration level:")
print("  1 → Low")
print("  2 → Medium")
print("  3 → High")
print("  q → Quit\n")

while True:
    user_input = input("Enter (1/2/3): ").strip()

    if user_input.lower() == 'q':
        print("👋 Exiting...")
        break

    if user_input not in pattern_map:
        print("⚠️ Invalid input. Please enter 1, 2, or 3.")
        continue

    # Convert mapped pattern
    pattern_number = pattern_map[user_input]
    message = f"vibrate:{pattern_number}"

    if user_input == '1':
        print(f"📤 Sending pattern {user_input}  →  Low Intensity")
    if user_input == '2':
        print(f"📤 Sending pattern {user_input}  →  Medium Intensity")
    if user_input == '3':
        print(f"📤 Sending pattern {user_input}  →  High Intensity")

    sock.sendto(message.encode(), (ESP32_IP, ESP32_PORT))

sock.close()
