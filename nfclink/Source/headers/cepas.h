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

typedef union cepas{
    uint8_t version;
    uint8_t purseStatus;
    uint8_t purseBalance[3];
    uint8_t autoloadAmt[3];
    uint64_t can;
    uint64_t csn;
    uint16_t purseExp;
    uint16_t purseCreate;
    uint32_t lastCreditTrp;
    uint64_t lastCreditHeader;
    uint8_t numTrans;
    uint8_t IssuerDataLen;
    uint32_t lastTransTRP;
    uint64_t lastTransHeader;
    uint16_t issuerDataPtr;
}CEPAS;

CEPAS card;

extern void initCEPAS();
extern void processCommand(uint8_t * pui8RxPayload , uint8_t ui8CmdLength);
