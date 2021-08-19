#include <Arduino.h>
#include <Servo.h>
#include <Wire.h>
#include <SPI.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <HTTPClient.h>
#include <AsyncMqttClient.h>


#define MQTT_HOST IPAddress(192, 168, 141, 99)
#define MQTT_PORT 1883
#define MQTT_QoS 1
#define MQTT_SUB_WINDOW "esp/sensorBoard/window"

char mqttCommandMsg = 'c'; //ggf. zu lokaler variable machen
static const int servoPin = 16;
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
    for (int posDegrees = 0; posDegrees <= 180; posDegrees++)
    {
        servo1.write(posDegrees);
    }
    Serial.println("... Fenster öffnet sich ...");
    delay(1000);
}

void window_close()
{
    for (int posDegrees = 180; posDegrees >= 0; posDegrees--)
    {
        servo1.write(posDegrees);
    }
    Serial.println("... Fenster schließt sich ... ");
    delay(1000);
}

void onMqttMessage(char *topic, char *payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total)
{
    for (size_t i = 0; i < len; ++i)
    {
        mqttCommandMsg = *payload;
    }

    if(mqttCommandMsg == '1'){ window_open(); }
    if(mqttCommandMsg == '0'){ window_close(); }
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
}

void loop()
{
    unsigned long currentMillis = millis();
    // Every X number of seconds (interval = 10 seconds)
    if (currentMillis - previousMills >= interval)
    {
        // Save the last time a new reading was published
        previousMills = currentMillis;
        uint16_t packetIdSubWindow = mqttClient.subscribe(MQTT_SUB_WINDOW, MQTT_QoS);
        Serial.println(packetIdSubWindow);
        mqttClient.onMessage(onMqttMessage);
    }
}