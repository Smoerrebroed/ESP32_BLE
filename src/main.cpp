#include <Arduino.h>
#include <FastLED.h>
#include <NimBLEDevice.h>

// How many leds in your strip?
#define NUM_LEDS 35

// For led chips like WS2812, which have a data line, ground, and power, you just
// need to define DATA_PIN.  For led chipsets that are SPI based (four wires - data, clock,
// ground, and power), like the LPD8806 define both DATA_PIN and CLOCK_PIN
// Clock pin only needed for SPI based chipsets when not using hardware SPI
#define DATA_PIN_1 14
#define DATA_PIN_2 16

// Define the array of leds
CRGB ledsHeim[NUM_LEDS];
CRGB ledsGast[NUM_LEDS];
CRGB color[10] = {
  CRGB::Red,
  CRGB::Green,
  CRGB::Blue,
  CRGB::Yellow,
  CRGB::White,
  CRGB::Purple,
  CRGB::Orange,
  CRGB::Cyan,
  CRGB::DarkGreen,
  CRGB::Magenta
};

int DigitMatrix[10][35] = {
    {1,1,1,1,1,1,0,0,0,1,1,0,0,0,1,1,0,0,0,1,1,0,0,0,1,1,0,0,0,1,1,1,1,1,1}, //0
    {0,0,1,0,0,0,0,1,1,0,1,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,1,1,1,0}, //1
    {1,1,1,1,1,1,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,1,1,1,1,1,1}, //2
    {1,1,1,1,1,1,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,1,1,0,0,0,0,1,1,1,1,1}, //3
    {1,0,0,0,0,0,0,1,0,1,1,0,1,0,0,1,1,1,1,1,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0}, //4
    {1,1,1,1,1,0,0,0,0,1,1,0,0,0,0,1,1,1,1,1,0,0,0,0,1,1,0,0,0,0,1,1,1,1,1}, //5
    {1,1,1,1,1,0,0,0,0,1,1,0,0,0,0,1,1,1,1,1,1,0,0,0,1,1,0,0,0,1,1,1,1,1,1}, //6
    {1,1,1,1,1,1,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,1}, //7
    {1,1,1,1,1,1,0,0,0,1,1,0,0,0,1,1,1,1,1,1,1,0,0,0,1,1,0,0,0,1,1,1,1,1,1}, //8
    {1,1,1,1,1,1,0,0,0,1,1,0,0,0,1,1,1,1,1,1,0,0,0,0,1,1,0,0,0,0,1,1,1,1,1}  //9
  };

class MeineServerCallbacks: public NimBLEServerCallbacks {
  void onConnect(NimBLEServer *pServer) {
    Serial.print("BLE Client ist verbunden. Adresse: ");
    Serial.println(pServer->getPeerInfo(0).getAddress());
        for (int j = 0; j < NUM_LEDS; j++) {
      if (DigitMatrix[0][j]) {
        ledsHeim[j] = CRGB::Blue;
        ledsGast[j] = CRGB::Red;
      }
      else {
        ledsHeim[j] = CRGB::Black;
        ledsGast[j] = CRGB::Black;
      }
    }
    FastLED.show(50);
  }

  void onDisconnect(NimBLEServer *pServer) {
    Serial.println("BLE Client ist getrennt.");
    FastLED.show(0);
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

    for (int j = 0; j < NUM_LEDS; j++) {
      if (DigitMatrix[iScoreHeim][j])
        ledsHeim[j] = CRGB::Blue;
      else ledsHeim[j] = CRGB::Black;
    }
    for (int j = 0; j < NUM_LEDS; j++) {
      if (DigitMatrix[iScoreGast][j])
        ledsGast[j] = CRGB::Red;
      else ledsGast[j] = CRGB::Black;
    }
    FastLED.show(50);
  }  
};

static NimBLEServer *pServer;
static NimBLEService *pServiceScore;
static NimBLECharacteristic *pCharacteristicScore;
static NimBLEAdvertising *pAdvertising;

void setup() {
  Serial.begin(115200);
  
  FastLED.addLeds<WS2811, DATA_PIN_1, RGB>(ledsHeim, NUM_LEDS);
  FastLED.addLeds<WS2811, DATA_PIN_2, RGB>(ledsGast, NUM_LEDS);

  while (!NimBLEDevice::getInitialized()) {
    Serial.println("ESP32 NimBLE Server wird gestartet ...");
    NimBLEDevice::init("TTC MJK Herten");
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