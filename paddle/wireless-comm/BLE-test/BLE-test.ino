#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

BLEServer* pServer = nullptr;
BLECharacteristic* pTxCharacteristic;
bool deviceConnected = false;
String rxValue = "";  // use Arduino String instead of std::string

#define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

class MyCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    std::string value = pCharacteristic->getValue();
    if (value.length() > 0) {
      rxValue = "";  // clear before storing new data
      for (int i = 0; i < value.length(); i++) {
        rxValue += value[i];
      }
      Serial.print("Received: ");
      Serial.println(rxValue);
    }
  }
};

void setup() {
  Serial.begin(115200);
  BLEDevice::init("ESP32S3_BLE");

  pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(SERVICE_UUID);

  pTxCharacteristic = pService->createCharacteristic(
                        CHARACTERISTIC_UUID_TX,
                        BLECharacteristic::PROPERTY_NOTIFY
                      );

  pTxCharacteristic->addDescriptor(new BLE2902());

  BLECharacteristic *pRxCharacteristic = pService->createCharacteristic(
                                          CHARACTERISTIC_UUID_RX,
                                          BLECharacteristic::PROPERTY_WRITE
                                        );

  pRxCharacteristic->setCallbacks(new MyCallbacks());
  pService->start();

  pServer->getAdvertising()->start();
  Serial.println("Waiting for a BLE connection...");
}

void loop() {
  if (rxValue.length() > 0) {
    String echoMsg = "Echo: " + rxValue;
    pTxCharacteristic->setValue(echoMsg.c_str());  // convert to const char*
    pTxCharacteristic->notify();
    rxValue = "";
  }
  delay(100);
}