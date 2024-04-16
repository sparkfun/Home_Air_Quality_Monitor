#ifndef SCREEN_DRIVER_CPP
#define SCREEN_DRIVER_CPP


// Standard Library Includes

// ESP32 System Includes
#include "esp_mac.h"
#include "driver/spi_master.h"
// HomeAir Includes
#include "HomeAir.h"
// #include "epaper_driver.h"
#include "src/EPDDriver/src/PDLS_EXT3_Basic_Fast.h"
#include "src/EPDDriver/src/hV_HAL_Peripherals_fast.h"
#include "src/EPDDriver/src/hV_Configuration_fast.h"
// Pin defns for breadboard

// Comment both for breadboard

// No vents
// #define HOMEAIR_BOARD 1

// New PCB
// #define ANTON_BOARD 1

#define SHOW_READINGS_WITHOUT_CONNECTION 1
#define ROTATE_FRAMES 1
#define ADJUST_REFRESH_RATE 1 //if enabled then refresh rate will be set to 1 second when pairing or updating

const pins_t myMfnBoard{
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
#ifdef ANTON_BOARD
  .panelBusy = 43,   //"BUSY", Red breadboard jumper
  .panelDC = 41,     //"D/C", Orange breadboard jumper
  .panelReset = 42,  //"RST", Yellow breadboard jumper
  .panelMISO = 44,   //"MISO", Green breadboard jumper // UNUSED
  .panelMOSI = 38,   //"MOSI", Blue breadboard jumper
  .panelSCK = 39,    //"SCK", Brown breadboard jumper
  .flashCS = 40,     //"FCSM", Purple breadboard jumper
  .panelCS = 40,     //"ECSM", Grey breadboard jumper // UNUSED
#elif defined(HOMEAIR_BOARD)
  .panelBusy = 38,   //"BUSY", Red breadboard jumper
  .panelDC = 40,     //"D/C", Orange breadboard jumper
  .panelReset = 39,  //"RST", Yellow breadboard jumper
  .panelMISO = 48,   //"MISO", Green breadboard jumper
  .panelMOSI = 43,   //"MOSI", Blue breadboard jumper
  .panelSCK = 42,    //"SCK", Brown breadboard jumper
  .flashCS = 37,     //"FCSM", Purple breadboard jumper
  .panelCS = 41,     //"ECSM", Grey breadboard jumper
#else
  .panelBusy = 41,   //"BUSY", Red breadboard jumper
  .panelDC = 40,     //"D/C", Orange breadboard jumper
  .panelReset = 39,  //"RST", Yellow breadboard jumper
  .panelMISO = 38,   //NOT NEEDED //"MISO", Green breadboard jumper
  .panelMOSI = 37,   //"MOSI", Blue breadboard jumper
  .panelSCK = 42,    //"SCK", Brown breadboard jumper
  .flashCS = 36,     //"FCSM", Purple breadboard jumper
  .panelCS = 35,     //"ECSM", Grey breadboard jumper
#endif

};

void screendriverEpaperSetup();

void screendriverRunScreenTask(void *pvParameter);

#endif