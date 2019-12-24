//*****************************************************************************
//
// nfc_rw_t5t.h - Implementation of T5T Reader/Writer
//
// Copyright (c) 2015 Texas Instruments Incorporated.  All rights reserved.
// TI Information - Selective Disclosure
//
//*****************************************************************************
#ifndef __NFC_RW_T5T_H__
#define __NFC_RW_T5T_H__

#include "trf79x0.h"

#define T5T_RESP_FLAG_NO_ERROR	0x00
#define T5T_RESP_FLAG_EXTENDED	0x08

#define T5T_REQ_FLAG_SUB_CARRIER	0x01
#define T5T_REQ_FLAG_HIGH_DATA		0x02
#define T5T_REQ_FLAG_INVENTORY		0x04
#define T5T_REQ_FLAG_EXTENDED		0x08
#define T5T_REQ_FLAG_SELECT			0x10
#define T5T_REQ_FLAG_ADDRESSED		0x20
#define T5T_REQ_FLAG_OPTION			0x40

#define T5T_INV_FLAG_AFI			0x10
#define T5T_INV_FLAG_SINGLE_SLOT	0x20
#define T5T_INV_FLAG_OPTION			0x40

#define T5T_GET_SYS_RESP_DSFID		0x01
#define T5T_GET_SYS_RESP_AFI		0x02
#define T5T_GET_SYS_RESP_MEM_SIZE	0x04


//*****************************************************************************
//
//
//
//*****************************************************************************
typedef enum
{
    NFC_RW_T5T_CONNECTION_IDLE = 0x00,

    //! When requesting an Inventory
    NFC_RW_T5T_CONNECTION_INVENTORY,

    NFC_RW_T5T_CONNECTION_INVENTORY_RSP,

    NFC_RW_T5T_CONNECTION_INVENTORY_SUCCESS,

    NFC_RW_T5T_CONNECTION_INVENTORY_FAIL,

    //! When sending Stay Quiet
    NFC_RW_T5T_CONNECTION_STAY_QUIET,

	//! When reading data from a tag
    NFC_RW_T5T_CONNECTION_READ_SINGLE_BLOCK,

    NFC_RW_T5T_CONNECTION_WAIT_READ_SINGLE_RSP,

    NFC_RW_T5T_CONNECTION_READ_SINGLE_SUCCESS,

    NFC_RW_T5T_CONNECTION_READ_SINGLE_FAIL,

    NFC_RW_T5T_CONNECTION_READ_MULT_BLOCK,

    NFC_RW_T5T_CONNECTION_WAIT_READ_MULT_RSP,

    NFC_RW_T5T_CONNECTION_READ_MULT_SUCCESS,

    NFC_RW_T5T_CONNECTION_READ_MULT_FAIL,

    //! When writing data to a tag
    NFC_RW_T5T_CONNECTION_WRITE_SINGLE_BLOCK,

    NFC_RW_T5T_CONNECTION_WAIT_WRITE_SINGLE_RSP,

    NFC_RW_T5T_CONNECTION_WRITE_SINGLE_SUCCESS,

    NFC_RW_T5T_CONNECTION_WRITE_SINGLE_FAIL,

    //! When locking a block on a tag
    NFC_RW_T5T_CONNECTION_LOCK_BLOCK,

    NFC_RW_T5T_CONNECTION_WAIT_LOCK_BLOCK_RSP,

    NFC_RW_T5T_CONNECTION_LOCK_BLOCK_SUCCESS,

    NFC_RW_T5T_CONNECTION_LOCK_BLOCK_FAIL,

    //! When issuing a select to the tag
    NFC_RW_T5T_CONNECTION_SELECT,

    NFC_RW_T5T_CONNECTION_WAIT_SELECT_RSP,

    NFC_RW_T5T_CONNECTION_SELECT_SUCCESS,

    NFC_RW_T5T_CONNECTION_SELECT_FAIL,

    //! When issuing a reset to ready to the tag
    NFC_RW_T5T_CONNECTION_RESET_TO_READY,

    NFC_RW_T5T_CONNECTION_WAIT_RESET_TO_READY_RSP,

    NFC_RW_T5T_CONNECTION_RESET_TO_READY_SUCCESS,

    NFC_RW_T5T_CONNECTION_RESET_TO_READY_FAIL,

    //! When writing an AFI to a tag
    NFC_RW_T5T_CONNECTION_WRITE_AFI_BLOCK,

    NFC_RW_T5T_CONNECTION_WAIT_WRITE_AFI_RSP,

    NFC_RW_T5T_CONNECTION_WRITE_AFI_SUCCESS,

    NFC_RW_T5T_CONNECTION_WRITE_AFI_FAIL,

    //! When locking the AFI of a tag
    NFC_RW_T5T_CONNECTION_LOCK_AFI_BLOCK,

    NFC_RW_T5T_CONNECTION_WAIT_LOCK_AFI_RSP,

    NFC_RW_T5T_CONNECTION_LOCK_AFI_SUCCESS,

    NFC_RW_T5T_CONNECTION_LOCK_AFI_FAIL,

    //! When requesting a Get System Info
    NFC_RW_T5T_CONNECTION_GET_SYS_INFO,

    NFC_RW_T5T_CONNECTION_GET_SYS_INFO_RSP,

    NFC_RW_T5T_CONNECTION_GET_SYS_INFO_SUCCESS,

    NFC_RW_T5T_CONNECTION_GET_SYS_INFO_FAIL,

    //! For Raw Packet Writes
    NFC_RW_T5T_CONNECTION_RAW_WRITE,

    NFC_RW_T5T_CONNECTION_RAW_WRITE_RSP,

    NFC_RW_T5T_CONNECTION_RAW_WRITE_SUCCESS,

    NFC_RW_T5T_CONNECTION_RAW_WRITE_FAIL,

    NFC_RW_T5T_PROTOCOL_ERROR

}tNfcRwT5TConnectionStatus;



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
	T5T_INVENTORY_CMD			  		=		0x01,
	T5T_STAY_QUIET_CMD			  		=		0x02,
	T5T_READ_SINGLE_BLOCK_CMD	  		=		0x20,
	T5T_WRITE_SINGLE_BLOCK_CMD	  		=		0x21,
	T5T_LOCK_BLOCK_CMD	  				=		0x22,
	T5T_READ_MULTIPLE_BLOCK_CMD	  		=		0x23,
	T5T_SELECT_CMD				  		=		0x25,
	T5T_RESET_TO_READY_CMD		  		=		0x26,
	T5T_WRITE_AFI_CMD	  				=		0x27,
	T5T_LOCK_AFI_CMD	  				=		0x28,
	T5T_GET_SYSTEM_INFO_CMD		  		=		0x2B,
	T5T_GET_SYSTEM_INFO_EXTENDED_CMD	=		0x3B
}tT5T_Cmd;


//*****************************************************************************
//
// Function Prototypes
//
//*****************************************************************************
extern void NFC_RW_T5T_init(void);

extern void NFC_RW_T5T_stateMachine(void);

extern tNfcRwT5TConnectionStatus NFC_RW_T5T_processReceivedData(uint8_t * pui8RxBuffer,
																uint8_t ui8Length);

extern tNfcRwT5TConnectionStatus NFC_RW_T5T_getInventoryStatus(void);

extern tNfcRwT5TConnectionStatus NFC_RW_T5T_getReadSingleStatus(void);

extern tNfcRwT5TConnectionStatus NFC_RW_T5T_getWriteSingleStatus(void);

extern tNfcRwT5TConnectionStatus NFC_RW_T5T_getLockBlockStatus(void);

extern tNfcRwT5TConnectionStatus NFC_RW_T5T_getSelectStatus(void);

extern tNfcRwT5TConnectionStatus NFC_RW_T5T_getResetToReadyStatus(void);

extern tNfcRwT5TConnectionStatus NFC_RW_T5T_getReadMultipleStatus(void);

extern tNfcRwT5TConnectionStatus NFC_RW_T5T_getWriteAFIStatus(void);

extern tNfcRwT5TConnectionStatus NFC_RW_T5T_getLockAFIStatus(void);

extern tNfcRwT5TConnectionStatus NFC_RW_T5T_getGetSysInfoStatus(void);

extern tNfcRwT5TConnectionStatus NFC_RW_T5T_getRawWriteStatus(void);

extern uint8_t NFC_RW_T5T_sendInventoryCmd(uint8_t ui8ReqFlag,
										   uint8_t ui8AFI,
										   bool bSendAFI);

extern uint8_t NFC_RW_T5T_sendStayQuietCmd(uint8_t ui8ReqFlag);

extern uint8_t NFC_RW_T5T_sendReadSingleCmd(uint8_t ui8ReqFlag,
											uint16_t ui16BlockNumber);

extern uint8_t NFC_RW_T5T_sendWriteSingleCmd(uint8_t ui8ReqFlag,
											 uint16_t ui16BlockNumber,
											 const uint8_t * pui8Buffer,
											 uint8_t ui8Length);

extern uint8_t NFC_RW_T5T_sendLockBlockCmd(uint8_t ui8ReqFlag,
										   uint16_t ui16BlockNumber);

extern uint8_t NFC_RW_T5T_sendSelect(uint8_t ui8ReqFlag);

extern uint8_t NFC_RW_T5T_sendResetToReady(uint8_t ui8ReqFlag);

extern uint8_t NFC_RW_T5T_sendReadMultipleCmd(uint8_t ui8ReqFlag,
											  uint16_t ui16BlockNumber,
											  uint8_t ui8BlockCount);

extern uint8_t NFC_RW_T5T_sendWriteAFICmd(uint8_t ui8ReqFlag,
										  uint8_t ui16NewAFI);

extern uint8_t NFC_RW_T5T_sendLockAFICmd(uint8_t ui8ReqFlag);

extern uint8_t NFC_RW_T5T_sendGetSysInfoCmd(uint8_t ui8ReqFlag,
											uint8_t ui8CommandCode);

extern uint8_t NFC_RW_T5T_sendRawWriteCmd(uint8_t ui8ReqFlag,
										  uint8_t ui8CommandCode,
										  const uint8_t * pui8Buffer,
										  uint8_t ui8Length,
										  bool bOptionDelay);

extern void NFC_RW_T5T_getPacketStatus(uint8_t ** pui8RXData,
									   uint8_t * pui8Length);

extern uint8_t NFC_RW_T5T_getT5TErrorCode(void);

extern uint8_t NFC_RW_T5T_getT5TDSFID(void);

extern uint8_t NFC_RW_T5T_getT5TAFI(void);

extern void NFC_RW_T5T_getT5TUID(uint8_t ** pui8NfcVId, uint8_t * ui8NfcVIdLength);

extern uint16_t NFC_RW_T5T_getVICCBlockCount(void);

extern uint8_t NFC_RW_T5T_getVICCBlockSize(void);

#endif //__NFC_RW_T5T_H__

