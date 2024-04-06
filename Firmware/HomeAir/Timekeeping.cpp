#include "Timekeeping.h"

ESP32Time rtc(0); // create an instance with a specifed offset in seconds
bool timeZoneConfigured = false;
bool timeConfigured = false;
bool dateConfigured = false;

void timekeepingSyncTask(void *pvParameter) {
  /*

  */
  while (1) {
    Serial.printf("online.pref: %d\n", online.pref);
    Serial.printf("restoreFromBackupTime: %d\n",
                  preferences.getBool("restoreFromBackupTime"));
    if (online.pref && preferences.getBool("restoreFromBackupTime")) {
      Serial.println("Setting time from backup!");
      rtc.setTime(preferences.getUInt("backupTime"));
      xEventGroupSetBits(appStateFlagGroup, APP_FLAG_CLOCK_DESYNC);
      timeConfigured = true;
      timeZoneConfigured = true;
      dateConfigured = true;
    } else {
      DBG("Timewarp not enabled");
    }
    while (!dateConfigured) {
      if (timeConfigured && timeZoneConfigured)
        dateConfigured = true;
      vTaskDelay(1000 / portTICK_RATE_MS);
    }
    while (xEventGroupGetBits(appStateFlagGroup) & APP_FLAG_RUNNING) {
      // Ask for current time update
      if (online.pref && dateConfigured) {
        Serial.println("Backing up time...");
        unsigned long currentTime = rtc.getEpoch();
        preferences.putULong("backupTime", currentTime);
      }
      vTaskDelay(ONE_MIN_MS / portTICK_RATE_MS);
    }

    vTaskDelay(5000 / portTICK_RATE_MS);
  }
}
