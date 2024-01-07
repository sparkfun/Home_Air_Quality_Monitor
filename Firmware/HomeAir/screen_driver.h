#ifndef SCREEN_DRIVER_CPP
#define SCREEN_DRIVER_CPP


// Standard Library Includes

// ESP32 System Includes
#include "esp_mac.h"
// HomeAir Includes
#include "mygpio.h"
#include "epaper_driver.h"

void screendriverEpaperSetup();

void screendriverRunScreenTask(void *pvParameter);

#endif