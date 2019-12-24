//*****************************************************************************
//
// iso_dep.c - Implementation of ISO DEP APIs.
//
// Copyright (c) 2015 Texas Instruments Incorporated.  All rights reserved.
// TI Information - Selective Disclosure
//
//*****************************************************************************

#include "iso_dep.h"

//*****************************************************************************
//
//! \addtogroup iso_dep_api ISO DEP API Functions
//! @{
//!
//! ISO DEP is the layer used to enabl synchronization in T4T command / responses.
//
//*****************************************************************************

//*****************************************************************************
//
// Stores the PCD Block Number (PICC and PCD modes)
//
//*****************************************************************************
volatile uint8_t g_ui8IsoDepBlockNumber;

//*****************************************************************************
//
// Stores the next ISO DEP PDU Type (PICC and PCD modes)
//
//*****************************************************************************
static tPCBBlock g_eIsoDepNextPduType;

//*****************************************************************************
//
// Stores the current ISO DEP state(PICC and PCD modes)
//
//*****************************************************************************
static tIsoDepState g_eCurrentIsoDepState;

//*****************************************************************************
//
// Stores the T4T technology used for activating ISO DEP (ISO_DEP_A or 
// ISO_DEP_B)
//
//*****************************************************************************
static tIsoDepType g_eIsoDepType;


//*****************************************************************************
//
// ISO_DEP TX Buf PCD + CID (optional) + NAD (optional) + 255 + 2 Epilogue Bytes
//
//*****************************************************************************
static uint8_t g_pui8IsoDepTXBuffer[260];

//*****************************************************************************
//
// ISO_DEP RX Buf PCD + CID (optional) + NAD (optional) + 255 + 2 Epilogue Bytes
//
//*****************************************************************************
static uint8_t g_pui8IsoDepRXBuffer[260];

//*****************************************************************************
//
// Stores the ISO DEP RX Payload Index (PICC and PCD Modes)
//
//*****************************************************************************
static uint16_t g_ui16IsoDepRxIndex;

//*****************************************************************************
//
// Stores the ISO DEP TX Payload Index (PCD mode)
//
//*****************************************************************************
static uint16_t g_ui16IsoDepTXIndex;

//*****************************************************************************
//
// Stores the ISO DEP Remaining Bytes Counter (PCD mode)
//
//*****************************************************************************
static uint16_t g_ui16IsoDepTxRemaining;

//*****************************************************************************
//
// Stores the size of the previous ISO DEP TX packet (PCD mode)
//
//*****************************************************************************
static uint16_t g_ui16IsoDepPrevTxDataLength;


static uint16_t g_ui16IsoDepTxLength;

static bool g_bIsoDepRetransmit;
static bool g_bChainingEnable;
static bool g_bIsoDepCIDEnabled;
static uint8_t g_ui8CIDValue;

static uint16_t g_ui16StartUpGuardTime;
static uint16_t g_ui16IsoDepTimeOut;
static uint16_t g_ui16WtxTimeoOut;

static bool g_bWTXTimeOut;
static uint8_t g_ui8WTXValue;

static uint16_t g_ui16ISODEPMaxPayload;

static uint8_t g_ui8IsoDepWTXCount;
static uint8_t g_ui8ISODepMaxWTX;

static uint8_t g_ui8IsoDepACKCount;
static uint8_t g_ui8ISODepMaxACK;

static uint8_t g_ui8IsoDepNACKCount;
static uint8_t g_ui8ISODepMaxNACK;

static uint8_t g_ui8IsoDepDSLCount;
static uint8_t g_ui8ISODepMaxDSL;


void ISODEP_configure_RW(t_sIsoDEP_RWSetup sRWSetupOptions, uint8_t ui8IsoDepDid)
{
	g_ui8ISODepMaxWTX = sRWSetupOptions.bits.ui3RWMaxWTX;
	g_ui8ISODepMaxACK = sRWSetupOptions.bits.ui3RWMaxACK;
	g_ui8ISODepMaxNACK = sRWSetupOptions.bits.ui3RWMaxNACK;
	g_ui8ISODepMaxDSL = sRWSetupOptions.bits.ui3RWMaxDSL;

	g_ui8CIDValue = ui8IsoDepDid;

	if (g_ui8CIDValue > 0)
	{
		g_bIsoDepCIDEnabled = true;

		NFC_A_setCIDSettings(true,g_ui8CIDValue);
	}
	else
	{
		g_bIsoDepCIDEnabled = false;

		NFC_A_setCIDSettings(false,g_ui8CIDValue);
	}
}

void ISODEP_init(bool bPCDModeEnable)
{
	// Rule A ISO14443-4 (PCD Rule)
	if(bPCDModeEnable == true)
	{
		g_ui8IsoDepBlockNumber = 0;
	}
	// Rule C ISO14443-4 (PICC Rule)
	else
	{
		g_ui8IsoDepBlockNumber = 1;
	}
	g_eIsoDepNextPduType = I_BLOCK;

	g_eCurrentIsoDepState = ISO_DEP_PARAM_SEL_STATE;

	g_ui16IsoDepRxIndex = 0;

	g_ui16IsoDepTxLength = 0;

	g_bChainingEnable = false;

	g_bIsoDepRetransmit = false;

	g_ui16IsoDepTimeOut = 0;

	g_ui16StartUpGuardTime = 0;

	g_eIsoDepType = ISO_DEP_A;

	g_ui16ISODEPMaxPayload = 0xFF;

	g_ui16IsoDepTXIndex = 0;

	g_ui16IsoDepTxRemaining = 0;

	g_ui16WtxTimeoOut = 0;

	g_ui16IsoDepPrevTxDataLength = 0;

	g_ui8WTXValue = 0;

	g_bWTXTimeOut = false;

	g_ui8IsoDepWTXCount = 0;

	g_ui8IsoDepACKCount = 0;

	g_ui8IsoDepNACKCount = 0;

	g_ui8IsoDepDSLCount = 0;

	ISO_7816_4_init();
}

//
// This function will send requests to the PICC when the NFC transceiver is in Reader/Writer Mode.
//
void ISODEP_stateMachine(void)
{
	uint16_t ui16SFGT;

	if(g_eIsoDepType == ISO_DEP_A)
	{
		ui16SFGT = NFC_A_getSFGT();
	}
	else if(g_eIsoDepType == ISO_DEP_B)
	{
		ui16SFGT = NFC_B_getSFGT();
	}
	else
	{
	}

	// Delay Guard Time
	MCU_delayMillisecond(ui16SFGT);

	if(g_bIsoDepRetransmit == true)
	{
		g_bIsoDepRetransmit = false;
		// Re-send last packet.
		TRF79x0_writeFIFO(g_pui8IsoDepTXBuffer,CRC_BIT_ENABLE,g_ui16IsoDepTxLength,0);
	}
	else
	{
		switch(g_eCurrentIsoDepState)
		{
		case ISO_DEP_PARAM_SEL_STATE:

			g_eCurrentIsoDepState = ISO_DEP_ACTIVATED_STATE;

			ISODEP_sendI_Block_REQ();

			break;
		case ISO_DEP_ACTIVATED_STATE:

			if(g_eIsoDepNextPduType == I_BLOCK)
			{
				ISODEP_sendI_Block_REQ();
				// Reset index
				g_ui16IsoDepRxIndex = 0;
			}
			else if(g_eIsoDepNextPduType == R_ACK_BLOCK
					|| g_eIsoDepNextPduType == R_NACK_BLOCK)
			{
				ISODEP_sendR_Block_REQ();
			}
			else if(g_eIsoDepNextPduType == S_DSL_BLOCK
					|| g_eIsoDepNextPduType == S_WTX_BLOCK)
			{
				ISODEP_sendS_Block_REQ();
			}
			else
			{
				// Send nothing
			}

			break;
		case ISO_DEP_DEACTIVATED_STATE:

			if(g_eIsoDepNextPduType == I_BLOCK)
			{
				ISODEP_sendI_Block_REQ();
				// Reset index
				g_ui16IsoDepRxIndex = 0;
			}
			else if(g_eIsoDepNextPduType == R_ACK_BLOCK
					|| g_eIsoDepNextPduType == R_NACK_BLOCK)
			{
				ISODEP_sendR_Block_REQ();
			}
			else if(g_eIsoDepNextPduType == S_DSL_BLOCK
					|| g_eIsoDepNextPduType == S_WTX_BLOCK)
			{
				ISODEP_sendS_Block_REQ();
			}
			else
			{
				// Send nothing
			}

			break;
		case ISO_DEP_PROTOCOL_ERROR:
//			// Rule 4 ISO14443-4 When an invalid block is received or a FWT time-out occurs send a R(NAK) (PCD Rule)
//			if(g_bChainingEnable == false &&
//					g_eIsoDepNextPduType != S_DSL_BLOCK)
//			{
//				g_eIsoDepNextPduType = R_ACK_BLOCK;
//				ISODEP_sendR_Block_REQ();
//			}
//			// Rule 5 ISO14443-4 (PCD Rule)
//			else if(g_bChainingEnable == true)
//			{
//				g_eIsoDepNextPduType = R_NACK_BLOCK;
//				ISODEP_sendR_Block_REQ();
//			}
			break;
		}
	}
}

//
// This function will handle the received requests when the NFC transceiver is in Card Emulation Mode.
//
tIsoDepState ISODEP_processReceivedRequest(uint8_t * pui8RxPayload , uint8_t ui8CmdLength)
{
	uint8_t ui8Command;
	uint8_t ui8Counter;
	uint8_t ui8Offset = 1;

	ui8Command = pui8RxPayload[PCD_INDEX];

	switch(g_eCurrentIsoDepState)
	{
	case ISO_DEP_PARAM_SEL_STATE:
		if(g_eIsoDepType == ISO_DEP_A)
		{
			// Check for RATS command
			if(ui8CmdLength == 0x02 && ui8Command == RATS_CMD)
			{
				g_ui16IsoDepTimeOut = 200;
				ISODEP_sendATS();
				// Switch state to activated state
				g_eCurrentIsoDepState = ISO_DEP_ACTIVATED_STATE;
				ISO_7816_4_setCommMode(ISO_7816_4_TAG_MODE);
				g_ui16ISODEPMaxPayload = 0xFF;
			}
			break;
		}
		else if(g_eIsoDepType == ISO_DEP_B)
		{
			if(ui8Command == ATTRIB_CMD)
			{
				NFC_B_send_ATTRIB_RES();
				g_ui16IsoDepTimeOut = 200;
				// PARAM Sel handled by the Target Statemachine
				g_eCurrentIsoDepState = ISO_DEP_ACTIVATED_STATE;
				ISO_7816_4_setCommMode(ISO_7816_4_TAG_MODE);
				g_ui16ISODEPMaxPayload = 0xFF;
			}
		}
		else
		{
			g_eCurrentIsoDepState = ISO_DEP_PARAM_SEL_STATE;
		}
		break;
	case ISO_DEP_ACTIVATED_STATE:

		// The length will be 0x02 if PPS1 is not transmitted
		if(((ui8Command & 0xF0) == PPS_CMD) && (ui8CmdLength == 0x02 || ui8CmdLength == 0x03))
		{
			// CID value (bits 3-0)
			g_ui8CIDValue &= 0x0F;
			ISODEP_sendPPS_RES();
			g_eIsoDepNextPduType = NO_REPLY_BLOCK;
		}
		// Check if the packet received is an I
		else if( (ui8Command & 0xC2) == 0x02 )
		{
			// Check for CID
			if(ui8Command & 0x08)
			{
				g_bIsoDepCIDEnabled = true;
				ui8Offset++;
				g_ui8CIDValue = pui8RxPayload[1];
			}
			else
			{
				g_bIsoDepCIDEnabled = false;
			}

			// Check for Chaining
			if(ui8Command & 0x10)
			{
				if(ui8CmdLength == 0xFF)
				{
					g_eIsoDepNextPduType = MUTE_BLOCK;
				}
				else
				{
					g_eIsoDepNextPduType = R_ACK_BLOCK;
				}
			}
			else
			{
				// Send I Block (Rule 10)
				g_eIsoDepNextPduType = I_BLOCK;
			}

			if(g_eIsoDepNextPduType != MUTE_BLOCK)
			{
				// Toggle Block number
				g_ui8IsoDepBlockNumber ^= 0x01;
			}

			// Check if RFU bit is cleared (required per the spec)
			if((ui8Command & 0x20) == 0x00)
			{
				// Copy the received data into the RX buffer
				for(ui8Counter=ui8Offset;ui8Counter<ui8CmdLength;ui8Counter++)
				{
					g_pui8IsoDepRXBuffer[g_ui16IsoDepRxIndex++] = pui8RxPayload[ui8Counter];
				}

			}
			else
			{
				// if it was incorrect set the RxIndex to 0
				g_ui16IsoDepRxIndex = 0;
			}


		}
		// Check if the packet received is an R
		else if( (ui8Command & 0xE0) == 0xA0 )
		{
			// Check for CID
			if(ui8Command & 0x08)
			{
				g_bIsoDepCIDEnabled = true;
				g_ui8CIDValue = pui8RxPayload[1];
			}
			else
			{
				g_bIsoDepCIDEnabled = false;
			}

			// Check for Chaining
			if(ui8Command & 0x10)
			{
				g_eIsoDepNextPduType = R_ACK_BLOCK;
			}

			// Check for correct block number
			if( (ui8Command & 0x01) == g_ui8IsoDepBlockNumber)
			{
				// Re-send last packet. (Rule 11)
				TRF79x0_writeFIFO(g_pui8IsoDepTXBuffer,CRC_BIT_ENABLE,g_ui16IsoDepTxLength,0);
				return g_eCurrentIsoDepState;
			}
			else
			{

				// Check for ACK
				if((ui8Command & 0x10) == 0x00)
				{
					// ACK received - chaining should be continued if chaining is set (Rule 13)
					g_ui8IsoDepBlockNumber ^= 0x01;
				}
				else
				{
					// NACK received -  Send R ACK (Rule 12)
					g_eIsoDepNextPduType = 	R_ACK_BLOCK;
				}
			}
		}
		// Check if the packet received is an S (Deselect or WTX)
		else if( ((ui8Command & 0xF0) == 0xC0)
				|| ((ui8Command & 0xF0) == 0xF0) )
		{
			if((ui8Command & 0xF0) == 0xC0)
			{
				// Received a deselect command
				g_eIsoDepNextPduType = S_DSL_BLOCK;
				g_eCurrentIsoDepState = ISO_DEP_PARAM_SEL_STATE;
			}
			else
			// If a FWT is received then mute the transceiver's decoder
			{
				g_eIsoDepNextPduType = MUTE_BLOCK;
			}

		}
		else
		{
			g_eIsoDepNextPduType = MUTE_BLOCK;
		}

		switch(g_eIsoDepNextPduType)
		{
		case I_BLOCK:
			if(ISO_7816_4_processReceivedRequest(g_pui8IsoDepRXBuffer,g_ui16IsoDepRxIndex) == STATUS_SUCCESS)
			{
				ISODEP_sendI_Block_REQ();
			}
			g_ui16IsoDepRxIndex = 0;
			break;
		case R_ACK_BLOCK:
			ISODEP_sendR_Block_REQ();
			break;
		case R_NACK_BLOCK:
			ISODEP_sendR_Block_REQ();
			break;
		case S_DSL_BLOCK:
			ISODEP_sendDSL_RES();
			break;
		case S_WTX_BLOCK:
			break;
		case NO_REPLY_BLOCK:
			// Do not reply
			break;
		case MUTE_BLOCK:
			TRF79x0_muteTimeMillisecond(MUTE_TIME_MS);
			break;
		}

		break;
	case ISO_DEP_DEACTIVATED_STATE:
		break;
	case ISO_DEP_PROTOCOL_ERROR:
		break;
	}

	return g_eCurrentIsoDepState;
}

tIsoDepState ISODEP_processReceivedData(uint8_t * pui8RxBuffer, uint8_t ui8Length)
{
	uint8_t ui8Counter;
	uint8_t ui8PCDByte;
	uint8_t ui8Offset = 0;

	switch(g_eCurrentIsoDepState)
	{
	case ISO_DEP_PARAM_SEL_STATE:
		if(g_eIsoDepType == ISO_DEP_A)
		{
			// Check for ATS - the first byte should the same as the packet length
			if(pui8RxBuffer[0] == ui8Length)
			{
				// Handle T0 - pui8RxBuffer[1]
				// BIT7 - Reserved for Future Use (RFU)
				// BIT6 - TA(1) Present
				// BIT5 - TB(1) Present
				// BIT4 - TC(1_ Present
				// BIT3:0 - FSCI = 1000, PICC capable of receiving 256 byte frames, from FSDI coding table

				// Preset to 1
				ui8Offset = 0x01;

				// Handle TA(1)
				if(pui8RxBuffer[0] & 0x40)
				{
					// TA(1) Interface Byte Codes DS and DR ( Default value must be 0x00)
					// BIT7 - Unable to support different divisor for each direction (1= True, 0 = False)
					// BIT6 - DS8 Supported
					// BIT5 - DS4 Supported
					// BIT4 - DS2 Supported
					// BIT3 - Reserved for Future Use (RFU)
					// BIT2 - DR8 Supported
					// BIT1 - DR4 Supported
					// BIT0 - DR2 Supported

					ui8Offset++;
				}

				// Handle TB(1)
				if(pui8RxBuffer[0] & 0x20)
				{
					// TB(1) Interface Byte Codes FWI and SFGI
					//
					// BIT7:4 - FWI  => FWT  = (256 x 16 / fc) x 2 ^ FWI (See Section 7.2 ISO14443-4:2000)
					//        - FWI = 4 => FWT = 4.833 mS

					// SFGT is a defined guard time needed by the PICC before it is ready to receive the next frame after the ATS.
					// BIT3:0 - SFGI => SFGT = (256 x 16 / fc) x 2 ^SFGI (See Section 5.2.5 ISO14443-4:2000)
					//        -SFGI = 2 => SFGT = 604 uS (See Section 6.1.4 ISO144443-3:2000)
					g_ui16StartUpGuardTime  = 1 << (pui8RxBuffer[ui8Offset] & 0x0F);
					g_ui16StartUpGuardTime = g_ui16StartUpGuardTime * 3;
					g_ui16StartUpGuardTime = g_ui16StartUpGuardTime / 10;

					g_ui16IsoDepTimeOut = 1 << ((pui8RxBuffer[ui8Offset] & 0xF0) >> 4);
					g_ui16IsoDepTimeOut = g_ui16IsoDepTimeOut * 3;
					g_ui16IsoDepTimeOut = g_ui16IsoDepTimeOut / 10;

					MCU_delayMillisecond(g_ui16StartUpGuardTime);

					ui8Offset++;
				}

				// Handle TC(1)
				if(pui8RxBuffer[0] & 0x10)
				{
					// TC(1) Interface Byte Codes Protocol Options (See Section 5.2.6 ISO14443-4:2000)
					//
					// BIT7:2 - Shall be set to (000000)b, all other values are Reserved for Future Use (RFU)
					// BIT1   - CID Supported ( default to 1)
					// BIT0   - NAD Supported ( default to 0)

					ui8Offset++;
				}

			}
		}
		break;
	case ISO_DEP_PPS_SEL_STATE:
		break;
	case ISO_DEP_ACTIVATED_STATE:
		// PCD Byte is the first byte
		ui8PCDByte = pui8RxBuffer[0];

		if((ui8PCDByte & 0xE2) == I_BLOCK && ((ui8PCDByte & 0x01) == g_ui8IsoDepBlockNumber))
		{
			// The PICC needed to send an R_ACK
			if(g_ui16IsoDepTxRemaining != 0)
			{
				g_eCurrentIsoDepState = ISO_DEP_PROTOCOL_ERROR;
				break;
			}

			// Reset the number of WTXs received
			g_ui8IsoDepWTXCount = 0;

			// Reset the number of ACKs received
			g_ui8IsoDepACKCount = 0;

			// Clear the number of NACKs sent
			g_ui8IsoDepNACKCount = 0;

			// Clear the number of DSLs sent
			g_ui8IsoDepDSLCount = 0;

			// Rule B ISO14443-4 (PCD Rule)

			// Toggle Block Number
			g_ui8IsoDepBlockNumber ^= 0x01;

			// Rule 2 ISO14443-4 (General Rule)
			if(ui8PCDByte & ISODEP_I_BLOCK_CHAINING_MASK)
			{
				g_bChainingEnable = true;
				g_eIsoDepNextPduType = R_ACK_BLOCK;
			}
			else
			{
				g_bChainingEnable = false;
				g_eIsoDepNextPduType = I_BLOCK;
				g_ui16IsoDepTXIndex = 0;
			}

			// Initialize Offset to 1 (PCD)
			ui8Offset = 1;

			// Increment offset if NAD present
			if(ui8PCDByte & ISODEP_I_BLOCK_NAD_MASK)
			{
				ui8Offset++;
			}

			// Increment offset if CID present
			if(ui8PCDByte & ISODEP_I_BLOCK_CID_MASK && (g_bIsoDepCIDEnabled == true))
			{
				if(pui8RxBuffer[ui8Offset] != g_ui8CIDValue)
				{
					g_eCurrentIsoDepState = ISO_DEP_PROTOCOL_ERROR;
				}
				ui8Offset++;
			}
			else if(ui8PCDByte & ISODEP_I_BLOCK_CID_MASK && (g_bIsoDepCIDEnabled == true))
			{
				g_eCurrentIsoDepState = ISO_DEP_PROTOCOL_ERROR;
			}

			// Check if the number of bytes received are less than / equal 256 bytes
			if((uint16_t)(g_ui16IsoDepRxIndex + ui8Length-ui8Offset) <= (256))
			{
				for(ui8Counter=ui8Offset;ui8Counter<ui8Length;ui8Counter++)
				{
					g_pui8IsoDepRXBuffer[g_ui16IsoDepRxIndex++] = pui8RxBuffer[ui8Counter];
				}
			}
			else
			{
				g_eCurrentIsoDepState = ISO_DEP_PROTOCOL_ERROR;
			}

			g_ui16IsoDepTXIndex = 0;

			if(g_bChainingEnable == false)
			{
				ISO_7816_4_processReceivedData(g_pui8IsoDepRXBuffer,g_ui16IsoDepRxIndex);
			}
		}
		else if((ui8PCDByte & 0xF2) == R_ACK_BLOCK)
		{
			// Increment offset if CID present
			if(ui8PCDByte & ISODEP_I_BLOCK_CID_MASK && (g_bIsoDepCIDEnabled == true))
			{
				if(pui8RxBuffer[ui8Offset] != g_ui8CIDValue)
				{
					g_eCurrentIsoDepState = ISO_DEP_PROTOCOL_ERROR;
				}
				else
				{
					ui8Offset++;
				}
			}
			else if(ui8PCDByte & ISODEP_I_BLOCK_CID_MASK && (g_bIsoDepCIDEnabled == true))
			{
				g_eCurrentIsoDepState = ISO_DEP_PROTOCOL_ERROR;
			}

			// Toggle block number if time out count is larger than one
			if(g_ui8IsoDepNACKCount  > 0)
			{
				g_ui8IsoDepACKCount++;

				if(g_ui8IsoDepACKCount > g_ui8ISODepMaxACK)
				{
					g_eCurrentIsoDepState = ISO_DEP_PROTOCOL_ERROR;
				}


				// Request for block repitition
				if(( ui8PCDByte & 0x01) != g_ui8IsoDepBlockNumber)
				{
					g_ui8IsoDepBlockNumber ^= 0x01;
					// Increment the remaining bytes with the previous packet length
					g_ui16IsoDepTxRemaining = g_ui16IsoDepTxRemaining + g_ui16IsoDepPrevTxDataLength;
					// Move the index back to the beginning of the previous packet
					g_ui16IsoDepTXIndex = g_ui16IsoDepTXIndex - g_ui16IsoDepPrevTxDataLength;
				}
			}
			else
			{
				// Clear ACK count
				g_ui8IsoDepACKCount = 0x00;
			}

			// Clear the number of NACK sent
			g_ui8IsoDepNACKCount = 0;

			// We have replied to the LT's I-PDU previously
			if(g_eIsoDepNextPduType == R_ACK_BLOCK)
			{
				if(( ui8PCDByte & 0x01) == g_ui8IsoDepBlockNumber)
				{
					g_eCurrentIsoDepState = ISO_DEP_PROTOCOL_ERROR;
				}
				else
				{
					// Do not change PDU
				}
			}
			// Rule B ISO14443-4 (PCD Rule)
			else if(( ui8PCDByte & 0x01) == g_ui8IsoDepBlockNumber)
			{
				// Toggle Block Number
				g_ui8IsoDepBlockNumber ^= 0x01;

				// We are in the middle of sending a packet
				if(g_ui16IsoDepTxRemaining > 0)
				{
					g_eIsoDepNextPduType = I_BLOCK;
				}
				// If there are no remaining bytes to be sent
				else if(g_eIsoDepNextPduType == R_NACK_BLOCK)
				{
					g_eCurrentIsoDepState = ISO_DEP_PROTOCOL_ERROR;
				}
				else
				{
					g_eIsoDepNextPduType = R_ACK_BLOCK;
				}
			}
			// Rule 6 ISO14443-4 (PCD Rule)
			else
			{
				g_eCurrentIsoDepState = ISO_DEP_PROTOCOL_ERROR;
			}
		}
		else if((ui8PCDByte & 0xF2) == R_NACK_BLOCK)
		{
			g_eCurrentIsoDepState = ISO_DEP_PROTOCOL_ERROR;
		}
		// Rule 3 (General Rule)
		else if((ui8PCDByte & 0xF2) == S_DSL_BLOCK)
		{
			g_eCurrentIsoDepState = ISO_DEP_PROTOCOL_ERROR;
		}
		// Rule 3 (General Rule)
		else if((ui8PCDByte & 0xF2) == S_WTX_BLOCK)
		{
			g_ui8IsoDepWTXCount++;

			if(g_ui8IsoDepWTXCount > g_ui8ISODepMaxWTX)
			{
				g_eCurrentIsoDepState = ISO_DEP_PROTOCOL_ERROR;
			}

			// Increment offset if CID present
			if(ui8PCDByte & ISODEP_I_BLOCK_CID_MASK && (g_bIsoDepCIDEnabled == true))
			{
				if(pui8RxBuffer[ui8Offset] != g_ui8CIDValue)
				{
					g_eCurrentIsoDepState = ISO_DEP_PROTOCOL_ERROR;
				}
				else
				{
					ui8Offset++;
				}
			}
			else if(ui8PCDByte & ISODEP_I_BLOCK_CID_MASK && (g_bIsoDepCIDEnabled == true))
			{
				g_eCurrentIsoDepState = ISO_DEP_PROTOCOL_ERROR;
			}

			if(g_eCurrentIsoDepState != ISO_DEP_PROTOCOL_ERROR)
			{
				g_ui8WTXValue = pui8RxBuffer[ui8Offset+1] & 0x3F;

				if(g_ui8WTXValue < 60 && (g_ui8WTXValue != 0x00))
				{
					g_eIsoDepNextPduType = S_WTX_BLOCK;
				}
				else
				{
					g_eCurrentIsoDepState = ISO_DEP_PROTOCOL_ERROR;
				}
			}
		}
		else
		{
			g_eCurrentIsoDepState = ISO_DEP_PROTOCOL_ERROR;
		}
		break;
	case ISO_DEP_DEACTIVATED_STATE:
		// PCD Byte is the first byte
		ui8PCDByte = pui8RxBuffer[0];

		if((ui8PCDByte & 0xF2) == S_DSL_BLOCK)
		{
			if(ui8PCDByte & 0x04)
			{
				g_eCurrentIsoDepState = ISO_DEP_PROTOCOL_ERROR;
			}
			else
			{
				// Clear the number of DSLs sent
				g_ui8IsoDepDSLCount = 0;

				g_eIsoDepNextPduType = R_NACK_BLOCK;
			}
		}
		else
		{
			g_eCurrentIsoDepState = ISO_DEP_PROTOCOL_ERROR;
		}
		break;
	case ISO_DEP_PROTOCOL_ERROR:
		break;
	}

	return g_eCurrentIsoDepState;
}

uint16_t ISODEP_getLinkTimeOut(void)
{
	uint16_t ui16TempTimeOut;

	if(g_bWTXTimeOut == true)
	{
		g_bWTXTimeOut = false;
		ui16TempTimeOut = g_ui16WtxTimeoOut;
	}
	else
	{
		ui16TempTimeOut = g_ui16IsoDepTimeOut;
	}

	return ui16TempTimeOut;
}

void ISODEP_setLinkTimeOut(uint16_t ui16IsoDepTimeout)
{
	g_ui16IsoDepTimeOut = ui16IsoDepTimeout;
}

void ISODEP_setMaxPayload(uint16_t ui16MaxPayload)
{
	g_ui16ISODEPMaxPayload = ui16MaxPayload;
}

void ISODEP_setNFCType(tIsoDepType eNfcType)
{
	g_eIsoDepType = eNfcType;
}

void ISODEP_sendATS(void)
{
	uint8_t ui8PacketOffset = 1;

	// TO (Format Byte):
	// BIT7 - Reserved for Future Use (RFU)
	// BIT6 - TA(1) Present
	// BIT6 - TB(1) Present
	// BIT4 - TC(1_ Present
	// BIT3:0 - FSCI = 1000, PICC capable of receiving 256 byte frames, from FSDI coding table
	g_pui8IsoDepTXBuffer[ui8PacketOffset++] = 0x78;
	// TA(1) Interface Byte Codes DS and DR ( Default value must be 0x00)
	// BIT7 - Unable to support different divisor for each direction (1= True, 0 = False)
	// BIT6 - DS8 Supported
	// BIT5 - DS4 Supported
	// BIT4 - DS2 Supported
	// BIT3 - Reserved for Future Use (RFU)
	// BIT2 - DR8 Supported
	// BIT1 - DR4 Supported
	// BIT0 - DR2 Supported
	g_pui8IsoDepTXBuffer[ui8PacketOffset++] = 0x80;
	// TB(1) Interface Byte Codes FWI and SFGI
	//
	// BIT7:4 - FWI  => FWT  = (256 x 16 / fc) x 2 ^ FWI (See Section 7.2 ISO14443-4:2000)
	//        - FWI = 4 => FWT = 4.833 mS
	// BIT3:0 - SFGI => SFGT = (256 x 16 / fc) x 2 ^SFGI (See Section 5.2.5 ISO14443-4:2000)
	//        -SFGI = 2 => SFGT = 604 uS (See Section 6.1.4 ISO144443-3:2000)
	g_pui8IsoDepTXBuffer[ui8PacketOffset++] = 0x81;
	// TC(1) Interface Byte Codes Protocol Options (See Section 5.2.6 ISO14443-4:2000)
	//
	// BIT7:2 - Shall be set to (000000)b, all other values are Reserved for Future Use (RFU)
	// BIT1   - CID Supported ( default to 1)
	// BIT0   - NAD Supported ( default to 0)
	g_pui8IsoDepTXBuffer[ui8PacketOffset++] = 0x02;

//	//
//	// Historical Data Byte - See 8ection 8.1.1. ISO7816:2000
//	//
//	// Category Indicator Byte - A status indicator may be present in a COMPACT-TLV data object (one, two, or three bytes)
//	g_pui8IsoDepTXBuffer[ui8PacketOffset++] = 0x80;

	// Length Byte
	g_pui8IsoDepTXBuffer[0] = ui8PacketOffset;

	g_ui16IsoDepTxLength = ui8PacketOffset;

	TRF79x0_writeFIFO(g_pui8IsoDepTXBuffer,CRC_BIT_ENABLE,g_ui16IsoDepTxLength,0);
}

void ISODEP_sendI_Block_REQ(void)
{
	uint8_t ui8PacketOffset = 0;
	uint16_t ui16TempLength = 0;
	uint16_t ui16Counter;

	// Card IDentifier (CID) Optional
	if(g_bIsoDepCIDEnabled)
	{
		// Protocol Control Byte
		g_pui8IsoDepTXBuffer[ui8PacketOffset++] = (uint8_t) I_BLOCK |  g_ui8IsoDepBlockNumber | CID_BIT_MASK;
		g_pui8IsoDepTXBuffer[ui8PacketOffset++] = g_ui8CIDValue;
	}
	else
	{
		// Protocol Control Byte
		g_pui8IsoDepTXBuffer[ui8PacketOffset++] = (uint8_t) I_BLOCK |  g_ui8IsoDepBlockNumber;
	}

	// 7816-4 Handling

	// Node ADdress (NAD) Optional - Not supported

	if(g_ui16IsoDepTxRemaining == 0)
	{
		g_ui16IsoDepTxLength = ui8PacketOffset + ISO_7816_4_stateMachine(&g_pui8IsoDepTXBuffer[ui8PacketOffset],(0xFF - ui8PacketOffset) );

//		switch(eTempStatus)
//		{
//		case ISO_7816_4_CONNECTION_IDLE:
//			break;
//		case ISO_7816_4_CONNECTION_SELECT:
//			g_ui16IsoDepTxLength = ui8PacketOffset + ISO_7816_4_stateMachine(&g_pui8IsoDepTXBuffer[ui8PacketOffset],(0xFF - ui8PacketOffset) );
//			break;
//		case ISO_7816_4_CONNECTION_WAIT_SELECT_RSP:
//			break;
//		case ISO_7816_4_CONNECTION_WRITE:
//			break;
//		case ISO_7816_4_CONNECTION_WAIT_WRITE_RSP:
//			break;
//		case ISO_7816_4_CONNECTION_READ:
//			break;
//		case ISO_7816_4_CONNECTION_WAIT_READ_RSP:
//			break;
//		case ISO_7816_4_TAG_MODE:
//			g_ui16IsoDepTxLength = ui8PacketOffset + ISO_7816_4_stateMachine(&g_pui8IsoDepTXBuffer[ui8PacketOffset],(0xFF - ui8PacketOffset) );
//			break;
//		}
	}
	else
	{
		g_ui16IsoDepTxLength = ui8PacketOffset + g_ui16IsoDepTxRemaining;
	}

	if(g_ui16IsoDepTxLength == ui8PacketOffset)
	{
		// Do a presence check
		ui8PacketOffset = 0;

		// Card IDentifier (CID) Optional
		if(g_bIsoDepCIDEnabled)
		{
			// Protocol Control Byte
			g_pui8IsoDepTXBuffer[ui8PacketOffset++] = (uint8_t) R_NACK_BLOCK |  g_ui8IsoDepBlockNumber | CID_BIT_MASK;
			g_pui8IsoDepTXBuffer[ui8PacketOffset++] = g_ui8CIDValue;
		}
		else
		{
			// Protocol Control Byte
			g_pui8IsoDepTXBuffer[ui8PacketOffset++] = (uint8_t) R_NACK_BLOCK |  g_ui8IsoDepBlockNumber;
		}
	}
	else if(((g_ui16IsoDepTxLength) > (g_ui16ISODEPMaxPayload-2))
			&& (g_ui16IsoDepTxRemaining == 0))
	{
		// Make a copy of the TX buffer inside the RX buffer, in case there is a synchronization error
		memcpy(g_pui8IsoDepRXBuffer,&g_pui8IsoDepTXBuffer[ui8PacketOffset],(g_ui16IsoDepTxLength-ui8PacketOffset));

		if(g_bIsoDepCIDEnabled)
		{
			g_pui8IsoDepTXBuffer[ui8PacketOffset-2] |= ISODEP_I_BLOCK_CHAINING_MASK;
		}
		else
		{
			g_pui8IsoDepTXBuffer[ui8PacketOffset-1] |= ISODEP_I_BLOCK_CHAINING_MASK;
		}

		// Set the length of the data size of the previous packet not including the PCD, CID (optional), and CRC
		g_ui16IsoDepPrevTxDataLength = g_ui16ISODEPMaxPayload - ui8PacketOffset - 2;

		// Remaining number of bytes is equal to the packet length
		// Minus 2 CRC bytes
		g_ui16IsoDepTxRemaining = g_ui16IsoDepTxLength - (g_ui16ISODEPMaxPayload - 2);
		// Subtract the current packet offset and 2 bytes because of the CRC bytes
		g_ui16IsoDepTXIndex = g_ui16ISODEPMaxPayload - 2 - ui8PacketOffset;

		g_ui16IsoDepTxLength = g_ui16ISODEPMaxPayload - 2;
	}
	// We are chaining
	else if(g_ui16IsoDepTxRemaining > 0)
	{
		if(g_ui16IsoDepTxRemaining > (g_ui16ISODEPMaxPayload-ui8PacketOffset-2))
		{
			// We are still chaining
			// Subtract the current packet offset and 2 bytes because of the CRC bytes
			ui16TempLength = g_ui16ISODEPMaxPayload - ui8PacketOffset - 2;
			if(g_bIsoDepCIDEnabled)
			{
				g_pui8IsoDepTXBuffer[ui8PacketOffset-2] |= ISODEP_I_BLOCK_CHAINING_MASK;
			}
			else
			{
				g_pui8IsoDepTXBuffer[ui8PacketOffset-1] |= ISODEP_I_BLOCK_CHAINING_MASK;
			}
		}
		else
		{
			ui16TempLength = g_ui16IsoDepTxRemaining;
		}

		// Clear previous packet length
		g_ui16IsoDepPrevTxDataLength = 0;

		for(ui16Counter = 0; ui16Counter < ui16TempLength; ui16Counter++)
		{
			// Increment TX Index
			g_pui8IsoDepTXBuffer[ui8PacketOffset++] = g_pui8IsoDepRXBuffer[g_ui16IsoDepTXIndex++];
			// Increment the current packet length
			g_ui16IsoDepPrevTxDataLength++;
			// Decrement remaining bytes
			g_ui16IsoDepTxRemaining--;
		}

		g_ui16IsoDepTxLength = ui8PacketOffset;
	}
	else
	{
		if(g_ui16IsoDepTxLength > ui8PacketOffset)
		{
			// Make a copy of the TX buffer inside the RX buffer, in case there is a synchronization error
			memcpy(g_pui8IsoDepRXBuffer,&g_pui8IsoDepTXBuffer[ui8PacketOffset],(g_ui16IsoDepTxLength-ui8PacketOffset));
		}

		// Set the length of the data size of the previous packet not including the PCD, CID (optional), and CRC
		g_ui16IsoDepPrevTxDataLength = g_ui16IsoDepTxLength - ui8PacketOffset;

		// Set the TX index to the length of the previous packet data
		g_ui16IsoDepTXIndex = g_ui16IsoDepPrevTxDataLength;

		g_ui16IsoDepTxRemaining = 0;
	}

	TRF79x0_writeFIFO(g_pui8IsoDepTXBuffer,CRC_BIT_ENABLE,g_ui16IsoDepTxLength,0);
}

void ISODEP_sendR_Block_REQ(void)
{
	uint8_t ui8PacketOffset = 0;

	// Card IDentifier (CID) Optional
	if(g_bIsoDepCIDEnabled)
	{
		// Protocol Control Byte
		g_pui8IsoDepTXBuffer[ui8PacketOffset++] = (uint8_t) g_eIsoDepNextPduType |  g_ui8IsoDepBlockNumber | CID_BIT_MASK;
		g_pui8IsoDepTXBuffer[ui8PacketOffset++] = g_ui8CIDValue;
	}
	else
	{
		// Protocol Control Byte
		g_pui8IsoDepTXBuffer[ui8PacketOffset++] = (uint8_t) g_eIsoDepNextPduType |  g_ui8IsoDepBlockNumber;
	}

	// Node ADdress (NAD) Optional - Not supported

	g_ui16IsoDepTxLength = ui8PacketOffset;

	TRF79x0_writeFIFO(g_pui8IsoDepTXBuffer,CRC_BIT_ENABLE,g_ui16IsoDepTxLength,0);
}

void ISODEP_sendS_Block_REQ(void)
{
	uint8_t ui8PacketOffset = 0;
	uint32_t ui32TempWTXTimeout;

	// Protocol Control Byte
	if (g_bIsoDepCIDEnabled)
	{
		g_pui8IsoDepTXBuffer[ui8PacketOffset++] = (uint8_t) g_eIsoDepNextPduType | CID_BIT_MASK;
		g_pui8IsoDepTXBuffer[ui8PacketOffset++] = g_ui8CIDValue;
	}
	else
	{
		g_pui8IsoDepTXBuffer[ui8PacketOffset++] = (uint8_t) g_eIsoDepNextPduType;
	}

	if(g_eIsoDepNextPduType == S_WTX_BLOCK)
	{
		//
		g_pui8IsoDepTXBuffer[ui8PacketOffset++] = g_ui8WTXValue;

		g_bWTXTimeOut = true;

	    // Calculate and set the correct timeout value: RWTint + Delta RWT (RWTint = RWT*RTOX)
		// Also have to ensure that this value doesn't exceed RWTmax = (256*16/fc) * 2^WTmax
		ui32TempWTXTimeout = (uint32_t) (g_ui16IsoDepTimeOut * (uint16_t) g_ui8WTXValue);

		if (ui32TempWTXTimeout <= FWT_INT_MAX_MS)
		{
			g_ui16WtxTimeoOut = (uint16_t) ui32TempWTXTimeout;
		}
		else
		{
			g_ui16WtxTimeoOut = FWT_INT_MAX_MS;
		}
	}
	else if (g_eIsoDepNextPduType == S_DSL_BLOCK)
	{
		// Sent Deslect Block
		g_eCurrentIsoDepState = ISO_DEP_DEACTIVATED_STATE;
	}
	else
	{
		// Do nothing
	}

	g_ui16IsoDepTxLength = ui8PacketOffset;

	TRF79x0_writeFIFO(g_pui8IsoDepTXBuffer,CRC_BIT_ENABLE,g_ui16IsoDepTxLength,0);
}

void ISODEP_sendPPS_RES(void)
{
	uint8_t ui8PacketOffset = 0;

	// Protocol Control Byte
	g_pui8IsoDepTXBuffer[ui8PacketOffset++] = PPS_CMD | g_ui8CIDValue;

	g_ui16IsoDepTxLength = ui8PacketOffset;

	TRF79x0_writeFIFO(g_pui8IsoDepTXBuffer,CRC_BIT_ENABLE,g_ui16IsoDepTxLength,0);
}

void ISODEP_sendDSL_RES(void)
{
	uint8_t ui8PacketOffset = 0;

	// Protocol Control Byte
	if (g_bIsoDepCIDEnabled)
	{
		g_pui8IsoDepTXBuffer[ui8PacketOffset++] = 0xC2 | CID_BIT_MASK;
		g_pui8IsoDepTXBuffer[ui8PacketOffset++] = g_ui8CIDValue;
	}
	else
	{
		g_pui8IsoDepTXBuffer[ui8PacketOffset++] = 0xC2;
	}

	g_ui16IsoDepTxLength = ui8PacketOffset;

	TRF79x0_writeFIFO(g_pui8IsoDepTXBuffer,CRC_BIT_ENABLE,g_ui16IsoDepTxLength,0);
}
void ISODEP_sendWTX_REQ(void);
void ISODEP_sendWTX_RES(void);

// This function is used in order to keep track of how many protocol errors have occured.
// A maximum number of protocol errors will be entered during initialization.
// When the maximum number of protocol errors is reached, the software will reset the part.
// Otherwise, it will send a NACK PDU.
// This occurs during the NFC_P2P_DATA_EXCHANGE_PROTOCOL state.

uint8_t ISODEP_triggerProtocolError(void)
{
	uint8_t ui8Status;

	ui8Status = STATUS_SUCCESS;

	if(g_eIsoDepNextPduType == S_DSL_BLOCK)
	{
		g_ui8IsoDepDSLCount = g_ui8IsoDepDSLCount + 1;

		if(g_ui8IsoDepDSLCount > g_ui8ISODepMaxDSL)
		{
			ui8Status = STATUS_FAIL;
		}
	}
	else
	{
		g_ui8IsoDepNACKCount = g_ui8IsoDepNACKCount + 1;

		if(g_eIsoDepNextPduType == I_BLOCK || g_eIsoDepNextPduType == S_WTX_BLOCK)
		{
			g_eIsoDepNextPduType = R_NACK_BLOCK;
		}

		if(g_ui8IsoDepNACKCount > g_ui8ISODepMaxNACK)
		{
			ui8Status = STATUS_FAIL;
		}
	}

	return ui8Status;
}


uint8_t ISODEP_getCID(void)
{
	return g_ui8CIDValue;
}

void ISODEP_sendDeselect(void)
{
	g_eIsoDepNextPduType = S_DSL_BLOCK;
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
