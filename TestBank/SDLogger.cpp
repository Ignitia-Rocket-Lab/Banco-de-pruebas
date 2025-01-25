#include "SDLogger.h"
#include "ADCSetup.h"
#include <Arduino.h>
#include <SPI.h>

// Variables globales
File testBankLog;
String logFilename;

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
    return true;
    delay(100);
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
bool initFile(void) {
    filenameGen();
    testBankLog = SD.open(logFilename, FILE_WRITE);
    testBankLog.println("Time (ms), Force (N)");
    if (testBankLog) {
        Serial.print("Opened file: ");
        Serial.println(logFilename);
        return true;
    } else {
        Serial.println("Error opening file");
        return false;
    }
}

// Escribe datos en el archivo (con o sin salto de línea)
bool logToSD(const String& data, bool newline = false) {
    if (testBankLog) {
        newline ? testBankLog.println(data) : testBankLog.print(data);
        return true;
    } else {
        Serial.println("Error: File is not open.");
        return false;
    }
}

void logMeasurement() {
    if (isTestRunning) {
        unsigned long elapsedTime = millis() - startTime;  // Tiempo transcurrido en milisegundos
        String logEntry = String(elapsedTime) + "," + String(WEIGHT(adcMeasurement)*9.81);
        logToSD(logEntry, true);
        Serial.println(logEntry);  // Mostrar la entrada en el monitor serie
    }
}

// Cierra el archivo
bool closeSD() {
    if (testBankLog) {
        testBankLog.close();
        testBankLog = File(); // Reiniciar la referencia
        Serial.println("File closed.");
        return true;
    } else {
        Serial.println("Error: No file to close.");
        return false;
    }
}