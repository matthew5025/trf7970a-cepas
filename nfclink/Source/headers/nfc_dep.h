//*****************************************************************************
//
// nfc_dep.h - Defines for sending packets of P2P
//
// Copyright (c) 2015 Texas Instruments Incorporated.  All rights reserved.
// TI Information - Selective Disclosure
//
//*****************************************************************************
#ifndef __NFC_DEP_H__
#define __NFC_DEP_H__

#include "nfc_controller.h"
#include "nfc_config.h"
#include "llcp.h"
#include "nfc_p2p.h"
#include "trf79x0.h"

//*****************************************************************************
//
// List of Commands
//
//*****************************************************************************
//        REQUESTS			//
#define	ATR_REQ_CMD		0xD400
#define PSL_REQ_CMD		0xD404
#define DEP_REQ_CMD		0xD406
#define DSL_REQ_CMD		0xD408
#define RSL_REQ_CMD		0xD40A

//        RESPONSES			//
#define ATR_RES_CMD		0xD501
#define PSL_RES_CMD		0xD505
#define DEP_RES_CMD		0xD507
#define DSL_RES_CMD		0xD509
#define RSL_RES_CMD		0xD50B

#define		BSi				0x00
#define		BRi				0x00
//*****************************************************************************
//
// Initiator Maximum payload size + General bytes available (BIT1)
// B6 B5 - '00' Max Payload 64 bytes
// B6 B5 - '01' Max Payload 128 bytes
// B6 B5 - '10' Max Payload 192 bytes
// B6 B5 - '11' Max Payload 254 bytes (default)
//
//*****************************************************************************
#define		PPi				0x30

#define		BSt				0x00
#define		BRt				0x00
#define		TO				0x08
//*****************************************************************************
//
// Target Maximum payload size + General bytes available (BIT1)
// B6 B5 - '00' Max Payload 64 bytes
// B6 B5 - '01' Max Payload 128 bytes
// B6 B5 - '10' Max Payload 192 bytes
// B6 B5 - '11' Max Payload 254 bytes (default)
//
//*****************************************************************************
#define		PPt						0x30

#define		NFC_DEP_CHAIN_ENABLE	0x10
#define		NFC_DEP_DID_ENABLE		0x04
//*****************************************************************************

// RWTmax = (256*16/fc) * 2^WTmax (WTmax = 14)
#define		RWT_INT_MAX_MS			4949
//
//
//
//*****************************************************************************
typedef enum
{
	INFORMATION_PDU = 		0x00,
	ACK_PDU	= 				0x40,
	NACK_PDU =	 			0x50,
	ATN_PDU = 				0x80,
	RTOX_REQ_PDU =	 		0x90,
	NO_REPLY_PDU = 			0xFF
}tPDUBlock;

typedef enum
{
	DEP_ATR_REQ_STATE = 0x00,
	DEP_PSL_REQ_STATE,
	DEP_ACTIVATED_STATE,
	DEP_DEACTIVATED_STATE,
	DEP_DESELECTED_STATE
}tDepState;

typedef union
{
	struct {
		uint8_t bP2PSupportLLCP  			: 1;    /*! Support LLCP Protocol */
		uint8_t bP2PSupportLoopback 		: 1;    /*! Support Loopback for DEP packets */
        uint8_t ui3P2PMaxTimeouts	 		: 3;	/*! Maximum number of Timeouts allowed before reset */
        uint8_t ui3P2PMaxProtocolErrors 	: 3;	/*! Maximum number of Protocol Errors allowed before reset */
	}bits;
    uint8_t     ui8byte;             /*! Whole status byte access */
}t_sNfcDEP_P2PSetup;



//*****************************************************************************
//
// Function Prototypes
//
//*****************************************************************************
extern void NFCDEP_configure_P2P(t_sNfcDEP_P2PSetup sP2PSetupOptions,
								 bool bTestEnable,
								 uint8_t ui8DIDi);

extern void NFCDEP_init();

extern void NFCDEP_sendATR_REQ(void);

extern void NFCDEP_sendDEP_REQ(void);

extern void NFCDEP_sendPSL_REQ(void);

extern void NFCDEP_sendRSL_REQ(void);

extern void NFCDEP_sendDSL_REQ(void);

extern uint8_t NFCDEP_sendATR_RES(void);

extern void NFCDEP_sendDEP_RES(void);

extern void NFCDEP_sendPSL_RES(void);

extern void NFCDEP_sendRSL_RES(void);

extern void NFCDEP_sendDSL_RES(void);

extern uint16_t NFCDEP_getDEPCommand(uint8_t * puiRxPayload,
									 uint8_t ui8CmdLength);

extern void NFCDEP_stateMachine(void);

extern uint8_t NFCDEP_processReceivedRequest(uint8_t * pui8RxPayload ,
											 uint8_t ui8CmdLength);

extern tDepState NFCDEP_processReceivedData(uint8_t * pui8RxBuffer,
											uint8_t ui8Length);

extern t_sTRF79x0_Bitrate NFCDEP_getBitrate(void);

extern uint16_t NFCDEP_getLinkTimeOut(void);

extern void NFCDEP_setNFCID3t(uint8_t * pui8NFCID3,uint8_t ui8Length);

extern void NFCDEP_setNFCID2i(uint8_t * pui8NFCID2, uint8_t ui8Length);

extern uint8_t NFCDEP_sendSOT(uint8_t ui8Offset);

extern bool NFCDEP_checkEOT(uint8_t * pui8RxBuffer, uint8_t ui8Length);

extern uint8_t NFCDEP_triggerTimeout(void);

extern uint8_t NFCDEP_triggerProtocolError(void);

#endif //__NFC_DEP_H__
