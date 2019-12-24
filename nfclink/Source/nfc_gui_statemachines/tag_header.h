//*****************************************************************************
//
// tag_header.h - Reader/Writer Tag Definitions
//
// Copyright (c) 2015 Texas Instruments Incorporated.  All rights reserved.
// TI Information - Selective Disclosure
//
//*****************************************************************************

#ifndef __NFC_TAG_HEADER_H__
#define __NFC_TAG_HEADER_H__

#include "nfc_rw_t3t.h"
#include "nfc_rw_t5t.h"
#include "nfc_controller.h"

// TLV used for T2T, and T5T
#define TLV_NULL				0x00
#define TLV_LOCK_CONTROL		0x01
#define TLV_MEM_CONTROL			0x02
#define TLV_NDEF_MSG			0x03
#define TLV_PROP_INFO			0xFD
#define TLV_TERMINATOR			0xFE


typedef enum
{
	T2T_READ_CC = 0,
	T2T_READ_DATA,
	T2T_READ_NDEF,
	T2T_WRITE_NDEF,
	T2T_SELECTED_IDLE,
	T2T_SECTOR_SELECT
}tT2TStateMachine;

// T2T Fucntions
void T2T_init(uint8_t * pui8Ndef, uint16_t ui16NdefMaxSize);
void T2T_writeNDEF(uint8_t * pui8Data, uint16_t ui16NdefLen);
void T2T_stateMachine(void);

typedef enum
{
	T3T_SENSF_REQ,
	T3T_READ_CC,
	T3T_READ_DATA,
	T3T_READ_NDEF,
	T3T_WRITE_NDEF,
	T3T_SELECTED_IDLE
}tT3TStateMachine;

// T3T Functions
void T3T_init(uint8_t * pui8Ndef, uint16_t ui16NdefMaxSize);
void T3T_writeNDEF(uint8_t * pui8Data, uint16_t ui16NdefLen);
void T3T_stateMachine(void);

typedef enum
{
	T4T_APP_SEL = 0,
	T4T_SEL_CC,
	T4T_READ_CC,
	T4T_SEL_NDEF_FILE,
	T4T_READ_NDEF_LEN,
	T4T_READ_NDEF,
	T4T_SELECTED_IDLE,
	T4T_WRITE_NDEF,
}tNDEFStateMachine;

// T4T Functions
void T4T_init(uint8_t * pui8Ndef, uint16_t ui16NdefMaxSize);
void T4T_writeNDEF(uint8_t * pui8Data, uint16_t ui16NdefLen);
void T4T_stateMachine(void);

typedef enum
{
	T5T_INVENTORY = 0,
	T5T_INVENTORY_2,
	T5T_GET_SYS_INFO_EXT,
	T5T_GET_SYS_INFO_1,
	T5T_GET_SYS_INFO_2,
	T5T_READ_CC,
	T5T_READ_DATA,
	T5T_READ_NDEF,
	T5T_SELECTED_IDLE,
	T5T_WRITE_NDEF
}tT5TStateMachine;

// T5T Functions
void T5T_init(uint8_t * pui8Ndef, uint16_t ui16NdefMaxSize);
void T5T_writeNDEF(uint8_t * pui8Data, uint16_t ui16NdefLen);
void T5T_stateMachine(void);
void T5T_formatTag(void);

#endif
