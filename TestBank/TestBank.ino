#include "ADCSetup.h"

void setup() {
  setupADC();
}

void loop() {
  if (handleConversion() == true && adcMeasurement > 10)
  {
    Serial.print("Lectura ADC:\t");
    Serial.print(adcMeasurement);
    Serial.print("\t\tPeso Calculado:\t");
    Serial.println(WEIGHT(adcMeasurement));
  }

  // delay 10mS
  //delay(10);
}