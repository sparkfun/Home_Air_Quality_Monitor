/*
* Date: 2/10/24
* Author: Zane McMorris
* 
* This file contains simple test code for the SEN5x sensor communicating over I2C
* For our ESP32-S3 we're using pins 17 and 18 for I2C communication.
*/

#include <SensirionI2CSen5x.h> // https://github.com/Sensirion/arduino-i2c-sen5x - Version 0.3.0

#define I2C_SDA 17
#define I2C_SCL 18
#define I2C_FREQ_HZ 100000

SensirionI2CSen5x sen5x;
static bool senStatus = false;
float senDataArray[8];
enum sensorMap {
  PPM_1_0 = 1,
  PPM_2_5 = 2,
  PPM_4_0 = 3,
  PPM_10 = 4,
  HUMIDITY = 5,
  TEMP = 6,
  VOC = 7,
  NOX = 8
};

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
    senStatus = true;
    delay(3000);
  }
}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Starting SEN5x test program...");
  Wire.begin(I2C_SDA, I2C_SCL, I2C_FREQ_HZ);
  setupSENSensor();
}

char errorMessage[256];
uint16_t error;

void loop() {
  error = sen5x.readMeasuredValues(senDataArray[PPM_1_0], senDataArray[PPM_2_5],
                                    senDataArray[PPM_4_0], senDataArray[PPM_10],
                                    senDataArray[HUMIDITY], senDataArray[TEMP],
                                    senDataArray[VOC], senDataArray[NOX]);

  if (error) {
    Serial.print("Error trying to execute readMeasuredValues(): ");
    errorToString(error, errorMessage, 256);
    Serial.println(errorMessage);
  } else {
    for(int i = 0;i<=8;i++){
      Serial.printf("%d, ", senDataArray[i]);
    }
    Serial.printf("\n");
  }
  delay(3000);
}

