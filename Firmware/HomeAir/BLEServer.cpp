#include "BLEServer.h"

NimBLECharacteristic *pSensorCharacteristic;
size_t updateSize = 0;
size_t MTUSize = 512;
bool updateSizeRecieved = false; // Switch used when downloading OTA update

void BLEServerSetAdvertisingNameMAC();
void BLEServerSetCustomAdvertisingName(String newName);

void BLEServerSetAdvertisingNameMAC();
void BLEServerSetCustomAdvertisingName(String newName);

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
  }

  void onWrite(NimBLECharacteristic *pCharacteristic) {
    std::string value = pSensorCharacteristic->getValue();

    if (value.length() > 0) {

      Serial.println("*********");
      Serial.print("New value: ");
      for (int i = 0; i < value.length(); i++)
        Serial.print(value[i]);
      Serial.println();
      Serial.println("*********");
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
        float GMTOffset = stof(value.substr(5));
        long epoch_prev = rtc.getEpoch();
        timeZoneConfigured = true;
        // Change RTC offset
        rtc.setTime(epoch_prev + (3600 * GMTOffset));
        xEventGroupSetBits(BLEStateFlagGroup, BLE_FLAG_CLIENT_CONNECTED); // We only receive this command on connection

      } else if (BLEMessageType == "READ!" || value == "READ") {
        xEventGroupClearBits(appStateFlagGroup, APP_FLAG_RUNNING);
        xEventGroupSetBits(appStateFlagGroup, APP_FLAG_TRANSMITTING);
      } else if (BLEMessageType == "DEL!!") {
        // Manually cull the entire root directory
        Serial.println("Received command to clear SPIFFS. Clearing...");
        deleteAllFiles(SPIFFS);
      } else if (BLEMessageType == "TEST!") {
        Serial.println("TEST! received...");
        delay(3000);
        pSensorCharacteristic->setValue("a");
        delay(3000);
        pSensorCharacteristic->notify();
      } else if (BLEMessageType == "UPDAT") {
        if (xSemaphoreTake(rawDataMutex, portMAX_DELAY)) {
          // Acquire mutex
          Serial.println("UPDAT recieved!");
          if (xEventGroupGetBits(appStateFlagGroup) & APP_FLAG_OTA_DOWNLOAD) {
            return;
          }
          // Calling mygpioReadAllSensors() too often stalls bluetooth due to
          // its high priority and locks system
          // mygpioReadAllSensors(&rawDataArray[0], RAW_DATA_ARRAY_SIZE);
          char message[90];
          int charsWritten = snprintf(
              message, 90,
              "%d,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f,%."
              "1f\n\n",
              rtc.getEpoch(), rawDataArray[0], rawDataArray[1], rawDataArray[2],
              rawDataArray[3], rawDataArray[4], rawDataArray[5],
              rawDataArray[6], rawDataArray[7], rawDataArray[8],
              rawDataArray[9], rawDataArray[10], rawDataArray[11]);
          xSemaphoreGive(rawDataMutex); // Release mutex
          pSensorCharacteristic->setValue(message);
          pSensorCharacteristic->notify();
          Serial.printf("Set BLE value to: ");
          Serial.println(message);
        }
      } else if (BLEMessageType == "KAZAM") {
        Serial.println("KAZAM! - Starting to listen");
        xEventGroupClearBits(
            appStateFlagGroup,
            xEventGroupGetBits(appStateFlagGroup)); // Clear current state
        xEventGroupSetBits(appStateFlagGroup, APP_FLAG_OTA_DOWNLOAD);
        vTaskSuspend(
            mygpioSensorReadTaskHandle); // Suspend GPIO task while we update
        // Set state to download new firmware
        // Send an ACK to start download
        pSensorCharacteristic->setValue("a");
        Serial.printf("\tAck sent!\n");
        delay(100);
        pSensorCharacteristic->notify();
        Serial.printf("\tKazam: Ack sent\n");

      } else if (BLEMessageType == "END!!") {
        Serial.println("Download finished!");
        xEventGroupSetBits(appStateFlagGroup, APP_FLAG_RUNNING);
      } else if (BLEMessageType == "SIZE=") {
        if (xEventGroupGetBits(appStateFlagGroup) & APP_FLAG_OTA_DOWNLOAD) {
          // We're actively downloading, so new packet must be new info to
          // process
          if (!updateSizeRecieved) {
            updateSize = stoi(value.substr(5));
            updateSizeRecieved = true;
            Serial.printf("Size of incoming OTA update: %d\n",
                          stoi(value.substr(5)));
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
        // Respond with a status message
        String reply = "";

        if (online.pasco2) {
          reply += "1,";
        } else {
          reply += "0,";
        }

        if (online.sen5x) {
          reply += "1,";
        } else {
          reply += "0,";
        }

        char percent[5];
        snprintf(percent, 4, "%.01f",
                 100.0 * SPIFFS.usedBytes() / SPIFFS.totalBytes());
        reply += percent;
        Serial.printf("Replying with status: %s\n", reply.c_str());

        pSensorCharacteristic->setValue(reply.c_str());

      } else if (BLEMessageType == "NAME!") {
        Serial.printf("Received command to set name to %s\n",
                      value.substr(6).c_str());
        BLEServerSetCustomAdvertisingName(value.substr(6).c_str());

      } else if (BLEMessageType == "DISC!") {
        xEventGroupClearBits(appStateFlagGroup, APP_FLAG_ALL);
        xEventGroupClearBits(BLEStateFlagGroup, BLE_FLAG_ALL);
        xEventGroupSetBits(appStateFlagGroup, APP_FLAG_RUNNING);
        Serial.println("Client disconnected!\nResuming normal operation");
      } else if (value.substr(0, 3) == "EPD") {
        // std::string messageValue = value.substr(6);
        int messageValue = stoi(value.substr(6));
        Serial.printf("Received EPD config type: %s\n", BLEMessageType);
        Serial.printf("Received EPD config value: %d\n", messageValue);
        if (BLEMessageType == "EPDDE") {
          // EPD Dot Enable
          preferences.putBool("dotEnabled", messageValue);
        } else if (BLEMessageType == "EPDDL") {
          // EPD Dot location
          preferences.putUShort("dotLocation", messageValue);
        } else if (BLEMessageType == "EPDCE") {
          // EPD Clock Enable
          preferences.putUShort("clockEnable", messageValue);
        } else if (BLEMessageType == "EPDCL") {
          // EPD Clock Location
          preferences.putUShort("clockLocation", messageValue);
        } else if (BLEMessageType == "EPDLF") {
          // EPD Left Frame Data
          preferences.putUShort("frame1Sensor", messageValue);
        } else if (BLEMessageType == "EPDRF") {
          // EPD Right Frame Data
          preferences.putUShort("frame2Sensor", messageValue);
        } else if (BLEMessageType == "EPDRP") {
          // EPD Refresh Period
          preferences.putUShort("refreshPeriod", messageValue);
        } else if (BLEMessageType == "EPDRO") {
          if (messageValue == 1) {
            // Set orientation one way
          } else {
            // Set orientation the other way
          }
        }
        // ReadPeriod
        // averagingMode
        // MQ disable (4 or 7)
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
          xEventGroupWaitBits(BLEStateFlagGroup, BLE_FLAG_SAVE_COMPLETE,
                              BLE_FLAG_SAVE_COMPLETE, false, ONE_MIN_MS);
          // Serial.printf("\tPost Ack sent!\n");
          pSensorCharacteristic->setValue("a");
          pSensorCharacteristic->notify();
        } else {
          Serial.println("Packet with no header received!");
        }
      }
    }
  }
};

void BLEServerCommunicationTask(void *pvParameter) {
  BLEServerSetupBLE();
  EventBits_t BLEStatus;
  while (1) {
    // printCurrentBLEFlagStatus();
    while (xEventGroupGetBits(appStateFlagGroup) & APP_FLAG_TRANSMITTING) {
      Serial.print("Current number of clients: ");
      Serial.println(pSensorCharacteristic->getSubscribedCount());
      BLEStatus = xEventGroupWaitBits(
          BLEStateFlagGroup, BLE_FLAG_FILE_EXISTS | BLE_FLAG_FILE_DONE,
          BLE_FLAG_FILE_EXISTS | BLE_FLAG_FILE_DONE, false, 600000);
      if (BLEStatus & BLE_FLAG_FILE_DONE) {
        break;
      }
      // Else: File exists and buffer will be written to and read out
      xEventGroupWaitBits(BLEStateFlagGroup, BLE_FLAG_BUFFER_READY,
                          BLE_FLAG_BUFFER_READY, false, 60000);
      pSensorCharacteristic->setValue(BLEMessageBuffer);
      pSensorCharacteristic->notify();
      Serial.println(BLEMessageBuffer);
      // Wait for data to be read before preparing new buffer
      xEventGroupWaitBits(BLEStateFlagGroup, BLE_FLAG_READ_COMPLETE,
                          BLE_FLAG_READ_COMPLETE, false, ONE_MIN_MS);
      // Signal to SPIFFS to write new data
      xEventGroupSetBits(appStateFlagGroup, APP_FLAG_PUSH_BUFFER);
    }
    if (xEventGroupGetBits(appStateFlagGroup) & APP_FLAG_DONE_TRANSMITTING) {
      xEventGroupClearBits(appStateFlagGroup, APP_FLAG_DONE_TRANSMITTING);
      // uint8_t message[] = {65};
      pSensorCharacteristic->setValue("DONE");
      // pSensorCharacteristic->notify(&message[0], 1, true);
      Serial.println("Ending transmission!");
      deleteAllFiles(SPIFFS);
    }
    EventBits_t uxBits =
          xEventGroupWaitBits(appStateFlagGroup, APP_FLAG_TRANSMITTING,
                              false, pdFALSE, 1000 / portTICK_PERIOD_MS);
    // Check uxBits here if you're interested 
  }
}

void BLEServerSetAdvertisingNameMAC() {
  // Uses the last 2 bytes of the MAC address to set a unique advertising name
  uint8_t macOut[8];
  char retArr[14];
  esp_err_t espErr = esp_efuse_mac_get_default(&macOut[0]);
  if (espErr == ESP_OK) {
    snprintf(retArr, sizeof(retArr), "HomeAir-%02hhX%02hhX", macOut[4],
             macOut[5]);
    Serial.printf("Setting MAC to %s\n", retArr);
    NimBLEDevice::init(retArr);
  } else
    Serial.println("Mac address failed to be read");
}

void BLEServerSetCustomAdvertisingName(String newName) {
  // Uses the last 2 bytes of the MAC address to set a unique advertising name
  if (online.pref) {
    preferences.putString("customBLEName", newName.c_str());
    delay(100); // Give prefs a moment to set the new name
    if (preferences.getString("customBLEName") == newName.c_str()) {
      Serial.println("New name was saved successfully");
      delay(2000);
      // Perhaps show the new name and show that we're restarting?
      ESP.restart();
    } else {
      Serial.println("New name was not saved successfully");
      preferences.putString("customBLEName", "NONE");
    }
  }
}

void BLEServerSetupBLE() {
  String customName = preferences.getString("customBLEName");
  Serial.printf("Saved custom name is %s\n", customName.c_str());
  delay(500);
  if (preferences.getString("customBLEName") != "NONE") {
    Serial.println("Setting custom name for BLE Broadcasting ID");
    NimBLEDevice::init(preferences.getString("customBLEName").c_str());
  } else {
    Serial.println("Setting BLE Broadcasting ID to MAC address");
    BLEServerSetAdvertisingNameMAC();
  }

  NimBLEDevice::setMTU(MTUSize);
  NimBLEServer *pServer = NimBLEDevice::createServer();
  NimBLEService *pService = pServer->createService(SERVICE_UUID);
  pSensorCharacteristic = pService->createCharacteristic(
      CHARACTERISTIC_UUID,
      NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::NOTIFY);

  pSensorCharacteristic->setCallbacks(new MyCallbacks());
  pService->start();
  NimBLEAdvertising *pAdvertising = NimBLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  // pAdvertising->setScanResponse(true);
  // pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone
  // connections issue pAdvertising->setMinPreferred(0x12);
  pAdvertising->setScanResponse(true);
  pAdvertising->start();
  online.BLE = true;
}