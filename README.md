# Lufty IoT-Projekt2021
![alt text][logo]


## Inhalt
1. [Projektübersicht](#1-projektübersicht)
   1. Szenario - ToDo
   3. Projektziel - ToDo
2. [Benötigte Teile](#benoetigte-teile)
3. [Vorrausetzungen]
   1. Sensor-ESP
   2. Diffusor-ESP
   3. Window-ESP
   4. Raspberry Pi 4
4. [Anwendungslogik und Funktionsweise]
   1. Architekturmodell
   2. Aktivitätsdiagram
5. [Installation]
   1. Allgemein
   2. Sensor-ESP
   3. Diffusor-ESP
   4. Window-ESP
   5. MQTT-Setup
      1. MQTT auf Client
      2. Mosquitto-Broker auf Raspberry Pi einrichten
6. [Ausblick / Erweiterungsmöglichkeiten]


## 1. Projektübersicht
Lufty ermöglicht, dass in jedem Raum eine bessere Luftqualität vorhanden ist duch ein automatisches Lüftungssystem.

Das Lufty-Projekt besteht aus drei ESP32, einem Raspberry Pi 4, einem BME680-Sensor zum Messen der Luftqualität in Innenräumen, einen Servo-Motor zum öffnen eines Fensters und einen Diffusor zum um die Luftfeuchtigkeit zu erhöhen. 

Hier möchten wir dir zeigen, wie du das Projekt bei dir zu Hause selbst umsetzen kannst.
Jeder ESP hat eine eigene README, in der alle Details für das jeweilige ESP-Board erklärt wird. In den Ordner findest du auch den Code für die Entsprechenden ESP’s.


## 2. Benötigte Teile
+ 3x [ESP32][1] Mikrocontroller oder vergleichbar
+ 1x [Raspberry Pi][2] + Micro SD Karte
+ 1x [BME680-Grove-Sensor][3]
+ 1x Servo-Motor für Fenster Prototyp
+ 1x [Diffusor][4]
+ 2x Project Board (Breadboard)
+ 1x [Optocoupler][5]
+ Wire (10x M/M und 10x M/F Jumpwire)


## 3. Vorrausetzungen
### Allgemein
#### Visual Studio Code mit PlatformIO oder Arduino IDE

Wir haben für unser Projekt das PlatformIO Plug-In für VSC verwendet. Als erstes müsst ihr dafür in VSC PlatformIO installieren. Danach könnt ihr ein Projekt in PlatformIO anlegen. Euer Projekt-Setup wird in der platformio.ini-Datei gespeichert. Für jedes Projekt wird eine eigene platformio.ini-Datei mit den jeweiligen Abhängigkeiten angelegt. Hier ist als Beispiel der Inhalt unserer Datei:

```
[env:nodemcu-32s]
platform = espressif32
board = nodemcu-32s
framework = arduino
```

Am besten legt ihr für jeden ESP ein eigenes Projekt in PlatformIO an, da ihr für die unterschiedlichen ESPs nicht immer alle bzw. die gleichen Abhängikeiten benötigt.


### ESP-Spezifisch
In dem Projekt haben wir für jeden ESP einen eignenen Ordner mit dem Source-Code, den dazugehörigen Dateien im /lib Folder und den jeweiligen platformio.ini-Datei angelegt. Die ESP-spezifischen Vorrausetzungen und Konfigurationen findet ihr in den README-Dateien in den entsprechenden Ordnern.

#### Sensor-ESP
[hier geht es zur README des Sensor-Board](./ESP-Sensor-Board/README.md)

#### Diffusor ESP
[hier geht es zur README des Diffusor-Board](./ESP-Aktor-Board-Diffusor/README.md)

#### Window ESP
[hier geht es zur README des Window-Board](./ESP-Aktor-Board-Window/README.md)



## 4. Anwendungslogik und Funktionsweise
### Architekturmodell
![alt text][Architekturmodell]


### Aktivitätsdiagram
![alt text][Aktivitätsmodell]


## 6. Installation
### 1. Allgemein
Als erstes Clont ihr euch das Repository oder ladet euch das Projekt als zip-Datei lokal auf euren Rechner. Danach navigiert ihr in die Ordner der einzelnen Komponenten, führt entsprechende Änderungen am Code durch und ladet den Code auf den dafür vorgesehenen ESP. Welche änderungen ihr wo vornehmen müsst, könnt ihr in den READMEs der einzelnen ESPs entnehmen.

### 2. Sensor-ESP
[hier geht es zur README des Sensor-Board](./ESP-Sensor-Board/README.md)

### 3. Diffusor ESP
[hier geht es zur README des Diffusor-Board](./ESP-Aktor-Board-Diffusor/README.md)

### 4. Window ESP
[hier geht es zur README des Window-Board](./ESP-Aktor-Board-Window/README.md)

### 5. MQTT
#### Mosquitto-Broker auf Raspberry Pi einrichten
Für das Projekt nutzen wir einen [Mosquitto-Broker][6], der auf einem Raspberry Pi 4 installiert ist.
Ihr könnt für euer Projekt einen beliebigen Broker verwenden. Zur besseren Skalierbarkeit des Projektes ist auch ein Cloud-Broker denkbar. 

Das Installieren und Einrichten des MQTT-Brokers geht recht schnell: 
```shell
pi@raspberry:~ $ sudo apt update
pi@raspberry:~ $ sudo apt install -y mosquitto mosquitto-clients
```
mit **Y** und **Enter** bestätigst du die Installation. Um den Broker nach dem Neustart automatisch zu starten musst du folgendes eingeben
```shell
pi@raspberry:~ $ sudo systemctl enable mosquitto.service
```

Durch das eingeben von ```pi@raspberry:~ $ hostname -I ``` findest du die IP-Adresse deines Raspberry Pi herraus. Diese Benötigst du später bei der Konfiguration des Clients.


#### MQTT auf Client
Wie du die MQTT Clients konfigurierst zeigen wir dir Beispielhaft am ESP-Sensor. 

1. [Einzubinden der benötigten Bibliotheken](./ESP-Sensor-Board/README.md#mqtt-libraries)
2. [Anpassen der Variablen und Arbeiten mit MQTT](./ESP-Sensor-Board/README.md#mqtt) 



## 7. Ausblick / Erweiterungsmöglichkeiten

### Cloud MQTT Broker:
Um das System portabler und effitienter zu machen ist es sinnvol den MQTT Broker zu deployen und auf einem Cloud laufen zu lassen. damit verbraucht man weniger Material und die Installation und Portabilität des Systems wird einfacher und schneller.
### Das System in mehreren Raüme aufsetzen:
Die Grundidee von Lufty war es, kleine Zimmern (wie zb. Schlafzimmer oder Wohnzimmer) gut zu belüften. Allerdings gibt es sehr viele größere Möglichgeiten, wo das Konzept eine wichtige Role spielen kann. Größere Zimmer bzw. Gebäude könnten auch von Lufty profitieren. Dafür aber müssen Erweiterungen zum System hinzugefügt werden und zwar mehr Aktoren und Sensoren damit Lufty alle Fenster steurn kann, alle Zimmer befeuchtet und die richtige Messwerte abliest für eine genauere Präzision.
### Optimale Lüftung auch bei extremen Temperatur:
Unsere Lösung für die Lüftung bei extremen Temperaturen (unter 5°c oder über 30°c) war es das Fenster kurzeitig (5min) zu öffnen und für eine Weile (2stunden) zu blockieren, damit die Temperatur des Zimmers angenehm bleibt. Das ist natürlich keine optimale Lösung, da es manchmal zu Fälle kommen kann, wo die Luft Qualität im Zimmer schlecht ist und das Fenster blockiert ist.Eine bessere Lösung wäre es, noch ein paar Aktoren im System zu haben, und zwar eine Klimaanlage und ein Heizungsgerät. Lufty kann schon die inneren und äußeren Temperaturwerte herausfinden und mit den neuen Aktoren wird es einfacher gleichzeitig die Luftqualität und die Temperatur angenehm zu halten. Dafür ist es natürlich auch erforderlich ein paar Zeilen in unser Programmlogik zu ändern.


[1]: https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/index.html
[2]: https://www.raspberrypi.org/products/raspberry-pi-4-model-b/
[3]: https://wiki.seeedstudio.com/Grove-Temperature_Humidity_Pressure_Gas_Sensor_BME680/
[4]: https://www.conrad.de/de/p/boneco-u50-luftbefeuchter-schwarz-1-st-2316569.html
[5]: https://www.conrad.de/de/p/isocom-components-optokoppler-phototransistor-sfh615a-4x-dip-4-transistor-dc-183249.html 
[6]: https://mosquitto.org/
[Architekturmodell]:https://github.com/JJJS777/Lufty/blob/main/Artefakte/Architektur-Architekturdiagramm.png
[Aktivitätsmodell]:https://github.com/JJJS777/Lufty/blob/main/Artefakte/Architektur-Aktivit%C3%A4tsdiagramm.png
[logo]:https://github.com/JJJS777/Lufty/blob/main/Artefakte/341e316e-742d-4b23-a379-6eae5d3b70f6.jpeg
