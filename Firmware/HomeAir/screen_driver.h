#ifndef SCREEN_DRIVER_CPP
#define SCREEN_DRIVER_CPP

// Standard Library Includes

// ESP32 System Includes
#include "driver/spi_master.h"
#include "esp_mac.h"

// HomeAir Includes
#include "HomeAir.h"
// #include "epaper_driver.h"
#include "src/EPDDriver/src/PDLS_EXT3_Basic_Fast.h"
#include "src/EPDDriver/src/hV_Configuration_fast.h"
#include "src/EPDDriver/src/hV_HAL_Peripherals_fast.h"

// Pin defns for breadboard

// #define BREAD_BOARD 1
// #define FRANKENSTEIN_BOARD 1
#define FINAL_HARDWARE 1

#define ADJUST_REFRESH_RATE                                                    \
  1 // if enabled then refresh rate will be set to 1 second when pairing or
    // updating

// #define FRAME_TESTING_MODE 1

const pins_t myMfnBoard{
#ifdef FINAL_HARDWARE
    .panelBusy = 43,  //"BUSY", Red breadboard jumper
    .panelDC = 41,    //"D/C", Orange breadboard jumper
    .panelReset = 42, //"RST", Yellow breadboard jumper
    .panelMISO = 44,  //"MISO", Green breadboard jumper // UNUSED
    .panelMOSI = 38,  //"MOSI", Blue breadboard jumper
    .panelSCK = 39,   //"SCK", Brown breadboard jumper
    .flashCS = 40,    //"FCSM", Purple breadboard jumper
    .panelCS = 40,    //"ECSM", Grey breadboard jumper // UNUSED
#elif defined(FRANKENSTEIN_BOARD)
    .panelBusy = 38,  //"BUSY", Red breadboard jumper
    .panelDC = 40,    //"D/C", Orange breadboard jumper
    .panelReset = 39, //"RST", Yellow breadboard jumper
    .panelMISO = 48,  //"MISO", Green breadboard jumper
    .panelMOSI = 43,  //"MOSI", Blue breadboard jumper
    .panelSCK = 42,   //"SCK", Brown breadboard jumper
    .flashCS = 37,    //"FCSM", Purple breadboard jumper
    .panelCS = 41,    //"ECSM", Grey breadboard jumper
#elif defined(BREAD_BOARD)
    .panelBusy = 41,  //"BUSY", Red breadboard jumper
    .panelDC = 40,    //"D/C", Orange breadboard jumper
    .panelReset = 39, //"RST", Yellow breadboard jumper
    .panelMISO = 38,  // NOT NEEDED //"MISO", Green breadboard jumper
    .panelMOSI = 37,  //"MOSI", Blue breadboard jumper
    .panelSCK = 42,   //"SCK", Brown breadboard jumper
    .flashCS = 36,    //"FCSM", Purple breadboard jumper
    .panelCS = 35,    //"ECSM"
#endif

};

void screendriverEpaperSetup();

void screendriverRunScreenTask(void *pvParameter);

#endif