#ifndef HELLOWORLD_H
#define HELLOWORLD_H


#include "my_gpio.h"
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
// File system / storage
#include <Preferences.h>
#include "spiffs_helper.h"
#include "BLEServer.h"
#include "Timekeeping.h"
#include <float16.h>

String sensorMap[11] = {"CO2", "PPM1.0", "PPM2.5", "PPM4.0", "PPM10.0", "Humidity", "Temperature",
  "VOCs", "CO", "NG", "AQI"};
// Defined Values
#define RAW_DATA_ARRAY_SIZE 11
#define ONE_DAY_MS 86400000
// Pin defns
#define LED_PIN_0 A3
#define LED_PIN_1 A1
#define FORMAT_SPIFFS_IF_FAILED false


extern ESP32Time rtc;


#endif