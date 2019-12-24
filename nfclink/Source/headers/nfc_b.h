//*****************************************************************************
//
// nfc_b.h - NFC-B definitions
//
// Copyright (c) 2015 Texas Instruments Incorporated.  All rights reserved.
// TI Information - Selective Disclosure
//
//*****************************************************************************
#ifndef __NFC_B_H__
#define __NFC_B_H__

#include "trf79x0.h"
#include "nfc_controller.h"
#include "iso_dep.h"

//
// NFC-B List of Commands
//

//        REQUESTS			//
#define	SENSB_REQ_CMD			0x05
#define	ALLB_REQ_PARAM_MASK		0x08
#define	SLOT_MARKER				0x93
#define	SLPB_REQ_CMD			0x50
#define ATTRIB_CMD				0x1D

//        RESPONSES				//
#define	SENSB_RES_CMD			0x50
#define	SLPB_RES_CMD			0x00

#define NFC_B_PICC_TO_PCD_212_KBPS		0x10
#define NFC_B_PICC_TO_PCD_424_KBPS		0x20
#define NFC_B_PICC_TO_PCD_848_KBPS		0x40
#define NFC_B_PCD_TO_PICC_212_KBPS		0x01
#define NFC_B_PCD_TO_PICC_424_KBPS		0x02
#define NFC_B_PCD_TO_PICC_848_KBPS		0x04
#define NFC_B_SAME_BIT_RATE				0x80


// SENSB_RES MASKS
#define SENSB_RES_AFI_INDEX				0x05
#define SENSB_RES_CRC_1_INDEX			0x06
#define SENSB_RES_CRC_2_INDEX			0x07
#define SENSB_RES_NUMBER_APP_INDEX		0x08
#define SENSB_RES_BIT_RATE_INDEX		0x09
#define SENSB_RES_MAX_FRAME_PROT_INDEX	0x0A
#define SENSB_RES_FWI_ADC_FO_INDEX		0x0B


typedef enum
{
	NFC_B_POLL_SENSB_REQ,
	NFC_B_POLL_SLPB_REQ,
	NFC_B_POLL_ATTRIB_REQ,
	NFC_B_POLL_WAKEUP,
	NFC_B_POLL_ACTIVATED,
	NFC_B_PROTOCOL_ERROR
}tNfcBPollState;

//*****************************************************************************
//
// Function Prototypes
//
//*****************************************************************************
extern void NFC_B_init(void);

// RW Commands

extern void NFC_B_send_SENSB_REQ(void);

extern void NFC_B_send_ALLB_REQ(void);

extern void NFC_B_send_SLPB_REQ(void);

extern void NFC_B_send_ATTRIB_REQ(void);

extern void NFC_B_stateMachine(void);

extern tNfcBPollState NFC_B_processReceivedRsp(uint8_t * pui8RxBuffer,
											   uint8_t ui8Length);

extern void NFC_B_send_SENSB_RES(void);

extern void NFC_B_send_SLPB_RES(void);

extern void NFC_B_send_ATTRIB_RES(void);

extern uint8_t NFC_B_getSENSBN(uint8_t * pui8Payload,
							   uint8_t ui8Length);

extern uint8_t NFC_B_checkSENSBAfi(uint8_t * puiPayload,
								   uint8_t ui8Length);

extern uint8_t NFC_B_checkAttribId(uint8_t * puiPayload,
								   uint8_t ui8Length);

extern uint8_t NFC_B_checkAttribParam(uint8_t * puiPayload,
									  uint8_t ui8Length);

extern uint8_t NFC_B_checkSLPBId(uint8_t * puiPayload,
								 uint8_t ui8Length);

extern uint8_t NFC_B_setNfcBId(uint8_t * ui8NewNfcBId,
							   uint8_t ui8Length);

extern void NFC_B_getNfcBId(uint8_t ** pui8NfcBId,
							uint8_t * ui8NfcBIdLength);

extern uint16_t NFC_B_getSFGT(void);

extern uint16_t NFC_B_getFWT(void);

extern bool NFC_B_checkDIDSupported(void);

extern bool NFC_B_isSelected(void);

extern bool NFC_B_isISOCompliant(void);

#endif
