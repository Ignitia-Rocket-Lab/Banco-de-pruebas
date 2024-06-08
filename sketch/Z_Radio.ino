void radio_Setup(bool radioNumber){ //Set up del radio
    // initialize the transceiver on the SPI bus
  if (!radio.begin()) {
    Serial.println(F("radio hardware is not responding!!"));
    while (1) {}  // hold in infinite loop
  }
  // Set the PA Level low to try preventing power supply related problems
  // because these examples are likely run with nodes in close proximity to
  // each other.
  radio.setPALevel(RF24_PA_LOW);  // RF24_PA_MAX is default.

  // to use ACK payloads, we need to enable dynamic payload lengths (for all nodes)
  radio.enableDynamicPayloads();  // ACK payloads are dynamically sized

  // Acknowledgement packets have no payloads by default. We need to enable
  // this feature for all nodes (TX & RX) to use ACK payloads.
  radio.enableAckPayload();

  // set the TX address of the RX node into the TX pipe
  radio.openWritingPipe(address[radioNumber]);  // always uses pipe 0

  // set the RX address of the TX node into a RX pipe
  radio.openReadingPipe(1, address[!radioNumber]);  // using pipe 1

  // additional setup specific to the node's role
  if (radioNumber) {
    // setup the TX payload
    memcpy(payload.message, "Hello ", 6);  // set the payload message
    radio.stopListening();                 // put radio in TX mode
    //Serial.println("Transmisor");
  } else {
    // setup the ACK payload & load the first response into the FIFO
    memcpy(payload.message, "World  ", 6);  // set the payload message
    // load the payload for the first received transmission on pipe 0
    radio.writeAckPayload(1, &payload, sizeof(payload));
    radio.startListening();  // put radio in RX mode
    //Serial.println("Receptor");
  }
}//Setup de la radio

bool recivirDatos(){
  uint8_t pipe;

  if (radio.available(&pipe)) {                     // is there a payload? get the pipe number that recieved it
    uint8_t bytes = radio.getDynamicPayloadSize();  // get the size of the payload
    PayloadStruct received;
    radio.read(&received, sizeof(received));  // get incoming payload
    Serial.print(F("Received "));
    Serial.print(bytes);  // print the size of the payload
    Serial.print(F(" bytes on pipe "));
    Serial.print(pipe);  // print the pipe number
    Serial.print(F(": "));
    Serial.print(received.message);  // print incoming message
    Serial.print("-");
    Serial.print(received.counter);  // print incoming counter
        
    arduinos = procesarRecepcionDatos(received);//Procesar el mensaje recivido

    Serial.print(F(" Sent: "));
    Serial.print(payload.message);    // print outgoing message
    Serial.println(payload.counter);  // print outgoing counter

    // save incoming counter & increment for next outgoing
    payload.counter = received.counter + 1;

  }
  return arduinos;
}//Recepcion de paquetes de datos

bool procesarRecepcionDatos(PayloadStruct payload){
  //Aqui se procesara el mensaje que se envia

  if (strcmp(payload.message, "Activ ") == 0) {
    //Activa el switch fisico
    Serial.println("Activar->");
    memcpy(payload.message, "ActSW ", 6);
    servoMover(true);
    
  } else if (strcmp(payload.message, "Desac ") == 0){
  // Desactiva el switch fisico
    Serial.println("DesActivar->");
    memcpy(payload.message, "DesSW ", 6);
    servoMover(false);

  }else if (strcmp(payload.message, "Prend ") == 0){
  // Suelta el voltaje
    Serial.println("Prender motor->");
    memcpy(payload.message, "FUEGO ", 6);

  }else if (strcmp(payload.message, "Cambi ") == 0){
  // Girar el servo desde 180 hasta 0 grados
    Serial.println("Cambair TX/RX->");
    memcpy(payload.message, "Chang ", 6);
    empezarEnviarDatos();
    radio.writeAckPayload(1, &payload, sizeof(payload));
    return true;

  } else{
    //El mensaje captado no tiene respuesta programada, el mensaje se corrompio o no se tiene respuesta
    Serial.println("Mensaje sin respuesta disponible");
    memcpy(payload.message, "NONME ", 6);

  }

  radio.writeAckPayload(1, &payload, sizeof(payload));
  return false;
}//Procesar datos recividos

void mandarDatos(){
  //Serial.println(payload.message);
    
  unsigned long start_timer = micros();                  // start the timer
  bool report = radio.write(&payload, sizeof(payload));  // transmit & save the report
  unsigned long end_timer = micros();                    // end the timer

  if (report) {
    Serial.print(F("Transmission successful! "));  // payload was delivered
    Serial.print(F("Time to transmit = "));
    Serial.print(end_timer - start_timer);  // print the timer result
    Serial.print(F(" us. Sent: "));
    Serial.print(payload.message);  // print the outgoing message
    Serial.println("");
    Serial.print(payload.counter);  // print the outgoing counter
    uint8_t pipe;
    if (radio.available(&pipe)) {  // is there an ACK payload? grab the pipe number that received it
      PayloadStruct received;
      radio.read(&received, sizeof(received));  // get incoming ACK payload
      Serial.print(F(" Recieved "));
      Serial.print(radio.getDynamicPayloadSize());  // print incoming payload size
      Serial.print(F(" bytes on pipe "));
      Serial.print(pipe);  // print pipe number that received the ACK
      Serial.print(F(": "));
      Serial.print(received.message);    // print incoming message
      Serial.println(received.counter);  // print incoming counter

      // save incoming counter & increment for next outgoing
      payload.counter = received.counter + 1;

    } else {
      Serial.println(F(" Recieved: an empty ACK packet"));  // empty ACK packet received
    }

  } else {
    Serial.println(F("Transmission failed or timed out"));  // payload was not delivered
  }
  //delay(1000);  // slow transmissions down by 1 second

}//Mandar Datos

void confirmarMensaje(){
  memcpy(payload.message, "Envio ", 6);  // change payload message
  mandarDatos();
}

void procesarEnvioDatos(int optMenu){//Mandadr mensajes personalizados

  bool mensajeCorrecto = false;

  Serial.println("Que mensaje deseas mandar : ");
  switch(optMenu) {
    case 3:
      Serial.println("Activar || Desactivar");
      break;
    case 4:
      Serial.println("Prender");
    case 7:
      Serial.println("Cambiar");
  }
  
  while (!Serial.available()) {
    // wait for user input
  }

  // Lee la cadena de caracteres desde el puerto serial hasta encontrar un salto de línea ('\n')
  char inputString[15]; // Creamos un array para almacenar el mensaje recibido
  for (int i = 0; i < 15; i++) {//Limpiamos el array
      inputString[i] = '\0';
  }

  int bytesRead = Serial.readBytesUntil('\n', inputString, sizeof(inputString));

  switch(optMenu){
    case 3: //Switch fisico
      if(strcmp(inputString, "Activar") == 0 ){//Si es 0 el mensaje deja de mandar mensajes aqui
        Serial.println("Se detecto Activar");
        strcpy(payload.message, "Activ ");
        mensajeCorrecto = true;

      } else if(strcmp(inputString, "Desactivar") == 0 ){//Si es 0 el mensaje deja de mandar mensajes aqui
        Serial.println("Se detecto Desactivar");
        strcpy(payload.message, "Desac ");
        mensajeCorrecto = true;

      } else{
        Serial.print("Este mensaje '"); 
        Serial.print(inputString);
        Serial.print("' no esta preconfigurado.");
        Serial.println("");
      }

      break;
    case 4:
      if(strcmp(inputString, "Prender") == 0 ){//Si es 0 el mensaje deja de mandar mensajes aqui
        Serial.println("Se detecto prender motor");
        strcpy(payload.message, "Prend ");
        mensajeCorrecto = true;
      } else {
        Serial.print("Este mensaje "); 
        Serial.print(inputString);
        Serial.print(" no esta preconfigurado.");
        Serial.println("");
      }
      break;
    
    case 7:
      if(strcmp(inputString, "Cambiar") == 0 ){//Si es 0 el mensaje deja de mandar mensajes aqui
        Serial.println("Se detecto Cambiar");
        strcpy(payload.message, "Cambi ");
        mensajeCorrecto = true;
      } else {
        Serial.print("Este mensaje "); 
        Serial.print(inputString);
        Serial.print(" no esta preconfigurado.");
        Serial.println("");
      }
      break;
  }

  if(mensajeCorrecto){
    
    mandarDatos();
    confirmarMensaje();
    mensajeCorrecto = false;

  }

  delay(1000);

  //}
  
}//Checar que los datos a enviar concuerdan con la funcion seleccionada

void empezarEnviarDatos(){
  Serial.println("Empezar configuracion para mandar datos");

  memcpy(payload.message, "Hello ", 6);  // change payload message
  radio.stopListening();                 // this also discards any unused ACK payloads

  arduinos = !arduinos;
}//Configuracion para enviar datos

void empezarEscucharDatos(){
  Serial.println(F("Empezar configuracion para recibir datos"));
  memcpy(payload.message, "World ", 6);  // change payload message

  // load the payload for the first received transmission on pipe 0
  radio.writeAckPayload(1, &payload, sizeof(payload));
  radio.startListening();

  arduinos = !arduinos;
  input = "0";

}//Configuracion para enviar datos