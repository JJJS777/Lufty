#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <DHT.h>
#include "WiFi.h"

#define DHTPIN 18
#define DHTTYPE DHT11

const char* ssid = "Duckn3t";
const char* password =  "quack1QUACK4quack1";

//Globale Variable
float temperature, humidity;
int ledPin = 14;
int buttonPressed = 0; //Variable global def.
int buttonPin = 26; 

DHT my_sensor( DHTPIN, DHTTYPE);

void ISRbuttonClicked(){
  buttonPressed = digitalRead(buttonPin);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  /*WLAN*/
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }

  Serial.println("Connected to the WiFi network");

  pinMode(ledPin, OUTPUT); //Output Pin wird initalisiert, Zahl auf dem Mainboard am gelben Kabel
  pinMode(buttonPin, INPUT); //Input pin initalisieren
  attachInterrupt(buttonPin, ISRbuttonClicked, CHANGE);
  my_sensor.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
  
  humidity = my_sensor.readHumidity();
  temperature = my_sensor.readTemperature();

  if (temperature < 19.00 || temperature > 23.00 || humidity < 50.00  ){
    digitalWrite(ledPin, HIGH);
    Serial.println("Messung Prüfen -> Knopf Drücken");
  }

  if ( buttonPressed > 0 ) {
    Serial.print("Temperatur: ");
    Serial.print(temperature);
    Serial.print("°C / Humidity: ");
    Serial.print(humidity);
    Serial.println(" %");
  }

  delay(2000);
}