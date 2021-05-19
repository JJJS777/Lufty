#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <DHT.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

#define BLYNK_PRINT Serial
#define DHTPIN 18
#define DHTTYPE DHT11

const char* ssid = "Duckn3t";
const char* password =  "quack1QUACK4quack1";
const char auth[] = "h14JLgNFT8QLSKFXGJ9c9z9sSv5Lm8TX";

//Globale Variable
float temperature, humidity;
int ledPin = 14;
int buttonPressed = 0; //Variable global def.
int buttonPin = 26; 

DHT my_sensor( DHTPIN, DHTTYPE);
BlynkTimer timer;

void ISRbuttonClicked(){
  buttonPressed = digitalRead(buttonPin);
}

void sendSensor() {
  humidity = my_sensor.readHumidity();
  temperature = my_sensor.readTemperature();

  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

    if (temperature < 19.00 || temperature > 22.00 || humidity < 50.00  ){
    digitalWrite(ledPin, HIGH);
    Serial.println("Messung Prüfen -> Knopf Drücken");
  } else {
    digitalWrite(ledPin, LOW);
  }

  if ( buttonPressed > 0 ) {
    Serial.print("Temperatur: ");
    Serial.print(temperature);
    Serial.print("°C / Humidity: ");
    Serial.print(humidity);
    Serial.println(" %");
  }

  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(V5, humidity);
  Blynk.virtualWrite(V6, temperature);
}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  /*Blynk*/
  WiFi.begin(ssid, password);
  Blynk.config(auth);
  Blynk.connect();

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }

  Serial.println("Connected to the WiFi network");

  pinMode(ledPin, OUTPUT); //Output Pin wird initalisiert, Zahl auf dem Mainboard am gelben Kabel
  pinMode(buttonPin, INPUT); //Input pin initalisieren
  attachInterrupt(buttonPin, ISRbuttonClicked, CHANGE);
  my_sensor.begin();

  // Setup a function to be called every second
  timer.setInterval(1000L, sendSensor);
}

void loop() {
  // put your main code here, to run repeatedly:
Blynk.run();
timer.run();

}
