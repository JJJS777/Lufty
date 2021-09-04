#include <Arduino.h>
#include <Adafruit_I2CDevice.h>
#include <Wire.h>
#include <SPI.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <Arduino_JSON.h>
#include <HTTPClient.h>
#include <AsyncMqttClient.h>
#include "bsec.h"
#include "Adafruit_BME680.h"

extern "C" {
  #include "freertos/FreeRTOS.h"
  #include "freertos/timers.h"
}

#define BLYNK_PRINT Serialear
#define MQTT_HOST IPAddress(192, 168, 141, 99)
#define MQTT_PORT 1883
#define MQTT_PUB_DIFFUSOR "esp/sensorBoard/diffusor"
#define MQTT_PUB_WINDOW "esp/sensorBoard/window"

// Helper functions declarations used by BSEC
void checkIaqSensorStatus(void);
void errLeds(void);

// Create an instance of the class Bsec
Bsec iaqSensor;

// Erzeugen eine Instanz der Klasse AsyncMqttClient namens mqttClient um die MQTT clients zu verwalten
// Timer Instanzen verwalten das wieder verbinden mit MQTT Broke und Router
AsyncMqttClient mqttClient;
TimerHandle_t mqttReconnectTimer;
TimerHandle_t wifiReconnectTimer;

const char *ssid = "Duckn3t";
const char *password = "quack1QUACK4quack1";
const char auth[] = "h14JLgNFT8QLSKFXGJ9c9z9sSv5Lm8TX";

//Globale Variable
float rawTemperature, temperature, rawHumidity, humidity, pressure, iaqData, staticIaq, gasResistance, co2Equivalent, breathVocEquivalent;
double apiTemp, windspeed;
int aqiApi, iaqAccuracy;
BlynkTimer timer;

/*OpenWeatherMap.org*/
// Your Domain name with URL path or IP address with path
String openWeatherMapApiKey = "a07094aa292e8325eaf597b2f9ce6e44";
String weatherbitAirQualityApiKey = "8039659f67584d818415250bf91dfb17";

// Replace with your country code and city
String city = "Cologne";
String countryCode = "DE";
String unitsApi = "metric";
/*lat und lon können auch aus erstem API Req. rausgeholt und verwendet werden, anstelle sie hart zu coden*/
String latitude = "50.935173";
String longitude = "6.953101";

// THE DEFAULT TIMER IS SET TO 10 SECONDS FOR TESTING PURPOSES
// For a final application, check the API call limits per hour/minute to avoid getting blocked/banned
unsigned long lastTime = 0;
unsigned long timerDelay = 600000;

//MQTT-Timer-Hilfsvariable die alle 60 sec die Anweisung an den Broker publiziert
unsigned long previousMills = 0;
const long interval = 60000;
// time to unblock window
unsigned long window_unblock_time = 0;

String jsonBufferWeather, jsonBufferPollution, output;

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
  if (iaqSensor.run())
  { // If new data is available
    Serial.println("_____\n");
    Serial.println("BME680-Sensor Daten\n");
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

    Serial.println("_____\n");

    Serial.println();
  }
  else
  {
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

void decodingJSON()
{
  if ((millis() - lastTime) > timerDelay)
  {
    // Check WiFi connection status
    if (WiFi.status() == WL_CONNECTED)
    {
      String serverPathWeather = "http://api.openweathermap.org/data/2.5/weather?q=" + city + "," + countryCode + "&units=" + unitsApi + "&APPID=" + openWeatherMapApiKey;
      String serverPathAirQuality = "https://api.weatherbit.io/v2.0/current/airquality?lat=" + latitude + "&lon=" + longitude + "&key=" + weatherbitAirQualityApiKey;

      Serial.println("_____\n");
      Serial.println("API-Call\n");

      /* Sendet einen HTTP GET request uns speichert die Res. in der Variable jsonBuffer. The httpGETRequest() function makes a 
      request to OpenWeatherMap and it retrieves a string with a JSON object that contains all the information about the weather for your city.*/
      jsonBufferWeather = httpGETRequest(serverPathWeather.c_str());
      jsonBufferPollution = httpGETRequest(serverPathAirQuality.c_str());

      JSONVar myObjectWeather = JSON.parse(jsonBufferWeather);
      JSONVar myObjectPollution = JSON.parse(jsonBufferPollution);

      // JSON.typeof(jsonVar) can be used to get the type of the var
      if (JSON.typeof(myObjectWeather) == "undefined")
      {
        Serial.println("Parsing Weather-Input failed!");
        return;
      }

      if (JSON.typeof(myObjectPollution) == "undefined")
      {
        Serial.println("Parsing Pollution-Input failed!");
        return;
      }

      /*Außentemperatur aus API Req. in Variable speichern*/
      apiTemp = myObjectWeather["main"]["temp"];

      /*Air Quality Index aus API Req. in Variable speichern*/
      aqiApi = myObjectPollution["data"][0]["aqi"];

      /*Außenwindgeschwindigkeit aus API Req. in Variable speichern*/
      windspeed = myObjectWeather["wind"]["speed"];

      Serial.println("\n\nDaten aus Open-WeatherMap:");
      Serial.print("Temperatur aus API: ");
      Serial.print(apiTemp);
      Serial.println("°C");
      Serial.print("Air Quality Index aus API: ");
      Serial.println(aqiApi);
      Serial.println();
      Serial.println(myObjectPollution);
      Serial.println("\n_____\n\n");
    }
    else
    {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }
}

/* WiFi Connection: Callback-Fkt, wird asynch ausgeführt */
void connectToWifi()
{
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
}

/*Callback-Fkt, wird asynch ausgeführt*/
void connectToMqtt()
{
  Serial.println("Connecting to MQTT Broker ...");
  mqttClient.connect();
}

/*WiFiEventHandler Verwaltet die WiFi-Events: Callback-Fkt, wird asynch ausgeführt*/
void WiFiEventHandler(WiFiEvent_t event)
{
  Serial.printf("[WiFi-event] event: %d\n", event);
  switch (event)
  {
  case SYSTEM_EVENT_STA_GOT_IP:
    Serial.print("Connected to WiFi network with IP Address: ");
    Serial.println(WiFi.localIP());
    connectToMqtt();
    break;
  case SYSTEM_EVENT_STA_DISCONNECTED:
    Serial.println("Lost WiFi connection ...");
    xTimerStop(mqttReconnectTimer, 0);
    xTimerStart(wifiReconnectTimer, 0);
    break;
  }
}

/*The onMqttConnect() function runs after starting a session with the broker : Callback-Fkt, wird asynch ausgeführt*/
void onMqttConnect(bool sessionPresent)
{
  Serial.println("Connected to MQTT.");
  Serial.print("Session present: ");
  Serial.println(sessionPresent);
}

/*Wenn ESP32 die verbindung zum MQTT-Broker verliert wird onMqttDisconnect aufgerufen: Callback-Fkt, wird asynch ausgeführt*/
void onMqttDisconnect(AsyncMqttClientDisconnectReason reason)
{
  Serial.println("Disconnected from MQTT.");
  if (WiFi.isConnected())
  {
    xTimerStart(mqttReconnectTimer, 0);
  }
}

/*onMqttPublish wird aufgerufen, wenn eine MSG auf einem MQTT-Topic publiziert wird: Callback-Fkt, wird asynch ausgeführt*/
void onMqttPublish(uint16_t packetId)
{
  Serial.println("Publish acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
}

/****** ANWENDUNGSLOGIK ******/

// void closeWindow()
// {
//   // Publish an MQTT message on topic esp/sensorBoard/window/close
//   uint16_t packetIdPub1 = mqttClient.publish(MQTT_PUB_TEMP, MQTT_QoS, true, String(temperature).c_str());
//   Serial.printf("Publishing on topic %s at %i, packetId: %i", MQTT_PUB_TEMP, MQTT_QoS, packetIdPub1);
//   Serial.printf("Message: closing Window...");
// }

// void checkConditions()
// {
//   if (badConditionsInside && badConditionsOutside)
//   {
//     closeWindow();
//     deffusorOn();
//   }
//   else if (badConditionsInside && goodConditionsOutside)
//   {
//     openWindow();
//     diffusorOff();
//   }
//   else
//   {
//     msgUser();
//   }
// }

// void diffusorControle()
// {
// }

/****** ANWENDUNGSLOGIK ENDE ******/

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(9600);

  /*The next two lines create timers that will allow both the MQTT broker and Wi-Fi connection to reconnect, in case the connection is lost.*/
  mqttReconnectTimer = xTimerCreate("mqttTimer", pdMS_TO_TICKS(2000), pdFALSE, (void *)0, reinterpret_cast<TimerCallbackFunction_t>(connectToMqtt));
  wifiReconnectTimer = xTimerCreate("wifiTimer", pdMS_TO_TICKS(2000), pdFALSE, (void *)0, reinterpret_cast<TimerCallbackFunction_t>(connectToWifi));

  /*Zuweisung einer Callback-Fkt: wenn der ESP sich mit dem WiFi verbindet wird die WiFiEventHandler-Fkt aufgerufen*/
  WiFi.onEvent(WiFiEventHandler);

  /*Zuweisung weiterer Callback-Fkt'en. Die zugewiesen Fkt werden automatisch aufgerufen, sobald diese Benötigt werden*/
  mqttClient.onConnect(onMqttConnect);
  mqttClient.onDisconnect(onMqttDisconnect);
  mqttClient.onPublish(onMqttPublish);
  mqttClient.setServer(MQTT_HOST, MQTT_PORT);

  /*MQTT-Broker Authentifizierung: wenn der MQTT-Broker auth benötigt*/
  mqttClient.setCredentials("REPlACE_WITH_YOUR_USER", "REPLACE_WITH_YOUR_PASSWORD");

  /*mit WiFi verbindne durch das aufrufen der connectToWifi-Fkt*/
  connectToWifi();

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
  /*Blynk*/
  Blynk.run();
  timer.run();

  getBME680data();
  decodingJSON();

  unsigned long currentMillis = millis();

  // Every X number of seconds (interval = 10 seconds)
  // it publishes a new MQTT message
  if (currentMillis - previousMills >= interval)
  {
    // Save the last time a new reading was published
    previousMills = currentMillis;

    if (aqiApi < iaqData && windspeed < 60 && window_unblock_time <= currentMillis)
    {
      //Diffusor OFF
      uint16_t packetIdPub2 = mqttClient.publish(MQTT_PUB_DIFFUSOR, 1, true, String(0).c_str());
      Serial.printf("Publishing on topic %s at QoS 1, packetId: %i \n", MQTT_PUB_DIFFUSOR, packetIdPub2);
      //Window OPEN
      uint16_t packetIdPub3 = mqttClient.publish(MQTT_PUB_WINDOW, 1, true, String(1).c_str());
      Serial.printf("Publishing on topic %s at QoS 1, packetId %i: \n", MQTT_PUB_WINDOW, packetIdPub3);
      //Grenzwertige Temp Fall
      if (apiTemp <= 5 || apiTemp >= 30)
      {
        delay(5000);
            // Window CLOSE
            uint16_t packetIdPub4 = mqttClient.publish(MQTT_PUB_WINDOW, 1, true, String(0).c_str());
        Serial.printf("Publishing on topic %s at QoS 1, packetId %i: \n", MQTT_PUB_WINDOW, packetIdPub4);
        //Block fenster für 2 stunden
        window_unblock_time = millis() + 7200000;
      }
    }
    else
    {
      // Window CLOSE
      uint16_t packetIdPub4 = mqttClient.publish(MQTT_PUB_WINDOW, 1, true, String(0).c_str());
      Serial.printf("Publishing on topic %s at QoS 1, packetId %i: \n", MQTT_PUB_WINDOW, packetIdPub4);
      if (humidity <= 40)
      {
        //DIFFUSOR ON
        uint16_t packetIdPub1 = mqttClient.publish(MQTT_PUB_DIFFUSOR, 1, true, String(1).c_str());
        Serial.printf("Publishing on topic %s at QoS 1, packetId: %i \n", MQTT_PUB_DIFFUSOR, packetIdPub1);
      }
      if (humidity >= 60)
      {
        //DIFFUSOR OFF
        uint16_t packetIdPub2 = mqttClient.publish(MQTT_PUB_DIFFUSOR, 1, true, String(0).c_str());
        Serial.printf("Publishing on topic %s at QoS 1, packetId: %i \n", MQTT_PUB_DIFFUSOR, packetIdPub2);
      }
    }
  }

  checkIaqSensorStatus();

}



// Helper function definitions

void checkIaqSensorStatus(void)
{
  if (iaqSensor.status != BSEC_OK)
  {
    if (iaqSensor.status < BSEC_OK)
    {
      output = "BSEC error code : " + String(iaqSensor.status);
      Serial.println(output);
      for (;;)
        errLeds(); /* Halt in case of failure */
    }
    else
    {
      output = "BSEC warning code : " + String(iaqSensor.status);
      Serial.println(output);
    }
  }
  if (iaqSensor.bme680Status != BME680_OK)
  {
    if (iaqSensor.bme680Status < BME680_OK)
    {
      output = "BME680 error code : " + String(iaqSensor.bme680Status);
      Serial.println(output);
      for (;;)
        errLeds(); /* Halt in case of failure */
    }
    else
    {
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