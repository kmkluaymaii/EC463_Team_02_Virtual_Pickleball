#include <WiFi.h>
#include <WiFiUdp.h>
#include <Wire.h>
#include <Adafruit_DRV2605.h>

const char* ssid = "BU Guest (unencrypted)";
const char* password = "";

// Your PC's IP address
const char* udpAddress = "168.122.149.186";
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

  // Select channel 0 (SDA0/SCL0)
  tcaSelect(0);
  delay(50);

  // Initialize DRV2605L
  if (devicePresent(DRV_ADDR)) {
    if (drv.begin()) {
      drv.selectLibrary(1);
      drv.setMode(DRV2605_MODE_INTTRIG);
      Serial.println("✅ DRV2605L initialized on Channel 0!");
    } else {
      Serial.println("❌ Failed to initialize DRV2605L");
    }
  } else {
    Serial.println("❌ No DRV2605L detected on Channel 0");
  }

  Serial.println("Ready to receive UDP commands!");
}

void loop() {
  int packetSize = udp.parsePacket();
  if (packetSize) {
    char incomingPacket[255];
    int len = udp.read(incomingPacket, 255);
    if (len > 0) incomingPacket[len] = 0;
    String message = String(incomingPacket);
    Serial.printf("📩 Received UDP message: %s\n", incomingPacket);

    // Expected format: vibrate:<patternNumber>
    if (message.startsWith("vibrate:")) {
      int colonIndex = message.indexOf(':');
      int pattern = message.substring(colonIndex + 1).toInt();

      if (pattern < 0 || pattern > 116) {
        Serial.println("⚠️ Invalid pattern number. Must be between 0–116.");
        return;
      }

      Serial.printf("🔔 Triggering vibration pattern %d on SDA0/SCL0...\n", pattern);
      tcaSelect(0);
      drv.setWaveform(0, pattern); // waveform index (intensity/pattern)
      drv.setWaveform(1, 0);       // end waveform
      drv.go();
      Serial.println("✅ Vibration triggered!");
    }
  }
}
