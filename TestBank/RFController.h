#ifndef RFCONTROLLER_H
#define RFCONTROLLER_H

#include <Arduino.h>
#include <SPI.h>
#include <RF24.h>

// --- Configuración de Pines ---
#define CE_PIN  7
#define CSN_PIN 8

// Define el pin al que se conecta el pin IRQ de la antena
// Considerar un pin con capacidad de interrupción (p. ej., 2 o 3 en Arduino UNO/Nano)
#define IRQ_PIN 3

// --- Direcciones ---
// Direcciones para la comunicación. "TBdir" será la dirección de escucha del nodo 0
// y la dirección de escritura del nodo 1. "TRdir" será la dirección de escucha
// del nodo 1 y la dirección de escritura del nodo 0.
extern const byte addresses[2][6];

// --- Constantes ---
// Tamaño máximo del payload que este driver manejará al leer.
#define RF24_MAX_PAYLOAD_SIZE 8

// Selector del dispositivo para realizar las configuraciones adecuadas
#define TESTBANK 0
#define TRANSMITTER 1
#define RADIO_CHANNEL 76

// --- Tipos de Mensaje (Primer Byte del Payload) ---
enum MessageType : uint8_t {
  MSG_TYPE_INVALID       = 0x00, // Para indicar un error o no inicializado
  MSG_TYPE_STATE_UPDATE  = 0x01, // El remitente informa su estado FSM actual
  MSG_TYPE_DATA_REPORT   = 0x02, // El remitente envía un valor de datos/diagnóstico
  MSG_TYPE_COMMAND       = 0x03  // El remitente envía una orden al receptor
};

// --- Estados de la Máquina de Estados (FSM) ---
//     Asegurarse que estos valores coincidan EXACTAMENTE con tu enum State
//     en el código de la FSM principal.
enum FsmState : uint8_t {
  STATE_INIT          = 0, // Valor numérico de Init
  STATE_DIAG          = 1, // Valor numérico de Diag
  STATE_FAILDIAG      = 2, // Valor numérico de Fail Diagnostic
  STATE_IGNITION_WAIT = 3, // Valor numérico de IgnitionWait
  STATE_ARMED_WAIT    = 4, // Valor numérico de ArmedWait
  STATE_IGNITION      = 5, // Valor numérico de Ignition

  STATE_SUCESS        = 6,  // Valor numérico de Sucess
  STATE_END           = 7  // Valor numérico de Sucess
};

// --- Subtipos para DATA_REPORT (Segundo Byte si MSG_TYPE es DATA_REPORT) ---
enum DataSubtype : uint8_t {
  DATA_SUBTYPE_ADC_VALUE     = 0x10, // Siguientes 2 bytes son int16_t del ADC
  DATA_SUBTYPE_SD_STATUS     = 0x11, // Siguiente byte es estado SD (0=OK, 1=NoInit, 2=FileErr, etc.)
  DATA_SUBTYPE_DIAG_RESULT   = 0x12, // Siguiente byte es resultado Diag (0=Fail, 1=Pass)
};

// --- Códigos para COMMAND (Segundo Byte si MSG_TYPE es COMMAND) ---
enum CommandCode : uint8_t {
  CMD_GOTO_DIAG_WAIT        = 0xB0, // Comando para ir a DiagWait (si aplica)
  CMD_GOTO_ARMED            = 0xA0, // Comando para ir a ArmedWait (si aplica)
  CMD_ABORT                 = 0xA1, // Comando para Abortar y volver a IgnitionWait
  CMD_REQUEST_DIAGNOSTICS   = 0xB1, // Comando para solicitar un diagnóstico
  CMD_SUCCESS               = 0XC0, // Comando para terminar ACQ y pasar a Success
  CMD_END                   = 0xC1, // Comando para cerrar SD y apagar
};


// --- Variables Globales (Extern) ---
extern volatile bool rf24_data_available_flag;
extern uint8_t payload[RF24_MAX_PAYLOAD_SIZE];
extern uint8_t payload_size;


// --- Prototipos de Funciones del Driver ---

/**
 * @brief Inicializa y configura el módulo nRF24L01+.
 * Configura pines, direcciones, interrupción y pone el radio en modo escucha.
 * @param radioId TESTBANK (0) o TRANSMITTER (1) . Determina qué dirección usa
 *                para escuchar (pipe 1) y a cuál envía (writing pipe).
 *                TESTBANK escucha en "TBdir", envía a "TRdir".
 *                TRANSMITTER escucha en "TRdir", envía a "TBdir".
 * @param paLevel Nivel de potencia (RF24_PA_MIN, RF24_PA_LOW, RF24_PA_HIGH, RF24_PA_MAX). Default: RF24_PA_HIGH.
 * @param dataRate Velocidad de datos (RF24_1MBPS, RF24_2MBPS, RF24_250KBPS). Default: RF24_1MBPS.
 * @return True si la inicialización fue exitosa, False en caso contrario.
 */
bool setupRF24(bool radioId, rf24_pa_dbm_e paLevel = RF24_PA_HIGH, rf24_datarate_e dataRate = RF24_1MBPS);

/**
 * @brief Envía un payload de datos. Cambia a modo TX, envía y vuelve a modo RX.
 * @param data Puntero a los datos a enviar.
 * @param len Número de bytes a enviar (máximo 32).
 * @return True si el envío fue exitoso (ACK recibido), False en caso contrario.
 */
bool sendDataRF24(const void* data, uint8_t len);

/**
 * @brief Comprueba si hay datos nuevos disponibles (basado en la bandera de interrupción).
 * @return True si la bandera rf24_data_available_flag está activa, False si no.
 */
bool isDataAvailable();
//void sendACK();

/**
 * @brief Lee los datos recibidos y los guarda en el buffer global payload.
 *        Reinicia la bandera de datos disponibles.
 * @return El número de bytes leídos (guardado también en payload_size),
 *         o 0 si no había datos disponibles (la bandera estaba en false).
 */
uint8_t readDataSimpleRF24();

/**
 * @brief Función de manejo de interrupción para el pin IRQ del nRF24L01+.
 *        ¡NO LLAMAR DIRECTAMENTE! Se configura con attachInterrupt.
 */
void rf24InterruptHandler();

bool sendAdcData(int16_t adcValue);

bool sendSdStatus(uint8_t sdStatusCode);

bool sendDiagResult(bool pass);

bool sendRfCommand(CommandCode command);

bool sendStateUpdate(FsmState currentState);

void processReceivedMessage(uint8_t* payload, uint8_t len);

#endif //RFCONTROLLER_H