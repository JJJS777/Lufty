#include <Arduino.h>
#include <AsyncMqttClient.h>
#include <WiFi.h>
#include <WiFiClient.h>

const char *ssid = "Duckn3t";
const char *password = "quack1QUACK4quack1";

#define MQTT_HOST IPAddress(192, 168, 141, 99)
#define MQTT_PORT 1883

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

/*Zuweisung weiterer Callback-Fkt'en. Die zugewiesen Fkt werden automatisch aufgerufen, sobald diese Benötigt werden*/
  mqttClient.onConnect(onMqttConnect);
  mqttClient.onDisconnect(onMqttDisconnect);
  mqttClient.onSubscribe(onMqttSubscribe);
  //mqttClient.onUnsubscribe(onMqttUnsubscribe);
  //mqttClient.onPublish(onMqttPublish);
  mqttClient.setServer(MQTT_HOST, MQTT_PORT);

    /*MQTT-Broker Authentifizierung: wenn der MQTT-Broker auth benötigt*/
  mqttClient.setCredentials("REPlACE_WITH_YOUR_USER", "REPLACE_WITH_YOUR_PASSWORD");

  /*mit WiFi verbindne durch das aufrufen der connectToWifi-Fkt*/
  connectToWifi();

/*Subcribe*/
void onMqttSubscribe(uint16_t packetId, uint8_t qos) {
  
  uint16_t packetId = mqttClient.subscribe("esp/sensorBoard/diffusor/on", 2);
  Serial.print("Subscribing esp/sensorBoard/diffusor/on at QoS 2, packetId: ");
  Serial.println(packetId);
}

void onMqttSubscribe(uint16_t packetId, uint8_t qos) {
  
  uint16_t packetId = mqttClient.subscribe("esp/sensorBoard/diffusor/off", 2);
  Serial.print("Subscribing esp/sensorBoard/diffusor/off at QoS 2, packetId: ");
  Serial.println(packetId);
}
/*Unsubcribe*/
void onMqttUnsubscribe(uint16_t packetId) {
  Serial.println("Unsubscribe acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
}

Adafruit_MQTT_Subscribe on = Adafruit_MQTT_Subscribe(&mqtt, MQTT_NAME "esp/sensorBoard/diffusor/on");
Adafruit_MQTT_Subscribe off = Adafruit_MQTT_Subscribe(&mqtt, MQTT_NAME "esp/sensorBoard/diffusor/off");
 
static const int diffusorPin = 32;
void setup()
{
  Serial.begin(9600);
  pinMode(diffusorPin, OUTPUT); // Set diffusorPin as digital output pin
}
void diffusor_on()
{
  digitalWrite(diffusorPin, HIGH);
  delay(60);
  digitalWrite(diffusorPin, LOW);
}
void diffusor_off()
{
  digitalWrite(diffusorPin, HIGH);
  delay(60);
  digitalWrite(diffusorPin, LOW);
  delay(100);
  digitalWrite(diffusorPin, HIGH);
  delay(60);
  digitalWrite(diffusorPin, LOW);
}
void loop()
{
  diffusor_on();
  delay(6000);
  diffusor_off();
  delay(6000);
}