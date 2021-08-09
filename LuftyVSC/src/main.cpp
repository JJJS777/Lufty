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
#include "bsec.h"

#define BLYNK_PRINT Serialear

// Helper functions declarations
void checkIaqSensorStatus(void);
void errLeds(void);

// Create an object of the class Bsec
Bsec iaqSensor;

const char *ssid = "Duckn3t";
const char *password = "quack1QUACK4quack1";
const char auth[] = "h14JLgNFT8QLSKFXGJ9c9z9sSv5Lm8TX";

//Globale Variable
float rawTemperature, temperature, rawHumidity, humidity, pressure, iaqData, iaqAccuracy, staticIaq, gasResistance, co2Equivalent, breathVocEquivalent;
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
unsigned long timerDelay = 30000;

String jsonBuffer, output;

void getBME680data()
{
  rawTemperature = iaqSensor.rawTemperature;
  temperature = iaqSensor.temperature;
  rawHumidity = iaqSensor.rawHumidity;
  humidity = iaqSensor.humidity;
  pressure = iaqSensor.pressure;
  iaqData = iaqSensor.iaq;
  iaqAccuracy = iaqSensor.iaqAccuracy;
  staticIaq = iaqSensor.staticIaq;
  gasResistance = iaqSensor.gasResistance;
  co2Equivalent = iaqSensor.co2Equivalent;
  breathVocEquivalent = iaqSensor.breathVocEquivalent;

  unsigned long time_trigger = millis();
  if (iaqSensor.run()) { // If new data is available
    Serial.print(F("Raw-Temperature = "));
    Serial.print(rawTemperature);
    Serial.println(F(" °C"));

    Serial.print(F("Temperature = "));
    Serial.print(temperature);
    Serial.println(F(" °C"));

    Serial.print(F("Raw-Humidity = "));
    Serial.print(rawHumidity);
    Serial.println(F(" %"));

    Serial.print(F("Humidity = "));
    Serial.print(humidity);
    Serial.println(F(" %"));

    Serial.print(F("Pressure = "));
    Serial.print(pressure / 100.0);
    Serial.println(F(" hPa"));

    Serial.print(F("Index of Air Quality = "));
    Serial.print(iaqData);
    Serial.println();

    /*IAQ Accuracy (begins at 0 after start up, goes to 1 after a few minutes and 
    reaches 3 when the sensor is calibrated). The initial value of the IAQ index is 25.00. 
    and it stays that way for a good while. Only after several minutes does the IAQ value starts to drift. */
    Serial.print(F("Genauigkeit des Index of Air Quality = "));
    Serial.print(iaqAccuracy);
    Serial.println();

    Serial.print(F("Gas = "));
    Serial.print(gasResistance / 1000.0);
    Serial.println(F(" KOhms"));

    Serial.println();
  } else {
    checkIaqSensorStatus();
  }

  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(V5, humidity);
  Blynk.virtualWrite(V6, temperature);
  delay(2000);
}

String httpGETRequest(const char *serverName)
{
  HTTPClient http;

  // Your IP address with path or Domain name with URL path
  http.begin(serverName);

  // Send HTTP POST request
  int httpResponseCode = http.GET();

  String payload = "{}";

  if (httpResponseCode > 0)
  {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  }
  else
  {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();

  return payload;
}

void sendHttpGetReq(){
  // Send an HTTP GET request
  if ((millis() - lastTime) > timerDelay)
  {
    // Check WiFi connection status
    if (WiFi.status() == WL_CONNECTED)
    {
      String serverPath = "http://api.openweathermap.org/data/2.5/weather?q=" + city + "," + countryCode + "&APPID=" + openWeatherMapApiKey;

      jsonBuffer = httpGETRequest(serverPath.c_str());
      Serial.println(jsonBuffer);
      JSONVar myObject = JSON.parse(jsonBuffer);

      // JSON.typeof(jsonVar) can be used to get the type of the var
      if (JSON.typeof(myObject) == "undefined")
      {
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
    else
    {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }
}

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(9600);

  /* WiFi Connection */
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }

  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

  /*Blynk*/
  Blynk.config(auth);
  Blynk.connect();
  
  /*BME680 w/ BSEC*/
  Wire.begin();
  iaqSensor.begin(BME680_I2C_ADDR_PRIMARY, Wire);
  checkIaqSensorStatus();

  bsec_virtual_sensor_t sensorList[10] = {
      BSEC_OUTPUT_RAW_TEMPERATURE,
      BSEC_OUTPUT_RAW_PRESSURE,
      BSEC_OUTPUT_RAW_HUMIDITY,
      BSEC_OUTPUT_RAW_GAS,
      BSEC_OUTPUT_IAQ,
      BSEC_OUTPUT_STATIC_IAQ,
      BSEC_OUTPUT_CO2_EQUIVALENT,
      BSEC_OUTPUT_BREATH_VOC_EQUIVALENT,
      BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_TEMPERATURE,
      BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_HUMIDITY,
  };

  iaqSensor.updateSubscription(sensorList, 10, BSEC_SAMPLE_RATE_LP);
  checkIaqSensorStatus();
}

void loop()
{
  // put your main code here, to run repeatedly:
  Blynk.run();
  timer.run();
  getBME680data();
  sendHttpGetReq();
}


// Helper function definitions
void checkIaqSensorStatus(void)
{
  if (iaqSensor.status != BSEC_OK) {
    if (iaqSensor.status < BSEC_OK) {
      output = "BSEC error code : " + String(iaqSensor.status);
      Serial.println(output);
      for (;;)
        errLeds(); /* Halt in case of failure */
    } else {
      output = "BSEC warning code : " + String(iaqSensor.status);
      Serial.println(output);
    }
  }

  if (iaqSensor.bme680Status != BME680_OK) {
    if (iaqSensor.bme680Status < BME680_OK) {
      output = "BME680 error code : " + String(iaqSensor.bme680Status);
      Serial.println(output);
      for (;;)
        errLeds(); /* Halt in case of failure */
    } else {
      output = "BME680 warning code : " + String(iaqSensor.bme680Status);
      Serial.println(output);
    }
  }
}

void errLeds(void)
{
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(100);
  digitalWrite(LED_BUILTIN, LOW);
  delay(100);
}