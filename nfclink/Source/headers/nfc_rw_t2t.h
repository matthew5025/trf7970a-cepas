//*****************************************************************************
//
// nfc_rw_t2t.h - Implementation of T2T Reader/Writer
//
// Copyright (c) 2015 Texas Instruments Incorporated.  All rights reserved.
// TI Information - Selective Disclosure
//
//*****************************************************************************
#ifndef __NFC_RW_T2T_H__
#define __NFC_RW_T2T_H__

#include "trf79x0.h"

#define T2T_ACK_RSP				0x0A
#define T2T_NACK_RSP			0x05

//*****************************************************************************
//
//
//
//*****************************************************************************
typedef enum
{
    NFC_RW_T2T_CONNECTION_IDLE = 0x00,

	//! When writing data to a tag
    NFC_RW_T2T_CONNECTION_WRITE,

    NFC_RW_T2T_CONNECTION_WAIT_WRITE_RSP,

    NFC_RW_T2T_CONNECTION_WRITE_SUCCESS,

    NFC_RW_T2T_CONNECTION_WRITE_FAIL,

    //! When reading data from a tag
    NFC_RW_T2T_CONNECTION_READ,

    NFC_RW_T2T_CONNECTION_WAIT_READ_RSP,

    NFC_RW_T2T_CONNECTION_READ_SUCCESS,

    NFC_RW_T2T_CONNECTION_READ_FAIL,

    //! When Selecting another sector from the tag
    NFC_RW_T2T_CONNECTION_SECTOR_SELECT,

    NFC_RW_T2T_CONNECTION_WAIT_SECTOR_SELECT_RSP_PT1,

    NFC_RW_T2T_CONNECTION_SECTOR_SELECT_PT2,

    NFC_RW_T2T_CONNECTION_WAIT_SECTOR_SELECT_RSP,

    NFC_RW_T2T_CONNECTION_SECTOR_SELECT_SUCCESS,

    NFC_RW_T2T_CONNECTION_SECTOR_SELECT_FAIL,

    NFC_RW_T2T_CONNECTION_SEND_SLEEP,

    NFC_RW_T2T_PROTOCOL_ERROR

}tNfcRwT2TConnectionStatus;



//*****************************************************************************
//
// List of Commands
//
//*****************************************************************************

//*****************************************************************************
//
//
//
//*****************************************************************************
typedef enum
{
	T2T_READ_CMD	  =		0x30,
	T2T_WRITE_CMD	  =		0xA2,
	T2T_SECTOR_SEL_CMD = 	0xC2
}tT2T_Cmd;

//*****************************************************************************
//
// Function Prototypes
//
//*****************************************************************************
extern void NFC_RW_T2T_init(void);

extern void NFC_RW_T2T_stateMachine(void);

extern tNfcRwT2TConnectionStatus NFC_RW_T2T_processReceivedData(uint8_t * pui8RxBuffer,
																uint8_t ui8Length);

extern tNfcRwT2TConnectionStatus NFC_RW_T2T_getWriteStatus(void);

extern tNfcRwT2TConnectionStatus NFC_RW_T2T_getReadStatus(void);

extern tNfcRwT2TConnectionStatus NFC_RW_T2T_getSectorSelStatus(void);

extern uint8_t NFC_RW_T2T_sendReadCmd(uint8_t ui8BlockNumber);

extern uint8_t NFC_RW_T2T_sendWriteCmd(uint8_t ui8BlockNumber,
									   const uint8_t * pui8Buffer,
									   uint8_t ui8Length);

extern uint8_t NFC_RW_T2T_sendSectorSel(uint8_t ui8SectorNumber);

extern uint8_t NFC_RW_T2T_sendSleep(void);

extern void NFC_RW_T2T_getPacketStatus(uint8_t ** pui8RXData,
									   uint8_t * pui8Length);

#endif //__NFC_RW_T2T_H__
