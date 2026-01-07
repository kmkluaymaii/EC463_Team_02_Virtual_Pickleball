#pragma once
#include "Config.h"

#define LED_PIN 23
#define NUM_LEDS 50
#define BRIGHTNESS 60
#define LED_TYPE WS2812B
#define COLOR_ORDER GRB

CRGB leds[NUM_LEDS];

void initLEDs() {
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
  Serial.println("LED Ready!");
}

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