#ifndef HELLOWORLD_H
#define HELLOWORLD_H

// C Standard Library
#include <stdio.h>
// Extended Data Types (Non-standard)
#include <float16.h>  // https://github.com/RobTillaart/float16 - Version 0.1.8
// HomeAir Files
#include "BLEServer.h"
#include "Timekeeping.h"
#include "mygpio.h"
#include "os_flags.h"
#include "spiffs_helper.h"
#include "screen_driver.h"
#include "SPIFFS.h"
#include "Timekeeping.h"
#include "spiffs_helper.h"
#include "settings.h"
#include "errorHandling.h"
#include "AQI.h"
// ESP32 Built-ins
#include "FS.h"
#include "SPIFFS.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <Preferences.h>
#include "esp_ota_ops.h"
#include "esp_timer.h"

extern TaskHandle_t mygpioSensorReadTaskHandle, spiffsStorageTaskHandle,
  BLEServerCommunicationTaskHandle, timekeepingSyncTaskHandle,
  screendriverRunScreenTaskHandle;

extern Preferences preferences;
extern SemaphoreHandle_t rawDataMutex, otaDownloadPercentageMutex;
extern TimerHandle_t  gpio0Timer;

#define VERSION_NUMBER 201
#define FORMAT_SPIFFS_IF_FAILED false
#define TEN_KiB 10 * 1024

#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_PRIMARY_CORE 1
#define ARDUINO_AUX_CORE 0
#endif

#endif