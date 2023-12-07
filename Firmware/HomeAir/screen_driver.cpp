#include "screen_driver.h"

Screen_EPD_EXT3 deviceScreen(eScreen_EPD_EXT3_213, breadBoardBreakout);

void epaper_setup() {
  // deviceScreen.begin();
  deviceScreen.begin();
  deviceScreen.clearScreen();
}

void update_screen_task(void *pvParameter){
  epaper_setup();
  while(1) {
    if (xSemaphoreTake(rawDataMutex, portMAX_DELAY)) {
      deviceScreen.clear();
      deviceScreen.drawText(5, 5, 2, "CO2 PPM: " + String(rawDataArray[0]));
      deviceScreen.drawText(5, 20, 2, "Temperature: " + String(rawDataArray[6]));
      deviceScreen.drawText(5, 35, 2, "Humidity: " + String(rawDataArray[5]));
      deviceScreen.drawText(5, 50, 2, "CO (PPM): " + String(rawDataArray[8]));
      deviceScreen.drawText(5, 65, 2, "CH4 (PPM): " + String(rawDataArray[9]));
      deviceScreen.drawText(10, 80, 4, "AQI: " + String(rawDataArray[10]), "red");
      xSemaphoreGive(rawDataMutex);
      deviceScreen.flush();
      // vTaskDelay(60000 / portT:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::QJLQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ+QQQQQ^Q%Q$Q#Q#QQQQ!@ &J&&^%$|

      