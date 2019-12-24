//*****************************************************************************
//
// nfc_controller.h - Implementation of NFC Controller APIs.
//
// Copyright (c) 2015 Texas Instruments Incorporated.  All rights reserved.
// TI Information - Selective Disclosure
//
//*****************************************************************************

#ifndef __NFC_CONTROLLER_H__
#define __NFC_CONTROLLER_H__

#include "trf79x0.h"
#include "nfc_p2p.h"
#include "nfc_ce.h"
#include "nfc_rw.h"
#include "nfc_config.h"
#include "nfc_initiator.h"
#include "nfc_target.h"

//*****************************************************************************

#define NFC_FW_VERSION		"1.03.00"

// Commands NFC Host <-- NFC Controller
typedef enum
{
	// General NFC commands
	COMM_START 			= 0x80,
	COMM_END 			= 0x81,
	NFC_TEST_CONFIG 	= 0x82,
	TRF_SETTINGS  		= 0x83,

	// P2P Commands
	P2P_START_CMD 		= 0xA0,
	P2P_STOP_CMD 		= 0xA1,
	P2P_PUSH_PAYLOAD 	= 0xA2,

	// CE Commands
	CE_START_CMD 		= 0xC0,
	CE_STOP_CMD 		= 0xC1,
	CE_NDEF_CONFIG 		= 0xC2,

	// RW Commands
	RW_START_CMD 		= 0xE0,
	RW_STOP_CMD 		= 0xE1,
	RW_FORMAT_TAG 		= 0xE2,
	RW_WRITE_TAG 		= 0xE3

}tNFCControllerCommands;

// States for all NFC protocol state machines
typedef enum {
	NFC_STATE_IDLE = 0,
	NFC_TARGET_WAIT_FOR_ACTIVATION,
	NFC_PROTOCOL_ACTIVATION,
	NFC_PARAMETER_SELECTION,
	NFC_DATA_EXCHANGE_PROTOCOL,
	NFC_MIFARE_DATA_EXCHANGE_PROTOCOL,
	NFC_DEACTIVATION,
	NFC_DISABLED
} tNfcState;


typedef union {
	struct
	{
		uint8_t bNfcModePoll 	: 1;
		uint8_t bNfcModeListen 	: 1;
		uint8_t bReserved		: 1;
	}bits;
	uint8_t ui8Byte;
} tNfcMode;

//*****************************************************************************
//
// Function Prototypes
//
//*****************************************************************************

// NFC Controller APIs

extern void NFC_init(void);

extern void NFC_setListenTime(uint16_t ui16ListenTime);

extern tNfcState NFC_run(void);

extern tNfcState NFC_pollStateMachine(void);

extern tNfcState NFC_listenStateMachine(void);

extern void NFC_disable(void);

extern void NFC_setSupportCertification(bool bSupportCertification);

extern tTRF79x0_IRQFlag NFC_waitForCommand(uint16_t ui16TimeOut);

// End of Controller APIs


// NFC Peer to Peer APIs

extern bool NFC_P2P_getModeStatus(t_sNfcP2PMode * psP2PMode,
						   t_sNfcP2PCommBitrate * psP2PBitrate);

extern void NFC_P2P_configure(t_sNfcP2PMode sP2PModes,
							  t_sNfcP2PCommBitrate sP2PTargetBitrates,
							  t_sNfcP2PCommBitrate sP2PInitiatorBitrates);

extern void NFC_P2P_disable(void);

extern tNfcP2PRxStatus NFC_P2P_getReceiveState(void);

extern uint8_t NFC_P2P_sendNdefPacket(uint8_t * pui8Ndef,
									  bool bPacketStart,
									  uint8_t ui8FragmentLength,
									  uint32_t ui32PacketSize);

extern void NFC_P2P_setModes(t_sNfcP2PMode sP2PModes);

extern void NFC_P2P_setTargetBitrates(t_sNfcP2PCommBitrate sP2PTargetBitrates);

extern void NFC_P2P_setInitiatorBitrates(t_sNfcP2PCommBitrate sP2PInitiatorBitrates);

extern t_sNfcP2PCommBitrate NFC_P2P_getInitiatorSupportedBitrates(void);

extern t_sNfcP2PCommBitrate NFC_P2P_getInitiatorBitrate(void);

// End of Peer to Peer APIs


// RW APIs

extern bool NFC_RW_getModeStatus(t_sNfcRWMode * psRWMode,
						  t_sNfcRWCommBitrate * psRWBitrate);

extern void NFC_RW_configure(t_sNfcRWMode sRWModes,
					  t_sNfcRWCommBitrate sRWBitrates);

extern void NFC_RW_disable(void);

extern t_sNfcRWCommBitrate NFC_RW_getRWSuportedBitrates(void);

extern void NFC_RW_triggerRWProtocolError(void);

extern void NFC_RW_enableMifareMode(void);

// End of RW APIs


// NFC Card Emulation APIs

extern bool NFC_CE_getModeStatus(t_sNfcCEMode * psCEMode);

extern void NFC_CE_configure(t_sNfcCEMode sCEModes);

extern void NFC_CE_disable(void);

// End of Card Emulation APIs

#endif //__NFC_CONTROLLER_H__
