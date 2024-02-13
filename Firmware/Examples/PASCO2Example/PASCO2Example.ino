/*
* Date: 2/10/24
* Author: Zane McMorris
* 
* This is the test file for the PAS CO2 Sensor hooked up via I2C.
* Using our ESP32-S3 we have our default I2C pins on 6 & 7.
* SCL = GPIO17, SDA = GPIO18
*/

#define I2C_SDAPin 17
#define I2C_SCLPin 18
#define I2C_FREQ_HZ 100000

#include <pas-co2-ino.hpp>  // https://github.com/Infineon/arduino-pas-co2-sensor - Version 3.1.1
PASCO2Ino co2Sensor;        // Global instance of the CO2 Sensor
bool sensorStatus = false;


void setupCO2Sensor() {
  Error_t errorPtr = co2Sensor.begin();
  Serial.println("PAS CO2: Setting up...");
  if (XENSIV_PASCO2_OK != errorPtr) {
    Serial.print("PAS CO2: initialization error: ");
    Serial.println(errorPtr);
  }
  /* We can set the reference pressure before starting
   * the measure
   */
  errorPtr = co2Sensor.setPressRef(1000);  // Use 1000hPa for standard pressure reference
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
    sensorStatus = true;
  }
  delay(5000);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.printf("Starting PAS CO2 test program...\n");
  bool wireStatus = Wire.begin(I2C_SDAPin, I2C_SCLPin, I2C_FREQ_HZ);  // Used for Sparkfun Thing
  if (wireStatus == false){
    Serial.println("Wire failed to start.");
    while(1);
  }
  setupCO2Sensor();
}
int16_t co2PPM;
Error_t errorPtr;

void loop() {
  // put your main code here, to run repeatedly:
  if (sensorStatus) {
    co2PPM = 0;
    do {
      errorPtr = co2Sensor.getCO2(co2PPM);
      if (errorPtr != 0) {
        Serial.print("Error reading CO2 w/ error code:");
        Serial.println(errorPtr);
      }
      delay(250);
    } while (co2PPM == 0);
    Serial.printf("CO2 Reading: %d\n", co2PPM);
    delay(5000); // Sensor has a minimum delay of 5 seconds between reads
  }
}
