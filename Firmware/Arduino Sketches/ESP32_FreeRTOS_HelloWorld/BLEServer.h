#ifndef BLESERVER_H
#define BLESERVER_h

#define CONFIG_NIMBLE_CPP_ATT_VALUE_INIT_LENGTH 512

#include <NimBLEDevice.h>
#include "Timekeeping.h"
#include "os_flags.h"

// BLE Defns
#define SERVICE_UUID "9194f647-3a6c-4cf2-a6d5-187cb05728cd"
#define CHARACTERISTIC_UUID "588d30b0-33aa-4654-ab36-56dfa9974b13"


void setupBLE();

extern char BLEMessageBuffer[CONFIG_NIMBLE_CPP_ATT_VALUE_INIT_LENGTH];
extern NimBLECharacteristic *pSensorCharacteristic;
#endif