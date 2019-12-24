//*****************************************************************************
//
// nfc_rw_t3t.h - Implementation of T3T Reader/Writer
//
// Copyright (c) 2015 Texas Instruments Incorporated.  All rights reserved.
// TI Information - Selective Disclosure
//
//*****************************************************************************
#ifndef __NFC_RW_T3T_H__
#define __NFC_RW_T3T_H__

#include "trf79x0.h"
#include "nfc_f.h"

//*****************************************************************************
//
//
//
//*****************************************************************************
typedef enum
{
    NFC_RW_T3T_CONNECTION_IDLE = 0x00,

	//! When writing data to a tag
    NFC_RW_T3T_CONNECTION_UPDATE,

    NFC_RW_T3T_CONNECTION_WAIT_UPDATE_RSP,

    NFC_RW_T3T_CONNECTION_UPDATE_SUCCESS,

    NFC_RW_T3T_CONNECTION_UPDATE_FAIL,

    //! When reading data from a tag
    NFC_RW_T3T_CONNECTION_CHECK,

    NFC_RW_T3T_CONNECTION_WAIT_CHECK_RSP,

    NFC_RW_T3T_CONNECTION_CHECK_SUCCESS,

    NFC_RW_T3T_CONNECTION_CHECK_FAIL,

    //! Send SENSF_REQ
    NFC_RW_T3T_CONNECTION_SENSF_REQ,

    NFC_RW_T3T_CONNECTION_WAIT_SENSF_REQ_RSP,

    NFC_RW_T3T_CONNECTION_SENSF_REQ_SUCCESS,

    NFC_RW_T3T_CONNECTION_SENSF_REQ_FAIL,

//    NFC_RW_T3T_CONNECTION_SEND_SLEEP,

    NFC_RW_T3T_PROTOCOL_ERROR

}tNfcRwT3TConnectionStatus;



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
	T3T_CHECK_CMD	  =		0x06,
	T3T_CHECK_RES	  =		0x07,
	T3T_UPDATE_CMD	  =		0x08,
	T3T_UPDATE_RES	  =		0x09
}tT3T_Cmd;

typedef struct
{
	uint8_t * pui8Services;
	uint8_t ui8ServiceCount;
	uint8_t * pui8Blocks;
	uint8_t ui8BlockLen;
	uint8_t * pui8Data;
	uint8_t ui8DataLen;
	bool bThreeByteBlock;
}tT3TPacketData;

//*****************************************************************************
//
// Function Prototypes
//
//*****************************************************************************
extern void NFC_RW_T3T_init(void);

extern void NFC_RW_T3T_stateMachine(void);

extern tNfcRwT3TConnectionStatus NFC_RW_T3T_processReceivedData(uint8_t * pui8RxBuffer,
																uint8_t ui8Length);

extern tNfcRwT3TConnectionStatus NFC_RW_T3T_getUpdateStatus(void);

extern tNfcRwT3TConnectionStatus NFC_RW_T3T_getCheckStatus(void);

extern tNfcRwT3TConnectionStatus NFC_RW_T3T_getSensFStatus(void);

extern uint8_t NFC_RW_T3T_sendUpdateCmd(tT3TPacketData sT3TData);

extern uint8_t NFC_RW_T3T_sendCheckCmd(tT3TPacketData sT3TData);

extern uint8_t NFC_RW_T3T_sendSensFReq(uint16_t ui16SC,
									   uint8_t ui8RC,
									   uint8_t ui8Slot);

extern void NFC_RW_T3T_getPacketStatus(uint8_t ** pui8RXData,
									   uint8_t * pui8Length);

#endif //__NFC_RW_T3T_H__
