#pragma once

#include <WiFi.h>
#include <WiFiUdp.h>
#include <Wire.h>
#include <Adafruit_DRV2605.h>
#include <driver/i2s_std.h>
#include <FastLED.h>
#include <Adafruit_BNO055.h>
#include <Adafruit_Sensor.h>
#include <utility/imumaths.h>

// WIFI
const char* ssid = "BU Guest (unencrypted)";
const char* password = "";

// UDP
WiFiUDP udp;
const int UDP_PORT = 4210;                    // receive port, game -> paddle
const char* udpAddress = "10.193.33.237";     // the IP address of the PC (where the UE game is running)
const int REMOTE_PORT = 5678;                 // send port, paddle -> game

// HAPTICS MODE
// When `oneHaptic` is true the design assumes a single DRV2605 is connected
// directly (no TCA multiplexer). In that mode we skip using the TCA and
// route SDA/SCL directly to the haptic driver pins (defaults 21/22).
bool oneHaptic = true;