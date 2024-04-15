#include <DHT_Base.h>
#define SERIAL_DEBUG_SPD 115200


const uint8_t pinDHT22 = A0;
DHT22 dht22(pinDHT22);

void setup() {
  Serial.begin(SERIAL_DEBUG_SPD);
}

void loop() {
  // start
  Serial.println("=================================");
  Serial.println("Sample DHT22...");
  
  // get float data
  float temperature = 0;
  float humidity = 0;
  DHT_STATUS err = DHT_SUCCESS;
  if ((err = dht22.read2(&temperature, &humidity, NULL)) != DHT_SUCCESS) {
    Serial.print("Read DHT22 failed, err=");
    Serial.print(DHT_SELF_ERRCODE(err));
    Serial.print(",");
    Serial.println(DHT_ERR_DURATION(err));
    delay(2000);
    return;
  }
  
  Serial.print("Sample OK: ");
  Serial.print((float)temperature);
  Serial.print(" *C, ");
  Serial.print((float)humidity);
  Serial.println(" RH%");
  
  // DHT22 sampling rate is 0.5HZ.
  delay(2500);

  // get float data
  uint8_t temperature2 = 0;
  uint8_t humidity2 = 0;
  if ((err = dht22.read(&temperature2, &humidity2, NULL)) != DHT_SUCCESS) {
    Serial.print("Read DHT22 failed, err=");
    Serial.print(DHT_SELF_ERRCODE(err));
    Serial.print(",");
    Serial.println(DHT_ERR_DURATION(err));
    delay(2000);
    return;
  }
  
  Serial.print("Sample OK: ");
  Serial.print((int)temperature2);
  Serial.print(" *C, ");
  Serial.print((int)humidity2);
  Serial.println(" RH%");
}
