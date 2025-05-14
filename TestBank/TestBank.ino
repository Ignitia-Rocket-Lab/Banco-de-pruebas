#include "RFController.h"
#include "ADCSetup.h"
#include "SDLogger.h"
#include "ServoSwitch.h"
#include "Leds.h"
#include "Igniter.h"

// Definicion de usos de leds y estados
#define CASO1 turnOff();
#define CASO2 turnOnRight();
#define CASO3 turnOnLeft();
#define CASO4 turnOnAll();

// Definición de los estados de la máquina
enum State {
  Init, // Estado de Init
  DiagWait, // Estado de espera para Diagnostico
  Diag, 
  FailDiag, // Estado de Fail Diagnostic
  IgnitionWait, // Estado de IgnitionWait
  ArmedWait, // Estado de ArmedWait
  ACQ, // Estado de Ignition

  Success, // Estado de Exito
  End   //Estado de prueba terminada
};

// Variable para almacenar el estado actual, inicializado en Diag
State currentState = Init;

uint8_t payload[RF24_MAX_PAYLOAD_SIZE];
uint8_t payload_Size;

bool isTestRunning = false;

// Variables para manejar los tiempos de espera (timeouts)
unsigned long ignitionWaitEntryTime = 0;
unsigned long armedWaitEntryTime = 0;
unsigned long startTime = 0;

// Constantes para los tiempos de espera en milisegundos
const unsigned long IGNITION_WAIT_TIMEOUT = 30000;  // 30 segundos
const unsigned long ARMED_WAIT_TIMEOUT = 10000; // 10 segundos

// --- Funciones Placeholder ---
/*
 * @brief Realiza diagnósticos de los periféricos principales (SD, ADC, RF).
 * @return true si todos los diagnósticos pafsan, false si alguno falla.
*/
bool performDiagnostics() {

    bool diagOk = true; // Asumimos éxito inicial
    Serial.println(F("--- Iniciando Diagnósticos ---"));

    // --- 1. Comprobar Estado de la Tarjeta SD y Archivo ---
    Serial.println(F("  Diag - [1/4] Verificando SD y Archivo Log..."));
    if (!logFile) { // La variable 'logFile' viene de SDLogger.h/.cpp
        Serial.println(F("   ERROR: El archivo de log no está abierto o la SD falló en initFile()."));
        diagOk = false;
    } else {
        Serial.println(F("   OK: Archivo log parece estar abierto."));
    }

    // --- 2. Leer un Dato del ADC ---
    int16_t diagnosticAdcValue = 0; // Para guardar el valor leído
    if (diagOk) { // Solo continuar si lo anterior fue exitoso
        Serial.println(F("  Diag - [2/4] Leyendo valor del ADC..."));
        bool adcReadSuccess = false;
        if (handleConversion()) { // Llama a la función de tu driver ADCSetup
            diagnosticAdcValue = adcMeasurement; // Guarda el valor leído (variable global de ADCSetup)
            adcReadSuccess = true;
            Serial.print(F("   OK: Lectura ADC obtenida: "));
            Serial.println(diagnosticAdcValue);
        }
        if (!adcReadSuccess) {
            Serial.println(F("   ERROR: No se pudo obtener una lectura del ADC en el tiempo esperado."));
            diagOk = false;
        }
    } else {
        Serial.println(F("  Diag - [2/4] Omitido (Fallo previo)"));
    }

    // --- 3. Escribir y Flushear Dato en SD ---
    if (diagOk) {
        Serial.println(F("  Diag - [3/4] Escribiendo y Flusheando dato de prueba en SD..."));
        String testData = "DiagTest," + String(diagnosticAdcValue);
        if (!logToSD(testData)) { // Usa la función de SDLogger
            Serial.println(F("   ERROR: Falló logToSD()."));
            diagOk = false;
        } else {
            // Forzar el flush para asegurar escritura física inmediata
            if (!flushBuffer()) { // Usa la función de SDLogger
                Serial.println(F("    ERROR: Falló flushBuffer() después de escribir."));
                diagOk = false;
            } else {
                Serial.println(F("    OK: Dato de prueba escrito y flusheado en SD."));
            }
        }
    } else {
        Serial.println(F("  Diag - [3/4] Omitido (Fallo previo)"));
    }

    // --- 4. Transmitir Dato por RF y Esperar ACK ---
    if (diagOk) {
        Serial.println(F("  Diag - [4/4] Transmitiendo dato de prueba por RF..."));
        if (!sendAdcData(diagnosticAdcValue)) { // Usa la función de RFController
            Serial.println(F("    ERROR: Falló sendDataRF24() (No se recibió ACK). Verificar receptor."));
            diagOk = false;
        } else {
            Serial.println(F("    OK: Dato de prueba enviado por RF y ACK recibido."));
        }
    } else {
        Serial.println(F("  Diag - [4/4] Omitido (Fallo previo)"));
    }

    // --- Resultado Final ---
    Serial.println(F("--- Diagnósticos Finalizados ---"));
    sendDiagResult(diagOk);
    return diagOk;
}

bool checkGoToDiagSignal() {
    if(isDataAvailable()){
        uint8_t size = readDataSimpleRF24();
        if (size == 2){
            MessageType msgType = (MessageType)payload[0];
            if(msgType == MSG_TYPE_COMMAND){
                CommandCode receivedCommand = (CommandCode)payload[1];
                if (receivedCommand == CMD_GOTO_DIAG_WAIT){
                    Serial.println(F("    Executing: Go to Diagnostic Wait"));
                    return true;
                } else{ Serial.println(F("  Error: Unexpected command received")); }
            } else{ Serial.println(F("  Error: Message is not a Command Type")); }
        } else{ Serial.println(F("  Error: Size does not match de required lenght")); }
    } //else{ Serial.println(F("  Error: No data available on the buffer 1")); }
    return false;
}

bool checkDiagnosticRequested() {
    if(isDataAvailable()){
        uint8_t size = readDataSimpleRF24();
        if (size == 2){
            MessageType msgType = (MessageType)payload[0];
            if(msgType == MSG_TYPE_COMMAND){
                CommandCode receivedCommand = (CommandCode)payload[1];
                if (receivedCommand == CMD_REQUEST_DIAGNOSTICS){
                    Serial.println(F("    Executing: Go to Diagnostics"));
                    //sendACK();
                    return true;
                } else{ Serial.println(F("  Error: Unexpected command received")); }
            } else{ Serial.println(F("  Error: Message is not a Command Type")); }
        } else{ Serial.println(F("  Error: Size does not match de required lenght")); }
    } //else{ Serial.println(F("  Error: No data available on the buffer 2")); }
    return false;
}

bool checkInitialSignal() {
    if(isDataAvailable()){
      Serial.print(F("  isDataAvailable(): "));
      Serial.println(isDataAvailable());
        uint8_t size = readDataSimpleRF24();
        if (size == 2){
            MessageType msgType = (MessageType)payload[0];
            if(msgType == MSG_TYPE_COMMAND){
                CommandCode receivedCommand = (CommandCode)payload[1];
                if (receivedCommand == CMD_GOTO_DIAG_WAIT){
                    Serial.println(F("    Executing: Go to Wait Diag"));
                    return true;
                } else{ Serial.println(F("  Error: Unexpected command received")); }
            } else{ Serial.println(F("  Error: Message is not a Command Type")); }
        } else{ Serial.println(F("  Error: Size does not match de required lenght")); }
    } //else{ Serial.println(F("  Error: No data available on the buffer 3")); }
    return false;
}

bool checkIgnitionSignal() {
    if(isDataAvailable()){
        uint8_t size = readDataSimpleRF24();
        if (size == 2){
            MessageType msgType = (MessageType)payload[0];
            if(msgType == MSG_TYPE_COMMAND){
                CommandCode receivedCommand = (CommandCode)payload[1];
                if (receivedCommand == CMD_GOTO_ARMED){
                    Serial.println(F("    Executing: Go to Armed Wait"));
                    return true;
                } else{ Serial.println(F("  Error: Unexpected command received")); }
            } else{ Serial.println(F("  Error: Message is not a Command Type")); }
        } else{ Serial.println(F("  Error: Size does not match de required lenght")); }
    } //else{ Serial.println(F("  Error: No data available on the buffer 4")); }
    return false;
}

bool checkAbortSignal() {
    if(isDataAvailable()){
        uint8_t size = readDataSimpleRF24();
        if (size == 2){
            MessageType msgType = (MessageType)payload[0];
            if(msgType == MSG_TYPE_COMMAND){
                CommandCode receivedCommand = (CommandCode)payload[1];
                if (receivedCommand == CMD_ABORT){
                    Serial.println(F("    Executing: Abortion"));
                    return true;
                } else{ Serial.println(F("  Error: Unexpected command received")); }
            } else{ Serial.println(F("  Error: Message is not a Command Type")); }
        } else{ Serial.println(F("  Error: Size does not match de required lenght")); }
    } //else{ Serial.println(F("  Error: No data available on the buffer 5")); }
    return false;
}

bool checkSuccessSignal() {
    if(isDataAvailable()){
        uint8_t size = readDataSimpleRF24();
        if (size == 2){
            MessageType msgType = (MessageType)payload[0];
            if(msgType == MSG_TYPE_COMMAND){
                CommandCode receivedCommand = (CommandCode)payload[1];
                if (receivedCommand == CMD_SUCCESS){
                    Serial.println(F("    Executing: End of Test"));
                    return true;
                } else{ Serial.println(F("  Error: Unexpected command received")); }
            } else{ Serial.println(F("  Error: Message is not a Command Type")); }
        } else{ Serial.println(F("  Error: Size does not match de required lenght")); }
    } //else{ Serial.println(F("  Error: No data available on the buffer 6")); }
    return false;
}

bool checkEndSignal() {
    if(isDataAvailable()){
        uint8_t size = readDataSimpleRF24();
        if (size == 2){
            MessageType msgType = (MessageType)payload[0];
            if(msgType == MSG_TYPE_COMMAND){
                CommandCode receivedCommand = (CommandCode)payload[1];
                if (receivedCommand == CMD_END){
                    Serial.println(F("    Executing: End of Test"));
                    return true;
                } else{ Serial.println(F("  Error: Unexpected command received")); }
            } else{ Serial.println(F("  Error: Message is not a Command Type")); }
        } else{ Serial.println(F("  Error: Size does not match de required lenght")); }
    } //else{ Serial.println(F("  Error: No data available on the buffer 6")); }
    return false;
}

void performAcquisition() {
  Serial.println(F("  Realizando Adquisición (ACQ)..."));
  // Inicialización
  startTime = millis();
  isTestRunning = true;

  // Estadísticas
  uint32_t successCount = 0;
  uint32_t failCount = 0;

  // Bucle principal de adquisición
  while (isTestRunning) {
    // Comprobar si se recibe la señal de Success ANTES de tomar la muestra
    if (checkSuccessSignal()) {
      isTestRunning = false; // Detener la adquisición
      currentState = Success; // Pasar al estado Success
      Serial.println(F("  Comando Success recibido. Deteniendo adquisición."));
      break; // Salir del bucle de adquisición
    }

    // Espera activa con interrupción del ADC
    if (handleConversion()) {
      if (logMeasurement()) {
        successCount++;
      } else {
        failCount++;
      }
    }
  }

  // Reporte final
  Serial.println(F(">> Adquisición finalizada."));
  Serial.print(F("Lecturas exitosas: "));
  Serial.println(successCount);
  Serial.print(F("Lecturas fallidas: "));
  Serial.println(failCount);
}

// --- Configuración Inicial ---
void setup() {
    Serial.begin(115200); // Inicia comunicación serie para depuración
    while (!Serial); // Espera a que el puerto serie se conecte (necesario para algunos Arduinos como Leonardo/Micro)
    Serial.println(F("Test Bank Initializing..."));

    while(!setupRF24(TESTBANK, RF24_PA_MAX, RF24_1MBPS)){ // Comentado temporalmente
    // Es un bucle de reintento para asegurar que el módulo de radio NRF24L01 esté inicializado correctamente antes de continuar.
      Serial.println(F("Retrying RF..."));
      delay(1000);
    }

    setupLEDs();
    setupServo();
    offSwitch();
    setupIgniter();
    resetIgniterSafeState();


    setupADC();
    while(!setupSD()){
        Serial.println(F("Retrying..."));
    }
    initFile("Time (ms), Force (N)"); 
}

// Loop para probar posiciones del servo
void loop() {
  switch (currentState) {
    case Init:
        offSwitch();

        // LEDs con delays para verificación visual
        CASO4;
        delay(500);
        CASO1;
        delay(500);
        CASO4;
        delay(500);
        CASO1;
        delay(500);
        CASO4;
        delay(500);
        CASO1;

        if (checkGoToDiagSignal()) {
            currentState = DiagWait;
        }
        break; //Fin case Init
      
    case DiagWait:

        CASO4;
        if(checkDiagnosticRequested()){
            currentState = Diag;
        }
        break; // Fin case Diag

    case Diag:

        CASO1;
        delay(10000);
        CASO4;
        if (performDiagnostics()) {
            ignitionWaitEntryTime = millis(); // Guarda el tiempo de entrada a IgnitionWait
            currentState = IgnitionWait;
        } else {
            currentState = FailDiag;
        }
        break;

    case FailDiag:
        Serial.println("Estamos en FailDiag");

        Serial.println(F("Estado: FailDiag - BUCLE DE FALLO"));
        currentState = Init;
        break; // Fin case FailDiag

    case IgnitionWait:

        CASO4;
        if (checkIgnitionSignal()) {
            resetIgniterSafeState();
            onSwitch();
            armedWaitEntryTime = millis(); // Guarda el tiempo de entrada a ArmedWait
            currentState = ArmedWait;
        }
        // Comprobar si ha pasado el tiempo de espera (timeout)
        else if (millis() - ignitionWaitEntryTime >= IGNITION_WAIT_TIMEOUT) {
            currentState = Init; // Regresa a Diag si pasa el tiempo
        }
        // Si no pasa nada, sigue esperando en este estado en la próxima iteración del loop()
        break; // Fin case IgnitionWait

    case ArmedWait:
        Serial.println("Estamos en ArmedWait");

        CASO4;
        delay(400);
        CASO1;
        delay(400);
        CASO4;
        delay(400);
        CASO1;
        // Comprobar si se recibe la señal de abortar
        if (checkAbortSignal()) {
            ignitionWaitEntryTime = millis(); // Reinicia timer de IgnitionWait al volver
            currentState = IgnitionWait;
        }
        // Comprobar si ha pasado el tiempo de espera (timeout)
        else if (millis() - armedWaitEntryTime >= ARMED_WAIT_TIMEOUT) {

          prepareIgniter();
          igniteMotor(); 
          currentState = ACQ; // Pasa a ACQ si pasa el tiempo
        }
        // Si no pasa nada, sigue esperando en este estado
        break; // Fin case ArmedWait

    case ACQ:
      performAcquisition();
      break; // Fin case ACQ

    case Success:

        Serial.println(F("Estado: Sucess - BUCLE DE ÉXITO"));
        CASO4;
        delay(2000);
        CASO1;
        delay(2000);
        CASO4;
        delay(2000);
        CASO1;
        delay(2000);
        CASO4;
        delay(2000);
        CASO1;   

        if (checkEndSignal()) {
            currentState = End;
        }

        // Se queda en este estado permanentemente
        break; // Fin case Sucess

    case End:
        resetIgniterSafeState();
        CASO1; // Leds
        closeSD();
        break;

    default:
      currentState = Init;
      break;

    } // Fin del switch
} // Fin del loop 
