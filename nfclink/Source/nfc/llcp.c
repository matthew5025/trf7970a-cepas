//*****************************************************************************
//
// llcp.c - Logic Link Control Protocol : used to establish / mantain a link to 
// send packets via NPP or SNEP
//          NPP:  NDEF Push Protocol
//          SNEP: Simple NDEF Exchange protocol
// NOTE: currently only SNEP is Supported
//
// Copyright (c) 2015 Texas Instruments Incorporated.  All rights reserved.
// TI Information - Selective Disclosure
//
//*****************************************************************************

#include "llcp.h"

//*****************************************************************************
//
//! \addtogroup nfc_llcp_api NFC LLCP API Functions
//! @{
//! Logical Link Control Protocol is the NFC transport layer used to open /  
//! close virtual link used to transfer NDEFs between two devices in Peer to
//! Peer mode via the Simple NDEF Exchange Protocol. For more information
//! on LLCP please read the Logical Link Control Protocol Specification 
//! Version 1.1.
//
//*****************************************************************************

//*****************************************************************************
//
// The next PDU to be sent to the destination device - updated based on
//  incoming packets.
//
//*****************************************************************************
tLLCPPduPtype g_eNextPduQueue;

//*****************************************************************************
//
// Connection Status
//
//*****************************************************************************
tLLCPConnectionStatus g_eLLCPConnectionStatus;

//*****************************************************************************
//
// Destination Service Access Point Address
//
//*****************************************************************************
uint8_t g_ui8dsapValue;

//*****************************************************************************
//
// Source Service Access Point Address
//
//*****************************************************************************
uint8_t g_ui8ssapValue;

//*****************************************************************************
//
// Current Service Name - SNEP by default
//
//*****************************************************************************
tServiceName g_eCurrentServiceEnabled ;

//*****************************************************************************
//
// Acknowledged packets
//
//*****************************************************************************
uint8_t g_ui8NSNR;

//*****************************************************************************
//
// Disconnected Mode Reason
//
//*****************************************************************************
tDisconnectModeReason g_eDMReason;

//*****************************************************************************
//
// LLCP Link Time Out
//
//*****************************************************************************
uint16_t g_ui16LLCPlto;

//*****************************************************************************
//
// LLCP MIUX of the initiator / target communicating with the TRF7970A.
// 248 by default.
//
//*****************************************************************************
uint8_t g_ui8LLCPmiu;


//*****************************************************************************
//
// This is offset is meant for NFC-A
//
//*****************************************************************************
uint8_t g_ui8LLCPmiuOffset;

//*****************************************************************************
//
//! LLCP_init - Initializes the Logical Link Control Protocol layer.
//!
//! This function must be called prior to any other function offer by the LLCP
//! driver. This function initializes the acknowledge packets, Maximum
//! information units, current service enabled, the next PDU for the
//! LLCP_stateMachine(), and initializes the SNEP layer with SNEP_init().
//!
//! \return None
//!
//
//*****************************************************************************
void LLCP_init(void)
{
    // Reset NS and NR
    g_ui8NSNR = 0x00;
    g_ui8LLCPmiu = 128;
    g_ui8LLCPmiuOffset = 0;
    g_ui16LLCPlto = 0x00;
    g_eLLCPConnectionStatus = LLCP_CONNECTION_IDLE;
    g_eCurrentServiceEnabled = SNEP_SERVICE;
    g_eNextPduQueue = LLCP_SYMM_PDU;
    SNEP_init();
    SNEP_setMaxPayload(g_ui8LLCPmiu);
}

//*****************************************************************************
//
//! LLCP_getLinkTimeOut - Gets link timout
//!
//! This function returns the Link Timeout which may be modified if the  
//! LLCP_processTLV() function processes a LLCP_LTO TLV.
//!
//! \return g_ui16LLCPlto the link timeout.
//
//*****************************************************************************
uint16_t LLCP_getLinkTimeOut(void)
{
    return g_ui16LLCPlto;
}

//*****************************************************************************
//
//! LLCP_addTLV - Adds a  LLCP parameter to the LLCP PDU with the Type Length
//! Value (TLV) format.
//!
//! \param eLLCPparam is the LLCP type that will be added.
//! \param pui8TLVBufferPtr is the pointer where the TLV will be written at.
//!
//! The \e eLLCPparam parameter can be any of the following:
//!
//! - \b LLCP_VERSION    - Version Number
//! - \b LLCP_MIUX       - Maximum Information Unit Extension
//! - \b LLCP_WKS        - Well-Known Service List
//! - \b LLCP_LTO        - Link Timeout
//! - \b LLCP_RW         - Receive Window Size
//! - \b LLCP_SN         - Service Name
//! - \b LLCP_OPT        - Option
//! - \b LLCP_SDREQ      - Service Discovery Request
//! - \b LLCP_SDRES      - Service Discovery Response
//! - \b LLCP_ERROR      - Reserved (used ro return length of 0)
//!
//! This function is used to add a LLCP Parameter to the LLCP PDU to include 
//! more information about the LLCP layer. This function must be called inside 
//! LLCP_sendCONNECT(), LLCP_sendCC(), NFCDEP_sendATR_REQ() and
//! NFCDEP_sendATR_RES().
//!
//! \return ui8PacketLength Length of the LLCP Parameter added to the LLCP.
//
//*****************************************************************************
uint8_t LLCP_addTLV(tLLCPParamaeter eLLCPparam, uint8_t * pui8TLVBufferPtr)
{
    uint8_t ui8PacketLength = 0;
    uint16_t ui16MixuSize;

    switch(eLLCPparam)
    {
    case LLCP_VERSION:
        // Type
        pui8TLVBufferPtr[0] = (uint8_t) LLCP_VERSION;
        // Length
        pui8TLVBufferPtr[1] = 0x01;
        // Value
        pui8TLVBufferPtr[2] = 0x11;     // Version 1.1
        break;
    case LLCP_MIUX:
        // Type
        pui8TLVBufferPtr[0] = (uint8_t) LLCP_MIUX;
        // Length
        pui8TLVBufferPtr[1] = 0x02;
        ui16MixuSize = LLCP_MIUX_SIZE - g_ui8LLCPmiuOffset;
        // Value
        // 128 + MIUX (120) = MIU (248)
        pui8TLVBufferPtr[2] = (ui16MixuSize >> 8) & 0xFF;     // MIUX 15:8
        pui8TLVBufferPtr[3] = (uint8_t) ui16MixuSize;        // MIUX 7:0
        break;
    case LLCP_WKS:
        // Type
        pui8TLVBufferPtr[0] = (uint8_t) LLCP_WKS;
        // Length
        pui8TLVBufferPtr[1] = 0x02;
        // Value
        pui8TLVBufferPtr[2] = 0x00;     
		// Look up 0x0003 (WKS LLC Link Management)
        pui8TLVBufferPtr[3] = 0x03;
        break;
    case LLCP_LTO:
        // Type
        pui8TLVBufferPtr[0] = (uint8_t) LLCP_LTO;
        // Length
        pui8TLVBufferPtr[1] = 0x01;
        // Value
        pui8TLVBufferPtr[2] = 0x64;     // (100 (0x64) * 10 mS = 1000 mS timeout, Figure 22, LLP)
        break;
    case LLCP_RW:
        // Type
        pui8TLVBufferPtr[0] = (uint8_t) LLCP_RW;
        // Length
        pui8TLVBufferPtr[1] = 0x01;
        // Value
        // Section 5.6.2.2 LLCP
        // A receive window size of zero indicates that the local LLC will not
        // accept I PDUs on that data link connection. A receive window size of
        // one indicates that the local LLC will acknowledge every I PDU before
        // accepting additional I PDUs.
        //
        pui8TLVBufferPtr[2] = 0x04;
        break;
    case LLCP_SN:
        // Type
        pui8TLVBufferPtr[0] = (uint8_t) LLCP_SN;
        if(g_eCurrentServiceEnabled == NPP_SERVICE)
        {
            // Length
            pui8TLVBufferPtr[1] = 0x0F;
            // Value
            pui8TLVBufferPtr[2] = 'c';
            pui8TLVBufferPtr[3] = 'o';
            pui8TLVBufferPtr[4] = 'm';
            pui8TLVBufferPtr[5] = '.';
            pui8TLVBufferPtr[6] = 'a';
            pui8TLVBufferPtr[7] = 'n';
            pui8TLVBufferPtr[8] = 'd';
            pui8TLVBufferPtr[9] = 'r';
            pui8TLVBufferPtr[10] = 'o';
            pui8TLVBufferPtr[11] = 'i';
            pui8TLVBufferPtr[12] = 'd';
            pui8TLVBufferPtr[13] = '.';
            pui8TLVBufferPtr[14] = 'n';
            pui8TLVBufferPtr[15] = 'p';
            pui8TLVBufferPtr[16] = 'p';
        }
        else if(g_eCurrentServiceEnabled == SNEP_SERVICE)
        {
            // Length
            pui8TLVBufferPtr[1] = 0x0F;
            // Value
            pui8TLVBufferPtr[2] = 'u';
            pui8TLVBufferPtr[3] = 'r';
            pui8TLVBufferPtr[4] = 'n';
            pui8TLVBufferPtr[5] = ':';
            pui8TLVBufferPtr[6] = 'n';
            pui8TLVBufferPtr[7] = 'f';
            pui8TLVBufferPtr[8] = 'c';
            pui8TLVBufferPtr[9] = ':';
            pui8TLVBufferPtr[10] = 's';
            pui8TLVBufferPtr[11] = 'n';
            pui8TLVBufferPtr[12] = ':';
            pui8TLVBufferPtr[13] = 's';
            pui8TLVBufferPtr[14] = 'n';
            pui8TLVBufferPtr[15] = 'e';
            pui8TLVBufferPtr[16] = 'p';

        }
        break;
    case LLCP_OPT:
        // Type
        pui8TLVBufferPtr[0] = (uint8_t) LLCP_OPT;
        // Length
        pui8TLVBufferPtr[1] = 0x01;
        // Value
        pui8TLVBufferPtr[2] = 0x03;     // (Class 3) (Table 7, LLP)
        break;
    case LLCP_SDREQ:
        break;
    case LLCP_SDRES:
        break;
    default:
        pui8TLVBufferPtr[0] = LLCP_ERROR;
        break;
    }

    if(pui8TLVBufferPtr[0] == LLCP_ERROR)
        ui8PacketLength = 0x00;
    else
        ui8PacketLength = pui8TLVBufferPtr[1] + 2;

    return ui8PacketLength;
}

//*****************************************************************************
//
//! LLCP_processTLV - Processes the LLCP Parameter TLV.
//!
//! \param pui8TLVBufferPtr is the pointer to the Type value of the TLV. 
//!
//! This function processes the LLCP Parameters included in the ATR_RES. This
//! function must be called inside the NFCDEP_processReceivedData(), to 
//! initialize the g_ui8LLCPmiu and g_ui16LLCPlto if they are included as part 
//! of ATR_RES.
//!
//! \return None
//
//*****************************************************************************
void LLCP_processTLV(uint8_t * pui8TLVBufferPtr)
{
	uint16_t ui16Miu;
    switch(pui8TLVBufferPtr[0])
    {
    case LLCP_VERSION:
        break;
    case LLCP_MIUX:
    	// MIU = 128 + MIUX
    	ui16Miu = (pui8TLVBufferPtr[2] << 8)+pui8TLVBufferPtr[3]+128;
    	// Check if the received MIU is less than LLCP_MIU, the modify the current MIU to it
    	if(ui16Miu < LLCP_MIU)
    	{
    		// Modify MIU to be less
    		g_ui8LLCPmiu = (uint8_t) ui16Miu;

    	}
    	else
    	{
    		// Maximum supported MIU is LLCP_MIU
    		g_ui8LLCPmiu = LLCP_MIU;
    	}

    	SNEP_setMaxPayload(g_ui8LLCPmiu-g_ui8LLCPmiuOffset);
        break;
    case LLCP_WKS:
        break;
    case LLCP_LTO:
        g_ui16LLCPlto = pui8TLVBufferPtr[2] * 10;
        break;
    case LLCP_RW:
        break;
    case LLCP_SN:
        break;
    case LLCP_OPT:
        break;
    case LLCP_SDREQ:
        break;
    case LLCP_SDRES:
        break;
    default:
        break;
    }
}

//*****************************************************************************
//
//! LLCP_stateMachine - Prepares the LLCP packet to be transmitted.
//!
//! \param pui8PduBufferPtr is the start pointer to add the LLCP PDU.
//!
//! This function is used to add the LLCP portion of the DEP_REQ / DEP_RES PDU.
//! This function must be called inside NFCDEP_sendDEP_REQ() and
//! NFCDEP_sendDEP_RES(). It currently does not support to send  the following
//! PDUs : LLCP_PAX_PDU, LLCP_AGF_PDU, LLCP_UI_PDU, LLCP_FRMR_PDU, LLCP_SNL_PDU,
//! LLCP_RNR_PDU, and LLCP_RESERVED_PDU.
//!
//! \return ui8PacketLength is the length of the LLCP PDU added to the
//! pui8PduBufferPtr.
//
//*****************************************************************************
uint8_t LLCP_stateMachine(uint8_t * pui8PduBufferPtr)
{
    uint8_t ui8PacketLength=0;

    switch(g_eNextPduQueue)
    {
        case LLCP_SYMM_PDU:
        {
#if NFC_LLCP_DEBUG
			Serial_printf("\nLLCP TX: SYMM\n",LLCP_LAYER_DATA);
#endif
            ui8PacketLength = LLCP_sendSYMM(pui8PduBufferPtr);
            break;
        }
        case LLCP_PAX_PDU:
        {
        	ui8PacketLength = LLCP_sendPAX(pui8PduBufferPtr);
            break;
        }
        case LLCP_AGF_PDU:
        {
        	// Not Supported
            break;
        }
        case LLCP_UI_PDU:
        {
        	// Not Supported
            break;
        }
        case LLCP_CONNECT_PDU:
        {
#if NFC_LLCP_DEBUG
			Serial_printf("\nLLCP TX: CONNECT\n",LLCP_LAYER_DATA);
#endif

			ui8PacketLength = LLCP_sendCONNECT(pui8PduBufferPtr);

            g_eNextPduQueue = LLCP_SYMM_PDU;
            break;
        }
        case LLCP_DISC_PDU:
        {
            if(g_eCurrentServiceEnabled == HANDOVER_SERVICE)
            {
                g_eCurrentServiceEnabled = SNEP_SERVICE;
            }
#if NFC_LLCP_DEBUG
			Serial_printf("\nLLCP TX: DISC\n",LLCP_LAYER_DATA);
#endif
            ui8PacketLength = LLCP_sendDISC(pui8PduBufferPtr);
            break;
        }
        case LLCP_CC_PDU:
        {
#if NFC_LLCP_DEBUG
			Serial_printf("\nLLCP TX: CC\n",LLCP_LAYER_DATA);
#endif
            ui8PacketLength = LLCP_sendCC(pui8PduBufferPtr);
            break;
        }
        case LLCP_DM_PDU:
        {
#if NFC_LLCP_DEBUG
			Serial_printf("\nLLCP TX: DM\n",LLCP_LAYER_DATA);
#endif
            g_eLLCPConnectionStatus = LLCP_CONNECTION_IDLE;
            ui8PacketLength = LLCP_sendDM(pui8PduBufferPtr,g_eDMReason);
            g_eNextPduQueue = LLCP_SYMM_PDU;
            break;
        }
        case LLCP_FRMR_PDU:
        {
        	// Not Supported
            break;
        }
        case LLCP_SNL_PDU:
        {
        	// Not Supported
            break;
        }
        case LLCP_I_PDU:
        {
#if NFC_LLCP_DEBUG
			Serial_printf("\nLLCP TX: I\n",LLCP_LAYER_DATA);
#endif
            ui8PacketLength = LLCP_sendI(pui8PduBufferPtr);
            break;
        }
        case LLCP_RR_PDU:
        {
#if NFC_LLCP_DEBUG
			Serial_printf("\nLLCP TX: RR\n",LLCP_LAYER_DATA);
#endif
            if(g_eCurrentServiceEnabled == HANDOVER_SERVICE)
            {
                g_eLLCPConnectionStatus = LLCP_CONNECTION_IDLE;
            }
            ui8PacketLength = LLCP_sendRR(pui8PduBufferPtr);
            break;
        }
        case LLCP_RNR_PDU:
        {
        	// Not Supported
            break;
        }
        case LLCP_RESERVED_PDU:
        {
        	// Not Supported
            break;
        }
        default:
        {
            break;
        }
    }

    return ui8PacketLength;
}

//*****************************************************************************
//
//! LLCP_processReceivedData - Processes LLCP Data Received.
//!
//! \param pui8RxBuffer is the start pointer of the LLCP data received.
//! \param ui16PduLength is the length of the LLCP PDU received.
//!
//! This function is used to handle the LLCP portion of the DEP_REQ / DEP_RES PDU.
//! This function must be called inside NFCDEP_processReceivedRequest() and
//! NFCDEP_processReceivedData().It currently does not support to handle the
//! following PDUs : LLCP_PAX_PDU, LLCP_AGF_PDU, LLCP_UI_PDU, LLCP_FRMR_PDU,
//! LLCP_SNL_PDU, LLCP_RNR_PDU, and LLCP_RESERVED_PDU.
//!
//! \return eLLCPStatus is the boolean status if the command was processed (1)
//! or not (0).
//
//*****************************************************************************
uint8_t LLCP_processReceivedData(uint8_t * pui8RxBuffer, uint16_t ui16PduLength)
{
	uint8_t ui8Counter;
    tLLCPPduPtype ePduType;
    uint8_t ui8LLCPStatus = STATUS_SUCCESS;
    tSNEPConnectionStatus eSnepProtocolStatus;

#if NFC_LLCP_DEBUG
    uint8_t pui8StringBuffer[5];
#endif

    ePduType = (tLLCPPduPtype) ( ((pui8RxBuffer[0] & 0x03) << 2) +
                                    ((pui8RxBuffer[1] & 0xC0) >> 6));





#if NFC_LLCP_DEBUG
    convertWordToAscii(ui16PduLength,(uint8_t *)pui8StringBuffer);

	Serial_printf("LLCP RX",LLCP_LAYER_DATA);

	Serial_printBuffer((char *) pui8StringBuffer, 5,LLCP_LAYER_DATA);

	Serial_printf(" bytes\n",LLCP_LAYER_DATA);
#endif

	switch(ePduType)
	{
	case LLCP_SYMM_PDU:
        if(g_eCurrentServiceEnabled == SNEP_SERVICE)
        {
            eSnepProtocolStatus = SNEP_getProtocolStatus();
            if((g_eNextPduQueue == LLCP_CONNECT_PDU) ||
                (g_eNextPduQueue == LLCP_I_PDU))
            {
                //
                // Do no modify the next PDU
                //
#if NFC_LLCP_DEBUG
            	Serial_printf("LLCP RX: SYMM\n",LLCP_LAYER_DATA);
#endif
            }
            else if(eSnepProtocolStatus == SNEP_CONNECTION_SEND_COMPLETE)
            {
                SNEP_setProtocolStatus(SNEP_CONNECTION_IDLE);
#if NFC_LLCP_DEBUG
                Serial_printf("LLCP RX: SYMM\n",LLCP_LAYER_DATA);
#endif
                g_eNextPduQueue = LLCP_DISC_PDU;
            }
            else if((eSnepProtocolStatus ==
                                    SNEP_CONNECTION_RECEIVED_FIRST_PACKET) ||
                    (eSnepProtocolStatus ==
                                    SNEP_CONNECTION_RECEIVE_COMPLETE) ||
					(eSnepProtocolStatus ==
									SNEP_CONNECTION_EXCESS_SIZE) ||
                    (eSnepProtocolStatus ==
                    		 	    SNEP_CONNECTION_SENDING_N_FRAGMENTS))
            {
                //UARTprintf("RX: SYMM ");
                g_eNextPduQueue = LLCP_I_PDU;
            }
            else if((eSnepProtocolStatus == SNEP_CONNECTION_DATA_AVAILABLE)
            		&& (g_eLLCPConnectionStatus == LLCP_CONNECTION_IDLE
            				|| g_eLLCPConnectionStatus == LLCP_CONNECTION_ESTABLISHED))
            {
            	g_eNextPduQueue = LLCP_CONNECT_PDU;
            }
            else if((eSnepProtocolStatus == SNEP_CONNECTION_DATA_AVAILABLE)
            		&& (g_eLLCPConnectionStatus == LLCP_CONNECTION_SENDING))
            {
            	g_eNextPduQueue = LLCP_DISC_PDU;
            }
            else
            {
                if(g_eLLCPConnectionStatus != LLCP_CONNECTION_IDLE)
                {
                    g_eNextPduQueue = LLCP_SYMM_PDU;
                }
            }
        }
        else if(g_eCurrentServiceEnabled == HANDOVER_SERVICE)
        {
            if(g_eLLCPConnectionStatus == LLCP_CONNECTION_IDLE)
            {
                g_eNextPduQueue = LLCP_DISC_PDU;
            }
            else
                g_eNextPduQueue = LLCP_SYMM_PDU;
        }
        else
        {
            if(g_eLLCPConnectionStatus != LLCP_CONNECTION_IDLE)
            {
#if NFC_LLCP_DEBUG
            	Serial_printf("LLCP RX: SYMM\n",LLCP_LAYER_DATA);
#endif
                g_eNextPduQueue = LLCP_SYMM_PDU;
            }
        }
		break;
	case LLCP_PAX_PDU:
		g_eNextPduQueue = LLCP_CC_PDU;
#if NFC_LLCP_DEBUG
		Serial_printf("LLCP RX: PAX\n",LLCP_LAYER_DATA);
#endif
		break;
	case LLCP_AGF_PDU:
		// Not Supported
#if NFC_LLCP_DEBUG
		Serial_printf("LLCP RX: AGF\n",LLCP_LAYER_DATA);
#endif
		break;
	case LLCP_UI_PDU:
		// Not Supported
#if NFC_LLCP_DEBUG
		Serial_printf("LLCP RX: UI\n",LLCP_LAYER_DATA);
#endif
		break;
	case LLCP_CONNECT_PDU:
#if NFC_LLCP_DEBUG
		Serial_printf("LLCP RX: CONNECT\n",LLCP_LAYER_DATA);
#endif
		g_ui8dsapValue = (pui8RxBuffer[1] & 0x3F);

		ui8Counter = 0;
		g_eNextPduQueue = LLCP_CC_PDU;
        while(ui16PduLength > (ui8Counter+2))
        {
            //
            // Process the TLV - pass the starting address of the TLV
            //
            LLCP_processTLV(&pui8RxBuffer[ui8Counter+2]);

    		// Check Service Name TLV
    		if(pui8RxBuffer[ui8Counter+2] == 0x06)
    		{
    			if (pui8RxBuffer[ui8Counter+3] == 0x0F && pui8RxBuffer[ui8Counter+4] == 'u'
    					&& pui8RxBuffer[ui8Counter+5] == 'r' && pui8RxBuffer[ui8Counter+6] == 'n'
    					&& pui8RxBuffer[ui8Counter+7] == ':' && pui8RxBuffer[ui8Counter+8] == 'n'
    					&& pui8RxBuffer[ui8Counter+9] == 'f' && pui8RxBuffer[ui8Counter+10] == 'c'
    					&& pui8RxBuffer[ui8Counter+11] == ':' && pui8RxBuffer[ui8Counter+12] == 's'
    					&& pui8RxBuffer[ui8Counter+13] == 'n' && pui8RxBuffer[ui8Counter+14] == ':'
    					&& pui8RxBuffer[ui8Counter+15] == 's' && pui8RxBuffer[ui8Counter+16] == 'n'
    					&& pui8RxBuffer[ui8Counter+17] == 'e' && pui8RxBuffer[ui8Counter+18] == 'p')
    			{
    				// SNEP
    				g_eCurrentServiceEnabled = SNEP_SERVICE;
    				g_eNextPduQueue = LLCP_CC_PDU;
    			}
    			else if (pui8RxBuffer[ui8Counter+3] == 0x0F && pui8RxBuffer[ui8Counter+4] == 'c'
    					&& pui8RxBuffer[ui8Counter+5] == 'o' && pui8RxBuffer[ui8Counter+6] == 'm'
    					&& pui8RxBuffer[ui8Counter+7] == '.' && pui8RxBuffer[ui8Counter+8] == 'a'
    					&& pui8RxBuffer[ui8Counter+9] == 'n' && pui8RxBuffer[ui8Counter+10] == 'd'
    					&& pui8RxBuffer[ui8Counter+11] == 'r' && pui8RxBuffer[ui8Counter+12] == 'o'
    					&& pui8RxBuffer[ui8Counter+13] == 'i' && pui8RxBuffer[ui8Counter+14] == 'd'
    					&& pui8RxBuffer[ui8Counter+15] == '.' && pui8RxBuffer[ui8Counter+16] == 'n'
    					&& pui8RxBuffer[ui8Counter+17] == 'p' && pui8RxBuffer[ui8Counter+18] == 'p')
    			{
    //				g_eCurrentServiceEnabled = NPP_SERVICE;
    				g_eNextPduQueue = LLCP_SYMM_PDU;
    			}

    			else if (pui8RxBuffer[ui8Counter+3] == 0x13 && pui8RxBuffer[ui8Counter+4] == 'u'
    					&& pui8RxBuffer[ui8Counter+5] == 'r' && pui8RxBuffer[ui8Counter+6] == 'n'
    					&& pui8RxBuffer[ui8Counter+7] == ':' && pui8RxBuffer[ui8Counter+8] == 'n'
    					&& pui8RxBuffer[ui8Counter+9] == 'f' && pui8RxBuffer[ui8Counter+10] == 'c'
    					&& pui8RxBuffer[ui8Counter+11] == ':' && pui8RxBuffer[ui8Counter+12] == 's'
    					&& pui8RxBuffer[ui8Counter+13] == 'n' && pui8RxBuffer[ui8Counter+14] == ':'
    					&& pui8RxBuffer[ui8Counter+15] == 'h' && pui8RxBuffer[ui8Counter+16] == 'a'
    					&& pui8RxBuffer[ui8Counter+17] == 'n' && pui8RxBuffer[ui8Counter+18] == 'd'
    					&& pui8RxBuffer[ui8Counter+19] == 'o' && pui8RxBuffer[ui8Counter+20] == 'v'
    					&& pui8RxBuffer[ui8Counter+21] == 'o' && pui8RxBuffer[ui8Counter+22] == 'v')

    			{
    				// Handover
    //				g_eCurrentServiceEnabled = HANDOVER_SERVICE;
    			}
    			else
    			{
    				g_ui8ssapValue = 0x01;
    				g_eDMReason = DM_REASON_LLCP_RECEIVED_CONNECT_PDU_NO_SERVICE;
    				g_eNextPduQueue = LLCP_DM_PDU;
    			}

    		}

            //
            // Increment ui8Counter by the length+ 2 (type and length) of
            // the current TLV
            //
    		ui8Counter = ui8Counter+ pui8RxBuffer[ui8Counter+3] + 2;
		}

		if(ui16PduLength == 0x02)
		{
			g_eNextPduQueue = LLCP_CC_PDU;
		}

		break;
	case LLCP_DISC_PDU:
#if NFC_LLCP_DEBUG
		Serial_printf("LLCP RX: DISC\n",LLCP_LAYER_DATA);
#endif
		g_eDMReason = DM_REASON_LLCP_RECEIVED_DISC_PDU;
		g_eNextPduQueue = LLCP_DM_PDU;
		break;
	case LLCP_CC_PDU:
#if NFC_LLCP_DEBUG
		Serial_printf("LLCP RX: CC\n",LLCP_LAYER_DATA);
#endif
		g_ui8dsapValue = (pui8RxBuffer[1] & 0x3F);
		g_eNextPduQueue = LLCP_I_PDU;
		break;
	case LLCP_DM_PDU:
#if NFC_LLCP_DEBUG
		Serial_printf("LLCP RX: DM\n",LLCP_LAYER_DATA);
#endif
		g_eLLCPConnectionStatus = LLCP_CONNECTION_IDLE;
		// Reset the snep communication
		g_eNextPduQueue = LLCP_SYMM_PDU;
		break;
	case LLCP_FRMR_PDU:
		// Not Supported
#if NFC_LLCP_DEBUG
		Serial_printf("LLCP RX: FRMR\n",LLCP_LAYER_DATA);
#endif
		break;
	case LLCP_SNL_PDU:
		// Not Supported
#if NFC_LLCP_DEBUG
		Serial_printf("LLCP RX: SNL\n",LLCP_LAYER_DATA);
#endif
		break;
	case LLCP_I_PDU:
#if NFC_LLCP_DEBUG
		Serial_printf("LLCP RX: I\n",LLCP_LAYER_DATA);
#endif
		if(g_eCurrentServiceEnabled == SNEP_SERVICE)
			SNEP_processReceivedData(&pui8RxBuffer[3],ui16PduLength-3);
		else if(g_eCurrentServiceEnabled == NPP_SERVICE)
		{
			// Not Supported
		}
		else if(g_eCurrentServiceEnabled == HANDOVER_SERVICE)
		{
			// Not Supported
		}
		if(g_eLLCPConnectionStatus == LLCP_CONNECTION_ESTABLISHED)
		{
			g_eLLCPConnectionStatus = LLCP_CONNECTION_RECEIVING;
		}
		g_eNextPduQueue = LLCP_RR_PDU;
		break;
	case LLCP_RR_PDU:
#if NFC_LLCP_DEBUG
		Serial_printf("LLCP RX: RR\n",LLCP_LAYER_DATA);
#endif
		g_eNextPduQueue = LLCP_SYMM_PDU;
		eSnepProtocolStatus = SNEP_getProtocolStatus();
		if(g_eLLCPConnectionStatus == LLCP_CONNECTION_SENDING)
		{
			if(
				(eSnepProtocolStatus ==
									SNEP_CONNECTION_WAITING_FOR_CONTINUE) ||
			   (eSnepProtocolStatus ==
									SNEP_CONNECTION_WAITING_FOR_SUCCESS))
			{
			   g_eNextPduQueue = LLCP_SYMM_PDU;
			}
			else if(eSnepProtocolStatus ==
										SNEP_CONNECTION_SENDING_N_FRAGMENTS)
			{
				g_eNextPduQueue = LLCP_I_PDU;
			}
			else if(eSnepProtocolStatus == SNEP_CONNECTION_SEND_COMPLETE)
			{
				g_eNextPduQueue = LLCP_DISC_PDU;
			}
		}
		else
		{
			g_eNextPduQueue = LLCP_SYMM_PDU;
		}

		break;
	case LLCP_RNR_PDU:
		// Not Supported
#if NFC_LLCP_DEBUG
		Serial_printf("LLCP RX: RNR\n",LLCP_LAYER_DATA);
#endif
		break;
	case LLCP_RESERVED_PDU:
		// Not Supported
#if NFC_LLCP_DEBUG
		Serial_printf("LLCP RX: RESERVED\n",LLCP_LAYER_DATA);
#endif
		break;
	default:
#if NFC_LLCP_DEBUG
		Serial_printf("LLCP RX: LLCP\n",LLCP_LAYER_DATA);
#endif
		ui8LLCPStatus = STATUS_FAIL;
		break;
	}

	return ui8LLCPStatus;
}

//*****************************************************************************
//
//! LLCP_sendSYMM - Send SYMM message
//!
//! \param pui8PduBufferPtr is the start pointer to store the SYMM PDU.
//!
//! This function adds a SYMM PDU starting at pui8PduBufferPtr.For more
//~ details on this PDU read LLCP V1.1 Section 4.3.1.
//!
//! \return ui8IndexTemp is the length of the SYMM PDU.
//
//*****************************************************************************
uint8_t LLCP_sendSYMM(uint8_t * pui8PduBufferPtr)
{
    uint8_t ui8IndexTemp = 0;
    // DSAP (6 bits)  PTYPE (4 bits)  SSAP (6 bits)
    pui8PduBufferPtr[ui8IndexTemp++] = ( (LLCP_SYMM_PDU & 0xFC) >> 2);
    pui8PduBufferPtr[ui8IndexTemp++] = ( (LLCP_SYMM_PDU & 0x03) << 6);
    return ui8IndexTemp;
}

//*****************************************************************************
//
//! LLCP_sendPAX - Send PAX message
//!
//! \param pui8PduBufferPtr is the start pointer to store the PAX PDU.
//!
//! This function adds a PAX PDU starting at pui8PduBufferPtr.For more
//~ details on this PDU read LLCP V1.1 Section 4.3.2.
//!
//! \return ui8IndexTemp is the length of the PAX PDU.
//
//*****************************************************************************
uint8_t LLCP_sendPAX(uint8_t * pui8PduBufferPtr)
{
    uint8_t ui8IndexTemp = 0;
    // DSAP (6 bits)  PTYPE (4 bits)  SSAP (6 bits)
    pui8PduBufferPtr[ui8IndexTemp++] = ( (LLCP_PAX_PDU & 0xFC) >> 2);
    pui8PduBufferPtr[ui8IndexTemp++] = ( (LLCP_PAX_PDU & 0x03) << 6);
    pui8PduBufferPtr[ui8IndexTemp++] = LLCP_MIUX;
    pui8PduBufferPtr[ui8IndexTemp++] = LLCP_LTO;
    return ui8IndexTemp;
}

//*****************************************************************************
//
//! LLCP_sendCONNECT - Send CONNECT message
//!
//! \param pui8PduBufferPtr is the start pointer to store the CONNECT PDU.
//!
//! This function adds a CONNECT PDU starting at pui8PduBufferPtr.For more
//~ details on this PDU read LLCP V1.1 Section 4.3.5.
//!
//! \return ui8IndexTemp is the length of the CONNECT PDU.
//
//*****************************************************************************
uint8_t LLCP_sendCONNECT(uint8_t * pui8PduBufferPtr)
{
    uint8_t ui8IndexTemp = 0;

    g_eCurrentServiceEnabled = SNEP_SERVICE;

    //
    // Reset NR and NS
    //
    g_ui8NSNR = 0x00;

    g_eLLCPConnectionStatus = LLCP_CONNECTION_SENDING;

    g_ui8ssapValue = LLCP_SSAP_CONNECT_SEND;
    g_ui8dsapValue = DSAP_SERVICE_DISCOVERY_PROTOCOL;


    // DSAP (6 bits)  PTYPE (4 bits)  SSAP (6 bits)
    pui8PduBufferPtr[ui8IndexTemp++] = (g_ui8dsapValue << 2) |
                                        ( (LLCP_CONNECT_PDU & 0xFC) >> 2);
    pui8PduBufferPtr[ui8IndexTemp++] = ( (LLCP_CONNECT_PDU & 0x03) << 6) |
                                        g_ui8ssapValue;

    //
    // TLV Fields
    //
    ui8IndexTemp = ui8IndexTemp +
                        LLCP_addTLV(LLCP_SN, &pui8PduBufferPtr[ui8IndexTemp]);
    ui8IndexTemp = ui8IndexTemp +
                        LLCP_addTLV(LLCP_MIUX, &pui8PduBufferPtr[ui8IndexTemp]);
    ui8IndexTemp = ui8IndexTemp +
                        LLCP_addTLV(LLCP_RW, &pui8PduBufferPtr[ui8IndexTemp]);

    return ui8IndexTemp;
}

//*****************************************************************************
//
//! LLCP_sendDISC - Send DISC message
//!
//! \param pui8PduBufferPtr is the start pointer to store the DISC PDU.
//!
//! This function adds a DISC PDU starting at pui8PduBufferPtr.For more details
//! on this PDU read LLCP V1.1 Section 4.3.6.
//!
//! \return ui8IndexTemp is the length of the DISC PDU.
//
//*****************************************************************************
uint8_t LLCP_sendDISC(uint8_t * pui8PduBufferPtr)
{
    uint8_t ui8IndexTemp = 0;

    // DSAP (6 bits)  PTYPE (4 bits)  SSAP (6 bits)
    pui8PduBufferPtr[ui8IndexTemp++] = (g_ui8dsapValue << 2) |
                                        ( (LLCP_DISC_PDU & 0xFC) >> 2);
    pui8PduBufferPtr[ui8IndexTemp++] = ( (LLCP_DISC_PDU & 0x03) << 6) |
                                            g_ui8ssapValue;

    return ui8IndexTemp;
}

//*****************************************************************************
//
//! LLCP_sendCC - Send CC message
//!
//! \param pui8PduBufferPtr is the start pointer to store the CC PDU.
//!
//! This function adds a CC PDU starting at pui8PduBufferPtr.For more details
//! on this PDU read LLCP V1.1 Section 4.3.7.
//!
//! \return ui8IndexTemp is the length of the CC PDU.
//
//*****************************************************************************
uint8_t LLCP_sendCC(uint8_t * pui8PduBufferPtr)
{
    uint8_t ui8IndexTemp = 0;

    g_eLLCPConnectionStatus = LLCP_CONNECTION_ESTABLISHED;

    //
    // Reset NR and NS
    //
    g_ui8NSNR = 0x00;

    g_ui8ssapValue = LLCP_SSAP_CONNECT_RECEIVED;

    // DSAP (6 bits)  PTYPE (4 bits)  SSAP (6 bits)
    pui8PduBufferPtr[ui8IndexTemp++] = (g_ui8dsapValue << 2) |
                                            ( (LLCP_CC_PDU & 0xFC) >> 2);
    pui8PduBufferPtr[ui8IndexTemp++] = ( (LLCP_CC_PDU & 0x03) << 6) |
                                            g_ui8ssapValue;

    //
    // TLV Fields
    //
    ui8IndexTemp = ui8IndexTemp +
                        LLCP_addTLV(LLCP_MIUX, &pui8PduBufferPtr[ui8IndexTemp]);
    ui8IndexTemp = ui8IndexTemp +
                        LLCP_addTLV(LLCP_RW, &pui8PduBufferPtr[ui8IndexTemp]);

    return ui8IndexTemp;
}

//*****************************************************************************
//
//! LLCP_sendDM - Send DM message
//!
//! \param pui8PduBufferPtr is the start pointer to store the DM PDU.
//! \param eDmReason is the enumeration of the disconnection reason.
//!
//! The \e eDmReason parameter can be any of the following:
//!
//! - \b DM_REASON_LLCP_RECEIVED_DISC_PDU
//! - \b DM_REASON_LLCP_RECEIVED_CONNECTION_ORIENTED_PDU
//! - \b DM_REASON_LLCP_RECEIVED_CONNECT_PDU_NO_SERVICE
//! - \b DM_REASON_LLCP_PROCESSED_CONNECT_PDU_REQ_REJECTED
//! - \b DM_REASON_LLCP_PERMNANTLY_NOT_ACCEPT_CONNECT_WITH_SAME_SSAP
//! - \b DM_REASON_LLCP_PERMNANTLY_NOT_ACCEPT_CONNECT_WITH_ANY_SSAP
//! - \b DM_REASON_LLCP_TEMMPORARILY_NOT_ACCEPT_PDU_WITH_SAME_SSSAPT
//! - \b DM_REASON_LLCP_TEMMPORARILY_NOT_ACCEPT_PDU_WITH_ANY_SSSAPT
//!
//! This function adds a DM PDU starting at pui8PduBufferPtr with a dm_reason.
//! For more details on this PDU read LLCP V1.1 Section 4.3.8.
//!
//! \return ui8IndexTemp is the length of the DM PDU.
//
//*****************************************************************************
uint8_t LLCP_sendDM(uint8_t * pui8PduBufferPtr,tDisconnectModeReason eDmReason)
{
    uint8_t ui8IndexTemp = 0;

    // DSAP (6 bits)  PTYPE (4 bits)  SSAP (6 bits)
    pui8PduBufferPtr[ui8IndexTemp++] = (g_ui8dsapValue << 2) |
                                            ( (LLCP_DM_PDU & 0xFC) >> 2);
    pui8PduBufferPtr[ui8IndexTemp++] = ( (LLCP_DM_PDU & 0x03) << 6) |
                                            g_ui8ssapValue;

    pui8PduBufferPtr[ui8IndexTemp++] = (uint8_t) eDmReason;

    return ui8IndexTemp;
}

//*****************************************************************************
//
//! LLCP_sendI - Send I message
//!
//! \param pui8PduBufferPtr is the start pointer to store the I PDU.
//!
//! This function adds a I PDU starting at pui8PduBufferPtr.For more details
//! on this PDU read LLCP V1.1 Section 4.3.10.
//!
//! \return ui8IndexTemp is the length of the I PDU.
//
//*****************************************************************************
uint8_t LLCP_sendI(uint8_t * pui8PduBufferPtr)
{
    uint8_t ui8IndexTemp = 0;
    uint8_t ui8SnepPacketSize;

    if(g_eLLCPConnectionStatus == LLCP_CONNECTION_ESTABLISHED)
    {
        g_eLLCPConnectionStatus = LLCP_CONNECTION_SENDING;
    }

    if(SNEP_getProtocolStatus() == SNEP_CONNECTION_RECEIVE_COMPLETE)
    {
    	g_eLLCPConnectionStatus = LLCP_CONNECTION_ESTABLISHED;
    }

    if(g_eCurrentServiceEnabled == SNEP_SERVICE)
    {
    	// Check if there is an available SNEP packet.
    	ui8SnepPacketSize = SNEP_stateMachine(&pui8PduBufferPtr[ui8IndexTemp+3]);

    	if(ui8SnepPacketSize > 0)
		{
    	    // DSAP (6 bits)  PTYPE (4 bits)  SSAP (6 bits)
    	    pui8PduBufferPtr[ui8IndexTemp++] = (g_ui8dsapValue << 2) | ( (LLCP_I_PDU & 0xFC) >> 2);
    	    pui8PduBufferPtr[ui8IndexTemp++] = ( (LLCP_I_PDU & 0x03) << 6) | g_ui8ssapValue;

    	    pui8PduBufferPtr[ui8IndexTemp++] = g_ui8NSNR;

    	    g_ui8NSNR = (g_ui8NSNR & 0x0F) | (((g_ui8NSNR >> 4) + 0x01) << 4); // Increment N(S)

    		ui8IndexTemp = ui8IndexTemp + ui8SnepPacketSize;
		}
    	else
    	{
    		// Send Symmetry
    		ui8IndexTemp = LLCP_sendSYMM(pui8PduBufferPtr);
    	}
    }
    else if(g_eCurrentServiceEnabled == NPP_SERVICE)
    {
        // RFU
    }

    return ui8IndexTemp;
}


//*****************************************************************************
//
//! LLCP_sendRR - Send RR message
//!
//! \param pui8PduBufferPtr is the start pointer to store the RR PDU.
//!
//! This function adds a RR PDU starting at pui8PduBufferPtr.For more details
//! on this PDU read LLCP V1.1 Section 4.3.11.
//!
//! \return ui8IndexTemp is the length of the RR PDU.
//
//*****************************************************************************
uint8_t LLCP_sendRR(uint8_t * pui8PduBufferPtr)
{
    uint8_t ui8IndexTemp = 0;

    // DSAP (6 bits)  PTYPE (4 bits)  SSAP (6 bits)
    pui8PduBufferPtr[ui8IndexTemp++] = (g_ui8dsapValue << 2) | \
                                      ((LLCP_RR_PDU & 0xFC) >> 2);
    pui8PduBufferPtr[ui8IndexTemp++] = ( (LLCP_RR_PDU & 0x03) << 6) | \
                                         g_ui8ssapValue;

    // Increment N(R)
    g_ui8NSNR = (g_ui8NSNR & 0xF0) | ((g_ui8NSNR + 0x01) & 0x0F);

    pui8PduBufferPtr[ui8IndexTemp++] = (g_ui8NSNR & 0x0F);

    return ui8IndexTemp;
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
