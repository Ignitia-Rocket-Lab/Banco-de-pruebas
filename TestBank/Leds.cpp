#include "Leds.h"

void setupLEDs() {
  pinMode(LED_LEFT, OUTPUT);
  pinMode(LED_RIGHT, OUTPUT);
  digitalWrite(LED_LEFT, LOW);
  digitalWrite(LED_RIGHT, LOW);
}

void setLEDState(LEDState state) {
  digitalWrite(LED_LEFT, (state & LED_LEFT_ON) ? HIGH : LOW);
  digitalWrite(LED_RIGHT, (state & LED_RIGHT_ON) ? HIGH : LOW);
}

void turnOff() { setLEDState(LED_OFF); }
void turnOnRight() { setLEDState(LED_RIGHT_ON); }
void turnOnLeft() { setLEDState(LED_LEFT_ON); }
void turnOnAll() { setLEDState(LED_ALL_ON); }

// Procesar comando Serial para LEDs (devuelve true si cambió el estado)
bool ledsProcessSerialCommand() {
  if (Serial.available() > 0) {
    int command = Serial.parseInt();
    switch (command) {
      case 1:
        turnOff();
        return true;
      case 2:
        turnOnRight();
        return true;
      case 3:
        turnOnLeft();
        return true;
      case 4:
        turnOnAll();
        return true;
    }
    while (Serial.available() > 0) Serial.read();  // Limpiar buffer
  }
  return false;
}