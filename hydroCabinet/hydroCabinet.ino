#include "hydroCabinet.h"

hydroCabinet hydro;
const int AIR_PUMP = 13;
const int LIGHTS = 12;

unsigned long previous = 0;    
const int interval = 6000;

void setup() {
  Serial.begin(115200);
  pinMode(AIR_PUMP, OUTPUT);
  hydro.init(Serial);
  digitalWrite(AIR_PUMP, HIGH);
}

void loop() {
  hydro.clientHandler();
  
  unsigned long current = millis();
  if(current - previous >= interval){
    previous = millis();
    Serial.print("Update lights");
    hydro.lights(Serial);
  }
  
}
