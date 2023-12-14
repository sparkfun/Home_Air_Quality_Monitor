#ifndef SETTINGS_H
#define SETTINGS_H


struct struct_online{
  bool SPIFFS = false;
  bool BLE = false;
  bool sen5x = false;
  bool pasco2 = false;

};

extern struct_online online;

#endif