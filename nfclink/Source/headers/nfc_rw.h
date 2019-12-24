//*****************************************************************************
//
// nfc_rw.h - Reader/Writer Structure definition
//
// Copyright (c) 2015 Texas Instruments Incorporated.  All rights reserved.
// TI Information - Selective Disclosure
//
//*****************************************************************************
#ifndef __NFC_RW_H__
#define __NFC_RW_H__

// NFC Protocol Headers
#include "trf79x0.h"
#include "nfc_config.h"
#include "nfc_initiator.h"

typedef union
{
    struct {
        uint8_t bNfcA  	  : 1;    /*! NFC-A */
        uint8_t bNfcB	  : 1;    /*! NFC-B */
        uint8_t bNfcF     : 1;	  /*! NFC-F */
        uint8_t bISO15693 : 1;    /*! ISO15693 */
        uint8_t reserved  : 4;    /*! Reserved bits         */
    }bits;
    uint8_t     ui8byte;             /*! Whole status byte access */
}t_sNfcRWMode;

/*! NFCP2P Active/Passive Communication and Bitrate Register */
typedef union
{
    struct {
    	uint16_t bNfcA_106kbps  : 1;    /*! ISO14443-A 106kbps communication  */
    	uint16_t bNfcA_212kbps  : 1;    /*! ISO14443-A 212kbps communication  */
    	uint16_t bNfcA_424kbps  : 1;    /*! ISO14443-A 424kbps communication  */
    	uint16_t bNfcA_848kbps  : 1;    /*! ISO14443-A 848kbps communication  */
    	uint16_t bNfcB_106kbps  : 1;    /*! ISO14443-B 106kbps communication  */
    	uint16_t bNfcB_212kbps  : 1;    /*! ISO14443-B 212kbps communication  */
    	uint16_t bNfcB_424kbps  : 1;    /*! ISO14443-B 424kbps communication  */
    	uint16_t bNfcB_848kbps  : 1;    /*! ISO14443-B 848kbps communication  */
    	uint16_t bNfcF_212kbps  : 1;    /*! NFC-F 212kbps communication  */
    	uint16_t bNfcF_424kbps  : 1;    /*! NFC-F 424kbps communication  */
    	uint16_t bISO15693_26_48kbps : 1;    /*! 15693 high bitrate */
    	uint16_t reserved  : 5;    		/*! Reserved bits          */
    }bits;
    uint16_t     ui16byte;				/*! Whole status byte access */
}t_sNfcRWCommBitrate;

#endif
