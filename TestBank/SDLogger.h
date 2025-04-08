#ifndef SDLOGGER_H
#define SDLOGGER_H

#include <Arduino.h>
#include <SPI.h>
#include <SdFat.h>

// Definiciones
#define BUTTON_PIN 3
#define BUFFER_SIZE 256  // Tamaño del buffer

// Variables globales externas
extern SdFat SD;                   // Objeto de la tarjeta SD
extern File logFile;               // Archivo de log
extern char logBuffer[BUFFER_SIZE];// Buffer para almacenar logs en RAM
extern size_t bufferIndex;         // Índice actual del buffer
extern String logFilename;         // Nombre del archivo de log actual
extern unsigned long lastFlushTime;// Tiempo del último flush

// Inicializa la tarjeta SD
bool setupSD(int chipSelect);

// Genera un nombre de archivo único
void filenameGen();

// Inicializa el archivo de log
bool initFile(const String& header = "Time (ms), Value");

// Registra datos en el buffer
bool logToSD(const String& data);

// Registra medición específica (tiempo y valor)
void logMeasurement();

// Fuerza la escritura del buffer a la SD
bool flushBuffer();

// Cierra el archivo
bool closeSD();

#endif
