#include "rfid.h"

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

void loop() 
{
  delay(100);
  // Wait for card, exit if no card
  if (!mfrc.PICC_IsNewCardPresent()) return;
  if (!mfrc.PICC_ReadCardSerial()) return;
  Serial.println("HERE!");

  // Card availible, read it.
  String id = rfid_read(&mfrc);
  Serial.println(id);

  // Stop RFID device
  mfrc.PICC_HaltA(); 
  mfrc.PCD_StopCrypto1(); 
}
