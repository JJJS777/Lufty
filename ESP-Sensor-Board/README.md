# Sensor-ESP

Inhalt
--------
1. Vorrausetzungen
   1. Software
      1. PlatformIO für VSC
      2. Libraries
      3. API
      4. Blynk Mobile App
   3. Hardware
2. Wie der Code funktioniert


Vorrausetzungen
----------------
### Software
#### 1. Visual Studio Code mit PlatformIO oder Arduino IDE
Wir haben für unser Projekt den [PlatformIO Plug-In][1] für VSC verwendet. Als erstes müsst ihr dafür in VSC PlatformIO installieren. 
Danach könnt ihr ein Projekt in PlatformIO anlegen. Euer Projekt-Setup wird in der platformio.ini-Datei gespeichert. Hier ist als Beispiel der Inhalt unserer Datei:

```
[env:nodemcu-32s]
platform = espressif32
board = nodemcu-32s
framework = arduino
lib_deps = 
	adafruit/Adafruit Unified Sensor@^1.1.4
	adafruit/Adafruit BME680 Library@^2.0.1
	boschsensortec/BSEC Software Library@^1.6.1480
	blynkkk/Blynk@^1.0.1
	bblanchon/ArduinoJson@^6.18.3
	arduino-libraries/Arduino_JSON@^0.1.0
```

Als nächstes müsst Ihr die benötigten Bibliotheken installieren....

#### 2. Libraries
##### MQTT Libraries
Der MQTT-Broker muss auf dem Raspberry Pi oder in der Cloud implementiert werden. Hier zeigen wir, wie ihr einen MQTT-Client auf dem ESP32 implementiert. 
Damit der Sensor-ESP auf ein Topic publishen kann benötigen wir folgende Bibliotheken:

1. [Async MQTT Client Library][3]
   1. Ladet euch den Code als ZIP-Datei von dem GIT Repo Lokal auf euern Rechner
   2. Unzipt den Ordner und benennt diesen um in **async_mqtt_client**
   3. fügt den umbenannten Ordner per Drag-and-Drop in VSC unter /lib ein
   4. startet VSC neu
 
2. [Async TCP library.][4]
   1. Ladet euch den Code als ZIP-Datei von dem GIT Repo Lokal auf eueren Rechner
   2. Unzipt den Ordner und benennt diesen um in **AsyncTCP**
   3. fügt den umbenannten Ordner per Drag-and-Drop in VSC unter /lib ein
   4. startet VSC neu

Diese Bibliotheken müsst ihr auf allen ESP's installieren. 

##### BME680 Sensor Libraries
Damit ihr Werte von dem Sensor auslesen könnt, benötigt ihr zwei Bibliotheken

1. Adafruit_BME680 library
2. Adafruit_Sensor library

Sucht nach **adafruit bme680** und **Adafruit Unified Sensor** in PlatformIO unter _Libraries_ und fügt die Bibliotheken eurem Projekt hinzu.

##### BSEC-Arduino-library für BME680
In diesem Projekt arbeiten wir mit dem [Air Quality Index (AQI oder IAQ)][5]. Der AQI fasst die vom BME680 gemessenen Gase zu einem Index zusammen. Der Index wird uns später helfen die Luftqualität besser einschätzen zu können und die Luftqualität drinnen und draußen vergleiche zu können. 
Damit wir die gemessenen Werte im AQI zusammenfassen können, benötigen wir die BSEC-Bibliothek von [Bosch Sensortec Environment Cluster][6]. 
Um die Bibliothek einzubinden, sucht einfach in PlatformIO unter _Libraries_ nach **BSEC** und fügt sie eurem Projekt hinzu.

##### Blynk Libraries
Wir möchten uns später die Messwerte auf einem Smartphone anzeigen lassen. Dies realisieren wir mit der [Blynk-App][7]. Um diese nutzen zu können müssen wir die dazugehörige Bibliothek in PlatformIO einbinden. Dabei geht ihr wie bereits weiter oben beschrieben vor, nur sucht ihr jetzt nach **Blynk**.

Mit der Blynk-App ist es auch möglich die Komponenten anzusteuern. Denkbar ist z.B. das An und Aus schalten des Diffusors oder das Öffnen und Schließen des Fensters mithilfe der App. Dies haben wir in diesem Projekt aber nicht umgesetzt.  

##### Arduino_JSON Library
In dem Projekt werden wir über zwei unterschiedliche API's Wetter und Luftqualitätsdaten abfragen. Auf Basis der API-Daten, welche die Bedingungen außerhalb des Zimmers repräsentieren, und der Sensordaten werden wir später unsere Anwendungslogik aufbauen.

Damit wir mit den API-Daten arbeiten können benötigen wir die **Arduino_JSON-Bibliotheken**. Diese bindet ihr genau wie die anderen Bibliotheken ein.

#### 3. API's
In dem Projekt arbeiten wir mit zwei API's, die uns Informationen über das Wetter und die Luftqualität in Form des bereits erwähnt AQI liefern.
Wir haben uns für die [Open-Weather-Map-API][8] und die [Weather-Bit-API][9] entschieden, aber ihr könnt eine API eurer Wahl verwenden, dass vorgehen ist analog.
Open-Weather-Map-API liefert uns aktuelle Daten über Temperatur, Luftfeuchtigkeit und Wind, aber ihr könnt noch einige Parameter mehr auslesen. Weather-Bit liefert uns den aktuellen AQI.

Als erstes müsst ihr euch bei beiden Anbietern einen Account anlegen, dieser ist für unseren Projektumfang kostenlos. Wenn ihr einen Account angelegt habt, könnt ihr euch einen API-Key generieren. Den Key hinterlegt ihr an entsprechender stelle in der main.cpp. Wichtig: Behaltet eure API-Key immer für euch und teilt sie mit keinem außerhalb von eurem Projekt Projekts 

#### 4.  Blynk Mobile App
Zum Visualisieren der Mess- und API-Daten nutzen wir die Mobile-App von [Blynk][10]. Alternativ könnt ihr auch den [Web-Client][11] verwenden. Wir zeigen euch exemplarisch an der Mobile-App wie ihr diese mit eurem Sensor-ESP verbinden könnt. Blynk erfüllt in unserem Projekt zwei Ziele, zum einen wollen wir die Daten Visualisiert haben, um zu überprüfen ob die Anwendungslogik richtig funktioniert und zum anderen soll es perfektivisch möglich sein, die Aktoren (Window-ESP, Diffusor-ESP) mit Blynk anzusteuern.
Als erstes müsst ihr euch bei Blynk einen Account anlegen. Für unsere Zwecke reicht der kostenlose Plan. In eurem Account könnt ihr ein neues Projekt anlegen, dafür Klickt ihr einfach auf das „+“-Symbol in der Übersicht. Gebt eurem Projekt einen Namen, legt euer Gerät fest, wenn Ihr eine ESP32 habt wie wir, wählt ESP23 DEV Board aus. Im Letzten Schritt legt ihr die Art der Verbindung fest, wir haben uns für WiFi entschieden. Sobald ihr euer Projekt angelegt habt, bekommt ihr eine Email mit dem Authentifizierungs-Token. Jedes Projekt hat einen eigenen Token. 
Mit Blynk könnt ihr eure Daten auf unterschiedlichste Weise visualisieren.
Dafür stehen euch in eurem Projekt in der Widget Box unterschiedlichen Displays zur Verfügung. Wir verwenden für unser Projekt den Gauge-Display. In den Settings zu dem Gauge-Display tragt ihr einen Namen oder den Wert, den ihr mit dem Display anzeigen wollt, ein (z.B. Luftfeuchtigkeit). Als nächstes tragt ihr euren Virtuellen Input-Pin ein. Der Pin muss später zu dem Pin passen, den ihr im Code für den Wert definiert habt. Wir verwenden für dieses Beispiel den Virtuellen Input-Pin V5. Neben dem Input-Pin legt ihr den Wertbereich fest, innerhalb derer sich die Messung bewegen soll. Für die Luftfeuchtigkeit tragt ihr z.B. 0 – 100 (%) ein. Unter „Label“ legt ihr die Maßeinheit für den Input-Pin fest, also am Beispiel von Luftfeuchtigkeit „%“. Im Letzten Schritt legt ihr fest, wie die Messwerte aktualisiert werden sollen. „Push“ bedeutet, dass der ESP die neuen Messungen an die App immer dann übermittelt, wenn neue vorliegen. Das ist unsere favorisierte Variante. Alternativ werden euch Sekunden, Minuten und Stunden-Werte zur Auswahl gegeben. Wählt ihr z.B. 1 min aus, dann fragt die App beim ESP als 60 sec nach, ob neue Werte Vorliegen. 



### Hardware
+ ESP32 ([Grove System][2])
+ BME680 ([Grove System][2])

Wie der Code funktioniert
-------------------------
in diesem Teil möchten wir kurz darauf eingehen wie der Code in der main.cpp funktioniert und welche variablen ihr ggf. noch anpassen müsst. 

### WiFi

```cpp
const char *ssid = "REPLACE_WITH_YOUR_SSID";
const char *password = "REPLACE_WITH_YOUR_PASSWORD";
```

### MQTT
```cpp
#define MQTT_HOST IPAddress(xxx, xxx, xxx, xxx)
#define MQTT_PORT 1883
#define MQTT_PUB_DIFFUSOR "esp/sensorBoard/diffusor"
#define MQTT_PUB_WINDOW "esp/sensorBoard/window"
```

### API
```cpp
String openWeatherMapApiKey = "REPLACE_WITH_YOUR_API_KEY";
String weatherbitAirQualityApiKey = "REPLACE_WITH_YOUR_API_KEY";

// Replace with your country code and city
String city = "Cologne";
String countryCode = "DE";
String unitsApi = "metric";
/*lat und lon können auch aus erstem API Req. rausgeholt und verwendet werden, anstelle sie hart zu coden*/
String latitude = "50";
String longitude = "6";
```
### Blynk
Hier tragt ihr euren Authentifizierungs-Token ein, den ihr zuvor per Email bekommen habt:
```cpp
39 const char auth[] = "REPLACE_WITH_YOUR_BLYNK_AUTH_TOKEN";
```

Wie im oberen Abschnitt bereits erwähnt, definiert ihr wie folgt die Pins, an die die Messdaten geschickt werden sollen. Die Methode Blynk.virtualWrite() nimmt als erstes Argument den Pin und als zweites die Variable, in der ihr den Messwert gespeichert habt. In Unserem Fall heißt die Variable humidity und der entsprechende Messwert soll auf Pin V5 angezeigt werden. 
```cpp
139 Blynk.virtualWrite(V5, humidity);
140 Blynk.virtualWrite(V6, temperature);
```



[1]:https://docs.platformio.org/en/latest/what-is-platformio.html
[2]:https://wiki.seeedstudio.com/Grove_System/
[3]:https://github.com/marvinroger/async-mqtt-client
[4]:https://github.com/me-no-dev/AsyncTCP
[5]:https://en.wikipedia.org/wiki/Air_quality_index
[6]:https://www.bosch-sensortec.com/software-tools/software/bsec/
[7]:https://blynk.io/en/getting-started
[8]:https://openweathermap.org/current
[9]:https://www.weatherbit.io/api/airquality-current
[10]:https://docs.blynk.io/en/downloads/blynk-apps-for-ios-and-android
[11]:https://docs.blynk.io/en/blynk.console/console-overview
