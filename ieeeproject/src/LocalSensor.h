#ifndef LOCAL_SENSOR_H
#define LOCAL_SENSOR_H

#include <Arduino.h>
#include <DHT.h>

class LocalSensor
{
private:
    DHT dht;
    float temperature = 0;
    float humidity = 0;
    unsigned long lastReadTime = 0;
    const unsigned long READ_INTERVAL = 30000; // 30 seconds

public:
    LocalSensor(uint8_t pin, uint8_t type);
    void begin();
    bool update(bool force = false);
    float getTemperature() const;
    float getHumidity() const;
};

#endif // LOCAL_SENSOR_H
