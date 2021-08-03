#include <Arduino.h>
#include <Servo.h>

int servoPin = 26; 
Servo servo1;
#define servo1Pin 26 // Define the NodeMCU pin to attach the Servo
int pos = 0;

void setup() {
  servo1.attach(servo1Pin);
  }

void servoSetup(){
    for (pos = 0; pos <= 180; pos += 1) { //0 degrees to 180 degrees
    // in steps of 1 degree
    servo1.write(pos);              
    delay(20);                       
  }
    for (pos = 180; pos >= 0; pos -= 1) { //180 degrees to 0 degrees
      servo1.write(pos);              
      delay(20);                       
    }

  // potReading = analogRead(A0);
  // servo1Angle = map(potReading, 0, 1023, 0, 180);
  // servo1.write(servo1Angle); 
  // delay(20);
  // Serial.println(servo1Angle);
}
