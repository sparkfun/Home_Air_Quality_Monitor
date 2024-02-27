#include "hV_HAL_Peripherals_fast.h"

// Configuration
#include "hV_Configuration_fast.h"

#ifndef EPD_SENSOR
#define EPD_SENSOR

class epdSensor
{
  public:
    ///
    /// @brief Constructor
    ///
    epdSensor();
    
    static const uint8_t co2 = 0;
    static const uint8_t co = 1;
    static const uint8_t nox = 2;
    static const uint8_t particles = 3;
    static const uint8_t voc = 4;
    static const uint8_t ch4 = 5;
    static const uint8_t temperature = 6;
    static const uint8_t humidity = 7;
    static const uint8_t aqi = 8;

};

extern epdSensor mySensor;

#endif