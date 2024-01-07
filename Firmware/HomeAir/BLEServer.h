#ifndef BLESERVER_H
#define BLESERVER_H

// BLE Library
#include <NimBLEDevice.h> // https://github.com/h2zero/NimBLE-Arduino
// Standard Library
#include <stdint.h>
#include <string.h>
// ESP32 System Includes
#include "esp_mac.h"
// HomeAir files
#include "SPIFFS.h"
#include "Timekeeping.h"
#include "os_flags.h"
#include "spiffs_helper.h"
#include "settings.h"
#include "mygpio.h"


// BLE Defns
#define SERVICE_UUID "9194f647-3a6c-4cf2-a6d5-187cb05728cd"
#define CHARACTERISTIC_UUID "588d30b0-33aa-4654-ab36-56dfa9974b13"
const uint16_t BLE_BUFFER_LENGTH = 512;

void BLEServerSetupBLE();
void BLEServerCommunicationTask(void *);

extern char BLEMessageBuffer[BLE_BUFFER_LENGTH];
extern NimBLECharacteristic *pSensorCharacteristic;
#endif