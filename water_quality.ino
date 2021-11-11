#include<ArduinoJson.h>

const int SIZE = 100;

/////Config Sensor de temperatura
int TermistorPin = A0;
int V0;
float R1 = 10000;
float logR2, R2, T, Tc, Tf, temp;
float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;

/////Config Sensor de PH
int LdrPin0 = A1;
int V1;
float ph;

/////Config Sensor de turbidez
int LdrPin1 = A2;
int V2;
float minTurbidez = 99999990;
float maxTurbidez = 0;
float turbidez;


/////Config Led
int Led = 13;

/////Config Time
unsigned long timeIn = 0;
unsigned char initTime = 0;
unsigned int delayInMilliseconds = 30000;

void setup() {
  Serial.begin(9600);
  Serial.setTimeout(10);
  pinMode(TermistorPin, INPUT);
  pinMode(LdrPin0, INPUT);
  pinMode(LdrPin1, INPUT);
  pinMode(Led, OUTPUT);
}

void loop() {

  if(initTime == 1 && (millis() - timeIn > delayInMilliseconds)) {
    initTask();
  }

  if(initTime == 0) {
    resetTask();
  }

  if(Serial.available() > 0) {
    String recivedString = readStringSerial();
    
    if(recivedString == "ACTIVATE") {
      turnOnLed();
    }
  }

}

void initTask() {
  StaticJsonDocument<SIZE> json;
  
  temp = readTemp(TermistorPin);
  ph = readPh(LdrPin0);
  turbidez = readTurb(LdrPin1);
  
  json["sensorTemp"] = temp;
  json["sensorPh"] = ph;
  json["sensorTurbidez"] = turbidez;

  serializeJson(json, Serial);
  Serial.println();
  
  
  initTime = 0;
}

String readStringSerial() {
  String tmpString = "";
  char caracter;

  while(Serial.available() > 0) {
    caracter = Serial.read();

    if(caracter != '\n') {
      tmpString.concat(caracter);
    }
    delay(10);
  }
  return tmpString;
}

void turnOnLed() {
  digitalWrite(Led, HIGH);
  delay(1000);      
  digitalWrite(Led, LOW);
  delay(500); 
}
  
void resetTask() {
  initTime = 1;
  timeIn = millis();
}

float readPh(int tmpLdrPin0){
  int value = analogRead(tmpLdrPin0);
  float volt = float(value) / 1023 * 5.0;
  float phValue = 2.63 * volt;

  return phValue;
}

float readTurb(int tmpLdrPin1){
  int value = analogRead(tmpLdrPin1);
  
  if(value > maxTurbidez) {
    maxTurbidez = value;
  }

  if(value < minTurbidez) {
    minTurbidez = value;
  }
  
  int ldrValue = map(value, minTurbidez, maxTurbidez, 0, 100);

  if(ldrValue < 0) {
    ldrValue = 0;
  }

  if(ldrValue > 100) {
    ldrValue = 100;
  }
  
  return ldrValue;
}


float readTemp(int ThermistorPin){
  V0 = analogRead(ThermistorPin);
  R2 = R1 * (1023.0 / (float)V0 - 1.0);
  logR2 = log(R2);
  T = (1.0 / (c1 + c2*logR2 + c3*logR2*logR2*logR2));// Equação de Steinhart–Hart 
  Tc = T - 273.15; //temp em Graus celcius

  return Tc;
}
