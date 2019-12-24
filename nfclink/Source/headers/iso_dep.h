//*****************************************************************************
//
// iso_dep.h - Implementation of ISO DEP layer.
//
// Copyright (c) 2015 Texas Instruments Incorporated.  All rights reserved.
// TI Information - Selective Disclosure
//
//*****************************************************************************
#ifndef __ISO_DEP_H__
#define __ISO_DEP_H__

#include "iso_7816_4.h"
#include "nfc_config.h"
#include "nfc_a.h"
#include "nfc_b.h"

typedef union
{
	struct {
        uint16_t ui3RWMaxNACK		 		: 3;	/*! Maximum number of consecutive ACKs allowed to be sent*/
        uint16_t ui3RWMaxDSL			 	: 3;	/*! Maximum number of consecutive DSLs allowed to be sent*/
        uint16_t ui3RWMaxACK				: 3;	/*! Maximum number of consecutive received ACKs allowed */
        uint16_t ui3RWMaxWTX				: 3;	/*! Maximum number of consecutive received WTX allowed */
        uint16_t reserved					: 4;	/* Reserved */
	}bits;
    uint16_t     ui16byte;             /*! Whole status byte access */
}t_sIsoDEP_RWSetup;

// FWImax = (256*16/fc) * 2^FWImax (FWImax = 14)
#define		FWT_INT_MAX_MS			4949

//*****************************************************************************
//
// List of Commands
//
//*****************************************************************************

#define 	PCD_INDEX								0x00

#define		CID_BIT_MASK							0x08

#define		ISODEP_I_BLOCK_NAD_MASK					0x04
#define		ISODEP_I_BLOCK_CID_MASK					0x08
#define		ISODEP_I_BLOCK_CHAINING_MASK			0x10

#define 	ISODEP_R_BLOCK_CID_MASK					0x08
#define 	ISODEP_R_BLOCK_ACK_MASK					0x10

#define 	ISODEP_S_BLOCK_CID_MASK					0x08
#define 	ISODEP_S_BLOCK_WTX_MASK					0x30

//*****************************************************************************
//
//
//
//*****************************************************************************
typedef enum
{
	I_BLOCK = 		0x02,
	R_ACK_BLOCK = 	0xA2,
	R_NACK_BLOCK = 	0xB2,
	S_DSL_BLOCK = 	0xC2,
	S_WTX_BLOCK = 	0xF2,
	NO_REPLY_BLOCK = 0xFE,
	MUTE_BLOCK =	0xFF
}tPCBBlock;

typedef enum
{
	ISO_DEP_A 	=	0x00,
	ISO_DEP_B 	=	0x01
}tIsoDepType;

typedef enum
{
	ISO_DEP_PARAM_SEL_STATE = 0x00,
	ISO_DEP_PPS_SEL_STATE,
	ISO_DEP_ACTIVATED_STATE,
	ISO_DEP_DEACTIVATED_STATE,
	ISO_DEP_PROTOCOL_ERROR
}tIsoDepState;


//*****************************************************************************
//
// Function Prototypes
//
//*****************************************************************************
extern void ISODEP_configure_RW(t_sIsoDEP_RWSetup sRWSetupOptions,
								uint8_t ui8IsoDepDid);

extern void ISODEP_init(bool bPCDModeEnable);

extern void ISODEP_stateMachine(void);

extern tIsoDepState ISODEP_processReceivedRequest(uint8_t * pui8RxPayload ,
												  uint8_t ui8CmdLength);

extern tIsoDepState ISODEP_processReceivedData(uint8_t * pui8RxBuffer,
											   uint8_t ui8Length);

extern uint16_t ISODEP_getLinkTimeOut(void);

extern void ISODEP_setLinkTimeOut(uint16_t ui16IsoDepTimeout);

extern void ISODEP_setMaxPayload(uint16_t ui16MaxPayload);

extern void ISODEP_setNFCType(tIsoDepType eNfcType);

extern void ISODEP_sendATS(void);

extern void ISODEP_sendI_Block_REQ(void);

extern void ISODEP_sendR_Block_REQ(void);

extern void ISODEP_sendS_Block_REQ(void);

extern void ISODEP_sendPPS_RES(void);

extern void ISODEP_sendDSL_RES(void);

extern void ISODEP_sendWTX_REQ(void);

extern void ISODEP_sendWTX_RES(void);

extern uint8_t ISODEP_triggerProtocolError(void);

extern uint8_t ISODEP_getCID(void);

extern void ISODEP_sendDeselect(void);

#endif //__ISO_DEP_H__
