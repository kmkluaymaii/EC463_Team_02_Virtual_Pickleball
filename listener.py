import socket
PORT = 4211 # listen on
ESP32_IP = "192.168.4.75" # REPLACE WITH THE IP PRINTED BY THE ESP32 PROGRAM in serial monitor
ESP32_PORT = 4210 # send to

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM) # internet as fuck, sock the gram
sock.bind(("0.0.0.0", PORT))
print(f"Listening on {PORT} ..., sending to {ESP32_IP}:{ESP32_PORT}")
while True:
	# listen
	sock.settimeout(1)
	try:
		data, addr = sock.recvfrom(2048)
		print(f"{addr} -> {data!r}")
	except socket.timeout:
		pass

	# send
	msg = input("enter your msg: ")
	sock.sendto(msg.encode(), (ESP32_IP, ESP32_PORT))
	