#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <DHT.h>
#include "WiFi.h"
#include "bme680.h" // bme sensor
#include <Servo.h>  // servo 

#define DHTPIN 18
#define DHTTYPE DHT11

const char* ssid = "Duckn3t";
const char* password =  "quack1QUACK4quack1";

static bme680_sensor_t* sensor = 0;

//Globale Variable
float temperature,
    humidity,
    gas,
    pressure;
int servoPin = 14;
int SensorPin = 26; 
int buttonPressed = 0; //Variable Touchsensor global def.

DHT my_sensor( DHTPIN, DHTTYPE);

void ISRbuttonClicked(){
  buttonPressed = digitalRead(buttonPin);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  /*WLAN*/
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }

  Serial.println("Connected to the WiFi network");

  pinMode(ledPin, OUTPUT); //Output Pin wird initalisiert, Zahl auf dem Mainboard am gelben Kabel
  pinMode(buttonPin, INPUT); //Input pin initalisieren
  attachInterrupt(buttonPin, ISRbuttonClicked, CHANGE);
  my_sensor.begin();
}

void messen(void *pvParameters)
{
    bme680_values_float_t values;

    TickType_t last_wakeup = xTaskGetTickCount();

    // as long as sensor configuration isn't changed, duration is constant
    uint32_t duration = bme680_get_measurement_duration(sensor);

    while (1)
    {
        // trigger the sensor to start one TPHG measurement cycle
        if (bme680_force_measurement (sensor))
        {
            // passive waiting until measurement results are available
            vTaskDelay (duration);

            // alternatively: busy waiting until measurement results are available
            // while (bme680_is_measuring (sensor)) ;

            // Ausgabe der Results
            if (bme680_get_results_float (sensor, &values))
                printf("%.3f BME680 Sensor: %.2f °C, %.2f %%, %.2f hPa, %.2f Ohm\n",
                       (double)sdk_system_get_time()*1e-3,
                       values.temperature, values.humidity,
                       values.pressure, values.gas_resistance);
        }
        // 1 Sekunde warten
        vTaskDelayUntil(&last_wakeup, 1000 / portTICK_PERIOD_MS);
    }
}
void init(void)
{
    // Set UART Parameter.
    uart_set_baud(0, 115200);
    // Give the UART some time to settle
    vTaskDelay(1);
    
    /** -- MANDATORY PART -- */

    #ifdef SPI_USED

    spi_bus_init (SPI_BUS, SPI_SCK_GPIO, SPI_MISO_GPIO, SPI_MOSI_GPIO);

    // init the sensor connected to SPI_BUS with SPI_CS_GPIO as chip select.
    sensor = bme680_init_sensor (SPI_BUS, 0, SPI_CS_GPIO);
    
    #else  // I2C

    // Init all I2C bus interfaces at which BME680 sensors are connected
    i2c_init(I2C_BUS, I2C_SCL_PIN, I2C_SDA_PIN, I2C_FREQ);

    // init the sensor with slave address BME680_I2C_ADDRESS_2 connected to I2C_BUS.
    sensor = bme680_init_sensor (I2C_BUS, BME680_I2C_ADDRESS_2, 0);

    #endif  // SPI_USED

    if (sensor)
    {
        /** -- SENSOR CONFIGURATION PART (optional) --- */

        // Changes the oversampling rates to 4x oversampling for temperature
        // and 2x oversampling for humidity. Pressure measurement is skipped.
        bme680_set_oversampling_rates(sensor, osr_4x, osr_none, osr_2x);

        // Change the IIR filter size for temperature and pressure to 7.
        bme680_set_filter_size(sensor, iir_size_7);

        // Change the heater profile 0 to 200 degree Celcius for 100 ms.
        bme680_set_heater_profile (sensor, 0, 200, 100);
        bme680_use_heater_profile (sensor, 0);

        // Set ambient temperature to 10 degree Celsius
        bme680_set_ambient_temperature (sensor, 10);
            
        /** -- TASK CREATION PART --- */

        // must be done last to avoid concurrency situations with the sensor 
        // configuration part

        // Create a task that uses the sensor
        xTaskCreate(user_task, "user_task", TASK_STACK_DEPTH, NULL, 2, NULL);
    }
    else
        printf("Could not initialize BME680 sensor\n");
}



Servo myservo;  // create servo object to control a servo
// twelve servo objects can be created on most boards

int pos = 0;    // variable to store the servo position

void setup() {
  myservo.attach(13);  // attaches the servo on pin 13 to the servo object
}

void loop() {
  for (pos = 0; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
  for (pos = 180; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  
  humidity = my_sensor.readHumidity();
  temperature = my_sensor.readTemperature();

  if (temperature < 19.00 || temperature > 23.00 || humidity < 50.00  ){
    digitalWrite(ledPin, HIGH);
    Serial.println("Messung Prüfen -> Knopf Drücken");
  }

  if ( buttonPressed > 0 ) {
    Serial.print("Temperatur: ");
    Serial.print(temperature);
    Serial.print("°C / Humidity: ");
    Serial.print(humidity);
    Serial.println(" %");
  }

  delay(2000);
}

