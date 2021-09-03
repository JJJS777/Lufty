# Sensor-ESP

## Vorrausetzungen
### Software
#### Visual Studio Code mit PlatformIO oder Arduino IDE
Wir haben für unser Projekt [PlatformIO][1] für VSC verwendet. Als erstes müsst ihr dafür in VSC den Plugin für PlatformIO installieren. 
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

#### MQTT Libraries
Der MQTT-Broker muss auf dem Raspberry Pi oder in der Cloud implementiert werden. Hier zeigen wir, wie ihr einen MQTT-Client auf dem ESP32 implementiert. 
Damit der Sensor-ESP auf ein Topic publishen kann benötigen wir folgende Bibliotheken:

1. [Async MQTT Client Library][3]
   1. Ladet euch den Code als ZIP-Datei von dem GIT Repo Lokal auf eueren Rechner
   2. Unzipt den Ordner und bennent diesen um in **async_mqtt_client**
   3. fügt den umbenannten Ordner per Drag-and-Drop in VSC unter /lib ein
   4. startet VSC neu
 
2. [Async TCP library.][4]
   1. Ladet euch den Code als ZIP-Datei von dem GIT Repo Lokal auf eueren Rechner
   2. Unzipt den Ordner und bennent diesen um in **AsyncTCP**
   3. fügt den umbenannten Ordner per Drag-and-Drop in VSC unter /lib ein
   4. startet VSC neu

Diese Bibliotheken müsst ihr auf allen ESP's installieren. 

#### BME680 Sensor Libraries
Damit ihr Werte von dem Sensor auslesen könnt, benötigt ihr zwei Bibliotheken

1. Adafruit_BME680 library
2. Adafruit_Sensor library

Sucht nach **adafruit bme680** und **Adafruit Unified Sensor** in PlatformIO unter _Libraries_ und fügt die Bibliotheken eurem Projekt hinzu.

#### BSEC-Arduino-library für BME680
In diesem Projekt arbeiten wir mit dem [Air Quality Index (AQI oder IAQ)][5]. Der AQI fasst die vom BME680 gemessenen Gase zu einem Index zusammen. Der Index wird uns später helfen die Luftqualität besser einschätzen zu können und die Luftqualität drinnen und draußen vergleiche zu können. 
Damit wir die gemessenen Werte im AQI zusammenfassen können, benötigen wir die BSEC-Bibliothek von [Bosch Sensortec Environment Cluster][6]. 
Um die Bibliothek einzubinden sucht einfach in PlatformIO unter _Libraries_ nach **BSEC** und fügt sie eurem Projekt hinzu.

#### Blynk Libraries
#### Arduino_JSON Library
	
### Hardware
+ ESP32 ([Grove System][2])
+ BME680 ([Grove System][2])

## Wie der Code funktioniert

[1]:https://docs.platformio.org/en/latest/what-is-platformio.html
[2]:https://wiki.seeedstudio.com/Grove_System/
[3]:https://github.com/marvinroger/async-mqtt-client
[4]:https://github.com/me-no-dev/AsyncTCP
[5]:https://en.wikipedia.org/wiki/Air_quality_index
[6]:https://www.bosch-sensortec.com/software-tools/software/bsec/
