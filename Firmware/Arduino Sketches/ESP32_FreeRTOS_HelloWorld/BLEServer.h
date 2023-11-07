#ifndef BLESERVER_H
#define BLESERVER_h


#include <NimBLEDevice.h>
#include "Timekeeping.h"

// BLE Defns
#define SERVICE_UUID "9194f647-3a6c-4cf2-a6d5-187cb05728cd"
#define CHARACTERISTIC_UUID "588d30b0-33aa-4654-ab36-56dfa9974b13"

void setupBLE();


#endif