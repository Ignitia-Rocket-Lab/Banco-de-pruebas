#ifndef IGNITER_H
#define IGNITER_H

#include <Arduino.h>
#include "ServoSwitch.h"

const uint8_t IGNITER = 4;   // Pin para activación del cerillo electronico

extern bool currentIgniterState; // Variable de almacenamiento del estado electronico del MOSFET de activacion

void setupIgniter(); // Inicializa los pines necesarios para el ignitor 
void igniteMotor(); // Enciende el cerillo electronico
void prepareIgniter(); // Gira el servomotor para habilitar el ignitor
void resetIgniterSafeState(); // Apaga la señal del pin y cierra el circuito con el servo
bool getMosfetState(); // El usuario puede obtener el estado del pin

#endif