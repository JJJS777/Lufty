#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <DHT.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <Arduino_JSON.h>
#include <HTTPClient.h>

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
unsigned long timerDelay = 10000;

String jsonBuffer;


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

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }

  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

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