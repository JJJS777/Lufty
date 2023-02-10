# Lufty IoT-Projekt2021
![alt text][logo]


## Inhalt
1. [Projektübersicht](#1-projektübersicht)
2. [Benötigte Teile](#2-benötigte-teile)
3. [Vorrausetzungen](#3-vorrausetzungen)
   1. Sensor-ESP
   2. Diffusor-ESP
   3. Window-ESP
   4. Raspberry Pi 4
4. [Funktionsweise](#4-funktionsweise)
   1. Architekturmodell
   2. Aktivitätsdiagram
5. [Installation](#5-installation)
   1. Allgemein
   2. Sensor-ESP
   3. Diffusor-ESP
   4. Window-ESP
   5. MQTT-Setup
      1. MQTT auf Client
      2. Mosquitto-Broker auf Raspberry Pi einrichten
6. [Ausblick / Erweiterungsmöglichkeiten](#6-ausblick--erweiterungsmöglichkeiten)


## 1. Projektübersicht
Lufty ist ein Projekt für das Fach IoT im Rahmen von Web Development an der Technische Hochschule Köln im Sommer Semester 2021. Das Thema für dieses Semester war **Smart Environments**. Unter dem Einfluss der Corona Pandemie entstand bei uns die Idee, mit Sensoren und Aktoren für eine perfekte Luftqualität zu sorgen. Die Studierenden konnten aufgrund der Pandemie nicht mehr in die Bibliothek oder zu anderen Arbeitsplätzen in der Hochschule. Um auch bei einer/einem Studierenden im (WG-)Zimmer gute Arbeitsbedingungen herzustellen haben wir Lufty entwickelt. Das (WG-)Zimmer ist auch Ausgangspunkt unserer Überlegung, jedoch lässt sich das Projekt durch kleine Ergänzungen und Änderungen auch auf mehrere Räume oder ganze Gebaute skalieren. Durch das Erreichen einer optimalen Luftqualität in Arbeits-, Wohn-, Gemeinschafts- und Schlafzimmern können Ansteckungen verringert, das Wohlbefinden erhöht und die Leistungsfähigkeit gesteigert werden.

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



## 4. Funktionsweise
### Architekturmodell
![alt text][Architekturmodell]


### Aktivitätsdiagram
![alt text][Aktivitätsmodell]


## 5. Installation
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



## 6. Ausblick / Erweiterungsmöglichkeiten

### MQTT Cloud Broker:
Um Geld und Aufwand an Teilen und dem Setup zu sparen, kann man für den MQTT Broker auch auf eine Cloud-Lösung zurückgreifen, dadurch wird das Projekt auch portabler. Diese Änderung kann auch Sinnvoll sein, wenn man das Projekt Hochskalieren möchte und z.B. in unterschiedlichen Netzen betreibt. 


### Mehreren Räumen:
Lufty entstand während der Corona Pandemie aus der Vorstellung heraus, dass die Studierenden unter den mit der Pandemie verbunden Umständen jetzt ihre gesamte Arbeit im (WG )-Arbeitszimmer verrichten müssen. Um an diesem, i.d.R. kleinen Ort, ein gutes Arbeits- und Wohnklima zu schaffen wurde Lufty entwickelt.  
Allerdings gibt es sehr viele mehr Möglichgeiten, wo das Konzept eine wichtige Rolle spielen kann. Größere Zimmer bzw. Gebäude könnten auch von Lufty profitieren. Dafür muss das System jedoch erweitert werden. Mehr Aktoren und Sensoren sind nötig, damit Lufty in allen Räumen die Werte messen kann und auf die jeweilige Situation in dem entsprechenden Raum regieren kann. Möchte man Lufty auf mehre Räume Skalieren ist eine weitere Änderung nötig. Derzeit läuft die Anwendungslogik auf dem Sensor-ESP, was das System fehleranfällig macht, Semitisch koppelt und für unnötige Redundanz Sorgt. Um dies zu verbessern sollte die Anwendungslogik zentraler, z.B. in der Cloud oder auf dem Raspberry Pi, laufen.


### Optimale Lüftung auch bei extremen Temperatur:
Unsere Lösung für die Lüftung bei extremen Temperaturen (unter 5°c oder über 30°c) war es das Fenster kurzeitig (5min) zu öffnen und für eine Dauer von zwei Stunden zu blockieren, damit die Temperatur im Zimmer angenehm bleibt. Das ist keine optimale Lösung, da es zu Fällen kommen kann, wo die Luft Qualität im Zimmer schlecht ist und das Fenster blockiert. Eine bessere Lösung wäre es, noch die Heizung(en) und ggf. Klimaanlage(n) als weitere Aktoren in das System zu integrieren. Lufty misst bereits Innen- und Außentemperatur. Mit zusätzlichen Aktoren in Form von Heizung und Klimaanlage können nun auch die gemessenen Temperaturwerte adäquat in das System integriert werden. Dies bedeutet natürlich, das die Anwendungslogik überarbeitet werden muss. Mit dieser Erweiterung kann Lufty dafür sorgen, dass die Raumtemperatur immer auf einem angenehmen Nivau bleibt. 


### Mehr Praxis nähe ohne weitere Investitionen
Fenster öffnen verfolgt hier nur einen theoretischen Ansatz. Um Das Projekt praxisnaher bei euch umzusetzen könnt ihr einfach auf dem Sensor-ESP in der Methode closeWindow() und openWindow()  eine Nachricht an die Blynk App schicken. In Der Blynk App könnt ihr dann aus der Widget Box den LCD, Email oder eine Notification anlegen, die euch Anzeigt bzw. eine Aufforderung schickt, das Fenster zu öffnen oder zu schließen. Das öffnen/schließen müsst ihr in dem Fall dann selbst übernehmen.  



[1]: https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/index.html
[2]: https://www.raspberrypi.org/products/raspberry-pi-4-model-b/
[3]: https://wiki.seeedstudio.com/Grove-Temperature_Humidity_Pressure_Gas_Sensor_BME680/
[4]: https://www.conrad.de/de/p/boneco-u50-luftbefeuchter-schwarz-1-st-2316569.html
[5]: https://www.conrad.de/de/p/isocom-components-optokoppler-phototransistor-sfh615a-4x-dip-4-transistor-dc-183249.html 
[6]: https://mosquitto.org/
[Architekturmodell]:https://github.com/JJJS777/Lufty/blob/main/Artefakte/Architektur-Architekturdiagramm.png
[Aktivitätsmodell]:https://github.com/JJJS777/Lufty/blob/main/Artefakte/Architektur-Aktivitätsdiagramm.drawio.png
[logo]:https://github.com/JJJS777/Lufty/blob/main/Artefakte/341e316e-742d-4b23-a379-6eae5d3b70f6.jpeg
