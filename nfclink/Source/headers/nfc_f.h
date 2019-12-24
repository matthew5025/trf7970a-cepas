//*****************************************************************************
//
// nfc_f.h - NFC-F definitions
//
// Copyright (c) 2015 Texas Instruments Incorporated.  All rights reserved.
// TI Information - Selective Disclosure
//
//*****************************************************************************
#ifndef __NFC_F_H__
#define __NFC_F_H__

#include "trf79x0.h"

//
// List of Commands
//
#define	SENSF_REQ_CMD		0x00
#define	SENSF_RES_CMD		0x01

typedef struct
{
	bool bNfcDepSupoort;
}NfcFStatus;

//*****************************************************************************
//
// Function Prototypes
//
//*****************************************************************************
extern void NFC_F_Init(void);

extern void NFC_F_send_SENSF_REQ(uint16_t ui16SC,
								 uint8_t ui8RC,
								 uint8_t ui8Slot);

extern void NFC_F_send_SENSF_RES(uint8_t ui8RDValue);

extern uint8_t NFC_F_processReceivedRequest(uint8_t * pui8RxBuffer,
											uint8_t ui8Length);

extern void NFC_F_getNFCID2(uint8_t ** pui8NfcFId,
							uint8_t * ui8NfcFIdLength);

extern void NFC_F_setNfcDepSupport(bool bDepSupport);

extern uint8_t NFC_F_isNfcDepSupported(void);

extern uint8_t NFC_F_setNfcId2(uint8_t * ui8NewNfcId2,
							   uint8_t ui8Length);

#endif
