#include "SDLogger.h"
#include "ADCSetup.h"
#include <Arduino.h>
#include <SPI.h>

// Variables globales
SdFat SD;                         // Objeto de la tarjeta SD
File logFile;                     // Archivo de log
size_t bufferIndex = 0;           // Índice actual del buffer
char logBuffer[BUFFER_SIZE];      // Buffer para almacenar logs en RAM
String logFilename;               // Nombre del archivo de log actual
unsigned long lastFlushTime = 0;  // Tiempo del último flush


// Variables globales importadas de ADCSetup.h
extern int16_t adcMeasurement;
extern volatile bool conversionReady;
extern unsigned long startTime;
extern bool isTestRunning;

// Inicializa la tarjeta SD
bool setupSD(int chipSelect) {
    Serial.print("Initializing SD card...\t");
    delay(1000);

    if (!SD.begin(chipSelect)) {
        Serial.println("Initialization failed!");
        return false;
    }
    pinMode(BUTTON_PIN, INPUT);
    Serial.println("Initialization done.");
    bufferIndex = 0;  // Inicializar el índice del buffer
    lastFlushTime = millis(); // Inicializar el tiempo del último flush
    return true;
}

void filenameGen(void) {
    int counter = 1;
    String filename;
    do {
        filename = "log" + String(counter) + ".csv";
        counter++;
    } while (SD.exists(filename.c_str())); // Repite hasta encontrar un nombre que no exista
    logFilename = filename; // Almacena el nombre en la variable global
}

// Abre el archivo y lo inicializa
bool initFile(const String& header) {
    filenameGen();
    logFile = SD.open(logFilename, FILE_WRITE);
    
    if (!logFile) {
        Serial.println("Error al abrir el archivo");
        return false;
    }
    
    // Escribir encabezado
    logFile.println(header);
    logFile.flush(); // Asegurar que el encabezado se escriba
    
    Serial.print("Archivo abierto: ");
    Serial.println(logFilename);
    
    bufferIndex = 0; // Inicializar el índice del buffer
    lastFlushTime = millis(); // Inicializar el tiempo del último flush
    return true;
}

// Registra datos en el buffer
bool logToSD(const String& data) {
    if (!logFile) {
        Serial.println("Error: Archivo no está abierto.");
        return false;
    }
    
    // Convertir el dato a un arreglo de caracteres y añadirlo al buffer
    String dataWithNewline = data + "\n";
    dataWithNewline.toCharArray(logBuffer + bufferIndex, dataWithNewline.length() + 1);
    
    // Actualizar el índice del buffer
    bufferIndex += dataWithNewline.length();
    
    // Hora actual
    unsigned long currentTime = millis();
    
    // Hacer flush si:
    // 1. El buffer está casi lleno (dejando espacio para una entrada más) O
    // 2. Ha pasado al menos 1 segundo desde el último flush
    if (bufferIndex >= BUFFER_SIZE - 80 || (currentTime - lastFlushTime >= 1000)) {
        return flushBuffer();
    }
    
    return true;
}

void logMeasurement() {
    if (isTestRunning) {
        unsigned long elapsedTime = millis() - startTime;  // Tiempo transcurrido en milisegundos
        String logEntry = String(elapsedTime) + "," + String(WEIGHT(adcMeasurement)*9.81);//
        logToSD(logEntry);
        Serial.println(logEntry);  // Mostrar la entrada en el monitor serie
    }
}

// Fuerza la escritura del buffer a la SD
bool flushBuffer() {
    if (!logFile) {
        Serial.println("Error: Archivo no está abierto.");
        return false;
    }
    
    if (bufferIndex > 0) {
        logFile.write((const uint8_t*)logBuffer, bufferIndex);
        logFile.flush(); // Asegurar que los datos se escriban físicamente
        bufferIndex = 0; // Reiniciar el índice del buffer
        lastFlushTime = millis(); // Actualizar el tiempo del último flush
        return true;
    }
    
    // Aunque no haya nada que escribir, actualizamos el tiempo de último flush
    lastFlushTime = millis();
    return true; // Nada que escribir, pero no es un error
}

// Cierra el archivo
bool closeSD() {
    if (!logFile) {
        Serial.println("Error: No hay archivo para cerrar.");
        return false;
    }
    
    // Asegurarse de que cualquier dato en el buffer sea escrito antes de cerrar
    flushBuffer();
    
    logFile.close();
    Serial.println("Archivo cerrado.");
    return true;
}