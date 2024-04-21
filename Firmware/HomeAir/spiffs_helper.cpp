#include "spiffs_helper.h"
#include "FS.h"

/* You only need to format SPIFFS the first time you run a
   test or else use the SPIFFS plugin to create a partition
   https://github.com/me-no-dev/arduino-esp32fs-plugin */
#define FORMAT_SPIFFS_IF_FAILED true
float otaDownloadPercentage = 0;
float downloadRate = 0;
float uploadPercentage = 0;
float uploadRate = 0;

void listDir(fs::FS &fs, const char *dirname, uint8_t levels) {
  Serial.printf("Listing directory: %s\r\n", dirname);

  File root = fs.open(dirname);
  if (!root) {
    Serial.println("- failed to open directory");
    return;
  }
  if (!root.isDirectory()) {
    Serial.println(" - not a directory");
    return;
  }

  File file = root.openNextFile();
  while (file) {
    if (file.isDirectory()) {
      Serial.print("  DIR : ");
      Serial.println(file.name());
      if (levels) {
        listDir(fs, file.path(), levels - 1);
      }
    } else {
      Serial.print("  FILE: ");
      Serial.print(file.name());
      Serial.print("\tSIZE: ");
      Serial.println(file.size());
    }
    file = root.openNextFile();
  }
}

void appendLineToFile(fs::FS &fs, const char *path, const char *message) {
  // Serial.printf("Appending to file: %s\r\n", path);

  File file = fs.open(path, FILE_APPEND);
  if ((SPIFFS.totalBytes() - SPIFFS.usedBytes()) / SPIFFS.totalBytes() <=
      0.25) {
    if (!file) {
      Serial.println("- failed to open file for appending");
      return;
    }
    int i = 0;
    while (true) {
      if (message[i] != '\n') {
        if (!file.print(message[i++]))
          Serial.println("- append failed");
      } else {
        // Current char is \n
        file.print(message[i]);
        Serial.println("Message Appended");
        break;
      }
    }
    file.close();
  } else {
    Serial.println("SPIFFS is at or above 75% util. Did not append. Please "
                   "export and clear data ASAP.");
  }
}

void deleteFile(fs::FS &fs, const char *path) {
  Serial.printf("Deleting file: %s\r\n", path);
  if (fs.remove(path)) {
    Serial.println("- file deleted");
  } else {
    Serial.println("- delete failed");
  }
}

void deleteAllFiles(fs::FS &fs) {
  // Used to delete ALL files
  File root = fs.open("/");
  File file = root.openNextFile();
  while (file) {
    if (fs.remove(file.path())) {
      Serial.println("Removed file");
    } else {
      Serial.println("Failed to remove file");
    }
    file = root.openNextFile();
  }
  Serial.println("Deleted all files in directory.");
}

float reducePrecision(float var) {
  // 37.66666 * 100 =3766.66
  // 3766.66 + .5 =3767.16    for rounding off value
  // then type cast to int so value is 3767
  // then divided by 100 so the value converted into 37.67
  float value = (int)(var * 100 + .5);
  return (float)value / 100;
}

void onProgressCallback(size_t progress, size_t total) {
  float percentage = (progress / total) * 100;
  // Serial.println("Callback is hit!");
  if (percentage > 5 && (int)percentage % 5 <= 1) {
    Serial.printf("Progress: %f\n", percentage);
  }
}

void spiffsStorageTask(void *pvParameter) {
  char path[13] = {"/datalog.txt"};
  // std::string message;
  char message[512];
  // File file, root;
  int lineLength = 0;
  int numPackets = 6;
  int insertPoint = 0;
  int downloadIttr = 0;
  int chrono = millis();
  size_t writtenSize, prevSize = 0;
  uint32_t uploadedBytes = 0, totalBytesToUpload = 0;
  Update.onProgress(onProgressCallback);
  while (!SPIFFS.begin(true)) {
    Serial.println("SPIFFS Mount failed... retrying");
    vTaskDelay(5000 / portTICK_RATE_MS);
  }
  vTaskDelay(2000);
  Serial.printf("Total memory: %zu\n", SPIFFS.totalBytes());
  Serial.printf("Currently used memory: %zu\n", SPIFFS.usedBytes());
  Serial.printf("Remaining memory: %zu\n",
                SPIFFS.totalBytes() - SPIFFS.usedBytes());
  File root = SPIFFS.open("/");
  File file = root.openNextFile();
  if (!root) {
    Serial.println("Unable to mount spiffs");
  }
  while (file) {
    String fileName = file.name();
    size_t fileSize = file.size();
    Serial.printf("FS File: %s, size: %zu\n", fileName.c_str(), fileSize);
    file = root.openNextFile();
  }
  file.close();
  online.SPIFFS = true;
  while (1) {
    // printCurrentAppFlagStatus();
    if (xEventGroupGetBits(appStateFlagGroup) & APP_FLAG_SETUP) {
      while (!dateConfigured) {
        Serial.println("Waiting for time config...");
        vTaskDelay(5000 / portTICK_RATE_MS);
      }
      Serial.printf("Set path to: %s\n", path);
      // SPIFFS.remove(path);  // Don't cull on boot
      xEventGroupClearBits(appStateFlagGroup, APP_FLAG_SETUP);
      xEventGroupSetBits(appStateFlagGroup, APP_FLAG_RUNNING);
    } else if (xEventGroupGetBits(appStateFlagGroup) & APP_FLAG_RUNNING) {
      // Write data to SPIFFS
      if (xSemaphoreTake(rawDataMutex, portMAX_DELAY)) {
        snprintf(
            message, 80,
            "%d,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f\n",
            rtc.getEpoch(), rawDataArray[0], rawDataArray[1], rawDataArray[2],
            rawDataArray[3], rawDataArray[4], rawDataArray[5], rawDataArray[6],
            rawDataArray[7], rawDataArray[8], rawDataArray[9], rawDataArray[10],
            rawDataArray[11]);

        xSemaphoreGive(rawDataMutex); // Release mutex
        Serial.print("Saving ");
        Serial.print(message);
        appendLineToFile(SPIFFS, path, message);
        vTaskDelay(5000 / portTICK_RATE_MS);
      }
    } else if (xEventGroupGetBits(appStateFlagGroup) & APP_FLAG_TRANSMITTING) {
      file = SPIFFS.open(path);
      uploadedBytes = 0;
      while (file.available()) {
        xEventGroupSetBits(BLEStateFlagGroup, BLE_FLAG_FILE_EXISTS);
        for (int i = 0; i < numPackets; i++) {
          // Read file into BLEMessageBuffer with no \n
          lineLength = file.readBytesUntil('\n', &BLEMessageBuffer[insertPoint],
                                           BLE_BUFFER_LENGTH);
          if (lineLength < 2) {
            Serial.println("Empty message received and broke spiffs helper.");
            break;
          }
          insertPoint += lineLength; // Move insert point into BLEMessageBuffer
          BLEMessageBuffer[insertPoint++] = '\n'; // Add \n to end of line
        }
        totalBytesToUpload = file.size();
        uploadedBytes += insertPoint; //
        uploadPercentage =
            (uploadedBytes * 100 / (totalBytesToUpload)); // 0-100
        BLEMessageBuffer[insertPoint] = 0;                // Bam
        insertPoint = 0;
        // Notify BLE comm task of complete buffer
        Serial.printf("Upload progress: %f\n", uploadPercentage);
        xEventGroupSetBits(BLEStateFlagGroup, BLE_FLAG_BUFFER_READY);
        xEventGroupWaitBits(appStateFlagGroup, APP_FLAG_PUSH_BUFFER,
                            APP_FLAG_PUSH_BUFFER, false, ONE_MIN_MS);
      }
      file.close();
      Serial.println("Finished reading file!");
      // deleteFile(SPIFFS, path);
      // Wait for buffer to be read before notifying EOF
      xEventGroupWaitBits(BLEStateFlagGroup, BLE_FLAG_READ_COMPLETE,
                          BLE_FLAG_READ_COMPLETE, false, ONE_MIN_MS);
      xEventGroupClearBits(appStateFlagGroup,
                           APP_FLAG_TRANSMITTING); // Clear current app state
      xEventGroupSetBits(appStateFlagGroup,
                         APP_FLAG_DONE_TRANSMITTING); // set DONE_TRANSMITTING
                                                      // to send end of message
      xEventGroupSetBits(
          appStateFlagGroup,
          APP_FLAG_RUNNING); // set RUNNING to return to normal operation
      xEventGroupSetBits(
          BLEStateFlagGroup,
          BLE_FLAG_FILE_DONE); // set FILE_DONE to break BLE waiting loop
    } else if (xEventGroupGetBits(appStateFlagGroup) & APP_FLAG_OTA_DOWNLOAD) {
      // Init OTA download, and if successful begin writing to the partition
      if (updateSize != 0) {
        SPIFFS.remove("/dest_bin"); // Make room for new OTA
        Serial.printf("SPIFFS Storage Status: %d / %d\n", SPIFFS.usedBytes(),
                      SPIFFS.totalBytes());
        if (SPIFFS.totalBytes() - SPIFFS.usedBytes() >= updateSize) {
          // We have enough room to directly receive BLE data
          Serial.println("Space for OTA exists in SPIFFS...");
          chrono = millis();
          file = SPIFFS.open("/dest_bin", "w");
          if (!file) {
            Serial.println("Error opening dest file.");
          } else {
            // Main OTA Download loop
            while (xEventGroupGetBits(appStateFlagGroup) &
                   APP_FLAG_OTA_DOWNLOAD) {
              xEventGroupWaitBits(BLEStateFlagGroup, BLE_FLAG_WRITE_COMPLETE,
                                  BLE_FLAG_WRITE_COMPLETE, false, ONE_MIN_MS);
              writtenSize += file.write((uint8_t *)&BLEMessageBuffer[0], 509);
              if (xSemaphoreTake(otaDownloadPercentageMutex, 0)) {
                otaDownloadPercentage =
                    ((float)writtenSize / (float)updateSize) * 100;
                xSemaphoreGive(otaDownloadPercentageMutex);
              }
              if (++downloadIttr % 5 == 0) {
                downloadRate =
                    (float)(writtenSize - prevSize) / (millis() - chrono);
                prevSize = writtenSize;
                chrono = millis();
                Serial.printf("%.2fKB/sec\n", downloadRate);
                Serial.printf("Download percentage: %.2f\n",
                              otaDownloadPercentage);
              }
              // Add setBits for done writing to ack in BLE
              xEventGroupSetBits(BLEStateFlagGroup, BLE_FLAG_SAVE_COMPLETE);
              if (xEventGroupGetBits(BLEStateFlagGroup) &
                  BLE_FLAG_DOWNLOAD_COMPLETE) {
                break;
              }
            }
            // OTA Update has been received to SPIFFS
            Serial.printf("Percentage written %f\n", file.size() / updateSize);
            Serial.printf("File written: %zu\n", file.size());
            Serial.printf("WrittenSize: %zu\n", writtenSize);

            file.close();
            Serial.println("Verifying new BIN...");
            if (Update.begin(updateSize)) {
              Serial.println("Writing file to OTA partition...");
              writtenSize = Update.writeStream(file);
              file.close();
              if (writtenSize >= updateSize) {
                Serial.println("Entire update file was written.");
              } else {
                Serial.println("Failure writing update file.");
              }
              if (Update.end(true)) {
                Serial.println("OTA done!");
                if (Update.isFinished()) {
                  Serial.println("OTA Verified. Rebooting...");
                  preferences.putBool("startingFromOTA", true);
                  delay(3000);
                  ESP.restart();
                } else {
                  Serial.println("Update not finished. Rebooting...");
                  delay(3000);
                  ESP.restart();
                }
              } else {
                Serial.println("End failed. I don't know why. Rebooting...");
                delay(5000);
                ESP.restart();
              }
            }
          }
        } else {
          // Not enough room in SPIFFS for update even after clearing previous
          // OTA
          Serial.println("No room for OTA in SPIFFS!");
          delay(500);
          Serial.println("Restarting...");
          delay(5000);
          ESP.restart();
        }
      }
    }
  }
}
