#include <Arduino.h>
// #include <FastLED.h>
#include <NimBLEDevice.h>

class MeineServerCallbacks: public NimBLEServerCallbacks {
  void onConnect(NimBLEServer *pServer) {
    Serial.print("BLE Client ist verbunden. Adresse: ");
    Serial.println(pServer->getPeerInfo(0).getAddress());
  }

  void onDisconnect(NimBLEServer *pServer) {
    Serial.println("BLE Client ist getrennt.");
  }
};

/** Bluetooth LE Characteristic Callbacks. */
class MeineCallbacks: public NimBLECharacteristicCallbacks {
  std::int8_t iScoreHeim = 0, iScoreGast = 0;
  
  void onWrite(NimBLECharacteristic* pCharacteristic) {
    std::string rxCommand = pCharacteristic->getValue();

    // Serial.println(rxCommand.c_str());

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
static NimBLECharacteristic *pCharacteristicScore;
static NimBLEAdvertising *pAdvertising;

void setup() {
  Serial.begin(115200);

  while (!NimBLEDevice::getInitialized()) {
    Serial.println("ESP32 NimBLE Server wird gestartet ...");
    NimBLEDevice::init("ESP32");
  }
  Serial.println("ESP32 NimBLE Server ist initialisiert.");

  #ifdef ESP_PLATFORM
    NimBLEDevice::setPower(ESP_PWR_LVL_P9); // +9db
  #else
    NimBLEDevice::setPower(9); // +9db
  #endif

  // Server erzeugen
  pServer = NimBLEDevice::createServer();
  pServer->setCallbacks(new MeineServerCallbacks());

  // Service erzeugen
  pServiceScore = pServer->createService("5C0A");

  // Characteristic erzeugen
  pCharacteristicScore = pServiceScore->createCharacteristic("1111", NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE);
  pCharacteristicScore->setValue("0");
  pCharacteristicScore->setCallbacks(new MeineCallbacks());
  pServiceScore->start();

  // Advertising starten
  pAdvertising = NimBLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(pServiceScore->getUUID()); 
  pAdvertising->start(); 
} // End of setup.

// This is the Arduino main loop function.
void loop() {
} // End of loop