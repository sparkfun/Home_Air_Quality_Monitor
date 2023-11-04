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
char AQISource[2];
ESP32Time rtc(0);  // create an instance with a specifed offset in seconds
// Task handle definitions
TaskHandle_t sensor_read_task_handle;
TaskHandle_t spiffs_storage_task_handle, ble_comm_task_handle, time_sync_task_handle;
// Mutex creation
SemaphoreHandle_t rawDataMutex;
// Preferences object creation
Preferences preferences;

void setupPreferences() {
  // Preferences is good for single KVP storage.
  // We want to use SPIFFS for large storage
  // bool status = preferences.begin("my_app", false);
}

class MyCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    std::string value = pCharacteristic->getValue();

    if (value.length() > 0) {
      Serial.println("*********");
      Serial.print("New value: ");
      for (int i = 0; i < value.length(); i++)
        Serial.print(value[i]);

      Serial.println();
      Serial.println("*********");
    }
  }
};

void setupBLE() {
  BLEDevice::init("ESP32_Test");
  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(SERVICE_UUID);
  BLECharacteristic *pCharacteristic = pService->createCharacteristic(
    CHARACTERISTIC_UUID,
    BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);

  pCharacteristic->setCallbacks(new MyCallbacks());
  pCharacteristic->setValue("Hello World says Neil");
  pService->start();
  // BLEAdvertising *pAdvertising = pServer->getAdvertising();  // this still is working for backward compatibility
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  // pAdvertising->setScanResponse(true);
  // pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  // pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
  Serial.print("BLE from core ");
  Serial.println(xPortGetCoreID());
}



void loop() {}
void setup() {
  Serial.begin(115200);
  Serial.write("Setting up...");
  setup_GPIO();
  setupPreferences();
  setupBLE();
  // setupTime();
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
    Serial.print("BLE from core ");
    Serial.println(xPortGetCoreID());
    vTaskDelay(5000 / portTICK_RATE_MS);
  }
}

void sensor_read_task(void *pvParameter) {
  while (1) {
    Serial.print("Sensor Read from core ");
    Serial.println(xPortGetCoreID());
    xSemaphoreTake(rawDataMutex, portMAX_DELAY);  // Acquire mutex
    read_all_sensors(&rawDataArray[0], &AQISource[0], RAW_DATA_ARRAY_SIZE);
    xSemaphoreGive(rawDataMutex);  // Release mutex
    for (int i = 0; i < RAW_DATA_ARRAY_SIZE; i++)
      Serial.println(rawDataArray[i]);

    vTaskDelay(3000 / portTICK_RATE_MS);
  }
}

void spiffs_storage_task(void *pvParameter) {

  char *path = "/test";
  char message[40];
  long epochTimeLap = 0;
  if (!SPIFFS.begin(true)) {
    Serial.println("SPIFFS Mount Failed");
  }
  while (1) {
    xSemaphoreTake(rawDataMutex, portMAX_DELAY);  // Acquire mutex
    // epochTime = getTime();
    // Serial.print("Epoch Time: ");
    // Serial.println(epochTime);
    Serial.printf("Appending to file: %s\r\n", path);
    if (epochTimeLap >= ONE_DAY_MS) {
      // Set new path and reopen file
      path = "/newTest";
    }
    sprintf(message, "test: %u\n", rtc.getEpoch());
    appendFile(SPIFFS, path, message);
    readFile(SPIFFS, path);
    xSemaphoreGive(rawDataMutex);  // Release mutex
    vTaskDelay(5000 / portTICK_RATE_MS);
  }
}
