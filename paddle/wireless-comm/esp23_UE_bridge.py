import socket
import time

# config
PC_LISTEN_IP = "0.0.0.0"   # listen on all interfaces
PC_LISTEN_PORT = 5005      # ESP32 sends here

UE_HOST = "127.0.0.1"      # Unreal on same PC
UE_PORT = 6006

def main():
    print("starting ESP32 (WiFi) → UE bridge...")

    # socket to receive from ESP32
    rx = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    rx.bind((PC_LISTEN_IP, PC_LISTEN_PORT))
    rx.settimeout(1.0)

    # socket to send to Unreal
    tx = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

    print(f"listening for ESP32 UDP on {PC_LISTEN_IP}:{PC_LISTEN_PORT}")
    print(f"forwarding to UE on {UE_HOST}:{UE_PORT}")

    bro = 0

    try:
        while True:
            try:
                data, addr = rx.recvfrom(2048)
            except socket.timeout:
                continue

            try:
                text = data.decode("utf-8").strip()
            except UnicodeDecodeError:
                continue

            if not text:
                continue

            if bro > 50:
                values = text.split(",")
                if len(values) > 4:
                    values[4] = "1.0"
                    modified_text = ",".join(values)
                else:
                    modified_text = text

                print("wifi:", modified_text, "from", addr)
                tx.sendto(modified_text.encode("utf-8"), (UE_HOST, UE_PORT))
            else:
                bro += 1

    except KeyboardInterrupt:
        print("stopping bridge...")

    finally:
        rx.close()
        tx.close()

if __name__ == "__main__":
    main()
