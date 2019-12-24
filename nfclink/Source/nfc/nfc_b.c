//*****************************************************************************
//
// nfc_b.c - Implementation of ISO14443-B APIs.
//
// Copyright (c) 2015 Texas Instruments Incorporated.  All rights reserved.
// TI Information - Selective Disclosure
//
//*****************************************************************************

#include "nfc_b.h"

//*****************************************************************************
//
//! \addtogroup nfc_b_api NFC B API Functions
//! @{
//! NFC B is a NFC Layer used by NFC_Initiator and NFC_Target layers to
//! activate the NFC-B technology. For more information on NFC-B please
//! read the NFC Digital Specification Version 1.1.
//
//*****************************************************************************

//*****************************************************************************
//
// Stores the NFC-B ID in both modes as a PICC or when reading a tag
// when the transceiver is a PCD. (both PICC and PCD modes)
//
//*****************************************************************************
static uint8_t g_pui8NfcBId[4];

//*****************************************************************************
//
// Stores the NFC-B SENSB Response received from the PICC when the transceiver
// is PCD mode. (PCD mode)
//
//*****************************************************************************
static uint8_t g_ui8SENSB_Res[13];

//*****************************************************************************
//
// Stores the CID Value used in PCD mode. (PCD mode)
//
//*****************************************************************************
static uint8_t g_ui8NfcBCIDValue;

//*****************************************************************************
//
// Stores the time (mS) the transceiver should wait before sending a command to
// the PICC. (PCD mode)
//
//*****************************************************************************
static uint16_t g_ui16NfcBSfgt;

//*****************************************************************************
//
// Stores the max timeout (mS) the transceiver should wait for a PICC response.
// (PCD mode)
//
//*****************************************************************************
static uint16_t g_ui16NfcBFwt;

//*****************************************************************************
//
// Stores the current state of the RW statemachine. (PCD mode)
//
//*****************************************************************************
static tNfcBPollState g_sNfcBPollState;

//*****************************************************************************
//
// Stores the if the PICC ready by the PCD is ISO DEP compliant. (PCD mode)
//
//*****************************************************************************
static bool g_bNfcBIsoCompliant;

//*****************************************************************************
//
// Stores the number of bytes that can sent to the PICC. (PCD mode)
//
//*****************************************************************************
static uint16_t g_ui16NfcBFsd;

//*****************************************************************************
//
// Stores the current RW Bitrate. (PCD mode)
//
//*****************************************************************************
static t_sTRF79x0_Bitrate g_sRwInitBitrates;

//*****************************************************************************
//
//! NFC_B_init - Initialize the NFC B driver.(PCD and PICC modes)
//!
//! This function must be called prior to any other function offered by the
//! NFC B driver. This function initializes the NFC DEP and ISO support for
//! PICC and PCD, the length of the tag, driver current state, CID, and SAK
//! response.
//!
//! \return None.
//
//*****************************************************************************
void NFC_B_init(void)
{
	g_ui8NfcBCIDValue = 0x00;

	g_ui16NfcBSfgt = 0x00;

	g_ui16NfcBFwt = 0x00;

	g_ui16NfcBFsd = 0x00;

	g_bNfcBIsoCompliant = false;

	g_sNfcBPollState = NFC_B_POLL_SENSB_REQ;
}

//*****************************************************************************
//
//! NFC_B_send_SENSB_REQ - Sends the SENSB REQ to the PICC. (PCD mode)
//!
//! This function sends an SENSB Request - first PCD command used to activate
//! NFC-B tags.
//!
//! \return None.
//
//*****************************************************************************
void NFC_B_send_SENSB_REQ(void)
{
	uint8_t ui8NfcBPacket[3];
	uint8_t ui8Offset = 0x00;

	// SENSB/ALLB REQ
	ui8NfcBPacket[ui8Offset++] = SENSB_REQ_CMD;
	// AFI - 0x00 (all application families supported)
	ui8NfcBPacket[ui8Offset++] = 0x00;
	// PARASM -
	// BIT7:5 - 000 (RFU)
	// BIT4 extended SENSB_RES not supported
	// BIT3 - ALLREQ (1) or SENSB_REQ (0)
	// BIT2:0 - NI (single slot
	ui8NfcBPacket[ui8Offset++] = 0x00;

	//Send SENSB_REQ Packet and wait for TX Complete
	TRF79x0_writeFIFO(ui8NfcBPacket, CRC_BIT_ENABLE, ui8Offset,0);

	g_sNfcBPollState = NFC_B_POLL_SENSB_REQ;
}

//*****************************************************************************
//
//! NFC_B_send_ALLB_REQ - Sends the ALLB REQ to the PICC. (PCD mode)
//!
//! This function sends an ALLB Request - first PCD command used to wake / activate
//! NFC-B tags.
//!
//! \return None.
//
//*****************************************************************************
void NFC_B_send_ALLB_REQ(void)
{
	uint8_t ui8NfcBPacket[3];
	uint8_t ui8Offset = 0x00;

	// SENSB/ALLB REQ
	ui8NfcBPacket[ui8Offset++] = SENSB_REQ_CMD;
	// AFI - 0x00 (all application families supported)
	ui8NfcBPacket[ui8Offset++] = 0x00;
	// PARASM -
	// BIT7:5 - 000 (RFU)
	// BIT4 extended SENSB_RES not supported
	// BIT3 - ALLREQ (1) or SENSB_REQ (0)
	// BIT2:0 - NI (single slot
	ui8NfcBPacket[ui8Offset++] = 0x08;

	//Send ALLB_REQ Packet and wait for TX Complete
	TRF79x0_writeFIFO(ui8NfcBPacket, CRC_BIT_ENABLE, ui8Offset,0);

	g_sNfcBPollState = NFC_B_POLL_SENSB_REQ;
}

//*****************************************************************************
//
//! NFC_B_send_SLPB_REQ - Sends the SLPB REQ to the PICC. (PCD mode)
//!
//! This function sends a SLPB Request to the PCD.
//!
//! \return None.
//
//*****************************************************************************
void NFC_B_send_SLPB_REQ(void)
{
	uint8_t ui8NfcBPacket[5];
	uint8_t ui8Offset = 0x00;

	// SLPB REQ
	ui8NfcBPacket[ui8Offset++] = SLPB_REQ_CMD;
	// NFCID0 (4 bytes)
	ui8NfcBPacket[ui8Offset++] = g_pui8NfcBId[0];
	ui8NfcBPacket[ui8Offset++] = g_pui8NfcBId[1];
	ui8NfcBPacket[ui8Offset++] = g_pui8NfcBId[2];
	ui8NfcBPacket[ui8Offset++] = g_pui8NfcBId[3];

	//Send SLPB_REQ Packet and wait for TX Complete
	TRF79x0_writeFIFO(ui8NfcBPacket, CRC_BIT_ENABLE, ui8Offset,0);
}

//*****************************************************************************
//
//! NFC_B_send_ATTRIB_REQ - Sends the ATTRIB REQ to the PICC. (PCD mode)
//!
//! This function sends a ATTRIB Request to the PCD.
//!
//! \return None.
//
//*****************************************************************************
void NFC_B_send_ATTRIB_REQ(void)
{
	uint8_t ui8NfcBPacket[9];
	t_sNfcRWCommBitrate sRwBitrates;
	uint8_t ui8Offset = 0x00;

	// ATTRIB REQ
	ui8NfcBPacket[ui8Offset++] = ATTRIB_CMD;
	// NFCID0 (4 bytes)
	ui8NfcBPacket[ui8Offset++] = g_pui8NfcBId[0];
	ui8NfcBPacket[ui8Offset++] = g_pui8NfcBId[1];
	ui8NfcBPacket[ui8Offset++] = g_pui8NfcBId[2];
	ui8NfcBPacket[ui8Offset++] = g_pui8NfcBId[3];

	// Param 1
	// Bits 7:6 Minimum TR0 (default = 00b)
	// Bits 5:4 Minimum TR1 (default = 00b)
	// Bit 3 Suppression of EoS (EoS needed = 0)
	// Bit 2 Suppression of EoS (EoS needed = 0)
	// Bits 1:0 RFU (00b)
	ui8NfcBPacket[ui8Offset++] = 0x00;

	// Param 2
	// Bits 7:6 Bit rate Listen --> Poll
	// Bits 5:4 Bit rate Poll --> Listen
	// Bits 3:0 FSDI (256 bytes = 1000b)

	//  FSDI Configuration
	ui8NfcBPacket[ui8Offset] = 0x08;

	sRwBitrates = NFC_RW_getRWSuportedBitrates();

	// Check if same bitrate is supported

	// Check for 848kbps support
	if(g_ui8SENSB_Res[SENSB_RES_BIT_RATE_INDEX] & NFC_B_PICC_TO_PCD_848_KBPS
		&& g_ui8SENSB_Res[SENSB_RES_BIT_RATE_INDEX] & NFC_B_PCD_TO_PICC_848_KBPS
		&& sRwBitrates.bits.bNfcB_848kbps == 1)
	{
		ui8NfcBPacket[ui8Offset] |= 0xF0;
		g_sRwInitBitrates.ui16word = 0x00;
		g_sRwInitBitrates.bits.b848kpbs = 1;
	}
	// Check for 424kbps support
	else if(g_ui8SENSB_Res[SENSB_RES_BIT_RATE_INDEX] & NFC_B_PICC_TO_PCD_424_KBPS
			&& g_ui8SENSB_Res[SENSB_RES_BIT_RATE_INDEX] & NFC_B_PCD_TO_PICC_424_KBPS
			&& sRwBitrates.bits.bNfcB_424kbps == 1)
	{
		ui8NfcBPacket[ui8Offset] |= 0xA0;
		g_sRwInitBitrates.ui16word = 0x00;
		g_sRwInitBitrates.bits.b424kpbs = 1;
	}
	// Check for 212kbps support
	else if(g_ui8SENSB_Res[SENSB_RES_BIT_RATE_INDEX] & NFC_B_PICC_TO_PCD_212_KBPS
			&& g_ui8SENSB_Res[SENSB_RES_BIT_RATE_INDEX] & NFC_B_PCD_TO_PICC_212_KBPS
			&& sRwBitrates.bits.bNfcB_212kbps == 1)
	{
		ui8NfcBPacket[ui8Offset] |= 0x50;
		g_sRwInitBitrates.ui16word = 0x00;
		g_sRwInitBitrates.bits.b212kpbs = 1;
	}
	else
	{
		// Stay @ 106kbps
		ui8NfcBPacket[ui8Offset] |= 0x00;
		g_sRwInitBitrates.ui16word = 0x00;
		g_sRwInitBitrates.bits.b106kpbs = 1;
	}

	// Increment Offset
	ui8Offset++;

	// Param 3
	// Bits 7:4 Fixed value (0000b)
	// Bit 3 RFU (0)
	// Bit 2:1 Minimum TR2 (based on the SENSB_RES)
	// Bit 0 (based on the SENSB_RES)
	ui8NfcBPacket[ui8Offset++] = (g_ui8SENSB_Res[SENSB_RES_MAX_FRAME_PROT_INDEX] & 0x07);

	g_ui8NfcBCIDValue = ISODEP_getCID();

	// Param 4
	// Bits 7:4 RFU (0000b)
	// Bits 3:0 DID
	ui8NfcBPacket[ui8Offset++] = g_ui8NfcBCIDValue & (0x0F);

	//Send ATTRIB_REQ Packet and wait for TX Complete
	TRF79x0_writeFIFO(ui8NfcBPacket, CRC_BIT_ENABLE, ui8Offset,0);

	g_sNfcBPollState = NFC_B_POLL_ATTRIB_REQ;
}

//*****************************************************************************
//
//! NFC_B_stateMachine - This function handles the activation of the PICC. (PCD Mode)
//!
//! This function sends the SENSB Req, SLPB REQ and ATTRIB REQ.
//!
//! \return None.
//
//****************************************************************************
void NFC_B_stateMachine(void)
{
	switch(g_sNfcBPollState)
	{
	case NFC_B_POLL_SENSB_REQ:
		NFC_B_send_SENSB_REQ();
		break;
	case NFC_B_POLL_SLPB_REQ:
		// Awaiting SLPB_RES
		break;
	case NFC_B_POLL_ATTRIB_REQ:
		NFC_B_send_ATTRIB_REQ();
		break;
	case NFC_B_POLL_ACTIVATED:
		break;
	case NFC_B_POLL_WAKEUP:
		break;
	default:
		g_sNfcBPollState = NFC_B_PROTOCOL_ERROR;
		break;
	}
}

//*****************************************************************************
//
//! NFC_B_processReceivedRsp - This function processes the response from the
//! PICC. (PCD Mode)
//!
//! \param pui8RxBuffer is a pointer to the recevied data.
//! \param ui8Length is the length of received response.
//!
//! This function processes the response to the SENSB REQ and ATTRIB REQ commands.
//!
//! \return g_sNfcBPollState the current NFC-B state.
//
//****************************************************************************
tNfcBPollState NFC_B_processReceivedRsp(uint8_t * pui8RxBuffer, uint8_t ui8Length)
{
	uint8_t ui8Offset = 0;
	uint32_t ui32TempTimeout = 0;
	uint8_t ui8Fsci;

	switch(g_sNfcBPollState)
	{
	case NFC_B_POLL_SENSB_REQ:
		// Awaiting SENSB_RES
		if(ui8Length == 12 || ui8Length == 13)
		{
			// Copy the SENSB_RES
			for(ui8Offset=0;ui8Offset<ui8Length;ui8Offset++)
			{
				g_ui8SENSB_Res[ui8Offset] = pui8RxBuffer[ui8Offset];
			}

			// Copy the SENSB_ID
			for(ui8Offset=0;ui8Offset<4;ui8Offset++)
			{
				g_pui8NfcBId[ui8Offset] = g_ui8SENSB_Res[ui8Offset+1];
			}

			// Application Data Parsing (Bytes 6-9)
			// Protocol Info Format (Bytes 10-12 or 13)
			// Byte 10 - Bit-Rate Capability (Handled in ATTRIB)

			// Byte 11 - FSCI (Max Payload) and Protocol Type

			ui8Fsci = (g_ui8SENSB_Res[10] >> 4) & 0x0F;

			switch(ui8Fsci)
			{
				case 0:
					g_ui16NfcBFsd = 16;
					break;
				case 1:
					g_ui16NfcBFsd = 24;
					break;
				case 2:
					g_ui16NfcBFsd = 32;
					break;
				case 3:
					g_ui16NfcBFsd = 40;
					break;
				case 4:
					g_ui16NfcBFsd = 48;
					break;
				case 5:
					g_ui16NfcBFsd = 64;
					break;
				case 6:
					g_ui16NfcBFsd = 96;
					break;
				case 7:
					g_ui16NfcBFsd = 128;
					break;
				case 8:
					g_ui16NfcBFsd = 256;
					break;
				default:
					g_ui16NfcBFsd = 256;
					break;
			}

			// TR2 Handling
			g_ui16NfcBSfgt = 1;

			// Check if tag is compliant with ISO14443-4
			if(g_ui8SENSB_Res[10] & 0x01)
			{
				g_bNfcBIsoCompliant = true;
			}
			else
			{
				g_bNfcBIsoCompliant = false;
			}

			// Byte 12 FWI and ADC Handling
			if((g_ui8SENSB_Res[11] & 0xF0) == 0xF0)
			{
				ui32TempTimeout = 0x04;
			}
			else
			{
				ui32TempTimeout = (uint32_t)(g_ui8SENSB_Res[11] & 0xF0);
				// The value is in upper nibble, therefeore we need to shift right by 4
				ui32TempTimeout = (ui32TempTimeout >> 4);
			}
			// Calculate 2 ^ FWI
			ui32TempTimeout = (1 << ui32TempTimeout);

			// Set Max timeout

			// FWT = (16*256/fc)*2^(FWI)+T_POLL(20ms)
			ui32TempTimeout = ui32TempTimeout * 30;
			ui32TempTimeout = ui32TempTimeout / 100;
			ui32TempTimeout = ui32TempTimeout + 20;

			g_ui16NfcBFwt = (uint16_t)ui32TempTimeout;

			if(ui8Length == 13)
			{
				ui32TempTimeout = g_ui8SENSB_Res[12] & 0xF0;
				ui32TempTimeout = (ui32TempTimeout >> 4);

				// Check for valid SFGI
				if(ui32TempTimeout > 0 && ui32TempTimeout < 15)
				{
					// Calculate 2 ^ FWI
					ui32TempTimeout = (1 << ui32TempTimeout);

					// Set Max timeout

					// FWT = (16*256/fc)*2^(FWI)+T_POLL(20ms)
					ui32TempTimeout = ui32TempTimeout * 30;
					ui32TempTimeout = ui32TempTimeout / 100;

					g_ui16NfcBSfgt = (uint16_t) ui32TempTimeout;
				}
			}

			// Set ISO-DEP max payload
			ISODEP_setMaxPayload(g_ui16NfcBFsd);

			// Set ISO-DEP timeout
			ISODEP_setLinkTimeOut(g_ui16NfcBFwt);

			ISODEP_setNFCType(ISO_DEP_B);

			g_sNfcBPollState = NFC_B_POLL_ATTRIB_REQ;
		}
		else
		{
			g_sNfcBPollState = NFC_B_PROTOCOL_ERROR;
		}
		break;
	case NFC_B_POLL_SLPB_REQ:
		// Awaiting SLPB_RES
		break;
	case NFC_B_POLL_ATTRIB_REQ:
		// Awaiting ATTRIB_RES
		if(( pui8RxBuffer[0] & 0x0F) != g_ui8NfcBCIDValue)
		{
			g_sNfcBPollState = NFC_B_PROTOCOL_ERROR;
		}
		else
		{
			TRF79x0_incrementInitiatorBaudRate(g_sRwInitBitrates);
			g_sNfcBPollState = NFC_B_POLL_ACTIVATED;
		}
		break;
	case NFC_B_POLL_ACTIVATED:

		break;
	default:
		g_sNfcBPollState = NFC_B_PROTOCOL_ERROR;
		break;
	}

	return g_sNfcBPollState;
}

//*****************************************************************************
//
//! NFC_B_send_SENSB_RES - Sends the SENSB RES to the PCD. (PICC Mode)
//!
//! This function sends a SENSB Response to the PCD including the UID and
//! NFC B capabilitiy information.
//!
//! \return None.
//
//*****************************************************************************
void NFC_B_send_SENSB_RES(void)
{
	uint8_t nfc_b_packet[12];
	uint8_t ui8Offset = 0x00;

	// SENSB Response Packet
	nfc_b_packet[ui8Offset++] = SENSB_RES_CMD;
	// NFCID 3:0
	nfc_b_packet[ui8Offset++] = g_pui8NfcBId[0];
	nfc_b_packet[ui8Offset++] = g_pui8NfcBId[1];
	nfc_b_packet[ui8Offset++] = g_pui8NfcBId[2];
	nfc_b_packet[ui8Offset++] = g_pui8NfcBId[3];

	// Application Data Bytes
	nfc_b_packet[ui8Offset++] = 0x00; // AFI - Public Telephony, GSM ... ( See ISO14443-3 Table 12 AFI Coding)
	nfc_b_packet[ui8Offset++] = 0x00; // CRC_B
	nfc_b_packet[ui8Offset++] = 0x00; // CRC_B
	nfc_b_packet[ui8Offset++] = 0x11; // # of applications (1)

	// Protocol Info Bytes
	nfc_b_packet[ui8Offset++] = 0x00; // Date Rate Capability ( Only Support 106 kbps)
	// Max Frame/Protocol type
	// Bits 8:5 - 256 bytes
	// Bits 4   - RFU = 0
	// Bits 3:2 - Minimum TR2 10 etu + 256 /fs
	// Bit  1   - PICC compliant to 14443-4)
	nfc_b_packet[ui8Offset++] = 0x81;
	// (FWI/ADC/FO)
	// Bits 8:5 - FWT = (256 x 16 / fc) x 2^FWI  = 77.3mSec
	// Bits 4:3 - ADC = coded according to AFI
	// Bits 2:1 - NAD not supported, CID supported
	nfc_b_packet[ui8Offset++] = 0x85;

	TRF79x0_writeFIFO(nfc_b_packet, CRC_BIT_ENABLE, ui8Offset,0); //Send ATQB Packet and wait for TX Complete
}

//*****************************************************************************
//
//! NFC_B_send_SLPB_RES - Sends the SLPB RES to the PCD. (PICC Mode)
//!
//! This function sends a SLPB Response to the PCD to acknowledge the SLPB REQ.
//!
//! \return None.
//
//*****************************************************************************
void NFC_B_send_SLPB_RES(void)
{
	uint8_t nfc_b_packet[1];
	uint8_t ui8Offset = 0x00;

	// SENSB Response Packet
	nfc_b_packet[ui8Offset++] = SLPB_RES_CMD;

	TRF79x0_writeFIFO(nfc_b_packet, CRC_BIT_ENABLE, ui8Offset,0); //Send ATQB Packet and wait for TX Complete
}

//*****************************************************************************
//
//! NFC_B_send_ATTRIB_RES - Sends the ATTRIB RES to the PCD. (PICC Mode)
//!
//! This function sends a ATTRIB Response to the PCD to acknowledge the ATTRIB REQ.
//!
//! \return None.
//
//*****************************************************************************
void NFC_B_send_ATTRIB_RES(void)
{
	uint8_t nfc_b_packet[1];
	uint8_t ui8Offset = 0x00;

	// ATTRIB Response Packet
	nfc_b_packet[ui8Offset++] = 0x00;

	TRF79x0_writeFIFO(nfc_b_packet, CRC_BIT_ENABLE, ui8Offset,0); //Send ATQB Packet and wait for TX Complete
}

//*****************************************************************************
//
//! NFC_B_getSENSBN - Returns the SENSB N from the SENSB request. (PICC mode)
//!
//! \param pui8Payload is the pointer to the start of the SENSB REQ.
//! \param ui8Length is the length of the SENSB REQ.
//!
//! This function returns the N paramater from the SENSB Request.
//!
//! \return ui8NValue is the N paramater from the SENSB Request. .
//
//*****************************************************************************
uint8_t NFC_B_getSENSBN(uint8_t * pui8Payload, uint8_t ui8Length)
{
	uint8_t ui8NValue;

	if(ui8Length == 0x03)
	{
		ui8NValue = pui8Payload[2] & 0x07;
	}
	else
		ui8NValue = 0xFF;

	return ui8NValue;
}

//*****************************************************************************
//
//! NFC_B_checkSENSBAfi - Check if the AFI from the SENSB Request is correct.
//! (PICC mode)
//!
//! \param pui8Payload is the pointer to the start of the SENSB REQ.
//! \param ui8Length is the length of the SENSB REQ.
//!
//! This function checks if the AFI paramater from the SENSB Request is 0x00.
//!
//! \return ui8Status is STATUS_SUCCESS if the AFI value is 0x00, otherwise
//! it returns STATUS_FAIL.
//
//*****************************************************************************
uint8_t NFC_B_checkSENSBAfi(uint8_t * pui8Payload, uint8_t ui8Length)
{
	uint8_t ui8Status;

	// Check if AFI is correct
	if(pui8Payload[1] == 0x00)
	{
		ui8Status = STATUS_SUCCESS;
	}
	else
		ui8Status = STATUS_FAIL;

	return ui8Status;
}

//*****************************************************************************
//
//! NFC_B_checkAttribId - Check if the ID number from the ATTRIB Request is
//! correct. (PICC mode)
//!
//! \param pui8Payload is the pointer to the start of the ATTRIB REQ.
//! \param ui8Length is the length of the ATTRIB REQ.
//!
//! This function checks if the ID the ATTRIB command corresponds to
//! the emulated PICC.
//!
//! \return ui8Status is STATUS_SUCCESS if the ID is valid, otherwise
//! it returns STATUS_FAIL.
//
//*****************************************************************************
uint8_t NFC_B_checkAttribId(uint8_t * pui8Payload, uint8_t ui8Length)
{
	uint8_t ui8Status;

	// Check if the NFCID0 is correct
	if(   pui8Payload[1] == g_pui8NfcBId[0]
	   && pui8Payload[2] == g_pui8NfcBId[1]
	   && pui8Payload[3] == g_pui8NfcBId[2]
	   && pui8Payload[4] == g_pui8NfcBId[3])
	{
		ui8Status = STATUS_SUCCESS;
	}
	else
		ui8Status = STATUS_FAIL;

	return ui8Status;
}

//*****************************************************************************
//
//! NFC_B_checkSLPBId - Check if the ID number from the SLP Request is correct.
//! (PICC mode)
//!
//! \param pui8Payload is the pointer to the start of the SLPB REQ.
//! \param ui8Length is the length of the SLPB REQ.
//!
//! This function checks if the ID the SLPB command corresponds to
//! the emulated PICC.
//!
//! \return ui8Status is STATUS_SUCCESS if the ID is valid, otherwise
//! it returns STATUS_FAIL.
//
//*****************************************************************************
uint8_t NFC_B_checkSLPBId(uint8_t * pui8Payload, uint8_t ui8Length)
{
	uint8_t ui8Status;

	// Check if the NFCID0 is correct
	if(   pui8Payload[1] == g_pui8NfcBId[0]
	   && pui8Payload[2] == g_pui8NfcBId[1]
	   && pui8Payload[3] == g_pui8NfcBId[2]
	   && pui8Payload[4] == g_pui8NfcBId[3])
	{
		ui8Status = STATUS_SUCCESS;
	}
	else
		ui8Status = STATUS_FAIL;

	return ui8Status;
}

//*****************************************************************************
//
//! NFC_B_checkAttribParam - Check if the param values from the ATTRIB Request
//! are correct. (PICC mode)
//!
//! \param pui8Payload is the pointer to the start of the ATTRIB REQ.
//! \param ui8Length is the length of the ATTRIB REQ.
//!
//! This function checks if the params from the ATTRIB command are valid.
//! This function handles test cases BI_08_10 and BI_08_13
//!
//! \return ui8Status is STATUS_SUCCESS if the params are valid, otherwise
//! it returns STATUS_FAIL.
//
//*****************************************************************************
uint8_t NFC_B_checkAttribParam(uint8_t * pui8Payload, uint8_t ui8Length)
{
	uint8_t ui8Status;

	// Check if the NFCID0 is correct
	if((pui8Payload[6] & 0xC0) == 0x00 && // Added to deal with BI_08_10
			(pui8Payload[7] & 0x0F) != 0x0F && // Added to deal with BI_08_12
			(pui8Payload[8] & 0x0F) != 0x0F ) // Added to deal with BI_08_13
	{
		ui8Status = STATUS_SUCCESS;
	}
	else
		ui8Status = STATUS_FAIL;

	return ui8Status;
}

//*****************************************************************************
//
//! NFC_B_setNfcBId - This function sets the UID of the NFC-B when PICC mode is
//! activated. (PICC Mode)
//!
//! \param ui8NewNfcAId is the pointer to the NFCBId.
//! \param ui8Length is the length of the UID. (Must be 4)
//!
//! This function sets the ID and the size of the PICC UID.
//!
//! \return ui8Status when the length of the UID is 4 bytes, else it returns
//! STATUS_FAIL.
//
//****************************************************************************
uint8_t NFC_B_setNfcBId(uint8_t * ui8NewNfcBId, uint8_t ui8Length)
{
	uint8_t ui8Status = STATUS_FAIL;
	int count;

	if (ui8Length == 4)
	{
		for (count = 0; count < 4; count++)
		{
			g_pui8NfcBId[count] = ui8NewNfcBId[count];	// Re-Write ISO14443 T4TA Tag
		}
		ui8Status = STATUS_SUCCESS;
	}
	else
	{
		// Error - Set to default
		g_pui8NfcBId[0] = 0x08;
		g_pui8NfcBId[1] = 0x11;
		g_pui8NfcBId[2] = 0x22;
		g_pui8NfcBId[3] = 0x33;
		ui8Status = STATUS_FAIL;
	}
	return ui8Status;
}

//*****************************************************************************
//
//! NFC_B_getNfcBId - This function returns the NFC-B UID (PICC or PCD Mode)
//!
//! \param pui8NfcBId is a double pointer to be modified with the NFCBId buffer.
//! \param ui8NfcBIdLength is a pointer to store the NFC ID Length.
//!
//! This function returns the NFC-B UID.
//!
//! \return None.
//
//****************************************************************************
void NFC_B_getNfcBId(uint8_t ** pui8NfcBId, uint8_t * ui8NfcBIdLength)
{
	*pui8NfcBId = g_pui8NfcBId;

	*ui8NfcBIdLength = 4;
}

//*****************************************************************************
//
//! NFC_B_getSFGT - This function returns the g_ui16NfcBSfgt (PCD Mode)
//!
//! This function returns the time (mS) the transceiver must wait before
//! sending a command to the PICC.
//!
//! \return g_ui16NfcBSfgt is the guard time the PCD must wait.
//
//****************************************************************************
uint16_t NFC_B_getSFGT(void)
{
	return g_ui16NfcBSfgt ;
}

//*****************************************************************************
//
//! NFC_B_getFWT - This function returns the g_ui16NfcBFwt (PCD Mode)
//!
//! This function returns max timeout (mS) the transceiver should wait for
//! a PICC response.
//!
//! \return g_ui16NfcBFwt is the max timeout time the PCD must wait for a
//! response.
//
//****************************************************************************
uint16_t NFC_B_getFWT(void)
{
	return g_ui16NfcBFwt;
}

//*****************************************************************************
//
//! NFC_B_checkDIDSupported - This function returns if the PICC supports DID.
//! (PCD Mode)
//!
//! This function returns if the PICC supports DID.
//!
//! \return a boolean flag true if the the PICC supports DID, otherwise false.
//
//****************************************************************************
bool NFC_B_checkDIDSupported(void)
{
	return (g_ui8SENSB_Res[11] & 0x01);
}

//*****************************************************************************
//
//! NFC_B_isSelected - This function returns if the PICC is activated.
//! (PCD Mode)
//!
//! This function returns if the PICC has been activated.
//!
//! \return a boolean flag true if the the PICC is activated, otherwise false.
//
//****************************************************************************
bool NFC_B_isSelected(void)
{
	return (g_sNfcBPollState == NFC_B_POLL_ACTIVATED);
}

//*****************************************************************************
//
//! NFC_B_isISOCompliant - This function returns if the PICC is ISO compliant.
//! (PCD Mode)
//!
//! This function returns if the PICC is ISO compliant.
//!
//! \return a boolean flag true if the the PICC is ISO compliant, otherwise false.
//
//****************************************************************************
bool NFC_B_isISOCompliant(void)
{
	return g_bNfcBIsoCompliant;
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
