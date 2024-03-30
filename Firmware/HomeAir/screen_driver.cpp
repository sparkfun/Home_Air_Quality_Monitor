#include "screen_driver.h"

Screen_EPD_EXT3_Fast deviceScreen(eScreen_EPD_EXT3_266_0C_Fast, myMfnBoard);




void screendriverEpaperSetup() {
  #ifdef HOMEAIR_BOARD
    Serial.println("HOMEAIR_BOARD - TRUE");
  #else
    Serial.println("HOMEAIR_BOARD - FALSE");
  #endif
  #ifdef ANTON_BOARD
    Serial.println("Anton board!");
  #endif
  Serial.println("Setting up EPD");
  setupPreferences();
  deviceScreen.begin();
  // if(!deviceScreen.begin()) Serial.println("Error initiating EPD");
  deviceScreen.clear();
  // deviceScreen.regenerate();
  deviceScreen.globalRefresh(2);
  deviceScreen.setOrientation(0);       // Left-hand rotated Landscape
  deviceScreen.flushMode(UPDATE_FAST);  // Set Flush Mode
  deviceScreen.selectFont(1);
  deviceScreen.drawSparkfunLogo();
  Serial.println("Drew Sparkfun logo");
  deviceScreen.flush();
  // vTaskDelay(3000);
  vTaskDelay(1000 * preferences.getUShort("logoTime"));
  deviceScreen.setOrientation(1);
  deviceScreen.clear();
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
  // Shows the current time, if configured, in the upper right corner with 1 second accuracy in the format HH:MM:SS
  if (dateConfigured) {
    deviceScreen.gText(230, 5, String(rtc.getHour()) + ":" + String(rtc.getMinute()) + ":" + String(rtc.getSecond()));
  } else {
    deviceScreen.gText(230, 5, "Set Time!");
  }
}

void screendriverShowParingScreen(){
    uint8_t macOut[8];
    esp_err_t espErr = esp_efuse_mac_get_default(&macOut[0]);
    deviceScreen.gText(50, 25, "Awaiting BLE connection...");
    deviceScreen.gText(50, 50, "HomeAir-" + screendriverGetMacAddressLastFour());

    // if (espErr == ESP_OK) {
    //   deviceScreen.gText(50, 50, "HomeAir-%02hhx%02hhx", macOut[4], macOut[5]);
    // } else {
    //   Serial.println("unable to get mac address");
    //   deviceScreen.gText(50, 50, "HomeAir-%02hhx%02hhx", macOut[4], macOut[5]);
    // }

}

//For debugging purposes
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

void screendriverFlushWithChrono() {
  uint32_t chrono = millis();
  deviceScreen.flush();
  chrono = millis() - chrono;
  // Serial.printf("\tScreen refresh took %i\n", chrono);
}


// void globalRefresh() {
//   if (xSemaphoreTake(rawDataMutex, portMAX_DELAY)) {
//     deviceScreen.regenerate();
//   }
// }

void rotateFrames() {
  // CO2_PPM = 0,
  // PPM_1_0 = 1,// PPM_2_5 = 2,// PPM_4_0 = 3,// PPM_10 = 4,
  // HUMIDITY = 5, // TEMP = 6,
  // VOC = 7,

  // CO = 8,
  // NG = 9,
  // AQI = 10
  // NOX
  if(preferences.getUShort("frame1Sensor") == CO2_PPM) preferences.putUShort("frame1Sensor", PPM_2_5);
  else if(preferences.getUShort("frame1Sensor") == PPM_2_5) preferences.putUShort("frame1Sensor", HUMIDITY);
  else if(preferences.getUShort("frame1Sensor") == HUMIDITY) preferences.putUShort("frame1Sensor", VOC);
  else preferences.putUShort("frame1Sensor", CO2_PPM);

  if(preferences.getUShort("frame2Sensor") == CO) preferences.putUShort("frame2Sensor", NG);
  else if(preferences.getUShort("frame2Sensor") == NG) preferences.putUShort("frame2Sensor", AQI);

  // #ifdef USE_NOX
  // else if(preferences.getUShort("frame2Sensor") == AQI) preferences.putUShort("frame2Sensor", NOX);
  // #endif

  else preferences.putUShort("frame2Sensor", CO);
}

void updateSensorFrames() {
  if (xSemaphoreTake(rawDataMutex, portMAX_DELAY)) {
    deviceScreen.clear();
    // deviceScreen.drawSensorFrame(preferences.getUShort("frame1Sensor"), 0);
    // deviceScreen.drawSensorFrame(preferences.getUShort("frame2Sensor"), 1);
    for (int i = 0; i < 2; i++) {

      short currentSensor;

      if (i == 0) currentSensor = preferences.getUShort("frame1Sensor");
      else currentSensor = preferences.getUShort("frame2Sensor");

      if (currentSensor == TEMP || currentSensor == HUMIDITY) {
        deviceScreen.drawSensorFrame(mySensor.temperature, i);
        deviceScreen.updateFrameVal(i, mySensor.humidity, rawDataArray[HUMIDITY]);
        deviceScreen.updateFrameVal(i, mySensor.temperature, rawDataArray[TEMP]);
      }
      else if (currentSensor == CO) {
        deviceScreen.drawSensorFrame(mySensor.co, i);
        deviceScreen.updateFrameVal(i, mySensor.co, rawDataArray[CO]);
      }
      else if (currentSensor == NG) {
        deviceScreen.drawSensorFrame(mySensor.ch4, i);
        deviceScreen.updateFrameVal(i, mySensor.ch4, rawDataArray[NG]);
      }
      else if (currentSensor == CO2_PPM) {
        deviceScreen.drawSensorFrame(mySensor.co2, i);
        deviceScreen.updateFrameVal(i, mySensor.co2, rawDataArray[CO2_PPM]);
      }
      else if (currentSensor == VOC) {
        deviceScreen.drawSensorFrame(mySensor.voc, i);
        deviceScreen.updateFrameVal(i, mySensor.voc, rawDataArray[VOC]);
      }
      else if (currentSensor == AQI) {
        deviceScreen.drawSensorFrame(mySensor.aqi, i);
        deviceScreen.updateFrameVal(i, mySensor.aqi, rawDataArray[AQI]);
      }
      else if (currentSensor == PPM_2_5) {
        deviceScreen.drawSensorFrame(mySensor.particles, i);
        deviceScreen.updateFrameVal(i, mySensor.particles, rawDataArray[PPM_2_5]);
      }
      // else deviceScreen.updateFrameVal(i, currentSensor, String(rawDataArray[currentSensor]));
    }
    xSemaphoreGive(rawDataMutex);
  }
  #ifdef ROTATE_FRAMES
    rotateFrames();
  #endif
  // deviceScreen.flush();
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
      Serial.println("Warning: Invalid refresh indicator dot location");
    }
  }
  else {
    Serial.println("Warning: Dot not enabled but drawDot was called");
  }
  return;
}

void drawClock(bool indicatorOn) {
  uint8_t location = preferences.getUShort("clockLocation");
  uint16_t clockColor = myColours.white;
  if(preferences.getBool("clockEnabled")) {
    clockColor = indicatorOn ? myColours.black : myColours.white;
  }
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
    Serial.println("Warning: Invalid refresh indicator clock location");
  }

  return;
}

void firmwareUpdateScreen() {
  if(xSemaphoreTake(otaDownloadPercentageMutex, portMAX_DELAY)){
      deviceScreen.firmwareUpdateScreen(otaDownloadPercentage);
      xSemaphoreGive(otaDownloadPercentageMutex);
    } else {
      Serial.println("Couldn't acquire otaProgress semaphore!");
    }
}

int drawPairingScreen(int state) {
  deviceScreen.clear();
  // if(state == 1) {
  //   deviceScreen.drawText(25, 90, 3, "Pairing   ");
  // }
  deviceScreen.drawText(70, 50, 3, "HomeAir-" + screendriverGetMacAddressLastFour());
  if(state == 1) {
    deviceScreen.drawText(25, 90, 3, "Waiting to connect.  ");
  }
  else if(state == 2) {
    deviceScreen.drawText(25, 90, 3, "Waiting to connect.. ");
  }
  else if(state == 3) {
    deviceScreen.drawText(25, 90, 3, "Waiting to connect...");
    state = 0;
  }
  else {
    Serial.println("Warning: Invalid pairing state, resetting state to 0");
    state = 0;
  }
  state ++;
  return state;
}

int drawScreen(int state) {
  Serial.println("Ran drawScreen");
  // Sparkfun logo is drawn in epd setup function; first state drawn here is pairing screen
  if (xEventGroupGetBits(appStateFlagGroup) & APP_FLAG_SETUP) {
    // draw pairing screen
    #ifdef SHOW_READINGS_WITHOUT_CONNECTION
      updateSensorFrames();
    #else
      state = drawPairingScreen(state);
    #endif 
    deviceScreen.flush();
    return state;
  } else if (xEventGroupGetBits(appStateFlagGroup) & APP_FLAG_RUNNING) {
    // draw sensor screen
    // preferences.putUShort("refreshPeriod", preferences.getUShort("savedRefreshPeriod"));
    updateSensorFrames();
  } else if (xEventGroupGetBits(appStateFlagGroup) & APP_FLAG_OTA_DOWNLOAD) {
    // draw update screen
    firmwareUpdateScreen();
  } else {
    Serial.println("Warning: Unkown flag state in EPD");
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
      // Serial.print("EPD task loop ran, refreshFreq: "); 
      // Serial.print(refreshFreq);
      // Serial.print(", fullRefreshFreq: ");
      // Serial.print(fullRefreshFreq);
      // Serial.print(", indicatorFreq: ");
      // Serial.print(indicatorFreq);
      // Serial.print(", fullRefreshCounter: ");
      // Serial.println(fullRefreshCounter);

      fullRefreshFreq = preferences.getUShort("burninPeriod");
      if(fullRefreshFreq < 1) {
        fullRefreshFreq = 1;
        Serial.println("Warning: fullRefreshPeriod is 0. Adjusted to 1 to avoid div-by-zero.");
      }

      refreshFreq = preferences.getUShort("refreshPeriod");
      if(xEventGroupGetBits(appStateFlagGroup) & APP_FLAG_SETUP) refreshFreq = 1;
      if(xEventGroupGetBits(appStateFlagGroup) & APP_FLAG_OTA_DOWNLOAD) refreshFreq = 1;
      if(refreshFreq < 1) {
        refreshFreq = 1;
        Serial.println("Warning: refreshPeriod is 0. Adjusted to 1 to avoid div-by-zero.");
      }

      indicatorFreq = preferences.getUShort("indicatorPeriod");
      if(indicatorFreq < 1) {
        indicatorFreq = 1;
        Serial.println("Warning: indicatorPeriod is 0. Adjusted to 1 to avoid div-by-zero.");
      }
      
      
      deviceScreen.clear();

      //Burn-in prevention code
      if (fullRefreshCounter == 0)
      {
        //refreshing device screen
        deviceScreen.globalRefresh(2);
        pairingState = drawScreen(pairingState);
        // refreshCounter = 0;
      } 

      //Update screen
      if (refreshCounter == 0)
      {
        pairingState = drawScreen(pairingState);
        // screenUpdateCounter = 0;
      } 

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
        else {
          drawDot(false);
        }
      }

      indicatorCounter ++;
      indicatorCounter %= indicatorFreq;

      refreshCounter++;
      refreshCounter %= refreshFreq;

      fullRefreshCounter++;
      fullRefreshCounter %= fullRefreshFreq;

      //Wait 1 second
      vTaskDelay(1000);
    }
  }
}
