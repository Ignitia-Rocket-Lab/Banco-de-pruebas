/*
 * See documentation at https://nRF24.github.io/RF24
 * See License information at root directory of this library
 * Author: Brendan Doherty (2bndy5)
*/

//Librerias
//Antenas
#include <SPI.h>
#include "printf.h"
#include "RF24.h"

//Libreria del servomotor
#include <Servo.h>

//Pines antenas
#define CE_PIN 7
#define CSN_PIN 8

//Antenas
// instantiate an object for the nRF24L01 transceiver
RF24 radio(CE_PIN, CSN_PIN);
// an identifying device destination
// Let these addresses be used for the pair
uint8_t address[][6] = { "1Node", "2Node" };
// It is very helpful to think of an address as a path instead of as
// an identifying device destination
// to use different addresses on a pair of radios, we need a variable to
// uniquely identify which address this radio will use to transmit

bool radioNumber = 1;  // 0 uses address[0] to transmit, 1 uses address[1] to transmit

// Variable para : Receptor o transmitor
bool role = false;  // true = TX role, false = RX role

// For this example, we'll be using a payload containing
// a string & an integer number that will be incremented
// on every successful transmission.
// Make a data structure to store the entire payload of different datatypes
struct PayloadStruct {
  char message[7];  // only using 6 characters for TX & ACK payloads
  uint8_t counter;
};

PayloadStruct payload;

//Cosas del servoMotor
Servo myservo;
int pos = 0;

//Variables del menu

unsigned long startMillis;  // Tiempo inicial
unsigned long currentMillis; // Tiempo actual
const unsigned long period = 10000; // Tiempo de espera en milisegundos (5 segundos)
bool arduinos; //True = Pilotado aka Persona, False = banco de prueba
String input = "0";

// FUNCIONES

void menu(){

  Serial.println("Menu:");
  Serial.println("Por defecto se ira a recepcion de datos. (0)");
  Serial.println("1. Checar la conexión de antenas");
  Serial.println("2. Test de sistemas");
  Serial.println("3. Switch fisico");
  Serial.println("4. Prender motor");
  Serial.println("5. Lectura de fuerza");
  Serial.println("6. Leer SD");
  Serial.println("7. Cambiar TX -> RX");

} //Menu


bool esperandoInput() {
  bool inputReceived = false;

  while(currentMillis <= startMillis + period){
    currentMillis = millis();  // Actualiza el tiempo actual
  
    // Verifica si hay datos disponibles en el puerto serial
    if (Serial.available() > 0) {
      char input = Serial.read();
      Serial.println("Eres el piloto");
      inputReceived = true;
      break;
    }
        
  }
  return inputReceived;
}//Checar si es el arduino tripulado

void recepcionDatos() {

  Serial.println("Reciviendo datos ->");

  while(!arduinos){
    arduinos = recivirDatos();
  }
  Serial.println("Se salio de recepcion de datos");
}//Recepcion de datos 

void conexionAntenas() {

  Serial.println("Comprobar conexion entre antenas");
  Serial.println("Mandar hello, esperando world");

  strcpy(payload.message, "hello ");

  for(int x = 0; x < 10;x++){
    mandarDatos();
    delay(500); 
  }


}//Conexion de antenas

void testSistemas() {
  Serial.println("Test de sistemas.");
  Serial.println("0, regresar al menu. Esperando input. ");

  char inputTS = 123;


  while (inputTS != '0'){
    while (!Serial.available()) {
      // wait for user input
    }

    inputTS = Serial.read();

    Serial.print("Carácter ingresado: ");
    Serial.println(inputTS);

  }//While TS != 0

} //Test de sistemas

void swFisico() {
  Serial.println("Switch fisico.");
  procesarEnvioDatos(3);

}//Switch fisico

void prenderMotor() {
  Serial.println("Prender motor");
  procesarEnvioDatos(4);

}//Prender motor

void adcLectura(){
 Serial.println("Adc Lectura");
}//ADC lectura

void leerSD() {
  Serial.println("Lectura de tarjeta SD");
  Serial.println("0, regresar al menu. Esperando input. ");

  char inputSD = 123;


  while (inputSD != '0'){
    while (!Serial.available()) {
      // wait for user input
    }

    inputSD = Serial.read();

    Serial.print("Carácter ingresado: ");
    Serial.println(inputSD);

  }//While SD != 0

}//Leer sd

void cambiarTXRX(){
  Serial.println("Cambiar de TX -> RX");
  procesarEnvioDatos(7);
  empezarEscucharDatos();

}//Cambiar tx -> rx

void setup() {
  // Inicializar la comunicación serial
  Serial.begin(115200);
  
  Serial.println("Bienvenido al banco de pruebas de <Ignitia>. Presiona cualquier tecla en los siguientes 10 segundos para comenzar. Si no se ira a recepcion de datos.");
  startMillis = millis();  // Captura el tiempo inicial 
  arduinos = esperandoInput();

  radio_Setup(arduinos); //Radio set up
  servoMotorSetup(); //Servo motor set up
  mossfetSetUp(); //mossfet setup

  menu();
}//SETUPs

void loop() {
  // Comprobar si hay datos disponibles en el puerto serial
  if(arduinos == true){
    while (!Serial.available()) {
    // wait for user input
    }
    // Leer el dato del puerto serial
    input = Serial.readStringUntil('\n');
    input.trim();
  }
  
  // Llamar a la función correspondiente según el input
  if (input == "0") {
    recepcionDatos();
  } else if (input == "1") {
    conexionAntenas();
  } else if (input == "2") {
    testSistemas();
  } else if (input == "3") {
    swFisico();
  } else if (input == "4") {
    prenderMotor();
  } else if (input == "5") {
    adcLectura();
  }else if (input == "6") {
    leerSD();
  }else if (input == "7") {
    cambiarTXRX();
  } else {
    Serial.print("Opción no válida. Intente nuevamente ");
    Serial.print(input);
    Serial.println("");
  }

  menu();

}//LOOP