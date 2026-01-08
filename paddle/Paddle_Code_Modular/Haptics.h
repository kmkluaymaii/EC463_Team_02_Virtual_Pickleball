#pragma once
#include "Config.h"

#define TCA_ADDR 0x70
#define DRV_ADDR 0x5A
#define SDA_PIN 21
#define SCL_PIN 22

Adafruit_DRV2605 drv;
// Tracks whether the DRV2605 haptics driver was initialized successfully
bool hapticsReady = false;

// =========================
// MUX SELECT
// =========================
void tcaSelect(uint8_t channel) {
  // If using a single haptic device (no multiplexer), don't touch the TCA
  if (oneHaptic) return;
  if (channel > 7) return;
  Wire.beginTransmission(TCA_ADDR);
  Wire.write(1 << channel);
  Wire.endTransmission();
}

bool devicePresent(uint8_t addr) {
  Wire.beginTransmission(addr);
  return (Wire.endTransmission() == 0);
}

void initHaptics() {
  // If we have a TCA multiplexer, ensure channel 0 is selected first
  if (!oneHaptic) tcaSelect(0);

  // Probe for the DRV2605 on the active bus
  if (devicePresent(DRV_ADDR)) {
    if (drv.begin()) {
      hapticsReady = true;
      drv.selectLibrary(1);
      drv.setMode(DRV2605_MODE_INTTRIG);
      Serial.println("DRV2605L ready!");
    } else {
      hapticsReady = false;
      Serial.println("DRV2605 detected but drv.begin() failed!");
    }
  } else {
    hapticsReady = false;
    Serial.println("No DRV2605 detected!");
  }
}