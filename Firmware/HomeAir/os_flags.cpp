#include "os_flags.h"

struct AppFlag {
  int flag;
  const char *name;
};

// #define BLE_FLAG_IDLE (1 << 0)
// #define BLE_FLAG_BUSY (1 << 1)
// #define BLE_FLAG_READ_READY (1 << 2)
// #define BLE_FLAG_WRITE_READY (1 << 3)
// #define BLE_FLAG_BUFFER_READY (1 << 4)
// #define BLE_FLAG_READ_COMPLETE (1 << 5)
// #define BLE_FLAG_CLIENT_CONNECTED (1 << 6)
// #define BLE_FLAG_CLIENT_SUBSCRIBED (1 << 7)

AppFlag bleFlags[] = {
    {BLE_FLAG_IDLE, "BLE_FLAG_IDLE"},
    {BLE_FLAG_BUSY, "BLE_FLAG_BUSY"},
    {BLE_FLAG_READ_READY, "BLE_FLAG_READ_READY"},
    {BLE_FLAG_WRITE_READY, "BLE_FLAG_WRITE_READY"},
    {BLE_FLAG_BUFFER_READY, "BLE_FLAG_BUFFER_READY"},
    {BLE_FLAG_READ_COMPLETE, "BLE_FLAG_READ_COMPLETE"},
    {BLE_FLAG_CLIENT_CONNECTED, "BLE_FLAG_CLIENT_CONNECTED"},
    {BLE_FLAG_CLIENT_SUBSCRIBED, "BLE_FLAG_CLIENT_SUBSCRIBED"}};

void printCurrentBLEFlagStatus() {
  int bleStatus = xEventGroupGetBits(BLEStateFlagGroup);
  // Iterate through each flag and print its status
  for (const auto &flag : bleFlags) {
    Serial.printf("%s: %d\n", flag.name, (int)bleStatus & flag.flag);
  }
}

// #define APP_FLAG_SETUP (1 << 1)
// #define APP_FLAG_RUNNING (1 << 2)
// #define APP_FLAG_TRANSMITTING (1 << 3)
// #define APP_FLAG_IDLE (1 << 4)
// #define APP_FLAG_PUSH_BUFFER (1 << 5)
// #define APP_FLAG_DONE_TRANSMITTING (1 << 6)

// Array of all flags
AppFlag appFlags[] = {{APP_FLAG_SETUP, "app_setup"},
                      {APP_FLAG_RUNNING, "app_running"},
                      {APP_FLAG_TRANSMITTING, "app_transmitting"},
                      {APP_FLAG_IDLE, "app_idle"},
                      {APP_FLAG_PUSH_BUFFER, "app_push_buffer"},
                      {APP_FLAG_DONE_TRANSMITTING, "app_done_transmitting"},
                      {APP_FLAG_OTA_DOWNLOAD, "app_ota_download"},
                      {APP_FLAG_FACTORY_ROLLBACK, "app_factory_rollback"}};

void printCurrentAppFlagStatus() {
  int appStatus = xEventGroupGetBits(appStateFlagGroup);
  // Iterate through each flag and print its status
  for (const auto &flag : appFlags) {
    Serial.printf("%s: %d\n", flag.name, (int)appStatus & flag.flag);
  }
}