#include "RFController.h"

uint8_t payload[RF24_MAX_PAYLOAD_SIZE];
uint8_t payload_Size;
bool waitingForResponse = false; // Bandera para indicar si estamos esperando una respuesta
unsigned long responseTimeout = 0;  // Tiempo límite para la respuesta (en milisegundos)
const unsigned long RESPONSE_TIMEOUT_MS = 10000; // Por ejemplo, 10 segundos de timeout

// botones
const int botones[6] = {2, 4, 5, 6, 7, 8};
bool estadoAnterior[6] = {HIGH, HIGH, HIGH, HIGH, HIGH, HIGH};


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

  // Configura los pines de los botones como entrada con resistencia pull-up interna
  for (int i = 0; i < 6; i++) {
    pinMode(botones[i], INPUT_PULLUP);
  }

  
}

void loop() {
  
  for (int i = 0; i < 6; i++) {
    int estado = digitalRead(botones[i]);

    if (estado == LOW && estadoAnterior[i] == HIGH) {
      estadoAnterior[i] = LOW;

      char mensaje[12];
      sprintf(mensaje, "BOTON_%d", i);

      Serial.print("Enviado: ");
      Serial.println(mensaje);

      sendCommand(i);  // <- Aquí envías el número del botón presionado
      delay(200); // Antirebote
    }

    if (estado == HIGH) {
      estadoAnterior[i] = HIGH;
    }
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