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
  vTaskDelay(3000);
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

void screendriverShowTime(){
  // Shows the current time, if configured, in the upper right corner with 1 second accuracy in the format HH:MM:SS
  if (dateConfigured){
    deviceScreen.gText(230, 5, String(rtc.getHour()) + ":" + String(rtc.getMinute()) + ":" + String(rtc.getSecond()));
  } else {
    deviceScreen.gText(230, 5, "Set Time!");
  }
}

void screendriverShowDetailedMeasurements(){
  if(xSemaphoreTake(rawDataMutex, portMAX_DELAY)){
    deviceScreen.gText(5, 5, "CO2 PPM: " + String(rawDataArray[0]));
    // deviceScreen.gText(5, 5, "Counter: " + String(i++));
    deviceScreen.gText(5, 20, "Temperature: " + String(rawDataArray[6]));
    deviceScreen.gText(5, 35, "Humidity: " + String(rawDataArray[5]));
    deviceScreen.gText(5, 50, "CO (PPM): " + String(rawDataArray[8]));
    deviceScreen.gText(5, 65, "CH4 (PPM): " + String(rawDataArray[9]));
    deviceScreen.gText(5, 80, "AQI: " + String(rawDataArray[10]));
    deviceScreen.gText(5, 120, "MAC: " + screendriverGetMacAddress());
    xSemaphoreGive(rawDataMutex);
  }
}

void screendriverFlushWithChrono(){
  uint32_t chrono = millis();
  deviceScreen.flush();
  chrono = millis() - chrono;
  // Serial.printf("\tScreen refresh took %i\n", chrono);
}


void globalRefresh() {
  if(xSemaphoreTake(rawDataMutex, portMAX_DELAY)){
    deviceScreen.regenerate();
  }
}

void updateFrames(){
  if(xSemaphoreTake(rawDataMutex, portMAX_DELAY)){
    deviceScreen.drawSensorFrame(epd_settings.frame0sensor, 0);
    deviceScreen.drawSensorFrame(epd_settings.frame1sensor, 1);
    for(int i = 0; i < 2; i ++) {

      uint8_t currentSensor;

      if(i == 0) currentSensor = epd_settings.frame0sensor;
      else currentSensor = epd_settings.frame1sensor;

      if(currentSensor == mySensor.temperature || currentSensor == mySensor.humidity) {
        deviceScreen.updateFrameVal(i, mySensor.humidity, String(rawDataArray[5]));
        deviceScreen.updateFrameVal(i, mySensor.temperature, String(rawDataArray[6]));
      }
      else if(currentSensor == mySensor.co2) deviceScreen.updateFrameVal(i, mySensor.humidity, String(rawDataArray[0]));
      else if(currentSensor == mySensor.co) deviceScreen.updateFrameVal(i, mySensor.co, String(rawDataArray[8]));
      else if(currentSensor == mySensor.ch4) deviceScreen.updateFrameVal(i, mySensor.ch4, String(rawDataArray[9]));
      else if(currentSensor == mySensor.co2) deviceScreen.updateFrameVal(i, mySensor.co2, String(rawDataArray[0]));
      else if(currentSensor == mySensor.voc) deviceScreen.updateFrameVal(i, mySensor.voc, String(rawDataArray[7]));
      else if(currentSensor == mySensor.aqi) deviceScreen.updateFrameVal(i, mySensor.aqi, String(rawDataArray[10]));
      else if(currentSensor == mySensor.aqi) deviceScreen.updateFrameVal(i, mySensor.particles, String(rawDataArray[2]));
    }
    xSemaphoreGive(rawDataMutex);
  }
  deviceScreen.flush();
}

void drawScreen() {
  if(xEventGroupGetBits(appStateFlagGroup) & APP_FLAG_SETUP) {
    // draw pairing screen
  } else if(xEventGroupGetBits(appStateFlagGroup) & APP_FLAG_RUNNING) {
    // draw sensor screen
    updateFrames();
    
  } else if(xEventGroupGetBits(appStateFlagGroup) & APP_FLAG_OTA_DOWNLOAD) {
    // draw update screen
    updateScreen();
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
  while (1) {
     {
      // Prologue
      deviceScreen.clear();
      // Function Body
      screendriverShowTime();
      screendriverShowDetailedMeasurements();
      // Epilogue
      screendriverFlushWithChrono();
      vTaskDelay(1000);
    }
  }
}
