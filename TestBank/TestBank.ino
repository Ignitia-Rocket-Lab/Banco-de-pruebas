#include "ADCSetup.h"
#include "SDLogger.h"


void setup() {
  setupADC();
  while(!setupSD(10)){
    Serial.println("Retrying...");
  }
  initFile();
}

void loop() {
  if (handleConversion() == true && adcMeasurement > 50)
  {
    logMeasurement();
  }
  else if (digitalRead(BUTTON_PIN) == HIGH) {
      Serial.println("Button pressed. Closing SD file...");
      closeSD();
      while (digitalRead(BUTTON_PIN) == HIGH) {}
      delay(50); // Pequeño delay para evitar rebotes
  }
  // delay 10mS
  //delay(10);
}