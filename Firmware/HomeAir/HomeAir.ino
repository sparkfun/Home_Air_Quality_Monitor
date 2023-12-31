/*
  Read and write settings and calibration data to an external I2C EEPROM
  By: Zane McMorris
  For use by: SparkFun Electronics
  Date: November 14th, 2023
  License: This code is public domain but you buy me a beer if you use this
  and we meet someday (Beerware license).

  This is the firmware for the SparkFun HomeAir air-quality monitoring system.

  The program uses BLE (Bluetooth Low-Energy) to interface with the accompanying
  mobile app and uses an RTOS to manage communication, sensor reading, storage,
  and displaying live info on the screen.
*/

#include "HomeAir.h"

// Global Variables
unsigned long epochTime;
// float16 rawDataArray[RAW_DATA_ARRAY_SIZE];
char BLEMessageBuffer[BLE_BUFFER_LENGTH];
// Task handle definitions
TaskHandle_t mygpioSensorReadTaskHandle;
TaskHandle_t spiffsStorageTaskHandle, BLEServerCommunicationTaskHandle,
    timekeepingSyncTaskHandle, screendriverRunScreenTaskHandle;
// Flag Group definitions
EventGroupHandle_t appStateFlagGroup;
EventGroupHandle_t BLEStateFlagGroup;
// Mutex creation
SemaphoreHandle_t rawDataMutex;
// Preferences object creation
Preferences preferences;

void setup() {
  Serial.setTxTimeoutMs(0);
  Serial.begin(115200);
  Serial.write("Setting up...");
  // setupPreferences();
  // setupTime();

  // Setup Flag Event Groups
  appStateFlagGroup = xEventGroupCreate();
  BLEStateFlagGroup = xEventGroupCreate();
  xEventGroupSetBits(appStateFlagGroup, APP_FLAG_SETUP);

  // Setup Mutexes
  rawDataMutex = xSemaphoreCreateMutex();

  xTaskCreatePinnedToCore(
      mygpioSensorReadTask,        /*Function to call*/
      "Sensor Read Task",          /*Task name*/
      10000,                       /*Stack size*/
      NULL,                        /*Function parameters*/
      5,                           /*Priority*/
      &mygpioSensorReadTaskHandle, /*ptr to global TaskHandle_t*/
      ARDUINO_AUX_CORE);           /*Core ID*/

  xTaskCreatePinnedToCore(
      spiffsStorageTask,        /*Function to call*/
      "SPIFFS Storage Task",    /*Task name*/
      10000,                    /*Stack size*/
      NULL,                     /*Function parameters*/
      2,                        /*Priority*/
      &spiffsStorageTaskHandle, /*ptr to global TaskHandle_t*/
      ARDUINO_AUX_CORE);        /*Core ID*/

  xTaskCreatePinnedToCore(
      BLEServerCommunicationTask,        /*Function to call*/
      "BLE Communication Task",          /*Task name*/
      10000,                             /*Stack size*/
      NULL,                              /*Function parameters*/
      1,                                 /*Priority*/
      &BLEServerCommunicationTaskHandle, /*ptr to global TaskHandle_t*/
      ARDUINO_PRIMARY_CORE);             /*Core ID*/

  xTaskCreatePinnedToCore(
      timekeepingSyncTask,        /*Function to call*/
      "Time Sync Task",           /*Task name*/
      10000,                      /*Stack size*/
      NULL,                       /*Function parameters*/
      1,                          /*Priority*/
      &timekeepingSyncTaskHandle, /*ptr to global TaskHandle_t*/
      ARDUINO_AUX_CORE);          /*Core ID*/

  xTaskCreatePinnedToCore(
      screendriverRunScreenTask,        /*Function to call*/
      "Epaper Update Task",             /*Task name*/
      10000,                            /*Stack size*/
      NULL,                             /*Function parameters*/
      10,                               /*Priority*/
      &screendriverRunScreenTaskHandle, /*ptr to global TaskHandle_t*/
      ARDUINO_AUX_CORE);                /*Core ID*/
}
// All loop functionality is completed with tasks defined in setup()
void loop() {}

void setupPreferences() {
  // Preferences is good for single KVP storage.
  // We want to use SPIFFS for large storage
  // bool status = preferences.begin("my_app", false);
  // Currently not used, may be used for user customization options
}
