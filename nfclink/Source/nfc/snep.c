//*****************************************************************************
//
// snep.c - Implementation of Simple NDEF Exchange Protocol, used by LLCP 
// and the main application. 
//
// Copyright (c) 2015 Texas Instruments Incorporated.  All rights reserved.
// TI Information - Selective Disclosure
//
//*****************************************************************************

#include "snep.h"

//*****************************************************************************
//
//! \addtogroup nfc_snep_api NFC SNEP API Functions
//! @{
//! Simple NDEF Exchange Protocol is an application protocol used by the LLCP
//! layer to send / receive NDEFs between two NFC Forum Devices operating 
//! in Peer to Peer Mode ( 1 Target and 1 Initiator). For more information 
//! on SNEP please read the NFC Simple NDEF Exchange Protocol Specification 
//! Version 1.0.
//
//*****************************************************************************

//*****************************************************************************
//
// Stores the length of the tx/rx packet.
//
//*****************************************************************************
uint32_t g_ui32SNEPPacketLength;

//*****************************************************************************
//
// Stores the length of the available fragment queue.
//
//*****************************************************************************
uint8_t g_ui8FragQueue;

//*****************************************************************************
//
// Stores the length of the current tx fragment.
//
//*****************************************************************************
uint8_t g_ui8TFragLength;

//*****************************************************************************
//
// Stores the data to be sent.
//
//*****************************************************************************
uint8_t g_pui8SNEPTxBuffer[SNEP_MAX_BUFFER];

//*****************************************************************************
//
// g_pui8SNEPTxPacketPtr points to the first location of the data to be
// transferred
//
//*****************************************************************************
uint8_t * g_pui8SNEPRxPacketPtr;

//*****************************************************************************
//
// Stores the remaining rx byte count
//
//*****************************************************************************
uint32_t g_ui32SNEPRemainingRxPayloadBytes;

//*****************************************************************************
//
// Stores the bytes received in the current I-PDU transaction
//
//*****************************************************************************
uint16_t g_ui16SNEPReceivedBytes;

//*****************************************************************************
//
// Stores the status of the incoming packet
//
//*****************************************************************************
tPacketStatus g_eRxPacketStatus;

//*****************************************************************************
//
// Stores the status of the SNEP communication
//
//*****************************************************************************
tSNEPConnectionStatus g_eSNEPConnectionStatus;

//*****************************************************************************
//
// Stores the maximum size of each SNEP packet.
//
//*****************************************************************************
uint8_t g_ui8MaxPayload;

//*****************************************************************************
//
//! SNEP_init - Initialize the Simple NDEF Exchange Protocol driver.
//!
//! This function must be called prior to any other function offered by the
//! SNEP driver. This function initializes the SNEP status, tx/rx packet length
//! and maximum payload size. This function must be called by the LLCP_init().
//!
//! \return None.
//
//*****************************************************************************
void SNEP_init(void)
{
    g_eSNEPConnectionStatus = SNEP_CONNECTION_IDLE;
    g_eRxPacketStatus = RECEIVED_NO_FRAGMENT;
    g_ui32SNEPRemainingRxPayloadBytes = 0;
    g_ui16SNEPReceivedBytes = 0;
    g_ui8MaxPayload = SNEP_MAX_BUFFER;
    g_ui8FragQueue = g_ui8MaxPayload-6;
    g_ui32SNEPPacketLength = 0;
    g_ui8TFragLength = 0;
}

//*****************************************************************************
//
//! SNEP_setMaxPayload - Set the Maximum size of each fragment. 
//!
//! \param ui8MaxPayload is the maximum size of each fragment. 
//! 
//! This function must be called inside LLCP_processTLV() to define the maxium 
//! size of each fragment based on the Maximum Information Unit (MIU) supported 
//! by the target/initiator.
//!
//! \return None.
//
//*****************************************************************************
void SNEP_setMaxPayload(uint8_t ui8MaxPayload)
{
	if(ui8MaxPayload <= SNEP_MAX_BUFFER)
	{
		g_ui8MaxPayload = ui8MaxPayload;
		g_ui8FragQueue = ui8MaxPayload - 6;
	}
}

//*****************************************************************************
//
//! SNEP_stateMachine - Decides which request/response to fill inside the LLCP
//! packet.
//!
//! \param pui8BufferPtr is the starting pointer where the SNEP request / 
//! response should be written to.
//! 
//! This function must be called inside LLCP_sendI() to check if there are
//! any queued fragment to be transmitted or if there is a queued response.
//! If the packet length returned is equal to 0, then there are no
//! commands to be sent.
//!
//! \return ui8PacketLength is the length of the request/response written to
//! pui8BufferPtr.
//
//*****************************************************************************
uint8_t SNEP_stateMachine(uint8_t * pui8BufferPtr)
{
	uint8_t ui8PacketLength = 0;

	switch(g_eSNEPConnectionStatus)
	{
	case SNEP_CONNECTION_IDLE:
		    // Set the frag queue size available.
        g_ui8FragQueue = g_ui8MaxPayload-6;

        // Reset current fragment size to 0
        g_ui8TFragLength = 0;
		break;
	case SNEP_WRONG_VERSION_RECEIVED:
		ui8PacketLength = SNEP_sendResponse(pui8BufferPtr,SNEP_RESPONSE_REJECT);
		break;
	case SNEP_CONNECTION_RECEIVED_FIRST_PACKET:
		ui8PacketLength = SNEP_sendResponse(pui8BufferPtr,SNEP_RESPONSE_CONTINUE);
		break;
	case SNEP_CONNECTION_RECEIVING_N_FRAGMENTS:
		// Do Nothing
		break;
	case SNEP_CONNECTION_DATA_AVAILABLE:
		// Check if there is a fragment queued
		if(g_ui8TFragLength != 0)
		{
			ui8PacketLength = SNEP_sendRequest(pui8BufferPtr,SNEP_REQUEST_PUT);
		}
		break;
	case SNEP_CONNECTION_WAITING_FOR_CONTINUE:
		// Do Nothing
		break;
	case SNEP_CONNECTION_WAITING_FOR_SUCCESS:
		// Do nothing
		break;
	case SNEP_CONNECTION_SENDING_N_FRAGMENTS:
		// Check if there is a fragment queued
		if(g_ui8TFragLength != 0)
		{
			ui8PacketLength = SNEP_sendRequest(pui8BufferPtr,SNEP_REQUEST_PUT);
		}
		break;
	case SNEP_CONNECTION_SEND_COMPLETE:
		g_eSNEPConnectionStatus = SNEP_CONNECTION_IDLE;
		break;
	case SNEP_CONNECTION_RECEIVE_COMPLETE:
		ui8PacketLength = SNEP_sendResponse(pui8BufferPtr,SNEP_RESPONSE_SUCCESS);
		break;
	case SNEP_CONNECTION_EXCESS_SIZE:
		ui8PacketLength = SNEP_sendResponse(pui8BufferPtr,SNEP_RESPONSE_REJECT);
		break;
	default:
		// Do nothing
		break;
	}

	return ui8PacketLength;

}

//*****************************************************************************
//
//! SNEP_setupPacket - Setup a packet to be transmitted via the SNEP transport 
//! layer.
//!
//! \param pui8PacketPtr is the starting pointer of the data to be transmitted.
//! \param ui32PacketLength is the total length of the packet.
//! \param ui8FragmentLength is the length of the first fragment. It must not 
//! greater than SNEP_getTxBufferStatus(). If the total packet size is greater
//! than SNEP_getTxBufferStatus(), then ui8FragmentLength must be equal to 
//! SNEP_getTxBufferStatus() for best throughput. 
//! 
//! This function must be called by main application after calling 
//! SNEP_getTxBufferStatus() to initialize the packet to be sent to the SNEP
//! server. If the total packet is greater than SNEP_getTxBufferStatus(), the
//! remaining of the packet must be queued with SNEP_enqueue().
//!
//! \return ui8PacketSetupStatus is STATUS_SUCCESS if the packet was
//! queued or STATUS_FAIL if there was a current transfer ongoing. 
//
//*****************************************************************************
uint8_t SNEP_setupPacket(uint8_t * pui8PacketPtr,
						uint32_t ui32PacketLength, 
						uint8_t ui8FragmentLength)
{
    uint8_t ui8PacketSetupStatus;
    uint8_t ui8Index;

    if(g_eSNEPConnectionStatus == SNEP_CONNECTION_IDLE 
	   && ui8FragmentLength <= g_ui8FragQueue)
    {
    	// Copy the fragment into the local SNEP Tx buffer.
    	for(ui8Index = 0; ui8Index < ui8FragmentLength; ui8Index++)
    	{
    		g_pui8SNEPTxBuffer[ui8Index] = pui8PacketPtr[ui8Index];
    	}
        g_ui32SNEPPacketLength = ui32PacketLength;

        // Set the packet size available to 0.
        g_ui8FragQueue = 0;

        g_ui8TFragLength = ui8FragmentLength;

        ui8PacketSetupStatus = STATUS_SUCCESS;

        g_eSNEPConnectionStatus = SNEP_CONNECTION_DATA_AVAILABLE;
    }
    else
    {
        ui8PacketSetupStatus = STATUS_FAIL;
    }

    return ui8PacketSetupStatus;
}

//*****************************************************************************
//
//! SNEP_getTxBufferStatus - Returns the size of the available tx fragment. 
//! 
//! This function must be called by main application before calling 
//! SNEP_setupPacket() to check for the available tx fragment size.
//!
//! \return g_ui8FragQueue is the size of the available tx fragment.
//
//*****************************************************************************
uint8_t SNEP_getTxBufferStatus(void)
{
	return g_ui8FragQueue;
}

//*****************************************************************************
//
//! SNEP_enqueue - Used to queue fragments of the remaining of the packet.
//!
//! \param pui8PacketPtr is the starting pointer of the data to be transmitted.
//! \param ui8Length is the length of the fragment. It must not 
//! greater than SNEP_getTxBufferStatus(). If the remaining packet size is 
//! greater than SNEP_getTxBufferStatus(), then it must be equal to 
//! ui8FragmentLength for best throughput. 
//! 
//! This function must be called by main application after calling 
//! SNEP_getTxBufferStatus() to send the remaining of the packet.
//!
//! \return ui8PacketSetupStatus is STATUS_SUCCESS if the fragment was
//! queued or STATUS_FAIL if ui8Length was greater than the available fragment
//! size.
//
//*****************************************************************************
uint8_t SNEP_enqueue(uint8_t * pui8PacketPtr, uint8_t ui8Length)
{
	uint8_t ui8PacketSetupStatus;
	uint8_t ui8Index;

	// Check if the fragment length is not greater than the avaialble space
	if(g_eSNEPConnectionStatus == SNEP_CONNECTION_SENDING_N_FRAGMENTS &&
			ui8Length <= g_ui8FragQueue)
	{
		for(ui8Index = 0; ui8Index < g_ui8FragQueue; ui8Index++)
		{
			g_pui8SNEPTxBuffer[ui8Index] = pui8PacketPtr[ui8Index];
		}

        // Set the fragment size available to 0.
        g_ui8FragQueue = 0;

        g_ui8TFragLength = ui8Length;

		ui8PacketSetupStatus = STATUS_SUCCESS;
	}
	else
	{
		ui8PacketSetupStatus = STATUS_FAIL;
	}

	return ui8PacketSetupStatus;
}

//*****************************************************************************
//
//! SNEP_sendRequest - Sends request to the server.
//!
//! \param pui8DataPtr is the start pointer where the request will be written. 
//! \param eRequestCmd is the request command to be sent.
//!
//! The \e eRequestCmd parameter can be any of the following:
//!
//! - \b SNEP_REQUEST_CONTINUE          - Send remaining fragments
//! - \b SNEP_REQUEST_GET               - Return an NDEF message
//! - \b SNEP_REQUEST_PUT               - Accept an NDEF message
//! - \b SNEP_REQUEST_REJECT            - Do not send remaining fragments
//!
//! This function sends a SNEP request from the SNEP client to a SNEP server. 
//! It must be called from the SNEP_stateMachine().
//!
//! \return ui8offset is the length of the request written starting at
//!  pui8DataPtr.
//
//*****************************************************************************
uint8_t SNEP_sendRequest(uint8_t * pui8DataPtr, tSNEPCommands eRequestCmd)
{
    uint8_t ui8PacketLength;
    uint8_t ui8offset = 0;
    volatile uint8_t ui8counter;

    switch(eRequestCmd)
    {
    case SNEP_REQUEST_CONTINUE:
        break;
    case SNEP_REQUEST_GET:
        break;
    case SNEP_REQUEST_PUT:
        if(g_eSNEPConnectionStatus == SNEP_CONNECTION_DATA_AVAILABLE)
        {
#if NFC_SNEP_DEBUG
        	Serial_printf("SNEP TX 1st Fragment\n",NFC_SNEP_LAYER_DATA);
#endif

            //
            // SNEP Protocol Version
            //
            pui8DataPtr[ui8offset++] = SNEP_VERSION;

            //
            // Request Field
            //
            pui8DataPtr[ui8offset++] = (uint8_t) SNEP_REQUEST_PUT;

            //
            // Length (4 bytes)
            //
            pui8DataPtr[ui8offset++] = (uint8_t) ((g_ui32SNEPPacketLength & 0xFF000000) >> 24);
            pui8DataPtr[ui8offset++] = (uint8_t) ((g_ui32SNEPPacketLength & 0x00FF0000) >> 16);
            pui8DataPtr[ui8offset++] = (uint8_t) ((g_ui32SNEPPacketLength & 0x0000FF00) >> 8);
            pui8DataPtr[ui8offset++] = (uint8_t) (g_ui32SNEPPacketLength & 0x000000FF);

            // The PUT Request has 6 bytes of overhead (Version (1) Request Field (1) Length (4)).
            if( g_ui32SNEPPacketLength > (g_ui8MaxPayload - 6))
            {
                //
                // Remaining bytes = Total Length - (g_ui8MaxPayload - 6)
                //
                g_ui32SNEPPacketLength  = g_ui32SNEPPacketLength - (g_ui8MaxPayload - 6);
                ui8PacketLength = (g_ui8MaxPayload - 6);

                //
                // Change connection status to waiting for continue
                //
                g_eSNEPConnectionStatus = SNEP_CONNECTION_WAITING_FOR_CONTINUE;

    	        // Set the packet size available to g_ui8MaxPayload.
    	        g_ui8FragQueue = g_ui8MaxPayload;
    	        // Reset size to 0
    	        g_ui8TFragLength = 0;
            }
            else
            {
                ui8PacketLength = g_ui32SNEPPacketLength;
                g_ui32SNEPPacketLength = 0;

                //
                // Change connection status to waiting for success
                //
                g_eSNEPConnectionStatus = SNEP_CONNECTION_WAITING_FOR_SUCCESS;

    	        // Set the packet size available to g_ui8MaxPayload.
    	        g_ui8FragQueue = g_ui8MaxPayload-6;
    	        // Reset size to 0
    	        g_ui8TFragLength = 0;
            }

            //
            // Copy the snep_packet buffer into the pui8DataPtr
            //
            for(ui8counter = 0; ui8counter < ui8PacketLength; ui8counter++)
            {
                pui8DataPtr[ui8offset++] =  g_pui8SNEPTxBuffer[ui8counter];
            }
        }
        else if(g_eSNEPConnectionStatus == SNEP_CONNECTION_SENDING_N_FRAGMENTS)
        {
#if NFC_SNEP_DEBUG
        	Serial_printf("SNEP TX N Fragment\n",NFC_SNEP_LAYER_DATA);
#endif
            if( g_ui32SNEPPacketLength > g_ui8MaxPayload)
            {
                ui8PacketLength = g_ui8MaxPayload;

    	        // Set the packet size available to g_ui8MaxPayload.
    	        g_ui8FragQueue = g_ui8MaxPayload;
    	        // Reset size to 0
    	        g_ui8TFragLength = 0;
            }
            else
            {
                //
                // Remaining bytes = 0
                //
            	ui8PacketLength = g_ui32SNEPPacketLength;

                g_eSNEPConnectionStatus = SNEP_CONNECTION_WAITING_FOR_SUCCESS;

    	        // Set the packet size available to g_ui8MaxPayload.
    	        g_ui8FragQueue = g_ui8MaxPayload-6;
    	        // Reset size to 0
    	        g_ui8TFragLength = 0;
            }

            //
            // Copy the snep_packet buffer into the pui8DataPtr
            //
            for(ui8counter = 0; ui8counter < ui8PacketLength; ui8counter++)
            {
                pui8DataPtr[ui8offset++] =  g_pui8SNEPTxBuffer[ui8counter];
            }

            g_ui32SNEPPacketLength = g_ui32SNEPPacketLength - ui8PacketLength;

        }
        break;
    case SNEP_REQUEST_REJECT:
        break;
    }

    return ui8offset;
}


//*****************************************************************************
//
//! SNEP_sendResponse - Sends response to the client.
//!
//! \param pui8DataPtr is the start pointer where the response will be written. 
//! \param eResponseCmd is the response command to be sent.
//!
//! The \e eResponseCmd parameter can be any of the following:
//!
//! - \b SNEP_RESPONSE_CONTINUE         - Continue send remaining fragments
//! - \b SNEP_RESPONSE_SUCCESS          - Operation succeeded
//! - \b SNEP_RESPONSE_NOT_FOUND        - Resource not found
//! - \b SNEP_RESPONSE_EXCESS_DATA      - Resource exceeds data size limit
//! - \b SNEP_RESPONSE_BAD_REQUEST      - Malformed request not understood
//! - \b SNEP_RESPONSE_NOT_IMPLEMENTED  - Unsupported functionality requested
//! - \b SNEP_RESPONSE_UNSUPPORTED_VER  - Unsupported protocol version
//! - \b SNEP_RESPONSE_REJECT           - Do not send remaining fragments
//!
//! This function sends a SNEP response from the SNEP server to a SNEP client. 
//! It must be called from the SNEP_stateMachine().
//!
//! \return ui8offset is the length of the response written starting at
//!  pui8DataPtr.
//
//*****************************************************************************
uint8_t SNEP_sendResponse(uint8_t * pui8DataPtr, tSNEPCommands eResponseCmd)
{
    uint8_t ui8offset = 0;

    switch(eResponseCmd)
    {
    case SNEP_RESPONSE_CONTINUE:
#if NFC_SNEP_DEBUG
        	Serial_printf("SNEP TX Continue\n",NFC_SNEP_LAYER_DATA);
#endif
		//
		// SNEP Protocol Version
		//
		pui8DataPtr[ui8offset++] = SNEP_VERSION;

		//
		// Response Field
		//
		pui8DataPtr[ui8offset++] = (uint8_t) SNEP_RESPONSE_CONTINUE;

		//
		// Length (4 bytes)
		//
		pui8DataPtr[ui8offset++] = 0x00;
		pui8DataPtr[ui8offset++] = 0x00;
		pui8DataPtr[ui8offset++] = 0x00;
		pui8DataPtr[ui8offset++] = 0x00;
		g_eSNEPConnectionStatus = SNEP_CONNECTION_RECEIVING_N_FRAGMENTS;
        break;
    case SNEP_RESPONSE_SUCCESS:
#if NFC_SNEP_DEBUG
        	Serial_printf("SNEP TX Success\n",NFC_SNEP_LAYER_DATA);
#endif
		//
		// SNEP Protocol Version
		//
		pui8DataPtr[ui8offset++] = SNEP_VERSION;

		//
		// Response Field
		//
		pui8DataPtr[ui8offset++] = (uint8_t) SNEP_RESPONSE_SUCCESS;

		//
		// Length (4 bytes)
		//
		pui8DataPtr[ui8offset++] = 0x00;
		pui8DataPtr[ui8offset++] = 0x00;
		pui8DataPtr[ui8offset++] = 0x00;
		pui8DataPtr[ui8offset++] = 0x00;
		g_eSNEPConnectionStatus = SNEP_CONNECTION_IDLE;
        break;
    case SNEP_RESPONSE_NOT_FOUND:
        break;
    case SNEP_RESPONSE_EXCESS_DATA:
        break;
    case SNEP_RESPONSE_BAD_REQUEST:
        break;
    case SNEP_RESPONSE_NOT_IMPLEMENTED:
        break;
    case SNEP_RESPONSE_UNSUPPORTED_VER:
        break;
    case SNEP_RESPONSE_REJECT:
		//
		// SNEP Protocol Version
		//
		pui8DataPtr[ui8offset++] = SNEP_VERSION;

		//
		// Response Field
		//
		pui8DataPtr[ui8offset++] = (uint8_t) SNEP_RESPONSE_REJECT;

		//
		// Length (4 bytes)
		//
		pui8DataPtr[ui8offset++] = 0x00;
		pui8DataPtr[ui8offset++] = 0x00;
		pui8DataPtr[ui8offset++] = 0x00;
		pui8DataPtr[ui8offset++] = 0x00;
		g_eSNEPConnectionStatus = SNEP_CONNECTION_IDLE;
        break;
    }
    return ui8offset;
}

//*****************************************************************************
//
//! SNEP_processReceivedData - Processes the data received from a client/server.
//!
//! \param pui8RxBuffer is the starting pointer of the SNEP request/response 
//! received.
//! \param ui8RxLength is the length of the SNEP request/response received.
//!
//! This function handles the requests/responses received inside an I-PDU 
//! in the LLCP layer. This function must be called inside 
//! LLCP_processReceivedData(). 
//!
//! \return None
//
//*****************************************************************************
void SNEP_processReceivedData(uint8_t * pui8RxBuffer, uint16_t ui16RxLength)
{
    volatile uint8_t ui8SNEPversion;
    tSNEPCommands eCommandField;

#if NFC_SNEP_DEBUG
    uint8_t pui8StringBuffer[5];

    convertWordToAscii(ui16RxLength,(uint8_t *)pui8StringBuffer);

    Serial_printf("SNEP RX ",NFC_SNEP_LAYER_DATA);

	Serial_printBuffer((char *) pui8StringBuffer, 5,NFC_SNEP_LAYER_DATA);

	Serial_printf(" bytes\n",NFC_SNEP_LAYER_DATA);
#endif

    eCommandField = (tSNEPCommands) pui8RxBuffer[1];

	//
	// Handle the incoming fragments received after a PUT request from the 
	// SNEP client.
	//
    if(g_eSNEPConnectionStatus == SNEP_CONNECTION_RECEIVED_FIRST_PACKET
    		|| g_eSNEPConnectionStatus == SNEP_CONNECTION_RECEIVING_N_FRAGMENTS)
    {
        if(g_ui32SNEPRemainingRxPayloadBytes > ui16RxLength)
        {
            g_ui16SNEPReceivedBytes = ui16RxLength;
            g_eSNEPConnectionStatus = SNEP_CONNECTION_RECEIVING_N_FRAGMENTS;
            g_eRxPacketStatus = RECEIVED_N_FRAGMENT;
        }
        else
        {
            g_ui16SNEPReceivedBytes = ui16RxLength;
            g_eSNEPConnectionStatus = SNEP_CONNECTION_RECEIVE_COMPLETE;
            g_eRxPacketStatus = RECEIVED_FRAGMENT_COMPLETED;
        }
        g_ui32SNEPRemainingRxPayloadBytes = g_ui32SNEPRemainingRxPayloadBytes \
											- (uint32_t) g_ui16SNEPReceivedBytes;
        g_pui8SNEPRxPacketPtr = &pui8RxBuffer[0];
    }
    else if(eCommandField >= 0x80)
    {
        //
        // Process Responses
        //
        switch(eCommandField)
        {
        case SNEP_RESPONSE_CONTINUE:
#if NFC_SNEP_DEBUG
        	Serial_printf("SNEP RX Continue\n",NFC_SNEP_LAYER_DATA);
#endif
            if(g_eSNEPConnectionStatus == SNEP_CONNECTION_WAITING_FOR_CONTINUE)
            {
                g_eSNEPConnectionStatus = SNEP_CONNECTION_SENDING_N_FRAGMENTS;
            }
			break;
        case SNEP_RESPONSE_SUCCESS:
#if NFC_SNEP_DEBUG
        	Serial_printf("SNEP RX Success\n",NFC_SNEP_LAYER_DATA);
#endif
            if(g_eSNEPConnectionStatus == SNEP_CONNECTION_WAITING_FOR_SUCCESS)
            {
                g_eSNEPConnectionStatus = SNEP_CONNECTION_SEND_COMPLETE;
            }
            break;
        case SNEP_RESPONSE_NOT_FOUND:
            break;
        case SNEP_RESPONSE_EXCESS_DATA:
            break;
        case SNEP_RESPONSE_BAD_REQUEST:
            break;
        case SNEP_RESPONSE_NOT_IMPLEMENTED:
            break;
        case SNEP_RESPONSE_UNSUPPORTED_VER:
            break;
        case SNEP_RESPONSE_REJECT:
            break;
        default :
            break;
        }
    }
    else
    {
            //
            // Process Requests
            //
            switch(eCommandField)
            {
            case SNEP_REQUEST_CONTINUE:
                break;
            case SNEP_REQUEST_GET:
                break;
            case SNEP_REQUEST_PUT:
                ui8SNEPversion = pui8RxBuffer[0];
                if(ui8SNEPversion == SNEP_VERSION)
                {
				    //
					// Update remaining payload bytes
					//
                    g_ui32SNEPRemainingRxPayloadBytes =  (uint32_t) (pui8RxBuffer[5] & 0xFF)        + \
                                                       ((uint32_t) (pui8RxBuffer[4] & 0xFF) << 8)  + \
                                                       ((uint32_t) (pui8RxBuffer[3] & 0xFF) << 16) + \
                                                       ((uint32_t) (pui8RxBuffer[2] & 0xFF) << 24);

                    g_ui32SNEPPacketLength = g_ui32SNEPRemainingRxPayloadBytes;
													   
													   
                  if(g_ui32SNEPRemainingRxPayloadBytes > SNEP_MAX_PAYLOAD)
                  {
                      g_eSNEPConnectionStatus = SNEP_CONNECTION_EXCESS_SIZE;
                  }
                  else
                  {
                        if (g_ui32SNEPRemainingRxPayloadBytes
                                > (ui16RxLength - 6)) {
                            g_ui16SNEPReceivedBytes = (ui16RxLength - 6);
                            g_eSNEPConnectionStatus =
                                    SNEP_CONNECTION_RECEIVED_FIRST_PACKET;
                            g_eRxPacketStatus = RECEIVED_FIRST_FRAGMENT;
                        } else {
                            //
                            // Packet Length
                            //
                            g_ui16SNEPReceivedBytes = (ui16RxLength - 6);
                            g_eSNEPConnectionStatus =
                                    SNEP_CONNECTION_RECEIVE_COMPLETE;
                            g_eRxPacketStatus = RECEIVED_FRAGMENT_COMPLETED;
                        }
                        //
                        // Update remaining payload bytes
                        //
                        g_ui32SNEPRemainingRxPayloadBytes =
                                g_ui32SNEPRemainingRxPayloadBytes
                                        - (uint32_t) g_ui16SNEPReceivedBytes;

                        //
                        // Set the g_pui8SNEPRxPacketPtr to the start of payload
                        //
                        g_pui8SNEPRxPacketPtr = &pui8RxBuffer[6];
                  }
                }
                else
                    g_eSNEPConnectionStatus = SNEP_WRONG_VERSION_RECEIVED;
                break;
            case SNEP_REQUEST_REJECT:
                break;
            default :
                break;
        }
    }
	return;
}

//*****************************************************************************
//
//! SNEP_getReceiveStatus - Get RxStatus flag, Clear packet status flag,
//!      retrieve length of data, retrieve data
//!
//! \param peReceiveFlag is a pointer to store the rx state status.
//! \param pui8length is a pointer to store the number of received bytes.
//! \param pui8DataPtr is a double pointer to store the pointer of data
//! received.
//!
//! The \e peReceiveFlag parameter can be any of the following:
//!
//! - \b RECEIVED_NO_FRAGMENT        - No Fragment has been received
//! - \b RECEIVED_FIRST_FRAGMENT     - First fragment has been received.
//! - \b RECEIVED_N_FRAGMENT         - N Fragment has been received. 
//! - \b RECEIVED_FRAGMENT_COMPLETED - End of the fragment has been received. 
//!
//! This function must be called in the main application after the 
//! NFC_P2P_stateMachine() is called to ensure the data received is moved
//! to another buffer / handled when a fragment is received.
//!
//! \return None
//
//*****************************************************************************
void SNEP_getReceiveStatus(tPacketStatus * peReceiveFlag, uint16_t * pui16length,
     uint8_t ** pui8DataPtr, uint32_t * ui32NdefTotalLength)
{
    //
    // Save RX Packet Status Flag
    //
    *peReceiveFlag = g_eRxPacketStatus;

    //
    // Clear the packet status flag
    //
    g_eRxPacketStatus = RECEIVED_NO_FRAGMENT;

    //
    // Save Number of Byted received
    //
    *pui16length = g_ui16SNEPReceivedBytes;

    //
    // Set Data = ReceivedPacket
    //
    *pui8DataPtr = g_pui8SNEPRxPacketPtr;

    //
    // Set Total Length
    //
    *ui32NdefTotalLength = g_ui32SNEPPacketLength;

    return;
}

//*****************************************************************************
//
//! SNEP_getProtocolStatus - returns current SNEP Connection Status enumeration
//!
//! This function returns the current SNEP status flag. It must be called inside
//! LLCP_processReceivedData() to decide if further I-PDUs are required - this 
//! occurs when there are requests/responses queued. 
//!
//! \return g_eSNEPConnectionStatus the current connection status flag. 
//
//*****************************************************************************
tSNEPConnectionStatus SNEP_getProtocolStatus(void)
{
    return g_eSNEPConnectionStatus;
}

//*****************************************************************************
//
//! SNEP_setProtocolStatus - sets current SNEP Connection Status enumeration
//!
//! \param eProtocolStatus is the status flag used by the SNEP_stateMachine()
//! to send request/response. Only when it is set to SNEP_CONNECTION_IDLE,
//! new send transactions are allowed. 
//!
//! The \e eProtocolStatus parameter can be any of the following:
//!
//! - \b SNEP_CONNECTION_IDLE                   - No ongoing tx/rx
//! - \b SNEP_WRONG_VERSION_RECEIVED            - Wrong Version Received
//! - \b SNEP_CONNECTION_RECEIVED_FIRST_PACKET  - Received First Fragment
//! - \b SNEP_CONNECTION_RECEIVING_N_FRAGMENTS  - Received N Fragment
//! - \b SNEP_CONNECTION_WAITING_FOR_CONTINUE   - Waiting for Continue Rsp.
//! - \b SNEP_CONNECTION_WAITING_FOR_SUCCESS    - Waiting for Success Rsp.
//! - \b SNEP_CONNECTION_SENDING_N_FRAGMENTS    - Sending N Fragment
//! - \b SNEP_CONNECTION_SEND_COMPLETE          - Send Completed
//! - \b SNEP_CONNECTION_RECEIVE_COMPLETE       - Receive Completed
//! - \b SNEP_CONNECTION_EXCESS_SIZE            - Received Excess Size Req. 
//!
//! This function is called inside LLCP_processReceivedData(), to set the 
//! g_eSNEPConnectionStatus flag to SNEP_CONNECTION_IDLE after the 
//! a send transaction is completed to allow for further send transactions.
//!
//! \return None
//
//*****************************************************************************
void SNEP_setProtocolStatus(tSNEPConnectionStatus eProtocolStatus)
{
    g_eSNEPConnectionStatus = eProtocolStatus;
    return;
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
