#ifndef SCREEN_DRIVER_CPP
#define SCREEN_DRIVER_CPP


// Standard Library Includes

// ESP32 System Includes
#include "esp_mac.h"
#include "driver/spi_master.h"
// HomeAir Includes
#include "HomeAir.h"
// #include "epaper_driver.h"
#include "PDLS_EXT3_Basic_Fast.h"
#include "hV_HAL_Peripherals.h"
#include "hV_Configuration.h"
// Pin defns for breadboard
const pins_t breadBoardBreakout =
{
    // .panelBusy = 45, //"BUSY<Red
    // .panelDC = 48, //Orange
    // .panelReset = 47, //Yellow
    // .panelMISO = 36, //Blue
    // .panelMOSI = 37, //Green
    // .panelSCK = 35, //Brown
    // .flashCS = 21, //Purple
    // .panelCS = 11, ///Grey

    .panelBusy = 45, //"BUSY", Red breadboard jumper
    .panelDC = 48, //"D/C", Orange breadboard jumper
    .panelReset = 47, //"RST", Yellow breadboard jumper
    // .panelMISO = 37, //"MISO", Green breadboard jumper
    // .panelMOSI = 36, //"MOSI", Blue breadboard jumper
    // .panelSCK = 35, //"SCK", Brown breadboard jumper
    .flashCS = 21, //"FCSM", Purple breadboard jumper
    .panelCS = 11, //"ECSM", Grey breadboard jumper
};

void screendriverEpaperSetup();

void screendriverRunScreenTask(void *pvParameter);

#endif