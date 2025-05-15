#ifndef LORA_CONFIG_H
#define LORA_CONFIG_H

#include "Arduino.h"
#include <WiFi.h>
#include "LoRaWan_APP.h"

/* OTAA para */
extern uint8_t devEui[];
extern uint8_t appEui[];
extern uint8_t appKey[];

/* ABP para */
extern uint8_t nwkSKey[];
extern uint8_t appSKey[];
extern uint32_t devAddr;

extern uint16_t userChannelsMask[6];
extern LoRaMacRegion_t loraWanRegion;
extern DeviceClass_t loraWanClass;
extern uint32_t appTxDutyCycle;
extern bool overTheAirActivation;
extern bool loraWanAdr;
extern bool isTxConfirmed;
extern uint8_t appPort;
extern uint8_t confirmedNbTrials;

extern uint8_t appData[];

#define NO_WIFI_NODES_NEEDED    (uint8_t)(3)
#define NO_MAX_WIFI_NODES       (uint8_t)(5)

/* function declarations */
void prepareWifiTxFrame(uint8_t, const String);
bool isUAA(String);
char hex_char_to_bin(char);
char str_to_hex_to_dec(char);
String collect_wifi_data(void);
void lora_send(char status, char percentage);

#endif


