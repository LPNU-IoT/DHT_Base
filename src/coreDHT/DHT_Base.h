

#ifndef DHT_BASE_H
#define DHT_BASE_H

#include <Arduino.h>

#define DATA_BUS_MAX 40

#define DHT_ERR_CODES \
    X(DHT_SUCCESS       , 0)\
    X(DHT_START_LOW     , 16)\
    X(DHT_START_HIGH    , 17)\
    X(DHT_DATA_LOW      , 18)\
    X(DHT_DATA_READ     , 19)\
    X(DHT_DATA_EOF      , 20)\
    X(DHT_DATA_CHECKSUM , 21)\
    X(DHT_ZERO_SAMPLES  , 22)\
    X(DHT_NO_PIN        , 23)\
    X(DHT_PIN_MODE      , 24)
    
#define X(a, b) a = (b),
typedef enum {
	DHT_ERR_CODES
} DHT_STATUS;
#undef X


// High 8bits are time duration.
// Low 8bits are error code.
#define DHT_ERR_DURATION(err) (((err) & 0xff00)>>8)
#define DHT_SELF_ERRCODE(err) ((err) & 0x00ff)
#define DHT_COMBINE_ERR(t, err) (((t) << 8) & 0xff00) | DHT_SELF_ERRCODE(err)

#define TO_DHT_STS(err) (DHT_STATUS)(err)


class DHT_Base {
protected:
    uint32_t levelTimeout = 500000; // 500ms
    int8_t pin;
    uint8_t pinInputMode = INPUT;
#ifdef __AVR
    uint8_t bitmask = 0xFF;
    uint8_t port    = 0xFF;
#endif
public:
    DHT_Base();
    DHT_Base(int8_t pin);
public:
    // void setDelay(unsigned long delay);
    /**
   * Reads and returns the humidity from the DHT11 sensor.
   *
   * @return: Humidity value in percentage. Returns DHT11_ERROR_TIMEOUT if reading times out.
   *          Returns DHT11_ERROR_CHECKSUM if checksum validation fails.
   */
//   int readHumidity();

    /**
   * Reads and returns the temperature from the DHT11 sensor.
   *
   * @return: Temperature value in Celsius. Returns DHT11_ERROR_TIMEOUT if reading times out.
   *          Returns DHT11_ERROR_CHECKSUM if checksum validation fails.
   */
//   int readTemperature();

    /**
   * Reads and returns the temperature and humidity from the DHT11 sensor.
   *
   * @param temperature: Reference to a variable where the temperature value will be stored.
   * @param humidity: Reference to a variable where the humidity value will be stored.
   * @return: true if the reading is successful, false if it fails due to timeout or checksum error.
   */
//   int readTemperatureHumidity(int &temperature, int &humidity);

    /**
   * Returns a human-readable error message based on the provided error code.
   *
   * @param errorCode: The error code for which the message is required.
   * @return: A string describing the error.
   */
//   static String getErrorString(int errorCode);

    virtual DHT_STATUS setPin(int8_t pin);
    virtual DHT_STATUS setPinInputMode(uint8_t mode);
public:
    virtual DHT_STATUS read(uint8_t* ptemperature, uint8_t* phumidity, uint8_t pdata[DATA_BUS_MAX]);
    virtual DHT_STATUS read(int8_t pin, uint8_t* ptemperature, uint8_t* phumidity, uint8_t pdata[DATA_BUS_MAX]);
    
    virtual DHT_STATUS read2(float* ptemperature, float* phumidity, uint8_t pdata[DATA_BUS_MAX]) = 0;
    virtual DHT_STATUS read2(int8_t pin, float* ptemperature, float* phumidity, uint8_t pdata[DATA_BUS_MAX]) = 0;
protected:
#ifdef __AVR
    virtual int getBitmask();
    virtual int getPort();
#endif
protected:
 
    virtual int32_t levelTime(uint8_t level, uint16_t firstWait = 10, uint16_t interval = 6);
   
    virtual uint8_t bits2byte(uint8_t data[8]);
   
    virtual DHT_STATUS sample(uint8_t data[DATA_BUS_MAX]) = 0;
   
    virtual DHT_STATUS parse(uint8_t data[DATA_BUS_MAX], int16_t* ptemperature, int16_t* phumidity);
};


#endif //!DHT_BASE_H