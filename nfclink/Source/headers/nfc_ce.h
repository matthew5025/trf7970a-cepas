//*****************************************************************************
//
// nfc_ce.h - Card Emulation Structure definition
//
// Copyright (c) 2015 Texas Instruments Incorporated.  All rights reserved.
// TI Information - Selective Disclosure
//
//*****************************************************************************
#ifndef __NFC_CE_H__
#define __NFC_CE_H__

// NFC Protocol Headers
#include "trf79x0.h"
#include "ce_t4t_config.h"

typedef union
{
    struct {
        uint8_t bT4TAEnabled  	: 1;    /*! Type 4 Tag A */
        uint8_t bT4TBEnabled	: 1;    /*! Type 4 Tag B */
        uint8_t reserved     	: 5;    /*! Reserved bits         */
    }bits;
    uint8_t     ui8byte;             /*! Whole status byte access */
}t_sNfcCEMode;

#endif
