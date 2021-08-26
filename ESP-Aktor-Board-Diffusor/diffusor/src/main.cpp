#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>


 
static const int diffusorPin = 32;
void setup()
{
  Serial.begin(9600);
  pinMode(diffusorPin, OUTPUT); // Set diffusorPin as digital output pin
}
void diffusor_on()
{
  digitalWrite(diffusorPin, HIGH);
  delay(110);
  digitalWrite(diffusorPin, LOW);
}
void diffusor_off()
{
  digitalWrite(diffusorPin, HIGH);
  delay(110);
  digitalWrite(diffusorPin, LOW);
  delay(110);
  digitalWrite(diffusorPin, HIGH);
  delay(110);
  digitalWrite(diffusorPin, LOW);
}
void loop()
{
  diffusor_on();
  delay(6000);
  diffusor_off();
  delay(6000);
}