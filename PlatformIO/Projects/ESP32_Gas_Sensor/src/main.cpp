#include <Arduino.h>

#define GAS_SENSOR_AO 2 // Analog pin (VP)
#define GAS_SENSOR_DO 4 // Digital pin (not used in code)
#define MIC_PIN 3       // Analog pin

// Function declarations
float readGas();
float getSoundLevelInDecibels();

void setup() {
    Serial.begin(115200);
    pinMode(GAS_SENSOR_AO, INPUT);
    pinMode(MIC_PIN, INPUT);
}

void loop() {
    float gasVoltage = readGas();
    float soundDB = getSoundLevelInDecibels();

    Serial.print("Sound dB Value: ");
    Serial.println(soundDB);

    delay(500);
}

// Function definitions
float readGas() {
    // Read analog value
    int sensorValue = analogRead(GAS_SENSOR_AO);
    float voltage = sensorValue * (3.3 / 4095.0); // Convert ADC to voltage
    float percentage = (voltage / 3.3) * 100;     // Convert voltage to percentage

    Serial.print("Gas Analog Value: ");
    Serial.print(sensorValue);
    Serial.print(" | Gas Voltage: ");
    Serial.print(voltage);
    Serial.print(" | Gas Percentage: ");
    Serial.println(percentage);

    return voltage;
}

float getSoundLevelInDecibels() {
    // Read the raw analog value from the microphone
    int rawValue = analogRead(MIC_PIN);

    // Convert the raw value to voltage (assuming 3.3V ADC reference on ESP32)
    float voltage = (rawValue / 4095.0) * 3.3;

    // Convert the voltage to decibels (example conversion, adjust based on sensor calibration)
    float decibels = 20 * log10(voltage / 0.00631); // 0.00631 is a reference voltage for 0 dB

    return decibels;
}