#include <Arduino.h>
#include <Servo.h>
#include <Wire.h>
#include <SPI.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <HTTPClient.h>
#include <AsyncMqttClient.h>


extern "C" {
	#include "freertos/FreeRTOS.h"
	#include "freertos/timers.h"
}

#define MQTT_HOST IPAddress(192, 168, 141, 99)
#define MQTT_PORT 1883
#define MQTT_QoS 1
#define MQTT_SUB_WINDOW "esp/sensorBoard/window"

char mqttCommandMsg = 'c'; //ggf. zu lokaler variable machen
static const int servoPin = 16;
int posDegrees = 0;
const char *ssid = "Duckn3t";
const char *password = "quack1QUACK4quack1";

//MQTT-Timer-Hilfsvariable die alle 10 sec die Anweisung an den Broker publiziert
unsigned long previousMills = 0;
const long interval = 60000;

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

/*Eventhandler für Subscriber*/
void onMqttSubscribe(uint16_t packetId, uint8_t qos) {
  Serial.println("Subscribe acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
  Serial.print("  qos: ");
  Serial.println(qos);
}

void onMqttUnsubscribe(uint16_t packetId) {
  Serial.println("Unsubscribe acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
}

/*onMqttPublish wird aufgerufen, wenn eine MSG auf einem MQTT-Topic publiziert wird: Callback-Fkt, wird asynch ausgeführt*/
void onMqttPublish(uint16_t packetId)
{
    Serial.println("Publish acknowledged.");
    Serial.print("  packetId: ");
    Serial.println(packetId);
}

void window_open()
{
    Serial.println(posDegrees);
    for (posDegrees = 0; posDegrees <= 180; posDegrees += 10 )
    {
        servo1.write(posDegrees);
    }
    Serial.println("... Fenster öffnet sich ...");
    Serial.println(posDegrees);
    // delay(1000);
}

void window_close()
{

    // if (posDegrees == 180) {
    //     Serial.println("Fenster ist bereits geschlossen!");
    // } else {
    //     for (int posDegrees = 180; posDegrees >= 0; posDegrees += 10) {
    //         servo1.write(posDegrees);
    //     }
        
    // }
    Serial.println("... Fenster schließt sich ... ");
    // delay(1000);
}


/*MQTT MSH auslesen und darauf regieren*/
void onMqttMessage(char *topic, char *payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total)
{
    for (size_t i = 0; i < len; ++i)
    {
        mqttCommandMsg = *payload;
    }

    if(mqttCommandMsg == '1'){ window_open(); }
    if(mqttCommandMsg == '0'){ window_close(); }

    unsigned long currentMillis = millis();
    Serial.println(currentMillis);
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
    mqttClient.onSubscribe(onMqttSubscribe);
    mqttClient.onUnsubscribe(onMqttUnsubscribe);
    mqttClient.setServer(MQTT_HOST, MQTT_PORT);

    /*mit WiFi verbindne durch das aufrufen der connectToWifi-Fkt*/
    connectToWifi();

    /*Bei Broker auf Topic Subscriben*/
    uint16_t packetIdSubWindow = mqttClient.subscribe(MQTT_SUB_WINDOW, MQTT_QoS);
    Serial.println(packetIdSubWindow);
    mqttClient.onMessage(onMqttMessage);
}

void loop()
{

    // // Every X number of seconds (interval = 10 seconds)
    // if (currentMillis - previousMills >= interval)
    // {
    //     // Save the last time a new reading was published
    //     previousMills = currentMillis;
    // }
}


// /*TEST START*/
// #define WIFI_SSID "Duckn3t"
// #define WIFI_PASSWORD "quack1QUACK4quack1"

// #define MQTT_HOST IPAddress(192, 168, 1, 10)
// #define MQTT_PORT 1883

// AsyncMqttClient mqttClient;
// TimerHandle_t mqttReconnectTimer;
// TimerHandle_t wifiReconnectTimer;

// void connectToWifi() {
//   Serial.println("Connecting to Wi-Fi...");
//   WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
// }

// void connectToMqtt() {
//   Serial.println("Connecting to MQTT...");
//   mqttClient.connect();
// }

// void WiFiEvent(WiFiEvent_t event) {
//     Serial.printf("[WiFi-event] event: %d\n", event);
//     switch(event) {
//     case SYSTEM_EVENT_STA_GOT_IP:
//         Serial.println("WiFi connected");
//         Serial.println("IP address: ");
//         Serial.println(WiFi.localIP());
//         connectToMqtt();
//         break;
//     case SYSTEM_EVENT_STA_DISCONNECTED:
//         Serial.println("WiFi lost connection");
//         xTimerStop(mqttReconnectTimer, 0); // ensure we don't reconnect to MQTT while reconnecting to Wi-Fi
//         xTimerStart(wifiReconnectTimer, 0);
//         break;
//     }
// }

// void onMqttConnect(bool sessionPresent) {
//   Serial.println("Connected to MQTT.");
//   Serial.print("Session present: ");
//   Serial.println(sessionPresent);
//   uint16_t packetIdSub = mqttClient.subscribe("test/lol", 2);
//   Serial.print("Subscribing at QoS 2, packetId: ");
//   Serial.println(packetIdSub);
//   mqttClient.publish("test/lol", 0, true, "test 1");
//   Serial.println("Publishing at QoS 0");
//   uint16_t packetIdPub1 = mqttClient.publish("test/lol", 1, true, "test 2");
//   Serial.print("Publishing at QoS 1, packetId: ");
//   Serial.println(packetIdPub1);
//   uint16_t packetIdPub2 = mqttClient.publish("test/lol", 2, true, "test 3");
//   Serial.print("Publishing at QoS 2, packetId: ");
//   Serial.println(packetIdPub2);
// }

// void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
//   Serial.println("Disconnected from MQTT.");

//   if (WiFi.isConnected()) {
//     xTimerStart(mqttReconnectTimer, 0);
//   }
// }

// void onMqttSubscribe(uint16_t packetId, uint8_t qos) {
//   Serial.println("Subscribe acknowledged.");
//   Serial.print("  packetId: ");
//   Serial.println(packetId);
//   Serial.print("  qos: ");
//   Serial.println(qos);
// }

// void onMqttUnsubscribe(uint16_t packetId) {
//   Serial.println("Unsubscribe acknowledged.");
//   Serial.print("  packetId: ");
//   Serial.println(packetId);
// }

// void onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {
//   Serial.println("Publish received.");
//   Serial.print("  topic: ");
//   Serial.println(topic);
//   Serial.print("  qos: ");
//   Serial.println(properties.qos);
//   Serial.print("  dup: ");
//   Serial.println(properties.dup);
//   Serial.print("  retain: ");
//   Serial.println(properties.retain);
//   Serial.print("  len: ");
//   Serial.println(len);
//   Serial.print("  index: ");
//   Serial.println(index);
//   Serial.print("  total: ");
//   Serial.println(total);
// }

// void onMqttPublish(uint16_t packetId) {
//   Serial.println("Publish acknowledged.");
//   Serial.print("  packetId: ");
//   Serial.println(packetId);
// }

// void setup() {
//   Serial.begin(9600);
//   Serial.println();
//   Serial.println();

//   mqttReconnectTimer = xTimerCreate("mqttTimer", pdMS_TO_TICKS(2000), pdFALSE, (void*)0, reinterpret_cast<TimerCallbackFunction_t>(connectToMqtt));
//   wifiReconnectTimer = xTimerCreate("wifiTimer", pdMS_TO_TICKS(2000), pdFALSE, (void*)0, reinterpret_cast<TimerCallbackFunction_t>(connectToWifi));

//   WiFi.onEvent(WiFiEvent);

//   mqttClient.onConnect(onMqttConnect);
//   mqttClient.onDisconnect(onMqttDisconnect);
//   mqttClient.onSubscribe(onMqttSubscribe);
//   mqttClient.onUnsubscribe(onMqttUnsubscribe);
//   mqttClient.onMessage(onMqttMessage);
//   mqttClient.onPublish(onMqttPublish);
//   mqttClient.setServer(MQTT_HOST, MQTT_PORT);

//   connectToWifi();
// }

// void loop() {
// }
// /*TEST ENDE*/