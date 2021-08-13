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
#include <AsyncMqttClient.h>
#include "bsec.h"

// extern "C" {
//   #include "freertos/FreeRTOS.h"
//   #include "freertos/timers.h"
// }


#define BLYNK_PRINT Serialear
#define MQTT_HOST IPAddress(192, 168, 141, 99)
#define MQTT_PORT 1883
#define MQTT_PUB_DIFFUSOR_ON "esp/sensorBoard/diffusor/on"
#define MQTT_PUB_DIFFUSOR_OFF "esp/sensorBoard/diffusor/off"
#define MQTT_PUB_WINDOW_OPEN "esp/sensorBoard/window/open"
#define MQTT_PUB_WINDOW_CLOSE "esp/sensorBoard/window/close"

/*Test Topics*/
#define MQTT_PUB_TEMP "esp/bme680/temperature"
#define MQTT_PUB_HUM  "esp/bme680/humidity"
#define MQTT_PUB_PRES "esp/bme680/pressure"
#define MQTT_PUB_GAS  "esp/bme680/gas"


// Helper functions declarations
void checkIaqSensorStatus(void);
void errLeds(void);

// Erzeugen eine Instanz der Klasse Bsec
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
float rawTemperature, temperature, rawHumidity, humidity, pressure, iaqData, iaqAccuracy, staticIaq, gasResistance, co2Equivalent, breathVocEquivalent;
int ledPin = 14;

BlynkTimer timer;

/*OpenWeatherMap.org*/
// Your Domain name with URL path or IP address with path
String openWeatherMapApiKey = "a07094aa292e8325eaf597b2f9ce6e44";

// Replace with your country code and city
String city = "Cologne";
String countryCode = "DE";
String unitsApi = "metric";

// THE DEFAULT TIMER IS SET TO 10 SECONDS FOR TESTING PURPOSES
// For a final application, check the API call limits per hour/minute to avoid getting blocked/banned
unsigned long lastTime = 0;
unsigned long timerDelay = 60000;

//MQTT-Timer-Hilfsvariable die alle 10 sec die Anweisung an den Broker publiziert
unsigned long previousMills = 0;
const long interval = 10000;

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

void decodingJSON(){
  if ((millis() - lastTime) > timerDelay)
  {
    // Check WiFi connection status
    if (WiFi.status() == WL_CONNECTED)
    {
      String serverPath = "http://api.openweathermap.org/data/2.5/weather?q=" + city + "," + countryCode + "&units=" + unitsApi + "&APPID=" + openWeatherMapApiKey;

      Serial.println("_____\n");
      Serial.println("API-Call\n");

      /* Sendet einen HTTP GET request uns speichert die Res. in der Variable jsonBuffer. The httpGETRequest() function makes a 
      request to OpenWeatherMap and it retrieves a string with a JSON object that contains all the information about the weather for your city.*/
      jsonBuffer = httpGETRequest(serverPath.c_str());
      //Serial.println(jsonBuffer);
      
      JSONVar myObject = JSON.parse(jsonBuffer);

      // JSON.typeof(jsonVar) can be used to get the type of the var
      if (JSON.typeof(myObject) == "undefined")
      {
        Serial.println("Parsing input failed!");
        return;
      }

      double apiTemp = myObject["main"]["temp"];

      Serial.println("\n\nDaten aus Open-WeatherMap:");
      Serial.print("Temperatur aus API: ");
      Serial.print(apiTemp);
      Serial.println("°C");
      Serial.println("_____\n\n");
    }
    else
    {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }
}

/* WiFi Connection: Callback-Fkt, wird asynch ausgeführt */
void connectToWifi(){
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
}

/*Callback-Fkt, wird asynch ausgeführt*/
void connectToMqtt(){
  Serial.println("Connecting to MQTT Broker ...");
  mqttClient.connect();
}

/*WiFiEventHandler Verwaltet die WiFi-Events: Callback-Fkt, wird asynch ausgeführt*/
void WiFiEventHandler( WiFiEvent_t event ){
  Serial.printf("[WiFi-event] event: %d\n", event);
  switch( event ){
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
void onMqttConnect( bool sessionPresent ){
  Serial.println("Connected to MQTT.");
  Serial.print("Session present: ");
  Serial.println(sessionPresent);
}

/*Wenn ESP32 die verbindung zum MQTT-Broker verliert wird onMqttDisconnect aufgerufen: Callback-Fkt, wird asynch ausgeführt*/
void onMqttDisconnect( AsyncMqttClientDisconnectReason reason ){
  Serial.println("Disconnected from MQTT.");
  if (WiFi.isConnected()) { 
    xTimerStart(mqttReconnectTimer, 0);
  }
}

/*onMqttPublish wird aufgerufen, wenn eine MSG auf einem MQTT-Topic publiziert wird: Callback-Fkt, wird asynch ausgeführt*/
void onMqttPublish(uint16_t packetId){
  Serial.println("Publish acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
}

void setup(){
  // put your setup code here, to run once:
  Serial.begin(9600);

  /*The next two lines create timers that will allow both the MQTT broker and Wi-Fi connection to reconnect, in case the connection is lost.*/
  mqttReconnectTimer = xTimerCreate("mqttTimer", pdMS_TO_TICKS(2000), pdFALSE, (void*)0, reinterpret_cast<TimerCallbackFunction_t>(connectToMqtt));
  wifiReconnectTimer = xTimerCreate("wifiTimer", pdMS_TO_TICKS(2000), pdFALSE, (void*)0, reinterpret_cast<TimerCallbackFunction_t>(connectToWifi));

  /*Zuweisung einer Callback-Fkt: wenn der ESP sich mit dem WiFi verbindet wird die WiFiEventHandler-Fkt aufgerufen*/
  WiFi.onEvent(WiFiEventHandler);

  /*Zuweisung weiterer Callback-Fkt'en. Die zugewiesen Fkt werden automatisch aufgerufen, sobald diese Benötigt werden*/
  mqttClient.onConnect(onMqttConnect);
  mqttClient.onDisconnect(onMqttDisconnect);
  //mqttClient.onSubscribe(onMqttSubscribe);
  //mqttClient.onUnsubscribe(onMqttUnsubscribe);
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

void loop(){
  // put your main code here, to run repeatedly:
  /*Blynk*/
  Blynk.run();
  timer.run();

  //getBME680data();
  decodingJSON();

  unsigned long currentMillis = millis();
  // Every X number of seconds (interval = 10 seconds) 
  // it publishes a new MQTT message
  if (currentMillis - previousMills >= interval) {
    // Save the last time a new reading was published
    previousMills = currentMillis;
    
    getBME680data();
    
    // Publish an MQTT message on topic esp/bme680/temperature
    uint16_t packetIdPub1 = mqttClient.publish(MQTT_PUB_TEMP, 1, true, String(temperature).c_str());
    Serial.printf("Publishing on topic %s at QoS 1, packetId: %i", MQTT_PUB_TEMP, packetIdPub1);
    Serial.printf("Message: %.2f \n", temperature);

    // Publish an MQTT message on topic esp/bme680/humidity
    uint16_t packetIdPub2 = mqttClient.publish(MQTT_PUB_HUM, 1, true, String(humidity).c_str());
    Serial.printf("Publishing on topic %s at QoS 1, packetId %i: ", MQTT_PUB_HUM, packetIdPub2);
    Serial.printf("Message: %.2f \n", humidity);

    // Publish an MQTT message on topic esp/bme680/pressure
    uint16_t packetIdPub3 = mqttClient.publish(MQTT_PUB_PRES, 1, true, String(pressure).c_str());
    Serial.printf("Publishing on topic %s at QoS 1, packetId %i: ", MQTT_PUB_PRES, packetIdPub3);
    Serial.printf("Message: %.2f \n", pressure);

    // Publish an MQTT message on topic esp/bme680/gas
    uint16_t packetIdPub4 = mqttClient.publish(MQTT_PUB_GAS, 1, true, String(gasResistance).c_str());
    Serial.printf("Publishing on topic %s at QoS 1, packetId %i: ", MQTT_PUB_GAS, packetIdPub4);
    Serial.printf("Message: %.2f \n", gasResistance);
  }
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