#include <Arduino.h>
#include <FastLED.h>
#include <NimBLEDevice.h>

// How many leds in your strip?
#define NUM_LEDS 35

// For led chips like WS2812, which have a data line, ground, and power, you just
// need to define DATA_PIN.  For led chipsets that are SPI based (four wires - data, clock,
// ground, and power), like the LPD8806 define both DATA_PIN and CLOCK_PIN
// Clock pin only needed for SPI based chipsets when not using hardware SPI
#define DATA_PIN_1 16
#define DATA_PIN_2 14

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

int farbeHeim = 2;
int farbeGast = 4;

int DigitMatrix[11][35] = {
    {0,1,1,1,0, 0,1,0,1,0, 0,1,0,1,0, 0,1,0,1,0, 0,1,0,1,0, 0,1,0,1,0, 0,1,1,1,0}, //0
    {0,0,0,1,0, 0,1,1,0,0, 0,1,0,1,0, 0,1,0,0,0, 0,0,0,1,0, 0,1,0,0,0, 0,0,0,1,0}, //1
    {0,1,1,1,0, 0,1,0,0,0, 0,0,0,1,0, 0,1,1,1,0, 0,1,0,0,0, 0,0,0,1,0, 0,1,1,1,0}, //2
    {0,1,1,1,0, 0,1,0,0,0, 0,0,0,1,0, 0,1,1,0,0, 0,0,0,1,0, 0,1,0,0,0, 0,1,1,1,0}, //3
    {0,1,0,0,0, 0,1,0,1,0, 0,1,0,1,0, 0,1,1,1,0, 0,0,0,1,0, 0,1,0,0,0, 0,0,0,1,0}, //4
    {0,1,1,1,0, 0,0,0,1,0, 0,1,0,0,0, 0,1,1,1,0, 0,0,0,1,0, 0,1,0,0,0, 0,1,1,1,0}, //5
    {0,1,1,1,0, 0,0,0,1,0, 0,1,0,0,0, 0,1,1,1,0, 0,1,0,1,0, 0,1,0,1,0, 0,1,1,1,0}, //6
    {0,1,1,1,0, 0,1,0,0,0, 0,0,0,1,0, 0,1,0,0,0, 0,0,0,1,0, 0,1,0,0,0, 0,0,0,1,0}, //7
    {0,1,1,1,0, 0,1,0,1,0, 0,1,0,1,0, 0,1,1,1,0, 0,1,0,1,0, 0,1,0,1,0, 0,1,1,1,0}, //8
    {0,1,1,1,0, 0,1,0,1,0, 0,1,0,1,0, 0,1,1,1,0, 0,0,0,1,0, 0,1,0,0,0, 0,1,1,1,0}, //9
    {1,0,1,1,1, 1,0,1,0,1, 1,0,1,0,1, 1,0,1,0,1, 1,0,1,0,1, 1,0,1,0,1, 1,0,1,1,1}  //10
  };

class MeineServerCallbacks: public NimBLEServerCallbacks {
  void onConnect(NimBLEServer *pServer) {
    Serial.print("BLE Client ist verbunden. Adresse: ");
    Serial.println(pServer->getPeerInfo(0).getAddress());

    FastLED.clear(true);
      for (int j = 0; j < NUM_LEDS; j++) {
        if (DigitMatrix[0][j]) {
          ledsHeim[j] = color[farbeHeim];
          ledsGast[j] = color[farbeGast];
        }
      }
    FastLED.show();
  }

  void onDisconnect(NimBLEServer *pServer) {
    Serial.println("BLE Client ist getrennt.");
    FastLED.clear(true);
  }
};

/** Bluetooth LE Characteristic Callbacks. */
class MeineCallbacks: public NimBLECharacteristicCallbacks {
  std::int8_t iScoreHeim = 0, iScoreGast = 0;
  
  void onWrite(NimBLECharacteristic* pCharacteristic) {
    std::string rxCommand = pCharacteristic->getValue();

    // Serial.println(rxCommand.c_str());

    int befehl = std::stoi(rxCommand);

    switch (befehl) {
      case 10:
        iScoreHeim = 0;
        break;
    
      case 11:
        if (iScoreHeim < 10) iScoreHeim++;
        break;

      case 12:
        if (iScoreHeim > 0) iScoreHeim--;
        break;

      case 13:
        if (farbeHeim < 9)
          farbeHeim++;
        else
          farbeHeim = 0;
        break;

      case 20:
        iScoreGast = 0;
        break;

      case 21:
        if (iScoreGast < 10) iScoreGast++;
        break;

      case 22:
        if (iScoreGast > 0) iScoreGast--;
        break;
      
      case 23:
        if (farbeGast < 9)
          farbeGast++;
        else
          farbeGast = 0;
        break;

      default:
        break;
    }

    Serial.print("Heim: ");
    Serial.println(iScoreHeim);
    Serial.print("Farbe Heim: ");
    Serial.println(farbeHeim);
    Serial.print("Gast: ");
    Serial.println(iScoreGast);
    Serial.print("Farbe Gast: ");
    Serial.println(farbeGast);

    if (befehl > 9 && befehl < 20) {
      FastLED[0].clearLeds();
      for (int j = 0; j < NUM_LEDS; j++) {
        if (DigitMatrix[iScoreHeim][j])
          ledsHeim[j] = color[farbeHeim];
      }
      FastLED[0].showLeds(100);
    }
    else if (befehl > 19 && befehl < 30) {
      FastLED[1].clearLeds();
      for (int j = 0; j < NUM_LEDS; j++) {
        if (DigitMatrix[iScoreGast][j])
          ledsGast[j] = color[farbeGast];
      };
      FastLED[1].showLeds(100);
    } 
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

  while (!NimBLEDevice::isInitialized()) {
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