///
/// @file Example_Fast_Speed.ino
/// @brief Protocol for speed test
///
/// @details Project Pervasive Displays Library Suite
/// @n Based on highView technology
///
/// @author Rei Vilo
/// @date 21 Nov 2023
/// @version 702
///
/// @copyright (c) Rei Vilo, 2010-2023
/// @copyright Creative Commons Attribution-ShareAlike 4.0 International (CC BY-SA 4.0)
///
/// @see ReadMe.md for references
/// @n
///
/// Release 542: First release
/// Release 604: Global and fast variants
/// Release 702: Added xE2150KS0Jx and xE2152KS0Jx
///

// Screen
// #include "PDLS_EXT3_Basic_Global.h"
#include "PDLS_EXT3_Basic_Fast.h"

// SDK
// #include <Arduino.h>
#include "hV_HAL_Peripherals.h"

// Include application, user and local libraries
#include <SPI.h>
#define HOMEAIR_ESP32 (1)

// Configuration
#include "hV_Configuration.h"

const pins_t breadBoardBreakout =
{
    // .panelBusy = 45, //"BUSY<Red
    // .panelDC = 48, //Orange
    // .panelReset = 47, //Yellow
    // .panelMISO = 36, //Blue
    // .panelMOSI = 37, //Green
    // .panelSCK = 35, //Brown
    // .flashCS = 21, //Purple
    // .panelCS = 11, ///Grey

    .panelBusy = 45, //"BUSY", Red breadboard jumper
    .panelDC = 48, //"D/C", Orange breadboard jumper
    .panelReset = 47, //"RST", Yellow breadboard jumper
    // .panelMISO = 37, //"MISO", Green breadboard jumper
    // .panelMOSI = 36, //"MOSI", Blue breadboard jumper
    // .panelSCK = 35, //"SCK", Brown breadboard jumper
    .flashCS = 21, //"FCSM", Purple breadboard jumper
    .panelCS = 11, //"ECSM", Grey breadboard jumper
};

// Set parameters

// Define structures and classes

// Define constants and variables
// Screen_EPD_EXT3 myScreen(eScreen_EPD_EXT3_271, boardRaspberryPiPico_RP2040);
// Screen_EPD_EXT3 myScreen(eScreen_EPD_EXT3_370, boardRaspberryPiPico_RP2040);

// Screen_EPD_EXT3_Fast myScreen(eScreen_EPD_EXT3_271_09_Fast, boardRaspberryPiPico_RP2040);
Screen_EPD_EXT3_Fast myScreen(eScreen_EPD_EXT3_266_0C_Fast, breadBoardBreakout);

// Prototypes

// Utilities
///
/// @brief Wait with countdown
/// @param second duration, s
///
void wait(uint8_t second)
{
    for (uint8_t i = second; i > 0; i--)
    {
        Serial.print(formatString(" > %i  \r", i));
        delay(1000);
    }
    Serial.print("         \r");
}

// Functions
///
/// @brief Perform the speed test
///
void performTest()
{
    uint32_t chrono;

    myScreen.clear();
    myScreen.setOrientation(3);

    uint16_t x = myScreen.screenSizeX();
    uint16_t y = myScreen.screenSizeY();
    uint16_t dx = 0;
    uint16_t dy = 0;
    uint16_t dz = y / 2;
    String text = "";

    myScreen.selectFont(Font_Terminal12x16);

    // 0
    dy = (dz - myScreen.characterSizeY()) / 2;
    text = myScreen.WhoAmI() + " - " + String(SCREEN_EPD_EXT3_RELEASE);
    Serial.println(text);
    dx = (x - myScreen.stringSizeX(text)) / 2;
    myScreen.gText(dx, dy, text);
    myScreen.dRectangle(0, dz * 0, x, dz, myColours.black);

    chrono = millis();
    myScreen.flush();
    chrono = millis() - chrono;

    // 1
    dy += dz;
    // text = formatString("Global update= %i ms", chrono);
    text = formatString("Fast update= %i ms", chrono);
    Serial.println(text);
    dx = (x - myScreen.stringSizeX(text)) / 2;
    myScreen.gText(dx, dy, text);
    myScreen.dRectangle(0, dz * 1, x, dz, myColours.black);

    myScreen.flush();
}

// Add setup code
///
/// @brief Setup
///
void setup()
{
    Serial.begin(115200);
    delay(500);
    Serial.println();
    Serial.println("=== " __FILE__);
    Serial.println("=== " __DATE__ " " __TIME__);
    Serial.println();

    Serial.println("begin... ");
    myScreen.begin();
    Serial.println(formatString("%s %ix%i", myScreen.WhoAmI().c_str(), myScreen.screenSizeX(), myScreen.screenSizeY()));

    Serial.println("Speed... ");
    myScreen.clear();
    performTest();
    wait(3);

    Serial.println("White... ");
    myScreen.clear();
    myScreen.flush();

    Serial.println("=== ");
    Serial.println();
}

// Add loop code
///
/// @brief Loop, empty
///
void loop()
{
  Serial.println("Stuck in loop().");
  delay(1000);
}
