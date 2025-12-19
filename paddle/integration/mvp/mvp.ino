#include <WiFi.h>
#include <WiFiUdp.h>
#include <Wire.h>
#include <Adafruit_DRV2605.h>
#include <driver/i2s_std.h>
#include <FastLED.h>
#include <Adafruit_BNO055.h>
#include <Adafruit_Sensor.h>
#include <utility/imumaths.h>

// =========================
// WIFI CONFIG 
// =========================
const char* ssid = "BU Guest (unencrypted)";
const char* password = "";

// =========================
// UDP
// =========================
WiFiUDP udp;
const int UDP_PORT = 4210;

const char* udpAddress = "10.192.53.166"; // Unreal IP Adress
const int REMOTE_PORT = 5005;

// =========================
// I2S AUDIO OUTPUT (SPEAKER)
// =========================
#define I2S_WS 25      // LRCLK
#define I2S_BCK 26     // Bit Clock
#define I2S_DOUT 27    // Audio Out
const int SAMPLE_RATE = 44100;
const int amplitude = 20000;

i2s_chan_handle_t tx_handle;

// ===== NON-BLOCKING AUDIO STATE =====
bool beepActive = false;
int beepFreq = 0;
int beepToggleSamples = 0;
int16_t beepSample = amplitude;
int beepCounter = 0;
unsigned long beepEndTime = 0;


// =========================
// HAPTICS SETUP
// =========================
#define TCA_ADDR 0x70
#define DRV_ADDR 0x5A
#define SDA_PIN 21
#define SCL_PIN 22
Adafruit_DRV2605 drv;

// =========================
// MUX SELECT
// =========================
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

// =========================
// I2S SETUP (NEW DRIVER)
// =========================
void setupI2S() {
  i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_0, I2S_ROLE_MASTER);
  i2s_new_channel(&chan_cfg, &tx_handle, NULL);

  i2s_std_config_t std_cfg = {
    .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(SAMPLE_RATE),
    .slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_MONO),
    .gpio_cfg = {
      .mclk = I2S_GPIO_UNUSED,
      .bclk = (gpio_num_t)I2S_BCK,
      .ws = (gpio_num_t)I2S_WS,
      .dout = (gpio_num_t)I2S_DOUT,
      .din = I2S_GPIO_UNUSED,
      .invert_flags = {
        .mclk_inv = false,
        .bclk_inv = false,
        .ws_inv = false,
      },
    },
  };

  i2s_channel_init_std_mode(tx_handle, &std_cfg);
  i2s_channel_enable(tx_handle);
}

// =========================
// AUDIO BEEP
// =========================
void playBeep(int freqHz, int durationMs) {
    if (freqHz <= 0 || durationMs <= 0) return; // ignore invalid

    beepFreq = freqHz;
    beepEndTime = millis() + durationMs;

    // Calculate how many samples per half-cycle
    beepToggleSamples = (SAMPLE_RATE / freqHz) / 2;
    if (beepToggleSamples < 1) beepToggleSamples = 1;

    beepCounter = 0;
    beepSample = amplitude;
    beepActive = true;
}


// =========================
// LED CONFIGURATION
// =========================
#define LED_PIN 23
#define NUM_LEDS 50
#define BRIGHTNESS 60
#define LED_TYPE WS2812B
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];

// =========================
// LED LIGHTING
// =========================
void showColor(const CRGB &c) {
  fill_solid(leds, NUM_LEDS, c);
  FastLED.show();
}

void showWinColor() {
  showColor(CRGB::Green);
}

void showLossColor() {
  showColor(CRGB::Red);
}

void showNeutralColor() {
  showColor(CRGB::Blue);
}

// =========================
// HAPTIC VIBRATION
// =========================
void playVibration(uint8_t channel, uint8_t effect) {
  if (!devicePresent(DRV_ADDR)) {
    Serial.println("⚠️ No DRV2605 detected. Cannot play vibration.");
    return;  // Return early if the device is not present
  }

  tcaSelect(channel);  // Select haptic channel (0 or 1)
  drv.setWaveform(0, effect);  // Set effect
  drv.setWaveform(1, 0);       // End waveform
  drv.go();                     // Trigger
}

// =========================
// PARSE HELPERS
// =========================
int getIntParam(String msg, int index) {
  int start = 0;
  int count = 0;
  for (int i = 0; i < msg.length(); i++) {
    if (msg[i] == ':') {
      count++;
      if (count == index) start = i + 1;
      if (count == index + 1) return msg.substring(start, i).toInt();
    }
  }
  return msg.substring(start).toInt();
}

String getStringParam(String msg, int index) {
  int start = 0;
  int count = 0;
  for (int i = 0; i < msg.length(); i++) {
    if (msg[i] == ':') {
      count++;
      if (count == index) start = i + 1;
      if (count == index + 1) return msg.substring(start, i);
    }
  }
  return msg.substring(start);
}

// =========================
// CHANGE LED COLOR FUNCTION
// =========================
void changeLedColor(String color) {
  if (color == "green") {
    showWinColor();
  } else if (color == "red") {
    showLossColor();
  } else if (color == "blue") {
    showNeutralColor();
  }
}

// =========================
// BNO055 IMU SETUP
// =========================
TwoWire I2C_2 = TwoWire(1);
Adafruit_BNO055 bno(55, BNO055_ADDRESS_B, &I2C_2);
uint16_t BNO055_SAMPLERATE_DELAY_MS = 100;

// =========================
// SETUP
// =========================
void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\n=== ESP32 AUDIO + HAPTIC UNIT BOOTING ===");

  // WiFi
  // WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(400);
  }
  Serial.println("\nConnected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // UDP
  udp.begin(UDP_PORT);
  Serial.printf("Listening on UDP port %d\n", UDP_PORT);

  // I2C
  Wire.begin(SDA_PIN, SCL_PIN);
  delay(100);

  // Haptics on channel 0
  tcaSelect(0);
  if (devicePresent(DRV_ADDR)) {
    drv.begin();
    drv.selectLibrary(1);
    drv.setMode(DRV2605_MODE_INTTRIG);
    Serial.println("DRV2605L ready!");
  } else {
    Serial.println("No DRV2605 detected!");
  }

  // Speaker
  setupI2S();
  Serial.println("I2S Audio Ready!");

  // LEDs setup
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
  
  Serial.println("LED Ready!");

  // I2C for IMU
  I2C_2.begin(16, 17);
  Serial.println("Initializing BNO055...");
  if (!bno.begin()) {
    Serial.println("BNO055 not detected on I2C_2!");
    while (1) { delay(500); Serial.println("Check wiring or address!"); }
  }
  Serial.println("BNO055 detected!");

}

// =========================
// LOOP
// =========================

void loop() {

  // ===== BNO055 READ =====
  sensors_event_t orientationData, angVelocityData, linearAccelData;
  bno.getEvent(&orientationData, Adafruit_BNO055::VECTOR_EULER);
  bno.getEvent(&angVelocityData, Adafruit_BNO055::VECTOR_GYROSCOPE);
  bno.getEvent(&linearAccelData, Adafruit_BNO055::VECTOR_LINEARACCEL);
    
  // Format as CSV string for easy parsing
  String imuPacket = "";
  imuPacket += String(orientationData.orientation.x, 2) + ",";
  imuPacket += String(orientationData.orientation.y, 2) + ",";
  imuPacket += String(orientationData.orientation.z, 2) + ";";

  // 🔍 PRINT TO SERIAL
  // Serial.print("IMU → ");
  // Serial.println(imuPacket);

  // Send over UDP
  udp.beginPacket(udpAddress, REMOTE_PORT);
  udp.print(imuPacket);
  udp.endPacket();

  delay(BNO055_SAMPLERATE_DELAY_MS);

  // ===== ALWAYS PROCESS AUDIO (whether beeping or silent) =====
  if (beepActive) {
    // Check if beep duration has expired
    if (millis() >= beepEndTime) {
      beepActive = false;
      beepSample = 0; // Prepare silence
    } else {
      // Generate square wave for beep
      beepCounter++;
      if (beepCounter >= beepToggleSamples) {
        beepCounter = 0;
        beepSample = -beepSample;
      }
    }
  } else {
    // Not beeping = output silence
    beepSample = 0;
  }

  // ALWAYS write to I2S (beep or silence)
  size_t written;
  i2s_channel_write(tx_handle, &beepSample, sizeof(beepSample), &written, 0);

  // ===== NOW CHECK FOR UDP PACKETS =====
  int packetSize = udp.parsePacket();
  if (packetSize > 0){

    // Read packet
    char packet[255];
    int len = udp.read(packet, sizeof(packet) - 1);
    if (len <= 0) return;

    String msg;

    packet[len] = 0;
    if (len > 0) {
        packet[len] = '\0';
        
        float x, y, z;
        if (sscanf(packet, "%f,%f,%f", &x, &y, &z) == 3) {
          Serial.printf("📊 DATA: X: %.3f, Y: %.3f, Z: %.3f\n", x, y, z);
          msg = "combo:440:20:0:47:green";
        } else {
          msg = "combo:50:50:0:0:red";
        }

        // ======= AUDIO =======
        if (msg.startsWith("beep:")) {
          int c1 = msg.indexOf(':');
          int c2 = msg.indexOf(':', c1 + 1);

          int freq = msg.substring(c1 + 1, c2).toInt();
          int duration = msg.substring(c2 + 1).toInt();

          Serial.printf("🔊 Beep %d Hz, %d ms\n", freq, duration);
          playBeep(freq, duration);
          // return;
        }

        // ======= HAPTIC =======
        if (msg.startsWith("haptic:")) {
          int c1 = msg.indexOf(':');
          int c2 = msg.indexOf(':', c1 + 1);

          int channel = msg.substring(c1 + 1, c2).toInt();
          int effect = msg.substring(c2 + 1).toInt();

          Serial.printf("🔔 Haptic ch=%d effect=%d\n", channel, effect);

          tcaSelect(channel);
          drv.setWaveform(0, effect);
          drv.setWaveform(1, 0);
          drv.go();

          // return;
        }

        // ======= COMBO =======
        if (msg.startsWith("combo:")) {
          int c1 = msg.indexOf(':');
          int c2 = msg.indexOf(':', c1 + 1);
          int c3 = msg.indexOf(':', c2 + 1);
          int c4 = msg.indexOf(':', c3 + 1);
          int c5 = msg.indexOf(':', c4 + 1);


          int freq    = msg.substring(c1 + 1, c2).toInt();
          int duration = msg.substring(c2 + 1, c3).toInt();
          int channel  = msg.substring(c3 + 1, c4).toInt();
          int effect   = msg.substring(c4 + 1, c5).toInt();
          String color  = msg.substring(c5 + 1);

          Serial.printf("💥 COMBO beep %d Hz %d ms + haptic ch=%d eff=%d\n",
                        freq, duration, channel, effect, color);

          tcaSelect(channel);
          drv.setWaveform(0, effect);
          drv.setWaveform(1, 0);
          drv.go();

          playBeep(freq, duration);

          changeLedColor(color);

          // return;
        }

        // ======= LED COLOR =======
        if (msg.startsWith("led:")) {
          String color = msg.substring(4); // Extract color after "led:"
          Serial.printf("💡 LED color: %s\n", color.c_str());
          changeLedColor(color);
          // return;
        }

        Serial.println("⚠️ Unknown command.");
      }
    }
  }


