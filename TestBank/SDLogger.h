#ifndef SDLOGGER_H
#define SDLOGGER_H

#include <Arduino.h>
#include <SPI.h>
#include <SD.h>

const uint8_t CS_pin = 10;   // Pin para chip select

extern File testBankLog; // Archivo global externo
extern String logFilename; //Nombre del log actual

// Inicializa la tarjeta SD
bool setupSD();

// Generar nombre del log
void filenameGen(void);

// Abre el archivo y lo inicializa
bool initFile(void);

// Escribe datos en el archivo (con o sin salto de línea)
bool logToSD(const String& data, bool newline = false);

// Cierra el archivo
bool closeSD(void);

#endif
