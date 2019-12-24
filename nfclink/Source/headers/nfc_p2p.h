//*****************************************************************************
//
// nfc_p2p.h - Peer to Peer Structure definition
//
// Copyright (c) 2015 Texas Instruments Incorporated.  All rights reserved.
// TI Information - Selective Disclosure
//
//*****************************************************************************
#ifndef __NFC_P2P_H__
#define __NFC_P2P_H__

// NFC Protocol Headers
#include "trf79x0.h"
#include "snep.h"
#include "ndef_image.h"

typedef union
{
    struct {
        uint8_t bInitiatorEnabled  	: 1;    /*! Initiator Mode */
        uint8_t bTargetEnabled 		: 1;    /*! Target Mode */
        uint8_t reserved     		: 6;    /*! Reserved bits         */
    }bits;
    uint8_t     ui8byte;             /*! Whole status byte access */
}t_sNfcP2PMode;

/*! NFCP2P Active/Passive Communication and Bitrate Register */
typedef union
{
    struct {
    	uint8_t bPassive106kbps : 1;    /*! Passive 106kbps communication  */
    	uint8_t bPassive212kbps : 1;    /*! Passive 212kbps communication  */
    	uint8_t bPassive424kbps : 1;    /*! Passive 424kbps communication  */
    	uint8_t bActive106kbps 	: 1;    /*! Active 106kbps communication  */
    	uint8_t bActive212kbps 	: 1;    /*! Active 212kbps communication  */
    	uint8_t bActive424kbps 	: 1;    /*! Active 424kbps communication  */
    	uint8_t reserved : 2;    /*! Reserved bits          */
    }bits;
    uint8_t     ui8byte;             /*! Whole status byte access */
}t_sNfcP2PCommBitrate;

typedef struct{
	tPacketStatus sDataReceivedStatus;
	uint16_t ui16DataReceivedLength;
	uint32_t ui32PacketSize;
	uint8_t * pui8RxDataPtr;
}tNfcP2PRxStatus;

//*****************************************************************************
//
// Function Prototypes
//
//*****************************************************************************


#endif
