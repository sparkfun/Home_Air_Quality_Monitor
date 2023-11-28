/*
  Read and write settings and calibration data to an external I2C EEPROM
  By: Zane McMorris
  For use by: SparkFun Electronics
  Date: November 14th, 2023
  License: This code is public domain but you buy me a beer if you use this
  and we meet someday (Beerware license).

  This is the firmware for the Sparkfun HomeAir air-quality monitoring system.

  The program uses BLE to interface with the accompanying mobile app and uses an RTOS
  to manage communication, sensor reading, storage, and displaying live info on the screen
*/



#include "HomeAir.h"

// Global Variables
unsigned long epochTime;
// float16 rawDataArray[RAW_DATA_ARRAY_SIZE];
char BLEMessageBuffer[CONFIG_NIMBLE_CPP_ATT_VALUE_INIT_LENGTH];
// Task handle definitions
TaskHandle_t sensor_read_task_handle;
TaskHandle_t spiffs_storage_task_handle, ble_comm_task_handle, time_sync_task_handle;
// Flag Group definitions
EventGroupHandle_t appStateFG;
EventGroupHandle_t BLEStateFG;
// Mutex creation
SemaphoreHandle_t rawDataMutex;
// Preferences object creation
Preferences preferences;



void setup() {
  Serial.begin(115200);
  Serial.write("Setting up...");
  // setupPreferences();
  // setupTime();

  // Setup Flag Event Groups
  appStateFG = xEventGroupCreate();
  BLEStateFG = xEventGroupCreate();
  xEventGroupSetBits(appStateFG, APP_FLAG_SETUP);

  // Setup Mutexes
  rawDataMutex = xSemaphoreCreateMutex();

  // xTaskCreatePinnedToCore(gpio_sensor_read_task,         /*Function to call*/
  //                         "Sensor Read Task",       /*Task name*/
  //                         10000,                    /*Stack size*/
  //                         NULL,                     /*Function parameters*/
  //                         5,                        /*Priority*/
  //                         &sensor_read_task_handle, /*ptr to global TaskHandle_t*/
  //                         ARDUINO_AUX_CORE);        /*Core ID*/
  xTaskCreatePinnedToCore(spiffs_storage_task,         /*Function to call*/
                          "SPIFFS Storage Task",       /*Task name*/
                          10000,                       /*Stack size*/
                          NULL,                        /*Function parameters*/
                          2,                           /*Priority*/
                          &spiffs_storage_task_handle, /*ptr to global TaskHandle_t*/
                          ARDUINO_AUX_CORE);           /*Core ID*/

  xTaskCreatePinnedToCore(BLEServer_comm_task,            /*Function to call*/
                          "BLE Communication Task", /*Task name*/
                          10000,                    /*Stack size*/
                          NULL,                     /*Function parameters*/
                          1,                        /*Priority*/
                          &ble_comm_task_handle,    /*ptr to global TaskHandle_t*/
                          ARDUINO_PRIMARY_CORE);    /*Core ID*/

  xTaskCreatePinnedToCore(time_sync_task,         /*Function to call*/
                          "Time Sync Task",       /*Task name*/
                          10000,                  /*Stack size*/
                          NULL,                   /*Function parameters*/
                          1,                      /*Priority*/
                          &time_sync_task_handle, /*ptr to global TaskHandle_t*/
                          ARDUINO_AUX_CORE);      /*Core ID*/
}
// All loop functionality is completed with tasks defined in setup()
void loop() {}


void setupPreferences() {
  // Preferences is good for single KVP storage.
  // We want to use SPIFFS for large storage
  // bool status = preferences.begin("my_app", false);
}

void time_sync_task(void *pvParameter) {
  /* 

  */
  while (1) {
    while (xEventGroupGetBits(BLEStateFG) & BLE_FLAG_CLIENT_CONNECTED) {
      // Ask for current time update
      vTaskDelay(ONE_HOUR_MS / portTICK_RATE_MS);
    }

    vTaskDelay(ONE_DAY_MS / portTICK_RATE_MS);
  }
}






