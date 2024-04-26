#include "screen_driver.h"

Screen_EPD_EXT3_Fast deviceScreen(eScreen_EPD_EXT3_266_0C_Fast, myMfnBoard);

void screendriverEpaperSetup() {
#ifdef HOMEAIR_BOARD
  Serial.println("HOMEAIR_BOARD - TRUE");
#else
  Serial.println("HOMEAIR_BOARD - FALSE");
#endif
#ifdef FINAL_HARDWARE
  Serial.println("Final hardware!");
#endif
  Serial.println("Setting up EPD");
  deviceScreen.begin();
  deviceScreen.clear();
  deviceScreen.globalRefresh(2);
  if(preferences.getBool("wallMounted")) deviceScreen.setOrientation(2);
  else deviceScreen.setOrientation(0);
  // deviceScreen.setOrientation(0);      // Left-hand rotated Landscape
  deviceScreen.flushMode(UPDATE_FAST); // Set Flush Mode
  deviceScreen.selectFont(1);
  deviceScreen.drawSparkfunLogo();
  Serial.println("Drew Sparkfun logo");
  deviceScreen.flush();
  vTaskDelay(1000 * preferences.getUShort("logoTime"));
  if(preferences.getBool("wallMounted")) deviceScreen.setOrientation(3);
  else deviceScreen.setOrientation(1);
  deviceScreen.clear();
}

String screendriverGetMacAddress() {
  uint8_t macOut[8];
  esp_err_t espErr = esp_efuse_mac_get_default(&macOut[0]);
  if (espErr == ESP_OK) {
    char strOut[19];
    snprintf(strOut, 19, "%02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx", macOut[0],
             macOut[1], macOut[2], macOut[3], macOut[4], macOut[5]);
    return String(strOut);
  } else {
    Serial.println("Mac address failed to be read");
    return "";
  }
}

String screendriverGetMacAddressLastFour() {
  uint8_t macOut[8];
  esp_err_t espErr = esp_efuse_mac_get_default(&macOut[0]);
  if (espErr == ESP_OK) {
    char strOut[19];
    snprintf(strOut, 19, "%02hhX%02hhX", macOut[4], macOut[5]);
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
  // Shows the current time, if configured, in the upper right corner with 1
  // second accuracy in the format HH:MM:SS
  if (dateConfigured) {
    deviceScreen.gText(230, 5,
                       String(rtc.getHour()) + ":" + String(rtc.getMinute()) +
                           ":" + String(rtc.getSecond()));
  } else {
    deviceScreen.gText(230, 5, "Set Time!");
  }
}

// For debugging purposes
void screendriverShowDetailedMeasurements() {
  if (xSemaphoreTake(rawDataMutex, portMAX_DELAY)) {
    deviceScreen.gText(5, 5, "CO2 PPM: " + String(rawDataArray[CO2_PPM]));
    deviceScreen.gText(5, 20, "Temperature: " + String(rawDataArray[TEMP]));
    deviceScreen.gText(5, 35, "Humidity: " + String(rawDataArray[HUMIDITY]));
    deviceScreen.gText(5, 50, "CO (PPM): " + String(rawDataArray[CO]));
    deviceScreen.gText(5, 65, "CH4 (PPM): " + String(rawDataArray[NG]));
    deviceScreen.gText(5, 80, "AQI: " + String(rawDataArray[AQI]));
    deviceScreen.gText(5, 95, "PPM2.5: " + String(rawDataArray[PPM_2_5]));
    deviceScreen.gText(5, 110, "VOC: " + String(rawDataArray[VOC]));
    deviceScreen.gText(5, 125, "NOX: " + String(rawDataArray[NOX]));
    deviceScreen.gText(5, 140, "MAC: " + screendriverGetMacAddress());
    xSemaphoreGive(rawDataMutex);
  }
}

void rotateFrames() {

  /*
  CO2_PPM = 0,
  PPM_1_0 = 1,
  PPM_2_5 = 2,
  PPM_4_0 = 3,
  PPM_10 = 4,
  HUMIDITY = 5,
  TEMP = 6,
  VOC = 7,
  NOX = 8,
  CO = 9,
  NG = 10,
  AQI = 11
  */
  if (preferences.getBool("oneSensorOnly")) {
    if (preferences.getUShort("frame1Sensor") == CO2_PPM)
      preferences.putUShort("frame1Sensor", PPM_2_5);
    else if (preferences.getUShort("frame1Sensor") < HUMIDITY)
      preferences.putUShort("frame1Sensor", HUMIDITY);
    else if (preferences.getUShort("frame1Sensor") >= AQI)
      preferences.putUShort("frame1Sensor", CO2_PPM);
    else
      preferences.putUShort("frame1Sensor",
                            (preferences.getUShort("frame1Sensor") + 1));
  } else {
    if (preferences.getUShort("frame1Sensor") == CO2_PPM)
      preferences.putUShort("frame1Sensor", PPM_2_5);
    else if (preferences.getUShort("frame1Sensor") == PPM_2_5)
      preferences.putUShort("frame1Sensor", HUMIDITY);
    else if (preferences.getUShort("frame1Sensor") == HUMIDITY)
      preferences.putUShort("frame1Sensor", VOC);
    else
      preferences.putUShort("frame1Sensor", CO2_PPM);

    if (preferences.getUShort("frame2Sensor") == NOX)
      preferences.putUShort("frame2Sensor", CO);
    else if (preferences.getUShort("frame2Sensor") == CO)
      preferences.putUShort("frame2Sensor", NG);
    else if (preferences.getUShort("frame2Sensor") == NG)
      preferences.putUShort("frame2Sensor", AQI);
    else
      preferences.putUShort("frame2Sensor", NOX);
  }

  Serial.print("Frame 1 sensor: ");
  Serial.println(preferences.getUShort("frame1Sensor"));
  Serial.print("Frame 2 sensor: ");
  Serial.println(preferences.getUShort("frame2Sensor"));
}

void updateSensorFrames() {
  if (xSemaphoreTake(rawDataMutex, portMAX_DELAY)) {
    deviceScreen.clear();
    for (int i = 0; i < 2; i++) {

      short currentSensor;

      if (i == 0)
        currentSensor = preferences.getUShort("frame1Sensor");
      else
        currentSensor = preferences.getUShort("frame2Sensor");
      if (preferences.getBool("oneSensorOnly")) {
        i = 3;
      }
      // Serial.print("Current frame number: ");
      // Serial.println(i);
      if (currentSensor == TEMP || currentSensor == HUMIDITY) {
        if (i == 3) {
          if (currentSensor == TEMP) {
            deviceScreen.drawSensorFrame(mySensor.tempBig, i);
            deviceScreen.updateFrameVal(i, mySensor.temperature,
                                        rawDataArray[TEMP]);
          } else if (currentSensor == HUMIDITY) {
            deviceScreen.drawSensorFrame(mySensor.humidityBig, i);
            deviceScreen.updateFrameVal(i, mySensor.humidity,
                                        rawDataArray[HUMIDITY]);
          }
        } else {
          deviceScreen.drawSensorFrame(mySensor.temperature, i);
          deviceScreen.updateFrameVal(i, mySensor.humidity,
                                      rawDataArray[HUMIDITY]);
          deviceScreen.updateFrameVal(i, mySensor.temperature,
                                      rawDataArray[TEMP]);
        }
      } else if (currentSensor == CO) {
        deviceScreen.drawSensorFrame(mySensor.co, i);
        deviceScreen.updateFrameVal(i, mySensor.co, rawDataArray[CO]);
      } else if (currentSensor == NG) {
        deviceScreen.drawSensorFrame(mySensor.ch4, i);
        deviceScreen.updateFrameVal(i, mySensor.ch4, rawDataArray[NG]);
      } else if (currentSensor == CO2_PPM) {
        deviceScreen.drawSensorFrame(mySensor.co2, i);
        deviceScreen.updateFrameVal(i, mySensor.co2, rawDataArray[CO2_PPM]);
      } else if (currentSensor == VOC) {
        deviceScreen.drawSensorFrame(mySensor.voc, i);
        deviceScreen.updateFrameVal(i, mySensor.voc, rawDataArray[VOC]);
      } else if (currentSensor == AQI) {
        deviceScreen.drawSensorFrame(mySensor.aqi, i);
        deviceScreen.updateFrameVal(i, mySensor.aqi, rawDataArray[AQI]);
      } else if (currentSensor == PPM_2_5) {
        deviceScreen.drawSensorFrame(mySensor.particles, i);
        deviceScreen.updateFrameVal(i, mySensor.particles,
                                    rawDataArray[PPM_2_5]);
      }
      if (i == 3) {
        if (preferences.getBool("showDeviceID")) {
          if(preferences.getString("customBLEName") != "NONE"){
            int leftShift = (strlen(preferences.getString("customBLEName").c_str()) - 12) * 12; // *12 for character width
            deviceScreen.drawText(146 - leftShift, 4, 3, preferences.getString("customBLEName"));
          } else {
            deviceScreen.drawText(
              146, 4, 3, "HomeAir-" + screendriverGetMacAddressLastFour());
          }
        }
        if (xEventGroupGetBits(BLEStateFlagGroup) & BLE_FLAG_CLIENT_CONNECTED)
          deviceScreen.drawBluetoothConnected(true);
        else
          deviceScreen.drawBluetoothConnected(false);
      }
      xSemaphoreGive(rawDataMutex);
    }
    if (preferences.getBool("rotateFrames")) {
      rotateFrames();
    }
  }
}

void testFrames() {
  // deviceScreen.drawSensorFrame(mySensor.co2, 1);
  // deviceScreen.updateFrameVal(1, mySensor.co2, 1234);

  Serial.println("Drawing test frame");
  deviceScreen.drawUnits(mySensor.temperature, 2, 100, 55);
  // deviceScreen.drawPairingScreenBitmap(1);

  // deviceScreen.setOrientation(2);
  // deviceScreen.drawUnits(mySensor.humidity, 2, 0, 0);
  // deviceScreen.drawText(146, 126, 3, "HomeAir-" +
  // screendriverGetMacAddressLastFour()); deviceScreen.setOrientation(1);
}

void drawDot(bool indicatorOn) {
  if (preferences.getBool("dotEnabled")) {
    uint8_t location = preferences.getUShort("dotLocation");
    uint16_t dotColor = indicatorOn ? myColours.black : myColours.white;
    if (location == 0) {
      deviceScreen.circle(5, 5, preferences.getUShort("dotSize"), dotColor);
    } else if (location == 1) {
      deviceScreen.circle(291, 5, preferences.getUShort("dotSize"), dotColor);
    } else if (location == 2) {
      deviceScreen.circle(5, 147, preferences.getUShort("dotSize"), dotColor);
    } else if (location == 3) {
      deviceScreen.circle(291, 147, preferences.getUShort("dotSize"), dotColor);
    } else {
      Serial.println("Warning: Invalid refresh indicator dot location");
    }
  } else {
    Serial.println("Warning: Dot not enabled but drawDot was called");
  }
  return;
}

void drawClock(bool indicatorOn) {
  uint8_t location = preferences.getUShort("clockLocation");
  uint16_t clockColor = myColours.white;
  if (preferences.getBool("clockEnabled")) {
    clockColor = indicatorOn ? myColours.black : myColours.white;
  }
  if (location == 0) {
    deviceScreen.drawText(5, 5, 1,
                          String(rtc.getHour()) + ":" +
                              String(rtc.getMinute()) + ":" +
                              String(rtc.getSecond()),
                          clockColor);
  } else if (location == 1) {
    deviceScreen.drawText(230, 5, 1,
                          String(rtc.getHour()) + ":" +
                              String(rtc.getMinute()) + ":" +
                              String(rtc.getSecond()),
                          clockColor);
  } else if (location == 2) {
    deviceScreen.drawText(5, 139, 1,
                          String(rtc.getHour()) + ":" +
                              String(rtc.getMinute()) + ":" +
                              String(rtc.getSecond()),
                          clockColor);
  } else if (location == 3) {
    deviceScreen.drawText(230, 139, 1,
                          String(rtc.getHour()) + ":" +
                              String(rtc.getMinute()) + ":" +
                              String(rtc.getSecond()),
                          clockColor);
  } else {
    Serial.println("Warning: Invalid refresh indicator clock location");
  }

  return;
}

void firmwareUpdateScreen() {
  if (xSemaphoreTake(otaDownloadPercentageMutex, 0)) {
    deviceScreen.firmwareUpdateScreen(otaDownloadPercentage);
    xSemaphoreGive(otaDownloadPercentageMutex);
  } else {
    Serial.println("Couldn't acquire otaProgress mutex!");
  }
}

void dataUploadScreen() {
  // if (xSemaphoreTake(otaDownloadPercentageMutex, 0)) {
  deviceScreen.dataUploadScreen(uploadPercentage);
  //   xSemaphoreGive(otaDownloadPercentageMutex);
  // } else {
  //   Serial.println("Couldn't acquire otaProgress mutex!");
  // }
}

int drawPairingScreen(int state) {
  deviceScreen.setOrientation(1);
  deviceScreen.clear();
  if (state == 1) {
    // Serial.print("Pairing state: ");
    // Serial.println(state);
    deviceScreen.drawPairingScreenBitmap1Dot();
    // state ++;
  } else if (state == 2) {
    deviceScreen.drawPairingScreenBitmap2Dot();
    // state ++;
  } else if (state == 3) {
    deviceScreen.drawPairingScreenBitmap3Dot();
    state = 0;
  } else {
    Serial.println("Warning: Invalid pairing state, resetting state to 0");
    state = 0;
  }
  state++;
  if(preferences.getString("customBLEName") != "NONE"){
    // Display MAC with custom name
    deviceScreen.drawText(146, 126, 3,
                        preferences.getString("customBLEName"));
  } else {
    deviceScreen.drawText(146, 126, 3,
                        "HomeAir-" + screendriverGetMacAddressLastFour());
  }
  
  return state;
}

int drawScreen(int state) {
  // Serial.println("Ran drawScreen");
  // Sparkfun logo is drawn in epd setup function; first state drawn here is
  // pairing screen
  
  if(preferences.getBool("wallMounted")) deviceScreen.setOrientation(3);
  else deviceScreen.setOrientation(1);

  uint32_t eventBits = xEventGroupGetBits(appStateFlagGroup);
  if (eventBits & APP_FLAG_SETUP && !(eventBits & APP_FLAG_BYPASS_SETUP)) {
    // draw pairing screen
    bool skipPair = preferences.getBool("skipPair");
    if (skipPair) {
#ifndef FRAME_TESTING_MODE
      updateSensorFrames();
#else
      testFrames();
#endif
    } else {
      state = drawPairingScreen(state);
    }

    deviceScreen.flush();
    return state;
  } else if (xEventGroupGetBits(appStateFlagGroup) &
             (APP_FLAG_RUNNING | APP_FLAG_TRANSMITTING | APP_FLAG_BYPASS_SETUP)) {
#ifndef FRAME_TESTING_MODE
    updateSensorFrames();
#else
    testFrames();
#endif
  } else if (xEventGroupGetBits(appStateFlagGroup) & APP_FLAG_OTA_DOWNLOAD) {
    // draw update screen
    firmwareUpdateScreen();
  } else if (xEventGroupGetBits(appStateFlagGroup) & APP_FLAG_TRANSMITTING) {
    dataUploadScreen();
  } else {
    Serial.println("Warning: Unkown flag state in EPD, reverting to displaying "
                   "sensor data");
    updateSensorFrames();
    printCurrentAppFlagStatus();
  }
  deviceScreen.flush();
  return 0;
  // myScreen.flush();
  // return;
}

void screendriverRunScreenTask(void *pvParameter) {
  screendriverPrintMacAddress();
  screendriverEpaperSetup();

  volatile int refreshCounter = 1;
  volatile int fullRefreshCounter = 1;
  volatile int indicatorCounter = 1;
  volatile int pairingState = 1;
  volatile bool refreshIndicatorOn = false;

  // variables for div-by-zero error check/prevention
  volatile short refreshFreq = 1;
  volatile short fullRefreshFreq = 1;
  volatile short indicatorFreq = 1;
  while (1) {
    {

      fullRefreshFreq = preferences.getUShort("burninPeriod");
      if (fullRefreshFreq < 1) {
        fullRefreshFreq = 1;
        Serial.println("Warning: fullRefreshPeriod is 0. Adjusted to 1 to "
                       "avoid div-by-zero.");
      }

      refreshFreq = preferences.getUShort("refreshPeriod");
      bool skipPair = preferences.getBool("skipPair");

      if (preferences.getBool("adjustRefRate")) {
        if (xEventGroupGetBits(appStateFlagGroup) & APP_FLAG_SETUP) {
          if (!skipPair)
            refreshFreq = 1;
        }
        if (xEventGroupGetBits(appStateFlagGroup) & APP_FLAG_OTA_DOWNLOAD) {
          refreshFreq = 1;
        }
        if (xEventGroupGetBits(appStateFlagGroup) & APP_FLAG_TRANSMITTING) {
          refreshFreq = 1;
        }
      }

      if (refreshFreq < 1) {
        refreshFreq = 1;
        Serial.println(
            "Warning: refreshPeriod is 0. Adjusted to 1 to avoid div-by-zero.");
      }

      indicatorFreq = preferences.getUShort("indicatorPeriod");
      if (indicatorFreq < 1) {
        indicatorFreq = 1;
        Serial.println("Warning: indicatorPeriod is 0. Adjusted to 1 to avoid "
                       "div-by-zero.");
      }

      deviceScreen.clear();

      // Burn-in prevention code
      if (fullRefreshCounter == 0) {
        // refreshing device screen
        deviceScreen.globalRefresh(2);
        pairingState = drawScreen(pairingState);
        // refreshCounter = 0;
      }

      // Update screen
      if (refreshCounter == 0) {
        pairingState = drawScreen(pairingState);
        // screenUpdateCounter = 0;
      }

      // Clock/dot toggle code
      if (indicatorCounter == 0) {
        if (preferences.getBool("dotEnabled")) {
          drawDot(refreshIndicatorOn);
          indicatorCounter = !indicatorCounter;
        } else if (preferences.getBool("clockEnabled") &&
                   preferences.getBool("dotEnabled")) {
          drawClock(refreshIndicatorOn);
        } else if (preferences.getBool("clockEnabled") &&
                   !preferences.getBool("dotEnabled")) {
          drawClock(refreshIndicatorOn);
          indicatorCounter = !indicatorCounter;
        } else {
          drawDot(false);
        }
      }

      indicatorCounter++;
      indicatorCounter %= indicatorFreq;

      refreshCounter++;
      refreshCounter %= refreshFreq;

      fullRefreshCounter++;
      fullRefreshCounter %= fullRefreshFreq;

      EventBits_t uxBits =
          xEventGroupWaitBits(appStateFlagGroup, APP_FLAG_EPD_FORCE_UPDATE,
                              pdTRUE, pdFALSE, 1000 / portTICK_PERIOD_MS);
      if (uxBits & APP_FLAG_EPD_FORCE_UPDATE) {
        Serial.println("Button pressed - EPD update forced");
        DBG("EPD Force Update flag set");
        if (!preferences.getBool("rotateFrames"))
          rotateFrames();   // If rotateFrames bool is true then sensor will be
                            // rotated after refreshing anyway so rotateFrames
                            // should not be called
        refreshCounter = 0; // Force immediate update
      }
    }
  }
}
