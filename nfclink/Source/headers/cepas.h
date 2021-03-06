/*
 * cepas.h
 *
 *  Created on: 28 Dec 2019
 *      Author: natsa
 */

#ifndef NFC_CEPAS_H_
#define NFC_CEPAS_H_
#include "stdint.h"


#endif /* NFC_CEPAS_H_ */

typedef union {
     struct cepas{
        uint8_t version;
        uint8_t purseStatus;
        uint8_t purseBalance[3];
        uint8_t autoloadAmt[3];
        uint8_t can[8];
        uint8_t csn[8];
        uint8_t purseExp[2];
        uint8_t purseCreate[2];
        uint8_t lastCreditTrp[4];
        uint8_t lastCreditHeader[8];
        uint8_t numTrans;
        uint8_t IssuerDataLen;
        uint8_t lastTransTRP[4];
        uint8_t lastTransRec[16];
        void* issuerDataPtr;
        void* transPtr;
    }purse;
}CEPAS;

typedef struct TRANS{
    uint8_t type;
    uint8_t amount[3];
    uint8_t dateTime[4];
    uint8_t userData[8];
    void* nextTrans;
}TRANS;

typedef enum
{
    WRONG_LENGTH = 0x6700,
    COND_NOT_SASTFD = 0x6985,
    INCORRECT_DATA = 0x6A80,
    APP_NOT_FOUND = 0x6A82,
    COMMAND_SUCCESS = 0x9000
}apduResp;


uint8_t response[256];
apduResp responseCode;
uint8_t resLength;

extern CEPAS* initCEPAS();
extern void processCommand(CEPAS* card, uint8_t * pui8RxPayload , uint8_t ui8CmdLength);
extern void getResponse(uint8_t * pui8PduBufferPtr);
extern uint8_t getResponseLength();
