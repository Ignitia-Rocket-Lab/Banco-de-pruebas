/*
 * See documentation at https://nRF24.github.io/RF24
 * See License information at root directory of this library
 * Author: Brendan Doherty (2bndy5)

 *Codigo modificado para IGNITIA por parte de Stefano H.A.
 */

//Librerias para la Radio
#include <SPI.h>
#include "printf.h"
#include "RF24.h"

//Pines para Radio
#define CE_PIN 7
#define CSN_PIN 8

  // an identifying device destination
  // Let these addresses be used for the pair
  uint8_t address[][6] = { "1Node", "2Node" };
  // It is very helpful to think of an address as a path instead of as
  // an identifying device destination
  // to use different addresses on a pair of radios, we need a variable to

  // uniquely identify which address this radio will use to transmit
  bool radioNumber = 1;  // 0 uses address[0] to transmit, 1 uses address[1] to transmit

  // Used to control whether this node is sending or receiving
  bool role = false;  // true = TX role, false = RX role

// For this example, we'll be using a payload containing
  // a string & an integer number that will be incremented
  // on every successful transmission.
  // Make a data structure to store the entire payload of different datatypes
struct PayloadStruct {
  char message[15];  // only using 6 characters for TX & ACK payloads
  uint8_t counter;
};
PayloadStruct payload;
// initialize the transceiver on the SPI bus

  // instantiate an object for the nRF24L01 transceiver
  RF24 radio(CE_PIN, CSN_PIN);

//Librarias del servo
#include <Servo.h>

//Iniciador del servoMotor
Servo myservo;          //Variable del servo
int pos = 0;            //Variable para la posicion 

//Libreria del servomotor
#include <Servo.h>

void setup() {

  Serial.begin(115200);
  while (!Serial) {
    // some boards need to wait to ensure access to serial over USB
  }

  //SetUP del radio
  radio_Setup();

  //Setup del servoMotor
  servoMotorSetup();

  // For debugging info
  // printf_begin();             // needed only once for printing details
  // radio.printDetails();       // (smaller) function that prints raw register values
  // radio.printPrettyDetails(); // (larger) function that prints human readable data
}

void loop() {

 // role

 radioCommunication();
 

  if (Serial.available()) {
    char c = toupper(Serial.read());
    radio_Serial(c);
  }
}  // loop