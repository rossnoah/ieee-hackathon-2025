#include "weather.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// WiFi credentials
const char *ssid = "Noah";
const char *password = "11111111";

// Open-Meteo API configuration
const String latitude = "40.699155";   // Latitude for AEC
const String longitude = "-75.210961"; // Longitude for AEC
const String weatherUrl = "https://api.open-meteo.com/v1/forecast?latitude=" +
                          latitude + "&longitude=" + longitude +
                          "&current=temperature_2m,relative_humidity_2m," +
                          "precipitation,wind_speed_10m,weather_code" +
                          "&temperature_unit=fahrenheit&wind_speed_unit=mph";

// Timing variables
unsigned long lastFetchTime = 0;
unsigned long lastFakeDataChange = 0;
const unsigned long fetchInterval = 5 * 60 * 1000; // 5 minutes
const unsigned long fakeDataInterval = 5 * 1000;   // 5 seconds

// Current weather data
WeatherData currentWeather;

// Fake weather options for rotation
const int NUM_FAKE_WEATHER_TYPES = 4;
const String fakeWeatherTypes[NUM_FAKE_WEATHER_TYPES] = {
    "Sunny", "Cloudy", "Rainy", "Snowy"};
int currentFakeWeatherIndex = 0;

// Function prototypes
void connectToWiFi();
bool fetchRealWeatherData();
void generateFakeWeatherData();
String getWeatherTypeFromCode(int code);

void weatherInit()
{
    // Try to connect to WiFi
    connectToWiFi();

    // Get initial weather data (real or fake)
    refreshWeather();
}

WeatherData getWeather()
{
    // Check if it's time to refresh data
    unsigned long currentTime = millis();

    // Refresh real data if connected and interval passed
    if (WiFi.status() == WL_CONNECTED &&
        (currentTime - lastFetchTime >= fetchInterval || lastFetchTime == 0))
    {
        fetchRealWeatherData();
        lastFetchTime = currentTime;
    }
    // Update fake data if not connected and interval passed
    else if (WiFi.status() != WL_CONNECTED &&
             (currentTime - lastFakeDataChange >= fakeDataInterval || lastFakeDataChange == 0))
    {
        generateFakeWeatherData();
        lastFakeDataChange = currentTime;
    }

    return currentWeather;
}

void refreshWeather()
{
    if (WiFi.status() == WL_CONNECTED)
    {
        if (fetchRealWeatherData())
        {
            lastFetchTime = millis();
        }
        else
        {
            generateFakeWeatherData();
            lastFakeDataChange = millis();
        }
    }
    else
    {
        generateFakeWeatherData();
        lastFakeDataChange = millis();
    }
}

void connectToWiFi()
{
    Serial.print("Connecting to WiFi network: ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    // Wait for connection with timeout
    int timeout = 0;
    while (WiFi.status() != WL_CONNECTED && timeout < 20)
    {
        delay(500);
        Serial.print(".");
        timeout++;
    }

    if (WiFi.status() == WL_CONNECTED)
    {
        Serial.println("\nWiFi connected successfully!");
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
    }
    else
    {
        Serial.println("\nFailed to connect to WiFi. Using fake weather data.");
    }
}

bool fetchRealWeatherData()
{
    Serial.println("\n--- Fetching weather data ---");

    HTTPClient http;
    http.begin(weatherUrl);

    Serial.print("Sending GET request to: ");
    Serial.println(weatherUrl);

    int httpCode = http.GET();
    Serial.print("HTTP response code: ");
    Serial.println(httpCode);

    if (httpCode == 200)
    {
        String payload = http.getString();

        // Parse JSON
        DynamicJsonDocument doc(1024);
        DeserializationError error = deserializeJson(doc, payload);

        if (error)
        {
            Serial.print("JSON parsing failed: ");
            Serial.println(error.c_str());
            http.end();
            return false;
        }
        else
        {
            // Extract weather data
            float temperatureC = doc["current"]["temperature_2m"];
            float precipitation = doc["current"]["precipitation"];
            float windSpeed = doc["current"]["wind_speed_10m"];
            int weatherCode = doc["current"]["weather_code"];

            // Update current weather
            currentWeather.temperatureF = temperatureC; // Already in F due to API parameter
            currentWeather.windSpeedMPH = windSpeed;    // Already in MPH due to API parameter
            currentWeather.precipitationAmount = precipitation;

            // Determine precipitation chance based on amount (simplified)
            currentWeather.precipitationChance = precipitation > 0 ? min(100, (int)(precipitation * 100)) : 0;

            // Get weather type from code
            currentWeather.weatherType = getWeatherTypeFromCode(weatherCode);

            // Mark as real data
            currentWeather.isRealData = true;

            // Print formatted weather data
            Serial.println("\n=== Current Weather Conditions ===");
            Serial.print("Temperature: ");
            Serial.print(currentWeather.temperatureF);
            Serial.println(" °F");
            Serial.print("Wind Speed: ");
            Serial.print(currentWeather.windSpeedMPH);
            Serial.println(" MPH");
            Serial.print("Weather: ");
            Serial.println(currentWeather.weatherType);
            Serial.print("Precipitation: ");
            Serial.print(currentWeather.precipitationAmount);
            Serial.println(" mm");
            Serial.print("Precipitation Chance: ");
            Serial.print(currentWeather.precipitationChance);
            Serial.println("%");
            Serial.println("==================================\n");

            http.end();
            return true;
        }
    }
    else
    {
        Serial.print("HTTP request failed with error code: ");
        Serial.println(httpCode);
        http.end();
        return false;
    }
}

void generateFakeWeatherData()
{
    // Rotate through fake weather types
    currentFakeWeatherIndex = (currentFakeWeatherIndex + 1) % NUM_FAKE_WEATHER_TYPES;
    String weatherType = fakeWeatherTypes[currentFakeWeatherIndex];

    // Generate fake data based on weather type
    float tempF, windMPH, precipAmount;
    int precipChance;

    if (weatherType == "Sunny")
    {
        tempF = random(70, 95);
        windMPH = random(0, 10);
        precipAmount = 0;
        precipChance = 0;
    }
    else if (weatherType == "Cloudy")
    {
        tempF = random(60, 80);
        windMPH = random(5, 15);
        precipAmount = 0;
        precipChance = random(0, 30);
    }
    else if (weatherType == "Rainy")
    {
        tempF = random(50, 70);
        windMPH = random(5, 20);
        precipAmount = random(5, 25) / 10.0;
        precipChance = random(60, 100);
    }
    else
    { // Snowy
        tempF = random(20, 35);
        windMPH = random(5, 25);
        precipAmount = random(5, 30) / 10.0;
        precipChance = random(70, 100);
    }

    // Update current weather with fake data
    currentWeather.temperatureF = tempF;
    currentWeather.windSpeedMPH = windMPH;
    currentWeather.weatherType = weatherType;
    currentWeather.precipitationAmount = precipAmount;
    currentWeather.precipitationChance = precipChance;
    currentWeather.isRealData = false;

    Serial.println("\n=== Fake Weather Conditions ===");
    Serial.print("Temperature: ");
    Serial.print(currentWeather.temperatureF);
    Serial.println(" °F");
    Serial.print("Wind Speed: ");
    Serial.print(currentWeather.windSpeedMPH);
    Serial.println(" MPH");
    Serial.print("Weather: ");
    Serial.println(currentWeather.weatherType);
    Serial.print("Precipitation: ");
    Serial.print(currentWeather.precipitationAmount);
    Serial.println(" in");
    Serial.print("Precipitation Chance: ");
    Serial.print(currentWeather.precipitationChance);
    Serial.println("%");
    Serial.println("================================\n");
}

String getWeatherTypeFromCode(int code)
{
    // WMO Weather interpretation codes (https://open-meteo.com/en/docs)
    if (code == 0)
        return "Clear Sky";
    if (code >= 1 && code <= 3)
        return "Partly Cloudy";
    if (code >= 45 && code <= 48)
        return "Foggy";
    if (code >= 51 && code <= 55)
        return "Drizzle";
    if (code >= 56 && code <= 57)
        return "Freezing Drizzle";
    if (code >= 61 && code <= 65)
        return "Rain";
    if (code >= 66 && code <= 67)
        return "Freezing Rain";
    if (code >= 71 && code <= 77)
        return "Snow";
    if (code >= 80 && code <= 82)
        return "Rain Showers";
    if (code >= 85 && code <= 86)
        return "Snow Showers";
    if (code >= 95 && code <= 99)
        return "Thunderstorm";
    return "Unknown";
}
