#ifndef OS_FLAGS_H
#define OS_FLAGS_H

#include "Arduino.h"
#include "freertos/event_groups.h"

#define APP_FLAG_SETUP (1 << 1)
#define APP_FLAG_RUNNING (1 << 2)
#define APP_FLAG_TRANSMITTING (1 << 3)
#define APP_FLAG_IDLE (1 << 4)
#define APP_FLAG_PUSH_BUFFER (1 << 5)
#define APP_FLAG_DONE_TRANSMITTING (1 << 6)

#define BLE_FLAG_IDLE (1 << 0)
#define BLE_FLAG_BUSY (1 << 1)
#define BLE_FLAG_READ_READY (1 << 2)
#define BLE_FLAG_WRITE_READY (1 << 3)
#define BLE_FLAG_BUFFER_READY (1 << 4)
#define BLE_FLAG_READ_COMPLETE (1 << 5)
#define BLE_FLAG_CLIENT_CONNECTED (1 << 6)
#define BLE_FLAG_CLIENT_SUBSCRIBED (1 << 7)
#define BLE_FLAG_FILE_EXISTS (1 << 8)
#define BLE_FLAG_FILE_DONE (1 << 9)

extern EventGroupHandle_t appStateFG;
extern EventGroupHandle_t BLEStateFG;

void printCurrentBLEFlagStatus(void);
void printCurrentAppFlagStatus(void);

#endif