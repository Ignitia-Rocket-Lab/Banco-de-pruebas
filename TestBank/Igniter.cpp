#include "Igniter.h"
#include "ServoSwitch.h"

bool  currentIgniterState;

// Inicializa los pines necesarios para el ignitor 
void setupIgniter(){
<<<<<<< Updated upstream
  pinMode(IGNITER, OUTPUT);
  digitalWrite(IGNITER, LOW);
  
=======
  Serial.print(F("setupIGNITER: Initializing igniter... ")); delay(100);
  pinMode(IGNITER, OUTPUT);
  digitalWrite(IGNITER, LOW);
  Serial.println(F("Success")); delay(100);
>>>>>>> Stashed changes
} 

// Enciende el cerillo electronico
void igniteMotor(){
  digitalWrite(IGNITER, HIGH);
  currentIgniterState == true;
}; 

// Gira el servomotor para habilitar el ignitor
void prepareIgniter(){
  onSwitch(); // Usa la función del servo para habilitar la linea del cable
}; 

// Apaga la señal del pin y deshabilita la linea del cable
void resetIgniterSafeState(){
  digitalWrite(IGNITER, LOW);
  currentIgniterState = false;
  offSwitch();
}; 

// El usuario puede obtener el estado del pin
bool getMosfetState(){
  return currentIgniterState;
}; 