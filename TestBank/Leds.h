#ifndef LEDS_H
#define LEDS_H

#include <Arduino.h>

// Pines de los LEDs (MOSFETs)
const uint8_t LED_LEFT = 6;   // Pin para LEDs izquierdos
const uint8_t LED_RIGHT = 9;  // Pin para LEDs derechos

// Estados posibles
enum LEDState {
  LED_OFF = 0b00,      // 00: Apagado
  LED_RIGHT_ON = 0b01, // 01: Derecho encendido
  LED_LEFT_ON = 0b10,  // 10: Izquierdo encendido
  LED_ALL_ON = 0b11    // 11: Ambos encendidos
};

// Funciones de control
void setupLEDs();
void setLEDState(LEDState state);
void turnOff();
void turnOnRight();
void turnOnLeft();
void turnOnAll();
bool ledsProcessSerialCommand();

#endif