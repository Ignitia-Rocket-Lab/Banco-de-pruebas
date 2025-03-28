#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <SD.h>

// Pin definitions
#define NRF_CE_PIN 7      // NRF24L01 CE pin
#define NRF_CSN_PIN 8     // NRF24L01 CSN pin
#define SD_CS_PIN 10      // SD card module CS pin

// Create an instance of the RF24 class
RF24 radio(NRF_CE_PIN, NRF_CSN_PIN);

// Define the address for NRF24L01 communication
const byte address[6] = "00001";

// Global variable to store the current filename
String currentFilename;

// Function to enable the NRF24L01 module
void enableNRF24L01() {
  digitalWrite(SD_CS_PIN, HIGH); // Disable SD card module
  digitalWrite(NRF_CSN_PIN, LOW); // Enable NRF24L01 module
}

// Function to enable the SD card module
void enableSDCard() {
  digitalWrite(NRF_CSN_PIN, HIGH); // Disable NRF24L01 module
  digitalWrite(SD_CS_PIN, LOW);   // Enable SD card module
}

// Function to get the next available filename
String getNextFilename() {
  int fileNumber = 1; // Start with 1
  while (true) {
    String filename = String(fileNumber) + ".txt"; // Create filename (e.g., "1.txt")
    if (!SD.exists(filename)) {
      return filename; // Return the filename if it doesn't exist
    }
    fileNumber++; // Increment file number if the file exists
  }
}

void setup() {
  // Initialize Serial communication
  Serial.begin(9600);

  // Initialize the NRF24L01 module
  bool nrfInitialized = false;
  do {
    enableNRF24L01(); // Enable NRF24L01 module
    if (radio.begin()) {
      nrfInitialized = true;
      Serial.println("NRF24L01 initialized successfully!");
      radio.openWritingPipe(address);  // Set the address to which we will send data
      radio.stopListening();           // Set as transmitter
      Serial.println("NRF24L01 is in transmitter mode.");
    } else {
      Serial.println("NRF24L01 initialization failed. Retrying...");
      delay(1000); // Wait before retrying
    }
  } while (!nrfInitialized);

  // Initialize the SD card module
  bool sdInitialized = false;
  do {
    enableSDCard(); // Enable SD card module
    if (SD.begin(SD_CS_PIN)) {
      sdInitialized = true;
      Serial.println("SD card initialized.");

      // Get the next available filename
      currentFilename = getNextFilename();
      Serial.print("Creating new file: ");
      Serial.println(currentFilename);

      // Create and write to the new file
      File dataFile = SD.open(currentFilename, FILE_WRITE);
      if (dataFile) {
        dataFile.println("Initial data written to " + currentFilename);
        dataFile.close();
        Serial.println("Data written to file.");
      } else {
        Serial.println("Error creating file!");
      }
    } else {
      Serial.println("SD card initialization failed. Retrying...");
      delay(1000); // Wait before retrying
    }
  } while (!sdInitialized);

  // Print a message to indicate setup is complete
  Serial.println("Setup complete.");
}

void loop() {
  // Enable the NRF24L01 module
  enableNRF24L01();

  // Prepare a message to send
  char text[] = 1;
  
  // Send the message
  bool sent = radio.write(&text, sizeof(text));
  
  if (sent) {
    Serial.print("Sent: ");
    Serial.println(text);
  } else {
    Serial.println("Failed to send message");
  }

  // Disable the NRF24L01 module and enable the SD card module
  enableSDCard();

  // Append the sent data to the current file
  File dataFile = SD.open(currentFilename, FILE_WRITE);
  if (dataFile) {
    dataFile.println(text); // Write the sent data to the file
    dataFile.close();
    Serial.println("Data logged to SD card.");
  } else {
    Serial.println("Error opening file!");
  }

  // Add a delay to avoid overwhelming the system
  delay(1000);
}