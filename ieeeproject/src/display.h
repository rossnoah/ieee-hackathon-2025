// display.h
#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>
#include "weather.h"

// Initialize the OLED display
bool displayInit();

// Display weather information on the OLED
void displayWeather(const WeatherData &weather, float indoorTemp = 68.0, float indoorHumidity = 0.0);

// Display a message on the OLED
void displayMessage(const String &line1, const String &line2 = "", const String &line3 = "", const String &line4 = "");

// Clear the display
void clearDisplay();

#endif
