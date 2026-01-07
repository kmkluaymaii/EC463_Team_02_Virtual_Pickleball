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
const int UDP_PORT = 4210;
const char* udpAddress = "10.193.33.237";
const int REMOTE_PORT = 5678;