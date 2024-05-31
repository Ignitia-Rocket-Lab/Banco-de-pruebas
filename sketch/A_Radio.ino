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
    memcpy(payload.message, "Recivido ", 9);  // set the payload message
    // load the payload for the first received transmission on pipe 0
    radio.writeAckPayload(1, &payload, sizeof(payload));
    radio.startListening();  // put radio in RX mode
    //Serial.println("Receptor");
  }
}

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
    Serial.print(F(" Sent: "));
    Serial.print(payload.message);    // print outgoing message
    Serial.println(payload.counter);  // print outgoing counter

    // save incoming counter & increment for next outgoing
    payload.counter = received.counter + 1;
    // load the payload for the first received transmission on pipe 0
    radio.writeAckPayload(1, &payload, sizeof(payload));
    
    if (strcmp(received.message, "Activar") == 0) {
      Serial.println("Activar->");
      arduinos = true;

      /*
      for (pos = 0; pos <= 180; pos += 1) { // incrementa en pasos de 1 grado
        myservo.write(pos); // Mover el servo a la posición 'pos'
        delay(15);
      }
      */
    }

    if (strcmp(received.message, "Desactivar") == 0){
    // Girar el servo desde 180 hasta 0 grados
      Serial.println("DesActivar->");

    /*
      for (pos = 180; pos >= 0; pos -= 1) { // decrementa en pasos de 1 grado
        myservo.write(pos); // Mover el servo a la posición 'pos'
        delay(15);
      }
      */
    }

  }
}

void mandarDatos(){
  Serial.println("Que mensaje deseas mandar : ");
  
  char inputString[10]; // Creamos un array para almacenar el mensaje recibido
    while (!Serial.available()) {
      // wait for user input
    }

    // Lee la cadena de caracteres desde el puerto serial hasta encontrar un salto de línea ('\n')
    int bytesRead = Serial.readBytesUntil('\n', inputString, sizeof(inputString));
    // Agrega un carácter nulo al final de la cadena para formar una cadena de caracteres válida
    inputString[bytesRead] = '\0';
      // Copiar el mensaje recibido en la estructura PayloadStruct
    memcpy(payload.message, inputString, sizeof(payload.message));

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

    // to make this example readable in the serial monitor
    delay(2000);  // slow transmissions down by 1 second

}