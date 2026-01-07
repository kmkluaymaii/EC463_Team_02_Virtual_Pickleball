#include <Adafruit_NeoPixel.h>

#define LED_PIN    5
#define NUM_LEDS   70
#define BRIGHTNESS 80

Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

int position = 0;   // how many LEDs are lit

void setup() {
  strip.begin();
  strip.setBrightness(BRIGHTNESS);
  strip.show();
}

void loop() {

  // turn on all LEDs from 0 to position
  for(int i = 0; i <= position; i++){
    strip.setPixelColor(i, strip.Color(106, 0, 255)); // blue
  }

  strip.show();

  position++;

  if(position >= NUM_LEDS){
    position = 0; // reset to repeat
    strip.clear();  // turn all off
  }

  delay(30); // speed
}
