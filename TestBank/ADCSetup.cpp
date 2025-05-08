#include "ADCSetup.h"

// constructor del ADS115 en el bus I2C con address 0x48 (Pin ADDR -> GND)
ADS1115 ADS(0x48);

// lectura del adc
int16_t adcMeasurement = 0;

// bandera de interrupcion de conversion lista
volatile bool conversionReady = false;

// instante de tiempo del inicio de la prueba
unsigned long startTime = 0;

// bandera para indicar si la prueba ha comenzado
bool isTestRunning = false;

void setupADC() {
  Serial.println(F("setupADC: Iniciando...")); delay(100);

  Serial.println(F("setupADC: Wire.begin()...")); delay(100);
  Wire.begin();
  Serial.println(F("setupADC: Wire.begin() - OK")); delay(100);

  // ligar el pin 2 con la interrupcion del ADC y el interrupt handler
  Serial.println(F("setupADC: pinMode(2, INPUT_PULLUP)...")); delay(100);
  pinMode(2, INPUT_PULLUP);
  Serial.println(F("setupADC: pinMode(2, INPUT_PULLUP) - OK")); delay(100);

  Serial.println(F("setupADC: attachInterrupt...")); delay(100);
  // ¡OJO! Asegúrate que adsInterruptHandler es lo más simple posible (solo setear flag)
  attachInterrupt(digitalPinToInterrupt(2), adsInterruptHandler, RISING);
  Serial.println(F("setupADC: attachInterrupt - OK")); delay(100);

  Serial.println(F("setupADC: ADS.begin()...")); delay(100);

  // inicializar ads y checar conexión
  bool success = ADS.begin(); // Llama a begin y guarda el resultado
  Serial.print(F("setupADC: ADS.begin() - Resultado: ")); Serial.println(success); delay(100);
  if (!success) {
     Serial.println(F("###### FALLO EN ADS.begin() ######"));
     // Podrías quedarte en un bucle aquí para indicar fallo claro
     while(1) { 
      delay(500); Serial.print("."); 
      }
  }


  Serial.println(F("setupADC: ADS.isConnected()...")); delay(100);
  Serial.print(F("  ADC connected (check): ")); Serial.println(ADS.isConnected()); delay(100);

  // setea el maxVoltage que se puede medir a 4.096V (Ver tabla en la documentacion de la libreria)
  Serial.println(F("setupADC: ADS.setGain...")); delay(100);
  ADS.setGain(ADS_MAX_VOLTAGE_4V);
  Serial.println(F("setupADC: ADS.setGain - OK")); delay(100);

  // setea la velocidad de toma de muestras a 250 muestras por segundo (Ver tabla en la documentacion)
  Serial.println(F("setupADC: ADS.setDataRate...")); delay(100);
  ADS.setDataRate(ADS_RATE_250);  // 0 = minimo  4 = default   7 = maximo
  Serial.println(F("setupADC: ADS.setDataRate - OK")); delay(100);

  // setear el MSB de los registros Hi_thresh y Lo_thresh en 1 y 0 respectivamente
  // esto se hace para configurar el ALRT pin como pin de interrupcion cuando se completa la conversion
  // pag 20 de la datasheet para mayor informacion de este modo.
  Serial.println(F("setupADC: ADS.setComparatorThresholdHigh...")); delay(100);
  ADS.setComparatorThresholdHigh(0x8000);
  Serial.println(F("setupADC: ADS.setComparatorThresholdHigh - OK")); delay(100);

  Serial.println(F("setupADC: ADS.setComparatorThresholdLow...")); delay(100);
  ADS.setComparatorThresholdLow(0x0000);
  Serial.println(F("setupADC: ADS.setComparatorThresholdLow - OK")); delay(100);

  // establecer que se quiere disparar la interrupcion con cada conversion que se finaliza
  // 0 = cada 1 conversion  1 = cada dos conversiones   2 = cada cuatro conversiones  3 = deshabilitado
  Serial.println(F("setupADC: ADS.setComparatorQueConvert...")); delay(100);
  ADS.setComparatorQueConvert(TRIGG_ALRT_ONE_CONVERSION);
  Serial.println(F("setupADC: ADS.setComparatorQueConvert - OK")); delay(100);

  // poner el ADS en modo continuo
  Serial.println(F("setupADC: ADS.setMode...")); delay(100);
  ADS.setMode(ADS_MODE_CONTINUOUS);
  Serial.println(F("setupADC: ADS.setMode - OK")); delay(100);

  // disparar la primer conversion para activar el modo continuo
  // notese que para activar el modo continuo, solo se necesita llamar una vez la funcion
  Serial.println(F("setupADC: ADS.requestADC_Differential_0_1()...")); delay(100);
  ADS.requestADC_Differential_0_1();
  Serial.println(F("setupADC: ADS.requestADC_Differential_0_1() - OK")); delay(100);

  Serial.println(F("setupADC: Finalizado con ÉXITO.")); delay(100);
}

// funcion de manejo de interrupcion del ADC
void adsInterruptHandler()
{
  // levanta la bandera de interrupcion relacionada con el ADC
  conversionReady = true;
}

// funcion para manejar las conversiones del ADC
bool handleConversion()
{
  // corroborar que hay una conversion lista
  if (conversionReady)
  {
    // bajar la bandera de interrupcion
    conversionReady = false;
    // obtener la medicion del ADC
    adcMeasurement = ADS.getValue() + 43; // ajuste para centrar la lectura de ADC a cero (en reposo) *Falta restar peso del motor*
    // Condición para iniciar cronómetro
    if (!isTestRunning && adcMeasurement > TEST_TRESHOLD) {  //No encontramos donde se define el Treshold
      startTime = millis();  // Guarda el tiempo actual
      isTestRunning = true;  // Marca que la prueba ha comenzado
      Serial.println(F("Prueba iniciada."));
    }
    // inidicar que ya se manejo la interrupcion
    return true;
  }
  return false; 
}

void printMeasurement() {
    if (isTestRunning) {
        unsigned long elapsedTime = millis() - startTime;  // Tiempo transcurrido en milisegundos
        String logEntry = String(elapsedTime) + "," + String(WEIGHT(adcMeasurement)*9.81);//
        Serial.println(logEntry);  // Mostrar la entrada en el monitor serie
    }
}