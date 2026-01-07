#include <WiFi.h>
#include <WiFiUdp.h>
#include <Wire.h>
#include <Adafruit_DRV2605.h>

// ==== WiFi Configuration ====
const char* ssid = 
const char* password = 

// ==== UDP Target (your PC) ====
const char* udpAddress = "192.168.0.21";
const int udpPort = 4210;

WiFiUDP udp;

// ==== I2C + MUX + DRV ====
#define TCA_ADDR 0x70      // TCA9548A address
#define DRV_ADDR 0x5A      // DRV2605L address
#define SDA_PIN 8          // ✅ standard I2C pins
#define SCL_PIN 9

Adafruit_DRV2605 drv;

void tcaSelect(uint8_t channel) {
  if (channel > 7) return;
  Wire.beginTransmission(TCA_ADDR);
  Wire.write(1 << channel);
  Wire.endTransmission();
  delay(5);
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  // WiFi connect
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n✅ Connected to WiFi!");
  Serial.print("ESP32 IP Address: ");
  Serial.println(WiFi.localIP());

  // I2C init
  Wire.begin(SDA_PIN, SCL_PIN);
  Wire.setClock(400000);
  delay(100);

  // Select MUX channel 0
  Serial.println("🔀 Selecting MUX channel 0...");
  tcaSelect(0);

  // DRV2605 init
  if (drv.begin()) {
    drv.selectLibrary(1);
    drv.setMode(DRV2605_MODE_INTTRIG);
    Serial.println("✅ DRV2605L initialized on Channel 0!");
  } else {
    Serial.println("❌ DRV2605L init failed.");
  }

  // UDP setup
  udp.begin(udpPort);
  Serial.printf("Listening for UDP messages on port %d\n", udpPort);
}

void loop() {
  int packetSize = udp.parsePacket();
  if (packetSize) {
    char incomingPacket[255];
    int len = udp.read(incomingPacket, 255);
    if (len > 0) incomingPacket[len] = 0;
    String message = String(incomingPacket);

    Serial.printf("📩 Received: %s\n", incomingPacket);

    if (message.startsWith("vibrate:")) {
      int pattern = message.substring(message.indexOf(':') + 1).toInt();

      if (pattern < 0 || pattern > 117) {
        Serial.println("⚠️ Invalid pattern number (0–117 allowed).");
        return;
      }

      tcaSelect(0);
      drv.setWaveform(0, pattern);
      drv.setWaveform(1, 0);
      drv.go();

      Serial.printf("💥 Playing pattern #%d on Channel 0!\n", pattern);
    }
  }
}
