#include "BLEServer.h"

NimBLECharacteristic *pSensorCharacteristic;
size_t updateSize = 0;
size_t MTUSize = 512;
bool updateSizeRecieved = false;  // Switch used when downloading OTA update


class MyCallbacks : public NimBLECharacteristicCallbacks {

  void onConnect(NimBLECharacteristic *pCharacteristic) {
    Serial.println("Client connected...");
    xEventGroupSetBits(BLEStateFlagGroup, BLE_FLAG_CLIENT_CONNECTED);
  }

  void onSubscribe(NimBLECharacteristic *pCharacteristic) {
    Serial.println("\n\nClient subscribed!\n\n");
    xEventGroupSetBits(BLEStateFlagGroup, BLE_FLAG_CLIENT_SUBSCRIBED);
  }

  void onRead(NimBLECharacteristic *pCharacteristic) {
    Serial.println("Read request serviced");
    xEventGroupSetBits(BLEStateFlagGroup, BLE_FLAG_READ_COMPLETE);
    // std::string message = std::to_string(rtc.getSecond());
    // pCharacteristic->setValue(BLEMessageBuffer);
  }

  void onWrite(NimBLECharacteristic *pCharacteristic) {
    std::string value = pSensorCharacteristic->getValue();

    if (value.length() > 0) {

      // Serial.println("*********");
      // Serial.print("New value: ");
      // for (int i = 0; i < value.length(); i++)
      //   Serial.print(value[i]);
      // Serial.println();
      // Serial.println("*********");
      // Example : TIME=1699143542
      // Epoch time is guaranteed to be 10 digits
      std::string BLEMessageType = value.substr(0, 5);
      if (BLEMessageType == "TIME=") {
        rtc.setTime(stoi(value.substr(5, 10)));
        Serial.print("Current epoch time: ");
        Serial.print(rtc.getDateTime());
        timeConfigured = true;
      } else if (BLEMessageType == "TGMT=") {
        // Given +/- GMT offset
        // Ex: -06 = MST | +13 = Tonga | +9.5 = Adelaide
        float GMTOffset = stof(value.substr(5, value.length() - 5));
        long epoch_prev = rtc.getEpoch();
        timeZoneConfigured = true;
        // Change RTC offset
        rtc.setTime(epoch_prev + (3600 * GMTOffset));
      } else if (BLEMessageType == "READ!") {
        // String currDateAndTime = rtc.getDateTime();
        // std::string myString = "This is new!";
        // pCharacteristic->setValue("HI Aziz!");
        // pCharacteristic->notify();
        // Serial.println("Updated value...");
        xEventGroupSetBits(appStateFlagGroup, APP_FLAG_TRANSMITTING);
        xEventGroupClearBits(appStateFlagGroup, APP_FLAG_RUNNING);
      } else if (BLEMessageType == "DEL!!") {
        // Manually cull the entire root directory
        deleteAllFiles(SPIFFS);
      } else if(BLEMessageType == "TEST!"){
        Serial.println("TEST! received...");
        delay(3000);
        pSensorCharacteristic->setValue("a");
        delay(3000);
        pSensorCharacteristic->notify();
      } else if (BLEMessageType == "UPDAT") {
        if (xSemaphoreTake(rawDataMutex, portMAX_DELAY)) {
          // Acquire mutex
          Serial.println("UPDAT recieved!");
          mygpioReadAllSensors(&rawDataArray[0], RAW_DATA_ARRAY_SIZE);
          char message[90];
          snprintf(message, 90,
          "%d,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f\n\n",
          rtc.getEpoch(), rawDataArray[0], rawDataArray[1],
          rawDataArray[2], rawDataArray[3], rawDataArray[4], rawDataArray[5],
          rawDataArray[6], rawDataArray[7], rawDataArray[8], rawDataArray[9],
          rawDataArray[10]);
          pSensorCharacteristic->setValue(message);
          pSensorCharacteristic->notify();
          Serial.printf("Set BLE value to: ");
          Serial.println(BLEMessageBuffer);
          xSemaphoreGive(rawDataMutex);  // Release mutex
        }
      } else if (BLEMessageType == "KAZAM") {
        Serial.println("KAZAM! - Starting to listen");
        xEventGroupClearBits(appStateFlagGroup, xEventGroupGetBits(appStateFlagGroup));  // Clear current state
        xEventGroupSetBits(appStateFlagGroup, APP_FLAG_OTA_DOWNLOAD);      
        vTaskSuspend(mygpioSensorReadTaskHandle); // Suspend GPIO task while we update
        // Set state to download new firmware
        // Send an ACK to start download
        pSensorCharacteristic->setValue("a");
        Serial.printf("\tAck sent!\n");
        delay(1000);
        pSensorCharacteristic->notify();
        Serial.printf("\tKazam: Ack sent\n");

      } else if (BLEMessageType == "END!!") {
        Serial.println("Download finished!");
        xEventGroupSetBits(appStateFlagGroup, APP_FLAG_RUNNING);
      } else if (BLEMessageType == "SIZE=") {
        if (xEventGroupGetBits(appStateFlagGroup) & APP_FLAG_OTA_DOWNLOAD) {
          // We're actively downloading, so new packet must be new info to process
          if (!updateSizeRecieved) {
            updateSize = stoi(value.substr(5, value.length() - 5));
            updateSizeRecieved = true;
            Serial.printf("Size of incoming OTA update: %d\n", stoi(value.substr(5, value.length() - 5)));
          }
        }
      } else if (BLEMessageType == "DONE!") {
        if (xEventGroupGetBits(appStateFlagGroup) & APP_FLAG_OTA_DOWNLOAD) {
          xEventGroupClearBits(appStateFlagGroup, APP_FLAG_OTA_DOWNLOAD);
          xEventGroupSetBits(BLEStateFlagGroup, BLE_FLAG_DOWNLOAD_COMPLETE);
          xEventGroupSetBits(BLEStateFlagGroup, BLE_FLAG_WRITE_COMPLETE);
          vTaskResume(mygpioSensorReadTaskHandle);
          Serial.println("DONE! received");
        }

      } else if (BLEMessageType == "STAT!") {
        listDir(SPIFFS, "/", 0);
      } else if (BLEMessageType == "="){
        // Dot position & enable
        // time position & enable
        // leftFrame data
        // rightFrame data
        // sensorReadPeriod
        // EPDUpdatePeriod
        // averagingMode
        // MQ disable (4 or 7)
        // 
        
        
      } else {
        // Received message had no message type
        // Check to see if we're downloading, and if so, service this new packet
        if (xEventGroupGetBits(appStateFlagGroup) & APP_FLAG_OTA_DOWNLOAD) {
          for (int i = 0; i < value.length(); i++) {
            // BLEMessageBuffer[i] = *(value.data() + i);
            std::copy(&value[0], &value[value.length()], BLEMessageBuffer);
            // Serial.printf("L=%d\n", value.length());
            // Serial.println("Packet received");
          }
          BLEMessageBuffer[value.length()] = '\0'; // Set null terminator
          xEventGroupSetBits(BLEStateFlagGroup, BLE_FLAG_WRITE_COMPLETE);
          xEventGroupWaitBits(BLEStateFlagGroup, BLE_FLAG_SAVE_COMPLETE, BLE_FLAG_SAVE_COMPLETE, false, ONE_MIN_MS);
          // Serial.printf("\tPost Ack sent!\n");
          pSensorCharacteristic->setValue("a");
          pSensorCharacteristic->notify();
        }
      }
    }
  }
};

void BLEServerCommunicationTask(void *pvParameter) {
  BLEServerSetupBLE();
  EventBits_t BLEStatus;
  while (1) {
    while (xEventGroupGetBits(appStateFlagGroup) & APP_FLAG_TRANSMITTING) {
      // Serial.print("Current number of clients: ");
      // Serial.println(pSensorCharacteristic->getSubscribedCount());
      BLEStatus = xEventGroupWaitBits(
        BLEStateFlagGroup, BLE_FLAG_FILE_EXISTS | BLE_FLAG_FILE_DONE,
        BLE_FLAG_FILE_EXISTS | BLE_FLAG_FILE_DONE, false, 600000);
      if (BLEStatus & BLE_FLAG_FILE_DONE) {
        break;
      }
      // Else: File exists and buffer will be written to and read out
      // Serial.print("Waiting for buffer to be ready...");
      xEventGroupWaitBits(BLEStateFlagGroup, BLE_FLAG_BUFFER_READY,
                          BLE_FLAG_BUFFER_READY, false, 60000);
      // Serial.println("Buffer ready!");
      pSensorCharacteristic->setValue(BLEMessageBuffer);
      pSensorCharacteristic->notify();
      // Serial.print("Set value to: ");
      Serial.println(BLEMessageBuffer);
      // Notify
      // pSensorCharacteristic->notify((const uint8_t*) &BLEMessageBuffer,
      // BLE_BUFFER_LENGTH, true); pSensorCharacteristic->notify(false);
      // Serial.println("Notification sent!");
      // Serial.println("Waiting for data to be read...");
      // printCurrentBLEFlagStatus();
      xEventGroupWaitBits(BLEStateFlagGroup, BLE_FLAG_READ_COMPLETE,
                          BLE_FLAG_READ_COMPLETE, false, 600000);
      xEventGroupSetBits(appStateFlagGroup, APP_FLAG_PUSH_BUFFER);
      // Serial.println("Buffer read!");
    }
    if (xEventGroupGetBits(appStateFlagGroup) & APP_FLAG_DONE_TRANSMITTING) {
      xEventGroupClearBits(appStateFlagGroup, APP_FLAG_DONE_TRANSMITTING);
      uint8_t message[1] = { 65 };
      pSensorCharacteristic->notify(&message[0], 1, true);
      Serial.println("Ending transmission!");
    }
    vTaskDelay(1000 / portTICK_RATE_MS);
  }
}

void BLEServerSetAdvertisingName() {
  // Uses the last 2 bytes of the MAC address to set a unique advertising name
  uint8_t macOut[8];
  char retArr[14];
  esp_err_t espErr = esp_efuse_mac_get_default(&macOut[0]);
  if (espErr == ESP_OK) {
    snprintf(retArr, sizeof(retArr), "HomeAir-%02hhx%02hhx", macOut[4], macOut[5]);
    Serial.printf("Setting MAC to %s\n", retArr);
    NimBLEDevice::init(retArr);
  } else
    Serial.println("Mac address failed to be read");
}

void BLEServerSetupBLE() {
  BLEServerSetAdvertisingName();

  NimBLEDevice::setMTU(MTUSize);
  NimBLEServer *pServer = NimBLEDevice::createServer();
  NimBLEService *pService = pServer->createService(SERVICE_UUID);
  pSensorCharacteristic = pService->createCharacteristic(
    CHARACTERISTIC_UUID,
    NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::NOTIFY);

  pSensorCharacteristic->setCallbacks(new MyCallbacks());
  pService->start();
  // BLEAdvertising *pAdvertising = pServer->getAdvertising();  // this still is
  // working for backward compatibility
  NimBLEAdvertising *pAdvertising = NimBLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  // pAdvertising->setScanResponse(true);
  // pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone
  // connections issue pAdvertising->setMinPreferred(0x12);
  pAdvertising->setScanResponse(true);
  pAdvertising->start();
  online.BLE = true;
}