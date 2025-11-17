#include <WiFi.h>
#include <WiFiUdp.h>
#include <Wire.h>
#include <Adafruit_DRV2605.h>

// ==== WiFi Configuration ====

const char* ssid = 
const char* password = 

// ==== UDP Target (your PC) ====
const char* udpAddress = "10.193.8.32";
const int udpPort = 4210;

WiFiUDP udp;

#define TCA_ADDR 0x70
#define DRV_ADDR 0x5A
#define SDA_PIN 8
#define SCL_PIN 9

Adafruit_DRV2605 drv;

void tcaSelect(uint8_t channel) {
  if (channel > 7) return;
  Wire.beginTransmission(TCA_ADDR);
  Wire.write(1 << channel);
  Wire.endTransmission();
}

bool devicePresent(uint8_t addr) {
  Wire.beginTransmission(addr);
  return (Wire.endTransmission() == 0);
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n✅ Connected to WiFi!");
  Serial.print("ESP32 IP Address: ");
  Serial.println(WiFi.localIP());

  // Start I2C
  Wire.begin(SDA_PIN, SCL_PIN);
  delay(100);

  // Initialize UDP
  udp.begin(udpPort);
  Serial.printf("Listening for UDP messages on port %d\n", udpPort);

  Serial.println("Ready to receive MUX channel commands!");
}

void loop() {
  int packetSize = udp.parsePacket();
  if (packetSize) {
    char incomingPacket[255];
    int len = udp.read(incomingPacket, 255);
    if (len > 0) incomingPacket[len] = 0;
    String message = String(incomingPacket);
    Serial.printf("📩 Received UDP message: %s\n", incomingPacket);

    // Expected format: channel:<number>
    if (message.startsWith("channel:")) {
      int colonIndex = message.indexOf(':');
      int channel = message.substring(colonIndex + 1).toInt();

      if (channel < 0 || channel > 7) {
        Serial.println("⚠️ Invalid channel number. Must be between 0–7.");
        return;
      }

      Serial.printf("🔀 Selecting MUX channel %d...\n", channel);
      tcaSelect(channel);

      // Initialize DRV2605L on this channel
      if (devicePresent(DRV_ADDR)) {
        if (drv.begin()) {
          drv.selectLibrary(1);
          drv.setMode(DRV2605_MODE_INTTRIG);
          Serial.println("✅ DRV2605L detected and ready!");

          // Fixed pattern = 1
          int pattern = 1;
          drv.setWaveform(0, pattern);
          drv.setWaveform(1, 0);
          drv.go();
          Serial.println("💥 Vibration triggered with pattern 1!");
        } else {
          Serial.println("❌ Failed to initialize DRV2605L on this channel.");
        }
      } else {
        Serial.println("❌ No DRV2605L detected on this channel.");
      }
    } else {
      Serial.println("⚠️ Invalid message format. Use channel:<number>");
    }
  }
}
