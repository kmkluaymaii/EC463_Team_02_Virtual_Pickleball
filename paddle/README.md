# Note

## For IMU

For using ESP32 (38 pin) please follow the following set up:
1. SDA - 21, SCL - 22
2. On Arduino IDE choose "ESP32 Dev Module"
3. Network settings:
```
  const char* ssid = "BU Guest (unencrypted)";
  const char* password = "";
```
4. Memory Address use 29
```
Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x29, &Wire);
```
