#include "spiffs_helper.h"

/* You only need to format SPIFFS the first time you run a
   test or else use the SPIFFS plugin to create a partition
   https://github.com/me-no-dev/arduino-esp32fs-plugin */
#define FORMAT_SPIFFS_IF_FAILED true

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

void readFile(fs::FS &fs, const char *path) {
  Serial.printf("Reading file: %s\r\n", path);

  File file = fs.open(path);
  if (!file || file.isDirectory()) {
    Serial.println("- failed to open file for reading");
    return;
  }

  Serial.println("- read from file:");
  while (file.available()) {
    Serial.write(file.read());
  }
  file.close();
}

void writeFile(fs::FS &fs, const char *path, const char *message) {
  Serial.printf("Writing file: %s\r\n", path);

  File file = fs.open(path, FILE_WRITE);
  if (!file) {
    Serial.println("- failed to open file for writing");
    return;
  }
  if (file.print(message)) {
    Serial.println("- file written");
  } else {
    Serial.println("- write failed");
  }
  file.close();
}

void appendFile(fs::FS &fs, const char *path, const char *message) {
  Serial.printf("Appending to file: %s\r\n", path);

  File file = fs.open(path, FILE_APPEND);
  if (!file) {
    Serial.println("- failed to open file for appending");
    return;
  }
  if (file.print(message)) {
    Serial.println("- message appended");
  } else {
    Serial.println("- append failed");
  }
  file.close();
}

void renameFile(fs::FS &fs, const char *path1, const char *path2) {
  Serial.printf("Renaming file %s to %s\r\n", path1, path2);
  if (fs.rename(path1, path2)) {
    Serial.println("- file renamed");
  } else {
    Serial.println("- rename failed");
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

void testFileIO(fs::FS &fs, const char *path) {
  Serial.printf("Testing file I/O with %s\r\n", path);

  static uint8_t buf[512];
  size_t len = 0;
  File file = fs.open(path, FILE_WRITE);
  if (!file) {
    Serial.println("- failed to open file for writing");
    return;
  }

  size_t i;
  Serial.print("- writing");
  uint32_t start = millis();
  for (i = 0; i < 2048; i++) {
    if ((i & 0x001F) == 0x001F) {
      Serial.print(".");
    }
    file.write(buf, 512);
  }
  Serial.println("");
  uint32_t end = millis() - start;
  Serial.printf(" - %u bytes written in %lu ms\r\n", 2048 * 512, end);
  file.close();

  file = fs.open(path);
  start = millis();
  end = start;
  i = 0;
  if (file && !file.isDirectory()) {
    len = file.size();
    size_t flen = len;
    start = millis();
    Serial.print("- reading");
    while (len) {
      size_t toRead = len;
      if (toRead > 512) {
        toRead = 512;
      }
      file.read(buf, toRead);
      if ((i++ & 0x001F) == 0x001F) {
        Serial.print(".");
      }
      len -= toRead;
    }
    Serial.println("");
    end = millis() - start;
    Serial.printf("- %u bytes read in %lu ms\r\n", flen, end);
    file.close();
  } else {
    Serial.println("- failed to open file for reading");
  }
}

void spiffs_storage_task(void *pvParameter) {
  char path[64];
  char message[96];
  long epochLapTime = 0;
  while (!SPIFFS.begin(true)) {
    Serial.println("SPIFFS Mount failed... retrying");
    vTaskDelay(5000 / portTICK_RATE_MS);
  }
  while (1) {
    if (xEventGroupGetBits(appStateFG) & APP_FLAG_SETUP) {
      while (!dateConfigured) {
        Serial.println("Waiting for time config...");
        vTaskDelay(5000 / portTICK_RATE_MS);
      }
      sprintf(path, "/%04d-%02d-%02d %02d:%02d:%02d.csv", rtc.getYear(), rtc.getMonth() + 1, rtc.getDay(),
              rtc.getHour(), rtc.getMinute(), rtc.getSecond());
      Serial.printf("Set path to: %s", path);
      xEventGroupClearBits(appStateFG, APP_FLAG_SETUP);
      xEventGroupSetBits(appStateFG, APP_FLAG_RUNNING);

    } else if (xEventGroupGetBits(appStateFG) & APP_FLAG_RUNNING) {
      // Update path daily for separation of logs
      if (rtc.getEpoch() - epochLapTime >= ONE_DAY_SEC) {
        epochLapTime = rtc.getEpoch();
        sprintf(path, "/%04d-%02d-%02d %02d:%02d:%02d.csv", rtc.getYear(), rtc.getMonth() + 1, rtc.getDay(),
                rtc.getHour(), rtc.getMinute(), rtc.getSecond());
      }

      // Write data to SPIFFS
      if (xSemaphoreTake(rawDataMutex, portMAX_DELAY)) {
        // Acquire mutex
        sprintf(message, "%u,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f \n", rtc.getEpoch(), rawDataArray[0],
                rawDataArray[1], rawDataArray[2], rawDataArray[3],
                rawDataArray[4], rawDataArray[5], rawDataArray[6],
                rawDataArray[7], rawDataArray[8], rawDataArray[9],
                rawDataArray[10]);
        Serial.print("Wrote data to file: ");
        Serial.println(message);
        appendFile(SPIFFS, path, message);
        xSemaphoreGive(rawDataMutex);  // Release mutex
        vTaskDelay(15000 / portTICK_RATE_MS);
      }
    } else if (xEventGroupGetBits(appStateFG) & APP_FLAG_TRANSMITTING) {
      while (xEventGroupGetBits(appStateFG) & APP_FLAG_TRANSMITTING) {
        File file = SPIFFS.open(path);
        while (file.available()) {
          // file.readBytesUntil('\n', &BLEMessageBuffer[0], CONFIG_NIMBLE_CPP_ATT_VALUE_INIT_LENGTH);
          file.readBytes(&BLEMessageBuffer[0], 55);
          Serial.print("Read from file: ");
          Serial.println(BLEMessageBuffer);
          xEventGroupSetBits(BLEStateFG, BLE_FLAG_BUFFER_READY);
          xEventGroupWaitBits(appStateFG, APP_FLAG_PUSH_BUFFER, APP_FLAG_PUSH_BUFFER, false, 15000);
        }
        file.openNextFile();
      }
    }
  }
}
