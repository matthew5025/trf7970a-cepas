//*****************************************************************************
//
// llcp.h - Logic Link Control Protocol header file
//
// Copyright (c) 2015 Texas Instruments Incorporated.  All rights reserved.
// TI Information - Selective Disclosure
//
//*****************************************************************************
#ifndef __NFC_LLCP_H__
#define __NFC_LLCP_H__

#include "snep.h"
#include "nfc_config.h"
#include <stdint.h>

//*****************************************************************************
//
//! \addtogroup nfc_llcp_api NFC LLCP API Functions
//! @{
//
//*****************************************************************************

//*****************************************************************************
//
// List of Commands
//
//*****************************************************************************

//
// ! LLCP Magic Number is constant 0x46666D
//
#define LLCP_MAGIC_NUMBER_HIGH          0x46
#define LLCP_MAGIC_NUMBER_MIDDLE        0x66
#define LLCP_MAGIC_NUMBER_LOW           0x6D

//*****************************************************************************
//
// Service in local service Environment and is NOT advertised by local SDP
//
//*****************************************************************************

//
//! Source Service Access Point when sending
//
#define LLCP_SSAP_CONNECT_SEND              0x20

//
//! Source Service Access Point when receiving
//
#define LLCP_SSAP_CONNECT_RECEIVED          0x04

//
//! Destination Service Access Point for discovery
//
#define DSAP_SERVICE_DISCOVERY_PROTOCOL     0x01

//*****************************************************************************
//
// Maximum Information Units - The minimum SHALL be 128.  The total maximum buffer
// size will be MIU + 7 overhead bytes (NFC DEP Len + Command (2) + PFB + PDU Type (2) + Sequence)
//
//*****************************************************************************
#define LLCP_MIU                            248
#define LLCP_MIUX_SIZE                      (LLCP_MIU - 128)

//*****************************************************************************
//
//! LLCP Parameter Enumerations.
//
//*****************************************************************************
typedef enum
{
	//! See LLCP 4.5.1
    LLCP_VERSION =   0x01,
    //! See LLCP 4.5.2
    LLCP_MIUX =      0x02,
    //! See LLCP 4.5.3
    LLCP_WKS =       0x03,
    //! See LLCP 4.5.4
    LLCP_LTO =       0x04,
    //! See LLCP 4.5.5
    LLCP_RW =        0x05,
    //! See LLCP 4.5.6
    LLCP_SN =        0x06,
    //! See LLCP 4.5.7
    LLCP_OPT =       0x07,
    //! See LLCP 4.5.8
    LLCP_SDREQ =     0x08,
    //! See LLCP 4.5.9
    LLCP_SDRES =     0x09,
    LLCP_ERROR
}tLLCPParamaeter;

//*****************************************************************************
//
//! PDU Type Enumerations.
//
//*****************************************************************************
typedef enum
{
	//! See LLCP 4.3.1
    LLCP_SYMM_PDU =      0x00,
    //! See LLCP 4.3.2
    LLCP_PAX_PDU=        0x01,
    //! See LLCP 4.3.3
    LLCP_AGF_PDU=        0x02,
    //! See LLCP 4.3.4
    LLCP_UI_PDU =        0x03,
    //! See LLCP 4.3.5
    LLCP_CONNECT_PDU =   0x04,
    //! See LLCP 4.3.6
    LLCP_DISC_PDU =      0x05,
    //! See LLCP 4.3.7
    LLCP_CC_PDU =        0x06,
    //! See LLCP 4.3.8
    LLCP_DM_PDU =        0x07,
    //! See LLCP 4.3.9
    LLCP_FRMR_PDU =      0x08,
    //! See LLCP 4.3.10
    LLCP_SNL_PDU =       0x09,
    //! See LLCP 4.3.11
    LLCP_I_PDU   =       0x0C,
    //! See LLCP 4.3.12
    LLCP_RR_PDU  =       0x0D,
    //! See LLCP 4.3.13
    LLCP_RNR_PDU =       0x0E,
    //! See LLCP 4.3.14
    LLCP_RESERVED_PDU =  0x0F
}tLLCPPduPtype;

//*****************************************************************************
//
//! LLCP Connection Status Enumeration.
//
//*****************************************************************************
typedef enum
{
	//! No TX/RX ongoing.
    LLCP_CONNECTION_IDLE	= 0x00,

	//! When a virtual link is created either when we send a CONNECT PDU and
    //! receive a CC PDU, or when we receive a CONNECT PDU and respond a CC PDU.
    LLCP_CONNECTION_ESTABLISHED,

	//! When sending data via SNEP
    LLCP_CONNECTION_SENDING,

	//! When receiving data via SNEP
	LLCP_CONNECTION_RECEIVING

}tLLCPConnectionStatus;

//*****************************************************************************
//
//! Service Name Enumerations - Only support SNEP_SERVICE
//
//*****************************************************************************
typedef enum
{
    NPP_SERVICE = 0,
    SNEP_SERVICE,
    HANDOVER_SERVICE
}tServiceName;

//*****************************************************************************
//
//! Disconnected Mode Reasons Enumerations.
//
//*****************************************************************************
typedef enum
{
    //! See LLCP Section 4.3.8.
    DM_REASON_LLCP_RECEIVED_DISC_PDU =                               0x00,
    DM_REASON_LLCP_RECEIVED_CONNECTION_ORIENTED_PDU =                0x01,
    DM_REASON_LLCP_RECEIVED_CONNECT_PDU_NO_SERVICE =                 0x02,
    DM_REASON_LLCP_PROCESSED_CONNECT_PDU_REQ_REJECTED =              0x03,
    DM_REASON_LLCP_PERMNANTLY_NOT_ACCEPT_CONNECT_WITH_SAME_SSAP =    0x10,
    DM_REASON_LLCP_PERMNANTLY_NOT_ACCEPT_CONNECT_WITH_ANY_SSAP =     0x11,
    DM_REASON_LLCP_TEMMPORARILY_NOT_ACCEPT_PDU_WITH_SAME_SSSAPT =    0x20,
    DM_REASON_LLCP_TEMMPORARILY_NOT_ACCEPT_PDU_WITH_ANY_SSSAPT =     0x21
}tDisconnectModeReason;



//*****************************************************************************
//
// Function Prototypes
//
//*****************************************************************************
extern void LLCP_init(void);

extern uint16_t LLCP_getLinkTimeOut(void);

extern uint8_t LLCP_addTLV(tLLCPParamaeter eLLCPparam, uint8_t * pui8TLVBufferPtr);

extern void LLCP_processTLV(uint8_t * pui8TLVBufferPtr);

extern uint8_t LLCP_stateMachine(uint8_t * pui8PduBufferPtr);

extern uint8_t LLCP_processReceivedData(uint8_t * pui8RxBuffer, uint16_t ui16PduLength);

extern uint8_t LLCP_sendSYMM(uint8_t * pui8PduBufferPtr);

extern uint8_t LLCP_sendPAX(uint8_t * pui8PduBufferPtr);

extern uint8_t LLCP_sendCONNECT(uint8_t * pui8PduBufferPtr);

extern uint8_t LLCP_sendDISC(uint8_t * pui8PduBufferPtr);

extern uint8_t LLCP_sendCC(uint8_t * pui8PduBufferPtr);

extern uint8_t LLCP_sendDM(uint8_t * pui8PduBufferPtr,tDisconnectModeReason eDmReason);

extern uint8_t LLCP_sendI(uint8_t * pui8PduBufferPtr);

extern uint8_t LLCP_sendRR(uint8_t * pui8PduBufferPtr);
//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************

#endif // __NFC_LLCP_H__
