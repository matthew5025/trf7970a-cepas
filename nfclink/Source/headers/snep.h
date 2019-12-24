//*****************************************************************************
//
// snep.h - Simple NDEF Exchange Protocol definitions
//
// Copyright (c) 2015 Texas Instruments Incorporated.  All rights reserved.
// TI Information - Selective Disclosure
//
//*****************************************************************************
#ifndef __NFC_SNEP_H__
#define __NFC_SNEP_H__

#include "nfc_config.h"
#include "types.h"
#include <stdint.h>

//*****************************************************************************
//
// This size is used to limit the maximum size of the incoming/outgoing fragment.
// The maximum is dependent on the Maximum Information Units (MIU) defined in
// the LLCP layer.
// For example for MIU = 248, SNEP_MAX_BUFFER = 248
//
//*****************************************************************************
#define SNEP_MAX_BUFFER             248

//*****************************************************************************
//
// This size is used to limit the maximum size of the incoming NDEF message.
//
//*****************************************************************************
#define SNEP_MAX_PAYLOAD			500000

//*****************************************************************************
//
// SNEP Version Number
//
//*****************************************************************************
#define SNEP_VERSION                0x10

//*****************************************************************************
//
// List of SNEP Commands
//
//*****************************************************************************
typedef enum
{
    //
    // SNEP Request Field Value
    //
    SNEP_REQUEST_CONTINUE =          0x00,
    SNEP_REQUEST_GET =               0x01,
    SNEP_REQUEST_PUT =               0x02,
    // 03h-7Eh Reserved for future use
    SNEP_REQUEST_REJECT =            0x7F,
    // 80h-FFh Reserved for response field values

    //
    // SNEP Response Field Values
    //
    // 00h-7Fh Reserved for request field values
    SNEP_RESPONSE_CONTINUE =         0x80,
    SNEP_RESPONSE_SUCCESS =          0x81,
    SNEP_RESPONSE_NOT_FOUND =        0xC0,
    SNEP_RESPONSE_EXCESS_DATA =      0xC1,
    SNEP_RESPONSE_BAD_REQUEST =      0xC2,
    SNEP_RESPONSE_NOT_IMPLEMENTED =  0xE0,
    SNEP_RESPONSE_UNSUPPORTED_VER =  0xE1,
    SNEP_RESPONSE_REJECT =           0xFF
}tSNEPCommands;

typedef enum
{
    SNEP_CONNECTION_IDLE                    = 0x00,
    SNEP_WRONG_VERSION_RECEIVED,
    SNEP_CONNECTION_RECEIVED_FIRST_PACKET,
    SNEP_CONNECTION_RECEIVING_N_FRAGMENTS,
    SNEP_CONNECTION_DATA_AVAILABLE,
    SNEP_CONNECTION_WAITING_FOR_CONTINUE,
    SNEP_CONNECTION_WAITING_FOR_SUCCESS,
    SNEP_CONNECTION_SENDING_N_FRAGMENTS,
    SNEP_CONNECTION_SEND_COMPLETE,
    SNEP_CONNECTION_RECEIVE_COMPLETE,
    SNEP_CONNECTION_EXCESS_SIZE
}tSNEPConnectionStatus;

typedef enum
{
    RECEIVED_NO_FRAGMENT = 0,
    RECEIVED_FIRST_FRAGMENT,
    RECEIVED_N_FRAGMENT,
    RECEIVED_FRAGMENT_COMPLETED
}tPacketStatus;

//*****************************************************************************
//
// Function Prototypes
//
//*****************************************************************************
extern void SNEP_init(void);

extern void SNEP_setMaxPayload(uint8_t ui8MaxPayload);

extern uint8_t SNEP_stateMachine(uint8_t * pui8BufferPtr);

extern uint8_t SNEP_setupPacket(uint8_t * pui8PacketPtr,
								uint32_t ui32PacketLength,
								uint8_t ui8FragmentLength);

extern uint8_t SNEP_getTxBufferStatus(void);

extern uint8_t SNEP_enqueue(uint8_t * pui8PacketPtr,
							uint8_t ui8Length);

extern uint8_t SNEP_sendRequest(uint8_t * pui8DataPtr,
								tSNEPCommands eRequestCmd);

extern uint8_t SNEP_sendResponse(uint8_t * pui8DataPtr,
								 tSNEPCommands eResponseCmd);

extern void SNEP_processReceivedData(uint8_t * pui8RxBuffer,
									 uint16_t ui8RxLength);

extern void SNEP_getReceiveStatus(tPacketStatus * peReceiveFlag,
								  uint16_t * pui16length,
								  uint8_t ** pui8DataPtr,
								  uint32_t * ui32NdefTotalLength);

extern tSNEPConnectionStatus SNEP_getProtocolStatus(void);

extern void SNEP_setProtocolStatus(tSNEPConnectionStatus eProtocolStatus);

#endif // __NFC_SNEP_H__
