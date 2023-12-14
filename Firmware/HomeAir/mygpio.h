#ifndef MY_GPIO_H
#define MY_GPIO_H
// Non-standard data types
#include <float16.h>
// ESP32 Built-ins
#include "driver/gpio.h"
#include "driver/i2c.h"
#include <Arduino.h>

// Vendor Libraries
#include <pas-co2-ino.hpp> // https://github.com/Infineon/arduino-pas-co2-sensor
// #include <Adafruit_Sensor.h>
#include <SensirionI2CSen5x.h> // https://github.com/Sensirion/arduino-i2c-sen5x
// HomeAir Files
#include "AQI.h"
#include "settings.h"
/*
  0: CO2 PPM - PASCO2
  1: PPM 1.0 - SEN
  2: PPM 2.5 - SEN
  3: PPM 4.0 - SEN
  4: PPM 10.0 - SEN
  5: Humidity - SEN
  6: Temperature - SEN
  7: VOCs - SEN
  8: CO - MQ7
  9: NG - MQ4
  10: AQI - Composite
  */
enum sensorMap {
  CO2_PPM = 0,
  PPM_1_0 = 1,
  PPM_2_5 = 2,
  PPM_4_0 = 3,
  PPM_10 = 4,
  HUMIDITY = 5,
  TEMP = 6,
  VOC = 7,
  CO = 8,
  NG = 9,
  AQI = 10
};

const uint32_t I2C_FREQ_HZ = 100000;
const uint8_t I2C_SDA_PIN = 17;
const uint8_t I2C_SCL_PIN = 18;
const uint8_t PERIODIC_MEAS_INTERVAL_IN_SECONDS = 10;
const uint16_t PRESSURE_REFERENCE = 1000;
const uint8_t RAW_DATA_ARRAY_SIZE = 11;
// Standard GPIO Pin definitions
const uint8_t pin_NGInput = 38;
const uint8_t pin_COInput = 39;

void setupGPIO(void);
void read_all_sensors(float *ret_array, uint16_t array_size);
void gpio_sensor_read_task(void *pvParameter);

extern float rawDataArray[RAW_DATA_ARRAY_SIZE];
extern SemaphoreHandle_t rawDataMutex;

#endif
