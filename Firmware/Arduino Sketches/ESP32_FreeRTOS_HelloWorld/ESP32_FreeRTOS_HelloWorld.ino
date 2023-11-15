#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_PRIMARY_CORE 1
#define ARDUINO_AUX_CORE 0
#endif

#include "helloworld.h"

// Global Variables
unsigned long epochTime;
float16 rawDataArray[RAW_DATA_ARRAY_SIZE];
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

void setupPreferences() {
  // Preferences is good for single KVP storage.
  // We want to use SPIFFS for large storage
  // bool status = preferences.begin("my_app", false);
}



void loop() {}
void setup() {
  Serial.begin(115200);
  Serial.write("Setting up...");
  // setup_GPIO();
  // setupPreferences();
  // setupTime();
  // Setup Flag Event Groups
  appStateFG = xEventGroupCreate();
  BLEStateFG = xEventGroupCreate();

  // Setup Mutexes
  rawDataMutex = xSemaphoreCreateMutex();

  // xTaskCreatePinnedToCore(sensor_read_task,         /*Function to call*/
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

  xTaskCreatePinnedToCore(BLE_comm_task,            /*Function to call*/
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

void time_sync_task(void *pvParameter) {
  /* 

  */
  while (1) {


    vTaskDelay(ONE_DAY_MS / portTICK_RATE_MS);
  }
}



void BLE_comm_task(void *pvParameter) {
  setupBLE();
  while (1) {
    while (xEventGroupGetBits(appStateFG) & APP_FLAG_TRANSMITTING) {
      xEventGroupSetBits(appStateFG, APP_FLAG_PUSH_BUFFER);
      xEventGroupWaitBits(BLEStateFG, BLE_FLAG_BUFFER_READY, BLE_FLAG_BUFFER_READY, false, 5000);
      pSensorCharacteristic->setValue(BLEMessageBuffer);
      // Notify
      // TODO 
      uint8_t message[1] = {1};
      pSensorCharacteristic->notify(&message[0], 1, true);
      xEventGroupWaitBits(BLEStateFG, BLE_FLAG_READ_COMPLETE, BLE_FLAG_READ_COMPLETE, false, 5000);
    }
    vTaskDelay(1000 / portTICK_RATE_MS);
  }
}

void sensor_read_task(void *pvParameter) {
  while (1) {
    Serial.print("Sensor Read from core ");
    Serial.println(xPortGetCoreID());
    xSemaphoreTake(rawDataMutex, portMAX_DELAY);  // Acquire mutex
    read_all_sensors(&rawDataArray[0], RAW_DATA_ARRAY_SIZE);
    xSemaphoreGive(rawDataMutex);  // Release mutex
    // for (int i = 0; i < RAW_DATA_ARRAY_SIZE; i++) {
    //   Serial.print(rawDataArray[i]);
    //   Serial.println(sensorMap[i]);
    // }

    vTaskDelay(5000 / portTICK_RATE_MS);
  }
}
void spiffs_storage_task(void *pvParameter) {
  EventBits_t flagBits;
  char path[32] = { "temp:va:lf or:si:ze.csv" };
  char message[96];
  long epochLapTime = 0;
  if (!SPIFFS.begin(true)) {
    Serial.println("SPIFFS Mount Failed");
  }
  while (1) {
    if (xEventGroupGetBits(appStateFG) & APP_FLAG_SETUP) {
      while (!dateConfigured) {
        Serial.println("Waiting for time config...");
        vTaskDelay(5000 / portTICK_RATE_MS);
      }
      sprintf(path, "/%d:%d:%d:%d:%d:%d.csv", rtc.getYear(), rtc.getMonth() + 1, rtc.getDay(),
              rtc.getHour(), rtc.getMinute(), rtc.getSecond());
      Serial.printf("Set path to: %s", path);
      xEventGroupClearBits(appStateFG, APP_FLAG_SETUP);
      xEventGroupSetBits(appStateFG, APP_FLAG_RUNNING);

    } else if (xEventGroupGetBits(appStateFG) & APP_FLAG_RUNNING) {
      // Update path daily for separation of logs
      if (rtc.getEpoch() - epochLapTime >= ONE_DAY_SEC) {
        epochLapTime = rtc.getEpoch();
        sprintf(path, "%d:%d:%d:%d:%d:%d.csv", rtc.getYear(), rtc.getMonth() + 1, rtc.getDay(),
                rtc.getHour(), rtc.getMinute(), rtc.getSecond());
      }

      // Write data to SPIFFS
      xSemaphoreTake(rawDataMutex, portMAX_DELAY);  // Acquire mutex
      sprintf(message, "%u,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f \n", rtc.getEpoch(), rawDataArray[0].toDouble(),
              rawDataArray[1].toDouble(), rawDataArray[2].toDouble(), rawDataArray[3].toDouble(),
              rawDataArray[4].toDouble(), rawDataArray[5].toDouble(), rawDataArray[6].toDouble(),
              rawDataArray[7].toDouble(), rawDataArray[8].toDouble(), rawDataArray[9].toDouble(),
              rawDataArray[10].toDouble());
      appendFile(SPIFFS, path, message);
      xSemaphoreGive(rawDataMutex);  // Release mutex
      vTaskDelay(15000 / portTICK_RATE_MS);
    } else if (xEventGroupGetBits(appStateFG) & APP_FLAG_TRANSMITTING) {
      File file = SPIFFS.open(path);
      while (xEventGroupGetBits(appStateFG) & APP_FLAG_TRANSMITTING) {
        file.readBytes(&BLEMessageBuffer[0], CONFIG_NIMBLE_CPP_ATT_VALUE_INIT_LENGTH);
        xEventGroupSetBits(BLEStateFG, BLE_FLAG_BUFFER_READY);
        xEventGroupWaitBits(appStateFG, APP_FLAG_PUSH_BUFFER, APP_FLAG_PUSH_BUFFER, false, 1000);
        xEventGroupClearBits(appStateFG, APP_FLAG_PUSH_BUFFER);
      }
    }
  }
}