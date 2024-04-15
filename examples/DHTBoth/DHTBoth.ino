#include <Arduino.h>
#include <DHT_Base.h>
#define SERIAL_DEBUG_SPD 115200

#define dataPinSensor1 A1
#define dataPinSensor2 A0
// const uint8_t dataPinSensor1 = A1;
// const uint8_t dataPinSensor2 = A0;

DHT11 dht11;
DHT22 dht22;

void setup() {
  Serial.begin(SERIAL_DEBUG_SPD);

  dht11.setPin(dataPinSensor1);
  dht22.setPin(dataPinSensor2);
}

void loop() {
  // read both sensors accurate data

  Serial.println("=================================");

  // Reading data from DHT11
  float temperature = 0;
  float humidity = 0;
  DHT_STATUS err = DHT_SUCCESS;
  if ((err = dht11.read2(&temperature, &humidity, NULL)) != DHT_SUCCESS) {
    Serial.print("Read Sensor 1 failed, err=");
    Serial.print(DHT_SELF_ERRCODE(err));
    Serial.print(",");
    Serial.println(DHT_SELF_ERRCODE(err));
    delay(1000);
    return;
  }

   // converting Celsius to Fahrenheit
  float fahr_temp = temperature * 1.8 + 32;
  Serial.print("DHT11 Sample OK: ");
  Serial.print((float)temperature);
  Serial.print(" *C, "); 
  Serial.print((float)fahr_temp);
  Serial.print(" *F, "); 
  Serial.print((float)humidity);
  Serial.println(" H humidity");


  // Reading data from DHT22 

  float temperature2 = 0;
  float humidity2 = 0;
  if ((err = dht22.read2(&temperature2, &humidity2, NULL)) != DHT_SUCCESS) {
    Serial.print("Read Sensor 2 failed, err=");
    Serial.print(DHT_SELF_ERRCODE(err));
    Serial.print(",");
    Serial.println(DHT_ERR_DURATION(err));
    delay(1000);
    return;
  }

  // converting Celsius to Fahrenheit
  float fb = temperature2 * 1.8 + 32;
  Serial.print("DHT22 Sample OK: ");
  Serial.print((float)temperature2);
  Serial.print(" *C, "); 
  Serial.print((float)fb);
  Serial.print(" *F, "); 
  Serial.print((float)humidity2);
  Serial.println(" H humidity");

  delay(5000);
}
