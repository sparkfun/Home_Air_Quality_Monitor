#ifndef SETTINGS_H
#define SETTINGS_H

#include "HomeAir.h"
#include "src/EPDDriver/src/sensors.h"

struct struct_online{
  bool SPIFFS = false;
  bool BLE = false;
  bool sen5x = false;
  bool pasco2 = false;
  bool ng = false;
  bool co = false;
};

extern struct_online online;

struct display_settings{

  //UNIVERSAL SCREEN SETTINGS
  bool wallMounted = false;
  bool nightMode = false;
  bool clockEnabled = false;
  uint8_t clockLocation = 0; // 0=top right
  bool dotEnabled = false;
  uint8_t dotLocation = 0; // 0=top right
  uint16_t refreshTime = 1; // in seconds

  //BURN IN PREVENTION
  int cyclesBetweenFullRefresh = 300;
  //number of refresh cycles between global refresh. Global refresh redraws entire screen to black and white several times to prevent burn in. Lower number here better prevents burn in but is more distracting. 
  uint8_t numRefreshCycles = 5;
  //number of times to flash the screen to prevent burn in. Does not need to be more than 10.

  //LOGO SCREEN
  uint8_t logoTime = 3; // time in seconds that Sparkfun logo is displayed upon boot

  //PAIRING SCREEN

  //SENSOR READINGS (NORMAL OPERATION)
  uint8_t frame0sensor; // sensor on left
  uint8_t frame1sensor; // sensor on right

  //FIRMWARE UPDATE
  uint8_t updatePercent; // update progress, should not be more than 100
};

extern display_settings epd_settings;


#endif