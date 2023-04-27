#include <MFRC522.h>
#include <SPI.h>

#define RFID_SIZE_BUFFER     18
#define RFID_MAX_BLOCK_SIZE  16


MFRC522::MIFARE_Key key;
MFRC522::StatusCode status;

String remove_non_digits(String str) {
  String out = "";
  for (int i = 0; i < str.length(); i++) {
    if (!isDigit(str.charAt(i))) continue;
    out += str.charAt(i);
  }

  return out;
}

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

  String str = String((char*)buffer).substring(0, 16);
  return remove_non_digits(str);
}


void rfid_write(MFRC522* mfrc, String data, byte block = 1) {
  if (data.length() > RFID_MAX_BLOCK_SIZE) return;
  // mfrc->PICC_DumpDetailsToSerial(&(mfrc->uid)); 
  
  // Prepare the key - all keys are set to FFFFFFFFFFFFh
  for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;

  // Buffer for storing data to write
  char buffer[data.length()] = "";
  if (data.length() > 0) {
    
    for (int i = 0; i < RFID_SIZE_BUFFER; i++) buffer[i] = data.charAt(i);
  }

  // Auth for block
  status = mfrc->PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(mfrc->uid));

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
    Serial.println("{ \"data\": \"" + data + "\"}");
    Serial.println("Write success");
  }
}

void clear_rfid(MFRC522* mfrc) {
  rfid_write(mfrc, "");
}

