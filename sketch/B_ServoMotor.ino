void servoMotorSetup(){

  myservo.attach(9);      
  myservo.write(pos);     //Muevo el motor al punto de inicio

}

void servoMover(bool accion){
  if (accion) {
    Serial.println("Apretar");
    /*
        for (pos = 0; pos <= 180; pos += 1) { // incrementa en pasos de 1 grado
          myservo.write(pos); // Mover el servo a la posición 'pos'
          delay(15);
        }
        */
    }

    if (!accion){
      Serial.println("Aflojar");

      /*
    // Girar el servo desde 180 hasta 0 grados
      for (pos = 180; pos >= 0; pos -= 1) { // decrementa en pasos de 1 grado
        myservo.write(pos); // Mover el servo a la posición 'pos'
        delay(15);
      }
      */
    }
}