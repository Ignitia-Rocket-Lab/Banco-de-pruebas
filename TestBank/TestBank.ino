#include "ADCSetup.h"
#include "SDLogger.h"
#include "ServoSwitch.h"
#include "Leds.h"

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
  CASO2;
  delay(3000);
  Serial.println("Encendiendo LED izquierdo");
  CASO3;
  delay(3000);
  Serial.println("Encendiendo ambos LEDs");
  CASO4;
  delay(3000);
  Serial.println("Apagando todos los LEDs");
  CASO1;
  delay(3000);

  
  setupServo();                 // Inicializa servo
  // 2. Abrir y cerrar servo (una vez cada uno)
  Serial.println("Abriendo switch...");
  onSwitch();                 // Abre servo (posición ON)
  delay(1000);                  // Espera 1 segundo
  Serial.println("Cerrando switch...");
  offSwitch();                // Cierra servo (posición OFF)
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
//void loop() {
//    if (servoProcessSerialCommand()) {
//    logToSD("Servo movido a: " + String(currentAngle), true);
//  }
//}

//Loop para probar posiciones del servo
void loop() {
  if (ledsProcessSerialCommand()) {
    logToSD("LEDs cambiaron de estado: " + getLEDStateString());
  }
}