#include "ADCSetup.h"
#include "SDLogger.h"
#include "ServoSwitch.h"
#include "Leds.h"
#include "Igniter.h"

// Definicion de usos de leds y estados
#define CASO1 turnOff();
#define CASO2 turnOnRight();
#define CASO3 turnOnLeft();
#define CASO4 turnOnAll();

void setup() {
  Serial.println("Iniciando sistema...");

  setupLEDs();
  // Prueba de LEDs con delays para verificación visual
  Serial.println("Encendiendo LED derecho");
  CASO4;
  delay(500);
  Serial.println("Apagando todos los LEDs");
  CASO1;
  delay(500);
  Serial.println("Encendiendo LED derecho");
  CASO4;
  delay(500);
  Serial.println("Apagando todos los LEDs");
  CASO1;
  delay(500);
  Serial.println("Encendiendo LED derecho");
  CASO4;
  delay(500);
  Serial.println("Apagando todos los LEDs");
  CASO1;
  delay(500);

  
  setupServo();                 // Inicializa servo
  offSwitch(); 
  setupIgniter(); 
  // 2. Abrir y cerrar servo (una vez cada uno)
  Serial.println("Switch en ON...");
  onSwitch(); 

  delay(5000);     // Abre servo (posición ON)
  igniteMotor(); // Enciende el cerillo electronico
  delay(3000);
  resetIgniterSafeState(); // Apaga la señal del pin y cierra el circuito con el servo
  delay(1000);

  setupADC();
  while(!setupSD()){
    Serial.println("Retrying...");
  } 
  initFile();
}

//Loop de funcionamientos de ADS y SDLogger
//void loop() {
  //if (handleConversion() == true && adcMeasurement > 50)
  //  Serial.print("Lectura ADC:\t");
  //  logToSD(String(adcMeasurement), true);
  //}


  // delay 10mS
  //delay(10);
//}


// Loop para probar posiciones del servo
void loop() {
}

//Loop para probar posiciones del servo
//void loop() {
//  if (ledsProcessSerialCommand()) {
//    logToSD("LEDs cambiaron de estado: " + getLEDStateString());
//  }
//}