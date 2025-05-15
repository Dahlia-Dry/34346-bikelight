#ifndef RFIDREADER_H
#define RFIDREADER_H

#include <Arduino.h>
#include <MFRC522.h>

class RFIDReader {
public:
    // Constructor takes all SPI pins and reset pin
    RFIDReader(uint8_t sckPin, uint8_t misoPin, uint8_t mosiPin, uint8_t ssPin, uint8_t rstPin);

    void setup();
    bool detectCard();
    bool checkAccess(const String& allowedUID1, const String& allowedUID2);

private:
    uint8_t _sckPin;
    uint8_t _misoPin;
    uint8_t _mosiPin;
    uint8_t _ssPin;
    uint8_t _rstPin;

    MFRC522 mfrc522;

    String readUID();
};

#endif
