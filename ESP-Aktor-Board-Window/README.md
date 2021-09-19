# Prerequisites:
## Hardware
* ESP32
* Servo 
* Wires
* Project board
## Software
* ```vscode```
* ```platformio```


# Setup

## Hardware
* Connect the ESP32 chosen output pin (```servoPin``` ) with servo
* Connect servo to the ESP32 Ground

![]https://github.com/JJJS777/Lufty/blob/main/Artefakte/fenster-schaltung.png)



## Software
### Using Win10
* open ```/ESP-Aktor-Board-Window/window``` as existing project in platformio
### Using other OS
* copy ```/ESP-Aktor-Board-Window/window/src/main.cpp``` 
* creste new platformio project under ```/ESP-Aktor-Board-Window``` 
* paste the copied ```main.cpp``` file under ```/src``` 
* install ```ServoESP32``` dependency in platformio
### PIN and PORT
* Change ```servoPin``` variable in ```main.cpp``` so that it mathes your output pin in the ESP32
* Change ```upload_port``` variable in ```platformio.ini``` so that it mathes your USB port connnected to the ESP in case of Code Upload. platformio usually tells you which port that is in the **PIO Home /devices** (in the left slide menu)
### Functions
* ```window_open()```- opens window
* ```window_close()``` - closes window

# Team:
* Julian Schiller
* Manuel Hesse
* Mayess Sammoud
