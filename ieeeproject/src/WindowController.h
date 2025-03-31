#ifndef WINDOW_CONTROLLER_H
#define WINDOW_CONTROLLER_H

#include <Arduino.h>
#include <Servo.h>
#include "weather.h"

class WindowController
{
private:
    Servo servo;
    uint8_t pin;
    int currentPosition = 0;        // 0 = closed, 180 = fully open
    const float TARGET_TEMP = 75.0; // Target temperature in Fahrenheit
    unsigned long lastAdjustTime = 0;
    const unsigned long ADJUST_INTERVAL = 5 * 1000; // 5 sec between adjustments

public:
    WindowController(uint8_t servoPin);
    void begin();
    void performInitialTest();
    void adjustBasedOnTemperature(float indoorTemp, const WeatherData &outdoorWeather);
    int getCurrentPosition() const;
    void setPosition(int position);
};

#endif // WINDOW_CONTROLLER_H
