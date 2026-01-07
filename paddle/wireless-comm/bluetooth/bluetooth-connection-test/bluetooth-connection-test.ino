#include "BluetoothSerial.h"

BluetoothSerial SerialBT;  // create Bluetooth Serial object

void setup() {
  Serial.begin(115200);     // for debug messages
  SerialBT.begin("ESP32_BT");  // Bluetooth name visible to your Mac
  Serial.println("Bluetooth device is ready to pair!");
}

void loop() {
  // if something comes from Mac (Bluetooth)
  if (SerialBT.available()) {
    char incoming = SerialBT.read();
    Serial.print("Received: ");
    Serial.println(incoming);

    // echo back
    SerialBT.print("Echo: ");
    SerialBT.println(incoming);
  }

  // if something typed in Serial Monitor -> send to Mac
  if (Serial.available()) {
    char c = Serial.read();
    SerialBT.write(c);
  }

  delay(20);
}
