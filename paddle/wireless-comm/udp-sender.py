import socket
import time

UDP_IP = "10.193.31.240"   # replace with ESP32’s IP from Serial Monitor
UDP_PORT = 4210

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

while True:
    message = "Hello stupids!"
    sock.sendto(message.encode(), (UDP_IP, UDP_PORT))
    print(f"Sent: {message}")
    time.sleep(2)