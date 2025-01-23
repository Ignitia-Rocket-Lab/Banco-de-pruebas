#include "ADCSetup.h"
#include "SDLogger.h"

#define SD_CS_PIN 10
#define SD_close_button 3

bool Button_state = 0; 
float N = 0;

void setup() {
  setupADC();
  pinMode(SD_close_button, INPUT);
  while(!setupSD(SD_CS_PIN)){
    Serial.println("Retrying...");
  }
  initFile();
}

void loop() {
  Button_state = digitalRead(SD_close_button);
  if (handleConversion() == true && adcMeasurement > 50)
  {
    Serial.print("Newtons:\t");
    N = WEIGHT(adcMeasurement) * 9.81;
    logToSD(String(N), true);
  } 
    
  if (Button_state == 1)
  {  
    closeSD();
  } 
  
  // delay 10mS
  //delay(10);
}