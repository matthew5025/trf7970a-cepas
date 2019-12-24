//*****************************************************************************
//
// nfc_rw_t3t.c - Implementation of T3T Reader/Writer
//
// Copyright (c) 2015 Texas Instruments Incorporated.  All rights reserved.
// TI Information - Selective Disclosure
//
//*****************************************************************************

#include "nfc_rw_t3t.h"

//*****************************************************************************
//
//! \addtogroup nfc_rw_t3t_api NFC Type 3 Reader API Functions
//! @{
//!
//! A Type 3 Reader/Writer issues commands based on the FeliCa specifications.
//!
//! For more information on Type 3 Readers and Tags please read the FeliCa
//! Specifications.
//
//*****************************************************************************

//*****************************************************************************
//
// Stores the T3T TX buffer.
//
//*****************************************************************************
static uint8_t g_pui8T3TBuffer[256];

//*****************************************************************************
//
// Stores the number of bytes received from the T3T tag.
//
//*****************************************************************************
static uint8_t g_ui8T3TBytesReceived;

//*****************************************************************************
//
// Stores the T3T RX data pointer.
//
//*****************************************************************************
static uint8_t * g_pui8T3TRxData;

//*****************************************************************************
//
// Stores the T3T TX packet information
//
//*****************************************************************************
static tT3TPacketData g_psT3TTxData;

//*****************************************************************************
//
// Stores the connection status for the T3T communication.
//
//*****************************************************************************
static tNfcRwT3TConnectionStatus g_eNfcRwT3TCommStatus;

//*****************************************************************************
//
// Stores the T3T time out counter. When it reaches 3 times, an error is
// sent to the Initiatior statemachine.
//
//*****************************************************************************
static uint8_t g_ui8T3TTimeOut;

//*****************************************************************************
//
//! NFC_RW_T3T_init - Initializes the variables for the Type 3 Reader/Writer.
//!
//! This function must be called prior to any other NFC_RW_T3T function in
//! in order to properly initialize all variables which have been declared.
//! Failure to do so may cause the software to enter unknown states.
//!
//! \return None
//!
//
//*****************************************************************************
void NFC_RW_T3T_init(void)
{
	g_eNfcRwT3TCommStatus = NFC_RW_T3T_CONNECTION_IDLE;

	g_ui8T3TBytesReceived = 0x00;

	g_ui8T3TTimeOut = 0x00;
}

//*****************************************************************************
//
//! NFC_RW_T3T_stateMachine - Conditions and transmits the data packets for
//!	Type 3 Reader commands.
//!
//! This function constructs the data packets for Type 3 commands based on the
//! current status. The packet is then transmitted at the end of the state
//! machine. Currently the supported Type 3 commands are: Check, Update, and
//! SENSF_REQ.
//!
//! \return None.
//
//*****************************************************************************
void NFC_RW_T3T_stateMachine(void)
{
	uint8_t ui8Offset = 0x01;
	uint8_t * pui8NfcFId;
	uint8_t ui8NfcFIdLength;
	uint8_t TempCount;

	switch(g_eNfcRwT3TCommStatus)
	{
	case NFC_RW_T3T_CONNECTION_IDLE:
		NFC_F_getNFCID2(&pui8NfcFId,&ui8NfcFIdLength);

		g_pui8T3TBuffer[ui8Offset++] = T3T_CHECK_CMD;

		for (TempCount = 0; TempCount < 8; TempCount++)
		{
			g_pui8T3TBuffer[ui8Offset++] = pui8NfcFId[TempCount];
		}

		// Service Length
		g_pui8T3TBuffer[ui8Offset++] = 0x01;

		g_pui8T3TBuffer[ui8Offset++] = 0x09;
		g_pui8T3TBuffer[ui8Offset++] = 0x00;

		// Block Len
		g_pui8T3TBuffer[ui8Offset++] = 0x01;

		g_pui8T3TBuffer[ui8Offset++] = 0x80;
		g_pui8T3TBuffer[ui8Offset++] = 0x00;

		// Set Length
		g_pui8T3TBuffer[0] = ui8Offset;

		break;
	case NFC_RW_T3T_CONNECTION_UPDATE:
		NFC_F_getNFCID2(&pui8NfcFId,&ui8NfcFIdLength);

		g_pui8T3TBuffer[ui8Offset++] = T3T_UPDATE_CMD;

		for (TempCount = 0; TempCount < 8; TempCount++)
		{
			g_pui8T3TBuffer[ui8Offset++] = pui8NfcFId[TempCount];
		}

		g_pui8T3TBuffer[ui8Offset++] = g_psT3TTxData.ui8ServiceCount;

		for (TempCount = 0; TempCount < (g_psT3TTxData.ui8ServiceCount * 2); TempCount++)
		{
			g_pui8T3TBuffer[ui8Offset++] = g_psT3TTxData.pui8Services[TempCount];
		}

		g_pui8T3TBuffer[ui8Offset++] = g_psT3TTxData.ui8BlockLen;

		if(g_psT3TTxData.bThreeByteBlock)
		{
			g_psT3TTxData.ui8BlockLen = g_psT3TTxData.ui8BlockLen * 3;
		}
		else
		{
			g_psT3TTxData.ui8BlockLen = g_psT3TTxData.ui8BlockLen * 2;
		}

		for (TempCount = 0; TempCount < g_psT3TTxData.ui8BlockLen; TempCount++)
		{
			g_pui8T3TBuffer[ui8Offset++] = g_psT3TTxData.pui8Blocks[TempCount];
		}

		for (TempCount = 0; TempCount < g_psT3TTxData.ui8DataLen; TempCount++)
		{
			g_pui8T3TBuffer[ui8Offset++] = g_psT3TTxData.pui8Data[TempCount];
		}

		// Set Length
		g_pui8T3TBuffer[0] = ui8Offset;

		g_eNfcRwT3TCommStatus = NFC_RW_T3T_CONNECTION_WAIT_UPDATE_RSP;
		break;
	case NFC_RW_T3T_CONNECTION_WAIT_UPDATE_RSP:
		break;
	case NFC_RW_T3T_CONNECTION_UPDATE_SUCCESS:
		break;
	case NFC_RW_T3T_CONNECTION_UPDATE_FAIL:
		break;
	case NFC_RW_T3T_CONNECTION_CHECK:
		NFC_F_getNFCID2(&pui8NfcFId,&ui8NfcFIdLength);

		g_pui8T3TBuffer[ui8Offset++] = T3T_CHECK_CMD;

		for (TempCount = 0; TempCount < 8; TempCount++)
		{
			g_pui8T3TBuffer[ui8Offset++] = pui8NfcFId[TempCount];
		}

		g_pui8T3TBuffer[ui8Offset++] = g_psT3TTxData.ui8ServiceCount;

		for (TempCount = 0; TempCount < (g_psT3TTxData.ui8ServiceCount * 2); TempCount++)
		{
			g_pui8T3TBuffer[ui8Offset++] = g_psT3TTxData.pui8Services[TempCount];
		}

		g_pui8T3TBuffer[ui8Offset++] = g_psT3TTxData.ui8BlockLen;

		if(g_psT3TTxData.bThreeByteBlock)
		{
			g_psT3TTxData.ui8BlockLen = g_psT3TTxData.ui8BlockLen * 3;
		}
		else
		{
			g_psT3TTxData.ui8BlockLen = g_psT3TTxData.ui8BlockLen * 2;
		}

		for (TempCount = 0; TempCount < g_psT3TTxData.ui8BlockLen; TempCount++)
		{
			g_pui8T3TBuffer[ui8Offset++] = g_psT3TTxData.pui8Blocks[TempCount];
		}

		// Set Length
		g_pui8T3TBuffer[0] = ui8Offset;

		g_eNfcRwT3TCommStatus = NFC_RW_T3T_CONNECTION_WAIT_CHECK_RSP;
		break;
	case NFC_RW_T3T_CONNECTION_WAIT_CHECK_RSP:
		break;
	case NFC_RW_T3T_CONNECTION_CHECK_SUCCESS:
		break;
	case NFC_RW_T3T_CONNECTION_CHECK_FAIL:
		break;
	case NFC_RW_T3T_CONNECTION_SENSF_REQ:
		// Length of the SENSF_REQ = 0x06
		ui8Offset = 0x06;
		g_eNfcRwT3TCommStatus = NFC_RW_T3T_CONNECTION_WAIT_SENSF_REQ_RSP;
		break;
	case NFC_RW_T3T_CONNECTION_WAIT_SENSF_REQ_RSP:
		break;
	case NFC_RW_T3T_CONNECTION_SENSF_REQ_SUCCESS:
		break;
	case NFC_RW_T3T_CONNECTION_SENSF_REQ_FAIL:
		break;
	default:
		break;
	}

	if(ui8Offset > 0)
	{
		TRF79x0_writeFIFO(g_pui8T3TBuffer,CRC_BIT_ENABLE,ui8Offset,0);
	}

}

//*****************************************************************************
//
//! NFC_RW_T3T_processReceivedData - Processes the Type 3 Data Received.
//!
//! \param pui8RxBuffer is the pointer to where T3T data can be received.
//! \param ui8Length is the length of the T3T data packet received.
//!
//! This function is used to handle the processing of all Type 3 data packets
//! that have been received from Type 3 tags. This function includes a timeout
//! monitor to ensure the software will not be locked up if the tag is
//! unexpectedly removed from the RF field.
//!
//! This function will wait for replies specific to the command that was issued.
//! If it receives an expected reply then it will indicated a successful response
//! and the application layer would then have to determine what command to issue
//! next. This gives developers full control over the sequencing of Type 3
//! Reader commands. If an unexpected reply is received, it will be treated as
//! a protocol error and the Initiator state machine will reset.
//!
//! \return g_eNfcRwT3TCommStatus returns the current status of the Type 3
//! Reader based on what response was received.
//
//*****************************************************************************
tNfcRwT3TConnectionStatus NFC_RW_T3T_processReceivedData(uint8_t * pui8RxBuffer, uint8_t ui8Length)
{
	if(ui8Length == 0x00)
	{
		g_ui8T3TTimeOut++;
	}
	else
	{
		g_ui8T3TTimeOut = 0x00;
	}

	if(g_ui8T3TTimeOut == 3)
	{
		g_eNfcRwT3TCommStatus = NFC_RW_T3T_PROTOCOL_ERROR;
	}


	// Type F - FDT(F,POLL,MIN)
	MCU_delayMicrosecond(728);

	switch(g_eNfcRwT3TCommStatus)
	{
	case NFC_RW_T3T_CONNECTION_IDLE:

		g_ui8T3TBytesReceived = ui8Length;

		g_pui8T3TRxData = pui8RxBuffer;

		break;
	case NFC_RW_T3T_CONNECTION_UPDATE:
		break;
	case NFC_RW_T3T_CONNECTION_WAIT_UPDATE_RSP:
		g_ui8T3TBytesReceived = ui8Length;

		g_pui8T3TRxData = pui8RxBuffer;

		if(ui8Length == 0x0C)
		{
			if(pui8RxBuffer[1] == T3T_UPDATE_RES)
			{
				g_eNfcRwT3TCommStatus = NFC_RW_T3T_CONNECTION_UPDATE_SUCCESS;
			}
			else
			{
				g_eNfcRwT3TCommStatus = NFC_RW_T3T_CONNECTION_UPDATE_FAIL;
			}
		}
		else
		{
			g_eNfcRwT3TCommStatus = NFC_RW_T3T_CONNECTION_UPDATE_FAIL;
		}

		break;
	case NFC_RW_T3T_CONNECTION_UPDATE_SUCCESS:
		break;
	case NFC_RW_T3T_CONNECTION_UPDATE_FAIL:
		break;
	case NFC_RW_T3T_CONNECTION_CHECK:
		break;
	case NFC_RW_T3T_CONNECTION_WAIT_CHECK_RSP:
		g_ui8T3TBytesReceived = ui8Length;

		g_pui8T3TRxData = pui8RxBuffer;

		if(ui8Length > 0)
		{
			if(pui8RxBuffer[1] == T3T_CHECK_RES)
			{
				g_eNfcRwT3TCommStatus = NFC_RW_T3T_CONNECTION_CHECK_SUCCESS;
			}
			else
			{
				g_eNfcRwT3TCommStatus = NFC_RW_T3T_CONNECTION_CHECK_FAIL;
			}
		}
		else
		{
			g_eNfcRwT3TCommStatus = NFC_RW_T3T_CONNECTION_CHECK_FAIL;
		}
		break;
	case NFC_RW_T3T_CONNECTION_CHECK_SUCCESS:
		break;
	case NFC_RW_T3T_CONNECTION_CHECK_FAIL:
		break;
	case NFC_RW_T3T_CONNECTION_SENSF_REQ:
		break;
	case NFC_RW_T3T_CONNECTION_WAIT_SENSF_REQ_RSP:
		g_ui8T3TBytesReceived = ui8Length;

		g_pui8T3TRxData = pui8RxBuffer;

		g_eNfcRwT3TCommStatus = NFC_RW_T3T_CONNECTION_SENSF_REQ_SUCCESS;

		break;
	case NFC_RW_T3T_CONNECTION_SENSF_REQ_SUCCESS:
		break;
	case NFC_RW_T3T_CONNECTION_SENSF_REQ_FAIL:
		break;
	default:
		break;
	}

	return g_eNfcRwT3TCommStatus;
}

//*****************************************************************************
//
//! NFC_RW_T3T_getUpdateStatus - Checks for a Update response.
//!
//! This function checks if a Update response was received, and returns
//! the Type 3 Reader status of either a SUCCESS or a FAIL. If a Read Single
//! response has been received, the current status will be changed to IDLE
//! regardless of whether the Type 3 Reader status was a SUCCESS or FAIL.
//!
//! \return eStatusTemp is the Type 3 Reader status.
//
//*****************************************************************************
tNfcRwT3TConnectionStatus NFC_RW_T3T_getUpdateStatus(void)
{
	tNfcRwT3TConnectionStatus eStatusTemp;

	if(g_eNfcRwT3TCommStatus == NFC_RW_T3T_CONNECTION_UPDATE_SUCCESS)
	{
		eStatusTemp = g_eNfcRwT3TCommStatus;
		// Clear Current State
		g_eNfcRwT3TCommStatus = NFC_RW_T3T_CONNECTION_IDLE;
	}
	else if(g_eNfcRwT3TCommStatus == NFC_RW_T3T_CONNECTION_UPDATE_FAIL)
	{
		eStatusTemp = g_eNfcRwT3TCommStatus;
		// Clear Current State
		g_eNfcRwT3TCommStatus = NFC_RW_T3T_CONNECTION_IDLE;
	}

	return eStatusTemp;
}

//*****************************************************************************
//
//! NFC_RW_T3T_getCheckStatus - Checks for a check response.
//!
//! This function checks if a Check response was received, and returns
//! the Type 3 Reader status of either a SUCCESS or a FAIL. If a Check
//! response has been received, the current status will be changed to IDLE
//! regardless of whether the Type 3 Reader status was a SUCCESS or FAIL.
//!
//! \return eStatusTemp is the Type 3 Reader status.
//
//*****************************************************************************
tNfcRwT3TConnectionStatus NFC_RW_T3T_getCheckStatus(void)
{
	tNfcRwT3TConnectionStatus eStatusTemp;

	if(g_eNfcRwT3TCommStatus == NFC_RW_T3T_CONNECTION_CHECK_SUCCESS)
	{
		eStatusTemp = g_eNfcRwT3TCommStatus;
		// Clear Current State
		g_eNfcRwT3TCommStatus = NFC_RW_T3T_CONNECTION_IDLE;
	}
	else if(g_eNfcRwT3TCommStatus == NFC_RW_T3T_CONNECTION_CHECK_FAIL)
	{
		eStatusTemp = g_eNfcRwT3TCommStatus;
		// Clear Current State
		g_eNfcRwT3TCommStatus = NFC_RW_T3T_CONNECTION_IDLE;
	}

	return eStatusTemp;
}

//*****************************************************************************
//
//! NFC_RW_T3T_getSensFStatus - Checks for a SenF response.
//!
//! This function checks if a SENSF response was received, and returns
//! the Type 3 Reader status of either a SUCCESS or a FAIL. If a SENSF
//! response has been received, the current status will be changed to IDLE
//! regardless of whether the Type 3 Reader status was a SUCCESS or FAIL.
//!
//! \return eStatusTemp is the Type 3 Reader status.
//
//*****************************************************************************
tNfcRwT3TConnectionStatus NFC_RW_T3T_getSensFStatus(void)
{
	tNfcRwT3TConnectionStatus eStatusTemp;

	if(g_eNfcRwT3TCommStatus == NFC_RW_T3T_CONNECTION_SENSF_REQ_SUCCESS)
	{
		eStatusTemp = g_eNfcRwT3TCommStatus;
		// Clear Current State
		g_eNfcRwT3TCommStatus = NFC_RW_T3T_CONNECTION_IDLE;
	}
	else if(g_eNfcRwT3TCommStatus == NFC_RW_T3T_CONNECTION_SENSF_REQ_FAIL)
	{
		eStatusTemp = g_eNfcRwT3TCommStatus;
		// Clear Current State
		g_eNfcRwT3TCommStatus = NFC_RW_T3T_CONNECTION_IDLE;
	}

	return eStatusTemp;
}

//*****************************************************************************
//
//! NFC_RW_T3T_sendCheckCmd - Sends the Check Command.
//!
//! \param sT3TData is the Check command to be sent with the Type 3 command
//!
//! This function is used set the correct state for sending a Check
//! command, and also to process any application based variables that have
//! been passed into the Type 3 Reader layer.
//!
//! \return ui8Status indicates if the state was successfully changed or not.
//
//*****************************************************************************
uint8_t NFC_RW_T3T_sendCheckCmd(tT3TPacketData sT3TData)
{
	uint8_t ui8Status = STATUS_SUCCESS;

	if(g_eNfcRwT3TCommStatus == NFC_RW_T3T_CONNECTION_IDLE)
	{
		g_eNfcRwT3TCommStatus = NFC_RW_T3T_CONNECTION_CHECK;

		g_psT3TTxData.pui8Services = sT3TData.pui8Services;
		g_psT3TTxData.ui8ServiceCount = sT3TData.ui8ServiceCount;
		g_psT3TTxData.pui8Blocks = sT3TData.pui8Blocks;
		g_psT3TTxData.ui8BlockLen = sT3TData.ui8BlockLen;
	}
	else
	{
		ui8Status = STATUS_FAIL;
	}

	return ui8Status;
}

//*****************************************************************************
//
//! NFC_RW_T3T_sendSensFReq - Sends the SENSF Req Command.
//!
//! \param ui16SC is the System Code of the SENSF Req.
//! \param ui8RC is the Request Code of the SENSF Req.
//! \param ui8Slot is the Time Slot Number of the SENSF Req.
//!
//! This function is used set the correct state for sending a SENSF Req.
//! command, and also to process any application based variables that have
//! been passed into the Type 3 Reader layer.
//!
//! \return ui8Status indicates if the state was successfully changed or not.
//
//*****************************************************************************
uint8_t NFC_RW_T3T_sendSensFReq(uint16_t ui16SC, uint8_t ui8RC, uint8_t ui8Slot)
{
	uint8_t ui8Status = STATUS_SUCCESS;

	if(g_eNfcRwT3TCommStatus == NFC_RW_T3T_CONNECTION_IDLE)
	{
		g_eNfcRwT3TCommStatus = NFC_RW_T3T_CONNECTION_SENSF_REQ;

		//
		// Length
		//
		g_pui8T3TBuffer[0] = 0x06;
		//
		// SENSF_REQ Command
		//
		g_pui8T3TBuffer[1] = 0x00;

		g_pui8T3TBuffer[2] = (uint8_t) (ui16SC >> 8);		// System Code (SC) 7:0
		g_pui8T3TBuffer[3] = (uint8_t) (ui16SC & 0xFF);		// System Code (SC) 15:8

		g_pui8T3TBuffer[4] = ui8RC;		// Request Code (RC)

		g_pui8T3TBuffer[5] = ui8Slot;		// Time Slot Number (TSN) (DP, Table 42, 4 time slots)
	}
	else
	{
		ui8Status = STATUS_FAIL;
	}

	return ui8Status;
}

//*****************************************************************************
//
//! NFC_RW_T3T_sendUpdateCmd - Sends the Update Command.
//!
//! \param sT3TData is the Update command to be sent with the Type 3 command
//!
//! This function is used set the correct state for sending a Update
//! command, and also to process any application based variables that have
//! been passed into the Type 3 Reader layer.
//!
//! \return ui8Status indicates if the state was successfully changed or not.
//
//*****************************************************************************
uint8_t NFC_RW_T3T_sendUpdateCmd(tT3TPacketData sT3TData)
{
	uint8_t ui8Status = STATUS_SUCCESS;

	if(g_eNfcRwT3TCommStatus == NFC_RW_T3T_CONNECTION_IDLE)
	{
		g_eNfcRwT3TCommStatus = NFC_RW_T3T_CONNECTION_UPDATE;

		g_psT3TTxData = sT3TData;
	}
	else
	{
		ui8Status = STATUS_FAIL;
	}

	return ui8Status;
}

//*****************************************************************************
//
//! NFC_RW_T3T_getPacketStatus - Gets the last Type 3 data packet received.
//!
//! \param pui8RXData this is a double pointer to receive the current pointer
//! to the location of the Type 3 data bytes received buffer
//! \param pui8Length this is a pointer to get the length of the received Type
//! 3 data.
//!
//! This function returns the packet length and the location of the packet
//! contents of the latest Type 3 data packet that was received.
//!
//! \return None.
//
//*****************************************************************************
void NFC_RW_T3T_getPacketStatus(uint8_t ** pui8RXData, uint8_t * pui8Length)
{
	*pui8Length = g_ui8T3TBytesReceived;

	*pui8RXData = g_pui8T3TRxData;
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************

