#include <Wire.h>
#include <Adafruit_DRV2605.h>

#define TCA_ADDR 0x70   // Default I2C address for TCA9548A
#define DRV_ADDR 0x5A   // Default I2C address for DRV2605L
#define SDA_PIN 8      // ESP32 default SDA
#define SCL_PIN 9      // ESP32 default SCL

Adafruit_DRV2605 drv;

void tcaSelect(uint8_t channel) {
  if (channel > 7) return;
  Wire.beginTransmission(TCA_ADDR);
  Wire.write(1 << channel); // Select this channel
  Wire.endTransmission();
}

bool devicePresent(uint8_t addr) {
  Wire.beginTransmission(addr);
  return (Wire.endTransmission() == 0);
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("TCA9548A DRV2605L Channel Tester (Loop Version)");

  Wire.begin(SDA_PIN, SCL_PIN);
  delay(100);
}

void loop() {
  for (uint8_t ch = 0; ch < 8; ch++) {
    tcaSelect(ch);
    delay(50);

    Serial.print("Testing channel ");
    Serial.print(ch);
    Serial.print("... ");

    if (devicePresent(DRV_ADDR)) {
      Serial.println("DRV2605L detected ✅");

      if (!drv.begin()) {
        Serial.println("   ❌ Failed to init DRV2605L");
        continue;
      }

      drv.selectLibrary(1);
      drv.setMode(DRV2605_MODE_INTTRIG);

      // Example vibration pattern
      drv.setWaveform(0, 47);  // 47 = strong buzz
      drv.setWaveform(1, 0);   // end waveform
      drv.go();

      Serial.println("   🔊 Vibration effect played!");
    } else {
      Serial.println("No DRV2605L found ❌");
    }

    delay(1000); // wait before next channel
  }

  // Disable all channels after a full cycle
  Wire.beginTransmission(TCA_ADDR);
  Wire.write(0x00);
  Wire.endTransmission();

  Serial.println("Cycle complete. Restarting...\n");
  delay(2000); // Wait before looping again
}
