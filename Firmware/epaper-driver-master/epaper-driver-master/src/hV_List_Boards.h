
#include "stdint.h"

#ifndef hV_LIST_BOARDS_RELEASE
///
/// @brief Release
///
#define hV_LIST_BOARDS_RELEASE 700

///
/// @brief Not connected pin
///
#define NOT_CONNECTED (uint8_t)0xff

struct pins_t
{
    // ///< EXT3 and EXT3-1 pin 1 Black -> +3.3V
    // ///< EXT3 and EXT3-1 pin 2 Brown -> SPI SCK
    uint8_t panelBusy; ///< EXT3 and EXT3-1 pin 3 Red
    uint8_t panelDC; ///< EXT3 and EXT3-1 pin 4 Orange
    uint8_t panelReset; ///< EXT3 and EXT3-1 pin 5 Yellow
    // ///< EXT3 and EXT3-1 pin 6 Green -> SPI MISO
    // ///< EXT3 and EXT3-1 pin 7 Blue -> SPI MOSI
    uint8_t panelMISO;
    uint8_t panelMOSI;
    uint8_t panelSCK;
    uint8_t flashCS; ///< EXT3 and EXT3-1 pin 8 Violet
    uint8_t panelCS; ///< EXT3 and EXT3-1 pin 9 Grey
    // ///< EXT3 and EXT3-1 pin 10 White -> GROUND
    uint8_t panelCSS; ///< EXT3 and EXT3-1 pin 12 Grey2
    uint8_t flashCSS; ///< EXT3 pin 20 or EXT3-1 pin 11 Black2
    // ///< EXT3-Touch pin 1 Brown -> I2C SDA
    // ///< EXT3-Touch pin 2 Black -> I2C SCL
    uint8_t touchInt; ///< EXT3-Touch pin 3 Red
    uint8_t touchReset; ///< EXT3-Touch pin 4 Orange
    uint8_t panelPower; ///< Optional power circuit
    uint8_t cardCS; ///< Separate SD-card board
    uint8_t cardDetect; ///< Separate SD-card board
};


const pins_t boardESP32DevKitC =
{
    .panelBusy = 16, //"BUSY", Red devkit jumper
    .panelDC = 18, //"D/C", Orange devkit jumper
    .panelReset = 17, //"RST", Yellow devkit jumper
    .panelMISO = 12, //"MISO", Green devkit jumper
    .panelMOSI = 13, //"MOSI", Blue devkit jumper
    .panelSCK = 14, //"SCK", Brown devkit jumper
    .flashCS = 8, //"FCSM", Purple devkit jumper
    .panelCS = 3, //"ECSM", Grey devkit jumper
};

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
    .panelMISO = 37, //"MISO", Green breadboard jumper
    .panelMOSI = 36, //"MOSI", Blue breadboard jumper
    .panelSCK = 35, //"SCK", Brown breadboard jumper
    .flashCS = 21, //"FCSM", Purple breadboard jumper
    .panelCS = 11, //"ECSM", Grey breadboard jumper
};

#endif // hV_LIST_BOARDS_RELEASE

