#ifndef SCREEN_DRIVER_CPP
#define SCREEN_DRIVER_CPP

#include "mygpio.h"
#include "epaper_driver.h"

void screendriveEpaperSetup();

void screendriverRunScreenTask(void *pvParameter);

#endif