#include "RFIDReader.h"
#include <SPI.h>

RFIDReader::RFIDReader(uint8_t sckPin, uint8_t misoPin, uint8_t mosiPin, uint8_t ssPin, uint8_t rstPin)
    : _sckPin(sckPin), _misoPin(misoPin), _mosiPin(mosiPin), _ssPin(ssPin), _rstPin(rstPin), mfrc522(ssPin, rstPin) {}

void RFIDReader::setup() {
    SPI.begin(_sckPin, _misoPin, _mosiPin, _ssPin);
    mfrc522.PCD_Reset();
    mfrc522.PCD_Init();

    Serial.println("RFID Access Control System initialized");
    Serial.println("Ready to read card\n");
}

String RFIDReader::readUID() {
    String content = "";
    for (byte i = 0; i < mfrc522.uid.size; i++) {
        content += String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
        content += String(mfrc522.uid.uidByte[i], HEX);
    }
    content.toUpperCase();
    return content.substring(1);
}

bool RFIDReader::detectCard(){
  if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
        return false;
  }
  else{
        return true;
  }
}

bool RFIDReader::checkAccess(const String& allowedUID1, const String& allowedUID2) {
    Serial.print("UID tag:");
    String uid = readUID();
    Serial.println(uid);

    if (uid == allowedUID1 || uid == allowedUID2) {
        Serial.println("ACCESS GRANTED.\n");
        return true;
    } else {
        Serial.println("ACCESS DENIED!\n");
        return false;
    }
}
