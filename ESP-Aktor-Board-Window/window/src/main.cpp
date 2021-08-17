#include <Arduino.h>
#include <Servo.h>

static const int servoPin = 16;

Servo servo1;

void setup()
{
    Serial.begin(115200);
    servo1.attach(servoPin);
}

void window_open()
{
    for (int posDegrees = 0; posDegrees <= 180; posDegrees++)
    {
        servo1.write(posDegrees);
        Serial.println(posDegrees);
        delay(20);
    }
}
void window_close()
{
    for (int posDegrees = 180; posDegrees >= 0; posDegrees--)
    {
        servo1.write(posDegrees);
        Serial.println(posDegrees);
        delay(20);
    }
}

void loop()
{
    window_open();
    delay(2000);
    window_close();
    delay(2000);
}
