#include "WindowController.h"

WindowController::WindowController(uint8_t servoPin) : pin(servoPin) {}

void WindowController::begin()
{
    servo.attach(pin);
    setPosition(0); // Start with window closed
    Serial.println("Window controller initialized");
}

void WindowController::performInitialTest()
{
    Serial.println("Testing window controller...");

    // Close window
    setPosition(0);
    delay(1000);

    // Open window halfway
    setPosition(90);
    delay(1000);

    // Open window fully
    setPosition(180);
    delay(1000);

    // Return to closed position
    setPosition(0);
    delay(1000);

    Serial.println("Window controller test complete");
}

void WindowController::adjustBasedOnTemperature(float indoorTemp, const WeatherData &outdoorWeather)
{
    unsigned long currentMillis = millis();

    // Only adjust at certain intervals to prevent constant servo movement
    if (currentMillis - lastAdjustTime < ADJUST_INTERVAL)
    {
        return;
    }

    lastAdjustTime = currentMillis;

    // Calculate temperature difference from target
    float tempDifference = indoorTemp - TARGET_TEMP;

    // Decision logic for window position - only fully open or fully closed
    int newPosition = currentPosition;

    // Check for bad weather first - always close window
    if (outdoorWeather.weatherType.indexOf("Rain") >= 0 ||
        outdoorWeather.weatherType.indexOf("Snow") >= 0 ||
        outdoorWeather.weatherType.indexOf("Thunder") >= 0)
    {
        newPosition = 0; // Fully closed
        Serial.println("Closing window due to bad weather");
    }
    // If indoor temp is too high
    else if (tempDifference > 1.0)
    {
        // Check if outdoor temp is cooler than indoor
        if (outdoorWeather.temperatureF < indoorTemp)
        {
            // Open window fully to let cool air in
            newPosition = 180; // Fully open
            Serial.println("Opening window fully to cool room");
        }
        else
        {
            // Outdoor is warmer, close window
            newPosition = 0; // Fully closed
            Serial.println("Closing window to keep heat out");
        }
    }
    // If indoor temp is too low
    else if (tempDifference < -1.0)
    {
        // Check if outdoor temp is warmer than indoor
        if (outdoorWeather.temperatureF > indoorTemp)
        {
            // Open window fully to let warm air in
            newPosition = 180; // Fully open
            Serial.println("Opening window fully to warm room");
        }
        else
        {
            // Outdoor is colder, close window
            newPosition = 0; // Fully closed
            Serial.println("Closing window to keep cold out");
        }
    }
    // If temperature is in acceptable range
    else
    {
        // Close window when temperature is in acceptable range
        newPosition = 0; // Fully closed
        Serial.println("Closing window - temperature is in acceptable range");
    }

    // Apply the new position if it changed
    if (newPosition != currentPosition)
    {
        setPosition(newPosition);
    }
}

int WindowController::getCurrentPosition() const
{
    return currentPosition;
}

void WindowController::setPosition(int position)
{
    // Ensure position is within valid range
    position = constrain(position, 0, 180);

    // Update current position
    currentPosition = position;

    // Move servo
    servo.write(position);

    Serial.print("Window position set to: ");
    Serial.println(position);
}
