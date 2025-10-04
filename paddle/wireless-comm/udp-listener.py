import socket

UDP_IP = "0.0.0.0"   # Listen on all interfaces
UDP_PORT = 4210

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind((UDP_IP, UDP_PORT))

print(f"Listening on port {UDP_PORT}...")

while True:
    data, addr = sock.recvfrom(1024)  # Buffer size 1024 bytes
    print(f"Received from {addr}: {data.decode()}")