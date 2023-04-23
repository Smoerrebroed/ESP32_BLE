#include <Arduino.h>
// #include <FastLED.h>
#include <NimBLEDevice.h>

static NimBLEServer *pServer;

void setup() {
  Serial.begin(115200);
  Serial.println("Starting Arduino NimBLE Client application...");

  NimBLEDevice::init("ESP32");

  #ifdef ESP_PLATFORM
    NimBLEDevice::setPower(ESP_PWR_LVL_P9); // +9db
  #else
    NimBLEDevice::setPower(9); // +9db
  #endif

  pServer = NimBLEDevice::createServer();

  NimBLEService *pServiceHeim = pServer->createService("5C01");
  NimBLECharacteristic *pCharacteristicHeim0 = pServiceHeim->createCharacteristic("0000", NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE);
  NimBLECharacteristic *pCharacteristicHeimUp = pServiceHeim->createCharacteristic("0001", NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE);
  NimBLECharacteristic *pCharacteristicHeimDown = pServiceHeim->createCharacteristic("0002", NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE);
  pCharacteristicHeim0->setValue("Reset");
  pCharacteristicHeimUp->setValue("Up");
  pCharacteristicHeimDown->setValue("Down");
  pServiceHeim->start();

  NimBLEService *pServiceGast = pServer->createService("5C02");
  NimBLECharacteristic *pCharacteristicGast0 = pServiceGast->createCharacteristic("1110", NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE);
  NimBLECharacteristic *pCharacteristicGastUp = pServiceGast->createCharacteristic("1111", NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE);
  NimBLECharacteristic *pCharacteristicGastDown = pServiceGast->createCharacteristic("1112", NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE);
  pCharacteristicGast0->setValue("Reset");
  pCharacteristicGastUp->setValue("Up");
  pCharacteristicGastDown->setValue("Down");
  pServiceGast->start();

  NimBLEAdvertising *pAdvertising = NimBLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(pServiceHeim->getUUID()); 
  pAdvertising->addServiceUUID(pServiceGast->getUUID()); 
  pAdvertising->start(); 
} // End of setup.

// This is the Arduino main loop function.
void loop() {
  delay(1000); // Delay a second between loops.
} // End of loop