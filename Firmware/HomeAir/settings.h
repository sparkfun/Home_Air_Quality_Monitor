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

struct display_settings{
  bool wallMounted = false;
  bool nightMode = false;
  bool clockEnabled = true;
  bool dotEnabled = true;
  uint8_t clockLocation = 0;
  uint8_t dotLocation = 0;
  uint8_t frame1sensor;
  uint8_t frame2sensor;
  uint16_t refreshTime = 5;
};

extern struct_online online;
extern display_settings epd_settings;

bool setupPreferences();

#endif