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
  bool wallMounted = false;
  bool nightMode = false;
  uint8_t clockLocation = 0;
  uint8_t dotLocation = 0;
  uint8_t frame1sensor = mySensor.temperature;
  uint8_t frame2sensor = mySensor.co2;
  uint16_t refreshTime = 1;
  uint8_t state = 0;
};

extern display_settings epd_settings;


#endif