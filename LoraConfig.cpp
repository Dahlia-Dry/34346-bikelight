#include "LoraConfig.h"
#include <iostream>
#include <cstring>
#include <fstream>
using namespace std;

uint8_t devEui[] = { 0x00, 0x04, 0xA3, 0x0B, 0x01, 0x06, 0x8C, 0xD7 };
uint8_t appEui[] = {0x70, 0xB3, 0xD5, 0x7E, 0xD0, 0x07, 0x01, 0x2B};
uint8_t appKey[] = { 0xA7, 0x82, 0x2A, 0xF3, 0x38, 0xBE, 0x81, 0x67, 0xFC, 0x2B, 0xB0, 0x99, 0x24, 0x23, 0x02, 0x10};

uint8_t nwkSKey[] = { 0x15, 0xb1, 0xd0, 0xef, 0xa4, 0x63, 0xdf, 0xbe, 0x3d, 0x11, 0x18, 0x1e, 0x1e, 0xc7, 0xda, 0x85 };
uint8_t appSKey[] = { 0xd7, 0x2c, 0x78, 0x75, 0x8c, 0xdc, 0xca, 0xbf, 0x55, 0xee, 0x4a, 0x77, 0x8d, 0x16, 0xef, 0x67 };
uint32_t devAddr =  (uint32_t)0x007e6ae1;

uint16_t userChannelsMask[6] = { 0x00FF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 };
LoRaMacRegion_t loraWanRegion = ACTIVE_REGION;
DeviceClass_t loraWanClass = CLASS_A;
uint32_t appTxDutyCycle = 15000;
bool overTheAirActivation = true;
bool loraWanAdr = true;
bool isTxConfirmed = true;
uint8_t appPort = 6;
uint8_t confirmedNbTrials = 4;

/* Checking if the MAC address is unique */
bool isUAA(String MACAddr) {
  return !(((str_to_hex_to_dec(MACAddr.charAt(1)) & 0x2)!=0) || (MACAddr.startsWith("00:00:5E")) || (MACAddr.startsWith("84:3D")));
}

/* str to dec converter */
char str_to_hex_to_dec(char hex_str) {
  switch (hex_str) {
      case '0': return 0;
      case '1': return 1;
      case '2': return 2;
      case '3': return 3;
      case '4': return 4;
      case '5': return 5;
      case '6': return 6;
      case '7': return 7;
      case '8': return 8;
      case '9': return 9;
      case 'A': case 'a': return 10;
      case 'B': case 'b': return 11;
      case 'C': case 'c': return 12;
      case 'D': case 'd': return 13;
      case 'E': case 'e': return 14;
      case 'F': case 'f': return 15;
      default:  return -1;
  }
}

/* hexa char to binary converter */
char hex_char_to_bin(char c)
{
    switch(toupper(c))
    {
        case '0': return 0;
        case '1': return 1;
        case '2': return 2;
        case '3': return 3;
        case '4': return 4;
        case '5': return 5;
        case '6': return 6;
        case '7': return 7;
        case '8': return 8;
        case '9': return 9;
        case 'A': return 10;
        case 'B': return 11;
        case 'C': return 12;
        case 'D': return 13;
        case 'E': return 14;
        case 'F': return 15;
        default: return 0xFF;
    }
}

// Prepares the payload of the frame 
void prepareWifiTxFrame(uint8_t port, const String txFrame) {
  auto size = txFrame.length();
  memset(appData, 0, LORAWAN_APP_DATA_MAX_SIZE);
  appData[0] = txFrame[0];//status bit
  appData[1] = txFrame[1];//battery percentage
  size_t j = 2;
  //mac address compression
  for (auto i = 2; i < size; i+=2, j++) {
      appData[j] = (hex_char_to_bin(txFrame[i]) << 4) | hex_char_to_bin(txFrame[i+1]);
  }
  appDataSize = j;
  //Serial.println(appDataSize);
}


/* Collect wifi data and return it as a heavily truncated string */
String collect_wifi_data(void) {
    String ret = "";
    int numOfNetworks = WiFi.scanNetworks();
    if (numOfNetworks >= NO_WIFI_NODES_NEEDED) {
        int validMACCntr = 0;
        String truncatedWifiNodes = "";
        /* Read through all the found nodes up to a threshold */
        for (int i = 0; i < _min(numOfNetworks, NO_MAX_WIFI_NODES); i++) {
        String currentMAC = WiFi.BSSIDstr(i);
        int32_t currentRSSI = WiFi.RSSI(i);
        /* Check valid MAC addresses */
        if (isUAA(currentMAC)) {
            Serial.println("MAC: " + currentMAC + " RSSI: " + String(currentRSSI)); //debug
            char hexRSSI[3];
            sprintf(hexRSSI, "%02X", uint8_t(-currentRSSI));
            truncatedWifiNodes += currentMAC + hexRSSI;
            truncatedWifiNodes.replace(":", "");
            validMACCntr++;
        }
        }
        /* Update tx frame only if we have enough wifi nodes */
        if (validMACCntr >= NO_WIFI_NODES_NEEDED) {
        ret = truncatedWifiNodes;
        }
    }
    return ret;
}

void lora_send(char status, char percentage) {
    switch (deviceState) {
          case DEVICE_STATE_INIT:
          {
              #if(LORAWAN_DEVEUI_AUTO)
                  LoRaWAN.generateDeveuiByChipID();
              #endif
              LoRaWAN.init(loraWanClass, loraWanRegion);
              LoRaWAN.setDefaultDR(3);
              //deviceState = DEVICE_STATE_JOIN; // advance to next state
              break;
          }
          case DEVICE_STATE_JOIN:
          {
              LoRaWAN.join();
              //deviceState = DEVICE_STATE_SEND; // advance to next state
              break;
          }
          case DEVICE_STATE_SEND:
          {
              String wifinodes = "";
              wifinodes += status;
              wifinodes += percentage;
              wifinodes += collect_wifi_data();

              prepareWifiTxFrame(appPort, wifinodes);
              LoRaWAN.send();
              deviceState = DEVICE_STATE_CYCLE;
              return;
              //break;
          }
          case DEVICE_STATE_CYCLE:
          {
              txDutyCycleTime = appTxDutyCycle + randr(-APP_TX_DUTYCYCLE_RND, APP_TX_DUTYCYCLE_RND);
              LoRaWAN.cycle(txDutyCycleTime);
              deviceState = DEVICE_STATE_SLEEP;
              break;
          }
          case DEVICE_STATE_SLEEP:
          {
              LoRaWAN.sleep(loraWanClass);
              // End of cycle reached, return from function
              //return;
              break;
          }
          default:
          {
              deviceState = DEVICE_STATE_INIT;
              break;
          }
      }
}
