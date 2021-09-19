# Diffusor-ESP

## Prerequisites:

### Hardware
* ESP32
* Octocoppler
* Wires
* Diffusor (we used **BONECO U50**)
### Software
* ```vscode```
* ```platformio```
## Setup
### Hardware
* Connect the ESP32 chosen output pin (```diffusorPin``` ) with the Octocoppler
* Connect Octocoppler to ESP32 GND
* Connect diffusor button both sides to the Octocuppler
* Optional Connect diffusor power out + - to the VIN and Gnd ports of the ESP32

 ![]https://github.com/JJJS777/Lufty/blob/main/Artefakte/diffusor-schaltung.png)

### Software
#### Using Win10
* open ```/ESP-Aktor-Board-Difffusor/difffusor``` as existing project in platformio
#### Using other OS
* copy ```/ESP-Aktor-Board-Difffusor/difffusor/src/main.cpp``` 
* creste new platformio project under ```/ESP-Aktor-Board-Difffusor``` 
* paste the copied ```main.cpp``` file under ```/src``` 
#### PIN and PORT
* Change ```diffusorPin``` variable in ```main.cpp``` so that it mathes your output pin in the ESP32
* Change ```upload_port``` variable in ```platformio.ini``` so that it mathes your USB port connnected to the ESP in case of Code Upload. platformio usually tells you which port that is in the **PIO Home /devices** (in the left slide menu)
#### Functions
* ```difffusor_on()```- makes difffusor on
* ```difffusor_off()``` - makes difffusor off


## Team:
* Julian Schiller
* Manuel Hesse
* Mayess Sammoud
