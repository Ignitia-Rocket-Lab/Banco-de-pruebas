#ifndef SERVOSWITCH_H
#define SERVOSWITCH_H

#include <Servo.h>

// Constantes
#define SERVO_PIN 5           // Pin del servo
#define ANGLE_ON 66         // Ángulo para "ON" (ajustable)
#define ANGLE_OFF 31        // Ángulo para "OFF" (ajustable)

// Variables globales (externas)
extern Servo servo;           // Objeto Servo
extern bool isServoOn;        // Estado actual (ON/OFF)
extern int currentAngle;      // Ángulo actual

// Funciones
void setupServo();            // Inicializa el servo
void onSwitch();            // Mueve a posición ON
void offSwitch();           // Mueve a posición OFF
void toggleSwitch();          // Cambia entre ON/OFF
bool getServoState();         // Devuelve true si está ON
bool servoProcessSerialCommand(); // Procesar comando Serial (devuelve true si se movió el servo)
int getServoAngle();          // Devuelve el ángulo actual


#endif