#include "mygpio.h"
#include "projdefs.h"

// Sensor object definitions
PASCO2Ino co2Sensor;
int16_t co2PPM;
Error_t co2Error;
SensirionI2CSen5x sen5x;
// Global Variables
float rawDataArray[RAW_DATA_ARRAY_SIZE];

void mygpioSensorReadTask(void *pvParameter) {
  setupGPIO();
  uint16_t sensorReadLoop = 0;
  while (1) {
    // Serial.print("Sensor Read from core ");
    // Serial.println(xPortGetCoreID());
    sensorReadLoop++;
    if (sensorReadLoop >= SENSOR_READ_PERIOD_SEC) {
      sensorReadLoop = 0; // Reset counter
      if (!online.pasco2) {
        setupCO2Sensor(co2Error, co2Sensor); // Attempt to setup PASCO2 Sensor
      }
      if (!online.sen5x) {
        setupSENSensor(); // Attempt to setup SEN55
      }
      if (xSemaphoreTake(rawDataMutex, portMAX_DELAY)) {
        // Acquire mutex
        mygpioReadAllSensors(&rawDataArray[0], RAW_DATA_ARRAY_SIZE);
        xSemaphoreGive(rawDataMutex); // Release mutex
        // for (int i = 0; i < RAW_DATA_ARRAY_SIZE; i++) {
        //   Serial.print(rawDataArray[i]);
        //   Serial.println(sensorMap[i]);
        // }
      }
    }

    vTaskDelay(1000 / portTICK_RATE_MS);
  }
}

void setupSENSensor() {
  Serial.println("SEN5x: Setting up...");
  sen5x.begin(Wire);
  uint16_t error;
  char errorMessage[256];
  error = sen5x.deviceReset();
  if (error) {
    Serial.print("SEN5x: Error trying to execute deviceReset(): ");
    errorToString(error, errorMessage, 256);
    Serial.println(errorMessage);
  } else {
    error = sen5x.setTemperatureOffsetSimple(0); // No temp offset
    error = sen5x.startMeasurement();
    Serial.println("SEN5x: Set up sensor successfully!");
    online.sen5x = true;
  }
}

void setupCO2Sensor(Error_t errorPtr, PASCO2Ino CO2SensorPtr) {
  errorPtr = co2Sensor.begin();
  Serial.println("PAS CO2: Setting up...");
  if (XENSIV_PASCO2_OK != errorPtr) {
    Serial.print("PAS CO2: initialization error: ");
    Serial.println(errorPtr);
  }
  /* We can set the reference pressure before starting
   * the measure
   */
  errorPtr = co2Sensor.setPressRef(PRESSURE_REFERENCE);
  if (XENSIV_PASCO2_OK != errorPtr) {
    Serial.print("PAS CO2: pressure reference error: ");
    Serial.println(errorPtr);
  }
  errorPtr = co2Sensor.startMeasure(
      5); // Start continous measurement with 5 second period
  if (XENSIV_PASCO2_OK != errorPtr) {
    Serial.print("PAS CO2: startmeasure error: ");
    Serial.println(errorPtr);
  }

  if (errorPtr == XENSIV_PASCO2_OK) {
    Serial.println("PAS CO2: Set up sensor successfully!");
    online.pasco2 = true;
  }
}

void setupMQSensors() {
  // Enable and configure onboard ADCs
  // Both sensors are connected to ADC 1 with 10 channels
  // analogReadResolution(12);
  // analogSetAttenuation((adc_attenuation_t) 3); // 11db
  if (adc_set_data_width(ADC_UNIT_BOTH, ADC_WIDTH_BIT_12) == ESP_OK) {
    Serial.println("ADC1 & ADC2 Configured.");
    online.ng = true;
    online.co = true;
  } else {
    Serial.println("ADCs NOT configured!");
  }
  // if (adc2_config_width(ADC_WIDTH_BIT_12) == ESP_OK){
  //   Serial.println("ADC2 Configured.");
  //   online.co = true;
  // } else {
  //   Serial.println("ADC2 NOT configured")
  // }
}

float readNGSensor() {
  // float reading = analogRead(pin_NGInput);
  if (online.ng) {
    int NGReading = analogRead(pin_NGInput);
    // Serial.printf("\tNG: %d\n", NGReading);
    return NGReading;
  }
  return -1;
}

float readCOSensor() {
  if (online.co) {
    int COReading = analogRead(pin_COInput);
    // Serial.printf("\tCO: %d\n", COReading);
    if (COReading > 0 && COReading < 500) {
      return 0; // Background levels
    } else if (COReading > 500 && COReading < 650) {
      return 1; // Low reading
    } else if (COReading > 650 && COReading < 800) {
      return 2; // Medium
    } else if (COReading > 800 && COReading < 950) {
      return 3; // High
    } else {
      return COReading;
    }
  }
  return -1;
}

void readSENSensor(float *retArray, uint8_t arraySize) {
  char errorMessage[256];
  uint16_t error;
  float unusedValue = 0;
  // float tempArray[arraySize];
  if (online.sen5x) {
#ifdef USE_NOX
    error = sen5x.readMeasuredValues(retArray[PPM_1_0], retArray[PPM_2_5],
                                     retArray[PPM_4_0], retArray[PPM_10],
                                     retArray[HUMIDITY], retArray[TEMP],
                                     retArray[VOC], retArray[NOX]);
#else
    error = sen5x.readMeasuredValues(retArray[PPM_1_0], retArray[PPM_2_5],
                                     retArray[PPM_4_0], retArray[PPM_10],
                                     retArray[HUMIDITY], retArray[TEMP],
                                     retArray[VOC], unusedValue);
#endif

    if (error) {
      Serial.print("Error trying to execute readMeasuredValues(): ");
      errorToString(error, errorMessage, 256);
      Serial.println(errorMessage);
    }
  } else {
    Serial.println("SEN5x offline");
  }
}

uint16_t readCO2PPM(Error_t errorPtr, PASCO2Ino CO2SensorPtr) {
  co2PPM = 0;
  if (online.pasco2) {
    do {
      errorPtr = co2Sensor.getCO2(co2PPM);
      if (errorPtr != 0) {
        if (errorPtr != 7 || errorPtr != 1) {
          Serial.print("Error reading CO2 w/ error code:");
          Serial.println(errorPtr);
        }
      }
      delay(500);
    } while (co2PPM == 0);
    return co2PPM;
  }
  // Peripheral is not online - return -1 to indicate failure
  return -1;
}

void mygpioReadAllSensors(float *ret_array, uint16_t array_size) {
  // Reads all sensors and outputs into array
  /*
  0: CO2 PPM - PASCO2
  1: PPM 1.0 - SEN
  2: PPM 2.5 - SEN
  3: PPM 4.0 - SEN
  4: PPM 10.0 - SEN
  5: Humidity - SEN
  6: Temperature - SEN
  7: VOCs - SEN
  8: NOX - SEN
  9: CO - MQ7
  10: NG - MQ4
  11: AQI - Composite
  */
  // CO2
  ret_array[CO2_PPM] = readCO2PPM(co2Error, co2Sensor);
// SEN
// Uses ret_array[1] through ret_array[8]
#ifdef USE_NOX
  readSENSensor(ret_array, 8);
#else
  readSENSensor(ret_array, 7);
#endif
  // CO
  ret_array[CO] = readCOSensor();
  // Serial.print("CO ppm: ");
  // Serial.println(ret_array[9]);
  // NG
  ret_array[NG] = readNGSensor();
  // Serial.print("NG ppm: ");
  // Serial.println(ret_array[10]);
  ret_array[AQI] =
      aqiGetCompositeAQI(ret_array[PPM_2_5], ret_array[PPM_4_0], ret_array[CO]);

  Serial.print("Measurements: ");
  for (int i = 0; i < RAW_DATA_ARRAY_SIZE; i++) {
    Serial.printf("%f, ", ret_array[i]);
  }
  Serial.println();
}

void GPIO0_ISR() {
  DBG("GPIO ISR RAN");
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;

  if (digitalRead(GPIO0_PIN) == 0) {
    // Start timer
    if (xTimerStartFromISR(gpio0Timer, &xHigherPriorityTaskWoken) != pdPASS) {
      DBG("Failed to start timer from ISR.");
    } else {
      DBG("Started GPIO0 timer from ISR.");
    }

  } else if (digitalRead(GPIO0_PIN) == 1) {
    // Stop timer
    if (xTimerStopFromISR(gpio0Timer, &xHigherPriorityTaskWoken) != pdPASS) {
      DBG("Failed to stop GPIO0 timer from ISR.");
    } else {
      DBG("Stopped GPIO0 timer from ISR.");
    }
  }

  if (xHigherPriorityTaskWoken != pdFALSE) {
    portYIELD_FROM_ISR();
  }
}

void GPIO0_timercb() {
  Serial.println("Timer expired!");
  esp_err_t err;
  // xEventGroupSetBits(appStateFlagGroup, APP_FLAG_FACTORY_ROLLBACK);
  const esp_partition_t *running = esp_ota_get_running_partition();
  const esp_partition_t *nextPartition =
      esp_ota_get_next_update_partition(NULL);
  const esp_partition_t *factory = esp_partition_find_first(
      ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_APP_FACTORY, NULL);

  Serial.printf("Running partition: %s\n", running->label);
  Serial.printf("Running partition size: %d\n", running->size);
  Serial.printf("Next Update partition: %s\n", nextPartition->label);

  if (factory != NULL) {
    Serial.printf("Factory partition: %s\n", factory->label);
    // Serial.printf("Factory partition size: %d\n", factory->size);
    delay(150);
    err = esp_ota_set_boot_partition(factory);
    if (err != ESP_OK) {
      Serial.printf("Failed to set boot partition to factory: %d\n", err);
    } else {
      Serial.println("Factory rollback successful. Restarting...");
      delay(3000);
      esp_restart();
    }
    return;
  } else {
    Serial.println("Factory partition not found. Cannot rollback.");
  }
}

void setupGPIO() {
  // Setup I2C
  Serial.println("Setting up GPIO...");
  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN,
             I2C_FREQ_HZ); // Used for Sparkfun Thing
  // Wire.setClock(I2C_FREQ_HZ);  // 400KHz
  setupCO2Sensor(co2Error, co2Sensor); // Setup PASCO2 Sensor
  setupSENSensor();
  setupMQSensors();
  pinMode(GPIO0_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(GPIO0_PIN), GPIO0_ISR,
                  RISING | FALLING);

  Serial.println("Successfully set up GPIO.");
}
