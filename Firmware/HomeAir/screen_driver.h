#ifndef SCREEN_DRIVER_CPP
#define SCREEN_DRIVER_CPP


// Standard Library Includes

// ESP32 System Includes
#include "esp_mac.h"
#include "driver/spi_master.h"
// HomeAir Includes
#include "HomeAir.h"
// #include "epaper_driver.h"
#include "src/EPDDriver/PDLS_EXT3_Basic_Fast.h"
#include "src/EPDDriver/hV_HAL_Peripherals_fast.h"
#include "src/EPDDriver/hV_Configuration_fast.h"
// Pin defns for breadboard


const pins_t myMfnBoard 
{
    // .panelBusy = 45, //"BUSY<Red
    // .panelDC = 48, //Orange
    // .panelReset = 47, //Yellow
    // .panelMISO = 36, //Blue
    // .panelMOSI = 37, //Green
    // .panelSCK = 35, //Brown
    // .flashCS = 21, //Purple
    // .panelCS = 11, ///Grey

    // .panelBusy = 40, //"BUSY", Red breadboard jumper
    // .panelDC = 42, //"D/C", Orange breadboard jumper
    // .panelReset = 39, //"RST", Yellow breadboard jumper
    // // .panelMISO = NOT_CONNECTED, //"MISO", Green breadboard jumper
    // // .panelMOSI = 37, //"MOSI", Blue breadboard jumper
    // // .panelSCK = 41, //"SCK", Brown breadboard jumper
    // .flashCS = 38, //"FCSM", Purple breadboard jumper
    // .panelCS = 11 //"ECSM", Grey breadboard jumper

    
    .panelBusy = 41,  //"BUSY", Red breadboard jumper
    .panelDC = 40,    //"D/C", Orange breadboard jumper
    .panelReset = 39, //"RST", Yellow breadboard jumper
    .panelMISO = 38,  //"MISO", Green breadboard jumper
    .panelMOSI = 37,  //"MOSI", Blue breadboard jumper
    .panelSCK = 42,   //"SCK", Brown breadboard jumper
    .flashCS = 36,    //"FCSM", Purple breadboard jumper
    .panelCS = 35,    //"ECSM", Grey breadboard jumper
};

void screendriverEpaperSetup();

void screendriverRunScreenTask(void *pvParameter);

#endif