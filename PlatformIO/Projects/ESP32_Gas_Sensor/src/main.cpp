


#include <Arduino.h>

#define GAS_SENSOR_AO 2 //Analog pin ( VP)
#define GAS_SENSOR_DO 4 //Digital pin 



// put function declarations here:
float readGas();
int gas_output_voltage;

void setup() {
    Serial.begin(115200);
    pinMode(GAS_SENSOR_AO, INPUT);
}

void loop() {
  
readGas();

delay(500);
}

// put function definitions here:
float readGas(){
  // read analog value 
  int sensorValue = analogRead(GAS_SENSOR_AO);
  float voltage = sensorValue*(3.3/4095.0); //convert ADC to volatage 

  Serial.print("Analog Vlaue:");
  Serial.print(sensorValue);
  Serial.print(" | Voltage: ");
  Serial.print(voltage);

  return voltage;
}


