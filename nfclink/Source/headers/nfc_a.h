//*****************************************************************************
//
// nfc_a.h - NFC-A definitions
//
// Copyright (c) 2015 Texas Instruments Incorporated.  All rights reserved.
// TI Information - Selective Disclosure
//
//*****************************************************************************
#ifndef __NFC_A_H__
#define __NFC_A_H__

#include "trf79x0.h"
#include "nfc_controller.h"
#include "iso_dep.h"

//
// NFC-A List of Commands
//

//        REQUESTS			//
#define	SENS_REQ_CMD			0x26
#define	ALL_REQ_CMD				0x52
#define	SDD_REQ_CMD_CL1			0x93
#define	SDD_REQ_CMD_CL2			0x95
#define	SDD_REQ_CMD_CL3			0x97
#define	SLP_REQ_CMD				0x50

//        RESPONSES				//
#define	SENS_RES_CMD_SINGLE		0x00
#define	SENS_RES_CMD_DOUBLE		0x40
#define	SENS_RES_CMD_TRIPLE		0x80
#define	SDD_RES_CMD_CL1			0x00
#define	SDD_RES_CMD_CL2			0x40
#define	SDD_RES_CMD_CL3			0x80
#define	SEL_RES_CMD				0x01

#define	RATS_CMD				0xE0
#define	PPS_CMD					0xD0

// ATS command bytes
#define ATS_T0_IDX				0x01

// ATS-T0 Masks
#define T0_TA_MASK				0x40
#define T0_TB_MASK				0x20
#define T0_TC_MASK				0x10
#define T0_FSDI_MASK			0x0F

// ATS_TA Masks
#define TA_PICC_TO_PCD_212_KBPS		0x10
#define TA_PICC_TO_PCD_424_KBPS		0x20
#define TA_PICC_TO_PCD_848_KBPS		0x40
#define TA_PCD_TO_PICC_212_KBPS		0x01
#define TA_PCD_TO_PICC_424_KBPS		0x02
#define TA_PCD_TO_PICC_848_KBPS		0x04
#define TA_SAME_BIT_RATE			0x80

#define NFC_A_212_KBPS_RW			0x05
#define NFC_A_424_KBPS_RW			0x0A
#define NFC_A_848_KBPS_RW			0x0F

// ATS_TB Masks
#define TB_SFGI_MASK				0x0F
#define TB_FWI_MASK					0xF0

typedef enum
{
	NFC_A_UID_INCOMPLETE = 0x00,
	NFC_A_UID_SINGLE = 0x04,
	NFC_A_UID_DOUBLE = 0x07,
	NFC_A_UID_TRIPLE = 0x0A
}tUidLength;

typedef enum
{
	NFC_A_IDLE_STATE,
	NFC_A_RATS_STATE,
	NFC_A_PPS_STATE_212,
	NFC_A_PPS_STATE_424,
	NFC_A_PPS_STATE_848,
	NFC_A_ACTIVATED
}tUNfcAState;

typedef struct
{
	bool bNfcDepSupport;
	bool bISODepSupport;
	tUidLength eUidLength;
}NfcAStatus;

//*****************************************************************************
//
// Function Prototypes
//
//*****************************************************************************
extern void NFC_A_setCIDSettings(bool bNFCACIDEnable, uint8_t ui8NfcACID);

extern void NFC_A_init(void);

extern void NFC_A_send_SENS_REQ(void);

extern void NFC_A_send_ALL_REQ(void);

extern void NFC_A_send_SDD_REQ(uint8_t ui8CascadeLevel);

extern void NFC_A_send_SEL_REQ(uint8_t ui8CascadeLevel);

extern void NFC_A_send_SENS_RES(void);

extern void NFC_A_send_SDD_RES(uint8_t ui8CascadeLevel);

extern void NFC_A_send_SEL_RES(uint8_t ui8CascadeLevel);

extern uint8_t NFC_A_processSDDResponse(uint8_t * pui8RxBuffer,
										uint8_t ui8CascadeLevel);

extern uint8_t NFC_A_processSENSRes(uint8_t * pui8RxBuffer,
									uint8_t ui8Length);

extern uint8_t NFC_A_processSELResponse(uint8_t * pui8RxBuffer,
										uint8_t ui8Length);

extern uint8_t NFC_A_check_SEL_REQ(uint8_t ui8CascadeLevel,
								   uint8_t * pui8Payload,
								   uint8_t ui8Length);

extern uint8_t NFC_A_isInitiatorSelected(uint8_t ui8CascadeLevel);

extern uint8_t NFC_A_isTargetSelected(uint8_t ui8CascadeLevel);

extern void NFC_A_setDepSupport(bool bIsoSupport, bool bDepSupport);

extern uint8_t NFC_A_isNfcDepSupported(void);

extern uint8_t NFC_A_isIsoDepSupported(void);

extern void NFC_A_sendRATS(void);

extern void NFC_A_sendPPS_REQ(uint8_t ui8Bitrate);

extern void NFC_A_stateMachine(void);

extern tUNfcAState NFC_A_processReceivedData(uint8_t * pui8RxBuffer,
											 uint8_t ui8Length);

extern void NFC_A_setAutoSDD(bool bAutoSDD);

extern void NFC_A_setNfcAId(uint8_t * ui8NewNfcAId,
							uint8_t ui8Length);

extern uint16_t NFC_A_getSFGT(void);

extern uint16_t NFC_A_getFWT(void);

extern uint8_t NFC_A_getSAK(void);

extern void NFC_A_getNfcAId(uint8_t ** pui8NfcAId,
							uint8_t * ui8NfcAIdLength);

#endif
