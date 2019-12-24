//*****************************************************************************
//
// ISO_7816_4.h - Implementation of ISO 7816-4 layer.
//
// Copyright (c) 2015 Texas Instruments Incorporated.  All rights reserved.
// TI Information - Selective Disclosure
//
//*****************************************************************************
#ifndef __ISO_7816_4_H__
#define __ISO_7816_4_H__

#include "trf79x0.h"
#include "ce_t4t_config.h"

#define CLA_INDEX		0
#define INS_INDEX		1
#define P1_INDEX		2
#define P2_INDEX		3
#define LC_INDEX		4
#define DATA_INDEX		5

#define SW1_SW2_CMD_NOT_ALLOWED			0x6982
#define SW1_Sw2_FUNCTION_NOT_SUPPORTED	0x6A81
#define SW1_SW2_APP_NOT_FOUND			0x6A82
#define SW1_SW2_INCORRECT_P1_P2			0x6A86
#define SW1_SW2_APP_FOUND				0x9000

#define CE_READ_FLAG					0x01
#define CE_WRITE_FLAG					0x02

//*****************************************************************************
//
// List of Commands
//
//*****************************************************************************

typedef enum
{
    ISO_7816_4_CONNECTION_IDLE = 0x00,

    ISO_7816_4_CONNECTION_SELECT,

    ISO_7816_4_CONNECTION_WAIT_SELECT_RSP,

    ISO_7816_4_CONNECTION_SELECT_SUCCESS,

    ISO_7816_4_CONNECTION_SELECT_FAIL,

	//! When writing data to a tag
    ISO_7816_4_CONNECTION_WRITE,

    ISO_7816_4_CONNECTION_WAIT_WRITE_RSP,

    ISO_7816_4_CONNECTION_WRITE_SUCCESS,

    ISO_7816_4_CONNECTION_WRITE_FAIL,

    //! When reading data from a tag
    ISO_7816_4_CONNECTION_READ,

    ISO_7816_4_CONNECTION_WAIT_READ_RSP,

    ISO_7816_4_CONNECTION_READ_SUCCESS,

    ISO_7816_4_CONNECTION_READ_FAIL,

	//! When send RAW APDU to a tag
    ISO_7816_4_CONNECTION_RAW_WRITE,

    ISO_7816_4_CONNECTION_WAIT_RAW_WRITE_RSP,

    ISO_7816_4_CONNECTION_WRITE_RAW_SUCCESS,

    ISO_7816_4_CONNECTION_WRITE_RAW_FAIL,

    ISO_7816_4_TAG_MODE
}tISO7816ConnectionStatus;

typedef enum
{
	ISO_7816_4_SELECT_IDLE = 0x00,

	ISO_7816_4_SELECT_APP_ID,

	ISO_7816_4_SELECT_FILE_ID

}tISO7816ApplicationSelect;

//*****************************************************************************
//
// List of Commands
//
//*****************************************************************************
typedef enum
{
	INS_SELECT		  =		0xA4,
	INS_READ_BINARY	  =		0xB0,
	INS_UPDATE_BINARY = 	0xD6
}tISO_7816_4_INS;

//*****************************************************************************
//
// Function Prototypes
//
//*****************************************************************************
extern void ISO_7816_4_init(void);

extern void ISO_7816_4_setAppDS(tType4AppDS pType4DS);

extern uint8_t ISO_7816_4_stateMachine(uint8_t * pui8PduBufferPtr,
									   uint8_t ui8AvailableSpace);

extern uint8_t ISO_7816_4_processReceivedRequest(uint8_t * pui8RxPayload ,
												 uint8_t ui8CmdLength);

extern uint8_t ISO_7816_4_processReceivedData(uint8_t * pui8RxBuffer,
											  uint16_t ui16Length);

extern void ISO_7816_4_setCommMode(tISO7816ConnectionStatus eConnectionStatus);

extern tISO7816ConnectionStatus ISO_7816_4_getCommStatus(void);

extern tISO7816ConnectionStatus ISO_7816_4_getSelectStatus(void);

extern tISO7816ConnectionStatus ISO_7816_4_getWriteStatus(void);

extern tISO7816ConnectionStatus ISO_7816_4_getReadStatus(void);

extern tISO7816ConnectionStatus ISO_7816_4_getWriteRawStatus(void);

extern uint8_t ISO_7816_4_sendSelectApplication(const uint8_t * pui8Buffer,
												uint8_t ui8Length);

extern uint8_t ISO_7816_4_sendSelectFile(const uint8_t * pui8Buffer,
										 uint8_t ui8Length);

extern uint8_t ISO_7816_4_sendReadFile(uint16_t ui16StartIndex,
									   uint8_t ui8BytestoRead);

extern uint8_t ISO_7816_4_sendWriteFile(uint16_t ui16StartIndex,
										uint8_t ui8BytestoWrite,
										uint8_t * pui8Data);

extern uint8_t ISO_7816_4_sendAPDU(uint8_t * pui8Data,
								   uint8_t ui8Length);

extern tISO7816ConnectionStatus ISO_7816_4_getWriteAPDUStatus(void);

extern void ISO_7816_4_getPacketStatus(uint8_t ** pui8RXData,
									   uint16_t * pui16Length);

extern uint8_t ISO_7816_4_getCEStatus(uint16_t * pui16FileId,
									  uint16_t * pui16StartIdx,
									  uint8_t * pui8BufferLength);

#endif //__ISO_7816_4_H__
