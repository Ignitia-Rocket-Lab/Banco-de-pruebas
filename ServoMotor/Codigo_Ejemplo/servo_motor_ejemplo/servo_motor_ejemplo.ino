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
  for (pos = 0; pos <= 180; pos += 1) { // incrementa en pasos de 1 grado
    myservo.write(pos); // Mover el servo a la posición 'pos'
    delay(15); // Esperar 15 ms para permitir que el servo alcance la posición
  }
  
  // Girar el servo desde 180 hasta 0 grados
  for (pos = 180; pos >= 0; pos -= 1) { // decrementa en pasos de 1 grado
    myservo.write(pos); // Mover el servo a la posición 'pos'
    delay(15); // Esperar 15 ms para permitir que el servo alcance la posición
    while(1){
      
    }
  }
}
