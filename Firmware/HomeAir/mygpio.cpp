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
      xSemaphoreGive(rawDataMutex); // Release mutex
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
    Serial.print("SEN54: Error trying to execute deviceReset(): ");
    errorToString(error, errorMessage, 256);
    Serial.println(errorMessage);
  } else {
    error = sen5x.setTemperatureOffsetSimple(0); // No temp offset
    error = sen5x.startMeasurement();
    Serial.println("SEN54: Set up sensor successfully!");
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
  errorPtr = co2Sensor.startMeasure(5); // Start continous measurement with 5 second period
  if (XENSIV_PASCO2_OK != errorPtr) {
    Serial.print("PAS CO2: startmeasure error: ");
    Serial.println(errorPtr);
  }

  if (errorPtr == XENSIV_PASCO2_OK) {
    Serial.println("PAS CO2: Set up sensor successfully!");
    online.pasco2 = true;
  }
}

float readNGSensor() {
  float reading = (3.3 * analogRead(pin_NGInput)) / 4095;
  return 10.938 * exp(1.7742 * (reading * 3.3 / 4095));
}

float readCOSensor() {
  float sensorVoltage = (3.3 * analogRead(pin_COInput)) / 4095;
  return 3.027 * exp(1.0698 * sensorVoltage);
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
                                     retArray[VOC], unusedValue);

    if (error) {
      Serial.print("Error trying to execute readMeasuredValues(): ");
      errorToString(error, errorMessage, 256);
      Serial.println(errorMessage);
    }
  }
  else{
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
  10: AQI - Composite
  */
  // CO2
  ret_array[CO2_PPM] = readCO2PPM(co2Error, co2Sensor);
  // SEN
  // Uses ret_array[1] through ret_array[7]
  readSENSensor(ret_array, 7);
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
}

void setupGPIO() {
  // Setup I2C
  Serial.println("Setting up GPIO...");
  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN, I2C_FREQ_HZ); // Used for Sparkfun Thing
  // Wire.setClock(I2C_FREQ_HZ);  // 400KHz

  setupCO2Sensor(co2Error, co2Sensor); // Setup PASCO2 Sensor
  setupSENSensor();
  Serial.println("Successfully set up GPIO.");
}
