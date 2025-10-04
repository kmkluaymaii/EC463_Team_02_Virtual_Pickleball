#include <WiFi.h>
#include <WiFiUdp.h>

const char* ssid = "BU Guest (unencrypted)";
const char* password = "";

// Replace this with your PC's local IP address
const char* udpAddress = "168.122.149.186";  
const int udpPort = 4210;

WiFiUDP udp;

void setup() {
  Serial.begin(115200);
  
  // Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi!");
  Serial.print("ESP32 IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  // Example message
  String message = "Hello from ESP32!";
  
  udp.beginPacket(udpAddress, udpPort);
  udp.print(message);
  udp.endPacket();
  
  Serial.println("Sent: " + message);

  delay(2000); // Send every 2 seconds
}