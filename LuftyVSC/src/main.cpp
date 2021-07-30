#include <Arduino.h>
#include <Adafruit_I2CDevice.h>
#include <Wire.h>
#include <SPI.h>
#include <DHT.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <Arduino_JSON.h>
#include <HTTPClient.h>
#include "Adafruit_BME680.h"

#define BLYNK_PRINT Serial

Adafruit_BME680 bme; // I2C

const char* ssid = "Duckn3t";
const char* password =  "quack1QUACK4quack1";
const char auth[] = "h14JLgNFT8QLSKFXGJ9c9z9sSv5Lm8TX";

//Globale Variable
float temperature, humidity, pressure, gasResistance;
int ledPin = 14;

BlynkTimer timer;

/*OpenWeatherMap.org*/
// Your Domain name with URL path or IP address with path
String openWeatherMapApiKey = "a07094aa292e8325eaf597b2f9ce6e44";

// Replace with your country code and city
String city = "Cologne";
String countryCode = "DE";

// THE DEFAULT TIMER IS SET TO 10 SECONDS FOR TESTING PURPOSES
// For a final application, check the API call limits per hour/minute to avoid getting blocked/banned
unsigned long lastTime = 0;
// Timer set to 10 minutes (600000)
//unsigned long timerDelay = 600000;
// Set timer to 10 seconds (10000)
unsigned long timerDelay = 30000;

String jsonBuffer;

void getBME680data() {
  
  unsigned long endTime = bme.beginReading();
  if(endTime == 0){
    Serial.println(F("Failed to begin reading"));
    return;
  }

  if(!bme.endReading()){
    Serial.println(F("Failed to Complete reading"));
    return;
  }
  
  humidity = bme.humidity;
  temperature = bme.temperature;
  pressure = bme.pressure;
  gasResistance = bme.gas_resistance;

  // Serial.println(humidity);

  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(V5, humidity);
  Blynk.virtualWrite(V6, temperature);
}

String httpGETRequest(const char* serverName) {
  HTTPClient http;
    
  // Your IP address with path or Domain name with URL path 
  http.begin(serverName);
  
  // Send HTTP POST request
  int httpResponseCode = http.GET();
  
  String payload = "{}"; 
  
  if (httpResponseCode>0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();

  return payload;
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  /*Blynk*/
  WiFi.begin(ssid, password);
  Blynk.config(auth);
  Blynk.connect();

  /* WiFi Connection */
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }

  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

  //Init BME680
  if(!bme.begin()){
    Serial.println(F("Could not find a valid BME680 sensor, check wiring!"));
    while(1);
  }
  
  pinMode(ledPin, OUTPUT); //Output Pin wird initalisiert, Zahl auf dem Mainboard am gelben Kabel

  // Set up oversampling and filter initialization
  bme.setTemperatureOversampling(BME680_OS_8X);
  bme.setHumidityOversampling(BME680_OS_2X);
  bme.setPressureOversampling(BME680_OS_4X);
  bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
  bme.setGasHeater(320, 150); // 320*C for 150 ms

  // Setup a function to be called every second
  timer.setInterval(10000L, getBME680data );
}

void loop() {
  // put your main code here, to run repeatedly:
  Blynk.run();
  timer.run();
  getBME680data();

  // Send an HTTP GET request
  if ((millis() - lastTime) > timerDelay) {
    // Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED){
      String serverPath = "http://api.openweathermap.org/data/2.5/weather?q=" + city + "," + countryCode + "&APPID=" + openWeatherMapApiKey;
      
      jsonBuffer = httpGETRequest(serverPath.c_str());
      Serial.println(jsonBuffer);
      JSONVar myObject = JSON.parse(jsonBuffer);
  
      // JSON.typeof(jsonVar) can be used to get the type of the var
      if (JSON.typeof(myObject) == "undefined") {
        Serial.println("Parsing input failed!");
        return;
      }
    
      Serial.print("JSON object = ");
      Serial.println(myObject);
      Serial.print("Temperature: ");
      Serial.println(myObject["main"]["temp"]);
      Serial.print("Pressure: ");
      Serial.println(myObject["main"]["pressure"]);
      Serial.print("Humidity: ");
      Serial.println(myObject["main"]["humidity"]);
      Serial.print("Wind Speed: ");
      Serial.println(myObject["wind"]["speed"]);
    }
    else {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }
}