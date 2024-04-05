#include "projdefs.h"
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
  uint16_t sensorReadLoop = 0;
  while (1) {
    // Serial.print("Sensor Read from core ");
    // Serial.println(xPortGetCoreID());
    sensorReadLoop++;
    if(sensorReadLoop >= SENSOR_READ_PERIOD_SEC){
      sensorReadLoop = 0; // Reset counter
      if(!online.pasco2){
      setupCO2Sensor(co2Error, co2Sensor);  // Attempt to setup PASCO2 Sensor
      }
      if(!online.sen5x){
        setupSENSensor();                   // Attempt to setup SEN55
      }
      if (xSemaphoreTake(rawDataMutex, portMAX_DELAY)) {
        // Acquire mutex
        mygpioReadAllSensors(&rawDataArray[0], RAW_DATA_ARRAY_SIZE);
        xSemaphoreGive(rawDataMutex);  // Release mutex
        // for (int i = 0; i < RAW_DATA_ARRAY_SIZE; i++) {
        //   Serial.print(rawDataArray[i]);
        //   Serial.println(sensorMap[i]);
        // }
      }
    }

    if(!digitalRead(GPIO0_PIN) && xTimerIsTimerActive(gpio0Timer) == pdFALSE){
      // Button has been pressed the the timer hasn't been started yet
      if(xTimerStart(gpio0Timer, 0) == pdFALSE)
        Serial.println("GPIO0 Timer not started");
      else
        Serial.println("GPIO0 Timer started!");
    }

    if(digitalRead(GPIO0_PIN) && xTimerIsTimerActive(gpio0Timer) == pdTRUE){
      // Button was released while timer was active - kill timer
      if(xTimerStop(gpio0Timer, 0) == pdTRUE){
        Serial.println("Stopped timer");
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
    if(COReading > 0 && COReading < 500){
      return 0; // Background levels
    } else if (COReading > 500 && COReading < 650){
      return 1; // Low reading
    } else if (COReading > 650 && COReading < 800){
      return 2; // Medium
    } else if (COReading > 800 && COReading < 950){
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
        if(errorPtr != 7 || errorPtr != 1){
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

// void GPIO0_ISR(){
//   BaseType_t xHigherPriorityTaskWoken, xResult;
//   /* xHigherPriorityTaskWoken must be initialised to pdFALSE. */
//   xHigherPriorityTaskWoken = pdFALSE;

//   if(digitalRead(GPIO0_PIN) == 1){
//     xResult = xEventGroupSetBitsFromISR(appStateFlagGroup, APP_FLAG_GPIO0_PRESSED, &xHigherPriorityTaskWoken);
//     if(xResult != pdFAIL)
//       Serial.println("GPIO0 Pressed");
//     else
//       Serial.println("Failed to set bit on GPIO0 Press");
//   } else {
//     xResult = xEventGroupSetBitsFromISR(appStateFlagGroup, APP_FLAG_GPIO0_RELEASED, &xHigherPriorityTaskWoken);
//     if(xResult != pdFAIL)
//       Serial.println("GPIO0 Released");
//     else
//       Serial.println("Failed to set bit on GPIO0 Release");
//   }
// }

void GPIO0_timercb(){
  Serial.println("Timer expired!");
  xEventGroupSetBits(appStateFlagGroup, APP_FLAG_FACTORY_ROLLBACK);
}

void setupGPIO() {
  // Setup I2C
  Serial.println("Setting up GPIO...");
  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN, I2C_FREQ_HZ);  // Used for Sparkfun Thing
  // Wire.setClock(I2C_FREQ_HZ);  // 400KHz
  setupCO2Sensor(co2Error, co2Sensor);  // Setup PASCO2 Sensor
  setupSENSensor();
  setupMQSensors();
  pinMode(GPIO0_PIN, INPUT_PULLUP);
  // attachInterrupt(digitalPinToInterrupt(GPIO0_PIN), GPIO0_ISR, RISING | FALLING);

  Serial.println("Successfully set up GPIO.");
}
