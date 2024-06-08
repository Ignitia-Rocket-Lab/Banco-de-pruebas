void mossfetSetUp(){//Setup
  pinMode(13, OUTPUT);//Declarar pin 
}

void prender(){//Mandar voltaje para prender combustile

  const unsigned long ingTime = 500; // Tiempo de voltaje 0.5 seg
  unsigned long starIGNtMillis;  // Tiempo inicial
  unsigned long currenIGNtMillis; // Tiempo actual


  while(currenIGNtMillis <= starIGNtMillis + ingTime){
    currenIGNtMillis = millis();  // Actualiza el tiempo actual
    digitalWrite(13, HIGH);
  }

}