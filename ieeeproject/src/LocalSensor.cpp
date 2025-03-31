#include "LocalSensor.h"

LocalSensor::LocalSensor(uint8_t pin, uint8_t type) : dht(pin, type) {}

void LocalSensor::begin()
{
    dht.begin();
    Serial.println("DHT sensor initialized");
}

bool LocalSensor::update(bool force)
{
    unsigned long currentMillis = millis();

    if (!force && currentMillis - lastReadTime < READ_INTERVAL)
    {
        return false;
    }

    float newHumidity = dht.readHumidity();
    float newTemperature = dht.readTemperature(true); // true = Fahrenheit

    if (isnan(newHumidity) || isnan(newTemperature))
    {
        Serial.println("Failed to read from DHT sensor!");
        return false;
    }

    humidity = newHumidity;
    temperature = newTemperature;
    lastReadTime = currentMillis;

    Serial.print("Local Temperature: ");
    Serial.print(temperature);
    Serial.println(" °F");
    Serial.print("Local Humidity: ");
    Serial.print(humidity);
    Serial.println(" %");

    return true;
}

float LocalSensor::getTemperature() const
{
    return temperature;
}

float LocalSensor::getHumidity() const
{
    return humidity;
}
