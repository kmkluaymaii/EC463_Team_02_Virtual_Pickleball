import socket
import time

UDP_IP = "10.193.72.117"  # Replace with your ESP32 IP
UDP_PORT = 4210

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

while True:
    pattern = input("Enter pattern number (0–116): ")
    msg = f"vibrate:{pattern}"
    sock.sendto(msg.encode(), (UDP_IP, UDP_PORT))
    print(f"Sent: {msg}")
    time.sleep(1)
