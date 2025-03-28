#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// Function prototypes - add these before setup()
void connectToWiFi();
void getWeatherData();

// WiFi credentials
const char *ssid = "Noah";
const char *password = "11111111";

// Open-Meteo API configuration

const String latitude = "40.699155";   // Latitude for AEC
const String longitude = "-75.210961"; // Longitude for AEC
const String weatherUrl = "https://api.open-meteo.com/v1/forecast?latitude=" + latitude +
                          "&longitude=" + longitude +
                          "&current=temperature_2m,relative_humidity_2m,precipitation,wind_speed_10m";

unsigned long lastFetchTime = 0;
const unsigned long fetchInterval = 5 * 60 * 1000; // 5 minutes in milliseconds

void setup()
{
  // Initialize Serial communication
  Serial.begin(115200);

  // WAIT FOR SERIAL MONITOR TO OPEN
  // TODO: REMOVE THIS IN PROD
  while (!Serial)
  {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  delay(1000); // Give serial monitor time to start

  Serial.println("\n\n=== ESP32 Weather Station ===");
  Serial.println("Initializing...");

  // Connect to WiFi
  connectToWiFi();
  getWeatherData();
}

void loop()
{
  // Check WiFi connection
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("WiFi disconnected. Reconnecting...");
    connectToWiFi();
  }

  // Fetch weather data every 5 minutes
  unsigned long currentTime = millis();
  if (currentTime - lastFetchTime >= fetchInterval || lastFetchTime == 0)
  {
    getWeatherData();
    lastFetchTime = currentTime;
  }

  // Process any serial commands
  if (Serial.available())
  {
    String command = Serial.readStringUntil('\n');
    command.trim();

    if (command == "weather")
    {
      Serial.println("Manual weather update requested");
      getWeatherData();
    }
  }

  delay(1000); // Small delay to prevent CPU hogging
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
    Serial.println("\nFailed to connect to WiFi. Please check credentials.");
  }
}

void getWeatherData()
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

    // Print raw JSON for debugging
    Serial.println("Raw JSON response:");
    Serial.println(payload);

    // Parse JSON
    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, payload);

    if (error)
    {
      Serial.print("JSON parsing failed: ");
      Serial.println(error.c_str());
    }
    else
    {
      // Extract weather data
      float temperature = doc["current"]["temperature_2m"];
      float humidity = doc["current"]["relative_humidity_2m"];
      float precipitation = doc["current"]["precipitation"];
      float windSpeed = doc["current"]["wind_speed_10m"];
      String tempUnit = doc["current_units"]["temperature_2m"];
      String humidityUnit = doc["current_units"]["relative_humidity_2m"];
      String precipUnit = doc["current_units"]["precipitation"];
      String windUnit = doc["current_units"]["wind_speed_10m"];

      // Print formatted weather data
      Serial.println("\n=== Current Weather Conditions ===");
      Serial.print("Temperature: ");
      Serial.print(temperature);
      Serial.println(" " + tempUnit);

      Serial.print("Humidity: ");
      Serial.print(humidity);
      Serial.println(" " + humidityUnit);

      Serial.print("Precipitation: ");
      Serial.print(precipitation);
      Serial.println(" " + precipUnit);

      Serial.print("Wind Speed: ");
      Serial.print(windSpeed);
      Serial.println(" " + windUnit);

      Serial.println("==================================\n");
    }
  }
  else
  {
    Serial.print("HTTP request failed with error code: ");
    Serial.println(httpCode);
  }

  http.end();
}
