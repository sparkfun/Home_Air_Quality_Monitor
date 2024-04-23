#include "settings.h"

struct_online online;

bool settings_setupPreferences() {
  // Preferences is good for single KVP storage.
  // We want to use SPIFFS for large storage
  bool status =
      preferences.begin("HomeAirSettings", false); // false = not read-only
  delay(100);
  if (status) {
    Serial.println("Initialized settings");
    online.pref = true;
    Serial.println("Preferences started successfully!");

    /***********************************************
    EPD SETTINGS
    ***********************************************/
    preferences.putBool("wallMounted", false);

    // Refresh settings
    preferences.putUShort("refreshPeriod", 4); // Time in seconds to update screen
    preferences.putUShort("burninPeriod", 60); // Time in seconds between global refreshes
    preferences.putUShort("refreshCycles", 5); // Number of cycles/flashes during global refresh
    preferences.putBool("adjustRefRate", true); // If enabled then refresh rate will be set to 1 second when pairing or updating

    preferences.putBool("skipPair", false); // Skip pairing/startup screen (DEFAULT FALSE)
    preferences.putUShort("logoTime", 1); // Time to show Sparkfun logo on powerup

    preferences.putUShort("frame1Sensor", AQI);
    preferences.putUShort("frame2Sensor", AQI);
    preferences.putBool("rotateFrames", false); // Whether to automatically cycle between sensors

    preferences.putBool("oneSensorOnly", true);
    preferences.putBool("showDeviceID", true); // Only shows when oneSensorOnly is true
    preferences.putBool("showBTStatus", true); // Only shows when oneSensorOnly is true

    // Indicator settings
    preferences.putUShort("indicatorPeriod", 4); // Frequency in seconds for dot/clock
    preferences.putBool("clockEnabled", false);
    preferences.putBool("dotEnabled", true);
    preferences.putUShort("dotSize", 2); // dot radius
    preferences.putUShort("clockLocation", 1);
    preferences.putUShort("dotLocation", 0);

    /************************************************
    OTHER SETTINGS
    ************************************************/
    // TODO: ADD CHECK FOR FIRSTTIMESETUP BEFORE SETTING DEFAULT VALS
    preferences.putBool("restoreFromBackupTime", true);

    preferences.putBool("firstTimeSetupComplete", true);

    preferences.putUShort("sensorReadPeriod", 15);
    preferences.putUShort("MQ_enabled", 1);
    preferences.putUShort("MQ_OnPeriod", 90);
    preferences.putUShort("MQ_Offperiod", 60);

    const char *noneString = "NONE";
    // preferences.putString("customBLEName", noneString);

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

bool settings_setPrefsToDefault() {
  if (online.pref) {
    // TODO: ADD CHECK FOR FIRSTTIMESETUP BEFORE SETTING DEFAULT VALS
    preferences.putBool("restFromBackup", true);
    preferences.putBool("wallMounted", false);
    preferences.putBool("nightMode", false);
    preferences.putBool("clockEnabled", true);
    preferences.putBool("dotEnabled", true);
    preferences.putUShort("dotSize", 2); // dot radius
    preferences.putUShort("clockLocation", 1);
    preferences.putUShort("dotLocation", 0);

    preferences.putUShort("frame1Sensor", CO);
    preferences.putUShort("frame2Sensor", AQI);

    preferences.putUShort("logoTime", 1);
    preferences.putUShort("refreshPeriod", 10); // Time in seconds
    preferences.putUShort("burninPeriod", 60);
    preferences.putUShort("indicatorPeriod", 4);

    preferences.putUShort("numRefreshCycles", 5);
    preferences.putBool("setupComplete", true);

    preferences.putUShort("ReadPeriod", 15);
    preferences.putUShort("MQ_enabled", 1);
    preferences.putUShort("MQ_OnPeriod", 90);
    preferences.putUShort("MQ_Offperiod", 60);

    preferences.putString("customBLEName", "NONE");

    xEventGroupSetBits(appStateFlagGroup, APP_FLAG_FACTORY_ROLLBACK);

  } else {
    DBG("Prefs not set up - settings not cleared");
  }
}