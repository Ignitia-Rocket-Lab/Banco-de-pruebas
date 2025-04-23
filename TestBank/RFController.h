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
// Direcciones para la comunicación. "1Node" será la dirección de escucha del nodo 0
// y la dirección de escritura del nodo 1. "2Node" será la dirección de escucha
// del nodo 1 y la dirección de escritura del nodo 0.
extern const byte addresses[2][6];

// --- Constantes ---
// Tamaño máximo del payload que este driver manejará al leer.
#define RF24_MAX_PAYLOAD_SIZE 8

// Selector del dispositivo para realizar las configuraciones adecuadas
#define TESTBANK 0
#define TRANSMITTER 1
#define CURRENT_DEVICE TESTBANK
#define RADIO_CHANNEL 76

// --- Variables Globales del Driver ---
extern RF24 radio; // El objeto principal de la librería RF24

// Bandera de interrupción: se pone a true cuando llega un dato nuevo.
// Debe ser revisada en el loop() principal.
extern volatile bool rf24_data_available_flag;

// Buffer para almacenar el último payload recibido.
extern uint8_t rf24_received_payload[RF24_MAX_PAYLOAD_SIZE];

// Tamaño del último payload recibido.
extern uint8_t rf24_received_payload_size;

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

/**
 * @brief Lee los datos recibidos y los guarda en el buffer global rf24_received_payload.
 *        Reinicia la bandera de datos disponibles.
 * @return El número de bytes leídos (guardado también en rf24_received_payload_size),
 *         o 0 si no había datos disponibles (la bandera estaba en false).
 */
uint8_t readDataSimpleRF24();

/**
 * @brief Función de manejo de interrupción para el pin IRQ del nRF24L01+.
 *        ¡NO LLAMAR DIRECTAMENTE! Se configura con attachInterrupt.
 */
void rf24InterruptHandler();


#endif //RFCONTROLLER_H