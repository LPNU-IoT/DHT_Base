#include "DHT11.h"

DHT11::DHT11() {
}

DHT11::DHT11(int8_t pin) : DHT_Base (pin) {
}

DHT_STATUS DHT11::read2(float* ptemperature, float* phumidity, uint8_t pdata[DATA_BUS_MAX]) {
    DHT_STATUS ret = DHT_SUCCESS;

    if (pin == -1) {
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
        *ptemperature = (int)(temperature>>8);
    }
    if (phumidity) {
        *phumidity = (int)(humidity>>8);
    }

    if (temperature == 0 && humidity == 0) {
        return DHT_ZERO_SAMPLES;
    }

    return ret;
}

DHT_STATUS DHT11::read2(int8_t pin, float* ptemperature, float* phumidity, uint8_t pdata[DATA_BUS_MAX]) {
    setPin(pin);
    return read2(ptemperature, phumidity, pdata);
}

DHT_STATUS DHT11::sample(uint8_t data[DATA_BUS_MAX]) {
    memset(data, 0, DATA_BUS_MAX);

    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);            // 1.
    delay(20);                         // specs [2]: 18us

    // Pull high and set to input, before wait 40us.
    digitalWrite(pin, HIGH);           // 2.
    pinMode(pin, this->pinInputMode);
    delayMicroseconds(25);             // specs [2]: 20-40us

    // DHT11 starting:
    //    1. PULL LOW 80us
    //    2. PULL HIGH 80us
    int32_t time_lvl = levelTime(LOW);          // 1.
    if (time_lvl < 30) {                    // specs [2]: 80us
        return TO_DHT_STS(DHT_COMBINE_ERR(time_lvl, DHT_START_LOW));
    }

    time_lvl = levelTime(HIGH);             // 2.
    if (time_lvl < 50) {                    // specs [2]: 80us
        return TO_DHT_STS(DHT_COMBINE_ERR(time_lvl, DHT_START_HIGH));
    }

    // DHT11 data transmite:
    //    1. 1bit start, PULL LOW 50us
    //    2. PULL HIGH:
    //         - 26-28us, bit(0)
    //         - 70us, bit(1)
    for (uint8_t j = 0; j < DATA_BUS_MAX; j++) {
          time_lvl = levelTime(LOW);          // 1.
          if (time_lvl < 24) {                    // specs says: 50us
              return TO_DHT_STS(DHT_COMBINE_ERR(time_lvl, DHT_DATA_LOW));
          }

          // read a bit
          time_lvl = levelTime(HIGH);              // 2.
          if (time_lvl < 11) {                     // specs say: 20us
              return TO_DHT_STS(DHT_COMBINE_ERR(time_lvl, DHT_DATA_READ));
          }
          data[ j ] = (time_lvl > DATA_BUS_MAX ? 1 : 0);     // specs: 26-28us -> 0, 70us -> 1
    }

    // DHT11 EOF:
    //    1. PULL LOW 50us.
    time_lvl = levelTime(LOW);                     // 1.
    if (time_lvl < 24) {                           // specs say: 50us
        return TO_DHT_STS(DHT_COMBINE_ERR(time_lvl, DHT_DATA_EOF));
    }

    return DHT_SUCCESS;
}

