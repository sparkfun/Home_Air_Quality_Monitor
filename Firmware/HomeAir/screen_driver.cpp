#include "screen_driver.h"

Screen_EPD_EXT3_Fast deviceScreen(eScreen_EPD_EXT3_266_0C_Fast, myMfnBoard);




void screendriverEpaperSetup() {
  deviceScreen.begin();
  deviceScreen.clear();
  deviceScreen.regenerate();
  deviceScreen.setOrientation(3);       // Left-hand rotated Landscape
  deviceScreen.flushMode(UPDATE_FAST);  // Set Flush Mode
  deviceScreen.selectFont(1);
  deviceScreen.drawSparkfunLogo();
  deviceScreen.flush();
  vTaskDelay(1000 * epd_settings.logoTime);
}



String screendriverGetMacAddress() {
  uint8_t macOut[8];
  esp_err_t espErr = esp_efuse_mac_get_default(&macOut[0]);
  if (espErr == ESP_OK) {
    char strOut[19];
    snprintf(strOut, 19, "%02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx", macOut[0], macOut[1], macOut[2], macOut[3], macOut[4], macOut[5]);
    return String(strOut);
  } else {
    Serial.println("Mac address failed to be read");
    return "";
  }
}

void screendriverPrintMacAddress() {
  Serial.println("MAC Address: " + screendriverGetMacAddress());
}

void screendriverShowTime() {
  // Shows the current time, if configured, in the upper right corner with 1 second accuracy in the format HH:MM:SS
  if (dateConfigured) {
    deviceScreen.gText(230, 5, String(rtc.getHour()) + ":" + String(rtc.getMinute()) + ":" + String(rtc.getSecond()));
  } else {
    deviceScreen.gText(230, 5, "Set Time!");
  }
}


//For debugging purposes
void screendriverShowDetailedMeasurements() {
  if (xSemaphoreTake(rawDataMutex, portMAX_DELAY)) {
    deviceScreen.gText(5, 5, "CO2 PPM: " + String(rawDataArray[CO2_PPM]));
    // deviceScreen.gText(5, 5, "Counter: " + String(i++));
    deviceScreen.gText(5, 20, "Temperature: " + String(rawDataArray[TEMP]));
    deviceScreen.gText(5, 35, "Humidity: " + String(rawDataArray[HUMIDITY]));
    deviceScreen.gText(5, 50, "CO (PPM): " + String(rawDataArray[CO]));
    deviceScreen.gText(5, 65, "CH4 (PPM): " + String(rawDataArray[NG]));
    deviceScreen.gText(5, 80, "AQI: " + String(rawDataArray[AQI]));
    deviceScreen.gText(5, 120, "MAC: " + screendriverGetMacAddress());
    xSemaphoreGive(rawDataMutex);
  }
}

void screendriverFlushWithChrono() {
  uint32_t chrono = millis();
  deviceScreen.flush();
  chrono = millis() - chrono;
  // Serial.printf("\tScreen refresh took %i\n", chrono);
}


void globalRefresh() {
  if (xSemaphoreTake(rawDataMutex, portMAX_DELAY)) {
    deviceScreen.regenerate();
  }
}


void updateSensorFrames() {
  if (xSemaphoreTake(rawDataMutex, portMAX_DELAY)) {
    deviceScreen.drawSensorFrame(preferences.getUShort("frame1Sensor"), 0);
    deviceScreen.drawSensorFrame(preferences.getUShort("frame2Sensor"), 1);
    for (int i = 0; i < 2; i++) {

      uint8_t currentSensor;

      if (i == 0) currentSensor = preferences.getUShort("frame1Sensor");
      else currentSensor = preferences.getUShort("frame2Sensor");

      if (currentSensor == mySensor.temperature || currentSensor == mySensor.humidity) {
        deviceScreen.updateFrameVal(i, mySensor.humidity, String(rawDataArray[5]));
        deviceScreen.updateFrameVal(i, mySensor.temperature, String(rawDataArray[6]));
      } else if (currentSensor == mySensor.co2) deviceScreen.updateFrameVal(i, mySensor.humidity, String(rawDataArray[HUMIDITY]));
      else if (currentSensor == mySensor.co) deviceScreen.updateFrameVal(i, mySensor.co, String(rawDataArray[CO]));
      else if (currentSensor == mySensor.ch4) deviceScreen.updateFrameVal(i, mySensor.ch4, String(rawDataArray[NG]));
      else if (currentSensor == mySensor.co2) deviceScreen.updateFrameVal(i, mySensor.co2, String(rawDataArray[CO2_PPM]));
      else if (currentSensor == mySensor.voc) deviceScreen.updateFrameVal(i, mySensor.voc, String(rawDataArray[VOC]));
      else if (currentSensor == mySensor.aqi) deviceScreen.updateFrameVal(i, mySensor.aqi, String(rawDataArray[AQI]));
      else if (currentSensor == mySensor.particles) deviceScreen.updateFrameVal(i, mySensor.particles, String(rawDataArray[PPM_2_5]));
    }
    xSemaphoreGive(rawDataMutex);
  }
  deviceScreen.flush();
}

void drawDot(bool indicatorOn) {
  if(preferences.getBool("dotEnabled")) {
    uint8_t location = preferences.getUShort("dotLocation");
    uint16_t dotColor = indicatorOn ? myColours.black : myColours.white;
    if(location == 0) {
      deviceScreen.circle(5, 5, preferences.getUShort("dotSize"), dotColor);
    }
    else if(location == 1) {
      deviceScreen.circle(291, 5, preferences.getUShort("dotSize"), dotColor);
    }
    else if(location == 2) {
      deviceScreen.circle(5, 147, preferences.getUShort("dotSize"), dotColor);
    }
    else if(location == 3) {
      deviceScreen.circle(291, 147, preferences.getUShort("dotSize"), dotColor);
    }
    else {
      Serial.println("Invalid refresh indicator dot location");
    }
  }
  else {
    Serial.println("Dot not enabled but drawDot was called");
  }
  return;
}

void drawClock(bool indicatorOn) {
  uint8_t location = preferences.getUShort("clockLocation");
  if(preferences.getBool("clockEnabled")) {
    uint16_t clockColor = indicatorOn ? myColours.black : myColours.white;
    if(location == 0) {
      deviceScreen.drawText(5, 5, 1, String(rtc.getHour()) + ":" + String(rtc.getMinute()) + ":" + String(rtc.getSecond()), clockColor);
    }
    else if(location == 1) {
      deviceScreen.drawText(230, 5, 1, String(rtc.getHour()) + ":" + String(rtc.getMinute()) + ":" + String(rtc.getSecond()), clockColor);
    }
    else if(location == 2) {
      deviceScreen.drawText(5, 139, 1, String(rtc.getHour()) + ":" + String(rtc.getMinute()) + ":" + String(rtc.getSecond()), clockColor);
    }
    else if(location == 3) {
      deviceScreen.drawText(230, 139, 1, String(rtc.getHour()) + ":" + String(rtc.getMinute()) + ":" + String(rtc.getSecond()), clockColor);
    }
    else {
      Serial.println("Invalid refresh indicator clock location");
    }
  }
  else {
    Serial.println("Clock not enabled but drawClock was called");
  }
  return;
}

void firmwareUpdateScreen() {
  deviceScreen.firmwareUpdateScreen(otaDownloadPercentage);
}

int drawPairingScreen(int state) {
  if(state == 1) {
    deviceScreen.drawText(25, 90, 3, "Pairing   ");
  }
  else if(state == 2) {
    deviceScreen.drawText(25, 90, 3, "Pairing.  ");
  }
  else if(state == 3) {
    deviceScreen.drawText(25, 90, 3, "Pairing.. ");
  }
  else if(state == 4) {
    deviceScreen.drawText(25, 90, 3, "Pairing...");
    state = 0;
  }
  else {
    Serial.println("Invalid pairing state, resetting state");
    state = 0;
  }
  state ++;
  return state;
}

int drawScreen(int state) {
  // Sparkfun logo is drawn in epd setup function; first state drawn here is pairing screen
  if (xEventGroupGetBits(appStateFlagGroup) & APP_FLAG_SETUP) {
    // draw pairing screen
    preferences.putUShort("refreshPeriod", 1);
    state = drawPairingScreen(state);
    return state;
  } else if (xEventGroupGetBits(appStateFlagGroup) & APP_FLAG_RUNNING) {
    // draw sensor screen
    preferences.putUShort("refreshPeriod", preferences.getUShort("savedRefreshPeriod"));
    updateSensorFrames();
  } else if (xEventGroupGetBits(appStateFlagGroup) & APP_FLAG_OTA_DOWNLOAD) {
    // draw update screen
    // set refresh time to 1 for update
    preferences.putUShort("refreshPeriod", 1);
    firmwareUpdateScreen();
  } else {
    Serial.println("Unkown flag state in EPD");
    return 0;
  }
  return 0;
  // myScreen.flush();
  // return;
}

void screendriverRunScreenTask(void *pvParameter) {
  screendriverPrintMacAddress();
  screendriverEpaperSetup();

  //Counter for burn-in prevention
  volatile int refreshCounter = 0;
  volatile int screenUpdateCounter = 0;
  volatile int indicatorCounter = 0;
  volatile int pairingState = 0;
  volatile bool refreshIndicatorOn = false;
  while (1) {
    {
      // Prologue
      // deviceScreen.clear();
      // Function Body
      screendriverShowTime();
      screendriverShowDetailedMeasurements();
      // Epilogue
      screendriverFlushWithChrono();

      //Burn-in prevention code
      if (refreshCounter == 0) deviceScreen.globalRefresh(preferences.getUShort("numRefreshCycles"));
      refreshCounter++;
      refreshCounter %= preferences.getUShort("refreshPeriod");

      //Update screen
      if (refreshCounter == 0) pairingState = drawScreen(pairingState);
      screenUpdateCounter ++;
      screenUpdateCounter %= preferences.getUShort("cyclesBetweenFullRefresh");

      //Clock/dot toggle code
      if (indicatorCounter == 0) {
        if(preferences.getBool("dotEnabled")) {
          drawDot(refreshIndicatorOn);
          indicatorCounter = !indicatorCounter;
        }
        else if(preferences.getBool("clockEnabled") && preferences.getBool("dotEnabled")) {
          drawClock(refreshIndicatorOn);
        }
        else if(preferences.getBool("clockEnabled") && !preferences.getBool("dotEnabled")) {
          drawClock(refreshIndicatorOn);
          indicatorCounter = !indicatorCounter;
        }
      }
      indicatorCounter ++;
      screenUpdateCounter %= preferences.getUShort("indicatorPeriod");

      //Wait 1 second
      vTaskDelay(1000);
    }
  }
}
