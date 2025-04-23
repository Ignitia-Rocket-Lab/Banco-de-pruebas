#include "ADCSetup.h"
#include "SDLogger.h"

void setup() {
  Serial.begin(115200);
  Serial.println("Test Bank Initializing...");
  setupADC();
  while(!setupSD(10)){
    Serial.println("Retrying...");
  }
  initFile("Time (ms), Force (N)");
}

void loop() {
  if (handleConversion() && adcMeasurement > TEST_TRESHOLD){
    logMeasurement();
  }
}