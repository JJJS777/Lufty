#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <DHT.h>

#define DHTPIN 14
#define DHTTYPE DHT22

//Globale Variable
float temperature, humidity;

DHT my_sensor( DHTPIN, DHTTYPE);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  my_sensor.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
  humidity = my_sensor.readHumidity();
  temperature = my_sensor.readTemperature();

  Serial.print("Temperatur: ");
  Serial.print(temperature);
  Serial.print("°C / Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");

  delay(4000);
}
