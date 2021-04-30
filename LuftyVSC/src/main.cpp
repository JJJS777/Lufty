// #include <Arduino.h>

// void setup() {
//   // put your setup code here, to run once:
// }

// void loop() {
//   // put your main code here, to run repeatedly:
// }

#include <Arduino.h>

int buttonPressed = 0; //Variable global def.
int buttonPin = 26; 
int ledPin = 14;


void ISRbuttonClicked(){
  buttonPressed = digitalRead(buttonPin);
}

/*Wird am Anfang immer ausgeführt*/
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT); //Output Pin wird initalisiert, Zahl auf dem Mainboard am gelben Kabel
  pinMode(buttonPin, INPUT); //Input pin initalisieren
  attachInterrupt(buttonPin, ISRbuttonClicked, CHANGE);
  //digitalWrite(14, HIGH); LED wird beim Initalisieren angestellt
}

/*hier findet die eigentliche Logik statt*/
void loop() {
  //put your main code here, to run repeatedly:
  if(buttonPressed > 0) {
    digitalWrite(ledPin, HIGH);
  } else {
    digitalWrite(ledPin, LOW);
  }
}  