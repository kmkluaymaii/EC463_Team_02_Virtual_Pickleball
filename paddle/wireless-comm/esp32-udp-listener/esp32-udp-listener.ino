// ESP32 UDP listener (Arduino framework)
// Works with ESP32/ESP32-S3 boards in Arduino IDE / PlatformIO.

#include <WiFi.h>
#include <WiFiUdp.h>

// ---------- CONFIG ----------
const char* ssid     = "BU Guest (unencrypted)";
const char* password = "";
const unsigned int localPort = 4210; // port to listen on
// ----------------------------

WiFiUDP udp;
char incomingPacket[512]; // buffer for incoming packets

void setup() {
  Serial.begin(115200);
  delay(100);

  Serial.print("Connecting to WiFi ");
  Serial.print(ssid);
  WiFi.begin(ssid, password);

  // Wait for connection
  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    Serial.print(".");
    if (millis() - start > 150000) { // 15s timeout
      Serial.println("\nFailed to connect within 15s. Restarting...");
      ESP.restart();
    }
  }
  Serial.println("\nConnected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Start UDP
  if (udp.begin(localPort) == 1) {
    Serial.print("Listening for UDP on port ");
    Serial.println(localPort);
  } else {
    Serial.println("Failed to start UDP.");
  }
}

void loop() {
  int packetSize = udp.parsePacket();
  if (packetSize) {
    // read the packet into buffer (limit to buffer size-1 so we can null-terminate)
    int len = udp.read(incomingPacket, sizeof(incomingPacket) - 1);
    if (len < 0) len = 0;
    incomingPacket[len] = 0; // null-terminate for safe printing

    IPAddress remoteIp = udp.remoteIP();
    unsigned int remotePort = udp.remotePort();

    Serial.printf("Received %d bytes from %s:%u\n", len, remoteIp.toString().c_str(), remotePort);
    Serial.print("Payload (raw): ");
    Serial.println(incomingPacket);

    // Example: interpret payload as ASCII text and echo back
    String reply = String("ACK: ") + String(incomingPacket);
    udp.beginPacket(remoteIp, remotePort);
    udp.write((const uint8_t*)reply.c_str(), reply.length());
    udp.endPacket();
  }

  // do other tasks here...
  delay(10);
}