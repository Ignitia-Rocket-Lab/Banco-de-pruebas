// ----- SDLogger.cpp -----
// Implementación de las funciones del driver SDLogger.

#include "SDLogger.h"     // Incluye la cabecera de este propio driver
#include "ADCSetup.h"     // Incluye la cabecera del driver del ADC para acceder a sus variables globales
#include <Arduino.h>
#include <SPI.h>

// --- Definición de Variables Globales ---
//     Estas son las instancias reales de las variables declaradas como 'extern' en SDLogger.h
SdFat SD;                         ///< Instancia del objeto SdFat.
File logFile;                     ///< Instancia del objeto File para el archivo de log.
size_t bufferIndex = 0;           ///< Índice del buffer, inicializado a 0.
char logBuffer[BUFFER_SIZE];      ///< El buffer de RAM real.
String logFilename;               ///< String para almacenar el nombre del archivo.
unsigned long lastFlushTime = 0;  ///< Timestamp del último flush, inicializado a 0.


// --- Variables Globales Importadas ---
//     Estas variables son definidas en ADCSetup.cpp y declaradas como 'extern' en ADCSetup.h.
//     Se usan principalmente en la función logMeasurement().

/**
 * @brief (Importada de ADCSetup) Última lectura cruda del ADC. Usada en logMeasurement().
 */
extern int16_t adcMeasurement;

/**
 * @brief (Importada de ADCSetup) Bandera volátil que indica si una nueva conversión del ADC está lista.
 *        No se usa directamente en *este* archivo, pero su lógica controla cuándo se actualiza adcMeasurement.
 */
extern volatile bool conversionReady;

/**
 * @brief (Importada de ADCSetup) Timestamp (millis()) de cuándo comenzó la "prueba" o el período de interés.
 *        Usado en logMeasurement() para calcular el tiempo transcurrido.
 */
extern unsigned long startTime;

/**
 * @brief (Importada de ADCSetup) Bandera que indica si la lógica de la "prueba" está activa.
 *        Usado en logMeasurement() para decidir si registrar o no la medición.
 */
extern bool isTestRunning;

// --- Implementación de Funciones ---

/**
 * @brief Inicializa la tarjeta SD.
 * @copydoc setupSD(int)
 */
bool setupSD(int chipSelect) {
    Serial.print(F("Initializing SD card...\t"));
    delay(1000);

    // Intentar inicializar la tarjeta SD usando el pin CS especificado
    if (!SD.begin(chipSelect)) {
        Serial.println("Initialization failed!");
        return false;
    }

    // Configurar el pin del botón como entrada (su uso dependerá del sketch principal)
    pinMode(BUTTON_PIN, INPUT);

    Serial.println("Initialization done.");
    bufferIndex = 0;          // Asegurarse de que el índice del buffer esté a 0 al inicio.
    lastFlushTime = millis(); // Inicializar el tiempo del último flush al momento actual.
    return true;
}

/**
 * @brief Genera un nombre de archivo único.
 * @copydoc filenameGen()
 */
void filenameGen(void) {
    int counter = 1;
    String filenameBase = "log";
    String filenameExt = ".csv";
    String currentFilename;

    // Bucle para encontrar el primer nombre de archivo "logX.csv" que no exista
    do {
        // Limitar el contador para evitar bucles infinitos si hay demasiados archivos
        if (counter > 9999) {
            Serial.println(F("Error: Could not find an unused filename (log1.csv to log9999.csv)."));
            // Asignar un nombre por defecto o manejar el error de otra forma
            logFilename = F("error_log.csv");
            return;
        }
    } while (SD.exists(currentFilename.c_str())); // SD.exists() verifica si un archivo/directorio existe
    logFilename = currentFilename; // Almacenar el nombre encontrado en la variable global
    //Serial.print(F("Generated unique filename: "));
    //Serial.println(logFilename);
}

/**
 * @brief Abre/Crea el archivo de log e inicializa con encabezado.
 * @copydoc initFile(const String&)
 */
bool initFile(const String& header) {
    filenameGen(); // Generar el nombre único antes de intentar abrir

    // Abrir el archivo en modo escritura. FILE_WRITE abre para escribir al final,
    // o crea el archivo si no existe.
    logFile = SD.open(logFilename, FILE_WRITE);
    
    if (!logFile) { // Verificar si la apertura falló
        Serial.println("Error opening the file");
        return false;
    }
    
    // Escribir la línea de encabezado proporcionada
    logFile.println(header); // println añade automáticamente \r\n

    // Es crucial hacer flush después de escribir el encabezado para asegurar
    // que se guarde inmediatamente, especialmente si el programa pudiera
    // fallar poco después.
    if (!logFile.flush()) {
       Serial.println(F("Error flushing header to file."));
       logFile.close(); // Intentar cerrar el archivo si el flush falla
       return false;
    }
    
    Serial.print(F("Log file opened: "));
    Serial.println(logFilename);
    //Serial.print(F("Header written: "));
    //Serial.println(header);
    
     bufferIndex = 0;          // Resetear el índice del buffer al abrir un nuevo archivo.
    lastFlushTime = millis();  // Resetear el tiempo del último flush.
    return true;
}

/**
 * @brief Agrega datos al buffer y realiza flush si es necesario.
 * @copydoc logToSD(const String&)
 */
bool logToSD(const String& data) {
    // Primero, verificar si el archivo de log está realmente abierto y es válido
    if (!logFile) {
        Serial.println(F("Error: logToSD called but logFile is not open."));
        return false;
    }
    
    // Convertir el dato a un arreglo de caracteres y añadirlo al buffer
    String dataWithNewline = data + "\n";
    dataWithNewline.toCharArray(logBuffer + bufferIndex, dataWithNewline.length() + 1);
    
    // Actualizar el índice del buffer
    bufferIndex += dataWithNewline.length();
    
    // Hora actual para la lógica de flush por tiempo
    unsigned long currentTime = millis();
    
    // Condición para hacer flush:
    // 1. El buffer está "casi lleno". Dejamos un margen (ej. 80 bytes) para evitar
    //    el caso exacto de llenado que podría ser más complejo de manejar. O simplemente
    //    comparamos con BUFFER_SIZE.
    // 2. Ha pasado al menos 1 segundo (1000 ms) desde el último flush.
    //    Esto asegura que los datos se guarden periódicamente incluso si el buffer no se llena.
    if (bufferIndex >= BUFFER_SIZE - 80 || (currentTime - lastFlushTime >= 1000)) {
        return flushBuffer();
    }
    
    // Si no se hizo flush, la operación fue exitosa (datos añadidos al buffer)
    return true;
}

/**
 * @brief Registra una medición de tiempo y ADC.
 * @copydoc logMeasurement()
 * @note Esta función depende de las variables globales `isTestRunning`, `startTime`,
 *       `adcMeasurement` y la macro `WEIGHT` definidas externamente (probablemente en ADCSetup).
 */
void logMeasurement() {
  // Solo registrar si la bandera isTestRunning (del módulo ADC) está activa
    if (isTestRunning) {
        // Calcular el tiempo transcurrido desde el inicio de la prueba
        unsigned long elapsedTime = millis() - startTime;

        // Calcular el valor procesado (peso * gravedad en este caso)
        float calculatedValue = WEIGHT(adcMeasurement) * 9.81;

        // Crear la cadena de log en formato "tiempo,valor"
        String logEntry = String(elapsedTime) + "," + String(calculatedValue, 4); // 4 decimales de precisión

        // Enviar la cadena formateada al buffer/SD
        if (!logToSD(logEntry)) {
             Serial.println(F("Failed to log measurement to SD buffer."));
        }

        // Opcional: Imprimir también al monitor serie para depuración
        Serial.print(F("Logged: ")); // Comentar si es necesario
        Serial.println(logEntry);
    }
  // Si isTestRunning es false, no hace nada.
}

/**
 * @brief Fuerza la escritura del buffer a la SD.
 * @copydoc flushBuffer()
 */
bool flushBuffer() {
  // Verificar si el archivo está abierto
    if (!logFile) {
        Serial.println("Error: File is not open.");
        return false;
    }

    // Solo intentar escribir si hay datos en el buffer
    if (bufferIndex > 0) {
        logFile.write((const uint8_t*)logBuffer, bufferIndex);

        // Forzar la escritura física al medio (importante para asegurar datos)
        // ¡El flush puede tardar tiempo! Especialmente en tarjetas lentas.
        if (!logFile.flush()) {
            Serial.println(F("Error flushing file cache to SD card. Data might be cached but not fully saved."));
            // Esto es menos grave que un error de write, pero aún así es un problema.
            // Considerar cómo manejarlo. Por ahora, retornamos fallo.
            return false;
        }

        // Si la escritura y el flush fueron exitosos, reiniciar el índice del buffer
        bufferIndex = 0;

        // Actualizar el tiempo del último flush, incluso si el buffer estaba vacío.
        // Esto evita flushes innecesarios por tiempo justo después de un flush exitoso.
        lastFlushTime = millis();
        return true; // La operación (o la falta de necesidad de ella) fue exitosa.
    }
    
    // Aunque no haya nada que escribir, actualizamos el tiempo de último flush
    lastFlushTime = millis();
    return true; // Nada que escribir, pero no es un error
}

/**
 * @brief Cierra el archivo de log de forma segura.
 * @copydoc closeSD()
 */
bool closeSD() {
    // Verificar si el archivo está actualmente abierto
    if (!logFile) {
         Serial.println(F("Warning: closeSD called but no file seems to be open."));
         // No es estrictamente un error si ya estaba cerrado, pero puede indicar un problema lógico.
         return false;
    }
    
    Serial.println(F("Closing SD file..."));

    // Paso 1: Asegurarse de que todos los datos pendientes en el buffer se escriban.
    if (!flushBuffer()) {
        Serial.println(F("Error flushing buffer before closing file. Some data might be lost!"));
        // A pesar del error de flush, intentaremos cerrar el archivo de todos modos.
        logFile.close();
        return false; // Indicar que hubo un problema durante el cierre.
    }
    
    // Paso 2: Cerrar el archivo. Esto también debería hacer un flush final, pero hacerlo explícitamente antes es más seguro.
    logFile.close();

    // Marcar el archivo como no válido/cerrado (opcional, pero buena práctica)
    // logFile = File(); // O alguna forma de invalidar el objeto File si la librería lo permite

    Serial.println(F("Log file closed successfully."));
    return true;
}