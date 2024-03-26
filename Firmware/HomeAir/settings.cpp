#include "settings.h"


struct_online online;
struct display_settings epd_settings;

// struct display_settings{
//   bool wallMounted = false;
//   bool nightMode = false;
//   bool clockEnabled = true;
//   bool dotEnabled = true;
//   uint8_t clockLocation = 0;
//   uint8_t dotLocation = 0;
//   uint8_t frame1sensor;
//   uint8_t frame2sensor;
//   uint16_t refreshTime = 5;
// };

bool setupPreferences() {
  Serial.println("PREFERENCES SET UP");
  // Preferences is good for single KVP storage.
  // We want to use SPIFFS for large storage
  bool status = preferences.begin("HomeAirSettings", false);
  // if (status) {
    // online.pref = true;
    // Serial.println("Preferences started successfully!");
    // if (!preferences.getBool("firstTimeSetupComplete")) {
      Serial.println("Performing first time setup...");
      preferences.putBool("wallMounted", false);
      preferences.putBool("nightMode", false);
      preferences.putBool("clockEnabled", true);
      preferences.putBool("dotEnabled", true);
      preferences.putUShort("dotSize", 2); //dot radius
      preferences.putUShort("clockLocation", 1);
      preferences.putUShort("dotLocation", 0);

      preferences.putUShort("frame1Sensor", CO); 
      preferences.putUShort("frame2Sensor", AQI);

      preferences.putUShort("logoTime", 1);
      preferences.putUShort("refreshPeriod", 10); // Time in seconds
      preferences.putUShort("burninPeriod", 60);
      preferences.putUShort("indicatorPeriod", 4);

      preferences.putUShort("numRefreshCycles", 5);
      preferences.putBool("firstTimeSetupComplete", true);
      preferences.putBool("restoreFromBackupTime", true);
    // } 
    // else {
    //   Serial.println("Not performing first time setup...");
    // }

    if (preferences.getBool("startingFromOTA")) {
      Serial.println("Booting from fresh OTA firmware");
      Serial.printf("Version number: %d", VERSION_NUMBER);
      delay(3000);
      preferences.putBool("startingFromOTA", false);
    }

    return true;
  // }
  Serial.println("Error occured in starting preferences.");
  return false;
}