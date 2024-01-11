#include "screen_driver.h"

Screen_EPD_EXT3_Fast deviceScreen(eScreen_EPD_EXT3_266_0C_Fast, breadBoardBreakout);

void screendriverEpaperSetup() {
  deviceScreen.begin();
  deviceScreen.clear();
  deviceScreen.regenerate();
  deviceScreen.setOrientation(3); // Left-hand rotated Landscape
  deviceScreen.flushMode(UPDATE_FAST); // Set Flush Mode
  deviceScreen.selectFont();
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
int i = 0;
uint32_t chrono = 0;
void screendriverRunScreenTask(void *pvParameter){
  Serial.println("Running in screen task.");
  screendriverGetMacAddress();
  screendriverEpaperSetup();
  while(1) {
    if (xSemaphoreTake(rawDataMutex, portMAX_DELAY)) {
      Serial.println("Redrawing screen...");
      deviceScreen.clear();
      deviceScreen.gText(5, 5, "CO2 PPM: " + String(rawDataArray[0]));
      // deviceScreen.gText(5, 5, "Counter: " + String(i++));
      deviceScreen.gText(5, 20, "Temperature: " + String(rawDataArray[6]));
      deviceScreen.gText(5, 35, "Humidity: " + String(rawDataArray[5]));
      deviceScreen.gText(5, 50, "CO (PPM): " + String(rawDataArray[8]));
      deviceScreen.gText(5, 65, "CH4 (PPM): " + String(rawDataArray[9]));
      deviceScreen.gText(10, 80, "AQI: " + String(rawDataArray[10]));
      xSemaphoreGive(rawDataMutex);
      chrono = millis();
      deviceScreen.flush();
      chrono = millis() - chrono;
      Serial.printf("\tScreen Refresh took %i ms\n", chrono);
      vTaskDelay(1000);
    }
  }
}

      