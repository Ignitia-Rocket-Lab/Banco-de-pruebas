#include <Servo.h>

// Crear un objeto Servo para controlar el servo
Servo myservo;

// Declarar la variable para almacenar la posición del servo
int pos = 0;

void setup() {
  // Adjuntar el servo al pin 9
  myservo.attach(9);
}

void loop() {
  // Girar el servo desde 0 hasta 180 grados

  myservo.write(pos);



}
