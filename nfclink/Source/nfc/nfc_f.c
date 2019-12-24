//*****************************************************************************
//
// nfc_f.c - contains implementation of NFC Type F (Felica) protocol
//
// Copyright (c) 2015 Texas Instruments Incorporated.  All rights reserved.
// TI Information - Selective Disclosure
//
//*****************************************************************************

#include "nfc_f.h"

//*****************************************************************************
//
//! \addtogroup nfc_f_api NFC F API Functions
//! @{
//! NFC F is a NFC Layer used by NFC_Initiator and NFC_Target layers to
//! activate the NFC-F technology. For more information on NFC-F please
//! read the NFC Digital Specification Version 1.1.
//
//*****************************************************************************

//*****************************************************************************
//
// NFC ID for TYPE F cards
//
//*****************************************************************************
static uint8_t g_pui8NfcFId2[8];

//*****************************************************************************
//
// Stores the NFC DEP support.
//
//*****************************************************************************
static NfcFStatus g_sNfcFStatus;

//*****************************************************************************
//
//! NFC_F_Init - Initialize the NFC F driver.
//!
//! This function must be called prior to any other function offered by the
//! NFC F driver. This function initializes the NFC DEP support for
//! PICC and PCD,
//!
//! \return None.
//
//*****************************************************************************
void NFC_F_Init(void)
{
	g_sNfcFStatus.bNfcDepSupoort = false;
}

//*****************************************************************************
//
//! NFC_F_send_SENSF_REQ - Sends the SENSF REQ to the PICC.
//!
//! \param ui16SC is the system code.
//! \param ui8RC is the request code.
//! \param ui8Slot is the number of slots.
//!
//! This function sends a SENSF REQ to the PICC.
//!
//! \return None.
//
//*****************************************************************************
void NFC_F_send_SENSF_REQ(uint16_t ui16SC, uint8_t ui8RC, uint8_t ui8Slot)
{
	uint8_t nfc_f_packet[6];
	//
	// Length
	//
	nfc_f_packet[0] = 0x06;
	//
	// Command
	//
	nfc_f_packet[1] = SENSF_REQ_CMD;

	nfc_f_packet[2] = (uint8_t) (ui16SC >> 8);		// System Code (SC) 7:0
	nfc_f_packet[3] = (uint8_t) (ui16SC & 0xFF);		// System Code (SC) 15:8

	nfc_f_packet[4] = ui8RC;		// Request Code (RC)

	nfc_f_packet[5] = ui8Slot;		// Time Slot Number (TSN) (DP, Table 42, n time slots)
	TRF79x0_writeFIFO(nfc_f_packet,CRC_BIT_ENABLE,6,0);
}

//*****************************************************************************
//
//! NFC_F_send_SENSF_RES - Sends the SENSF RES to the PCD.
//!
//! \param ui8RDValue is based on the request SENS_REQ.
//!
//! This function sends a SENS Response to the PCD including the NFCID2.
//!
//! \return None.
//
//*****************************************************************************
void NFC_F_send_SENSF_RES(uint8_t ui8RDValue)
{
	uint8_t nfc_f_packet[20];
	uint8_t offset = 1;
	uint8_t counter = 0;

	//
	// Command
	//
	nfc_f_packet[offset++] = SENSF_RES_CMD;

	for(counter = 0; counter < 8; counter++)
	{
		nfc_f_packet[offset++] = g_pui8NfcFId2[counter];
	}

	// PAD 0
	nfc_f_packet[offset++] = 0xC0;
	nfc_f_packet[offset++] = 0xC1;
	// PAD 1
	nfc_f_packet[offset++] = 0xC2;
	nfc_f_packet[offset++] = 0xC3;
	nfc_f_packet[offset++] = 0xC4;
	// MRTI CHECK
	nfc_f_packet[offset++] = 0xC5;
	// MRTI UPDATE
	nfc_f_packet[offset++] = 0xC6;
	// PAD2
	nfc_f_packet[offset++] = 0xC7;

	if(ui8RDValue == 0x01)
	{
		// RD - System Code for NDEF-enabled Type 3 Tags.
		nfc_f_packet[offset++] = 0x12;
		nfc_f_packet[offset++] = 0xFC;
	}
	else if(ui8RDValue == 0x02)
	{
		// RD - Advance Protocols
		nfc_f_packet[offset++] = 0x00;
		// Support 212/424 kbps
		nfc_f_packet[offset++] = 0x83;
	}
	//
	// Length
	//
	nfc_f_packet[0] = offset;

	MCU_delayMicrosecond(2250);

	// This can be incremented by 1250 to answer in a different timeslot

	TRF79x0_writeFIFO(nfc_f_packet,CRC_BIT_ENABLE,offset,0);
}

//*****************************************************************************
//
//! NFC_F_processReceivedRequest - Processes the received SENSF_REQ or SENSFRES.
//!
//! \param pui8RxBuffer is the pointer to the received data.
//! \param ui8Length is the length of the received packet.
//!
//! This function checks if the command received is a SENSF_REQ or SENSF_RES.
//! If it is a SENSF_RES, it stores the NFFCID2. If the command is a SENSF_REQ,
//! it replies according the request code it received.
//!
//! \return ui8Status returns STATUS_SUCCESS if the packet is valid,
//! otherwise it returns STATUS_FAIL.
//
//*****************************************************************************
uint8_t NFC_F_processReceivedRequest(uint8_t * pui8RxBuffer, uint8_t ui8Length)
{
	volatile uint8_t command_length;
	uint8_t command;
	uint8_t counter;
	uint8_t nfc_f_status = STATUS_SUCCESS;

	command_length = pui8RxBuffer[0];
	command = pui8RxBuffer[1];

	if(command == SENSF_RES_CMD)
	{
		//
		// Store the g_nfcid2
		//
		for(counter = 0; counter < 8; counter++)
		{
			g_pui8NfcFId2[counter] = pui8RxBuffer[2+counter];
		}
	}
	else if(command == SENSF_REQ_CMD && command_length == 0x06 && ui8Length == 0x06)
	{
		if(pui8RxBuffer[2] == 0xFF && pui8RxBuffer[3] == 0xFF)
		{
			// Valid SENSF_REQ received - thus send a SENSF Response
			NFC_F_send_SENSF_RES(pui8RxBuffer[4]);
		}
		else
			nfc_f_status = STATUS_FAIL;
	}
	else
	{
		nfc_f_status = STATUS_FAIL;
	}
	return nfc_f_status;

}

//*****************************************************************************
//
//! NFC_F_getNFCID2 - This function returns the NFCID2 ID (PICC or PCD Mode)
//!
//! \param pui8NfcFId is a double pointer to be modified with the NFCID2 buffer.
//! \param ui8NfcFIdLength is a pointer to store the NFCID2 Length.
//!
//! This function returns the NFCID2.
//!
//! \return None.
//
//****************************************************************************
void NFC_F_getNFCID2(uint8_t ** pui8NfcFId, uint8_t * ui8NfcFIdLength)
{
	*pui8NfcFId = g_pui8NfcFId2;

	*ui8NfcFIdLength = 8;
}

//*****************************************************************************
//
//! NFC_F_setNfcDepSupport - Sets the NFCDEP support for the NFC-F driver.
//!
//! \param bDepSupport the NFC DEP support flag.
//!
//! This function sets the NFC DEP support.
//!
//! \return None.
//
//*****************************************************************************
void NFC_F_setNfcDepSupport(bool bDepSupport)
{
	g_sNfcFStatus.bNfcDepSupoort = bDepSupport;

}

//*****************************************************************************
//
//! NFC_F_isNfcDepSupported - Checks if NFC DEP is supported.
//!
//! This function checks if NFC DEP is supported.
//!
//! \return ui8RetStatus returns STATUS_SUCCESS if it is supported,
//! otherwise it returns STATUS_FAIL.
//
//*****************************************************************************
uint8_t NFC_F_isNfcDepSupported(void)
{
	uint8_t ui8RetStatus;

	if(g_sNfcFStatus.bNfcDepSupoort == true)
	{
		ui8RetStatus = STATUS_SUCCESS;
	}
	else
	{
		ui8RetStatus = STATUS_FAIL;
	}

	return ui8RetStatus;
}

//*****************************************************************************
//
//! NFC_F_setNfcId2 - This function sets the UID of the NFC-F when PICC mode is
//! activated. (PICC Mode)
//!
//! \param ui8NewNfcId2 is the pointer to the NFCID2.
//! \param ui8Length is the length of the UID. (Must be 8)
//!
//! This function sets the ID and the size of the PICC UID.
//!
//! \return ui8Status returns STATUS_SUCCESS if the command is valid,
//! otherwise it returns STATUS_FAIL.
//
//****************************************************************************
uint8_t NFC_F_setNfcId2(uint8_t * ui8NewNfcId2, uint8_t ui8Length)
{
	uint8_t ui8Status = STATUS_FAIL;
	int count;

	if (ui8Length == 8)
	{
		for (count = 0; count < 8; count++)
		{
			g_pui8NfcFId2[count] = ui8NewNfcId2[count];	// Re-Write ISO14443 T4TA Tag
		}
		ui8Status = STATUS_SUCCESS;
	}
	else
	{
		// Error - Set to default
		g_pui8NfcFId2[0] = 0x01;
		g_pui8NfcFId2[1] = 0x8F;
		g_pui8NfcFId2[2] = 0x88;
		g_pui8NfcFId2[3] = 0x77;
		g_pui8NfcFId2[4] = 0x66;
		g_pui8NfcFId2[5] = 0x55;
		g_pui8NfcFId2[6] = 0x44;
		g_pui8NfcFId2[7] = 0x33;

		ui8Status = STATUS_FAIL;
	}
	return ui8Status;
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
