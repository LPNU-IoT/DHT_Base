#include <Arduino.h>
#include "DHT_Base.h"

DHT_Base::DHT_Base() {

}

DHT_Base::DHT_Base(int8_t pin) {
    setPin(pin);
}

DHT_STATUS DHT_Base::setPin(int8_t pin) {
    if (pin < 0)
    {
        return DHT_NO_PIN;
    }
    
    this->pin = pin;

#ifdef __AVR
    // (only AVR) - set low level properties for configured pin
    bitmask = digitalPinToBitMask(pin);
    port = digitalPinToPort(pin);
#endif
    return DHT_SUCCESS;
}

DHT_STATUS DHT_Base::setPinInputMode(uint8_t mode) {
    if (mode != INPUT && mode != INPUT_PULLUP) {
        return DHT_PIN_MODE;
    }
    this->pinInputMode = mode;
    return DHT_SUCCESS;
}

DHT_STATUS DHT_Base::read(uint8_t* ptemperature, uint8_t* phumidity, uint8_t pdata[DATA_BUS_MAX]) {
    DHT_STATUS ret = DHT_SUCCESS;

    if (pin == -1) {
        return DHT_NO_PIN;
    }

    float temperature = 0;
    float humidity = 0;
    if ((ret = read2(&temperature, &humidity, pdata)) != DHT_SUCCESS) {
        return ret;
    }

    if (ptemperature) {
        *ptemperature = (uint8_t)(int)temperature;
    }

    if (phumidity) {
        *phumidity = (uint8_t)(int)humidity;
    }

    return ret;
}

DHT_STATUS DHT_Base::read(int8_t pin, uint8_t* ptemperature, uint8_t* phumidity, uint8_t pdata[DATA_BUS_MAX]) {
    setPin(pin);
    return read(ptemperature, phumidity, pdata);
}

#ifdef __AVR
int DHT_Base::getBitmask() {
    return bitmask;
}

int DHT_Base::getPort() {
    return port;
}
#endif

int32_t DHT_Base::levelTime(uint8_t level, uint16_t firstWait, uint16_t interval) {
    uint32_t time_start = micros();
    int32_t time = 0;

#ifdef __AVR
    uint8_t portState = level ? bitmask : 0;
#endif

    bool loop = true;
    for (int i = 0 ; loop; i++) {
        if (time < 0 || time > (int32_t)levelTimeout) {
            return -1;
        }

        if (i == 0) {
            if (firstWait > 0) {
                delayMicroseconds(firstWait);
            }
        } else if (interval > 0) {
            delayMicroseconds(interval);
        }

        // for an unsigned int type, the difference have a correct value
        // even if overflow, explanation here:
        //     https://arduino.stackexchange.com/questions/33572/arduino-countdown-without-using-delay
        time = micros() - time_start;

#ifdef __AVR
        loop = ((*portInputRegister(port) & bitmask) == portState);
#else
        loop = (digitalRead(pin) == level);
#endif
    }

    return time;
}

uint8_t DHT_Base::bits2byte(uint8_t data[8]) {
    uint8_t v = 0;
    for (int i = 0; i < 8; i++) {
        v += data[i] << (7 - i);
    }
    return v;
}

DHT_STATUS DHT_Base::parse(uint8_t data[DATA_BUS_MAX], int16_t* ptemperature, int16_t* phumidity) {
    int16_t humidity = bits2byte(data);
    int16_t humidity2 = bits2byte(data + 8);
    int16_t temperature = bits2byte(data + 16);
    int16_t temperature2 = bits2byte(data + 24);
    uint8_t check = bits2byte(data + 32);
    uint8_t expect = (uint8_t)humidity + (uint8_t)humidity2 + (uint8_t)temperature + (uint8_t)temperature2;
    if (check != expect) {
        return DHT_DATA_CHECKSUM;
    }

    *ptemperature = temperature<<8 | temperature2;
    *phumidity = humidity<<8 | humidity2;

    return DHT_SUCCESS;
}



