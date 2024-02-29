#include "settings.h"


struct_online online;

// display_settings epd_settings;

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
  // Preferences is good for single KVP storage.
  // We want to use SPIFFS for large storage
  bool status = preferences.begin("HomeAirSettings", false);
  if (status){
    if(!preferences.getBool("firstTimeSetupComplete"))
    {
      Serial.println("Preferences started successfully!");
      preferences.putBool("wallMounted", false);
      preferences.putBool("nightMode", false);
      preferences.putBool("clockEnabled", true);
      preferences.putBool("dotEnabled", true);
      preferences.putUShort("clockLocation", 1);
      preferences.putUShort("dotLocation", 0);
      preferences.putUShort("frame1Sensor", 1); // Add enum for reading those out more clearly
      preferences.putUShort("frame2Sensor", 5);
      preferences.putUShort("refreshPeriod", 5);
      preferences.putBool("firstTimeSetupComplete", true);
      return true;
    }
    if (preferences.getBool("startingFromOTA")){
      Serial.println("Booting from fresh OTA firmware");
      Serial.printf("Version number: %d", VERSION_NUMBER);
      preferences.putBool("startingFromOTA", false);
    }
  }
  Serial.println("Error occured in starting preferences.");
  return false;
  
}