#ifndef SETTINGS_H
#define SETTINGS_H

#include "HomeAir.h"

struct struct_online{
  bool SPIFFS = false;
  bool BLE = false;
  bool sen5x = false;
  bool pasco2 = false;
  bool ng = false;
  bool co = false;
};

extern struct_online online;

#endif