#include "RFController.h"

// --- Definición de Variables Globales ---

// Define las direcciones concretas
const byte rf24_addresses[2][6] = {"TBdir", "TRdir"};

// Crea la instancia del objeto RF24
RF24 radio(CE_PIN, CSN_PIN);

// Define la bandera de interrupción
volatile bool rf24_data_available_flag = false;

// Define el buffer de recepción y su tamaño
uint8_t rf24_received_payload[RF24_MAX_PAYLOAD_SIZE];
uint8_t rf24_received_payload_size = 0;

// --- Implementación de Funciones ---

/**
 * @brief Inicializa el modulo RF con las configuraciones básicas.
 */
bool setupRF24(bool radioId, rf24_pa_dbm_e paLevel, rf24_datarate_e dataRate) {
    Serial.println(F("Initializing RF24..."));

    // 1. Inicializar Radio
    if (!radio.begin()) {
        Serial.println(F("RF24 hardware is not responding!!"));
        return false;
    }

    // 2. Configurar parámetros básicos
    radio.setChannel(RADIO_CHANNEL);
    radio.setPALevel(paLevel);
    radio.setDataRate(dataRate);
    radio.setAutoAck(true); // Asegurarse de que el AutoAck esté habilitado
    radio.setCRCLength(RF24_CRC_16); // Usar CRC de 16 bits para mayor fiabilidad

    // 3. Configurar reintentos (opcional pero recomendado)
    radio.setRetries(5, 15); // delay: 5*250us = 1250us, count: 15 reintentos

    // 4. Configurar Direcciones (Pipes)
    //    radioId = TESTBANK (0) -> Escribe a "TRdir", Lee en "TBdir"
    //    radioId = TRANSMITTER(1) -> Escribe a "TBdir", Lee en "TRdir"
    radio.openWritingPipe(rf24_addresses[!radioId]); // Dirección a la que se envía
    radio.openReadingPipe(1, rf24_addresses[radioId]); // Dirección propia para escuchar (usamos pipe 1)

    // ¡Importante! No establecer un tamaño de payload fijo con radio.setPayloadSize()
    // si quieres recibir payloads de tamaño variable. Habilitar payloads dinámicos.
    radio.enableDynamicPayloads();

    // 5. Configurar Interrupción (IRQ)
    pinMode(IRQ_PIN, INPUT_PULLUP); // Configurar pin de IRQ como entrada con pull-up
    attachInterrupt(digitalPinToInterrupt(IRQ_PIN), rf24InterruptHandler, FALLING); // Adjuntar ISR

    // Enmascarar interrupciones: Queremos que IRQ se active SOLO en RX_DR (Recepción de Datos Lista)
    // maskIRQ(TX_DS_IRQ, TX_FAIL_IRQ, RX_DR_IRQ) -> 1=ignorar, 0=activar
    radio.maskIRQ(1, 1, 0);

    // 6. Empezar escuchando (Estado por defecto)
    radio.startListening();

    Serial.println(F("RF24 Initialized. In Listening Mode."));
    Serial.print(F(" - Radio ID: ")); Serial.println(radioId);
    Serial.print(F(" - Writing to: ")); Serial.println((char*)rf24_addresses[!radioId]);
    Serial.print(F(" - Listening on: ")); Serial.println((char*)rf24_addresses[radioId]);
    Serial.print(F(" - Channel: ")); Serial.println(radio.getChannel());
    Serial.print(F(" - IRQ Pin: ")); Serial.println(IRQ_PIN);
    // radio.printDetails(); // Descomentar para debugging detallado

    return true;
}

/**
 * @brief Envía un payload de datos. Cambia a modo TX, envía y vuelve a modo RX.
 */
bool sendDataRF24(const void* data, uint8_t len) {
    // 1. Salir del modo escucha
    radio.stopListening();

    // 2. Enviar los datos. radio.write() devuelve true si se recibió ACK.
    bool report = radio.write(data, len);

    // 3. Volver al modo escucha inmediatamente después de enviar
    radio.startListening();

    // 4. Informar del resultado
    if (report) {
        //Serial.print(F("-> Send OK. Payload Size: ")); Serial.println(len);
    } else {
        Serial.println(F("-> Send Failed (No ACK)."));
        // *** Futura implementacion de lógica para reintentar o manejar el fallo. ***
    }
    return report;
}

/**
 * @brief Comprueba si hay datos nuevos disponibles (basado en la bandera de interrupción).
 */
bool isDataAvailable() {
    // Simplemente devuelve el estado de la bandera que se activa en la ISR
    return rf24_data_available_flag;
}

/**
 * @brief Lee los datos recibidos y los guarda en el buffer global rf24_received_payload.
 *        Reinicia la bandera de datos disponibles.
 */
uint8_t readDataSimpleRF24() {
    // Solo intentar leer si la bandera de la ISR está activa
    if (rf24_data_available_flag) {

        // Comprobar si realmente hay datos en el FIFO del radio
        if (radio.available()) {
            // Obtener el tamaño del payload dinámico que acaba de llegar
            uint8_t payloadSize = radio.getDynamicPayloadSize();

            // Comprobar si el tamaño es válido y cabe en nuestro buffer
            if (payloadSize > 0 && payloadSize <= RF24_MAX_PAYLOAD_SIZE) {
                // Leer el payload en el buffer global
                radio.read(&rf24_received_payload, payloadSize);
                // Guardar el tamaño leído en la variable global
                rf24_received_payload_size = payloadSize;

                // ¡Importante! Reiniciar la bandera AHORA que los datos han sido leídos
                rf24_data_available_flag = false;

                // Devolver el número de bytes leídos
                return rf24_received_payload_size;

            } else if (payloadSize > RF24_MAX_PAYLOAD_SIZE) {
                // El payload es demasiado grande para nuestro buffer!
                Serial.print(F("Error: Payload received ("));
                Serial.print(payloadSize);
                Serial.print(F(" bytes) larger than buffer ("));
                Serial.print(RF24_MAX_PAYLOAD_SIZE);
                Serial.println(F(" bytes). Flushing RX FIFO."));
                // Limpiar el FIFO para evitar problemas. Perdemos este paquete.
                radio.flush_rx();
                rf24_received_payload_size = 0; // Indicar que no hay datos válidos
                rf24_data_available_flag = false; // Reiniciar bandera
                return 0;
            } else {
                // getDynamicPayloadSize() devolvió 0 o un valor inválido.
                // Esto no debería pasar si radio.available() era true, pero por si acaso.
                Serial.println(F("Warning: radio.available() true but getDynamicPayloadSize() <= 0."));
                rf24_received_payload_size = 0;
                rf24_data_available_flag = false; // Reiniciar bandera
                return 0;
            }
        } else {
            // La bandera estaba activa, pero radio.available() es falso.
            // Esto podría pasar si la ISR se disparó pero los datos se leyeron muy rápido
            // o hubo algún problema. Solo reseteamos la bandera.
             Serial.println(F("Warning: ISR flag set, but no data available in FIFO now. Resetting flag."));
             rf24_data_available_flag = false;
             rf24_received_payload_size = 0;
             return 0;
        }

    } // Fin de if (rf24_data_available_flag)

    // Si la bandera no estaba activa, no hay nada que hacer
    return 0;
}

/**
 * @brief Función de manejo de interrupción para el pin IRQ del nRF24L01+.
 *        ¡NO LLAMAR DIRECTAMENTE! Se configura con attachInterrupt.
 *        Esta función debe ser lo más RÁPIDA posible.
 */
void rf24InterruptHandler() {
    // Variables para almacenar el estado leído del radio
    bool tx_ok, tx_fail, rx_ready;

    // radio.whatHappened() lee el registro STATUS del nRF24,
    // determina qué causó la interrupción (TX ok, TX fail, RX ready)
    // y ¡MUY IMPORTANTE! limpia (resetea) las banderas de interrupción en el chip.
    radio.whatHappened(tx_ok, tx_fail, rx_ready);

    // Como hemos enmascarado las interrupciones de TX_OK y TX_FAIL en setupRF24,
    // teóricamente solo deberíamos recibir interrupciones por RX_READY.
    // Pero comprobamos por si acaso.
    if (rx_ready) {
        // Hay datos nuevos esperando en el FIFO de recepción.
        // Activamos nuestra bandera global para que el loop principal sepa que hay que leer.
        // NO leemos los datos aquí dentro de la ISR.
        rf24_data_available_flag = true;

        // Opcional: Se puede añadir un contador aquí para depurar cuántas veces se llama la ISR
        // static volatile uint32_t isr_count = 0;
        // isr_count++;
    }

    // Si por alguna razón se activara una interrupción de TX (aunque estén enmascaradas),
    // whatHappened() ya las habría limpiado. No necesitamos hacer nada más aquí para TX.
    // Evitar poner Serial.print() u otras operaciones lentas dentro de una ISR.
}

/** @brief Envía una lectura del ADC. */
bool sendAdcData(int16_t adcValue) {
  uint8_t payload[1 + 1 + sizeof(int16_t)]; // Tipo + Subtipo + Datos
  payload[0] = MSG_TYPE_DATA_REPORT;
  payload[1] = DATA_SUBTYPE_ADC_VALUE;
  memcpy(&payload[2], &adcValue, sizeof(int16_t)); // Copia los bytes del int16
  Serial.print(F("Data Report: ADC=")); Serial.println(adcValue);
  return sendDataRF24(payload, sizeof(payload));
}

/** @brief Envía el estado de la SD (simplificado). */
bool sendSdStatus(uint8_t sdStatusCode) {
  uint8_t payload[3]; // Tipo + Subtipo + Status
  payload[0] = MSG_TYPE_DATA_REPORT;
  payload[1] = DATA_SUBTYPE_SD_STATUS;
  payload[2] = sdStatusCode; // 0=OK, 1=Error, etc.
  Serial.print(F("Data Report: SD Status=")); Serial.println(sdStatusCode);
  return sendDataRF24(payload, sizeof(payload));
}

/** @brief Envía el resultado del diagnóstico general. */
bool sendDiagResult(bool pass) {
    uint8_t payload[3]; // Tipo + Subtipo + Resultado
    payload[0] = MSG_TYPE_DATA_REPORT;
    payload[1] = DATA_SUBTYPE_DIAG_RESULT;
    payload[2] = (pass ? 1 : 0);
    Serial.print(F("Data Report: Diag Result=")); Serial.println(pass ? "PASS" : "FAIL");
    return sendDataRF24(payload, sizeof(payload));
}


/** @brief Envía un comando específico. */
bool sendRfCommand(CommandCode command) {
  uint8_t payload[2];
  payload[0] = MSG_TYPE_COMMAND;
  payload[1] = command;
  Serial.print(F("Command: ")); Serial.println(command, HEX);
  return sendDataRF24(payload, sizeof(payload));
}

/** @brief Envía el estado actual de la FSM. */
bool sendStateUpdate(FsmState currentState) {
  uint8_t payload[2];
  payload[0] = MSG_TYPE_STATE_UPDATE;
  payload[1] = currentState; // El valor numérico del estado
  Serial.print(F("State Update: FSM State=")); Serial.println(currentState);
  return sendDataRF24(payload, sizeof(payload));
}

void processReceivedMessage(uint8_t* payload, uint8_t len) {
  if (len == 0 || payload == nullptr) {
    Serial.println(F("RF RX: Intento de procesar mensaje vacío/nulo."));
    return;
  }

  MessageType msgType = (MessageType)payload[0];
  Serial.print(F("RF RX: Msg Type=0x")); Serial.print(msgType, HEX);
  Serial.print(F(" Len=")); Serial.println(len);

  switch (msgType) {
    case MSG_TYPE_STATE_UPDATE:
      if (len == 2) {
        FsmState receivedState = (FsmState)payload[1];
        Serial.print(F("  State Update Received: ")); Serial.println(receivedState);
        // Actualizar alguna variable local que rastree el estado del otro dispositivo
        // displayRemoteState(receivedState); // Función ficticia
      } else {
        Serial.println(F("  Error: Tamaño incorrecto para State Update."));
      }
      break;

    case MSG_TYPE_DATA_REPORT:
      if (len >= 2) { // Necesita al menos Tipo + Subtipo
        DataSubtype subType = (DataSubtype)payload[1];
        Serial.print(F("  Data Report Received. Subtype=0x")); Serial.println(subType, HEX);
        switch (subType) {
          case DATA_SUBTYPE_ADC_VALUE:
            if (len == (1 + 1 + sizeof(int16_t))) { // Tipo+Subtipo+int16
              int16_t adcValue;
              memcpy(&adcValue, &payload[2], sizeof(int16_t));
              Serial.print(F("    ADC Value: ")); Serial.println(adcValue);
              // Usa el valor ADC recibido
            } else {
               Serial.println(F("    Error: Tamaño incorrecto para ADC Value."));
            }
            break;
          case DATA_SUBTYPE_SD_STATUS:
             if (len == 3) { // Tipo+Subtipo+Status(1 byte)
                uint8_t sdStatus = payload[2];
                Serial.print(F("    SD Status Code: ")); Serial.println(sdStatus);
                // Interpreta el código de estado SD
             } else {
                Serial.println(F("    Error: Tamaño incorrecto para SD Status."));
             }
             break;
           case DATA_SUBTYPE_DIAG_RESULT:
             if (len == 3) { // Tipo+Subtipo+Resultado(1 byte)
                bool diagPassed = (payload[2] == 1);
                Serial.print(F("    Diag Result: ")); Serial.println(diagPassed ? "PASS" : "FAIL");
                // Actúa según el resultado del diagnóstico remoto
             } else {
                Serial.println(F("    Error: Tamaño incorrecto para Diag Result."));
             }
             break;
          // Añade cases para otros DATA_SUBTYPE
          default:
            Serial.println(F("    Error: Subtipo de datos desconocido."));
            break;
        }
      } else {
         Serial.println(F("  Error: Tamaño insuficiente para Data Report."));
      }
      break;

    case MSG_TYPE_COMMAND:
      if (len == 2) {
        CommandCode receivedCommand = (CommandCode)payload[1];
        Serial.print(F("  Command Received: 0x")); Serial.println(receivedCommand, HEX);
        // Actuar según el comando recibido
        switch(receivedCommand) {
            case CMD_GOTO_ARMED:
                Serial.println(F("    Executing: Go to Armed Wait"));
                // Llama a la función que cambia el estado local si es aplicable
                // transitionToState(STATE_ARMED_WAIT); // Función ficticia
                break;
            case CMD_ABORT:
                Serial.println(F("    Executing: Abort to Ignition Wait"));
                // transitionToState(STATE_IGNITION_WAIT); // Función ficticia
                break;
            case CMD_REQUEST_DIAGNOSTICS:
                 Serial.println(F("    Executing: Run Diagnostics on Request"));
                 // bool diagRes = performDiagnostics(); // Ejecuta diag localmente
                 // sendDiagResult(diagRes); // Envía el resultado de vuelta
                 break;
            // Añade cases para otros CommandCode
            default:
                 Serial.println(F("    Warning: Comando desconocido recibido."));
                 break;
        }
      } else {
        Serial.println(F("  Error: Tamaño incorrecto para Command."));
      }
      break;

    default:
      Serial.println(F("  Error: Tipo de mensaje desconocido o inválido."));
      // Podrías imprimir los bytes recibidos en HEX para depurar
      Serial.print(F("    Raw Payload (HEX): "));
      for(int i=0; i<len; i++){
          if(payload[i] < 0x10) Serial.print("0");
          Serial.print(payload[i], HEX);
          Serial.print(" ");
      }
      Serial.println();
      break;
  }
}