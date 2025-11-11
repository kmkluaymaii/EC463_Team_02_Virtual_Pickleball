#include <WiFi.h>
#include <WiFiUdp.h>

const char* WIFI_SSID = "ssid"; // REPLACE WITH WIFI NAME
const char* WIFI_PASS = "pass"; // REPLACE WITH WIFI PASSWORD

const char* PC_IP = "192.168.4.60";  // PC's IP
const uint16_t PC_PORT = 4211;

const uint16_t ESP32_PORT = 4210;    // ESP32 listening port

WiFiUDP udp;

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Connected, ESP32 IP: ");
  Serial.println(WiFi.localIP());

  udp.begin(ESP32_PORT);
  Serial.printf("Listening on UDP port %d\n", ESP32_PORT);
}

void loop() {
  // send periodic message to PC
  static unsigned long lastSend = 0;
  if (millis() - lastSend > 1000) {
    lastSend = millis();
    udp.beginPacket(PC_IP, PC_PORT);
    udp.print("hello from ESP32");
    udp.endPacket();
  }

  // check for incoming data
  int packetSize = udp.parsePacket();
  if (packetSize) {
    char buf[256];
    int len = udp.read(buf, sizeof(buf) - 1);
    if (len > 0) buf[len] = 0;
    Serial.printf("Received from %s:%d -> %s\n",
                  udp.remoteIP().toString().c_str(),
                  udp.remotePort(), buf);
  }
}
