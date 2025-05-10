#include "RFController.h"

uint8_t payload[RF24_MAX_PAYLOAD_SIZE];
uint8_t payload_Size;

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
  printCommandMenu(); // Imprime el menú de comandos al inicio
}

void loop() {
  if (Serial.available() > 0) {
    int command = Serial.parseInt();
    processCommand(command);
    Serial.println();
    printCommandMenu(); // Vuelve a imprimir el menú
  }
  delay(100);
}

void processCommand(int command) {
  CommandCode commandToSend;
  bool success = false;

  switch (command) {
    case 0:
      commandToSend = CMD_GOTO_DIAG_WAIT;
      break;
    case 1:
      commandToSend = CMD_GOTO_ARMED;
      break;
    case 2:
      commandToSend = CMD_ABORT;
      break;
    case 3:
      commandToSend = CMD_REQUEST_DIAGNOSTICS;
      break;
    default:
      Serial.println(F("Invalid command. Please try again."));
      return;
  }

  success = sendRfCommand(commandToSend);

  if (success) {
    Serial.println(F("Command sent successfully. Waiting for ACK..."));
    delay(1000); // Simulate waiting for ACK (replace with proper ACK handling)
    Serial.println(F("ACK received (or timeout). Ready for next command."));
  } else {
    Serial.println(F("Failed to send command. Please try again."));
  }
}

void printCommandMenu() {
  Serial.println(F("--- Command Menu ---"));
  Serial.println(F("0: Go to DIAG WAIT"));
  Serial.println(F("1: Go to ARMED"));
  Serial.println(F("2: ABORT"));
  Serial.println(F("3: Request DIAGNOSTICS"));
  Serial.print(F("Enter command number: "));
}