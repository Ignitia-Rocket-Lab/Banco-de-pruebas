// ----- SDLogger.h -----
// Driver para gestionar el registro de datos en una tarjeta SD usando SdFat,
// con un sistema de buffer en RAM para optimizar escrituras.

#ifndef SDLOGGER_H
#define SDLOGGER_H

#include <Arduino.h>
#include <SPI.h>      // Necesario para la comunicación con la SD
#include <SdFat.h>    // Librería principal para el manejo de la SD

// --- Configuraciones y Constantes ---

/**
 * @brief Pin al que está conectado un botón (iniciar/detener logging o pruebas).
 *        Se configura como INPUT en setupSD. Su lógica de uso dependerá del sketch principal.
 */
#define BUTTON_PIN 5    // O el pin que estés usando

/**
 * @brief Tamaño (en bytes) del buffer en RAM utilizado para acumular datos antes de escribirlos en la SD.
 *        Un buffer más grande reduce la frecuencia de escrituras a la SD (bueno para la vida útil de la SD y velocidad),
 *        pero aumenta el riesgo de pérdida de datos si se corta la energía antes de un flush.
 *        IMPORTANTE!!!! Incrementar el tamaño del buffer, tambien puede consumir gran parte de la memoria del microcontrolador.
 */
#define BUFFER_SIZE 256

// --- Variables Globales del Driver (declaradas como extern) ---
//     La definición real de estas variables se encuentra en SDLogger.cpp

/**
 * @brief Objeto principal de la librería SdFat. Maneja la comunicación de bajo nivel con la tarjeta SD.
 */
extern SdFat SD;

/**
 * @brief Objeto File que representa el archivo de log actualmente abierto en la tarjeta SD.
 *        Todas las operaciones de escritura se realizan a través de este objeto.
 */
extern File logFile;

/**
 * @brief Buffer en memoria RAM para almacenar temporalmente las entradas de log antes de escribirlas
 *        en el archivo de la tarjeta SD (logFile). Su tamaño está definido por BUFFER_SIZE.
 */
extern char logBuffer[BUFFER_SIZE];

/**
 * @brief Índice que apunta a la siguiente posición libre dentro de logBuffer.
 *        Indica cuántos bytes están actualmente ocupados en el buffer. Se reinicia a 0 después de cada flush.
 */
extern size_t bufferIndex;

/**
 * @brief Almacena el nombre completo (String) del archivo de log actual (p. ej., "log1.csv").
 *        Generado por filenameGen() y utilizado por initFile() y potencialmente otras funciones.
 */
extern String logFilename;

/**
 * @brief Marca de tiempo (en milisegundos, usando millis()) de la última vez que el buffer
 *        fue volcado (flushed) al archivo en la tarjeta SD. Utilizado para forzar flushes periódicos.
 */
extern unsigned long lastFlushTime;

// --- Prototipos de Funciones del Driver ---

/**
 * @brief Inicializa la comunicación con la tarjeta SD.
 * @param chipSelect El pin de Arduino conectado al pin CS (Chip Select) de la tarjeta SD.
 * @return true si la inicialización de la tarjeta SD fue exitosa, false en caso contrario.
 */
bool setupSD(void);

/**
 * @brief Genera un nombre de archivo único para el log en formato "logX.csv",
 *        donde X es un número entero incremental. Busca el primer número X tal que
 *        "logX.csv" no exista en la tarjeta SD. Almacena el nombre generado en la variable global logFilename.
 * @param None
 * @return None
 */
void filenameGen();

/**
 * @brief Abre (o crea si no existe) el archivo de log en la tarjeta SD con el nombre generado
 *        por filenameGen(). Escribe una línea de encabezado opcional en el archivo.
 * @param header Una cadena (String) que se escribirá como la primera línea del archivo (típicamente encabezados CSV).
 *               Por defecto es "Time (ms), Value".
 * @return true si el archivo se abrió/creó y el encabezado se escribió correctamente, false si hubo un error.
 */
bool initFile(const String& header = "Time (ms), Value");

/**
 * @brief Agrega una cadena de datos al buffer de RAM (logBuffer).
 *        Automáticamente añade un carácter de nueva línea ('\n') al final de la cadena.
 *        Si el buffer se llena lo suficiente o ha pasado suficiente tiempo (1 segundo),
 *        llamará automáticamente a flushBuffer() para escribir los datos en la SD.
 * @param data La cadena (String) que se desea registrar en el log.
 * @return true si los datos se agregaron al buffer (y potencialmente se hizo flush), false si el archivo no estaba abierto.
 */
bool logToSD(const String& data);

/**
 * @brief Formatea y registra una medición específica, usando variables globales del ADC y de tiempo.
 *        Asume que las variables `isTestRunning`, `startTime` y `adcMeasurement` (y la macro `WEIGHT`)
 *        están disponibles (definidas y actualizadas en el módulo ADCSetup).
 *        Calcula el tiempo transcurrido desde `startTime` y formatea una línea "ElapsedTime,CalculatedWeight".
 *        Luego llama a logToSD() para agregar esta línea al buffer.
 * @param None (depende de variables globales externas)
 * @return None
 */
void logMeasurement();

/**
 * @brief Escribe forzosamente el contenido actual del buffer (logBuffer) al archivo en la tarjeta SD (logFile).
 *        Esta función se llama automáticamente por logToSD() bajo ciertas condiciones, pero también
 *        puede ser llamada manualmente si se necesita asegurar que los datos se guarden (p. ej., antes de apagar).
 *        Reinicia el buffer (bufferIndex = 0) después de escribir.
 * @param None
 * @return true si la escritura fue exitosa o si no había nada que escribir, false si el archivo no estaba abierto o hubo un error de escritura.
 */
bool flushBuffer();

/**
 * @brief Asegura que todos los datos restantes en el buffer se escriban en la tarjeta SD (llamando a flushBuffer())
 *        y luego cierra el archivo de log (logFile). Es importante llamar a esta función al final del
 *        proceso de logging para asegurar la integridad de los datos.
 * @param None
 * @return true si el flush y el cierre fueron exitosos, false si el archivo no estaba abierto inicialmente.
 */
bool closeSD();

#endif // SDLOGGER_H