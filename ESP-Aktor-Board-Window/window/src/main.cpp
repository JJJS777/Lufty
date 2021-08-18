#include <Arduino.h>
#include <Servo.h>
#include <Wire.h>
#include <SPI.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <HTTPClient.h>
#include <AsyncMqttClient.h>
#include <iostream>
#include <string>

using namespace std;

#define MQTT_HOST IPAddress(192, 168, 141, 99)
#define MQTT_PORT 1883
#define MQTT_QoS 1
#define MQTT_PUB_DIFFUSOR_ON "esp/sensorBoard/diffusor/on"
#define MQTT_PUB_DIFFUSOR_OFF "esp/sensorBoard/diffusor/off"
#define MQTT_PUB_WINDOW_OPEN "esp/sensorBoard/window/open"
#define MQTT_PUB_WINDOW_CLOSE "esp/sensorBoard/window/close"

/*Test Topics*/
#define MQTT_PUB_TEMP "esp/bme680/temperature"
#define MQTT_PUB_HUM "esp/bme680/humidity"
#define MQTT_PUB_PRES "esp/bme680/pressure"
#define MQTT_PUB_GAS "esp/bme680/gas"

static const int servoPin = 16;
const char *ssid = "Duckn3t";
const char *password = "quack1QUACK4quack1";

//MQTT-Timer-Hilfsvariable die alle 10 sec die Anweisung an den Broker publiziert
unsigned long previousMills = 0;
const long interval = 10000;

Servo servo1;

// Erzeugen eine Instanz der Klasse AsyncMqttClient namens mqttClient um die MQTT clients zu verwalten
// Timer Instanzen verwalten das wieder verbinden mit MQTT Broke und Router
AsyncMqttClient mqttClient;
TimerHandle_t mqttReconnectTimer;
TimerHandle_t wifiReconnectTimer;

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

void onMqttMessage(char *topic, char *payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total)
{
    Serial.println("Publish received.");
    Serial.print("  topic: ");
    Serial.println(topic);
    Serial.print("  qos: ");
    Serial.println(properties.qos);
    Serial.print("  dup: ");
    Serial.println(properties.dup);
    Serial.print("  retain: ");
    Serial.println(properties.retain);
    Serial.print("  len: ");
    Serial.println(len);
    Serial.print("  index: ");
    Serial.println(index);
    Serial.print("  total: ");
    Serial.println(total);
    Serial.println(payload);
    for (size_t i = 0; i < len; ++i)
    {
        Serial.print(payload[i]);
    }
}

void window_open()
{
    for (int posDegrees = 0; posDegrees <= 180; posDegrees++)
    {
        servo1.write(posDegrees);
        Serial.println(posDegrees);
        delay(20);
    }
}
void window_close()
{
    for (int posDegrees = 180; posDegrees >= 0; posDegrees--)
    {
        servo1.write(posDegrees);
        Serial.println(posDegrees);
        delay(20);
    }
}

void setup()
{
    Serial.begin(9600); //ggf. anpassen
    servo1.attach(servoPin);

    /*The next two lines create timers that will allow both the MQTT broker and Wi-Fi connection to reconnect, in case the connection is lost.*/
    mqttReconnectTimer = xTimerCreate("mqttTimer", pdMS_TO_TICKS(2000), pdFALSE, (void *)0, reinterpret_cast<TimerCallbackFunction_t>(connectToMqtt));
    wifiReconnectTimer = xTimerCreate("wifiTimer", pdMS_TO_TICKS(2000), pdFALSE, (void *)0, reinterpret_cast<TimerCallbackFunction_t>(connectToWifi));

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
}

void loop()
{
    // window_open();
    // delay(20000);
    // window_close();
    // delay(20000);

    unsigned long currentMillis = millis();
    // Every X number of seconds (interval = 10 seconds)
    // it publishes a new MQTT message
    if (currentMillis - previousMills >= interval)
    {
        // Save the last time a new reading was published
        previousMills = currentMillis;

        // SUB an MQTT message on topic esp/bme680/temperature
        uint16_t packetIdSub1 = mqttClient.subscribe(MQTT_PUB_TEMP, MQTT_QoS);
        mqttClient.onMessage(onMqttMessage);
    }
}