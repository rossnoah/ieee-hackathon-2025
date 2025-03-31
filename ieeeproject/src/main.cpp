#include <Arduino.h>
#include "weather.h"
#include "display.h"
#include "LocalSensor.h"
#include "WindowController.h"

// DHT sensor setup
#define DHTPIN 9
#define DHTTYPE DHT11

// Servo setup
#define SERVOPIN 3

// Create instances
LocalSensor localSensor(DHTPIN, DHTTYPE);
WindowController windowController(SERVOPIN);

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

  Serial.println("\n\n=== ESP32 Smart Window Controller ===");
  Serial.println("Initializing...");

  // Initialize display
  if (!displayInit())
  {
    Serial.println("Failed to initialize display!");
  }
  else
  {
    displayMessage("Smart Window", "Initializing...");
  }

  // Initialize DHT sensor
  localSensor.begin();

  // Initialize window controller
  windowController.begin();

  // Initialize weather module
  weatherInit();

  // Get initial local sensor readings
  localSensor.update(true);

  // Display startup message
  displayMessage("Smart Window", "Ready!", "Monitoring temp...");
}

void loop()
{
  // Update local sensor readings
  localSensor.update();

  // Get current weather data
  WeatherData weather = getWeather();

  // Update display with weather and window information
  String windowStatus = "Window: ";
  int position = windowController.getCurrentPosition();
  if (position == 0)
  {
    windowStatus += "Closed";
  }
  else if (position < 45)
  {
    windowStatus += "Barely Open";
  }
  else if (position < 90)
  {
    windowStatus += "Partly Open";
  }
  else if (position < 135)
  {
    windowStatus += "Mostly Open";
  }
  else
  {
    windowStatus += "Fully Open";
  }

  displayMessage(
      "In: " + String(localSensor.getTemperature()) + "F Out: " + String(weather.temperatureF) + "F",
      windowStatus,
      weather.weatherType);

  // Adjust window based on temperature
  windowController.adjustBasedOnTemperature(localSensor.getTemperature(), weather);

  // Process any serial commands
  if (Serial.available())
  {
    String command = Serial.readStringUntil('\n');
    command.trim();

    if (command == "weather")
    {
      Serial.println("Manual weather update requested");
      refreshWeather();
      weather = getWeather();

      // Display weather info
      Serial.println("\n=== Weather Conditions ===");
      Serial.print("Data Source: ");
      Serial.println(weather.isRealData ? "Real (API)" : "Simulated");
      Serial.print("Temperature: ");
      Serial.print(weather.temperatureF);
      Serial.println(" °F");
      Serial.print("Wind Speed: ");
      Serial.print(weather.windSpeedMPH);
      Serial.println(" MPH");
      Serial.print("Weather: ");
      Serial.println(weather.weatherType);
      Serial.print("Precipitation: ");
      Serial.print(weather.precipitationAmount);
      Serial.println(weather.isRealData ? " mm" : " in");
      Serial.print("Precipitation Chance: ");
      Serial.print(weather.precipitationChance);
      Serial.println("%");
      Serial.println("==========================\n");
    }
    else if (command == "local")
    {
      // Force update of local sensor readings
      localSensor.update(true);

      // Display local sensor readings
      Serial.println("\n=== Local Sensor Readings ===");
      Serial.print("Temperature: ");
      Serial.print(localSensor.getTemperature());
      Serial.println(" °F");
      Serial.print("Humidity: ");
      Serial.print(localSensor.getHumidity());
      Serial.println(" %");
      Serial.println("============================\n");
    }
    else if (command.startsWith("window "))
    {
      // Manual window control
      int pos = command.substring(7).toInt();
      Serial.print("Manual window position: ");
      Serial.println(pos);
      windowController.setPosition(pos);
    }
  }

  delay(1000); // Small delay to prevent CPU hogging
}
