#ifndef WEATHER_H
#define WEATHER_H

#include <Arduino.h>

// Weather data structure
struct WeatherData
{
    float temperatureF;        // Temperature in Fahrenheit
    float windSpeedMPH;        // Wind speed in MPH
    String weatherType;        // Weather condition (e.g., "Sunny", "Rainy")
    float precipitationAmount; // Precipitation amount in inches
    int precipitationChance;   // Precipitation chance as percentage (0-100)
    bool isRealData;           // Flag to indicate if data is real or fake
};

// Initialize the weather module
void weatherInit();

// Get current weather data (real or fake depending on WiFi status)
WeatherData getWeather();

// Force a refresh of weather data
void refreshWeather();

#endif
