#include "screen_driver.h"

Screen_EPD_EXT3 deviceScreen(eScreen_EPD_EXT3_213, breadBoardBreakout);

void screendriverEpaperSetup() {
  // deviceScreen.begin();
  deviceScreen.begin();
  deviceScreen.clearScreen();
}

void screendriverGetMacAddress(){
  // Test function for reading out the MAC address of the ESP32 device
  uint8_t macOut[8];
  esp_err_t espErr = esp_efuse_mac_get_default(&macOut[0]);
  if (espErr == ESP_OK){
    Serial.printf("MAC is %02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx\n",
        macOut[0], macOut[1], macOut[2], macOut[3], macOut[4], macOut[5]);
  }
  else
    Serial.println("Mac address failed to be read");
}

void screendriverRunScreenTask(void *pvParameter){
  Serial.println("Running in screen task.");
  screendriverGetMacAddress();
  screendriverEpaperSetup();
  // while(1) {
  //   if (xSemaphoreTake(rawDataMutex, portMAX_DELAY)) {
  //     deviceScreen.clear();
  //     deviceScreen.drawText(5, 5, 2, "CO2 PPM: " + String(rawDataArray[0]));
  //     deviceScreen.drawText(5, 20, 2, "Temperature: " + String(rawDataArray[6]));
  //     deviceScreen.drawText(5, 35, 2, "Humidity: " + String(rawDataArray[5]));
  //     deviceScreen.drawText(5, 50, 2, "CO (PPM): " + String(rawDataArray[8]));
  //     deviceScreen.drawText(5, 65, 2, "CH4 (PPM): " + String(rawDataArray[9]));
  //     deviceScreen.drawText(10, 80, 4, "AQI: " + String(rawDataArray[10]), "red");
  //     xSemaphoreGive(rawDataMutex);
  //     deviceScreen.flush();
  //     // vTaskDelay(60000 / portT:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::QJLQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ+QQQQQ^Q%Q$Q#Q#QQQQ!@ &J&&^%$|
  //   }
  // }
}

      