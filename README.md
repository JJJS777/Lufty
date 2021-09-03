# Lufty
IoT-Projekt

## Inhalt
1. Projektübersicht
2. Benötigte Teile
3. Vorrausetzungen
4. Anwendungslogik und Funktionsweise
5. MQTT
   1. MQTT auf Client
   2. Mosquitto-Broker auf Raspberry Pi einrichten
6. [Sensor-Board](./ESP-Sensor-Board/README.md)
7. [Aktor-Board-Diffusor](./ESP-Aktor-Board-Diffusor/README.md)
8. [Aktor-Board-Window](./ESP-Aktor-Board-Window/README.md)
9. Ausblick / Erweiterungsmöglichkeiten


## 1. Projektübersicht
Lufty ermöglicht euch für jeden Raum beste Luftqualität zu erreichen.

Das Lufty-Projekt besteht aus drei ESP32, einem Raspberry Pi 4, einem BME680-Sensor zu messen der Luftqualität in Innenräumen, einem Servo-Motor zum Fenster öffnen und einem Diffusor zum Luftfeuchtigkeit erhöhen. 

Hier möchten wir dir zeigen, wie du das Projekt bei dir zuhause selbst Umsetzen kannst.
Jeder ESP hat seine eigene README in der die Details beschreiben werden. In den Ordner findest du auch den Code für die Entsprechenden ESP’s.

## 2. Benötigte Teile

+ 3x [ESP32][1] Mikrocontroller oder vergleichbar
+ 1x [Raspberry Pi][2] + Micro SD Karte
+ 1x [BME680-Grove-Sensor][3]
+ 1x Servo-Motor für Fenster Prototyp
+ 1x [Diffusor][4]
+ 1x 100 ohm Resistor
+ 1x Project Board (Breadboard)
+ 2x [Optocoupler][5]
+ Wire (10x M/M und 10x M/F Jumpwire)

[1]: https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/index.html
[2]: https://www.raspberrypi.org/products/raspberry-pi-4-model-b/
[3]: https://wiki.seeedstudio.com/Grove-Temperature_Humidity_Pressure_Gas_Sensor_BME680/
[4]: https://www.conrad.de/de/p/boneco-u50-luftbefeuchter-schwarz-1-st-2316569.html
[5]: https://www.conrad.de/de/p/isocom-components-optokoppler-phototransistor-sfh615a-4x-dip-4-transistor-dc-183249.html 

## 3. Vorrausetzungen

## 4. Anwendungslogik und Funktionsweise
### Architekturmodell
### Aktivitätsmodell

## 5. MQTT
### Mosquitto-Broker auf Raspberry Pi einrichten
### MQTT auf Client

## 9. Ausblick / Erweiterungsmöglichkeiten
