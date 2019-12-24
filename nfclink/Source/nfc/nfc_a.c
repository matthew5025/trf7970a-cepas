//*****************************************************************************
//
// nfc_a.c - Implementation of ISO14443-A APIs.
//
// Copyright (c) 2015 Texas Instruments Incorporated.  All rights reserved.
// TI Information - Selective Disclosure
//
//*****************************************************************************

#include "nfc_a.h"

//*****************************************************************************
//
//! \addtogroup nfc_a_api NFC A API Functions
//! @{
//! NFC A is a NFC Layer used by NFC_Initiator and NFC_Target layers to
//! activate the NFC-A technology. For more information on NFC-A please
//! read the NFC Digital Specification Version 1.1.
//
//*****************************************************************************

//*****************************************************************************
//
// Stores the NFC-A ID in both modes as a PICC or when reading a tag
// when the transceiver is a PCD. (both PICC and PCD modes)
//
//*****************************************************************************
static uint8_t g_pui8NfcAId[10];

//*****************************************************************************
//
// Stores the boolean flag indicating if the NFCID is complete (PCD mode).
//
//*****************************************************************************
static bool g_bNfcId1Complete;

//*****************************************************************************
//
// Stores the boolean flag indicating if Auto SDD should be enabled on the
// transceiver (PICC mode).
//
//*****************************************************************************
static bool g_bAutoSDDEnabled;

//*****************************************************************************
//
// Stores the NFC DEP and ISO DEP support capabilities of the emulated PICC.
// (PICC mode)
//
//*****************************************************************************
static NfcAStatus g_sNfcACEStatus;

//*****************************************************************************
//
// Stores the NFC DEP and ISO DEP support capabilities of the PICC read by the
// PCD (PCD mode).
//
//*****************************************************************************
static NfcAStatus g_sRWNfcStatus;

//*****************************************************************************
//
// Stores the current state of the RW statemachine. (PCD mode)
//
//*****************************************************************************
static tUNfcAState g_eNfcACurrentState;

//*****************************************************************************
//
// Stores the number of bytes that can sent to the PICC. (PCD mode)
//
//*****************************************************************************
static uint16_t g_ui16NfcAFsd;

//*****************************************************************************
//
// Stores the max timeout (mS) the transceiver should wait for a PICC response.
// (PCD mode)
//
//*****************************************************************************
static uint16_t g_ui16NfcAFwt;

//*****************************************************************************
//
// Stores the time (mS) the transceiver should wait before sending a command to
// the PICC. (PCD mode)
//
//*****************************************************************************
static uint16_t g_ui16NfcASfgt;

//*****************************************************************************
//
// Stores the boolean flag indicating CID support from the PICC. (PCD mode)
//
//*****************************************************************************
static bool g_bNfcACIDSupport;

//*****************************************************************************
//
// Stores the CID number to be used in the PCD commands when g_bNfcACIDSupport
// is true. (PCD mode)
//
//*****************************************************************************
static uint8_t g_ui8NfcACID;

//*****************************************************************************
//
// Stores the boolean flag indicating to use CID if the PICC supports it.
// (PCD mode)
//
//*****************************************************************************
static bool g_bNFCACIDEnable;

//*****************************************************************************
//
// Stores the SAK response from the PICC. (PCD mode)
//
//*****************************************************************************
static uint8_t g_ui8NfcASak;

//*****************************************************************************
//
//! NFC_A_setCIDSettings - Sets the CID Enable flag and CID value.
//!
//! \param bNFCACIDEnable the CID Enable flag.
//! \param ui8NfcACID the CID number.
//!
//! This function sets the NFC CID enable flag. When the PICC supports CID the
//! code the PCD code will use the ui8NfcACID.
//!
//! \return None.
//
//*****************************************************************************
void NFC_A_setCIDSettings(bool bNFCACIDEnable, uint8_t ui8NfcACID)
{
	g_bNFCACIDEnable = bNFCACIDEnable;
	g_ui8NfcACID = ui8NfcACID;
}

//*****************************************************************************
//
//! NFC_A_init - Initialize the NFC A driver.
//!
//! This function must be called prior to any other function offered by the
//! NFC A driver. This function initializes the NFC DEP and ISO support for
//! PICC and PCD, the length of the tag, driver current state, CID, and SAK
//! response.
//!
//! \return None.
//
//*****************************************************************************
void NFC_A_init(void)
{
	// Reset NFCAId
	g_bNfcId1Complete = false;
	g_sNfcACEStatus.bISODepSupport = false;
	g_sNfcACEStatus.bNfcDepSupport = false;
	g_sRWNfcStatus.bISODepSupport = false;
	g_sRWNfcStatus.bNfcDepSupport = false;
	g_sRWNfcStatus.eUidLength = NFC_A_UID_INCOMPLETE;
	g_eNfcACurrentState = NFC_A_RATS_STATE;
	g_ui16NfcAFsd = 0;
	g_ui16NfcASfgt = 0;
	g_ui16NfcAFwt = 0;
	g_bNfcACIDSupport = false;
	g_ui8NfcASak = 0;
}

//*****************************************************************************
//
//! NFC_A_send_SENS_REQ - Sends the SENS REQ to the PICC.
//!
//! This function sends a SENS Request - first PCD command used to activate
//! NFC-A tags.
//!
//! \return None.
//
//*****************************************************************************
void NFC_A_send_SENS_REQ(void)
{
	uint8_t nfc_a_packet[1];

	// The next incoming response will not have a CRC
	TRF79x0_writeSingle(0x88,TRF79X0_ISO_CONTROL_REG);

	//
	// Command
	//
	nfc_a_packet[0] = SENS_REQ_CMD;

	TRF79x0_writeFIFO(nfc_a_packet,CRC_BIT_DISABLE,1,7);
}

//*****************************************************************************
//
//! NFC_A_send_ALL_REQ - Sends the ALL REQ to the PICC.
//!
//! This function sends an ALL Request - first PCD command used to wake / activate
//! NFC-A tags.
//!
//! \return None.
//
//*****************************************************************************
void NFC_A_send_ALL_REQ(void)
{
	uint8_t nfc_a_packet[1];

	//
	// Command
	//
	nfc_a_packet[0] = ALL_REQ_CMD;

	TRF79x0_writeFIFO(nfc_a_packet,CRC_BIT_DISABLE,1,7);
}

//*****************************************************************************
//
//! NFC_A_send_SDD_REQ - Sends the SDD REQ to the PICC.
//!
//! \param ui8CascadeLevel is the cascade level which determines the first byte
//! of the SDD Request.
//!
//! This function sends a SDD Request based on the indicated cascade level.
//!
//! \return None.
//
//*****************************************************************************
void NFC_A_send_SDD_REQ(uint8_t ui8CascadeLevel)
{
	uint8_t nfc_a_packet[2];
	uint8_t ui8Offset = 0;

	if(ui8CascadeLevel == 0x01)
	{
		nfc_a_packet[ui8Offset++] = SDD_REQ_CMD_CL1;
	}
	else if(ui8CascadeLevel == 0x02)
	{
		nfc_a_packet[ui8Offset++] = SDD_REQ_CMD_CL2;
	}
	else if(ui8CascadeLevel == 0x03)
	{
		nfc_a_packet[ui8Offset++] = SDD_REQ_CMD_CL3;
	}
	nfc_a_packet[ui8Offset++] = 0x20;

	// The next incoming response will not have a CRC
	TRF79x0_writeSingle(0x88,TRF79X0_ISO_CONTROL_REG);

	TRF79x0_writeFIFO(nfc_a_packet,CRC_BIT_DISABLE,ui8Offset,0);
}

//*****************************************************************************
//
//! NFC_A_send_SEL_REQ - Sends the SEL REQ to the PICC.
//!
//! \param ui8CascadeLevel is the cascade level which determines the first byte
//! of the SEL Request.
//!
//! This function sends a SEL Request based on the indicated cascade level.
//!
//! \return None.
//
//*****************************************************************************
void NFC_A_send_SEL_REQ(uint8_t ui8CascadeLevel)
{
	uint8_t nfc_a_packet[7];
	uint8_t ui8Offset = 0;

	if(ui8CascadeLevel == 0x01)
	{
		nfc_a_packet[ui8Offset++] = SDD_REQ_CMD_CL1;
		nfc_a_packet[ui8Offset++] = 0x70;
		if(g_bNfcId1Complete == false)
		{
			nfc_a_packet[ui8Offset++] = 0x88;
			nfc_a_packet[ui8Offset++] = g_pui8NfcAId[0];
			nfc_a_packet[ui8Offset++] = g_pui8NfcAId[1];
			nfc_a_packet[ui8Offset++] = g_pui8NfcAId[2];
			nfc_a_packet[ui8Offset++] = 0x88 ^ g_pui8NfcAId[0] ^ g_pui8NfcAId[1] ^ g_pui8NfcAId[2];
		}
		else
		{
			nfc_a_packet[ui8Offset++] = g_pui8NfcAId[0];
			nfc_a_packet[ui8Offset++] = g_pui8NfcAId[1];
			nfc_a_packet[ui8Offset++] = g_pui8NfcAId[2];
			nfc_a_packet[ui8Offset++] = g_pui8NfcAId[3];
			nfc_a_packet[ui8Offset++] = g_pui8NfcAId[0] ^ g_pui8NfcAId[1] ^ g_pui8NfcAId[2] ^ g_pui8NfcAId[3];
		}
	}
	else if(ui8CascadeLevel == 0x02)
	{
		nfc_a_packet[ui8Offset++] = SDD_REQ_CMD_CL2;
		nfc_a_packet[ui8Offset++] = 0x70;
		if(g_bNfcId1Complete == false)
		{
			nfc_a_packet[ui8Offset++] = 0x88;
			nfc_a_packet[ui8Offset++] = g_pui8NfcAId[3];
			nfc_a_packet[ui8Offset++] = g_pui8NfcAId[4];
			nfc_a_packet[ui8Offset++] = g_pui8NfcAId[5];
			nfc_a_packet[ui8Offset++] = 0x88 ^ g_pui8NfcAId[3] ^ g_pui8NfcAId[4] ^ g_pui8NfcAId[5];
		}
		else
		{
			nfc_a_packet[ui8Offset++] = g_pui8NfcAId[3];
			nfc_a_packet[ui8Offset++] = g_pui8NfcAId[4];
			nfc_a_packet[ui8Offset++] = g_pui8NfcAId[5];
			nfc_a_packet[ui8Offset++] = g_pui8NfcAId[6];
			nfc_a_packet[ui8Offset++] = g_pui8NfcAId[3] ^ g_pui8NfcAId[4] ^ g_pui8NfcAId[5] ^ g_pui8NfcAId[6];
		}
	}
	else if(ui8CascadeLevel == 0x03)
	{
		nfc_a_packet[ui8Offset++] = SDD_REQ_CMD_CL3;
		nfc_a_packet[ui8Offset++] = 0x70;
		nfc_a_packet[ui8Offset++] = g_pui8NfcAId[6];
		nfc_a_packet[ui8Offset++] = g_pui8NfcAId[7];
		nfc_a_packet[ui8Offset++] = g_pui8NfcAId[8];
		nfc_a_packet[ui8Offset++] = g_pui8NfcAId[9];
		nfc_a_packet[ui8Offset++] = g_pui8NfcAId[6] ^ g_pui8NfcAId[7] ^ g_pui8NfcAId[8] ^ g_pui8NfcAId[9];
	}

	// The next incoming response will have a CRC
	TRF79x0_writeSingle(0x08,TRF79X0_ISO_CONTROL_REG);

	TRF79x0_writeFIFO(nfc_a_packet,CRC_BIT_ENABLE,ui8Offset,0);
}

//*****************************************************************************
//
//! NFC_A_send_SENS_RES - Sends the SENS RES to the PCD.
//!
//! This function sends a SENS Response to the PCD including the size of the
//! UID.
//!
//! \return None.
//
//*****************************************************************************
void NFC_A_send_SENS_RES(void)
{
	uint8_t nfc_a_packet[2];

	//
	// Command - Bit Frame = 00100 (binary)
	//
	if(g_sNfcACEStatus.eUidLength == NFC_A_UID_SINGLE)
	{
		nfc_a_packet[0] = SENS_RES_CMD_SINGLE | 0x04;
	}
	else if(g_sNfcACEStatus.eUidLength == NFC_A_UID_DOUBLE)
	{
		nfc_a_packet[0] = SENS_RES_CMD_DOUBLE | 0x04;
	}
	else if(g_sNfcACEStatus.eUidLength == NFC_A_UID_TRIPLE)
	{
		nfc_a_packet[0] = SENS_RES_CMD_TRIPLE | 0x04;
	}
	nfc_a_packet[1] = 0x00;

	TRF79x0_writeFIFO(nfc_a_packet,CRC_BIT_DISABLE,2,0);

	// Register 01h. ISO Control Register - Receive @ 106 kbps without CRC.
	TRF79x0_writeSingle(0xA4, TRF79X0_ISO_CONTROL_REG);
}

//*****************************************************************************
//
//! NFC_A_send_SDD_RES - Sends the SDD RES to the PCD.
//!
//! \param ui8CascadeLevel is the cascade level which determines  what response
//! to send to the PCD depending on the UID length.
//!
//! This function sends a SDD Response to the PCD.
//!
//! \return None.
//
//*****************************************************************************
void NFC_A_send_SDD_RES(uint8_t ui8CascadeLevel)
{
	uint8_t nfc_a_packet[10];
	uint8_t ui8Offset = 0;

	if(g_sNfcACEStatus.eUidLength == NFC_A_UID_SINGLE && ui8CascadeLevel == 0x01)
	{
		nfc_a_packet[ui8Offset++] = g_pui8NfcAId[0];
		nfc_a_packet[ui8Offset++] = g_pui8NfcAId[1];
		nfc_a_packet[ui8Offset++] = g_pui8NfcAId[2];
		nfc_a_packet[ui8Offset++] = g_pui8NfcAId[3];
		nfc_a_packet[ui8Offset++] = g_pui8NfcAId[0] ^ g_pui8NfcAId[1] ^ g_pui8NfcAId[2] ^ g_pui8NfcAId[3];
	}
	else if(g_sNfcACEStatus.eUidLength == NFC_A_UID_DOUBLE && ui8CascadeLevel == 0x02)
	{
		nfc_a_packet[ui8Offset++] = g_pui8NfcAId[3];
		nfc_a_packet[ui8Offset++] = g_pui8NfcAId[4];
		nfc_a_packet[ui8Offset++] = g_pui8NfcAId[5];
		nfc_a_packet[ui8Offset++] = g_pui8NfcAId[6];
		nfc_a_packet[ui8Offset++] = g_pui8NfcAId[3] ^ g_pui8NfcAId[4] ^ g_pui8NfcAId[5] ^ g_pui8NfcAId[6];
	}
	else if(g_sNfcACEStatus.eUidLength == NFC_A_UID_TRIPLE && ui8CascadeLevel == 0x03)
	{
		nfc_a_packet[ui8Offset++] = g_pui8NfcAId[6];
		nfc_a_packet[ui8Offset++] = g_pui8NfcAId[7];
		nfc_a_packet[ui8Offset++] = g_pui8NfcAId[8];
		nfc_a_packet[ui8Offset++] = g_pui8NfcAId[9];
		nfc_a_packet[ui8Offset++] = g_pui8NfcAId[6] ^ g_pui8NfcAId[7] ^ g_pui8NfcAId[8] ^ g_pui8NfcAId[9];
	}
	else if(ui8CascadeLevel == 1)
	{
		nfc_a_packet[ui8Offset++] = 0x88;
		nfc_a_packet[ui8Offset++] = g_pui8NfcAId[0];
		nfc_a_packet[ui8Offset++] = g_pui8NfcAId[1];
		nfc_a_packet[ui8Offset++] = g_pui8NfcAId[2];
		nfc_a_packet[ui8Offset++] = 0x88 ^ g_pui8NfcAId[0] ^ g_pui8NfcAId[1] ^ g_pui8NfcAId[2];
	}
	else if(ui8CascadeLevel == 2)
	{
		nfc_a_packet[ui8Offset++] = 0x88;
		nfc_a_packet[ui8Offset++] = g_pui8NfcAId[3];
		nfc_a_packet[ui8Offset++] = g_pui8NfcAId[4];
		nfc_a_packet[ui8Offset++] = g_pui8NfcAId[5];
		nfc_a_packet[ui8Offset++] = 0x88 ^ g_pui8NfcAId[3] ^ g_pui8NfcAId[4] ^ g_pui8NfcAId[5];
	}

	TRF79x0_writeFIFO(nfc_a_packet,CRC_BIT_DISABLE,ui8Offset,0);

	// The next command will have a CRC
//	TRF79x0_writeSingle(0x21,TRF79X0_ISO_CONTROL_REG);
	// Changed due to test case TC_NFCA_LIS_BV_109
	TRF79x0_writeSingle(0xA4,TRF79X0_ISO_CONTROL_REG);
}

//*****************************************************************************
//
//! NFC_A_send_SEL_RES - Sends the SEL RES to the PCD.
//!
//! \param ui8CascadeLevel is the cascade level which determines  what response
//! to send to the PCD depending on the UID length.
//!
//! This function sends a SEL Response to the PCD. Once the complete UID
//! has been sent to the PCD. The ISO and NFC DEP flags will be indicated on the
//! 5th and 6th bits of the SAK response.
//!
//! \return None.
//
//*****************************************************************************
void NFC_A_send_SEL_RES(uint8_t ui8CascadeLevel)
{
	uint8_t nfc_a_packet[1];

	nfc_a_packet[0] = 0x00;

	// If NFCID1 is complete
	if(NFC_A_isNfcDepSupported() == true)
	{
		// Set Bit 6
		nfc_a_packet[0] |= 0x40;
	}

	if(NFC_A_isIsoDepSupported() == true)
	{
		// Set Bit 5
		nfc_a_packet[0] |= 0x20;
	}

	if(g_sNfcACEStatus.eUidLength == NFC_A_UID_SINGLE && ui8CascadeLevel == 0x01)
	{
		// Do not modify nfc_a_packet
	}
	else if(g_sNfcACEStatus.eUidLength == NFC_A_UID_DOUBLE && ui8CascadeLevel == 0x02)
	{
		// Do not modify nfc_a_packet
	}
	else if(g_sNfcACEStatus.eUidLength == NFC_A_UID_TRIPLE && ui8CascadeLevel == 0x03)
	{
		// Do not modify nfc_a_packet
	}
	else
	{
		// The next command will not have a CRC
		TRF79x0_writeSingle(0xA4,TRF79X0_ISO_CONTROL_REG);
		// NFCID1 not complete
		nfc_a_packet[0] |= 0x04;
	}

	TRF79x0_writeFIFO(nfc_a_packet,CRC_BIT_ENABLE,1,0);

	// Check if selection is completed
	if((nfc_a_packet[0] & 0x04) == 0x00)
	{
		// The next command will have a CRC
		TRF79x0_writeSingle(0x24,TRF79X0_ISO_CONTROL_REG);
	}
}

//*****************************************************************************
//
//! NFC_A_processSDDResponse - Processes the received SDD response from the PICC.
//!
//! \param pui8RxBuffer is the pointer to the received response from the PICC.
//! \param ui8CascadeLevel is the cascade level.
//!
//! This function checks the SDD response from the tag, and stores the UID in the
//! g_pui8NfcAId buffer.
//!
//! \return ui8Status returns STATUS_SUCCESS if the SDD response is valid,
//! otherwise it returns STATUS_FAIL.
//
//*****************************************************************************
uint8_t NFC_A_processSDDResponse(uint8_t * pui8RxBuffer, uint8_t ui8CascadeLevel)
{
	uint8_t ui8Counter;
	uint8_t ui8Status = STATUS_SUCCESS;

	// Check if NFCId is complete
	if(pui8RxBuffer[0] == 0x88)
	{
		g_bNfcId1Complete = false;
	}
	else
	{
		g_bNfcId1Complete = true;
	}

	if(ui8CascadeLevel == 0x01)
	{
		MCU_delayMicrosecond(500);
		if((g_bNfcId1Complete == false) && (g_sRWNfcStatus.eUidLength != NFC_A_UID_SINGLE))
		{
			// Bytes 1-3 available
			for(ui8Counter=0;ui8Counter<3;ui8Counter++)
			{
				g_pui8NfcAId[ui8Counter] = pui8RxBuffer[ui8Counter+1];
			}

			// Check whether or not the BCC returned is correct
			if ((0x88 ^ g_pui8NfcAId[0] ^ g_pui8NfcAId[1] ^ g_pui8NfcAId[2]) != pui8RxBuffer[4])
			{
				ui8Status = STATUS_FAIL;
			}

		}
		else if(g_bNfcId1Complete == true && (g_sRWNfcStatus.eUidLength == NFC_A_UID_SINGLE))
		{
			// Bytes 1-4 available
			for(ui8Counter=0;ui8Counter<4;ui8Counter++)
			{
				g_pui8NfcAId[ui8Counter] = pui8RxBuffer[ui8Counter];
			}

			// Check whether or not the BCC returned is correct
			if ((g_pui8NfcAId[0] ^ g_pui8NfcAId[1] ^ g_pui8NfcAId[2] ^ g_pui8NfcAId[3]) != pui8RxBuffer[4])
			{
				ui8Status = STATUS_FAIL;
			}
		}
		else
		{
			ui8Status = STATUS_FAIL;
		}
	}
	else if(ui8CascadeLevel == 0x02)
	{
		MCU_delayMicrosecond(500);
		if((g_bNfcId1Complete == false) && (g_sRWNfcStatus.eUidLength != NFC_A_UID_DOUBLE))
		{
			// Bytes 4-6 available
			for(ui8Counter=0;ui8Counter<3;ui8Counter++)
			{
				g_pui8NfcAId[ui8Counter+3] = pui8RxBuffer[ui8Counter+1];
			}

			// Check whether or not the BCC returned is correct
			if ((0x88 ^ g_pui8NfcAId[3] ^ g_pui8NfcAId[4] ^ g_pui8NfcAId[5]) != pui8RxBuffer[4])
			{
				ui8Status = STATUS_FAIL;
			}
		}
		else if(g_bNfcId1Complete == true && (g_sRWNfcStatus.eUidLength == NFC_A_UID_DOUBLE))
		{
			// Bytes 4-7 available
			for(ui8Counter=0;ui8Counter<4;ui8Counter++)
			{
				g_pui8NfcAId[ui8Counter+3] = pui8RxBuffer[ui8Counter];
			}

			// Check whether or not the BCC returned is correct
			if ((g_pui8NfcAId[3] ^ g_pui8NfcAId[4] ^ g_pui8NfcAId[5] ^ g_pui8NfcAId[6]) != pui8RxBuffer[4])
			{
				ui8Status = STATUS_FAIL;
			}
		}
		else
		{
			ui8Status = STATUS_FAIL;
		}
	}
	else if(ui8CascadeLevel == 0x03)
	{
		MCU_delayMicrosecond(500);
		if(g_bNfcId1Complete == true && (g_sRWNfcStatus.eUidLength == NFC_A_UID_TRIPLE))
		{
			MCU_delayMicrosecond(500);
			// Bytes 7-10 available
			for(ui8Counter=0;ui8Counter<4;ui8Counter++)
			{
				g_pui8NfcAId[ui8Counter+6] = pui8RxBuffer[ui8Counter];
			}

			// Check whether or not the BCC returned is correct
			if ((g_pui8NfcAId[6] ^ g_pui8NfcAId[7] ^ g_pui8NfcAId[8] ^ g_pui8NfcAId[9]) != pui8RxBuffer[4])
			{
				ui8Status = STATUS_FAIL;
			}
		}
		else
		{
			ui8Status = STATUS_FAIL;
		}
	}
	else
	{
		ui8Status = STATUS_FAIL;
	}

	return ui8Status;
}

//*****************************************************************************
//
//! NFC_A_processSENSRes - Processes the received SENS response from the PICC.
//!
//! \param pui8RxBuffer is the pointer to the received response from the PICC.
//! \param ui8Length is the length of the received response.
//!
//! This function checks the SENS response and sets the size of the PICC UID.
//!
//! \return ui8Status returns STATUS_SUCCESS if the SENS response is valid,
//! otherwise it returns STATUS_FAIL.
//
//*****************************************************************************
uint8_t NFC_A_processSENSRes(uint8_t * pui8RxBuffer, uint8_t ui8Length)
{
	uint8_t ui8Status = STATUS_SUCCESS;
	uint8_t ui8NFCIDSize;

	if((ui8Length == 0x02)
		// Don't reply to 00 00
		&& ((pui8RxBuffer[0] != 0x00) || (pui8RxBuffer[1] != 0x00))
		// Don't reply to 01 C0
		&& ((pui8RxBuffer[0] != 0x01) || (pui8RxBuffer[1] != 0x0C)))
	{
		ui8NFCIDSize = pui8RxBuffer[0] >> 6;
		switch(ui8NFCIDSize)
		{
		case 0x00:
			g_sRWNfcStatus.eUidLength = NFC_A_UID_SINGLE;
			break;
		case 0x01:
			g_sRWNfcStatus.eUidLength = NFC_A_UID_DOUBLE;
			break;
		case 0x02:
			g_sRWNfcStatus.eUidLength = NFC_A_UID_TRIPLE;
			break;
		default:
			ui8Status = STATUS_FAIL;
		}
	}
	else
	{
		ui8Status = STATUS_FAIL;
	}

	return ui8Status;
}

//*****************************************************************************
//
//! NFC_A_processSELResponse - Processes the received SEL response from the PICC.
//!
//! \param pui8RxBuffer is the pointer to the received response from the PICC.
//! \param ui8Length is the length of the received response.
//!
//! This function checks the SEL response and stores the SAK response in g_ui8NfcASak.
//!
//! \return ui8Status returns STATUS_SUCCESS if the SEL response is valid,
//! otherwise it returns STATUS_FAIL.
//
//*****************************************************************************
uint8_t NFC_A_processSELResponse(uint8_t * pui8RxBuffer, uint8_t ui8Length)
{
	uint8_t ui8Status = STATUS_SUCCESS;

	if(ui8Length == 1)
	{
		MCU_delayMicrosecond(500);
		// Check if the incomplete bit is set
		if(pui8RxBuffer[0] & 0x04)
		{
			// Do nothing
		}
		else
		{
			g_ui8NfcASak = pui8RxBuffer[0];

			if(pui8RxBuffer[0] & 0x40)
			{
				g_sRWNfcStatus.bNfcDepSupport = true;
			}

			if(pui8RxBuffer[0] & 0x20)
			{
				g_sRWNfcStatus.bISODepSupport = true;
			}
		}
	}
	else
	{
		ui8Status = STATUS_FAIL;
	}

	return ui8Status;
}

//*****************************************************************************
//
//! NFC_A_check_SEL_REQ - Checks the received SEL REQ from the PCD. (PICC Mode)
//!
//! \param ui8CascadeLevel is the current cascade level.
//! \param pui8Payload is the pointer to the received request from the PCD.
//! \param ui8Length is the length of the received response.
//!
//! This function checks the SEL request is valid based on the UID and BCC.
//!
//! \return ui8Status returns STATUS_SUCCESS if the SEL request is valid, otherwise
//! it returns STATUS_FAIL.
//
//*****************************************************************************
uint8_t NFC_A_check_SEL_REQ(uint8_t ui8CascadeLevel, uint8_t * pui8Payload, uint8_t ui8Length)
{
	uint8_t ui8Status;

	if(g_sNfcACEStatus.eUidLength == NFC_A_UID_SINGLE && ui8CascadeLevel == 0x01)
	{
		if(pui8Payload[0x02] == g_pui8NfcAId[0]
		   && pui8Payload[0x03] == g_pui8NfcAId[1]
		   && pui8Payload[0x04] == g_pui8NfcAId[2]
		   && pui8Payload[0x05] == g_pui8NfcAId[3]
		   && pui8Payload[0x06] == ( g_pui8NfcAId[0] ^ g_pui8NfcAId[1] ^ g_pui8NfcAId[2] ^ g_pui8NfcAId[3]))
		{
			ui8Status = STATUS_SUCCESS;
		}
		else
			ui8Status = STATUS_FAIL;
	}
	else if(g_sNfcACEStatus.eUidLength == NFC_A_UID_DOUBLE && ui8CascadeLevel == 0x02)
	{
		if(pui8Payload[0x02] == g_pui8NfcAId[3]
		   && pui8Payload[0x03] == g_pui8NfcAId[4]
		   && pui8Payload[0x04] == g_pui8NfcAId[5]
		   && pui8Payload[0x05] == g_pui8NfcAId[6]
		   && pui8Payload[0x06] == (g_pui8NfcAId[3] ^ g_pui8NfcAId[4] ^ g_pui8NfcAId[5] ^ g_pui8NfcAId[6]))
		{
			ui8Status = STATUS_SUCCESS;
		}
		else
			ui8Status = STATUS_FAIL;
	}
	else if(g_sNfcACEStatus.eUidLength == NFC_A_UID_TRIPLE && ui8CascadeLevel == 0x03)
	{
		if(pui8Payload[0x02] == g_pui8NfcAId[6]
		   && pui8Payload[0x03] == g_pui8NfcAId[7]
		   && pui8Payload[0x04] == g_pui8NfcAId[8]
		   && pui8Payload[0x05] == g_pui8NfcAId[9]
		   && pui8Payload[0x06] == (g_pui8NfcAId[6] ^ g_pui8NfcAId[7] ^ g_pui8NfcAId[8] ^ g_pui8NfcAId[9]))

		{
			ui8Status = STATUS_SUCCESS;
		}
		else
			ui8Status = STATUS_FAIL;
	}
	else if(ui8CascadeLevel == 1)
	{
		if(pui8Payload[0x02] == 0x88
		   && pui8Payload[0x03] == g_pui8NfcAId[0]
		   && pui8Payload[0x04] == g_pui8NfcAId[1]
		   && pui8Payload[0x05] == g_pui8NfcAId[2]
		   && pui8Payload[0x06] == (pui8Payload[2] ^ g_pui8NfcAId[0] ^ g_pui8NfcAId[1] ^ g_pui8NfcAId[2]))
		{
			ui8Status = STATUS_SUCCESS;
		}
		else
			ui8Status = STATUS_FAIL;
	}
	else if(ui8CascadeLevel == 2)
	{
		if(pui8Payload[0x02] == 0x88
		   && pui8Payload[0x03] == g_pui8NfcAId[3]
		   && pui8Payload[0x04] == g_pui8NfcAId[4]
		   && pui8Payload[0x05] == g_pui8NfcAId[5]
		   && pui8Payload[0x06] == ( pui8Payload[2] ^ g_pui8NfcAId[3] ^ g_pui8NfcAId[4] ^ g_pui8NfcAId[5]))
		{
			ui8Status = STATUS_SUCCESS;
		}
		else
			ui8Status = STATUS_FAIL;
	}
	else
	{
		ui8Status = STATUS_FAIL;
	}

	return ui8Status;
}

//*****************************************************************************
//
//! NFC_A_isInitiatorSelected - Checks if the initiator is selected. (PCD Mode)
//!
//! \param ui8CascadeLevel is the current cascade level.
//!
//! This function checks if the PCD activated the PICC.
//!
//! \return ui8Status returns STATUS_SUCCESS if the initiator is selected,
//! otherwise it returns STATUS_FAIL.
//
//*****************************************************************************
uint8_t NFC_A_isInitiatorSelected(uint8_t ui8CascadeLevel)
{
	uint8_t ui8Status;

	if(g_sRWNfcStatus.eUidLength == NFC_A_UID_SINGLE && ui8CascadeLevel == 0x01)
	{
		ui8Status = STATUS_SUCCESS;
	}
	else if(g_sRWNfcStatus.eUidLength == NFC_A_UID_DOUBLE && ui8CascadeLevel == 0x02)
	{
		ui8Status = STATUS_SUCCESS;
	}
	else if(g_sRWNfcStatus.eUidLength == NFC_A_UID_TRIPLE && ui8CascadeLevel == 0x03)
	{
		ui8Status = STATUS_SUCCESS;
	}
	else
		ui8Status = STATUS_FAIL;

	return ui8Status;
}

//*****************************************************************************
//
//! NFC_A_isTargetSelected - Checks if the target is selected. (PICC Mode)
//!
//! \param ui8CascadeLevel is the current cascade level.
//!
//! This function checks if the PICC is activated.
//!
//! \return ui8Status returns STATUS_SUCCESS if the target is selected,
//! otherwise it returns STATUS_FAIL.
//
//*****************************************************************************
uint8_t NFC_A_isTargetSelected(uint8_t ui8CascadeLevel)
{
	uint8_t ui8Status;
	uint8_t ui8Temp;

	if(g_sNfcACEStatus.eUidLength == NFC_A_UID_SINGLE && ui8CascadeLevel == 0x01)
	{
		ui8Status = STATUS_SUCCESS;
	}
	else if(g_sNfcACEStatus.eUidLength == NFC_A_UID_DOUBLE && ui8CascadeLevel == 0x02)
	{
		ui8Status = STATUS_SUCCESS;
	}
	else if(g_sNfcACEStatus.eUidLength == NFC_A_UID_TRIPLE && ui8CascadeLevel == 0x03)
	{
		ui8Status = STATUS_SUCCESS;
	}
	else
		ui8Status = STATUS_FAIL;

	if(ui8Status == STATUS_SUCCESS)
	{
		TRF79x0_readSingle(&ui8Temp,TRF79X0_SPECIAL_FUNC_1_REG);
		// Disable Anti-collision Frames for 14443A.
		TRF79x0_writeSingle(0x02 | ui8Temp, TRF79X0_SPECIAL_FUNC_1_REG);
	}

	return ui8Status;
}

//*****************************************************************************
//
//! NFC_A_setDepSupport - Sets the ISO and DEP support for the NFC-A driver.
//!
//! \param bIsoSupport the ISO support flag.
//! \param bDepSupport the NFC DEP support flag.
//!
//! This function sets the NFC DEP and ISO DEP support. If Auto-SDD is enabled
//! by the user, it enables the Auto-SDD in the transceiver. (only recommended
//! to enable Auto-SDD when testing NFC Wave 1 Certifcation).
//!
//! \return None.
//
//*****************************************************************************
void NFC_A_setDepSupport(bool bIsoSupport, bool bDepSupport)
{
	g_sNfcACEStatus.bISODepSupport = bIsoSupport;
	g_sNfcACEStatus.bNfcDepSupport = bDepSupport;

	// Initialize ISO & DEP support inside the TRF7970A.
	if (g_bAutoSDDEnabled)
	{
		TRF79x0_setAutoSdd(g_pui8NfcAId,g_sNfcACEStatus.eUidLength,g_sNfcACEStatus.bNfcDepSupport,g_sNfcACEStatus.bISODepSupport);
	}
}

//*****************************************************************************
//
//! NFC_A_isNfcDepSupported - Checks if NFC DEP is supported.
//!
//! This function checks if NFC DEP is supported.
//!
//! \return ui8RetStatus returns STATUS_SUCCESS if it is supported,
//! otherwise it returns STATUS_FAIL.
//
//*****************************************************************************
uint8_t NFC_A_isNfcDepSupported(void)
{
	uint8_t ui8RetStatus;

	if(g_sNfcACEStatus.bNfcDepSupport == true)
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
//! NFC_A_isIsoDepSupported - Checks if ISO DEP is supported.
//!
//! This function checks if ISO DEP is supported.
//!
//! \return ui8RetStatus returns STATUS_SUCCESS if it is supported,
//! otherwise it returns STATUS_FAIL.
//
//*****************************************************************************
uint8_t NFC_A_isIsoDepSupported(void)
{
	uint8_t ui8RetStatus;

	if(g_sNfcACEStatus.bISODepSupport == true)
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
//! NFC_A_sendRATS - This function sends RATs command. (PCD Mode)
//!
//! This function sends RATS command to the PICC.
//!
//! \return None.
//
//*****************************************************************************
void NFC_A_sendRATS(void)
{
	uint8_t ui8PacketOffset = 0;
	uint8_t ui8CID;
	uint8_t pui8TxBuffer[2];

	pui8TxBuffer[ui8PacketOffset++] = RATS_CMD;

	ui8CID = ISODEP_getCID();

	ui8CID = ui8CID & 0x0F;

	// FSDI = 0x08 ==> 256 bytes
	pui8TxBuffer[ui8PacketOffset++] = 0x80 | ui8CID;

	TRF79x0_writeFIFO(pui8TxBuffer,CRC_BIT_ENABLE,ui8PacketOffset,0);
}

//*****************************************************************************
//
//! NFC_A_sendPPS_REQ - This function sends PPS Req command. (PCD Mode)
//!
//! \param ui8Bitrate the PPS bitrate byte.
//!
//! This function sends PPS REQ to the PICC, to increase the bitrate communication.
//!
//! \return None.
//
//****************************************************************************
void NFC_A_sendPPS_REQ(uint8_t ui8Bitrate)
{
	uint8_t ui8Offset = 0;
	uint8_t pui8TxBuffer[3];

	if(g_bNfcACIDSupport == true && g_bNFCACIDEnable == true)
	{
		pui8TxBuffer[ui8Offset++] = PPS_CMD | (g_ui8NfcACID & 0x0F);
	}
	else
	{
		pui8TxBuffer[ui8Offset++] = PPS_CMD;
	}
	// PPS0
	pui8TxBuffer[ui8Offset++] = 0x11;
	// PPS1
	pui8TxBuffer[ui8Offset++] = 0x0F & ui8Bitrate;

	TRF79x0_writeFIFO(pui8TxBuffer,CRC_BIT_ENABLE,ui8Offset,0);
}

//*****************************************************************************
//
//! NFC_A_stateMachine - This function handles the activation of the PICC. (PCD Mode)
//!
//! This function sends the RATS, PPS (optional), and activates the T4T tags.
//!
//! \return None.
//
//****************************************************************************
void NFC_A_stateMachine(void)
{
	switch(g_eNfcACurrentState)
	{
	case NFC_A_RATS_STATE:
		NFC_A_sendRATS();
		break;
	case NFC_A_PPS_STATE_212:
		NFC_A_sendPPS_REQ(NFC_A_212_KBPS_RW);
		break;
	case NFC_A_PPS_STATE_424:
		NFC_A_sendPPS_REQ(NFC_A_424_KBPS_RW);
		break;
	case NFC_A_PPS_STATE_848:
		NFC_A_sendPPS_REQ(NFC_A_848_KBPS_RW);
		break;
	case NFC_A_ACTIVATED:
		break;
	default:
		break;
	}

}

//*****************************************************************************
//
//! NFC_A_processReceivedData - This function processes the response from the
//! PICC. (PCD Mode)
//!
//! \param pui8RxBuffer is a pointer to the recevied data.
//! \param ui8Length is the length of received response.
//!
//! This function processes the response to the RATS and PPS commands.
//!
//! \return g_eNfcACurrentState the current NFC-A state.
//
//****************************************************************************
tUNfcAState NFC_A_processReceivedData(uint8_t * pui8RxBuffer, uint8_t ui8Length)
{
	t_sNfcRWCommBitrate sRwBitrates;
	t_sTRF79x0_Bitrate sRwInitBitrates;
	uint8_t ui8Offset = 0;
	uint8_t ui8Fsdi;
	uint32_t ui32TempTimeout;

	switch(g_eNfcACurrentState)
	{
	case NFC_A_IDLE_STATE:
		break;
	case NFC_A_RATS_STATE:
		// ATS Response must be smaller than 20 bytes, check that the first packet has a valid length
		if(ui8Length < 21 && (pui8RxBuffer[0] == ui8Length))
		{
			if(ui8Length > 1)
			{
				// Store The a temp length in ui8Offset
				ui8Offset = 1;

				if (pui8RxBuffer[ATS_T0_IDX] & 0x40)
				{
					ui8Offset++;
				}

				if (pui8RxBuffer[ATS_T0_IDX] & 0x20)
				{
					ui8Offset++;
				}

				if (pui8RxBuffer[ATS_T0_IDX] & 0x10)
				{
					ui8Offset++;
				}

				if (pui8RxBuffer[0] < ui8Offset)
				{
					// Protocol Error
					g_eNfcACurrentState = NFC_A_IDLE_STATE;
				}
				else if (ui8Length < pui8RxBuffer[0])
				{
					// Protocol Error
					g_eNfcACurrentState = NFC_A_IDLE_STATE;
				}
				else
				{
					// Store maximum number of bytes allowed for communication
					ui8Fsdi = pui8RxBuffer[ATS_T0_IDX] & T0_FSDI_MASK;

					// Set FSD based on FSDI
					switch(ui8Fsdi)
					{
						case 0:
							g_ui16NfcAFsd = 16;
							break;
						case 1:
							g_ui16NfcAFsd = 24;
							break;
						case 2:
							g_ui16NfcAFsd = 32;
							break;
						case 3:
							g_ui16NfcAFsd = 40;
							break;
						case 4:
							g_ui16NfcAFsd = 48;
							break;
						case 5:
							g_ui16NfcAFsd = 64;
							break;
						case 6:
							g_ui16NfcAFsd = 96;
							break;
						case 7:
							g_ui16NfcAFsd = 128;
							break;
						case 8:
							g_ui16NfcAFsd = 256;
							break;
						default:
							g_ui16NfcAFsd = 256;
							break;
					}

					ui8Offset = ATS_T0_IDX;

					// Check the T0 byte for TA support
					if(pui8RxBuffer[ATS_T0_IDX] & T0_TA_MASK)
					{
						ui8Offset++;

						sRwBitrates = NFC_RW_getRWSuportedBitrates();

						// Check for 848kbps support
						if(pui8RxBuffer[ui8Offset] & TA_PICC_TO_PCD_848_KBPS
							&& pui8RxBuffer[ui8Offset] & TA_PCD_TO_PICC_848_KBPS
							&& sRwBitrates.bits.bNfcA_848kbps == 1)
						{
							// send PPs to 848
							g_eNfcACurrentState = NFC_A_PPS_STATE_848;
						}
						// Check for 424kbps support
						else if(pui8RxBuffer[ui8Offset] & TA_PICC_TO_PCD_424_KBPS
								&& pui8RxBuffer[ui8Offset] & TA_PCD_TO_PICC_424_KBPS
								&& sRwBitrates.bits.bNfcA_424kbps == 1)
						{
							// send PPs to 424
							g_eNfcACurrentState = NFC_A_PPS_STATE_424;
						}
						// Check for 212kbps support
						else if(pui8RxBuffer[ui8Offset] & TA_PICC_TO_PCD_212_KBPS
								&& pui8RxBuffer[ui8Offset] & TA_PCD_TO_PICC_212_KBPS
								&& sRwBitrates.bits.bNfcA_212kbps == 1)
						{
							// send PPS to 212
							g_eNfcACurrentState = NFC_A_PPS_STATE_212;
						}
						else
						{
							// Do not send PPS
							g_eNfcACurrentState = NFC_A_ACTIVATED;
						}
					}

					// Check the T0 byte for TB support
					if(pui8RxBuffer[ATS_T0_IDX] & T0_TB_MASK)
					{
						ui8Offset++;

						if((pui8RxBuffer[ui8Offset] & TB_SFGI_MASK) == TB_SFGI_MASK)
						{
							// Have to check
							ui32TempTimeout = 0;
						}
						else
						{
							ui32TempTimeout = (uint32_t) (1 << (pui8RxBuffer[ui8Offset] & TB_SFGI_MASK));
						}

						// Set Max timeout

						// SFGT = (16*256/fc)*2^(SFGI)+384x2^(SFGI) (delta SFGT)
						ui32TempTimeout = ui32TempTimeout * 33;
						ui32TempTimeout = ui32TempTimeout / 100;
						ui32TempTimeout++;
						g_ui16NfcASfgt = (uint16_t)ui32TempTimeout;

						if((pui8RxBuffer[ui8Offset] & TB_FWI_MASK) == TB_FWI_MASK)
						{
							// Have to check
							ui32TempTimeout = 0;
						}
						else
						{
							ui32TempTimeout = (uint32_t)(pui8RxBuffer[ui8Offset] & TB_FWI_MASK);
							// The value is in upper nibble, therefeore we need to shift right by 4
							ui32TempTimeout = (ui32TempTimeout >> 4);
							// Calculate 2 ^ FWI
							ui32TempTimeout = (1 << ui32TempTimeout);
						}

						// Set Max timeout

						// FWT = (16*256/fc)*2^(FWI)+T_POLL(20ms)

						ui32TempTimeout = ui32TempTimeout * 30;
						ui32TempTimeout = ui32TempTimeout / 100;
						ui32TempTimeout = ui32TempTimeout + 20;

						g_ui16NfcAFwt = (uint16_t)ui32TempTimeout;

					}
					else
					{
						// Have to check
						g_ui16NfcASfgt = 1;

						// Default FWI = 4 ==> FWT = 25mS
						g_ui16NfcAFwt = 25;
					}

					// Check the T0 byte for TC support
					if(pui8RxBuffer[ATS_T0_IDX] & T0_TC_MASK)
					{
						ui8Offset++;

						// Bit 1 - CID support
						if(pui8RxBuffer[ui8Offset] & 0x02)
						{
							g_bNfcACIDSupport = true;
						}
						else
						{
							g_bNfcACIDSupport = false;
						}

						// Bit 0 - NAD support
						// not supported
					}
				}
			}
			else
			{
				if (ui8Length == 1)
				{
					if (pui8RxBuffer[0] != 0x01)
					{
						g_eNfcACurrentState = NFC_A_IDLE_STATE;
					}
					else
					{
						// Do nothing
					}
				}
				else
				{
					g_eNfcACurrentState = NFC_A_IDLE_STATE;
				}

				g_ui16NfcAFsd = 32;

				// Have to check
				g_ui16NfcASfgt = 1;

				// Default FWI = 4 ==> FWT = 25mS
				g_ui16NfcAFwt = 25;
			}

			// If the current state had not been modified, then change to activated state
			if(g_eNfcACurrentState == NFC_A_RATS_STATE)
			{
				g_eNfcACurrentState = NFC_A_ACTIVATED;
			}

			if(g_eNfcACurrentState != NFC_A_IDLE_STATE)
			{
				// Set ISO-DEP timeout
				ISODEP_setLinkTimeOut(g_ui16NfcAFwt);

				// Set ISO-DEP max payload
				ISODEP_setMaxPayload(g_ui16NfcAFsd);

				ISODEP_setNFCType(ISO_DEP_A);
			}

		}
		else
		{
			g_eNfcACurrentState = NFC_A_IDLE_STATE;
		}
		break;
	case NFC_A_PPS_STATE_212:
		if(g_bNfcACIDSupport == true && g_bNFCACIDEnable == true)
		{
			if(pui8RxBuffer[0] == (PPS_CMD | (g_ui8NfcACID & 0x0F)) )
			{
				g_eNfcACurrentState = NFC_A_ACTIVATED;
				sRwInitBitrates.ui16word = 0x00;
				sRwInitBitrates.bits.b212kpbs = 1;
				TRF79x0_incrementInitiatorBaudRate(sRwInitBitrates);
			}
			else
			{
				g_eNfcACurrentState = NFC_A_IDLE_STATE;
			}
		}
		else
		{
			if(pui8RxBuffer[0] == PPS_CMD)
			{
				g_eNfcACurrentState = NFC_A_ACTIVATED;
				sRwInitBitrates.ui16word = 0x00;
				sRwInitBitrates.bits.b212kpbs = 1;
				TRF79x0_incrementInitiatorBaudRate(sRwInitBitrates);
			}
			else
			{
				g_eNfcACurrentState = NFC_A_IDLE_STATE;
			}
		}
		break;
	case NFC_A_PPS_STATE_424:
		if(g_bNfcACIDSupport == true && g_bNFCACIDEnable == true)
		{
			if(pui8RxBuffer[0] == (PPS_CMD | (g_ui8NfcACID & 0x0F)) )
			{
				g_eNfcACurrentState = NFC_A_ACTIVATED;
				sRwInitBitrates.ui16word = 0x00;
				sRwInitBitrates.bits.b424kpbs = 1;
				TRF79x0_incrementInitiatorBaudRate(sRwInitBitrates);
			}
			else
			{
				g_eNfcACurrentState = NFC_A_IDLE_STATE;
			}
		}
		else
		{
			if(pui8RxBuffer[0] == PPS_CMD)
			{
				g_eNfcACurrentState = NFC_A_ACTIVATED;
				sRwInitBitrates.ui16word = 0x00;
				sRwInitBitrates.bits.b424kpbs = 1;
				TRF79x0_incrementInitiatorBaudRate(sRwInitBitrates);
			}
			else
			{
				g_eNfcACurrentState = NFC_A_IDLE_STATE;
			}
		}
		break;
	case NFC_A_PPS_STATE_848:
		if(g_bNfcACIDSupport == true && g_bNFCACIDEnable == true)
		{
			if(pui8RxBuffer[0] == (PPS_CMD | (g_ui8NfcACID & 0x0F)) )
			{
				g_eNfcACurrentState = NFC_A_ACTIVATED;
				sRwInitBitrates.ui16word = 0x00;
				sRwInitBitrates.bits.b848kpbs = 1;
				TRF79x0_incrementInitiatorBaudRate(sRwInitBitrates);
			}
			else
			{
				g_eNfcACurrentState = NFC_A_IDLE_STATE;
			}
		}
		else
		{
			if(pui8RxBuffer[0] == PPS_CMD)
			{
				g_eNfcACurrentState = NFC_A_ACTIVATED;
				sRwInitBitrates.ui16word = 0x00;
				sRwInitBitrates.bits.b848kpbs = 1;
				TRF79x0_incrementInitiatorBaudRate(sRwInitBitrates);
			}
			else
			{
				g_eNfcACurrentState = NFC_A_IDLE_STATE;
			}
		}
		break;
	case NFC_A_ACTIVATED:
		break;
	default:
		break;
	}

	return g_eNfcACurrentState;
}

//*****************************************************************************
//
//! NFC_A_setAutoSDD - This function sets the AutoSDD enable flag. (PICC Mode)
//!
//! \param bAutoSDD is the AutoSDD flag.
//!
//! This function sets the AutoSDD enable flag.
//!
//! \return None.
//
//****************************************************************************
void NFC_A_setAutoSDD(bool bAutoSDD)
{
	g_bAutoSDDEnabled = bAutoSDD;
}

//*****************************************************************************
//
//! NFC_A_setNfcAId - This function sets the UID of the NFC-A when PICC mode is
//! activated. (PICC Mode)
//!
//! \param ui8NewNfcAId is the pointer to the NFCAId.
//! \param ui8Length is the length of the UID. (Must be 4,7, or 10)
//!
//! This function sets the ID and the size of the PICC UID.
//!
//! \return None.
//
//****************************************************************************
void NFC_A_setNfcAId(uint8_t * ui8NewNfcAId, uint8_t ui8Length)
{
	int count;
	if (ui8Length == 10)
	{
		for (count = 0; count < 10; count++)
		{
			g_pui8NfcAId[count] = ui8NewNfcAId[count];	// Re-Write ISO14443 T4TA Tag
		}
		g_sNfcACEStatus.eUidLength = NFC_A_UID_TRIPLE;
	}
	else if (ui8Length == 7)
	{
		for (count = 0; count < 7; count++)
		{
			g_pui8NfcAId[count] = ui8NewNfcAId[count];	// Re-Write ISO14443 T4TA Tag
		}
		g_sNfcACEStatus.eUidLength = NFC_A_UID_DOUBLE;
	}
	else if (ui8Length == 4)
	{
		for (count = 0; count < 4; count++)
		{
			g_pui8NfcAId[count] = ui8NewNfcAId[count];	// Re-Write ISO14443 T4TA Tag
		}
		g_sNfcACEStatus.eUidLength = NFC_A_UID_SINGLE;
	}
	else
	{
		// Error, set NFC A Id Length to Incomplete
		g_sNfcACEStatus.eUidLength = NFC_A_UID_INCOMPLETE;
	}
}

//*****************************************************************************
//
//! NFC_A_getSFGT - This function returns the SFGT (PCD Mode)
//!
//! This function returns the g_ui16NfcASfgt.
//!
//! \return g_ui16NfcASfgt the guard time, the PCD must delay before sending
//! a command to the PICC.
//
//****************************************************************************
uint16_t NFC_A_getSFGT(void)
{
	return g_ui16NfcASfgt ;
}

//*****************************************************************************
//
//! NFC_A_getFWT - This function returns the FWT (PCD Mode)
//!
//! This function returns the g_ui16NfcAFwt.
//!
//! \return g_ui16NfcAFwt the max time the PCD should wait before timing out.
//
//****************************************************************************
uint16_t NFC_A_getFWT(void)
{
	return g_ui16NfcAFwt;
}

//*****************************************************************************
//
//! NFC_A_getSAK - This function returns the SAK response (PCD Mode)
//!
//! This function returns the g_ui8NfcASak.
//!
//! \return g_ui8NfcASak the PICC response to the SEL REQ from the PCD.
//
//****************************************************************************
uint8_t NFC_A_getSAK(void)
{
	return g_ui8NfcASak;
}

//*****************************************************************************
//
//! NFC_A_getNfcAId - This function returns the NFC-A UID (PICC or PCD Mode)
//!
//! \param pui8NfcAId is a double pointer to be modified with the NFCAId buffer.
//! \param ui8NfcAIdLength is a pointer to store the NFC ID Length.
//!
//! This function returns the NFC-A UID.
//!
//! \return None.
//
//****************************************************************************
void NFC_A_getNfcAId(uint8_t ** pui8NfcAId, uint8_t * ui8NfcAIdLength)
{
	*pui8NfcAId = g_pui8NfcAId;

	if(g_sRWNfcStatus.eUidLength)
	{
		*ui8NfcAIdLength = (uint8_t) g_sRWNfcStatus.eUidLength;
	}
	else
	{
		*ui8NfcAIdLength = (uint8_t) g_sNfcACEStatus.eUidLength;
	}
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
