#include <MFRC522.h>
#include <SPI.h>

#define RFID_SIZE_BUFFER     18
#define RFID_MAX_SIZE_BLOCK  16


MFRC522::MIFARE_Key key;
MFRC522::StatusCode status;

String remove_invalid_chars(String str) {
  String result;
  for (int i = 0; i < str.length(); i++) {
    char c = str.charAt(i);
    if (c >= 0 && c < 128) result += c;
  }

  result = result.substring(0, result.indexOf(' '));
  result.trim();
  return result;
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
    digitalWrite(redPin, HIGH);
    delay(1000);
    digitalWrite(redPin, LOW);
    return;
  }

  // Read data from block
  status = mfrc->MIFARE_Read(block, buffer, &size);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Reading failed: "));
    Serial.println(mfrc->GetStatusCodeName(status));
    digitalWrite(redPin, HIGH);
    delay(1000);
    digitalWrite(redPin, LOW);
    return;
  }
  else{
      Serial.println("Card OK");
  }

  return remove_invalid_chars((String)(char*)buffer);
}


String parse_card_id(String data) {
  // card data: name=some_name
  return data.substring(data.indexOf('=') + 1);
}

void rfid_write(MFRC522* mfrc) {
  // mfrc->PICC_DumpDetailsToSerial(&(mfrc->uid)); 
  
  // Waits 30 seconds dor data entry via Serial 
  Serial.setTimeout(30000L) ;     
  Serial.println(F("Enter the data to be written with the '#' character at the end \n[maximum of 16 characters]:"));

  // Prepare the key - all keys are set to FFFFFFFFFFFFh
  for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;

  // Buffer for storing data to write
  byte buffer[RFID_MAX_SIZE_BLOCK] = "";
  byte block = 1;
  byte dataSize;

  // Recover on buffer the data from Serial all characters before chacactere '#'
  dataSize = Serial.readBytesUntil('#', (char*)buffer, RFID_MAX_SIZE_BLOCK);

  // Void positions that are left in the buffer will be filled with whitespace
  for(byte i=dataSize; i < RFID_MAX_SIZE_BLOCK; i++) buffer[i] = ' ';
 
  String str = (char*)buffer;
  Serial.println(str);

  // Auth for block
  status = mfrc->PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A,
                                    block, &key, &(mfrc->uid));

  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("PCD_Authenticate() failed: "));
    Serial.println(mfrc->GetStatusCodeName(status));
    digitalWrite(redPin, HIGH);
    delay(1000);
    digitalWrite(redPin, LOW);
    return;
  }
 
  // Writes in the block
  status = mfrc->MIFARE_Write(block, buffer, RFID_MAX_SIZE_BLOCK);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("MIFARE_Write() failed: "));
    Serial.println(mfrc->GetStatusCodeName(status));
    digitalWrite(redPin, HIGH);
    delay(1000);
    digitalWrite(redPin, LOW);
    return;
  }
  else{
    Serial.println("Write success");
  }
}

