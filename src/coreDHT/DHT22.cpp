#include "DHT22.h"

DHT22::DHT22() {
}

DHT22::DHT22(int8_t pin) : DHT_Base (pin) {
}

DHT_STATUS DHT22::read2(float* ptemperature, float* phumidity, uint8_t pdata[DATA_BUS_MAX]) {
    DHT_STATUS ret = DHT_SUCCESS;

    if (pin < 0) {
        return DHT_NO_PIN;
    }

    uint8_t data[DATA_BUS_MAX] = {0};
    if ((ret = sample(data)) != DHT_SUCCESS) {
        return ret;
    }

    int16_t temperature = 0;
    int16_t humidity = 0;
    if ((ret = parse(data, &temperature, &humidity)) != DHT_SUCCESS) {
        return ret;
    }

    if (pdata) {
        memcpy(pdata, data, DATA_BUS_MAX);
    }
    if (ptemperature) {
        *ptemperature = (float)((temperature & 0x8000 ? -1 : 1) * (temperature & 0x7FFF)) / 10.0;
    }
    if (phumidity) {
        *phumidity = (float)humidity / 10.0;
    }

    return ret;
}

DHT_STATUS DHT22::read2(int8_t pin, float* ptemperature, float* phumidity, uint8_t pdata[DATA_BUS_MAX]) {
    DHT_STATUS sts = setPin(pin);
    if (sts)
    {
        return sts;
    }
    
    sts = read2(ptemperature, phumidity, pdata);
    return sts; 
}

DHT_STATUS DHT22::sample(uint8_t data[DATA_BUS_MAX]) {
   
    memset(data, 0, DATA_BUS_MAX);

    // According to protocol: http://akizukidenshi.com/download/ds/aosong/AM2302.pdf
    // notify DHT22 to start:
    //    1. T(be), PULL LOW 1ms(0.8-20ms).
    //    2. T(go), PULL HIGH 30us(20-200us), use 40us.
    //    3. SET TO INPUT or INPUT_PULLUP.
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
    delayMicroseconds(1000);

    digitalWrite(pin, HIGH);
    pinMode(pin, this->pinInputMode);
    delayMicroseconds(40);

    // DHT22 starting:
    //    1. T(rel), PULL LOW 80us(75-85us).
    //    2. T(reh), PULL HIGH 80us(75-85us).
    int32_t time_lvl = 0;
    if ((time_lvl = levelTime(LOW)) < 30) {
        return TO_DHT_STS(DHT_COMBINE_ERR(time_lvl, DHT_START_LOW));
    }
    if ((time_lvl = levelTime(HIGH)) < 50) {
        return TO_DHT_STS(DHT_COMBINE_ERR(time_lvl, DHT_START_HIGH));
    }

    // DHT22 data transmite:
    //    1. T(LOW), 1bit start, PULL LOW 50us(48-55us).
    //    2. T(H0), PULL HIGH 26us(22-30us), bit(0)
    //    3. T(H1), PULL HIGH 70us(68-75us), bit(1)
    for (uint8_t j = 0; j < 40; j++) {
          time_lvl = levelTime(LOW);          // 1.
          if (time_lvl < 24) {                    // specs says: 50us
              return TO_DHT_STS(DHT_COMBINE_ERR(time_lvl, DHT_DATA_LOW));
          }

          // read a bit
          time_lvl = levelTime(HIGH);              // 2.
          if (time_lvl < 11) {                     // specs say: 26us
              return TO_DHT_STS(DHT_COMBINE_ERR(time_lvl, DHT_DATA_READ));
          }
          data[ j ] = (time_lvl > DATA_BUS_MAX ? 1 : 0);     // specs: 22-30us -> 0, 70us -> 1
    }

    // DHT22 EOF:
    //    1. T(en), PULL LOW 50us(45-55us).
    time_lvl = levelTime(LOW);
    if (time_lvl < 24) {
        return TO_DHT_STS(DHT_COMBINE_ERR(time_lvl, DHT_DATA_EOF));
    }

    return DHT_SUCCESS;
}