#include <Arduino.h>


void setup()
{
  Serial.begin(9600);
  pinMode(32, OUTPUT); // Set GPIO22 as digital output pin
}

void loop()
{
  delay(6000);

  //ON DIFFUSOR
  digitalWrite(32, HIGH);
  delay(60);              
  digitalWrite(32, LOW);

  delay(6000);

  //OFF DIFFUSOR
  digitalWrite(32, HIGH);
  delay(60);              
  digitalWrite(32, LOW);
  delay(100);
  digitalWrite(32, HIGH); 
  delay(60);              
  digitalWrite(32, LOW);
}