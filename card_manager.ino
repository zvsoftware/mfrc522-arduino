#include "rfid.h"

// SS_PIN IS BOARD DEPENDANT!!
#define SS_PIN 10 
#define RST_PIN  5

MFRC522 mfrc(SS_PIN, RST_PIN);

void setup() 
{
  Serial.begin(115200); 
  delay(1500);
  Serial.println("STARTING UP");
  SPI.begin();
  
  mfrc.PCD_Init(); 
  Serial.println("\nScan your card.");
  Serial.println();
}

void loop() 
{
  // Wait for card, exit if no card
  if (!mfrc.PICC_IsNewCardPresent()) return;
  if (!mfrc.PICC_ReadCardSerial()) return;

  // Card availible, read it.
  String rfid_data = rfid_read(&mfrc);
  String parsed_rfid = parse_card_id(rfid_data);

  Serial.println(parsed_rfid);

  // Stop RFID device
  mfrc.PICC_HaltA(); 
  mfrc.PCD_StopCrypto1(); 
}
