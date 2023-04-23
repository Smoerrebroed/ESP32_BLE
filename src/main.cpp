#include <Arduino.h>
// #include <FastLED.h>
#include <NimBLEDevice.h>

/** Bluetooth LE Characteristic Callbacks. */
class MeineCallbacks: public NimBLECharacteristicCallbacks {
  std::int8_t iScoreHeim = 0;
  std::int8_t iScoreGast = 0;
  
  void onWrite(NimBLECharacteristic* pCharacteristic) {
    std::string rxCommand = pCharacteristic->getValue();
    
    Serial.println(rxCommand.c_str());

    switch (std::stoi(rxCommand)) {
      case 10:
        iScoreHeim = 0;
      break;
    
      case 11:
        if (iScoreHeim < 9) iScoreHeim++;
      break;

      case 12:
        if (iScoreHeim > 0) iScoreHeim--;
      break;

      case 20:
        iScoreGast = 0;
      break;

      case 21:
        if (iScoreGast < 9) iScoreGast++;
      break;

      case 22:
        if (iScoreGast > 0) iScoreGast--;
      break;
      
      default:
      break;
    }

    Serial.print("Heim: ");
    Serial.println(iScoreHeim);
    Serial.print("Gast: ");
    Serial.println(iScoreGast);
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
  // pCharacteristicGast = pServiceScore->createCharacteristic("2222", NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE);
  // pCharacteristicGast->setValue("0");
  // pCharacteristicGast->setCallbacks(new MeineCallbacks());
  pServiceScore->start();

  pAdvertising = NimBLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(pServiceScore->getUUID()); 
  pAdvertising->start(); 
} // End of setup.

// This is the Arduino main loop function.
void loop() {
  delay(1000); // Delay a second between loops.
} // End of loop