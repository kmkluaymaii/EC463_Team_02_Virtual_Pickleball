#pragma once
#include "Config.h"

// =========================
// LED CONFIGURATION
// =========================
#define LED_PIN 23
#define NUM_LEDS 18
#define BRIGHTNESS 60
#define LED_TYPE WS2812B
#define COLOR_ORDER GRB

CRGB leds[NUM_LEDS];

void initLEDs() {
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
  Serial.println("LED Ready!");
}

// =========================
// LED LIGHTING
// =========================
void showColor(const CRGB &c) {
  fill_solid(leds, NUM_LEDS, c);
  FastLED.show();
}

void showWinColor()     { showColor(CRGB::Green); }
void showLossColor()    { showColor(CRGB::Red); }
void showNeutralColor() { showColor(CRGB::Blue); }

void changeLedColor(String color) {
  if (color == "green") showWinColor();
  else if (color == "red") showLossColor();
  else if (color == "blue") showNeutralColor();
}


//ripple
void displayHitColorAnim() {
  const uint32_t animDurationMs = 1000;    // total animation time
  const uint8_t pulseWidth = 6;            // width of the pulse
  const CRGB hitColor = CRGB::White;       // hit flash color

  uint32_t startTime = millis();

  while (millis() - startTime < animDurationMs) {
    uint32_t elapsed = millis() - startTime;

    // Map time → LED index sweep
    int center = map(elapsed, 0, animDurationMs, 0, NUM_LEDS - 1);

    // Clear LEDs
    fill_solid(leds, NUM_LEDS, CRGB::Black);

    // Draw pulse
    for (int i = -pulseWidth; i <= pulseWidth; i++) {
      int idx = center + i;
      if (idx >= 0 && idx < NUM_LEDS) {
        uint8_t brightness = map(abs(i), 0, pulseWidth, 255, 40);
        leds[idx] = hitColor;
        leds[idx].nscale8(brightness);
      }
    }

    FastLED.show();
    delay(5); // smooth animation (~66 FPS)
  }

  // Turn off LEDs after animation
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  FastLED.show();
}


// flash and decay
void displayHitFlashDecayAnim() {
  const CRGB hitColor = CRGB::White;   // impact color
  const uint32_t flashTimeMs = 60;     // instant flash duration
  const uint32_t decayTimeMs = 1200;   // fade-out duration
  const uint8_t startBrightness = 255;

  // ===== FLASH =====
  fill_solid(leds, NUM_LEDS, hitColor);
  FastLED.setBrightness(startBrightness);
  FastLED.show();
  delay(flashTimeMs);

  // ===== DECAY =====
  uint32_t startTime = millis();
  while (millis() - startTime < decayTimeMs) {
    uint32_t elapsed = millis() - startTime;

    // Exponential-like fade (feels more natural than linear)
    uint8_t brightness = map(elapsed, 0, decayTimeMs,
                             startBrightness, 0);

    FastLED.setBrightness(brightness);
    FastLED.show();
    delay(15); // smooth decay
  }

  // ===== OFF =====
  FastLED.setBrightness(BRIGHTNESS); // restore default
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  FastLED.show();
}