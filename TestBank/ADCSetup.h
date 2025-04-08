#ifndef ADCSETUP_H
#define ADCSETUP_H
#include <Wire.h>
#include <Arduino.h>
#include "ADS1X15.h"

// macros del driver
#define ADS_MODE_CONTINUOUS 0
#define ADS_RATE_16 1
#define ADS_RATE_250 5
#define ADS_MAX_VOLTAGE_4V 1
#define TRIGG_ALRT_ONE_CONVERSION 0
#define CALIBRATION_PARAM 0.20463847203 //0.23734177 originaal    0.220689655172413 prueba 1
#define WEIGHT(x) ((x) * (CALIBRATION_PARAM))
#define TEST_TRESHOLD 50 // Treshold de disparo de inicio de la prueba

// prototipado de funciones
bool handleConversion(void);
void adsInterruptHandler(void);
void setupADC(void);
void printMeasurement(void);


// variables globales
extern ADS1115 ADS;
extern int16_t adcMeasurement;
extern volatile bool conversionReady;
extern unsigned long startTime;
extern bool isTestRunning;

#endif