#pragma once
#include "Config.h"

#define TCA_ADDR 0x70
#define DRV_ADDR 0x5A
#define SDA_PIN 21
#define SCL_PIN 22

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

void initHaptics() {
  tcaSelect(0);
  if (devicePresent(DRV_ADDR)) {
    drv.begin();
    drv.selectLibrary(1);
    drv.setMode(DRV2605_MODE_INTTRIG);
    Serial.println("DRV2605L ready!");
  } else {
    Serial.println("No DRV2605 detected!");
  }
}