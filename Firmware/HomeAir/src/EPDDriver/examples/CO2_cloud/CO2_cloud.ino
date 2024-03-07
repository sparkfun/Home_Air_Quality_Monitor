
#include "PDLS_EXT3_Basic_Fast.h"

#include "hV_HAL_Peripherals_fast.h"

#include "hV_Configuration_fast.h"

//DEFAULT BOARD: boardESP32DevKitC
Screen_EPD_EXT3_Fast myScreen(eScreen_EPD_EXT3_266_0C_Fast);
/*
  Pins:
    "BUSY", Red devkit jumper = 16
    "D/C", Orange devkit jumper = 18 
    "RST", Yellow devkit jumper = 17
    "MISO", Green devkit jumper = 12
    "MOSI", Blue devkit jumper = 13
    "SCK", Brown devkit jumper = 14
    "FCSM", Purple devkit jumper = 8
    "ECSM", Grey devkit jumper = 3
*/

//BREADBOARD: breadBoardBreakout
// Screen_EPD_EXT3_Fast myScreen(eScreen_EPD_EXT3_266_0C_Fast, breadBoardBreakout);
/*
  Pins:
    "BUSY", Red devkit jumper = 45
    "D/C", Orange devkit jumper = 48 
    "RST", Yellow devkit jumper = 47
    "MISO", Green devkit jumper = 37
    "MOSI", Blue devkit jumper = 36
    "SCK", Brown devkit jumper = 35
    "FCSM", Purple devkit jumper = 21
    "ECSM", Grey devkit jumper = 11
*/

//CUSTOM PINS
//for redefinition of pins if necessary
const pins_t customPins =
{
    .panelBusy = 47, //"BUSY", Red breadboard jumper
    .panelDC = 37, //"D/C", Orange breadboard jumper
    .panelReset = 36, //"RST", Yellow breadboard jumper
    .panelMISO = 37, //"MISO", Green breadboard jumper
    .panelMOSI = 36, //"MOSI", Blue breadboard jumper
    .panelSCK = 35, //"SCK", Brown breadboard jumper
    .flashCS = NOT_CONNECTED, //"FCSM", Purple breadboard jumper
    .panelCS = NOT_CONNECTED, //"ECSM", Grey breadboard jumper
};

int per = 0;

void setup()
{
    Serial.begin(115200);
    myScreen.begin();
    myScreen.regenerate(); //globally clear screen
    // myScreen.drawSparkfunLogo();
    // delay(5000);
    myScreen.setOrientation(1);
    // myScreen.partialScreenBitmap(0, 0, co2_bitmap, 75, 144);

    myScreen.flush();
    delay(1000);
}

void loop()
{
  // myScreen.globalRefresh(3);
  // myScreen.drawSensorFrame(mySensor.temperature, 1);
  // myScreen.updateFrameVal(1, mySensor.temperature, (String)105);
  // myScreen.updateFrameVal(1, mySensor.humidity, (String)69);
  // myScreen.drawSensorFrame(mySensor.temperature, 0);
  // myScreen.updateFrameVal(0, mySensor.temperature, (String)105);
  // myScreen.updateFrameVal(0, mySensor.humidity, (String)69);
  // myScreen.dRectangle(48, 80, 202, 50, myColours.black);
  myScreen.firmwareUpdateScreen(per);
  per ++;
  per = per%100;
  myScreen.flush();
  delay(10);
}
