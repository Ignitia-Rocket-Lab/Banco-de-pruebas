#include "ADCSetup.h"
#include <Wire.h>
#include <Arduino.h>

// Constantes configurables
const uint8_t INTERRUPT_PIN = 2;         
const uint32_t SERIAL_BAUD_RATE = 115200;
const uint8_t OFFSET = 28; //Valor necesario para estabilizar la medicion en 0 (antes 43)

// constructor del ADS115 en el bus I2C con address 0x48 (Pin ADDR -> GND)
extern ADS1115 ADS(0x48);

// lectura del adc
extern int16_t adcMeasurement = 0;

// bandera de interrupcion de conversion lista
extern volatile bool conversionReady = false;

void setupADC() {
  Serial.begin(SERIAL_BAUD_RATE);
  Wire.begin();

  // ligar el pin 2 con la interrupcion del ADC y el interrupt handler
  pinMode(INTERRUPT_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), adsInterruptHandler, RISING);

  // inicializar ads y checar conexión
  ADS.begin();
  Serial.print("ADC connected: ");
  Serial.println(ADS.isConnected());

  // setea el maxVoltage que se puede medir a 4.096V (Ver tabla en la documentacion de la libreria)
  ADS.setGain(ADS_MAX_VOLTAGE_4V);

  // setea la velocidad de toma de muestras a 250 muestras por segundo (Ver tabla en la documentacion)
  ADS.setDataRate(ADS_RATE_250);  // 0 = minimo  4 = default   7 = maximo

  // setear el MSB de los registros Hi_thresh y Lo_thresh en 1 y 0 respectivamente
  // esto se hace para configurar el ALRT pin como pin de interrupcion cuando se completa la conversion
  // pag 20 de la datasheet para mayor informacion de este modo.
  ADS.setComparatorThresholdHigh(0x8000);
  ADS.setComparatorThresholdLow(0x0000);

  // establecer que se quiere disparar la interrupcion con cada conversion que se finaliza
  // 0 = cada 1 conversion  1 = cada dos conversiones   2 = cada cuatro conversiones  3 = deshabilitado
  ADS.setComparatorQueConvert(TRIGG_ALRT_ONE_CONVERSION);

  // poner el ADS en modo continuo
  ADS.setMode(ADS_MODE_CONTINUOUS);

  // disparar la primer conversion para activar el modo continuo
  // notese que para activar el modo continuo, solo se necesita llamar una vez la funcion
  ADS.requestADC_Differential_0_1();
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
    adcMeasurement = ADS.getValue() + OFFSET; // ajuste para centrar la lectura de ADC a cero (en reposo) *Falta restar peso del motor*
    // inidicar que ya se manejo la interrupcion
    return true;
  }
  return false; 
}