#include "settings.h"

struct_online online;


bool setupPreferences() {
  // Preferences is good for single KVP storage.
  // We want to use SPIFFS for large storage
  bool status =
      preferences.begin("HomeAirSettings", false); // false = not read-only
  delay(100);
  if (status) {
    online.pref = true;
    Serial.println("Preferences started successfully!");
    // TODO: ADD CHECK FOR FIRSTTIMESETUP BEFORE SETTING DEFAULT VALS
    preferences.putBool("restoreFromBackupTime", true);
    preferences.putBool("wallMounted", false);
    preferences.putBool("nightMode", false);
    preferences.putBool("clockEnabled", true);
    preferences.putBool("dotEnabled", true);
    preferences.putUShort("dotSize", 2); // dot radius
    preferences.putUShort("clockLocation", 1);
    preferences.putUShort("dotLocation", 0);

    preferences.putUShort("frame1Sensor", CO);
    preferences.putUShort("frame2Sensor", AQI);

    preferences.putBool("rotateFrames", true);

    preferences.putUShort("logoTime", 1);
    preferences.putUShort("refreshPeriod", 10); // Time in seconds
    preferences.putUShort("burninPeriod", 60);
    preferences.putUShort("indicatorPeriod", 4);

    preferences.putUShort("numRefreshCycles", 5);
    preferences.putBool("firstTimeSetupComplete", true);

    //SET TO FALSE TO SHOW PAIRING SCREEN (DEFAULT)
    preferences.putBool("skipPairingScreen", true);

    if (preferences.getBool("startingFromOTA")) {
      Serial.println("Booting from fresh OTA firmware");
      Serial.printf("Version number: %d", VERSION_NUMBER);
      delay(3000);
      preferences.putBool("startingFromOTA", false);
    }
    return true;
  }
  Serial.println("Error occured in starting preferences.");
  return false;
}