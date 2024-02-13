/*
* Date: 2/10/2024 
* Author: Zane McMorris
*
* This is the example code for the MQ sensors setup on any pins capable of ADC input. 
* For our ESP32-S3, we have chosen pins 6 and 7 to be our standard inputs.
*
*/

#define NGPin 6
#define COPin 7


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.printf("Starting MQ test program...\n");

}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.printf("NG Reading: %d\n", analogRead(NGPin));
  Serial.printf("CO Reading: %d\n", analogRead(COPin);

  delay(1000);
}
