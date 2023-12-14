#include "Timekeeping.h"

ESP32Time rtc(0); // create an instance with a specifed offset in seconds
bool timeZoneConfigured = false;
bool timeConfigured = false;
bool dateConfigured = false;

void timekeepingSyncTask(void *pvParameter) {
  /*

  */
  while (1) {
    while (!dateConfigured) {
      if (timeConfigured && timeZoneConfigured)
        dateConfigured = true;
      vTaskDelay(1000 / portTICK_RATE_MS);
    }
    while (xEventGroupGetBits(BLEStateFlagGroup) & BLE_FLAG_CLIENT_CONNECTED) {
      // Ask for current time update
      vTaskDelay(1000 / portTICK_RATE_MS);
    }

    vTaskDelay(ONE_MIN_MS / portTICK_RATE_MS);
  }
}
