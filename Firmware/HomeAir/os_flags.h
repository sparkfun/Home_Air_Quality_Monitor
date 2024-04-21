#ifndef OS_FLAGS_H
#define OS_FLAGS_H

#include "Arduino.h"
#include "HomeAir.h"
#include "freertos/event_groups.h"

#define APP_FLAG_SETUP (1 << 1)
#define APP_FLAG_RUNNING (1 << 2)
#define APP_FLAG_TRANSMITTING (1 << 3)
#define APP_FLAG_IDLE (1 << 4)
#define APP_FLAG_PUSH_BUFFER (1 << 5)
#define APP_FLAG_DONE_TRANSMITTING (1 << 6)
#define APP_FLAG_OTA_DOWNLOAD (1 << 7)
#define APP_FLAG_FACTORY_ROLLBACK (1 << 8)
#define APP_FLAG_CLOCK_DESYNC (1 << 9)
#define APP_FLAG_EPD_FORCE_UPDATE (1 << 10)
#define APP_FLAG_BYPASS_SETUP (1 << 11)
#define APP_FLAG_ALL                                                           \
  APP_FLAG_SETUP | APP_FLAG_RUNNING | APP_FLAG_TRANSMITTING | APP_FLAG_IDLE |  \
      APP_FLAG_PUSH_BUFFER | APP_FLAG_DONE_TRANSMITTING |                      \
      APP_FLAG_OTA_DOWNLOAD | APP_FLAG_FACTORY_ROLLBACK |                      \
      APP_FLAG_EPD_FORCE_UPDATE | APP_FLAG_BYPASS_SETUP

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
#define BLE_FLAG_WRITE_COMPLETE (1 << 10)
#define BLE_FLAG_DOWNLOAD_COMPLETE (1 << 11)
#define BLE_FLAG_SAVE_COMPLETE (1 << 12)
#define BLE_FLAG_ALL                                                           \
  BLE_FLAG_IDLE | BLE_FLAG_BUSY | BLE_FLAG_READ_READY | BLE_FLAG_WRITE_READY | \
      BLE_FLAG_BUFFER_READY | BLE_FLAG_READ_COMPLETE |                         \
      BLE_FLAG_CLIENT_CONNECTED | BLE_FLAG_CLIENT_SUBSCRIBED |                 \
      BLE_FLAG_FILE_EXISTS | BLE_FLAG_FILE_DONE | BLE_FLAG_WRITE_COMPLETE |    \
      BLE_FLAG_DOWNLOAD_COMPLETE | BLE_FLAG_SAVE_COMPLETE

extern EventGroupHandle_t appStateFlagGroup;
extern EventGroupHandle_t BLEStateFlagGroup;

void printCurrentBLEFlagStatus(void);
void printCurrentAppFlagStatus(void);

#endif