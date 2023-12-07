#ifndef SCREEN_DRIVER_CPP
#define SCREEN_DRIVER_CPP

#include "mygpio.h"
#include "epaper_driver.h"

void epaper_setup();

void update_screen_task(void *pvParameter);

#endif