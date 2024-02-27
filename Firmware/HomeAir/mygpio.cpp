#include "mygpio.h"

// Sensor object definitions
PASCO2Ino co2Sensor;
int16_t co2PPM;
Error_t co2Error;
SensirionI2CSen5x sen5x;
// Global Variables
float rawDataArray[RAW_DATA_ARRAY_SIZE];

void mygpioSensorReadTask(void *pvParameter) {
  setupGPIO();
  while (1) {
    // Serial.print("Sensor Read from core ");
    // Serial.println(xPortGetCoreID());
    if (xSemaphoreTake(rawDataMutex, portMAX_DELAY)) {
      // Acquire mutex
      mygpioReadAllSensors(&rawDataArray[0], RAW_DATA_ARRAY_SIZE);
      xSemaphoreGive(rawDataMutex);  // Release mutex
      // for (int i = 0; i < RAW_DATA_ARRAY_SIZE; i++) {
      //   Serial.print(rawDataArray[i]);
      //   Serial.println(sensorMap[i]);
      // }
    }

    vTaskDelay(5000 / portTICK_RATE_MS);
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
    error = sen5x.setTemperatureOffsetSimple(0);  // No temp offset
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
  errorPtr = co2Sensor.startMeasure(5);  // Start continous measurement with 5 second period
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
  if (adc_set_data_width(ADC_UNIT_BOTH, ADC_WIDTH_BIT_12) == ESP_OK){
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
    return COReading;
  }
  return -1;
}

void readSENSensor(float *retArray, uint8_t arraySize) {
  char errorMessage[256];
  uint16_t error;
  float unusedValue = 0;
  // float tempArray[arraySize];
  if (online.sen5x) {
    error = sen5x.readMeasuredValues(retArray[PPM_1_0], retArray[PPM_2_5],
                                     retArray[PPM_4_0], retArray[PPM_10],
                                     retArray[HUMIDITY], retArray[TEMP],
                                     retArray[VOC], retArray[NOX]);

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
        Serial.print("Error reading CO2 w/ error code:");
        Serial.println(errorPtr);
      }
      delay(250);
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
  8: CO - MQ7
  9: NG - MQ4
  10: NOX - SEN
  11: AQI - Composite
  */
  // CO2
  ret_array[CO2_PPM] = readCO2PPM(co2Error, co2Sensor);
  // SEN
  // Uses ret_array[1] through ret_array[8]
  readSENSensor(ret_array, 8);
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
  for(int i=0;i<RAW_DATA_ARRAY_SIZE;i++)
  {
    Serial.printf("%f, ", ret_array[i]);
  }
  Serial.println();
}

void setupGPIO() {
  // Setup I2C
  Serial.println("Setting up GPIO...");
  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN, I2C_FREQ_HZ);  // Used for Sparkfun Thing
  // Wire.setClock(I2C_FREQ_HZ);  // 400KHz

  setupCO2Sensor(co2Error, co2Sensor);  // Setup PASCO2 Sensor
  setupSENSensor();
  setupMQSensors();
  Serial.println("Successfully set up GPIO.");
}
