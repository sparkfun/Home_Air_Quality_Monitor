#ifndef SPIFFS_HELPER_H
#define SPIFFS_HELPER_H
// ESP32 Built-ins
#include <Arduino.h>
#include "FS.h"
#include "SPIFFS.h"
// HomeAir Files
#include "os_flags.h"
#include "Timekeeping.h"
#include "mygpio.h"
#include "BLEServer.h"


  void listDir(fs::FS &fs, const char *dirname, uint8_t levels);
  void readFile(fs::FS &fs, const char *path);
  void writeFile(fs::FS &fs, const char *path, const char *message);
  void appendFile(fs::FS &fs, const char *path, const char *message);
  void renameFile(fs::FS &fs, const char *path1, const char *path2);
  void deleteFile(fs::FS &fs, const char *path);
  void testFileIO(fs::FS &fs, const char *path);

  void spiffs_storage_task(void *pvParameter);

#endif
