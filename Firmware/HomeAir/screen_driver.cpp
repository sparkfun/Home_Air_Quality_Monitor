#include "screen_driver.h"

Screen_EPD_EXT3_Fast deviceScreen(eScreen_EPD_EXT3_266_0C_Fast, myMfnBoard);




void screendriverEpaperSetup() {
  #ifdef TEST_BOARD
    Serial.println("TEST_BOARD - TRUE");
  #else
    Serial.println("TEST_BOARD - FALSE");
  #endif
  deviceScreen.begin();
  deviceScreen.clear();
  deviceScreen.regenerate();
  deviceScreen.setOrientation(3);       // Left-hand rotated Landscape
  deviceScreen.flushMode(UPDATE_FAST);  // Set Flush Mode
  deviceScreen.selectFont(1);
  deviceScreen.drawSparkfunLogo();
  deviceScreen.flush();
  vTaskDelay(1000 * epd_settings.logoTime);
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
    deviceScreen.gText(50, 25, "Awaiting BLE connection...");
    uint8_t macOut[8];
    esp_err_t espErr = esp_efuse_mac_get_default(&macOut[0]);
    if (espErr == ESP_OK) {
    deviceScreen.gText(50, 50, "HomeAir-%02hhx%02hhx", macOut[4], macOut[5]);
    }

}

nj6a2]
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

void firmwareUpdateScreen() {
  deviceScreen.firmwareUpdateScreen(otaDownloadPercentage);
}

void drawPairingScreen() {
}

void drawScreen() {
  // Sparkfun logo is drawn in epd setup function; first state drawn here is pairing screen
  if (xEventGroupGetBits(appStateFlagGroup) & APP_FLAG_SETUP) {
    // draw pairing screen
    drawPairingScreen();
  } else if (xEventGroupGetBits(appStateFlagGroup) & APP_FLAG_RUNNING) {
    // draw sensor screen
    updateSensorFrames();
  } else if (xEventGroupGetBits(appStateFlagGroup) & APP_FLAG_OTA_DOWNLOAD) {
    // draw update screen
    firmwareUpdateScreen();
  } else {
    Serial.println("Unkown flag state in EPD");
    return;
  }
  // myScreen.flush();
  // return;
}

void screendriverRunScreenTask(void *pvParameter) {
  screendriverPrintMacAddress();
  screendriverEpaperSetup();

  //Counter for burn-in prevention
  volatile int refreshCounter = 0;
  while (1) {
    {
      // Prologue
      deviceScreen.clear();
      // Function Body
      screendriverShowTime();
      // screendriverShowDetailedMeasurements();
      screendriverShowParingScreen();
      // Epilogue
      screendriverFlushWithChrono();

      // drawScreen();

      //Burn-in prevention code
      refreshCounter++;
      refreshCounter %= epd_settings.cyclesBetweenFullRefresh;
      if (refreshCounter == 0) deviceScreen.globalRefresh(epd_settings.numRefreshCycles);

      vTaskDelay(preferences.getUShort("refreshPeriod") * 1000);
    }
  }
}
