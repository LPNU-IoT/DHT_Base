#include <DHT_Base.h>
#define SERIAL_DEBUG_SPD 115200


const uint8_t pinDHT11 = A1;
DHT11 dht11(pinDHT11);

void setup() {
  Serial.begin(SERIAL_DEBUG_SPD);
}

void loop() {
  // start working...
  Serial.println("=================================");
  Serial.println("Sample DHT11...");
  
  // get uint8_t data
  uint8_t temperature = 0;
  uint8_t humidity = 0;
  DHT_STATUS err = DHT_SUCCESS;
  if ((err = dht11.read(&temperature, &humidity, NULL)) != DHT_SUCCESS)
  {
    Serial.print("Read DHT11 failed, err="); 
    Serial.print(DHT_SELF_ERRCODE(err));
    Serial.print(","); 
    Serial.println(DHT_ERR_DURATION(err)); 
    delay(1000);
    return;
  }
  
  Serial.print("Sample OK: ");
  Serial.print((int)temperature); Serial.print(" *C, "); 
  Serial.print((int)humidity); Serial.println(" H");
  
  // DHT11 sampling rate is 1HZ.
  delay(1500);
}
