// display.cpp
#include "display.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// OLED display settings
#define SCREEN_WIDTH 128    // OLED display width, in pixels
#define SCREEN_HEIGHT 64    // OLED display height, in pixels
#define OLED_RESET -1       // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C // Typical I2C address for SSD1306 displays

// Define the pins for I2C
#define SDA_PIN 6
#define SCL_PIN 7

// Bicolor display constants
#define YELLOW_SECTION_HEIGHT 16 // Top 16 pixels are yellow
#define BLUE_SECTION_START 16    // Blue section starts at pixel 16

// Create display object
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

bool displayInit()
{
    // Initialize I2C with the specified pins
    Wire.begin(SDA_PIN, SCL_PIN);

    // Initialize the OLED display
    if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
    {
        Serial.println(F("SSD1306 allocation failed"));
        return false;
    }

    // Initial display setup
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);

    // Use yellow section for title
    display.setCursor(0, 4);
    display.println(F("Weather Station"));

    // Use blue section for status
    display.setCursor(0, BLUE_SECTION_START + 4);
    display.println(F("Initializing..."));
    display.display();

    Serial.println("OLED display initialized successfully");
    return true;
}

void displayWeather(const WeatherData &weather, float indoorTemp, float indoorHumidity)
{
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);

    // YELLOW SECTION - Header and indoor temperature
    display.setCursor(0, 0);
    display.print(weather.isRealData ? "LIVE" : "SIM");
    display.print(" - ");
    display.println(weather.weatherType);

    // Indoor temperature and humidity
    display.setCursor(0, 8);
    display.print("Indoor: ");
    display.print(int(indoorTemp));
    display.print((char)247); // Degree symbol
    display.print("F, ");
    display.print(int(indoorHumidity));
    display.print("% Hum");

    // BLUE SECTION - Outdoor weather data
    // Outdoor temperature (larger text)
    display.setCursor(0, BLUE_SECTION_START + 2);
    display.setTextSize(2);
    display.print(int(weather.temperatureF));
    display.print((char)247); // Degree symbol
    display.println("F");

    // Return to smaller text for details
    display.setTextSize(1);

    // Display wind speed
    display.setCursor(0, BLUE_SECTION_START + 20);
    display.print("Wind: ");
    display.print(weather.windSpeedMPH, 1);
    display.println(" MPH");

    // Display precipitation
    display.setCursor(0, BLUE_SECTION_START + 30);
    display.print("Precip: ");
    display.print(weather.precipitationAmount, 1);
    display.print(weather.isRealData ? " mm" : " in");
    display.print(" (");
    display.print(weather.precipitationChance);
    display.println("%)");

    // Display temperature difference
    display.setCursor(0, BLUE_SECTION_START + 40);
    float tempDiff = weather.temperatureF - indoorTemp;
    display.print("Diff: ");
    if (tempDiff > 0)
        display.print("+");
    display.print(int(tempDiff));
    display.print((char)247); // Degree symbol
    display.print("F");

    display.display();
}

void displayMessage(const String &line1, const String &line2, const String &line3, const String &line4)
{
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);

    // Use yellow section for first line
    display.setCursor(0, 4);
    display.println(line1);

    // Use blue section for remaining lines
    int yPos = BLUE_SECTION_START + 4;

    if (line2.length() > 0)
    {
        display.setCursor(0, yPos);
        display.println(line2);
        yPos += 10;
    }

    if (line3.length() > 0)
    {
        display.setCursor(0, yPos);
        display.println(line3);
        yPos += 10;
    }

    if (line4.length() > 0)
    {
        display.setCursor(0, yPos);
        display.println(line4);
    }

    display.display();
}

void clearDisplay()
{
    display.clearDisplay();
    display.display();
}
