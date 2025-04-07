#include "ServoSwitch.h"
#include <Arduino.h>

// Definiciones de variables globales
Servo servo;
bool isServoOn = false;
int currentAngle = 0;

// Inicialización
void setupServo() {
  servo.attach(SERVO_PIN);
}

// Mover servo a un ángulo específico
void moveServo(int angle) {
  angle = constrain(angle, 0, 180);  // Limita el rango
  servo.write(angle);
  currentAngle = angle;
}

// Abrir switch (ON)
void onSwitch() {
  servo.write(ANGLE_ON);
  currentAngle = ANGLE_ON;
  isServoOn = true;
}

// Cerrar switch (OFF)
void offSwitch() {
  servo.write(ANGLE_OFF);
  currentAngle = ANGLE_OFF;
  isServoOn = false;
}

// Alternar estado
void toggleSwitch() {
  if (isServoOn) {
    offSwitch();
  } else {
    onSwitch();
  }
}

// Obtener estado
bool getServoState() {
  return isServoOn;
}

// Obtener ángulo
int getServoAngle() {
  return currentAngle;
}

// Procesar comando Serial (devuelve true si se movió el servo)
bool servoProcessSerialCommand() {
  if (Serial.available() > 0) {
    int angle = Serial.parseInt();
    if (angle >= 0 && angle <= 180) {
      moveServo(angle);
      return true;
    }
    while (Serial.available() > 0) Serial.read();  // Limpiar buffer
  }
  return false;
}