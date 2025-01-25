#ifndef SDLOGGER_H
#define SDLOGGER_H

#include <Arduino.h>
#include <SPI.h>
#include <SD.h>

#define BUTTON_PIN 3

extern File testBankLog; // Archivo global externo
extern String logFilename; //Nombre del log actual

// Inicializa la tarjeta SD
bool setupSD(int chipSelect);

// Generar nombre del log
void filenameGen(void);

// Abre el archivo y lo inicializa
bool initFile(void);

// Escribe datos en el archivo (con o sin salto de línea)
bool logToSD(const String& data, bool newline = false);

// Cierra el archivo
bool closeSD(void);

void logMeasurement(void);

#endif
