#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <SD.h>

// Pin definitions
#define NRF_CE_PIN 7
#define NRF_CSN_PIN 8
#define SD_CS_PIN 10

RF24 radio(NRF_CE_PIN, NRF_CSN_PIN);
const byte address[6] = "00001"; // Address of the receiver

void setup() {
  Serial.begin(9600);

  // Initialize SD card
  pinMode(SD_CS_PIN, OUTPUT);
  digitalWrite(SD_CS_PIN, HIGH); // Disable SD card by default
  
  if (!SD.begin(SD_CS_PIN)) {
    Serial.println("SD card initialization failed!");
  } else {
    Serial.println("SD card initialized successfully!");
  }

  // Initialize nRF24L01+
  pinMode(NRF_CSN_PIN, OUTPUT);
  switchToNRF24L01();
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_LOW);
  radio.setChannel(108); // Choose a channel between 0 and 125
  radio.stopListening();

  // Ensure nRF24L01+ is connected
  while (!radio.isChipConnected()) {
    Serial.println("Waiting for nRF24L01+ connection...");
    while(1);
  }
  Serial.println("nRF24L01+ connected successfully!");
}

void loop() {
  // Example: Write to SD card
  Serial.println("Writing to SD card...");
  switchToSDCard();
  File file = SD.open("data.txt", FILE_WRITE);
  if (file) {
    file.println("Hello, SD card!");
    file.close();
    Serial.println("Data written to SD card successfully!");
  } else {
    Serial.println("Failed to write to SD card.");
  }
  delay(1000); // Simulate some delay for testing

  // Example: Send message with nRF24L01+
  Serial.println("Sending message with nRF24L01+...");
  switchToNRF24L01();
  const char text[] = "Hello";
  bool success = radio.write(&text, sizeof(text));

  if (success) {
    Serial.println("Message sent successfully!");
  } else {
    Serial.println("Message failed to send.");
  }

  delay(1000); // Wait before toggling back
}

// Function to enable SD card and disable nRF24L01+
void switchToSDCard() {
  digitalWrite(NRF_CSN_PIN, HIGH); // Disable nRF24L01+
  digitalWrite(SD_CS_PIN, LOW);    // Enable SD card
}

// Function to enable nRF24L01+ and disable SD card
void switchToNRF24L01() {
  digitalWrite(SD_CS_PIN, HIGH);   // Disable SD card
  digitalWrite(NRF_CSN_PIN, LOW);  // Enable nRF24L01+
}
