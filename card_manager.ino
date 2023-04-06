#include <ArduinoJson.h>
#include "rfid.h"
#include "serial.h"

// SS_PIN IS BOARD DEPENDANT!!
#define SS_PIN 10 
#define RST_PIN  5

MFRC522 mfrc(SS_PIN, RST_PIN);

void setup() 
{
  Serial.begin(9600); 
  Serial.println("STARTING UP");

  SPI.begin();
  mfrc.PCD_Init(); 
  Serial.println("\nScan your card.");
  Serial.println();
}

void write_random_id() {
  String id = "";
  randomSeed(analogRead(0));

  for (int i = 0; i < 16; i++) {
    id += random(0, 10);
  }

  rfid_write(&mfrc, id);
}


void handle_commands(StaticJsonDocument<200> doc) {
  // Retrieve values from the JSON document

  bool write = doc["write"];
  bool clear = doc["clear"];

  // Do something based on the values
  if (write) {
    write_random_id();
  } 
  else if (clear) {
    clear_rfid(&mfrc);
  }
}

void loop() 
{ 
  // Wait for card, exit if no card
  if (!mfrc.PICC_IsNewCardPresent()) return;
  if (!mfrc.PICC_ReadCardSerial()) return;

  String id = rfid_read(&mfrc);
  Serial.println("{ \"id\": \"" + id + "\" }");

  // clear_rfid(&mfrc);
  StaticJsonDocument<200> doc = listen_for_commands();
  handle_commands(doc);

  // Stop RFID device
  mfrc.PICC_HaltA(); 
  mfrc.PCD_StopCrypto1(); 
}
