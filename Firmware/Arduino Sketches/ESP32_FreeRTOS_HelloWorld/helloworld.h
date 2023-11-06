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
#include "ESP32Time.h"
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <float16.h>


typedef enum {
  CO2,
  PRESSURE,
  PPM1,
  PPM25,
  PPM40,
  PPM10,
  HUMIDITY,
  TEMP,
  VOC,
  CO,
  NG
} sensor_map;
// Defined Values
#define RAW_DATA_ARRAY_SIZE 10
#define ONE_DAY_MS 86400000
// Pin defns
#define LED_PIN_0 A3
#define LED_PIN_1 A1
#define FORMAT_SPIFFS_IF_FAILED false
// BLE Defns
#define SERVICE_UUID "9194f647-3a6c-4cf2-a6d5-187cb05728cd"
#define CHARACTERISTIC_UUID "588d30b0-33aa-4654-ab36-56dfa9974b13"


#endif