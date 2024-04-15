#pragma once

#include "DHT_Base.h"

class DHT11 : public DHT_Base {
public:
    DHT11();
    DHT11(int8_t pin);
public:
    virtual DHT_STATUS read2(float* ptemperature, float* phumidity, uint8_t pdata[DATA_BUS_MAX]);
    virtual DHT_STATUS read2(int8_t pin, float* ptemperature, float* phumidity, uint8_t pdata[DATA_BUS_MAX]);
protected:
    virtual DHT_STATUS sample(uint8_t data[DATA_BUS_MAX]);
};
