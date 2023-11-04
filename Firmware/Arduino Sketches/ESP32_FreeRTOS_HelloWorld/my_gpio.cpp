#include "my_gpio.h"


// Sensor object definitions
PASCO2Ino co2Sensor;
int16_t co2PPM;
Error_t co2Error;
SensirionI2CSen5x sen5x;

// void setup_i2c() {
//   int i2c_master_port = 0;
//   i2c_config_t conf = {
//     .mode = I2C_MODE_MASTER,
//     .sda_io_num = I2C_SDA_PIN,  // select SDA GPIO specific to your project
//     .sda_pullup_en = GPIO_PULLUP_ENABLE,
//     .scl_io_num = I2C_SCL_PIN,  // select SCL GPIO specific to your project
//     .scl_pullup_en = GPIO_PULLUP_ENABLE,
//     .master.clk_speed = I2C_FREQ_HZ,  // select frequency specific to your project
//     .clk_flags = 0,                          // optional; you can use I2C_SCLK_SRC_FLAG_* flags to choose i2c source clock here
//   };

// }

void setupSENSensor() {
  Serial.println("SEN54: Setting up...");
  sen5x.begin(Wire);
  uint16_t error;
  char errorMessage[256];
  error = sen5x.deviceReset();
  if (error) {
    Serial.print("SEN54: Error trying to execute deviceReset(): ");
    errorToString(error, errorMessage, 256);
    Serial.println(errorMessage);
  }
  error = sen5x.setTemperatureOffsetSimple(0);  // No temp offset
  error = sen5x.startMeasurement();
  Serial.println("SEN54: Set up sensor successfully!");
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
  errorPtr = co2Sensor.startMeasure(5);
  if (XENSIV_PASCO2_OK != errorPtr) {
    Serial.print("PAS CO2: startmeasure error: ");
    Serial.println(errorPtr);
  }

  if (errorPtr == XENSIV_PASCO2_OK)
    Serial.println("PAS CO2: Set up sensor successfully!");
}

float readNGSensor() {
  float reading = (3.3 * analogRead(NGPin)) / 4095;
  return 10.938 * exp(1.7742 * (reading * 3.3 / 4095));
}

float readCOSensor() {
  float sensorVoltage = (3.3 * analogRead(COPin)) / 4095;
  return 3.027 * exp(1.0698 * sensorVoltage);
}

void readSENSensor(float16 *retArray, uint8_t arraySize) {
  char errorMessage[256];
  uint16_t error;
  float tempArray[arraySize];

  error = sen5x.readMeasuredValues(
    tempArray[0], tempArray[1], tempArray[2],
    tempArray[3], tempArray[4], tempArray[5], tempArray[6],
    tempArray[7]);
  // Copy tempArray for conversion to float16
  for (int i = 0; i < arraySize; i++) {
    retArray[i] = tempArray[i];
  }

  if (error) {
    Serial.print("Error trying to execute readMeasuredValues(): ");
    errorToString(error, errorMessage, 256);
    Serial.println(errorMessage);
  }
}

uint16_t readCO2PPM(Error_t errorPtr, PASCO2Ino CO2SensorPtr) {
  co2PPM = 0;
  do {
    errorPtr = co2Sensor.getCO2(co2PPM);
    if (errorPtr != 0) {
      Serial.print("Error reading CO2 w/ error code:");
      Serial.println(errorPtr);
    }

    delay(1000);
  } while (co2PPM == 0);
  return co2PPM;
}

void read_all_sensors(float16 *ret_array, uint16_t array_size) {
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
  10: AQI
  */
  // CO2
  float16 co2_ppm_return = readCO2PPM(co2Error, co2Sensor);
  ret_array[0] = co2_ppm_return;
  // SEN
  // Uses ret_array[1] through ret_array[7]
  readSENSensor(&ret_array[1], 7);
  // CO
  float16 COReading = readCOSensor();
  ret_array[8] = COReading;
  // Serial.print("CO ppm: ");
  // Serial.println(ret_array[9]);
  // NG
  float16 NGReading = readNGSensor();
  ret_array[9] = NGReading;
  // Serial.print("NG ppm: ");
  // Serial.println(ret_array[10]);
  ret_array[10] = get_composite_AQI(ret_array[2], ret_array[4], COReading);
}

void setup_GPIO() {
  // Setup I2C
  // Wire.begin(); // For breadboard QT-PY ESP32
  Serial.begin(115200);
  Serial.println("Setting up GPIO...");
  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN, I2C_FREQ_HZ);  // Used for Sparkfun Thing
  // Wire.setClock(I2C_FREQ_HZ);  // 400KHz

  setupCO2Sensor(co2Error, co2Sensor);  // Setup PASCO2 Sensor
  setupSENSensor();
  Serial.println("Successfully set up GPIO.");
}
