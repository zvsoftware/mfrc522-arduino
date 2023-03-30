#include <MFRC522.h>
#include <SPI.h>

#define RFID_SIZE_BUFFER     18
#define RFID_MAX_BLOCK_SIZE  16


MFRC522::MIFARE_Key key;
MFRC522::StatusCode status;


String rfid_read(MFRC522* mfrc)
{ 
  // mfrc->PICC_DumpDetailsToSerial(&(mfrc->uid)); 
  
  // Prepare the key - all keys are set to FFFFFFFFFFFFh
  for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;
  
  byte buffer[RFID_SIZE_BUFFER] = {0};
 
  //the block to operate
  byte block = 1;
  byte size = RFID_SIZE_BUFFER;  

  // Auth for block
  status = mfrc->PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(mfrc->uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Authentication failed: "));
    Serial.println(mfrc->GetStatusCodeName(status));
    return;
  }

  // Read data from block
  status = mfrc->MIFARE_Read(block, buffer, &size);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Reading failed: "));
    Serial.println(mfrc->GetStatusCodeName(status));
    return;
  }
  else{
      Serial.println("Card OK");
  }

  return String((char*)buffer).substring(0, 16);
}


void rfid_write(MFRC522* mfrc, String id, byte block = 1) {
  // mfrc->PICC_DumpDetailsToSerial(&(mfrc->uid)); 
  
  // Waits 30 seconds dor data entry via Serial 
  Serial.setTimeout(30000L) ;     
  Serial.println(F("Enter the data to be written with the '#' character at the end \n[maximum of 16 characters]:"));

  // Prepare the key - all keys are set to FFFFFFFFFFFFh
  for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;

  // Buffer for storing data to write
  byte buffer[RFID_MAX_BLOCK_SIZE] = "";
  
  for (int i = 0; i < RFID_SIZE_BUFFER; i++) buffer[i] = id[i];

  // Void positions that are left in the buffer will be filled with whitespace
  for(byte i = id.length(); i < RFID_MAX_BLOCK_SIZE; i++) buffer[i] = ' ';
 
  String str = (char*)buffer;
  Serial.println(str);

  // Auth for block
  status = mfrc->PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A,
                                    block, &key, &(mfrc->uid));

  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("PCD_Authenticate() failed: "));
    Serial.println(mfrc->GetStatusCodeName(status));
    return;
  }
 
  // Writes in the block
  status = mfrc->MIFARE_Write(block, buffer, RFID_MAX_BLOCK_SIZE);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("MIFARE_Write() failed: "));
    Serial.println(mfrc->GetStatusCodeName(status));
    return;
  }
  else{
    Serial.println("Write success");
  }
}

