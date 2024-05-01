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

#define BREAD_BOARD 1
// #define FRANKENSTEIN_BOARD 1
// #define FINAL_HARDWARE 1

#define ADJUST_REFRESH_RATE                                                    \
  1 // if enabled then refresh rate will be set to 1 second when pairing or
    // updating

// #define FRAME_TESTING_MODE 1

const pins_t myMfnBoard{
#ifdef FINAL_HARDWARE
    .panelBusy = 43,  //"BUSY"
    .panelDC = 41,    //"D/C"
    .panelReset = 42, //"Reset"
    .panelMISO = 44,  //"MISO", Not Used
    .panelMOSI = 38,  //"MOSI"
    .panelSCK = 39,   //"SCK"
    .flashCS = 45,    //"FCSM", Not Used
    .panelCS = 40,    //"ECSM"
#elif defined(FRANKENSTEIN_BOARD)
    .panelBusy = 38,  //"BUSY"
    .panelDC = 40,    //"D/C"
    .panelReset = 39, //"Reset"
    .panelMISO = 48,  //"MISO", Not Used
    .panelMOSI = 43,  //"MOSI"
    .panelSCK = 42,   //"SCK"
    .flashCS = 37,    //"FCSM", Not Used
    .panelCS = 41,    //"ECSM"
#elif defined(BREAD_BOARD)
    .panelBusy = 41,  //"BUSY", Brown breadboard jumper
    .panelDC = 40,    //"D/C", Red breadboard jumper
    .panelReset = 39, //"RST", Orange breadboard jumper
    .panelMISO = 38,  // NOT NEEDED //"MISO", Yellow breadboard jumper
    .panelMOSI = 37,  //"MOSI", Green breadboard jumper
    .panelSCK = 42,   //"SCK", Black breadboard jumper
    .flashCS = 36,    //"FCSM", Blue breadboard jumper
    .panelCS = 35,    //"ECSM", Purple breadboard jumper
#endif
};

void screendriverScreendriverEpaperSetup();
void screendriverRunScreenTask(void *pvParameter);

#endif