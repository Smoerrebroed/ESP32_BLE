#include <Arduino.h>
// #include <FastLED.h>
#include <NimBLEDevice.h>

/** Bluetooth LE Characteristic Callbacks. */
class MeineCallbacks: public NimBLECharacteristicCallbacks {
  std::int8_t iScore = 0;
  
  void onWrite(NimBLECharacteristic* pCharacteristic) {
    std::string rxCharacteristicUUID = pCharacteristic->getUUID().toString(), rxCommand = pCharacteristic->getValue();
    iScore++;
    Serial.println(iScore);
    Serial.println(rxCharacteristicUUID.c_str());
    if (rxCommand.length() > 0) {
      Serial.print("Received Value: ");
      Serial.println(rxCommand.c_str());
    }
  }  
};

static NimBLEServer *pServer;
static NimBLEService *pServiceScore;
static NimBLECharacteristic *pCharacteristicHeim, *pCharacteristicGast;
static NimBLEAdvertising *pAdvertising;

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
  pServiceScore = pServer->createService("5C0A");

  pCharacteristicHeim = pServiceScore->createCharacteristic("1111", NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE);
  pCharacteristicHeim->setValue("0");
  pCharacteristicHeim->setCallbacks(new MeineCallbacks());
  pCharacteristicGast = pServiceScore->createCharacteristic("2222", NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE);
  pCharacteristicGast->setValue("0");
  pCharacteristicGast->setCallbacks(new MeineCallbacks());
  pServiceScore->start();

  pAdvertising = NimBLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(pServiceScore->getUUID()); 
  pAdvertising->start(); 
} // End of setup.

// This is the Arduino main loop function.
void loop() {
  delay(1000); // Delay a second between loops.
} // End of loop