#include "ADCSetup.h"
#include "SDLogger.h"

void setup() {
  setupADC();
  while(!setupSD(4)){
    Serial.println("Retrying...");
  }
  initFile();
}

void loop() {
  if (handleConversion() == true && adcMeasurement > 50)
  {
    Serial.print("Lectura ADC:\t");
    logToSD(String(adcMeasurement), true);
  }

  // delay 10mS
  //delay(10);
}