#ifndef MY_GPIO_H
#define MY_GPIO_H
// Non-standard data types
#include <float16.h>
// ESP32 Built-ins
#include "driver/adc.h"
#include "driver/gpio.h"
#include "driver/i2c.h"
#include "esp32-hal-adc.h"
#include <Arduino.h>


// Vendor Libraries
#include <SensirionI2CSen5x.h> // https://github.com/Sensirion/arduino-i2c-sen5x - Version 0.3.0
#include <pas-co2-ino.hpp> // https://github.com/Infineon/arduino-pas-co2-sensor - Version 3.1.2

// HomeAir Files
#include "HomeAir.h"

#define USE_NOX
#define SENSOR_READ_PERIOD_SEC 15
#define GPIO0_RESET_TIME_SEC 5
#define GPIO0_RESET_TIME_MS GPIO0_RESET_TIME_SEC * 1000

#define GPIO0_PIN 0
#define DBG_LED 34 // On-board LED for debugging
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
#ifdef USE_NOX
const uint8_t RAW_DATA_ARRAY_SIZE = 12;
enum sensorMap {
  CO2_PPM = 0,
  PPM_1_0 = 1,
  PPM_2_5 = 2,
  PPM_4_0 = 3,
  PPM_10 = 4,
  HUMIDITY = 5,
  TEMP = 6,
  VOC = 7,
  NOX = 8,
  CO = 9,
  NG = 10,
  AQI = 11
};
#else
const uint8_t RAW_DATA_ARRAY_SIZE = 11;
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
#endif

const uint32_t I2C_FREQ_HZ = 100000;
#if defined(HOMEAIR_BOARD) || defined(ANTON_BOARD)
const uint8_t I2C_SDA_PIN = 8;
const uint8_t I2C_SCL_PIN = 9;
#else
const uint8_t I2C_SDA_PIN = 17;
const uint8_t I2C_SCL_PIN = 18;
#endif
const uint8_t PERIODIC_MEAS_INTERVAL_IN_SECONDS = 10;
const uint16_t PRESSURE_REFERENCE = 1000;
// Standard GPIO Pin definitions
const uint8_t pin_NGInput = 11;
const uint8_t pin_COInput = 12;
// const gpio_num_t esp_NGPin = GPIO_NUM_7;
// const gpio_num_t esp_COPin = GPIO_NUM_6;
// const adc1_channel_t esp_NGChannel = ADC1_CHANNEL_6;
// const adc1_channel_t esp_COChannel = ADC1_CHANNEL_5;

void setupGPIO(void);
void mygpioReadAllSensors(float *ret_array, uint16_t array_size);
void mygpioSensorReadTask(void *pvParameter);
void setupCO2Sensor(Error_t errorPtr, PASCO2Ino CO2SensorPtr);
void setupSENSensor(void);
void GPIO0_timercb();

extern float rawDataArray[RAW_DATA_ARRAY_SIZE];

#endif
