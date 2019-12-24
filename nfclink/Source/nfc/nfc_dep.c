//*****************************************************************************
//
// nfc_dep.c - used to send packets of P2P
//
// Copyright (c) 2015 Texas Instruments Incorporated.  All rights reserved.
// TI Information - Selective Disclosure
//
//*****************************************************************************

#include "nfc_dep.h"

//*****************************************************************************
//
//! \addtogroup nfc_dep_api NFC DEP API Functions
//! @{
//!
//! NFC Data Exchange Protocol Statemachine based on the NFC Forum Digital
//! Protocol ver 1.1.
//
//*****************************************************************************


//*****************************************************************************
//
// Stores the target 10 byte CID.
//
//*****************************************************************************
static uint8_t g_pui8NFCID3t[10];

//*****************************************************************************
//
// Stores the initiator 8 byte CID.
//
//*****************************************************************************
static uint8_t g_pui8NFCID2i[8];

//*****************************************************************************
//
// Stores the initiator DID byte CID set by the user.
//
//*****************************************************************************
static uint8_t g_ui8NfcDepDIDi;

//*****************************************************************************
//
// Stores the packet PNI.
//
//*****************************************************************************
static uint8_t g_ui8NfcDepPni;

//*****************************************************************************
//
// Stores the RTOX byte used for time extension.
//
//*****************************************************************************
static uint8_t g_ui8RtoxTransportData;

//*****************************************************************************
//
// Stores the initiator DID byte CID.
//
//*****************************************************************************
static uint8_t g_ui8NfcDepDID;

//*****************************************************************************
//
// Stores the next NFC DEP PDU type.
//
//*****************************************************************************
static tPDUBlock g_eNextPduType;

//*****************************************************************************
//
// Stores the temp PDU holder used during timeout extension.
//
//*****************************************************************************
static tPDUBlock g_eTimeoutPduType;

//*****************************************************************************
//
// Stores the current NFC DEP state.
//
//*****************************************************************************
static tDepState g_eCurrentDepState;

//*****************************************************************************
//
// Stores the DEP loopback (used for certification).
//
//*****************************************************************************
static bool g_bNfcDEPLoopback;

//*****************************************************************************
//
// Stores whether LLCP is enabled or not.
//
//*****************************************************************************
static bool g_bLLCPEnabled;

//*****************************************************************************
//
// Stores whether DID should be used or not.
//
//*****************************************************************************
static bool g_bNfcDepDIDEnabled;

//*****************************************************************************
//
// Stores whether an Attention PDU was received or not.
//
//*****************************************************************************
static bool g_bATNReceived;

//*****************************************************************************
//
// Stores whether the FW is in certification mode or not
//
//*****************************************************************************
static bool g_bNfcDepTestEnable;

//*****************************************************************************
//
// Stores whether SOT was transmitted in certification mode.
//
//*****************************************************************************
static bool g_bSendSOT;

//*****************************************************************************
//
// Stores whether a RTOX PDU was received or not.
//
//*****************************************************************************
static bool g_bRTOXTimeout;

//*****************************************************************************
//
// Stores the size of the maximum data payload.
//
//*****************************************************************************
static uint16_t g_ui16NFCDEPMaxPayload;

//*****************************************************************************
//
// Stores the number of NFC DEP protocol errors
//
//*****************************************************************************
static uint8_t g_ui8NfcDepProtocolErrors;

//*****************************************************************************
//
// Stores the number of max NFC DEP protocol errors (defined by user)
//
//*****************************************************************************
static uint8_t g_ui8NfcDepMaxProtocolErrors;

//*****************************************************************************
//
// Stores the number of NFC DEP timeouts
//
//*****************************************************************************
static uint8_t g_ui8NfcDepInitTimeoutCount;

//*****************************************************************************
//
// Stores the number of max NFC DEP timeouts (defined by user)
//
//*****************************************************************************
static uint8_t g_ui8NfcDepInitiatorMaxTimeouts;

//*****************************************************************************
//
// Stores TX and RX NFC DEP buffers
//
//*****************************************************************************
static struct
{
	uint8_t pui8DEPRXBuffer[LLCP_MIU+7];
	uint8_t pui8DEPBuffer[LLCP_MIU+7];
	uint8_t ui8DEPLength;
}g_sDEPPacket;

//*****************************************************************************
//
// Stores the index for the incoming commands
//
//*****************************************************************************
static uint16_t g_ui16Index;

//*****************************************************************************
//
// Stores the length for the incoming commands
//
//*****************************************************************************
static uint16_t g_ui16PacketLength;

//*****************************************************************************
//
// Stores the length of the outgoing command
//
//*****************************************************************************
static uint16_t g_ui16TXIndex;

//*****************************************************************************
//
// Stores the length of the remaining bytes to be transmitted
//
//*****************************************************************************
static uint16_t g_ui16TXRemaining;

//*****************************************************************************
//
// Stores the length of the requested timeout
//
//*****************************************************************************
static uint16_t g_ui16NfcDepTimeOut;

//*****************************************************************************
//
// Stores the length of the timeout the upper layer should wait for response
//
//*****************************************************************************
static uint16_t g_ui16NfcDepRTOXTimeOut;

//*****************************************************************************
//
// Stores the length of the I-PDU packet for the case when the packet needs to
// be re-sent.
//
//*****************************************************************************
static uint8_t g_ui8PrevPDULen;

//*****************************************************************************
//
//! NFCDEP_configure_P2P - Configures the NFC DEP layer for certifcation purposes.
//!
//! \param sP2PSetupOptions holds the configuration options required for certifcation.
//! Including Loopback and enable/disable the LLCP layer.
//! \param bTestEnable boolean to enable/disable certfication testing
//! \param ui8DIDi the DID for initiator.
//!
//! This function must be called in the NFC Configuration function.
//!
//! \return None.
//
//*****************************************************************************
void NFCDEP_configure_P2P(t_sNfcDEP_P2PSetup sP2PSetupOptions, bool bTestEnable, uint8_t ui8DIDi)
{
	g_bNfcDEPLoopback = sP2PSetupOptions.bits.bP2PSupportLoopback;
	g_bLLCPEnabled = sP2PSetupOptions.bits.bP2PSupportLLCP;

	g_ui8NfcDepInitiatorMaxTimeouts = sP2PSetupOptions.bits.ui3P2PMaxTimeouts;
	g_ui8NfcDepMaxProtocolErrors = sP2PSetupOptions.bits.ui3P2PMaxProtocolErrors;

	g_bNfcDepTestEnable = bTestEnable;

	g_ui8NfcDepDIDi = ui8DIDi;
}

//*****************************************************************************
//
//! NFCDEP_init - Initializes the variables for the NFC DEP layer.
//!
//! This function must be called prior to any other NFC DEP function in
//! in order to properly initialize all variables which have been declared.
//! Failure to do so may cause the software to enter unknown states. It will
//! initialize the LLCP layer.
//!
//! \return None.
//
//*****************************************************************************
void NFCDEP_init(void)
{
	g_ui16Index = 0;
	g_ui16PacketLength = 0;
	g_bNfcDepDIDEnabled = false;
	g_bATNReceived = false;
	g_bSendSOT = true;
	g_bRTOXTimeout = false;
	g_ui16NFCDEPMaxPayload = LLCP_MIU+6;
	// Reset g_pui8NFCID3t
	g_pui8NFCID3t[0] = 0x01;
	g_pui8NFCID3t[1] = 0xFE;
	g_pui8NFCID3t[2] = 0x88;
	g_pui8NFCID3t[3] = 0x77;
	g_pui8NFCID3t[4] = 0x66;
	g_pui8NFCID3t[5] = 0x55;
	g_pui8NFCID3t[6] = 0x44;
	g_pui8NFCID3t[7] = 0x33;
	g_pui8NFCID3t[8] = 0x00;
	g_pui8NFCID3t[9] = 0x00;

	// Reset g_pui8NFCID2i
	g_pui8NFCID2i[0] = 0x01;
	g_pui8NFCID2i[1] = 0xFE;
	g_pui8NFCID2i[2] = 0x03;
	g_pui8NFCID2i[3] = 0x04;
	g_pui8NFCID2i[4] = 0x05;
	g_pui8NFCID2i[5] = 0x06;
	g_pui8NFCID2i[6] = 0x07;
	g_pui8NFCID2i[7] = 0x08;

	// Initialize Current State
	g_eCurrentDepState = DEP_ATR_REQ_STATE;

	g_eNextPduType = INFORMATION_PDU;

	g_eTimeoutPduType = INFORMATION_PDU;

	// Reset the PNI
	g_ui8NfcDepPni = 0x00;

	g_ui8NfcDepDID  = 0;

	g_ui8NfcDepInitTimeoutCount = 0;

	g_ui16NfcDepTimeOut = 0;

	g_ui16NfcDepRTOXTimeOut = 0;

	g_ui16TXIndex = 0;

	g_ui16TXRemaining = 0;

	g_ui8NfcDepProtocolErrors = 0;

	g_ui8PrevPDULen = 0;

	if(g_bLLCPEnabled)
	{
		LLCP_init();
	}
}

//*****************************************************************************
//
//! NFCDEP_sendATR_REQ - Sends ATR Request.
//!
//! This function sends ATR Request.
//!
//! \return None.
//
//*****************************************************************************
void NFCDEP_sendATR_REQ(void)
{
    uint8_t ui8Counter = 0;
    uint8_t ui8Offset = 1;

	//
	// Command
	//
	g_sDEPPacket.pui8DEPBuffer[ui8Offset++] = (uint8_t) ((ATR_REQ_CMD & 0xFF00) >> 8);
	g_sDEPPacket.pui8DEPBuffer[ui8Offset++] = (uint8_t) (ATR_REQ_CMD & 0x00FF);

	//
	// NFCID3i
	//
	for(ui8Counter=0;ui8Counter<8;ui8Counter++)
	{
		g_sDEPPacket.pui8DEPBuffer[ui8Offset++] = g_pui8NFCID2i[ui8Counter];
	}
	g_sDEPPacket.pui8DEPBuffer[ui8Offset++] = 0x00;
	g_sDEPPacket.pui8DEPBuffer[ui8Offset++] = 0x00;

	if (g_ui8NfcDepDIDi > 0)
	{
		g_bNfcDepDIDEnabled = true;
		g_ui8NfcDepDID = g_ui8NfcDepDIDi;
	}

	g_sDEPPacket.pui8DEPBuffer[ui8Offset++] = g_ui8NfcDepDIDi;
	g_sDEPPacket.pui8DEPBuffer[ui8Offset++] = BSi;
	g_sDEPPacket.pui8DEPBuffer[ui8Offset++] = BRi;

	if (g_bLLCPEnabled)
	{
		g_sDEPPacket.pui8DEPBuffer[ui8Offset++] = (PPi | 0x02);		// Max Payload 64 bytes
		//
		// LLCP Magic Number
		//
		g_sDEPPacket.pui8DEPBuffer[ui8Offset++] = LLCP_MAGIC_NUMBER_HIGH;
		g_sDEPPacket.pui8DEPBuffer[ui8Offset++] = LLCP_MAGIC_NUMBER_MIDDLE;
		g_sDEPPacket.pui8DEPBuffer[ui8Offset++] = LLCP_MAGIC_NUMBER_LOW;

		ui8Offset = ui8Offset + LLCP_addTLV(LLCP_VERSION,&g_sDEPPacket.pui8DEPBuffer[ui8Offset]);
		ui8Offset = ui8Offset + LLCP_addTLV(LLCP_MIUX,&g_sDEPPacket.pui8DEPBuffer[ui8Offset]);
		ui8Offset = ui8Offset + LLCP_addTLV(LLCP_WKS,&g_sDEPPacket.pui8DEPBuffer[ui8Offset]);
		ui8Offset = ui8Offset + LLCP_addTLV(LLCP_LTO,&g_sDEPPacket.pui8DEPBuffer[ui8Offset]);
		ui8Offset = ui8Offset + LLCP_addTLV(LLCP_OPT,&g_sDEPPacket.pui8DEPBuffer[ui8Offset]);
	}
	else
	{
		g_sDEPPacket.pui8DEPBuffer[ui8Offset++] = PPi;		// Max Payload 64 bytes
	}
	//
	// Length
	//
	g_sDEPPacket.pui8DEPBuffer[0] = ui8Offset;

	g_sDEPPacket.ui8DEPLength = ui8Offset;

	TRF79x0_writeFIFO(g_sDEPPacket.pui8DEPBuffer,CRC_BIT_ENABLE,g_sDEPPacket.ui8DEPLength,0);
}

//*****************************************************************************
//
//! NFCDEP_sendDEP_REQ - Sends DEP Request.
//!
//! This function sends DEP Request.
//!
//! \return None.
//
//*****************************************************************************
void NFCDEP_sendDEP_REQ(void)
{
    uint8_t ui8Offset = 1;
	uint16_t ui16Counter;
	uint16_t ui16TempLength;
	uint32_t ui32TempRTOXTimeout = 0;

	g_sDEPPacket.ui8DEPLength = 0;

	//
	// Command
	//
	g_sDEPPacket.pui8DEPBuffer[ui8Offset++] = (uint8_t) ((DEP_REQ_CMD & 0xFF00) >> 8);
	g_sDEPPacket.pui8DEPBuffer[ui8Offset++] = (uint8_t) (DEP_REQ_CMD & 0x00FF);

	if (g_ui8NfcDepInitTimeoutCount != 0) // If Timeout occurred
	{
		// For the first timeout, save current PDU type for re-sending later if ATN is responded to
		if (g_ui8NfcDepInitTimeoutCount == 1)
		{
			g_eTimeoutPduType = g_eNextPduType;
		}
		// Send ATN PDU instead of current PDU
		g_eNextPduType = ATN_PDU;
	}

	if(g_ui8NfcDepProtocolErrors > 0)
	{
		g_eNextPduType = NACK_PDU;
	}

	if(g_eNextPduType == INFORMATION_PDU)
	{
    	if(g_bNfcDepDIDEnabled)
    	{
    		g_sDEPPacket.pui8DEPBuffer[ui8Offset++] = NFC_DEP_DID_ENABLE | g_eNextPduType | ((g_ui8NfcDepPni++) & 0x03);
    		g_sDEPPacket.pui8DEPBuffer[ui8Offset++] = g_ui8NfcDepDID;
    	}
    	else
    	{
    		g_sDEPPacket.pui8DEPBuffer[ui8Offset++] =  g_eNextPduType | ((g_ui8NfcDepPni++) & 0x03);
    	}

    	if(g_bNfcDepTestEnable)
    	{
    		if (g_bSendSOT)
    		{
    			ui8Offset = NFCDEP_sendSOT(ui8Offset);
    		}
    		else
    		{
    	    	if(g_bNfcDEPLoopback)
    	    	{
    	    		// Checking that current packet fits on the specified length provided by the Target and we are not currently chaining
    	    		if((g_ui16PacketLength > (g_ui16NFCDEPMaxPayload-ui8Offset+1))
    	    				&& (g_ui16TXRemaining == 0))
    	    		{
    	    			if(g_bNfcDepDIDEnabled)
    	    			{
    	    				g_sDEPPacket.pui8DEPBuffer[ui8Offset-2] |= NFC_DEP_CHAIN_ENABLE;
    	    			}
    	    			else
    	    			{
    	    				g_sDEPPacket.pui8DEPBuffer[ui8Offset-1] |= NFC_DEP_CHAIN_ENABLE;
    	    			}

    	    			// Remaining number of bytes is equal to the packet length
    	    			g_ui16TXRemaining = g_ui16PacketLength;
    	    			// The payload doesn't include the length byte (which is taken into account by ui8Offset)
    	    			ui16TempLength = g_ui16NFCDEPMaxPayload - ui8Offset + 1;
    	    			for(ui16Counter = 0; ui16Counter < ui16TempLength; ui16Counter++)
    	    			{
    	    				// Increment TX Index
    	    				g_sDEPPacket.pui8DEPBuffer[ui8Offset++] = g_sDEPPacket.pui8DEPRXBuffer[g_ui16TXIndex++];
    	    				// Decrement remaining bytes
    	    				g_ui16TXRemaining--;
    	    			}
    	    		}
    	    		// We are chaining
    	    		else if(g_ui16TXRemaining > 0)
    				{
    	    			if(g_ui16TXRemaining > (g_ui16NFCDEPMaxPayload-ui8Offset+1))
    	    			{
    	    				// We are still chaining
    	    				// The payload doesn't include the length byte (which is taken into account by ui8Offset)
    	    				ui16TempLength = g_ui16NFCDEPMaxPayload-ui8Offset + 1;
    	        			if(g_bNfcDepDIDEnabled)
    	        			{
    	        				g_sDEPPacket.pui8DEPBuffer[ui8Offset-2] |= NFC_DEP_CHAIN_ENABLE;
    	        			}
    	        			else
    	        			{
    	        				g_sDEPPacket.pui8DEPBuffer[ui8Offset-1] |= NFC_DEP_CHAIN_ENABLE;
    	        			}
    	    			}
    	    			else
    	    			{
    	    				ui16TempLength = g_ui16TXRemaining;
    	    			}

    	    			for(ui16Counter = 0; ui16Counter < ui16TempLength; ui16Counter++)
    	    			{
    	    				// Increment TX Index
    	    				g_sDEPPacket.pui8DEPBuffer[ui8Offset++] = g_sDEPPacket.pui8DEPRXBuffer[g_ui16TXIndex++];
    	    				// Decrement remaining bytes
    	    				g_ui16TXRemaining--;
    	    			}
    	    			if (g_ui16TXRemaining == 0)
    	    			{
    	    				g_ui16TXIndex = 0;
    	    			}
    				}
    	    		else
    	    		{
    	    			for(ui16Counter = 0; ui16Counter < g_ui16PacketLength; ui16Counter++)
    	    			{
    	    				g_sDEPPacket.pui8DEPBuffer[ui8Offset++] = g_sDEPPacket.pui8DEPRXBuffer[ui16Counter];
    	    			}
    	    		}
    	    	}
    	    	else if(g_bLLCPEnabled)
    	    	{
    	            //
    	            // Total = 1 byte Length + 1 SOF (Type A) + 2 bytes Command + 1 byte PFB + n PDU
    	            //
    	    		ui8Offset = ui8Offset + LLCP_stateMachine(&g_sDEPPacket.pui8DEPBuffer[ui8Offset]);
    	    	}
    	    	else
    	    	{
    	    		ui8Offset = 0;
    	    	}
    		}
    	}
    	else if(g_bATNReceived == true)
    	{
    		// Clear ATN Received Flag
    		g_bATNReceived = false;

    		// Set the offset to previous packet length
    		ui8Offset = g_ui8PrevPDULen;
    	}
    	else
    	{
    		if(g_bLLCPEnabled)
    		{
    			//
    			// Total = 1 byte Length + 1 SOF (Type A) + 2 bytes Command + 1 byte PFB + n PDU
    			//
    			ui8Offset = ui8Offset + LLCP_stateMachine(&g_sDEPPacket.pui8DEPBuffer[ui8Offset]);
    		}
    		else
    		{
    			// Do Nothing
    		}
    	}

    	// Save the I-PDU Previous Length
    	g_ui8PrevPDULen = ui8Offset;
	}
	else if(g_eNextPduType == RTOX_REQ_PDU)
	{
		//
		// PFB Byte
		//
		g_sDEPPacket.pui8DEPBuffer[ui8Offset++] = (g_eNextPduType);
		g_sDEPPacket.pui8DEPBuffer[ui8Offset++] = g_ui8RtoxTransportData;
		g_bRTOXTimeout = true;
	    // Calculate and set the correct timeout value: RWTint + Delta RWT (RWTint = RWT*RTOX)
		// Also have to ensure that this value doesn't exceed RWTmax = (256*16/fc) * 2^WTmax
		ui32TempRTOXTimeout = (uint32_t) (g_ui16NfcDepTimeOut* (uint16_t) g_ui8RtoxTransportData);

		if (ui32TempRTOXTimeout <= RWT_INT_MAX_MS)
		{
			g_ui16NfcDepRTOXTimeOut = (uint16_t) ui32TempRTOXTimeout;
		}
		else
		{
			g_ui16NfcDepRTOXTimeOut = RWT_INT_MAX_MS;
		}
	}
	else if(g_eNextPduType == ACK_PDU)
	{
		//
		// PFB Byte
		//
		g_sDEPPacket.pui8DEPBuffer[ui8Offset++] = g_eNextPduType | ((g_ui8NfcDepPni++) & 0x03);
	}
	else if(g_eNextPduType == NACK_PDU)
	{
		// Decrement PNI
		if(g_ui8NfcDepPni == 0x00)
		{
			g_ui8NfcDepPni = 0x03;
		}
		else
		{
			g_ui8NfcDepPni = g_ui8NfcDepPni - 1;
		}
		//
		// PFB Byte
		//
		g_sDEPPacket.pui8DEPBuffer[ui8Offset++] = g_eNextPduType | ((g_ui8NfcDepPni++) & 0x03);
	}
	else if(g_eNextPduType == ATN_PDU)
	{
		g_sDEPPacket.pui8DEPBuffer[ui8Offset++] = g_eNextPduType;
	}

	//
	// Length
	//
	g_sDEPPacket.pui8DEPBuffer[0] = ui8Offset;

	g_sDEPPacket.ui8DEPLength = ui8Offset;

	TRF79x0_writeFIFO(g_sDEPPacket.pui8DEPBuffer,CRC_BIT_ENABLE,g_sDEPPacket.ui8DEPLength,0);
}

//*****************************************************************************
//
//! NFCDEP_sendPSL_REQ - Sends PSL Request.
//!
//! This function sends PSL Request.
//!
//! \return None.
//
//*****************************************************************************
void NFCDEP_sendPSL_REQ(void)
{
    t_sNfcP2PCommBitrate sBitrates;
    t_sNfcP2PCommBitrate sCurrentBitrate;
    uint8_t ui8Offset = 1;

	//
	// Command
	//
	g_sDEPPacket.pui8DEPBuffer[ui8Offset++] = (uint8_t) ((PSL_REQ_CMD & 0xFF00) >> 8);
	g_sDEPPacket.pui8DEPBuffer[ui8Offset++] = (uint8_t) (PSL_REQ_CMD & 0x00FF);

	//
	// DID
	//
	g_sDEPPacket.pui8DEPBuffer[ui8Offset++] = g_ui8NfcDepDID;

	//
	// BRS -
	// B5 B4 B3 (DSI) Initiator to Target
	// B2 B1 B0 (DRI) Target to Initiator
	// 0  0  0  106kbaud
	// 0  0  1  212kbaud
	// 0  1  0  424kbaud (default)
	// 0  1  1  848kbaud
	//
    sCurrentBitrate = NFC_P2P_getInitiatorBitrate();
    sBitrates = NFC_P2P_getInitiatorSupportedBitrates();

    // Check if current passive bitrate is 212kbps and passive 424kbps is supported
    if(sCurrentBitrate.bits.bPassive212kbps == 1 &&
    	sBitrates.bits.bPassive424kbps == 1)
    {
    	g_sDEPPacket.pui8DEPBuffer[ui8Offset++] = 0x12;
    }
    // Check if current active bitrate is 212kbps and active 424kbps is supported
    else if(sCurrentBitrate.bits.bActive212kbps == 1 &&
        	sBitrates.bits.bActive424kbps == 1)
    {
    	g_sDEPPacket.pui8DEPBuffer[ui8Offset++] = 0x12;
    }
    // Check if current passive bitrate is 106kbps and passive 424kbps is supported
    else if(sCurrentBitrate.bits.bPassive106kbps == 1 &&
        	sBitrates.bits.bPassive424kbps == 1)
    {
    	g_sDEPPacket.pui8DEPBuffer[ui8Offset++] = 0x12;
    }
    // Check if current passive bitrate is 106kbps and passive 212kbps is supported
    else if(sCurrentBitrate.bits.bPassive106kbps == 1 &&
        	sBitrates.bits.bPassive212kbps == 1)
    {
    	g_sDEPPacket.pui8DEPBuffer[ui8Offset++] = 0x09;
    }
    // Check if current active bitrate is 106kbps and active 424kbps is supported
    else if(sCurrentBitrate.bits.bActive106kbps == 1 &&
        	sBitrates.bits.bActive424kbps == 1)
    {
    	g_sDEPPacket.pui8DEPBuffer[ui8Offset++] = 0x12;
    }
    // Check if current active bitrate is 106kbps and active 212kbps is supported
    else if(sCurrentBitrate.bits.bActive106kbps == 1 &&
        	sBitrates.bits.bActive212kbps == 1)
    {
    	g_sDEPPacket.pui8DEPBuffer[ui8Offset++] = 0x09;
    }

	//
	// FSL
	// B1-B0 Max Payload Size (11b: Max payload size is 254 bytes)
	//
    if(g_ui16NFCDEPMaxPayload == 64)
    {
    	g_sDEPPacket.pui8DEPBuffer[ui8Offset++] = 0x00;
    }
    else if(g_ui16NFCDEPMaxPayload == 128)
    {
    	g_sDEPPacket.pui8DEPBuffer[ui8Offset++] = 0x01;
    }
    else if(g_ui16NFCDEPMaxPayload == 192)
    {
    	g_sDEPPacket.pui8DEPBuffer[ui8Offset++] = 0x02;
    }
    else if(g_ui16NFCDEPMaxPayload == 254)
    {
    	g_sDEPPacket.pui8DEPBuffer[ui8Offset++] = 0x03;
    }

	//
	// Length
	//
	g_sDEPPacket.pui8DEPBuffer[0] = ui8Offset;

	g_sDEPPacket.ui8DEPLength = ui8Offset;

	TRF79x0_writeFIFO(g_sDEPPacket.pui8DEPBuffer,CRC_BIT_ENABLE,g_sDEPPacket.ui8DEPLength,0);
}

//*****************************************************************************
//
//! NFCDEP_sendRSL_REQ - Sends RSL Request.
//!
//! This function sends RSL Request.
//!
//! \return None.
//
//*****************************************************************************
void NFCDEP_sendRSL_REQ(void)
{
    uint8_t ui8Offset = 1;

	//
	// Command
	//
	g_sDEPPacket.pui8DEPBuffer[ui8Offset++] = (uint8_t) ((RSL_REQ_CMD & 0xFF00) >> 8);
	g_sDEPPacket.pui8DEPBuffer[ui8Offset++] = (uint8_t) (RSL_REQ_CMD & 0x00FF);

	//
	// DID
	//
	if(g_bNfcDepDIDEnabled)
	{
		g_sDEPPacket.pui8DEPBuffer[ui8Offset++] = g_ui8NfcDepDID;
	}
	//
	// Length
	//

	g_sDEPPacket.pui8DEPBuffer[0] = ui8Offset;

	g_sDEPPacket.ui8DEPLength = ui8Offset;

	TRF79x0_writeFIFO(g_sDEPPacket.pui8DEPBuffer,CRC_BIT_ENABLE,g_sDEPPacket.ui8DEPLength,0);
}

//*****************************************************************************
//
//! NFCDEP_sendDSL_REQ - Sends DSL Request.
//!
//! This function sends DSL Request.
//!
//! \return None.
//
//*****************************************************************************
void NFCDEP_sendDSL_REQ(void)
{
    uint8_t ui8Offset = 1;

	//
	// Command
	//
	g_sDEPPacket.pui8DEPBuffer[ui8Offset++] = (uint8_t) ((DSL_REQ_CMD & 0xFF00) >> 8);
	g_sDEPPacket.pui8DEPBuffer[ui8Offset++] = (uint8_t) (DSL_REQ_CMD & 0x00FF);

	if(g_bNfcDepDIDEnabled)
	{
		g_sDEPPacket.pui8DEPBuffer[ui8Offset++] = g_ui8NfcDepDID;
	}

	//
	// Length
	//

	g_sDEPPacket.pui8DEPBuffer[0] = ui8Offset;

	g_sDEPPacket.ui8DEPLength = ui8Offset;

	TRF79x0_writeFIFO(g_sDEPPacket.pui8DEPBuffer,CRC_BIT_ENABLE,g_sDEPPacket.ui8DEPLength,0);
}

//*****************************************************************************
//
//! NFCDEP_sendATR_RES - Sends ATR Response.
//!
//! This function sends ATR Response.
//!
//! \return STATUS_SUCCESS(0x01) if packet is sent succesfully else
//! STATUS_FAIL(0x00).
//
//*****************************************************************************
uint8_t NFCDEP_sendATR_RES(void)
{
    uint8_t ui8Counter = 0;
    uint8_t ui8Offset = 1;

	//
	// Command
	//
	g_sDEPPacket.pui8DEPBuffer[ui8Offset++] = (uint8_t) ((ATR_RES_CMD & 0xFF00) >> 8);
	g_sDEPPacket.pui8DEPBuffer[ui8Offset++] = (uint8_t) (ATR_RES_CMD & 0x00FF);

	//
	// NFCID3t
	//
	for(ui8Counter=0;ui8Counter<10;ui8Counter++)
	{
		g_sDEPPacket.pui8DEPBuffer[ui8Offset++] = g_pui8NFCID3t[ui8Counter];
	}

	g_sDEPPacket.pui8DEPBuffer[ui8Offset++] = g_ui8NfcDepDID;
	g_sDEPPacket.pui8DEPBuffer[ui8Offset++] = BSt;
	g_sDEPPacket.pui8DEPBuffer[ui8Offset++] = BRt;
	g_sDEPPacket.pui8DEPBuffer[ui8Offset++] = TO;

	if (g_bLLCPEnabled)
	{
		g_sDEPPacket.pui8DEPBuffer[ui8Offset++] = (PPt | 0x02);		// Max Payload 64 bytes

		//
		// LLCP Magic Number
		//
		g_sDEPPacket.pui8DEPBuffer[ui8Offset++] = LLCP_MAGIC_NUMBER_HIGH;
		g_sDEPPacket.pui8DEPBuffer[ui8Offset++] = LLCP_MAGIC_NUMBER_MIDDLE;
		g_sDEPPacket.pui8DEPBuffer[ui8Offset++] = LLCP_MAGIC_NUMBER_LOW;

		ui8Offset = ui8Offset + LLCP_addTLV(LLCP_VERSION,&g_sDEPPacket.pui8DEPBuffer[ui8Offset]);
		ui8Offset = ui8Offset + LLCP_addTLV(LLCP_MIUX,&g_sDEPPacket.pui8DEPBuffer[ui8Offset]);
		ui8Offset = ui8Offset + LLCP_addTLV(LLCP_WKS,&g_sDEPPacket.pui8DEPBuffer[ui8Offset]);
		ui8Offset = ui8Offset + LLCP_addTLV(LLCP_LTO,&g_sDEPPacket.pui8DEPBuffer[ui8Offset]);
		ui8Offset = ui8Offset + LLCP_addTLV(LLCP_OPT,&g_sDEPPacket.pui8DEPBuffer[ui8Offset]);
	}
	else
	{
		g_sDEPPacket.pui8DEPBuffer[ui8Offset++] = PPt;		// Max Payload 254 bytes
	}

	//
	// Length
	//
	g_sDEPPacket.pui8DEPBuffer[0] = ui8Offset;

	g_sDEPPacket.ui8DEPLength = ui8Offset;

	return TRF79x0_writeFIFO(g_sDEPPacket.pui8DEPBuffer,CRC_BIT_ENABLE,g_sDEPPacket.ui8DEPLength,0);
}

//*****************************************************************************
//
//! NFCDEP_sendDEP_RES - Sends DEP Response.
//!
//! This function sends DEP Response.
//!
//! \return STATUS_SUCCESS(0x01) if packet is sent succesfully else
//! STATUS_FAIL(0x00).
//
//*****************************************************************************
void NFCDEP_sendDEP_RES(void)
{
	uint8_t ui8Offset = 1;
	uint16_t ui16Counter;
	uint16_t ui16TempLength;

	g_sDEPPacket.ui8DEPLength = 0;

	//
	// Command
	//
	g_sDEPPacket.pui8DEPBuffer[ui8Offset++] = (uint8_t) ((DEP_RES_CMD & 0xFF00) >> 8);
	g_sDEPPacket.pui8DEPBuffer[ui8Offset++] = (uint8_t) (DEP_RES_CMD & 0x00FF);

    if(g_eNextPduType == INFORMATION_PDU)
    {

    	if(g_bATNReceived)
    	{
    		if(g_ui8NfcDepPni == 0x00)
    		{
    			g_ui8NfcDepPni = 0x03;
    		}
    		else
    		{
    			g_ui8NfcDepPni = g_ui8NfcDepPni - 1;
    		}

    		// Valid packet received
    		g_bATNReceived = false;
    	}

        //
        // PFB Byte
        //
    	if(g_bNfcDepDIDEnabled)
    	{
    		g_sDEPPacket.pui8DEPBuffer[ui8Offset++] = NFC_DEP_DID_ENABLE | g_eNextPduType | ((g_ui8NfcDepPni++) & 0x03);
    		g_sDEPPacket.pui8DEPBuffer[ui8Offset++] = g_ui8NfcDepDID;
    	}
    	else
    	{
    		g_sDEPPacket.pui8DEPBuffer[ui8Offset++] =  g_eNextPduType | ((g_ui8NfcDepPni++) & 0x03);
    	}

    	if(g_bNfcDEPLoopback)
    	{
    		// Checking that current packet fits on the specified length provided by the Target and we are not currently chaining
    		if((g_ui16PacketLength > (g_ui16NFCDEPMaxPayload-ui8Offset+1))
    				&& (g_ui16TXRemaining == 0))
    		{
    			if(g_bNfcDepDIDEnabled)
    			{
    				g_sDEPPacket.pui8DEPBuffer[ui8Offset-2] |= NFC_DEP_CHAIN_ENABLE;
    			}
    			else
    			{
    				g_sDEPPacket.pui8DEPBuffer[ui8Offset-1] |= NFC_DEP_CHAIN_ENABLE;
    			}

    			// Remaining number of bytes is equal to the packet length
    			g_ui16TXRemaining = g_ui16PacketLength;
    			// The payload doesn't include the length byte (which is taken into account by ui8Offset)
    			ui16TempLength = g_ui16NFCDEPMaxPayload - ui8Offset + 1;
    			for(ui16Counter = 0; ui16Counter < ui16TempLength; ui16Counter++)
    			{
    				// Increment TX Index
    				g_sDEPPacket.pui8DEPBuffer[ui8Offset++] = g_sDEPPacket.pui8DEPRXBuffer[g_ui16TXIndex++];
    				// Decrement remaining bytes
    				g_ui16TXRemaining--;
    			}
    			g_sDEPPacket.ui8DEPLength = ui8Offset;
    		}
    		// We are chaining
    		else if(g_ui16TXRemaining > 0)
			{
    			if(g_ui16TXRemaining > (g_ui16NFCDEPMaxPayload-ui8Offset+1))
    			{
    				// We are still chaining
    				// The payload doesn't include the length byte (which is taken into account by ui8Offset)
    				ui16TempLength = g_ui16NFCDEPMaxPayload-ui8Offset + 1;
        			if(g_bNfcDepDIDEnabled)
        			{
        				g_sDEPPacket.pui8DEPBuffer[ui8Offset-2] |= NFC_DEP_CHAIN_ENABLE;
        			}
        			else
        			{
        				g_sDEPPacket.pui8DEPBuffer[ui8Offset-1] |= NFC_DEP_CHAIN_ENABLE;
        			}
    			}
    			else
    			{
    				ui16TempLength = g_ui16TXRemaining;
    			}

    			for(ui16Counter = 0; ui16Counter < ui16TempLength; ui16Counter++)
    			{
    				// Increment TX Index
    				g_sDEPPacket.pui8DEPBuffer[ui8Offset++] = g_sDEPPacket.pui8DEPRXBuffer[g_ui16TXIndex++];
    				// Decrement remaining bytes
    				g_ui16TXRemaining--;
    			}

    			if (g_ui16TXRemaining == 0)
    			{
    				g_ui16TXIndex = 0;
    			}

    			g_sDEPPacket.ui8DEPLength = ui8Offset;
			}
    		else
    		{
    			for(ui16Counter = 0; ui16Counter < g_ui16PacketLength; ui16Counter++)
    			{
    				g_sDEPPacket.pui8DEPBuffer[ui8Offset++] = g_sDEPPacket.pui8DEPRXBuffer[ui16Counter];
    			}
    			g_sDEPPacket.ui8DEPLength = ui8Offset;
    		}
    	}
    	else if(g_bLLCPEnabled)
    	{
            //
            // Total = 1 byte Length + 1 SOF (Type A) + 2 bytes Command + 1 byte PFB + n PDU
            //
            g_sDEPPacket.ui8DEPLength = ui8Offset + LLCP_stateMachine(&g_sDEPPacket.pui8DEPBuffer[ui8Offset]);
    	}
    	else
    	{
    		g_sDEPPacket.ui8DEPLength = 0;
    	}

    }
    else if(g_eNextPduType == ATN_PDU)
    {
        //
        // PFB Byte
        //
    	g_sDEPPacket.pui8DEPBuffer[ui8Offset++] = (g_eNextPduType);

        g_sDEPPacket.ui8DEPLength = ui8Offset;
    }
    else if(g_eNextPduType == ACK_PDU)
    {
    	if(g_bATNReceived)
    	{
    		if(g_ui8NfcDepPni == 0x00)
    		{
    			g_ui8NfcDepPni = 0x03;
    		}
    		else
    		{
    			g_ui8NfcDepPni = g_ui8NfcDepPni - 1;
    		}

    		// Valid packet received
    		g_bATNReceived = false;
    	}

        //
        // PFB Byte
        //
    	g_sDEPPacket.pui8DEPBuffer[ui8Offset++] =  g_eNextPduType | ((g_ui8NfcDepPni++) & 0x03);

        g_sDEPPacket.ui8DEPLength = ui8Offset;
    }

	//
	// Length
	//
	g_sDEPPacket.pui8DEPBuffer[0] = g_sDEPPacket.ui8DEPLength;

	if(g_sDEPPacket.ui8DEPLength > 0)
	{
		TRF79x0_writeFIFO(g_sDEPPacket.pui8DEPBuffer,CRC_BIT_ENABLE,g_sDEPPacket.ui8DEPLength,0);
	}
}

//*****************************************************************************
//
//! NFCDEP_sendPSL_RES - Sends PSL Response.
//!
//! This function sends PSL Response.
//!
//! \return STATUS_SUCCESS(0x01) if packet is sent succesfully else
//! STATUS_FAIL(0x00).
//
//*****************************************************************************
void NFCDEP_sendPSL_RES(void)
{
	uint8_t ui8Offset = 1;

	//
	// Command
	//
	g_sDEPPacket.pui8DEPBuffer[ui8Offset++] = (uint8_t) ((PSL_RES_CMD & 0xFF00) >> 8);
	g_sDEPPacket.pui8DEPBuffer[ui8Offset++] = (uint8_t) (PSL_RES_CMD & 0x00FF);

	g_sDEPPacket.pui8DEPBuffer[ui8Offset++] = g_ui8NfcDepDID;

	//
	// Length
	//
	g_sDEPPacket.pui8DEPBuffer[0] = ui8Offset;

	g_sDEPPacket.ui8DEPLength = ui8Offset;

	TRF79x0_writeFIFO(g_sDEPPacket.pui8DEPBuffer,CRC_BIT_ENABLE,g_sDEPPacket.ui8DEPLength,0);
}

//*****************************************************************************
//
//! NFCDEP_sendRSL_RES - Sends RSL Response.
//!
//! This function sends RSL Response.
//!
//! \return STATUS_SUCCESS(0x01) if packet is sent succesfully else
//! STATUS_FAIL(0x00).
//
//*****************************************************************************
void NFCDEP_sendRSL_RES(void)
{
	uint8_t ui8Offset = 1;

	//
	// Command
	//
	g_sDEPPacket.pui8DEPBuffer[ui8Offset++] = (uint8_t) ((RSL_RES_CMD & 0xFF00) >> 8);
	g_sDEPPacket.pui8DEPBuffer[ui8Offset++] = (uint8_t) (RSL_RES_CMD & 0x00FF);

	if(g_bNfcDepDIDEnabled)
	{
		g_sDEPPacket.pui8DEPBuffer[ui8Offset++] = g_ui8NfcDepDID;
	}

	//
	// Length
	//
	g_sDEPPacket.pui8DEPBuffer[0] = ui8Offset;

	g_sDEPPacket.ui8DEPLength = ui8Offset;

	TRF79x0_writeFIFO(g_sDEPPacket.pui8DEPBuffer,CRC_BIT_ENABLE,g_sDEPPacket.ui8DEPLength,0);

	// Added to handle TC_LIS_NFCA_UND_BI_18_xy_00
	TRF79x0_setNfcMode(false);

}

//*****************************************************************************
//
//! NFCDEP_sendDSL_RES - Sends DSL Response.
//!
//! This function sends DSL Response.
//!
//! \return STATUS_SUCCESS(0x01) if packet is sent succesfully else
//! STATUS_FAIL(0x00).
//
//*****************************************************************************
void NFCDEP_sendDSL_RES(void)
{
	uint8_t ui8Offset = 1;

	//
	// Command
	//
	g_sDEPPacket.pui8DEPBuffer[ui8Offset++] = (uint8_t) ((DSL_RES_CMD & 0xFF00) >> 8);
	g_sDEPPacket.pui8DEPBuffer[ui8Offset++] = (uint8_t) (DSL_RES_CMD & 0x00FF);

	if(g_bNfcDepDIDEnabled == true)
	{
		g_sDEPPacket.pui8DEPBuffer[ui8Offset++] = g_ui8NfcDepDID;
	}

	//
	// Length
	//
	g_sDEPPacket.pui8DEPBuffer[0] = ui8Offset;

	g_sDEPPacket.ui8DEPLength = ui8Offset;

	TRF79x0_writeFIFO(g_sDEPPacket.pui8DEPBuffer,CRC_BIT_ENABLE,g_sDEPPacket.ui8DEPLength,0);

	// Added to handle TC_LIS_NFCA_UND_BV_105_xy_00
	TRF79x0_setNfcMode(false);
}

//*****************************************************************************
//
//! NFCDEP_getDEPCommand - This function provides the NFCDEP command
//!
//! \param pui8RxPayload is the pointer to the received NFC DEP packet.
//! \param ui8CmdLength is the length of the received NFC DEP packet.
//!
//! This function returns the NFC DEP command code if the length of the NFC
//! DEP Packet is greater than 2 bytes.
//!
//! \return ui16DEPCmd is the NFC DEP command code.
//
//*****************************************************************************
uint16_t NFCDEP_getDEPCommand(uint8_t * pui8RxPayload , uint8_t ui8CmdLength)
{
	uint16_t ui16DEPCmd;

	if(ui8CmdLength > 2)
	{
		ui16DEPCmd = (pui8RxPayload[2]  + (pui8RxPayload[1] << 8));
	}
	else
	{
		ui16DEPCmd = 0x0000;
	}

	return ui16DEPCmd;
}

//*****************************************************************************
//
//! NFCDEP_stateMachine - This function executes when the transceiver is in
//! initiator mode.
//!
//! This function executes the main statemachine when the transceiver is in
//! initiator mode.
//!
//! \return None.
//
//*****************************************************************************
void NFCDEP_stateMachine(void)
{
	switch(g_eCurrentDepState)
	{
	case DEP_ATR_REQ_STATE:
		NFCDEP_sendATR_REQ();
	    // Set the correct timeout value
		// RWTactivation + Delta RWT ==> ~1238 mS
	    g_ui16NfcDepTimeOut = 1260;
		break;
	case DEP_PSL_REQ_STATE:
		NFCDEP_sendPSL_REQ();
		break;
	case DEP_ACTIVATED_STATE:
		// Fetch the current timeout count to check if ATN PDU needs to be sent
		NFCDEP_sendDEP_REQ();
		break;
	case DEP_DEACTIVATED_STATE:
		NFCDEP_sendRSL_REQ();
		break;
	case DEP_DESELECTED_STATE:
		NFCDEP_sendDSL_REQ();
		break;
	}
}

//*****************************************************************************
//
//! NFCDEP_processReceivedRequest - This function executes when the transceiver
//! is in target mode.
//!
//! \param pui8RxPayload is a pointer to the received payload.
//! \param ui8CmdLength is the length of the received payload.
//!
//! This function parses the incoming requests from the initiator. If the
//! request is valid, the function replies with the corresponding cmd response.
//!
//! \return ui8NfcDepStatus whether the request command is valid or not.
//
//*****************************************************************************
uint8_t NFCDEP_processReceivedRequest(uint8_t * pui8RxPayload , uint8_t ui8CmdLength)
{
	 volatile uint8_t ui8CommandLength;
	 uint16_t ui16CommandId;
	 uint8_t ui8NfcDepStatus = STATUS_SUCCESS;
	 uint8_t ui8PFBValue;
	 uint8_t ui8Offset = 0;
	 uint8_t ui8Counter;
	 t_sTRF79x0_Bitrate sTRF79x0_Bitrate;

#if NFC_DEP_DEBUG
	 uint8_t pui8StringBuffer[5];
#endif

	 sTRF79x0_Bitrate.ui16word = 0x0000;
        
	ui8CommandLength = pui8RxPayload[ui8Offset];
	ui16CommandId = pui8RxPayload[ui8Offset+2]  + (pui8RxPayload[ui8Offset+1] << 8);

	ui8PFBValue = pui8RxPayload[ui8Offset+3];

	if(ui16CommandId == ATR_REQ_CMD)
	{
		// Temp read
		g_ui16NFCDEPMaxPayload = pui8RxPayload[ui8Offset+16];
		g_ui16NFCDEPMaxPayload = g_ui16NFCDEPMaxPayload & 0x30;
		g_ui16NFCDEPMaxPayload = g_ui16NFCDEPMaxPayload >> 4;

		switch(g_ui16NFCDEPMaxPayload)
		{
		case 0x00:
			g_ui16NFCDEPMaxPayload = 64;
			break;
		case 0x01:
			g_ui16NFCDEPMaxPayload = 128;
			break;
		case 0x02:
			g_ui16NFCDEPMaxPayload = 192;
			break;
		case 0x03:
			g_ui16NFCDEPMaxPayload = 254;
			break;
		default:
			g_ui16NFCDEPMaxPayload = 64;
			break;
		}

		// Calculate # of Bytes not including Len Byte or NFC-A SOF (0xF0)
		if((ui8CmdLength - 1 - ui8Offset) > g_ui16NFCDEPMaxPayload)
		{
			// Error occured
			ui8NfcDepStatus = STATUS_FAIL;
		}
		else
		{
			// Maximum Time is T0 = 14
			g_ui16NfcDepTimeOut = (uint16_t) (1 << (0x0E));
			g_ui16NfcDepTimeOut = g_ui16NfcDepTimeOut * 3;
			g_ui16NfcDepTimeOut = g_ui16NfcDepTimeOut / 10;

			ui8Counter = 0;

			while(ui8CommandLength > (ui8Counter+20+ui8Offset))
			{
				//
				// Process the TLV - pass the starting address of the TLV
				//
				LLCP_processTLV(&pui8RxPayload[ui8Counter+20+ui8Offset]);

				if(pui8RxPayload[ui8Counter+20+ui8Offset] == LLCP_LTO)
				{
					g_ui16NfcDepTimeOut = LLCP_getLinkTimeOut();
				}

				//
				// Increment ui8Counter by the length+ 2 (type and length) of
				// the current TLV
				//
				ui8Counter = ui8Counter+ pui8RxPayload[ui8Counter+21+ui8Offset] + 2;
			}

			g_ui8NfcDepDID = pui8RxPayload[ui8Offset+13]; // Store DID byte into a global variable

			if(g_ui8NfcDepDID > 0x00)
			{
				g_bNfcDepDIDEnabled = true;
			}
			else
			{
				g_bNfcDepDIDEnabled = false;
			}
			ui8NfcDepStatus = NFCDEP_sendATR_RES();
			// Reset the PNI
			g_ui8NfcDepPni = 0x00;
		}
	}
	else if(ui16CommandId == PSL_REQ_CMD)
	{
		// Check if Bit 3 = DID is set
		if(pui8RxPayload[ui8Offset+3] == g_ui8NfcDepDID)
		{
			// Check if the DSI (Bits 5-3) == 010b => 424kbaud (Init. to Target)
			//  if the DRI (2-0) == 010b => 424kbaud (Target to Initiator)
			if(((pui8RxPayload[ui8Offset+4] & 0x38) == 0x10) &&
			   ((pui8RxPayload[ui8Offset+4] & 0x07) == 0x02))
			{
				NFCDEP_sendPSL_RES();
				sTRF79x0_Bitrate.bits.b424kpbs = 1;
				TRF79x0_incrementTargetBaudRate(sTRF79x0_Bitrate);
			}
			// Check if the DSI (Bits 5-3) == 001b => 212kbaud (Init. to Target)
			//  if the DRI (2-0) == 001b => 212kbaud (Target to Initiator)
			else if ( ((pui8RxPayload[ui8Offset+4] & 0x38) == 0x08) &&
					 ((pui8RxPayload[ui8Offset+4] & 0x07) == 0x01))
			{
				NFCDEP_sendPSL_RES();
				sTRF79x0_Bitrate.bits.b212kpbs = 1;
				TRF79x0_incrementTargetBaudRate(sTRF79x0_Bitrate);
			}
			// Check if the DSI (Bits 5-3) == 000b => 106kbaud (Init. to Target)
			//  if the DRI (2-0) == 000b => 106kbaud (Target to Initiator)
			else if ( ((pui8RxPayload[ui8Offset+4] & 0x38) == 0x00) &&
					 ((pui8RxPayload[ui8Offset+4] & 0x07) == 0x00))
			{
				NFCDEP_sendPSL_RES();
				sTRF79x0_Bitrate.bits.b106kpbs = 1;
				TRF79x0_incrementTargetBaudRate(sTRF79x0_Bitrate);
			}
		}
		else
		{
			// Do not reply if the DID is incorrect
		}
	}
	else if(ui16CommandId == DEP_REQ_CMD)
	{
//		// Calculate # of Bytes not including Len Byte or NFC-A SOF (0xF0)
//		if((ui8CmdLength - 1 - ui8Offset) > g_ui16NFCDEPMaxPayload)
//		{
//			// Error occured
//			ui8NfcDepStatus = STATUS_FAIL;
//			g_eNextPduType = NO_REPLY_PDU;
//		}
		// Check for information PDU
		if((ui8PFBValue & 0xE0) == 0x00)
		{
			// Check if Bit 3 = DID is set
			if(ui8PFBValue & 0x04)
			{
				if(pui8RxPayload[ui8Offset+4] == g_ui8NfcDepDID
						&& g_bNfcDepDIDEnabled == true )
				{
					ui8Offset++;
				}
				else
				{
					g_eNextPduType = NO_REPLY_PDU;
				}
			}

			// Check if Bit 4 = NAD is set
			if(ui8PFBValue & 0x08)
			{
				ui8Offset++;
			}

			if(g_bATNReceived == false || ((ui8PFBValue & 0x03 ) == g_ui8NfcDepPni))
			{
				// Reset ATN flag
				g_bATNReceived = false;
				// Copy the buffer
				for(ui8Counter = (ui8Offset+4); ui8Counter < ui8CmdLength; ui8Counter++)
				{
					g_sDEPPacket.pui8DEPRXBuffer[g_ui16Index++] = pui8RxPayload[ui8Counter];
				}
				// Store the packet length based on the index counter
				g_ui16PacketLength = g_ui16Index;
				g_ui16TXIndex = 0;
			}

			// Check if chaining is not set
			if((ui8PFBValue & 0x10) == 0x00)
			{
				// Reset Index to 0
				g_ui16Index = 0;
				if(g_eNextPduType != NO_REPLY_PDU)
				{
					g_eNextPduType = INFORMATION_PDU;
				}
			}
			else
			{
				g_eNextPduType = ACK_PDU;
			}
		}
		// Check for response PDU
		else if((ui8PFBValue & 0xE0) == 0x40)
		{
			if(ui8PFBValue & 0x10)
			{
				// Acknowledge
				g_eNextPduType = NACK_PDU;
			}
			else
			{
				// Check if we are in the middle of chaining
				if(g_ui16TXRemaining > 0)
				{
					g_eNextPduType = INFORMATION_PDU;
				}
				else
				{
					g_eNextPduType = ACK_PDU;
				}
			}
		}
		// Check for supervisory PDU
		else if((ui8PFBValue & 0xE0) == 0x80)
	    {
			if(ui8PFBValue & 0x10)
			{
				// Timeout
				g_eNextPduType = RTOX_REQ_PDU;
			}
			else
			{
				// Ensure PNI is only decremented once
				if(g_bATNReceived == false)
				{
					g_bATNReceived  = true;
				}
				g_eNextPduType = ATN_PDU;
			}
	    }

		if(g_eNextPduType == INFORMATION_PDU)
		{
#if NFC_DEP_DEBUG
			convertWordToAscii(g_ui16PacketLength,(uint8_t *)pui8StringBuffer);

			Serial_printf("DEP RX Information PDU\n",DEP_LAYER_DATA);

			Serial_printf("DEP RX ",DEP_LAYER_DATA);

			Serial_printBuffer((char *) pui8StringBuffer, 5,DEP_LAYER_DATA);

			Serial_printf(" bytes\n",DEP_LAYER_DATA);
#endif

			if(g_bNfcDEPLoopback)
			{
				NFCDEP_sendDEP_RES();
			}
			else if(g_bLLCPEnabled)
			{
				//
				// LLCP Packet Handler
				//
				ui8NfcDepStatus = LLCP_processReceivedData(g_sDEPPacket.pui8DEPRXBuffer, g_ui16PacketLength);

				if(ui8NfcDepStatus == STATUS_SUCCESS)
				{
					NFCDEP_sendDEP_RES();
				}
			}
		}
		else if(g_eNextPduType == NACK_PDU)
		{
			// Re-send last packet.
			TRF79x0_writeFIFO(g_sDEPPacket.pui8DEPBuffer,CRC_BIT_ENABLE,g_sDEPPacket.ui8DEPLength,0);
		}
		else if(g_eNextPduType == ACK_PDU || g_eNextPduType == ATN_PDU)
		{
			NFCDEP_sendDEP_RES();
		}
		else if(g_eNextPduType == NO_REPLY_PDU)
		{
			//  Do not reply
		}
		else
		{
			//  Do not reply
		}

	}
	else if(ui16CommandId == DSL_REQ_CMD)
	{
		// Check if DID is included
		if(ui8CommandLength == 0x04 && g_bNfcDepDIDEnabled == true)
		{
			if(g_ui8NfcDepDID == pui8RxPayload[ui8Offset+3])
			{
				NFCDEP_sendDSL_RES();
			}
			else
			{
				// Do not reply
			}
		}
		// No DID included
		else if(ui8CommandLength == 0x03)
		{
			NFCDEP_sendDSL_RES();
		}
		else
		{
			// Do not reply
		}
	}
    else if(ui16CommandId == RSL_REQ_CMD)
    {
		// Check if DID is included
		if(ui8CommandLength == 0x04 && g_bNfcDepDIDEnabled == true)
		{
			if(g_ui8NfcDepDID == pui8RxPayload[ui8Offset+3])
			{
				 NFCDEP_sendRSL_RES();
			}
			else
			{
				// Do not reply
			}
		}
		// No DID included
		else if(ui8CommandLength == 0x03)
		{
			g_bNfcDepDIDEnabled = false;
			NFCDEP_sendRSL_RES();
		}
		else
		{
			// Do not reply
		}
    }
    else
    {
		ui8NfcDepStatus = STATUS_FAIL;
    }
    return ui8NfcDepStatus;
}

//*****************************************************************************
//
//! NFCDEP_processReceivedData - This function executes when the transceiver
//! is in initiator mode.
//!
//! \param pui8RxPayload is a pointer to the received payload.
//! \param ui8CmdLength is the length of the received payload.
//!
//! This function parses the incoming response from the target. If the
//! response is valid, the function sets the g_eCurrentDepState state.
//!
//! \return g_eCurrentDepState the current DEP state.
//
//*****************************************************************************
tDepState NFCDEP_processReceivedData(uint8_t * pui8RxBuffer, uint8_t ui8Length)
{
    volatile uint8_t ui8CommandLength;
    uint16_t ui16CommandId;
    uint8_t ui8Counter;
    uint8_t ui8PFBValue;
    uint8_t ui8Offset;
    t_sNfcP2PCommBitrate sBitrates;
    t_sNfcP2PCommBitrate sCurrentBitrate;
    t_sTRF79x0_Bitrate sTrfBitrate;

	ui8Offset = 0x00;

	ui8CommandLength = pui8RxBuffer[ui8Offset];
	ui16CommandId = pui8RxBuffer[ui8Offset+2]  + (pui8RxBuffer[ui8Offset+1] << 8);

	ui8PFBValue = pui8RxBuffer[3+ui8Offset];

	sCurrentBitrate = NFC_P2P_getInitiatorBitrate();
	sBitrates = NFC_P2P_getInitiatorSupportedBitrates();
	if (sCurrentBitrate.bits.bPassive106kbps
			&& sBitrates.bits.bPassive106kbps)
	{
		// Type A - FDT(A,POLL,MIN)
		MCU_delayMicrosecond(500);
	}
	else if ((sCurrentBitrate.bits.bPassive212kbps && sBitrates.bits.bPassive212kbps)
			|| (sCurrentBitrate.bits.bPassive424kbps && sBitrates.bits.bPassive424kbps))
	{
		// Type F - FDT(F,POLL,MIN) 728
		MCU_delayMicrosecond(728);
	}
	else
	{
		// Delay 728 uS
		MCU_delayMicrosecond(728);
	}

	if(ui16CommandId == ATR_RES_CMD)
    {
        //
        // Store the g_pui8NFCID3t
        //
        for(ui8Counter = 0; ui8Counter < 10; ui8Counter++)
        {
            g_pui8NFCID3t[ui8Counter] = pui8RxBuffer[3+ui8Counter];
        }

        // Handle T0
        g_ui16NfcDepTimeOut = 1 << (pui8RxBuffer[ui8Offset+16] & 0x0F);
        g_ui16NfcDepTimeOut = g_ui16NfcDepTimeOut * 3;
        g_ui16NfcDepTimeOut = g_ui16NfcDepTimeOut / 10;
        // This handles the inaccuracies from the above estimated calculation.
        g_ui16NfcDepTimeOut = g_ui16NfcDepTimeOut + 35;

        // Check Size supported
        switch(pui8RxBuffer[17] & 0x30)
        {
        case 0x00:
        	g_ui16NFCDEPMaxPayload = 64;
        	break;
        case 0x10:
        	g_ui16NFCDEPMaxPayload = 128;
        	break;
        case 0x20:
        	g_ui16NFCDEPMaxPayload = 192;
        	break;
        case 0x30:
        	g_ui16NFCDEPMaxPayload = 254;
        	break;
        default:
        	g_ui16NFCDEPMaxPayload = 254;
        	break;
        }

		if (g_bLLCPEnabled) {
			//
			// LLCP Decoding - RFU
			//
			if(pui8RxBuffer[ui8Offset+18] == LLCP_MAGIC_NUMBER_HIGH && \
					pui8RxBuffer[ui8Offset+19] == LLCP_MAGIC_NUMBER_MIDDLE && \
					pui8RxBuffer[ui8Offset+20] == LLCP_MAGIC_NUMBER_LOW)
			{
				ui8Counter = 0;
				while(ui8CommandLength > (ui8Counter+21+ui8Offset))
				{
					//
					// Process the TLV - pass the starting address of the TLV
					//
					LLCP_processTLV(&pui8RxBuffer[ui8Counter+21+ui8Offset]);

					if(pui8RxBuffer[ui8Counter+21+ui8Offset] == LLCP_LTO)
					{
						g_ui16NfcDepTimeOut = LLCP_getLinkTimeOut();
					}

					//
					// Increment ui8Counter by the length+ 2 (type and length) of
					// the current TLV
					//
					ui8Counter = ui8Counter+ pui8RxBuffer[ui8Counter+22+ui8Offset] + 2;
				}
			}
		}

		if (g_bNfcDepTestEnable == 1)
		{
			// Check if current passive bitrate is 212kbps and passive 424kbps is supported
			if(sCurrentBitrate.bits.bPassive212kbps == 1 &&
					sBitrates.bits.bPassive424kbps == 1)
			{
				g_eCurrentDepState = DEP_PSL_REQ_STATE;
			}
			// Check if current active bitrate is 212kbps and active 424kbps is supported
			else if(sCurrentBitrate.bits.bActive212kbps == 1 &&
					sBitrates.bits.bActive424kbps == 1)
			{
				g_eCurrentDepState = DEP_PSL_REQ_STATE;
			}
			else
			{
				g_eCurrentDepState = DEP_ACTIVATED_STATE;
			}
		}
		else
		{
			// Check if current passive bitrate is 212kbps and passive 424kbps is supported
			if(sCurrentBitrate.bits.bPassive212kbps == 1 &&
					sBitrates.bits.bPassive424kbps == 1)
			{
				g_eCurrentDepState = DEP_PSL_REQ_STATE;
			}
			// Check if current active bitrate is 212kbps and active 424kbps is supported
			else if(sCurrentBitrate.bits.bActive212kbps == 1 &&
					sBitrates.bits.bActive424kbps == 1)
			{
				g_eCurrentDepState = DEP_PSL_REQ_STATE;
			}
			// Check if current passive bitrate is 106kbps and passive 212/424kbps is supported
			else if(sCurrentBitrate.bits.bPassive106kbps == 1 &&
					(sBitrates.bits.bPassive212kbps == 1 ||
							sBitrates.bits.bPassive424kbps == 1))
			{
				g_eCurrentDepState = DEP_PSL_REQ_STATE;
			}
			// Check if current active bitrate is 106kbps and active 212/424kbps is supported
			else if(sCurrentBitrate.bits.bActive106kbps == 1 &&
					(sBitrates.bits.bActive212kbps == 1 ||
							sBitrates.bits.bActive424kbps == 1))
			{
				g_eCurrentDepState = DEP_PSL_REQ_STATE;
			}
			else
			{
				g_eCurrentDepState = DEP_ACTIVATED_STATE;
			}
		}
	}
	else if(ui16CommandId == PSL_RES_CMD)
	{
		// Check if DID is correct
		if(pui8RxBuffer[3+ui8Offset] == 0x00)
		{
	        sTrfBitrate.ui16word = 0x0000;

		    // Check if current passive bitrate is 212kbps and passive 424kbps is supported
		    if(sCurrentBitrate.bits.bPassive212kbps == 1 &&
		    	sBitrates.bits.bPassive424kbps == 1)
		    {
				sTrfBitrate.bits.b424kpbs = 1;
		    }
		    // Check if current active bitrate is 212kbps and active 424kbps is supported
		    else if(sCurrentBitrate.bits.bActive212kbps == 1 &&
		        	sBitrates.bits.bActive424kbps == 1)
		    {
				sTrfBitrate.bits.b424kpbs = 1;
		    }
		    // Check if current passive bitrate is 106kbps and passive 424kbps is supported
		    else if(sCurrentBitrate.bits.bPassive106kbps == 1 &&
		        	sBitrates.bits.bPassive424kbps == 1)
		    {
				sTrfBitrate.bits.b424kpbs = 1;
		    }
		    // Check if current passive bitrate is 106kbps and passive 212kbps is supported
		    else if(sCurrentBitrate.bits.bPassive106kbps == 1 &&
		        	sBitrates.bits.bPassive212kbps == 1)
		    {
				sTrfBitrate.bits.b212kpbs = 1;
		    }
		    // Check if current active bitrate is 106kbps and active 424kbps is supported
		    else if(sCurrentBitrate.bits.bActive106kbps == 1 &&
		        	sBitrates.bits.bActive424kbps == 1)
		    {
				sTrfBitrate.bits.b424kpbs = 1;
		    }
		    // Check if current active bitrate is 106kbps and active 212kbps is supported
		    else if(sCurrentBitrate.bits.bActive106kbps == 1 &&
		        	sBitrates.bits.bActive212kbps == 1)
		    {
				sTrfBitrate.bits.b212kpbs = 1;
		    }
		    TRF79x0_incrementInitiatorBaudRate(sTrfBitrate);

			g_eCurrentDepState = DEP_ACTIVATED_STATE;
		}
	}
	else if(ui16CommandId == DEP_RES_CMD)
	{
		// Reset protocol errors to 0
		g_ui8NfcDepProtocolErrors = 0;

		// Reset timeout counter to 0
		g_ui8NfcDepInitTimeoutCount = 0;

		if((ui8PFBValue & 0xE0) == 0x00)
		{
			// Successfully received DEP_RES after sending an SOT frame
			if (g_bNfcDepTestEnable)
			{
				g_bSendSOT = false;
			}
			if(ui8PFBValue & 0x04)
			{
				if(pui8RxBuffer[ui8Offset+4] == g_ui8NfcDepDID
						&& g_bNfcDepDIDEnabled == true )
				{
					ui8Offset++;
				}
				else
				{
					g_eNextPduType = NO_REPLY_PDU;
				}
			}
			// Check if Bit 4 = NAD is set
			if(ui8PFBValue & 0x08)
			{
				ui8Offset++;
			}
			if(g_bATNReceived == false || ((ui8PFBValue & 0x03 ) == g_ui8NfcDepPni))
			{
				// Reset ATN flag
				g_bATNReceived = false;
				// Copy the buffer
				for(ui8Counter = (ui8Offset+4); ui8Counter < ui8Length; ui8Counter++)
				{
					g_sDEPPacket.pui8DEPRXBuffer[g_ui16Index++] = pui8RxBuffer[ui8Counter];
				}
				// Store the packet length based on the index counter
				g_ui16PacketLength = g_ui16Index;
				g_ui16TXIndex = 0;
			}

			// Check if chaining is not set
			if((ui8PFBValue & 0x10) == 0x00)
			{
				// Reset Index to 0
				g_ui16Index = 0;
				if(g_eNextPduType != NO_REPLY_PDU)
				{
					g_eNextPduType = INFORMATION_PDU;
				}
			}
			else
			{
				g_eNextPduType = ACK_PDU;
			}
		}
		else if((ui8PFBValue & 0xE0) == 0x80)
		{
			if (ui8PFBValue & 0x10)
			{
				g_eNextPduType = RTOX_REQ_PDU;
				g_ui8RtoxTransportData = (0x3F & pui8RxBuffer[4+ui8Offset]);
			}
			// ATN Response
			else
			{
				// Send the last PDU received before the time out
				g_eNextPduType = g_eTimeoutPduType;

				// Set Flag to resend the last packet
				g_bATNReceived = true;

				// Reduce the PNI by 1 so the PNI matches that of the last sent PDU
				if(g_ui8NfcDepPni == 0x00)
				{
					g_ui8NfcDepPni = 0x03;
				}
				else
				{
					g_ui8NfcDepPni = g_ui8NfcDepPni - 1;
				}
			}
		}

		if(g_eNextPduType == INFORMATION_PDU)
		{
		//
		// LLCP Packet Handler
		//
			if(LLCP_processReceivedData(&pui8RxBuffer[4+ui8Offset], (ui8CommandLength-4)) == STATUS_SUCCESS)
			{
				if (g_bNfcDepTestEnable) {
					if (NFCDEP_checkEOT(pui8RxBuffer,ui8Length))
					{
						if (pui8RxBuffer[ui8Offset+8] == 0x01)
						{
							g_eCurrentDepState = DEP_DESELECTED_STATE;
						}
						else if (pui8RxBuffer[ui8Offset+8] == 0x02)
						{
							g_eCurrentDepState = DEP_DEACTIVATED_STATE;
						}
						else
						{
							// Do Nothing
						}
					}
					else
					{
						g_eCurrentDepState = DEP_ACTIVATED_STATE;
					}
				}
				else
				{
					g_eCurrentDepState = DEP_ACTIVATED_STATE;
				}
			}
			else
			{
				// Do Nothing
			}
		}

	}
	else if(ui16CommandId == DSL_RES_CMD)
	{

	}
	else if(ui16CommandId == RSL_RES_CMD)
	{

	}

	return g_eCurrentDepState;
}

//*****************************************************************************
//
//! NFCDEP_getBitrate - This function returns the current connection bitrate.
//!
//! This function returns the current initiator or target bitrate.
//!
//! \return sPayloadFrequency the current communication bitrate.
//
//*****************************************************************************
t_sTRF79x0_Bitrate NFCDEP_getBitrate(void)
{
	tTRF79x0_Status sTRFStatus;
	t_sTRF79x0_Bitrate sPayloadFrequency;

	sTRFStatus = TRF79x0_getStatus();

	if(sTRFStatus.eCurrentMode == INITIATOR_MODE)
	{
		sPayloadFrequency = sTRFStatus.sInitiatorPayloadFrequency;
	}
	else if(sTRFStatus.eCurrentMode == TARGET_MODE)
	{
		sPayloadFrequency = sTRFStatus.sTargetPayloadFrequency;
	}

	return sPayloadFrequency;
}

//*****************************************************************************
//
//! NFCDEP_getLinkTimeOut - This function returns the current connection timeout.
//!
//! This function returns the current connection timeout. If there is a RTOX
//! request then it returns the RTOXTimeout.
//!
//! \return g_ui16NfcDepTimeOut the current NFC DEP timeout.
//
//*****************************************************************************
uint16_t NFCDEP_getLinkTimeOut(void)
{
	if (g_bRTOXTimeout)
	{
		g_bRTOXTimeout = false;
		return g_ui16NfcDepRTOXTimeOut;
	}
	else
	{
		return g_ui16NfcDepTimeOut;
	}
}

//*****************************************************************************
//
//! NFCDEP_setNFCID3t - This function initializes the NFCID3t buffer.
//!
//! \param pui8NFCID3 is the pointer to the NFCID3 payload.
//! \param ui8Length is the length of the NFCID3 buffer.
//!
//! This function sets the NFCID3t buffer. The length must be 10 bytes.
//!
//! \return None.
//
//*****************************************************************************
void NFCDEP_setNFCID3t(uint8_t * pui8NFCID3,uint8_t ui8Length)
{
	uint8_t ui8Index;

	// Error Length
	if(ui8Length > 10)
		return;

	for(ui8Index=0;ui8Index < ui8Length;ui8Index++)
	{
		g_pui8NFCID3t[ui8Index] = pui8NFCID3[ui8Index];
	}
}

//*****************************************************************************
//
//! NFCDEP_setNFCID2i - This function initializes the NFCID2i buffer.
//!
//! \param pui8NFCID2 is the pointer to the NFCID2 payload.
//! \param ui8Length is the length of the NFCID2 buffer.
//!
//! This function sets the NFCID3t buffer. The length must be 8 bytes.
//!
//! \return None.
//
//*****************************************************************************
void NFCDEP_setNFCID2i(uint8_t * pui8NFCID2, uint8_t ui8Length)
{
	uint8_t ui8Index;

	// Error Length
	if(ui8Length > 8)
		return;

	for(ui8Index=0;ui8Index < ui8Length;ui8Index++)
	{
		g_pui8NFCID2i[ui8Index] = pui8NFCID2[ui8Index];
	}
}

//*****************************************************************************
//
//! NFCDEP_sendSOT - This function sends the Start Of Test Frame for NFC Wave
//! 1 Certification.
//!
//! \param ui8Offset is the offset on the current TX packet to write the SOT.
//!
//! This function handles sending the Start of Test frames for Wave 1
//! NFC Forum Certification
//!
//! \return ui8Offset the length of the total SOT packet inluding the header
//! overhead and the SOT Test frame.
//
//*****************************************************************************
uint8_t NFCDEP_sendSOT(uint8_t ui8Offset)
{
//Start of Test Frame: 	0x004000011002010E
	g_sDEPPacket.pui8DEPBuffer[ui8Offset++] = 0x00;
	g_sDEPPacket.pui8DEPBuffer[ui8Offset++] = 0x40;
	g_sDEPPacket.pui8DEPBuffer[ui8Offset++] = 0x00;
	g_sDEPPacket.pui8DEPBuffer[ui8Offset++] = 0x01;
	g_sDEPPacket.pui8DEPBuffer[ui8Offset++] = 0x10;
	g_sDEPPacket.pui8DEPBuffer[ui8Offset++] = 0x02;
	g_sDEPPacket.pui8DEPBuffer[ui8Offset++] = 0x01;
	g_sDEPPacket.pui8DEPBuffer[ui8Offset++] = 0x0E;

	return ui8Offset;
}

//*****************************************************************************
//
//! NFCDEP_checkEOT - This function initializaes checks if the received packet
//! is the End of Test Frame.
//!
//! \param pui8RxBuffer is the pointer to the received buffer.
//! \param ui8Length is the length of the received packet.
//!
//! This function checks if the received packet is the End of Test Frame indicating
//! that the NFC DEP test has been completed.
//!
//! \return bReceiveEOT indicates if the EOT frame has been received.
//
//*****************************************************************************
bool NFCDEP_checkEOT(uint8_t * pui8RxBuffer, uint8_t ui8Length)
{
	bool bReceiveEOT;
	//End of Test Frame: 	0xFFFFFF0101 (DSL) or 0xFFFFFF0102 (RLS)
	uint8_t ui8Offset = 0;

	if (g_bNfcDepDIDEnabled)
	{
		ui8Offset++;
	}

	if (ui8Length == (ui8Offset+9)) {

		if ((pui8RxBuffer[ui8Offset+4] == 0xFF)
				&& (pui8RxBuffer[ui8Offset+5] == 0xFF)
				&& (pui8RxBuffer[ui8Offset+6] == 0xFF)
				&& (pui8RxBuffer[ui8Offset+7] == 0x01))
		{
			if ((pui8RxBuffer[ui8Offset+8] == 0x01)
					|| (pui8RxBuffer[ui8Offset+8] == 0x02))
			{
				bReceiveEOT = true;
			}
			else
			{
				bReceiveEOT = false;
			}
		}
		else
		{
			bReceiveEOT = false;
		}
	}
	else
	{
		bReceiveEOT = false;
	}
	return bReceiveEOT;
}

//*****************************************************************************
//
//! NFCDEP_triggerTimeout - This function is used to trigger a timeout event to
//! the NFC DEP layer.
//!
//! This function is used in order to keep track of how many sequential
//! timeouts have occured. A maximum number of timeouts will be entered during
//! initialization.When the maximum number of timeouts is reached, the software
//! will reset the part. Otherwise, it will send an ATN PDU.
//!
//! \return ui8Status indicates STATUS_SUCCESS if the timeout has not reached
//! the max timeout value, else it returns STATUS_FAIL.
//
//*****************************************************************************
uint8_t NFCDEP_triggerTimeout(void)
{
	uint8_t ui8Status;

	g_ui8NfcDepInitTimeoutCount = g_ui8NfcDepInitTimeoutCount + 1;

	if(g_ui8NfcDepInitTimeoutCount > g_ui8NfcDepInitiatorMaxTimeouts)
	{
		ui8Status = STATUS_FAIL;
	}
	else
	{
		ui8Status = STATUS_SUCCESS;
	}

	return ui8Status;
}

//*****************************************************************************
//
//! NFCDEP_triggerTimeout - This function is used to trigger a protocol error
//! event to the NFC DEP layer.
//!
//! This function is used in order to keep track of how many protocol errors
//! have occured. A maximum number of protocol errors will be entered during
//! initialization.When the maximum number of protocol errors is reached, the software
//! will reset the part. Otherwise, it will send an NACK PDU.
//!
//! \return ui8Status indicates STATUS_SUCCESS if the protocol error variable
//! has not reached the max number of protocol errors, else it returns STATUS_FAIL.
//
//*****************************************************************************
uint8_t NFCDEP_triggerProtocolError(void)
{
	uint8_t ui8Status;

	g_ui8NfcDepProtocolErrors = g_ui8NfcDepProtocolErrors + 1;

	if(g_ui8NfcDepProtocolErrors > g_ui8NfcDepMaxProtocolErrors)
	{
		ui8Status = STATUS_FAIL;
	}
	else
	{
		ui8Status = STATUS_SUCCESS;
	}

	return ui8Status;
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
