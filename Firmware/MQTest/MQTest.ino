#define gatePin 4
#define sensorInputPin 7


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(gatePin, OUTPUT);
  pinMode(sensorInputPin, INPUT);
}



int i = 0;
bool sensorStatus = true;
void loop() {
  // put your main code here, to run repeatedly:
  i++;
  if(sensorStatus)
  {
    if (i >= 60){
      sensorStatus = !sensorStatus;
      analogWrite(gatePin, sensorStatus ? 255 : 72);
      i=0;
    }
    
  } else {
    if(i>=90){
      sensorStatus = !sensorStatus;
      analogWrite(gatePin, sensorStatus ? 255 : 72);
      i=0;
    }
  }

  Serial.printf("\n");
  Serial.printf("Time: %i\n", i);
  Serial.printf("Sensor reading: %i\n", analogRead(sensorInputPin));
  Serial.printf("Gate status: %s\n", sensorStatus ? "true" : "False");
  delay(1000);

}
