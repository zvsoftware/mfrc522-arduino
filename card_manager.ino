#include <ArduinoJson.h>
#include "rfid.h"
#include "serial.h"
// #include <avr/wdt.h>

#define RST_PIN 7
#define SS_PIN 6

// #define RST_PIN  49 // Arduino Mega
// #define SS_PIN 53 // Arduino Mega

// #define RST_PIN  5 // Arduino Uno
// #define SS_PIN 10 // Arduino Uno



MFRC522 mfrc(SS_PIN, RST_PIN);
bool doWrite;
bool doClear;
String rfid;
String id;

void setup() 
{
  Serial.begin(9600); 
  Serial.println("STARTING UP");
  SPI.begin();
  mfrc.PCD_Init(); 
  Serial.println("\nScan your card.");
  Serial.println();
}

void handle_commands(StaticJsonDocument<200> doc) {
  // Retrieve values from the JSON document

  doWrite = doc["write"];
  doClear = doc["clear"];

  // Do something based on the values
  if (doWrite) {
    String id = doc["id"];
    rfid_write(&mfrc, id);
  } 
  else if (doClear) {
    clear_rfid(&mfrc);
  }
}

void loop() 
{ 

  // Wait for card, exit if no card
  if (!mfrc.PICC_IsNewCardPresent()) return;
  if (!mfrc.PICC_ReadCardSerial()) return;

  rfid = rfid_read(&mfrc);
  Serial.println("{ \"id\": \"" + rfid + "\" }");

  // clear_rfid(&mfrc);
  StaticJsonDocument<200> doc = listen_for_commands();
  handle_commands(doc);
  

  // Stop RFID device
  mfrc.PICC_HaltA(); 
  mfrc.PCD_StopCrypto1(); 
}
