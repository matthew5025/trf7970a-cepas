//*****************************************************************************
//
// nfc_rw_t2t.c - Implementation of T2T Reader/Writer
//
// Copyright (c) 2015 Texas Instruments Incorporated.  All rights reserved.
// TI Information - Selective Disclosure
//
//*****************************************************************************

#include "nfc_rw_t2t.h"

//*****************************************************************************
//
//! \addtogroup nfc_rw_t2t_api NFC Type 2 Reader API Functions
//! @{
//!
//! A Type 2 Reader/Writer issues commands based on the ISO14443-3 specifications.
//!
//! For more information on Type 2 Readers and Tags please read the NFC Forum
//! Type 2 Specifications.
//
//*****************************************************************************

//*****************************************************************************
//
// Stores the block number to be read/written from the T2T tag.
//
//*****************************************************************************
static uint8_t g_ui8T2TBlockNumber;

//*****************************************************************************
//
// Stores the T2T TX buffer.
//
//*****************************************************************************
static uint8_t g_pui8T2TBuffer[16];

//*****************************************************************************
//
// Stores the number of bytes received from the T2T tag.
//
//*****************************************************************************
static uint8_t g_ui8T2TBytesReceived;

//*****************************************************************************
//
// Stores the T2T TX data pointer.
//
//*****************************************************************************
static uint8_t * g_pui8T2TTxData;

//*****************************************************************************
//
// Stores the T2T RX data pointer.
//
//*****************************************************************************
static uint8_t * g_pui8T2TRxData;

//*****************************************************************************
//
// Stores the T2T Sector Number.
//
//*****************************************************************************
static uint8_t g_ui8SectorNumber;

//*****************************************************************************
//
// Stores the connection status for the T2T communication.
//
//*****************************************************************************
static tNfcRwT2TConnectionStatus g_eNfcRwT2TCommStatus;

//*****************************************************************************
//
// Stores the T2T time out counter. When it reaches 3 times, an error is
// sent to the Initiatior statemachine.
//
//*****************************************************************************
static uint8_t g_ui8T2TTimeOut;

//*****************************************************************************
//
//! NFC_RW_T2T_init - Initializes the variables for the Type 2 Reader/Writer.
//!
//! This function must be called prior to any other NFC_RW_T2T function in
//! in order to properly initialize all variables which have been declared.
//! Failure to do so may cause the software to enter unknown states.
//!
//! \return None.
//!
//
//*****************************************************************************
void NFC_RW_T2T_init(void)
{
	g_eNfcRwT2TCommStatus = NFC_RW_T2T_CONNECTION_IDLE;

	g_ui8T2TBlockNumber = 0x00;

	g_ui8T2TBytesReceived = 0x00;

	g_ui8SectorNumber = 0x00;

	g_ui8T2TTimeOut = 0x00;
}

//*****************************************************************************
//
//! NFC_RW_T2T_stateMachine - Conditions and transmits the data packets for
//!	Type 2 Reader commands.
//!
//! This function constructs the data packets for Type 2 commands based on the
//! current status. The packet is then transmitted at the end of the state
//! machine. Currently the supported Type 2 commands are: Read, Sector Select,
//! and Write.
//!
//! \return None.
//
//*****************************************************************************
void NFC_RW_T2T_stateMachine(void)
{
	uint8_t ui8Offset = 0x00;

	switch(g_eNfcRwT2TCommStatus)
	{
	case NFC_RW_T2T_CONNECTION_IDLE:
		// Dummy Read of the starting with block 0
		g_pui8T2TBuffer[ui8Offset++] = (uint8_t) T2T_READ_CMD;
		g_pui8T2TBuffer[ui8Offset++] = 0x00;
		break;
	case NFC_RW_T2T_CONNECTION_WRITE:

		TRF79x0_writeSingle(0x88,TRF79X0_ISO_CONTROL_REG);

		// Enable 4-bit replay on the TRF7970A (for example, ACK, NACK used by some cards)
		TRF79x0_writeSingle(0x04,TRF79X0_SPECIAL_FUNC_1_REG);

		g_pui8T2TBuffer[ui8Offset++] = (uint8_t) T2T_WRITE_CMD;

		g_pui8T2TBuffer[ui8Offset++] = g_ui8T2TBlockNumber;

		g_pui8T2TBuffer[ui8Offset++] = g_pui8T2TTxData[0];
		g_pui8T2TBuffer[ui8Offset++] = g_pui8T2TTxData[1];
		g_pui8T2TBuffer[ui8Offset++] = g_pui8T2TTxData[2];
		g_pui8T2TBuffer[ui8Offset++] = g_pui8T2TTxData[3];

		g_eNfcRwT2TCommStatus = NFC_RW_T2T_CONNECTION_WAIT_WRITE_RSP;
		break;
	case NFC_RW_T2T_CONNECTION_WAIT_WRITE_RSP:
		break;
	case NFC_RW_T2T_CONNECTION_WRITE_SUCCESS:
		break;
	case NFC_RW_T2T_CONNECTION_WRITE_FAIL:
		break;
	case NFC_RW_T2T_CONNECTION_READ:

		g_pui8T2TBuffer[ui8Offset++] = (uint8_t) T2T_READ_CMD;
		g_pui8T2TBuffer[ui8Offset++] = g_ui8T2TBlockNumber;

		g_eNfcRwT2TCommStatus = NFC_RW_T2T_CONNECTION_WAIT_READ_RSP;
		break;
	case NFC_RW_T2T_CONNECTION_WAIT_READ_RSP:
		break;
	case NFC_RW_T2T_CONNECTION_READ_SUCCESS:
		break;
	case NFC_RW_T2T_CONNECTION_READ_FAIL:
		break;
	case NFC_RW_T2T_CONNECTION_SECTOR_SELECT:
		g_pui8T2TBuffer[ui8Offset++] = (uint8_t) T2T_SECTOR_SEL_CMD;
		g_pui8T2TBuffer[ui8Offset++] = 0xFF;

		g_eNfcRwT2TCommStatus = NFC_RW_T2T_CONNECTION_WAIT_SECTOR_SELECT_RSP_PT1;
		break;
	case NFC_RW_T2T_CONNECTION_SECTOR_SELECT_PT2:
		g_pui8T2TBuffer[ui8Offset++] = g_ui8SectorNumber;
		// Bytes 2-4 RFU
		g_pui8T2TBuffer[ui8Offset++] = 0x00;
		g_pui8T2TBuffer[ui8Offset++] = 0x00;
		g_pui8T2TBuffer[ui8Offset++] = 0x00;

		g_eNfcRwT2TCommStatus = NFC_RW_T2T_CONNECTION_WAIT_SECTOR_SELECT_RSP;
		break;
	case NFC_RW_T2T_CONNECTION_WAIT_SECTOR_SELECT_RSP:
		break;
	case NFC_RW_T2T_CONNECTION_SECTOR_SELECT_SUCCESS:
		break;
	case  NFC_RW_T2T_CONNECTION_SECTOR_SELECT_FAIL:
		break;
	case NFC_RW_T2T_CONNECTION_SEND_SLEEP:
		// Send SLEEP Cmd
		g_pui8T2TBuffer[ui8Offset++] = 0x50;
		g_pui8T2TBuffer[ui8Offset++] = 0x00;

		g_eNfcRwT2TCommStatus = NFC_RW_T2T_CONNECTION_IDLE;
		break;
	default:
		break;
	}

	if(ui8Offset > 0)
	{
		TRF79x0_writeFIFO(g_pui8T2TBuffer,CRC_BIT_ENABLE,ui8Offset,0);

	}

}

//*****************************************************************************
//
//! NFC_RW_T2T_processReceivedData - Processes the Type 2 Data Received.
//!
//! \param pui8RxBuffer is the pointer to where T2T data can be received.
//! \param ui8Length is the length of the T2T data packet received.
//!
//! This function is used to handle the processing of all Type 2 data packets
//! that have been received from Type 3 tags. This function includes a timeout
//! monitor to ensure the software will not be locked up if the tag is
//! unexpectedly removed from the RF field.
//!
//! This function will wait for replies specific to the command that was issued.
//! If it receives an expected reply then it will indicated a successful response
//! and the application layer would then have to determine what command to issue
//! next. This gives developers full control over the sequencing of Type 2
//! Reader commands. If an unexpected reply is received, it will be treated as
//! a protocol error and the Initiator state machine will reset.
//!
//! \return g_eNfcRwT2TCommStatus returns the current status of the Type 2
//! Reader based on what response was received.
//
//*****************************************************************************
tNfcRwT2TConnectionStatus NFC_RW_T2T_processReceivedData(uint8_t * pui8RxBuffer, uint8_t ui8Length)
{
	if(ui8Length == 0x00)
	{
		g_ui8T2TTimeOut++;
	}
	else
	{
		g_ui8T2TTimeOut = 0x00;
	}

	if(g_ui8T2TTimeOut == 3)
	{
		g_eNfcRwT2TCommStatus = NFC_RW_T2T_PROTOCOL_ERROR;
	}

	switch(g_eNfcRwT2TCommStatus)
	{
	case NFC_RW_T2T_CONNECTION_IDLE:

		g_ui8T2TBytesReceived = ui8Length;

		g_pui8T2TRxData = pui8RxBuffer;

		break;
	case NFC_RW_T2T_CONNECTION_WRITE:
		break;
	case NFC_RW_T2T_CONNECTION_WAIT_WRITE_RSP:
		g_ui8T2TBytesReceived = ui8Length;

		g_pui8T2TRxData = pui8RxBuffer;

		TRF79x0_writeSingle(0x08,TRF79X0_ISO_CONTROL_REG);

		TRF79x0_writeSingle(0x00,TRF79X0_SPECIAL_FUNC_1_REG);

		if(ui8Length == 1)
		{
			if(pui8RxBuffer[0] == T2T_ACK_RSP || pui8RxBuffer[0] == 0xA0)
			{
				g_eNfcRwT2TCommStatus = NFC_RW_T2T_CONNECTION_WRITE_SUCCESS;
			}
			else if(pui8RxBuffer[0] == 0x00 || (pui8RxBuffer[0] & T2T_NACK_RSP))
			{
				g_eNfcRwT2TCommStatus = NFC_RW_T2T_PROTOCOL_ERROR;
			}
			else
			{
				g_eNfcRwT2TCommStatus = NFC_RW_T2T_CONNECTION_WRITE_FAIL;
			}
		}
		else
		{
			g_eNfcRwT2TCommStatus = NFC_RW_T2T_CONNECTION_WRITE_FAIL;
		}

		break;
	case NFC_RW_T2T_CONNECTION_WRITE_SUCCESS:
		break;
	case NFC_RW_T2T_CONNECTION_WRITE_FAIL:
		break;
	case NFC_RW_T2T_CONNECTION_READ:
		break;
	case NFC_RW_T2T_CONNECTION_WAIT_READ_RSP:
		g_ui8T2TBytesReceived = ui8Length;

		g_pui8T2TRxData = pui8RxBuffer;

		if(ui8Length == 1)
		{
			if(pui8RxBuffer[0] == 0x00 || (pui8RxBuffer[0] & T2T_NACK_RSP))
			{
				g_eNfcRwT2TCommStatus = NFC_RW_T2T_PROTOCOL_ERROR;
			}
			else
			{
				g_eNfcRwT2TCommStatus = NFC_RW_T2T_CONNECTION_READ_FAIL;
			}
		}
		else if(ui8Length == 16)
		{
			g_eNfcRwT2TCommStatus = NFC_RW_T2T_CONNECTION_READ_SUCCESS;
		}
		else
		{
			g_eNfcRwT2TCommStatus = NFC_RW_T2T_CONNECTION_READ_FAIL;
		}
		break;
	case NFC_RW_T2T_CONNECTION_READ_SUCCESS:
		break;
	case NFC_RW_T2T_CONNECTION_READ_FAIL:
		break;
	case NFC_RW_T2T_CONNECTION_SECTOR_SELECT:
		break;
	case NFC_RW_T2T_CONNECTION_WAIT_SECTOR_SELECT_RSP_PT1:
		if(ui8Length == 1)
		{
			if(pui8RxBuffer[0] == T2T_ACK_RSP)
			{
				g_eNfcRwT2TCommStatus = NFC_RW_T2T_CONNECTION_SECTOR_SELECT_PT2;
			}
			else
			{
				g_eNfcRwT2TCommStatus = NFC_RW_T2T_PROTOCOL_ERROR;
			}
		}
		else
		{
			g_eNfcRwT2TCommStatus = NFC_RW_T2T_PROTOCOL_ERROR;
		}
		break;
	case NFC_RW_T2T_CONNECTION_SECTOR_SELECT_PT2:
		break;
	case NFC_RW_T2T_CONNECTION_WAIT_SECTOR_SELECT_RSP:
		g_ui8T2TBytesReceived = ui8Length;

		g_pui8T2TRxData = pui8RxBuffer;

		if(ui8Length == 1)
		{
			if(pui8RxBuffer[0] == T2T_ACK_RSP)
			{
				g_eNfcRwT2TCommStatus = NFC_RW_T2T_CONNECTION_SECTOR_SELECT_SUCCESS;
			}
			else if(pui8RxBuffer[0] == 0x00 || (pui8RxBuffer[0] & T2T_NACK_RSP))
			{
				g_eNfcRwT2TCommStatus = NFC_RW_T2T_PROTOCOL_ERROR;
			}
			else
			{
				g_eNfcRwT2TCommStatus = NFC_RW_T2T_CONNECTION_SECTOR_SELECT_FAIL;
			}
		}
		else
		{
			g_eNfcRwT2TCommStatus = NFC_RW_T2T_CONNECTION_SECTOR_SELECT_FAIL;
		}
		break;
	case NFC_RW_T2T_CONNECTION_SECTOR_SELECT_SUCCESS:
		break;
	case  NFC_RW_T2T_CONNECTION_SECTOR_SELECT_FAIL:
		break;
	default:
		break;
	}

	return g_eNfcRwT2TCommStatus;
}

//*****************************************************************************
//
//! NFC_RW_T2T_getWriteStatus - Checks for a Write response.
//!
//! This function checks if a Write response was received, and returns
//! the Type 2 Reader status of either a SUCCESS or a FAIL. If a Write
//! response has been received, the current status will be changed to IDLE
//! regardless of whether the Type 2 Reader status was a SUCCESS or FAIL.
//!
//! \return eStatusTemp is the Type 2 Reader status.
//
//*****************************************************************************
tNfcRwT2TConnectionStatus NFC_RW_T2T_getWriteStatus(void)
{
	tNfcRwT2TConnectionStatus eStatusTemp;

	if(g_eNfcRwT2TCommStatus == NFC_RW_T2T_CONNECTION_WRITE_SUCCESS)
	{
		eStatusTemp = g_eNfcRwT2TCommStatus;
		// Clear Current State
		g_eNfcRwT2TCommStatus = NFC_RW_T2T_CONNECTION_IDLE;
	}
	else if(g_eNfcRwT2TCommStatus == NFC_RW_T2T_CONNECTION_WRITE_FAIL)
	{
		eStatusTemp = g_eNfcRwT2TCommStatus;
		// Clear Current State
		g_eNfcRwT2TCommStatus = NFC_RW_T2T_CONNECTION_IDLE;
	}

	return eStatusTemp;
}

//*****************************************************************************
//
//! NFC_RW_T2T_getReadStatus - Checks for a Read response.
//!
//! This function checks if a Read response was received, and returns
//! the Type 2 Reader status of either a SUCCESS or a FAIL. If a Read
//! response has been received, the current status will be changed to IDLE
//! regardless of whether the Type 2 Reader status was a SUCCESS or FAIL.
//!
//! \return eStatusTemp is the Type 2 Reader status.
//
//*****************************************************************************
tNfcRwT2TConnectionStatus NFC_RW_T2T_getReadStatus(void)
{
	tNfcRwT2TConnectionStatus eStatusTemp;

	if(g_eNfcRwT2TCommStatus == NFC_RW_T2T_CONNECTION_READ_SUCCESS)
	{
		eStatusTemp = g_eNfcRwT2TCommStatus;
		// Clear Current State
		g_eNfcRwT2TCommStatus = NFC_RW_T2T_CONNECTION_IDLE;
	}
	else if(g_eNfcRwT2TCommStatus == NFC_RW_T2T_CONNECTION_READ_FAIL)
	{
		eStatusTemp = g_eNfcRwT2TCommStatus;
		// Clear Current State
		g_eNfcRwT2TCommStatus = NFC_RW_T2T_CONNECTION_IDLE;
	}

	return eStatusTemp;
}

//*****************************************************************************
//
//! NFC_RW_T2T_getSectorSelStatus - Checks for a Sector Sel response.
//!
//! This function checks if a Sector Sel response was received, and returns
//! the Type 2 Reader status of either a SUCCESS or a FAIL. If a Sector Sel
//! response has been received, the current status will be changed to IDLE
//! regardless of whether the Type 2 Reader status was a SUCCESS or FAIL.
//!
//! \return eStatusTemp is the Type 2 Reader status.
//
//*****************************************************************************
tNfcRwT2TConnectionStatus NFC_RW_T2T_getSectorSelStatus(void)
{
	tNfcRwT2TConnectionStatus eStatusTemp;

	if(g_eNfcRwT2TCommStatus == NFC_RW_T2T_CONNECTION_SECTOR_SELECT_SUCCESS)
	{
		eStatusTemp = g_eNfcRwT2TCommStatus;
		// Clear Current State
		g_eNfcRwT2TCommStatus = NFC_RW_T2T_CONNECTION_IDLE;
	}
	else if(g_eNfcRwT2TCommStatus == NFC_RW_T2T_CONNECTION_SECTOR_SELECT_FAIL)
	{
		eStatusTemp = g_eNfcRwT2TCommStatus;
		// Clear Current State
		g_eNfcRwT2TCommStatus = NFC_RW_T2T_CONNECTION_IDLE;
	}

	return eStatusTemp;
}

//*****************************************************************************
//
//! NFC_RW_T2T_sendReadCmd - Sends the Read Command.
//!
//! \param ui8BlockNumber is the block number to be read.
//!
//! This function is used set the correct state for sending a Update
//! command, and also to process any application based variables that have
//! been passed into the Type 2 Reader layer.
//!
//! \return ui8Status indicates if the state was successfully changed or not.
//
//*****************************************************************************
uint8_t NFC_RW_T2T_sendReadCmd(uint8_t ui8BlockNumber)
{
	uint8_t ui8Status = STATUS_SUCCESS;

	if(g_eNfcRwT2TCommStatus == NFC_RW_T2T_CONNECTION_IDLE)
	{
		g_eNfcRwT2TCommStatus = NFC_RW_T2T_CONNECTION_READ;

		g_ui8T2TBlockNumber = ui8BlockNumber;
	}
	else
	{
		ui8Status = STATUS_FAIL;
	}

	return ui8Status;
}

//*****************************************************************************
//
//! NFC_RW_T2T_sendWriteCmd - Sends the Write Command.
//!
//! \param ui8BlockNumber is the block number to be written.
//! \param pui8Buffer is a pointer to the data to be written
//! \param ui8Length is the length of the packet to be written.
//!
//! This function is used set the correct state for sending a Write
//! command, and also to process any application based variables that have
//! been passed into the Type 2 Reader layer.
//!
//! \return ui8Status indicates if the state was successfully changed or not.
//
//*****************************************************************************
uint8_t NFC_RW_T2T_sendWriteCmd(uint8_t ui8BlockNumber, const uint8_t * pui8Buffer, uint8_t ui8Length)
{
	uint8_t ui8Status = STATUS_SUCCESS;

	if(g_eNfcRwT2TCommStatus == NFC_RW_T2T_CONNECTION_IDLE && ui8Length == 0x04)
	{
		g_eNfcRwT2TCommStatus = NFC_RW_T2T_CONNECTION_WRITE;

		g_ui8T2TBlockNumber = ui8BlockNumber;

		g_pui8T2TTxData = (uint8_t *)pui8Buffer;
	}
	else
	{
		ui8Status = STATUS_FAIL;
	}

	return ui8Status;
}

//*****************************************************************************
//
//! NFC_RW_T2T_sendSectorSel - Sends the Sector Sel Command.
//!
//! \param ui8SectorNumber is the sector number to select.
//!
//! This function is used set the correct state for sending a Sector Select
//! command, and also to process any application based variables that have
//! been passed into the Type 2 Reader layer.
//!
//! \return ui8Status indicates if the state was successfully changed or not.
//
//*****************************************************************************
uint8_t NFC_RW_T2T_sendSectorSel(uint8_t ui8SectorNumber)
{
	uint8_t ui8Status = STATUS_SUCCESS;

	if(g_eNfcRwT2TCommStatus == NFC_RW_T2T_CONNECTION_IDLE && ui8SectorNumber != 0xFF)
	{
		g_ui8SectorNumber = ui8SectorNumber;

		g_eNfcRwT2TCommStatus = NFC_RW_T2T_CONNECTION_SECTOR_SELECT;

	}
	else
	{
		ui8Status = STATUS_FAIL;
	}

	return ui8Status;
}

//*****************************************************************************
//
//! NFC_RW_T2T_sendSleep - Sends the Sleep Command.
//!
//! This function is used set the correct state for sending a Sleep
//! command, and also to process any application based variables that have
//! been passed into the Type 2 Reader layer.
//!
//! \return ui8Status indicates if the state was successfully changed or not.
//
//*****************************************************************************
uint8_t NFC_RW_T2T_sendSleep(void)
{
	uint8_t ui8Status = STATUS_SUCCESS;

	if(g_eNfcRwT2TCommStatus == NFC_RW_T2T_CONNECTION_IDLE)
	{
		g_eNfcRwT2TCommStatus = NFC_RW_T2T_CONNECTION_SEND_SLEEP;
	}
	else
	{
		ui8Status = STATUS_FAIL;
	}

	return ui8Status;
}

//*****************************************************************************
//
//! NFC_RW_T2T_getPacketStatus - Gets the last Type 2 data packet received.
//!
//! \param pui8RXData this is a double pointer to receive the current pointer
//! to the location of the Type 2 data bytes received buffer
//! \param pui8Length this is a pointer to get the length of the received Type
//! 2 data.
//!
//! This function returns the packet length and the location of the packet
//! contents of the latest Type 2 data packet that was received.
//!
//! \return None.
//
//*****************************************************************************
void NFC_RW_T2T_getPacketStatus(uint8_t ** pui8RXData, uint8_t * pui8Length)
{
	*pui8Length = g_ui8T2TBytesReceived;

	*pui8RXData = g_pui8T2TRxData;
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
