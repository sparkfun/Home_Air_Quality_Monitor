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

// String sensorMap[11] = {"CO2", "PPM1.0", "PPM2.5", "PPM4.0", "PPM10.0",
// "Humidity", "Temperature",
//   "VOCs", "CO", "NG", "AQI"};

extern TaskHandle_t mygpioSensorReadTaskHandle, spiffsStorageTaskHandle,
  BLEServerCommunicationTaskHandle, timekeepingSyncTaskHandle,
  screendriverRunScreenTaskHandle;

#define FORMAT_SPIFFS_IF_FAILED false

#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_PRIMARY_CORE 1
#define ARDUINO_AUX_CORE 0
#endif

#endif