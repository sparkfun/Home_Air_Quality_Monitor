#ifndef SPIFFS_HELPER_H
#define SPIFFS_HELPER_H
// ESP32 Built-ins
#include "FS.h"
#include "SPIFFS.h"
#include <Arduino.h>
// Standard Library Functions
#include <string>
#include <cmath>
// HomeAir Files
#include "BLEServer.h"
#include "Timekeeping.h"
#include "mygpio.h"
#include "os_flags.h"

void listDir(fs::FS &fs, const char *dirname, uint8_t levels);
void readFile(fs::FS &fs, const char *path);
void writeFile(fs::FS &fs, const char *path, const char *message);
void appendFile(fs::FS &fs, const char *path, const char *message);
void renameFile(fs::FS &fs, const char *path1, const char *path2);
void deleteFile(fs::FS &fs, const char *path);
void testFileIO(fs::FS &fs, const char *path);
void deleteAllFiles(fs::FS &fs);

void spiffs_storage_task(void *pvParameter);

#endif
