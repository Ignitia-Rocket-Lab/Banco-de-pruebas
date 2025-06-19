#include "RFController.h"

uint8_t payload[RF24_MAX_PAYLOAD_SIZE];
uint8_t payload_Size;
bool waitingForResponse = false; // Bandera para indicar si estamos esperando una respuesta
unsigned long responseTimeout = 0;  // Tiempo límite para la respuesta (en milisegundos)
const unsigned long RESPONSE_TIMEOUT_MS = 10000; // Por ejemplo, 10 segundos de timeout

// --- Configuración Inicial ---
void setup() {
  Serial.begin(115200);
  while (!Serial);
  Serial.println(F("Control Station Initializing..."));

  while (!setupRF24(TRANSMITTER, RF24_PA_MAX, RF24_1MBPS)) {
    Serial.println(F("Retrying RF..."));
    delay(1000);
  }
  Serial.println(F("RF24 setup complete."));
  printCommandMenu(); // Vuelve a imprimir el menú
}

void loop() {
  
  if (Serial.available() > 0) {
    int command = Serial.parseInt();
    sendCommand(command);
      // Manejar la recepción de mensajes y el timeout
    /*if (isDataAvailable()) {
      payload_Size = readDataSimpleRF24();
      processReceivedMessage(payload, payload_Size);
      waitingForResponse = false; // Ya no estamos esperando
    } else if (waitingForResponse && millis() - responseTimeout > RESPONSE_TIMEOUT_MS) {
      Serial.println(F("Response Timeout! No response received."));
      waitingForResponse = false; // Se acabó el tiempo de espera
    }*/
      Serial.println();
    }
}

void sendCommand(int command) {
  CommandCode commandToSend;
  bool success = false;

  switch (command) {
    case 0:
      commandToSend = CMD_GOTO_DIAG_WAIT;
      break;
    case 1:
      commandToSend = CMD_REQUEST_DIAGNOSTICS;
      break;
    case 2:
      commandToSend = CMD_GOTO_ARMED;
      break;
    case 3:
      commandToSend = CMD_ABORT;
      break;
    case 4: 
      commandToSend = CMD_SUCCESS;
      break; 
    case 5: 
      commandToSend = CMD_END;
      break; 
    default:
      Serial.println(F("Invalid command. Please try again."));
      return;
  }

  success = sendRfCommand(commandToSend);

  if (success) {
    Serial.println(F("Command sent successfully. Waiting for response..."));
    waitingForResponse = true;           // Esperando una respuesta
    responseTimeout = millis();          // Guarda el tiempo en que se envió el comando
  } else {
    Serial.println(F("Failed to send command. Please try again."));
  }
}

void printCommandMenu() {
  Serial.println(F("--- Command Menu ---"));
  Serial.println(F("0: Go to DIAG WAIT"));
  Serial.println(F("1: Request DIAGNOSTICS"));
  Serial.println(F("2: Go to ARMED WAIT"));
  Serial.println(F("3: Abort"));
  Serial.println(F("4: End TestBank ACQ"));
  Serial.println(F("5: END"));
  Serial.print(F("Enter command number: "));
}