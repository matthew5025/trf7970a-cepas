//*****************************************************************************
//
// nfc_rw_t5t.c - Implementation of T5T Reader/Writer
//
// Copyright (c) 2015 Texas Instruments Incorporated.  All rights reserved.
// TI Information - Selective Disclosure
//
//*****************************************************************************

#include "nfc_rw_t5t.h"

//*****************************************************************************
//
//! \addtogroup nfc_rw_t5t_api NFC Type V Reader API Functions
//! @{
//!
//! A Type V Reader/Writer issues commands based on the ISO15693 specifications.
//! The ISO15693 specification is for Vicinity Integrated Circuit Cards (VICCs)
//! and communication with VICCs that are Type V tags occur at bitrates of
//! 6.62 kbps (low data rate) or 26.48 kbps (high data rate).
//!
//! For more information on Type V Readers and Tags please read the ISO15693
//! Specifications.
//
//*****************************************************************************

//*****************************************************************************
//
// Stores the T5T TX buffer.
//
//*****************************************************************************
static uint8_t g_pui8T5TBuffer[256];

//*****************************************************************************
//
// Stores the error code received from the T5T tags.
//
//*****************************************************************************
static uint8_t g_ui8T5TErrorCode;

//*****************************************************************************
//
// Stores the number of bytes received from the T5T tag.
//
//*****************************************************************************
static uint8_t g_ui8T5TBytesReceived;

//*****************************************************************************
//
// Stores the block number to be read/written from the T5T tag.
//
//*****************************************************************************
static uint16_t g_ui16T5TBlockNumber;

//*****************************************************************************
//
// Stores the number of blocks to be read from the T5T tag.
//
//*****************************************************************************
static uint8_t g_ui8T5TBlockCount;

//*****************************************************************************
//
// Stores the T5T tag UID.
//
//*****************************************************************************
static uint8_t g_ui8T5TUID[8];

//*****************************************************************************
//
// Stores the T5T tag FID.
//
//*****************************************************************************
static uint8_t g_ui8T5TDSFID;

//*****************************************************************************
//
// Stores the T5T tag AFI.
//
//*****************************************************************************
static uint8_t g_ui8T5TAFI;

//*****************************************************************************
//
// Stores the T5T VICC Block Size.
//
//*****************************************************************************
static uint8_t g_ui8T5TVICCBlockSize;

//*****************************************************************************
//
// Stores the T5T VICC Block Size.
//
//*****************************************************************************
static uint16_t g_ui16T5TVICCBlockCount;

//*****************************************************************************
//
// Stores the T5T VICC Memory Size.
//
//*****************************************************************************
static uint8_t g_ui8T5TVICCBlockCountBuffer[2];

//*****************************************************************************
//
// Stores the T5T AFI used in the Inventory Command.
//
//*****************************************************************************
static bool g_bSendInvAFI;

//*****************************************************************************
//
// Stores the T5T AFI used in the Write Command.
//
//*****************************************************************************
static uint8_t g_ui8T5TWriteAFI;

//*****************************************************************************
//
// Stores the T5T TX data pointer.
//
//*****************************************************************************
static uint8_t * g_pui8T5TTxData;

//*****************************************************************************
//
// Stores the T5T RX data pointer.
//
//*****************************************************************************
static uint8_t * g_pui8T5TRxData;

//*****************************************************************************
//
// Stores the length of the TX packet.
//
//*****************************************************************************
static uint8_t g_ui8T5TTxLength;

//*****************************************************************************
//
// Stores the T5T request flags.
//
//*****************************************************************************
static uint8_t g_ui8T5TRequestFlag;

//*****************************************************************************
//
// Stores the T5T command code.
//
//*****************************************************************************
static uint8_t g_ui8T5TCommandCode;

//*****************************************************************************
//
// Stores the T5T option delay flag.
//
//*****************************************************************************
static bool g_bT5TOptionDelay;

//*****************************************************************************
//
// Stores the connection status for the T5T communication.
//
//*****************************************************************************
static tNfcRwT5TConnectionStatus g_eNfcRwT5TCommStatus;


//*****************************************************************************
//
// Stores the T5T time out counter. When it reaches 3 times, an error is
// sent to the Initiatior statemachine.
//
//*****************************************************************************
static uint8_t g_ui8T5TTimeOut;

//*****************************************************************************
//
//! NFC_RW_T5T_init - Initializes the variables for the Type V Reader/Writer.
//!
//! This function must be called prior to any other NFC_RW_T5T function in
//! in order to properly initialize all variables which have been declared.
//! Failure to do so may cause the software to enter unknown states.
//!
//! \return None.
//!
//
//*****************************************************************************
void NFC_RW_T5T_init(void)
{
	g_eNfcRwT5TCommStatus = NFC_RW_T5T_CONNECTION_IDLE;

	g_ui8T5TErrorCode = 0x00;

	g_ui8T5TBytesReceived = 0x00;

	g_ui16T5TBlockNumber = 0x00;
	g_ui8T5TBlockCount = 0x00;

	g_ui8T5TDSFID = 0x00;
	g_ui8T5TAFI = 0x00;

	g_bSendInvAFI = false;

	g_ui8T5TWriteAFI = 0x00;

	g_ui8T5TTxLength = 0x00;

	g_ui8T5TRequestFlag = 0x02;
	g_ui8T5TCommandCode = T5T_GET_SYSTEM_INFO_CMD;

	// Default for TI HF-I Pro/Standard Tags which do not reply to Get System Info
	g_ui8T5TVICCBlockSize = 0x04;
	g_ui16T5TVICCBlockCount = 0x0B;
	g_ui8T5TTimeOut = 0x00;
	g_ui8T5TVICCBlockCountBuffer[0] = 0x00;
	g_ui8T5TVICCBlockCountBuffer[1] = 0x00;

	// Init Type V UID to all zeroes
	g_ui8T5TUID[0] = 0x00;
	g_ui8T5TUID[1] = 0x00;
	g_ui8T5TUID[2] = 0x00;
	g_ui8T5TUID[3] = 0x00;
	g_ui8T5TUID[4] = 0x00;
	g_ui8T5TUID[5] = 0x00;
	g_ui8T5TUID[6] = 0x00;
	g_ui8T5TUID[7] = 0x00;

	g_bT5TOptionDelay = false;
}

//*****************************************************************************
//
//! NFC_RW_T5T_stateMachine - Conditions and transmits the data packets for
//!	Type V Reader commands.
//!
//! This function constructs the data packets for Type V commands based on the
//! current status. The packet is then transmitted at the end of the state
//! machine. Currently the supported Type V commands are: Inventory, Stay Quiet,
//! Read Single Block, Write Single Block, Lock Block, Read Multiple Blocks,
//! Select, Reset to Ready, Write AFI, Lock AFI, and Get System Information.
//! A raw packet write command is also available to handle any commands that
//! are not directly supported by the software.
//!
//! \return None.
//
//*****************************************************************************
void NFC_RW_T5T_stateMachine(void)
{
	uint8_t ui8Offset = 0x00;
	uint8_t ui8TempCount;

	switch(g_eNfcRwT5TCommStatus)
	{
	case NFC_RW_T5T_CONNECTION_IDLE:
		// Presence Check - Addressed Read Single Block of Block 0 with no Option Flag

		// Addressed and High Data Rate
		g_pui8T5TBuffer[ui8Offset++] = T5T_REQ_FLAG_HIGH_DATA | T5T_REQ_FLAG_ADDRESSED;
		// Read Single Command
		g_pui8T5TBuffer[ui8Offset++] = T5T_READ_SINGLE_BLOCK_CMD;

		// Copy UID into Buffer
		for (ui8TempCount = 0; ui8TempCount < 8; ui8TempCount++)
		{
			g_pui8T5TBuffer[ui8Offset++] = g_ui8T5TUID[ui8TempCount];
		}

		// Read Block 0
		g_pui8T5TBuffer[ui8Offset++] = 0x00;

		// Set Option Flag Delay bool for TRF driver
		g_bT5TOptionDelay = false;
		break;
	case NFC_RW_T5T_CONNECTION_INVENTORY:
		// Single Slot, High Data Rate
		g_pui8T5TBuffer[ui8Offset++] = g_ui8T5TRequestFlag;
		g_pui8T5TBuffer[ui8Offset++] = T5T_INVENTORY_CMD;

		if (g_bSendInvAFI)
		{
			g_pui8T5TBuffer[ui8Offset++] = g_ui8T5TAFI;
		}

		// Mask Length = 0
		g_pui8T5TBuffer[ui8Offset++] = 0x00;

		// Set Option Flag Delay bool for TRF driver
		g_bT5TOptionDelay = false;
		g_eNfcRwT5TCommStatus = NFC_RW_T5T_CONNECTION_INVENTORY_RSP;
		break;
	case NFC_RW_T5T_CONNECTION_INVENTORY_RSP:
		break;
	case NFC_RW_T5T_CONNECTION_INVENTORY_SUCCESS:
		break;
	case NFC_RW_T5T_CONNECTION_INVENTORY_FAIL:
		break;
	case NFC_RW_T5T_CONNECTION_STAY_QUIET:
		// Stay Quiet must be addressed
		g_pui8T5TBuffer[ui8Offset++] = g_ui8T5TRequestFlag | T5T_REQ_FLAG_ADDRESSED;
		// Stay Quiet Command
		g_pui8T5TBuffer[ui8Offset++] = T5T_STAY_QUIET_CMD;

		for (ui8TempCount = 0; ui8TempCount < 8; ui8TempCount++)
		{
			g_pui8T5TBuffer[ui8Offset++] = g_ui8T5TUID[ui8TempCount];
		}

		// Set Option Flag Delay bool for TRF driver
		g_bT5TOptionDelay = false;
		g_eNfcRwT5TCommStatus = NFC_RW_T5T_CONNECTION_IDLE;
		break;
	case NFC_RW_T5T_CONNECTION_READ_SINGLE_BLOCK:
		g_pui8T5TBuffer[ui8Offset++] = g_ui8T5TRequestFlag;
		g_pui8T5TBuffer[ui8Offset++] = T5T_READ_SINGLE_BLOCK_CMD;

		if (g_ui8T5TRequestFlag & T5T_REQ_FLAG_ADDRESSED)
		{
			// Copy UID into Buffer
			for (ui8TempCount = 0; ui8TempCount < 8; ui8TempCount++)
			{
				g_pui8T5TBuffer[ui8Offset++] = g_ui8T5TUID[ui8TempCount];
			}
		}

		// Block # to be read
		g_pui8T5TBuffer[ui8Offset++] = (uint8_t)(g_ui16T5TBlockNumber & 0xFF);
		if (g_ui8T5TRequestFlag & T5T_REQ_FLAG_EXTENDED)
		{
			g_pui8T5TBuffer[ui8Offset++] = (uint8_t)(g_ui16T5TBlockNumber >> 8);
		}

		// Set Option Flag Delay bool for TRF driver
		g_bT5TOptionDelay = false;
		g_eNfcRwT5TCommStatus = NFC_RW_T5T_CONNECTION_WAIT_READ_SINGLE_RSP;
		break;
	case NFC_RW_T5T_CONNECTION_WAIT_READ_SINGLE_RSP:
		break;
	case NFC_RW_T5T_CONNECTION_READ_SINGLE_SUCCESS:
		break;
	case NFC_RW_T5T_CONNECTION_READ_SINGLE_FAIL:
		break;
	case NFC_RW_T5T_CONNECTION_WRITE_SINGLE_BLOCK:
		g_pui8T5TBuffer[ui8Offset++] = g_ui8T5TRequestFlag;
		g_pui8T5TBuffer[ui8Offset++] = T5T_WRITE_SINGLE_BLOCK_CMD;

		if (g_ui8T5TRequestFlag & T5T_REQ_FLAG_ADDRESSED)
		{
			// Copy UID into Buffer
			for (ui8TempCount = 0; ui8TempCount < 8; ui8TempCount++)
			{
				g_pui8T5TBuffer[ui8Offset++] = g_ui8T5TUID[ui8TempCount];
			}
		}

		// Block # to be written
		g_pui8T5TBuffer[ui8Offset++] = (uint8_t)(g_ui16T5TBlockNumber & 0xFF);
		if (g_ui8T5TRequestFlag & T5T_REQ_FLAG_EXTENDED)
		{
			g_pui8T5TBuffer[ui8Offset++] = (uint8_t)(g_ui16T5TBlockNumber >> 8);
		}

		// Data to be written to the block
		for (ui8TempCount = 0; ui8TempCount < g_ui8T5TTxLength; ui8TempCount++)
		{
			g_pui8T5TBuffer[ui8Offset++] = g_pui8T5TTxData[ui8TempCount];
		}

		// Set Option Flag Delay bool for TRF driver
		if (g_ui8T5TRequestFlag & T5T_REQ_FLAG_OPTION)
		{
			g_bT5TOptionDelay = true;
		}
		else
		{
			g_bT5TOptionDelay = false;
		}
		g_eNfcRwT5TCommStatus = NFC_RW_T5T_CONNECTION_WAIT_WRITE_SINGLE_RSP;
		break;
	case NFC_RW_T5T_CONNECTION_WAIT_WRITE_SINGLE_RSP:
		break;
	case NFC_RW_T5T_CONNECTION_WRITE_SINGLE_SUCCESS:
		break;
	case NFC_RW_T5T_CONNECTION_WRITE_SINGLE_FAIL:
		break;
	case NFC_RW_T5T_CONNECTION_READ_MULT_BLOCK:
		// High Data Rate
		g_pui8T5TBuffer[ui8Offset++] = g_ui8T5TRequestFlag;
		g_pui8T5TBuffer[ui8Offset++] = T5T_READ_MULTIPLE_BLOCK_CMD;

		if (g_ui8T5TRequestFlag & T5T_REQ_FLAG_ADDRESSED)
		{
			// Copy UID into Buffer
			for (ui8TempCount = 0; ui8TempCount < 8; ui8TempCount++)
			{
				g_pui8T5TBuffer[ui8Offset++] = g_ui8T5TUID[ui8TempCount];
			}
		}

		// First Block # to be read
		g_pui8T5TBuffer[ui8Offset++] = (uint8_t)(g_ui16T5TBlockNumber & 0xFF);
		if (g_ui8T5TRequestFlag & T5T_REQ_FLAG_EXTENDED)
		{
			g_pui8T5TBuffer[ui8Offset++] = (uint8_t)(g_ui16T5TBlockNumber >> 8);
		}
		// Number of Blocks to be read
		g_pui8T5TBuffer[ui8Offset++] = g_ui8T5TBlockCount;

		// Set Option Flag Delay bool for TRF driver
		g_bT5TOptionDelay = false;
		g_eNfcRwT5TCommStatus = NFC_RW_T5T_CONNECTION_WAIT_READ_MULT_RSP;
		break;
	case NFC_RW_T5T_CONNECTION_WAIT_READ_MULT_RSP:
		break;
	case NFC_RW_T5T_CONNECTION_READ_MULT_SUCCESS:
		break;
	case NFC_RW_T5T_CONNECTION_READ_MULT_FAIL:
		break;
	case NFC_RW_T5T_CONNECTION_LOCK_BLOCK:
		// Addressed Lock Blocks only
		g_pui8T5TBuffer[ui8Offset++] = g_ui8T5TRequestFlag | T5T_REQ_FLAG_ADDRESSED;
		g_pui8T5TBuffer[ui8Offset++] = T5T_LOCK_BLOCK_CMD;

		for (ui8TempCount = 0; ui8TempCount < 8; ui8TempCount++)
		{
			g_pui8T5TBuffer[ui8Offset++] = g_ui8T5TUID[ui8TempCount];
		}

		// Block # to be locked
		g_pui8T5TBuffer[ui8Offset++] = (uint8_t)(g_ui16T5TBlockNumber & 0xFF);
		if (g_ui8T5TRequestFlag & T5T_REQ_FLAG_EXTENDED)
		{
			g_pui8T5TBuffer[ui8Offset++] = (uint8_t)(g_ui16T5TBlockNumber >> 8);
		}

		// Set Option Flag Delay bool for TRF driver
		if (g_ui8T5TRequestFlag & T5T_REQ_FLAG_OPTION)
		{
			g_bT5TOptionDelay = true;
		}
		else
		{
			g_bT5TOptionDelay = false;
		}
		g_eNfcRwT5TCommStatus = NFC_RW_T5T_CONNECTION_WAIT_LOCK_BLOCK_RSP;
		break;
	case NFC_RW_T5T_CONNECTION_WAIT_LOCK_BLOCK_RSP:
		break;
	case NFC_RW_T5T_CONNECTION_LOCK_BLOCK_SUCCESS:
		break;
	case NFC_RW_T5T_CONNECTION_LOCK_BLOCK_FAIL:
		break;
	case NFC_RW_T5T_CONNECTION_SELECT:
		// Select Command MUST be Addressed
		g_pui8T5TBuffer[ui8Offset++] = g_ui8T5TRequestFlag | T5T_REQ_FLAG_ADDRESSED;
		g_pui8T5TBuffer[ui8Offset++] = T5T_SELECT_CMD;

		for (ui8TempCount = 0; ui8TempCount < 8; ui8TempCount++)
		{
			g_pui8T5TBuffer[ui8Offset++] = g_ui8T5TUID[ui8TempCount];
		}

		// Set Option Flag Delay bool for TRF driver
		g_bT5TOptionDelay = false;
		g_eNfcRwT5TCommStatus = NFC_RW_T5T_CONNECTION_WAIT_SELECT_RSP;
		break;
	case NFC_RW_T5T_CONNECTION_WAIT_SELECT_RSP:
		break;
	case NFC_RW_T5T_CONNECTION_SELECT_SUCCESS:
		break;
	case NFC_RW_T5T_CONNECTION_SELECT_FAIL:
		break;
	case NFC_RW_T5T_CONNECTION_RESET_TO_READY:
		g_pui8T5TBuffer[ui8Offset++] = g_ui8T5TRequestFlag;
		g_pui8T5TBuffer[ui8Offset++] = T5T_RESET_TO_READY_CMD;

		if (g_ui8T5TRequestFlag & T5T_REQ_FLAG_ADDRESSED)
		{
			// Copy UID into Buffer
			for (ui8TempCount = 0; ui8TempCount < 8; ui8TempCount++)
			{
				g_pui8T5TBuffer[ui8Offset++] = g_ui8T5TUID[ui8TempCount];
			}
		}

		// Set Option Flag Delay bool for TRF driver
		g_bT5TOptionDelay = false;
		g_eNfcRwT5TCommStatus = NFC_RW_T5T_CONNECTION_WAIT_RESET_TO_READY_RSP;
		break;
	case NFC_RW_T5T_CONNECTION_WAIT_RESET_TO_READY_RSP:
		break;
	case NFC_RW_T5T_CONNECTION_RESET_TO_READY_SUCCESS:
		break;
	case NFC_RW_T5T_CONNECTION_RESET_TO_READY_FAIL:
		break;
	case NFC_RW_T5T_CONNECTION_WRITE_AFI_BLOCK:
		g_pui8T5TBuffer[ui8Offset++] = g_ui8T5TRequestFlag;
		g_pui8T5TBuffer[ui8Offset++] = T5T_WRITE_AFI_CMD;

		if (g_ui8T5TRequestFlag & T5T_REQ_FLAG_ADDRESSED)
		{
			for (ui8TempCount = 0; ui8TempCount < 8; ui8TempCount++)
			{
				g_pui8T5TBuffer[ui8Offset++] = g_ui8T5TUID[ui8TempCount];
			}
		}

		g_pui8T5TBuffer[ui8Offset++] = g_ui8T5TWriteAFI;

		// Set Option Flag Delay bool for TRF driver
		if (g_ui8T5TRequestFlag & T5T_REQ_FLAG_OPTION)
		{
			g_bT5TOptionDelay = true;
		}
		else
		{
			g_bT5TOptionDelay = false;
		}
		g_eNfcRwT5TCommStatus = NFC_RW_T5T_CONNECTION_WAIT_WRITE_AFI_RSP;
		break;
	case NFC_RW_T5T_CONNECTION_WAIT_WRITE_AFI_RSP:
		break;
	case NFC_RW_T5T_CONNECTION_WRITE_AFI_SUCCESS:
		break;
	case NFC_RW_T5T_CONNECTION_WRITE_AFI_FAIL:
		break;
	case NFC_RW_T5T_CONNECTION_LOCK_AFI_BLOCK:
		// Addressed Lock AFI Block only
		g_pui8T5TBuffer[ui8Offset++] = g_ui8T5TRequestFlag | T5T_REQ_FLAG_ADDRESSED;
		g_pui8T5TBuffer[ui8Offset++] = T5T_LOCK_AFI_CMD;

		for (ui8TempCount = 0; ui8TempCount < 8; ui8TempCount++)
		{
			g_pui8T5TBuffer[ui8Offset++] = g_ui8T5TUID[ui8TempCount];
		}

		// Set Option Flag Delay bool for TRF driver
		if (g_ui8T5TRequestFlag & T5T_REQ_FLAG_OPTION)
		{
			g_bT5TOptionDelay = true;
		}
		else
		{
			g_bT5TOptionDelay = false;
		}
		g_eNfcRwT5TCommStatus = NFC_RW_T5T_CONNECTION_WAIT_LOCK_AFI_RSP;
		break;
	case NFC_RW_T5T_CONNECTION_WAIT_LOCK_AFI_RSP:
		break;
	case NFC_RW_T5T_CONNECTION_LOCK_AFI_SUCCESS:
		break;
	case NFC_RW_T5T_CONNECTION_LOCK_AFI_FAIL:
		break;
	case NFC_RW_T5T_CONNECTION_GET_SYS_INFO:
		g_pui8T5TBuffer[ui8Offset++] = g_ui8T5TRequestFlag;
		g_pui8T5TBuffer[ui8Offset++] = g_ui8T5TCommandCode;

		if (g_ui8T5TRequestFlag & T5T_REQ_FLAG_ADDRESSED)
		{
			// Copy UID into Buffer
			for (ui8TempCount = 0; ui8TempCount < 8; ui8TempCount++)
			{
				g_pui8T5TBuffer[ui8Offset++] = g_ui8T5TUID[ui8TempCount];
			}
		}

		// Set Option Flag Delay bool for TRF driver
		g_bT5TOptionDelay = false;
		g_eNfcRwT5TCommStatus = NFC_RW_T5T_CONNECTION_GET_SYS_INFO_RSP;
		break;
	case NFC_RW_T5T_CONNECTION_GET_SYS_INFO_RSP:
		break;
	case NFC_RW_T5T_CONNECTION_GET_SYS_INFO_SUCCESS:
		break;
	case NFC_RW_T5T_CONNECTION_GET_SYS_INFO_FAIL:
		break;
	case NFC_RW_T5T_CONNECTION_RAW_WRITE:
		g_pui8T5TBuffer[ui8Offset++] = g_ui8T5TRequestFlag;
		g_pui8T5TBuffer[ui8Offset++] = g_ui8T5TCommandCode;

		// Data to be written to the block
		for (ui8TempCount = 0; ui8TempCount < g_ui8T5TTxLength; ui8TempCount++)
		{
			g_pui8T5TBuffer[ui8Offset++] = g_pui8T5TTxData[ui8TempCount];
		}

		// Setting of the Option Flag Delay bool is handled in the NFC_RW_T5T_sendRawWriteCmd function
		g_eNfcRwT5TCommStatus = NFC_RW_T5T_CONNECTION_RAW_WRITE_RSP;
		break;
	case NFC_RW_T5T_CONNECTION_RAW_WRITE_RSP:
		break;
	case NFC_RW_T5T_CONNECTION_RAW_WRITE_SUCCESS:
		break;
	case NFC_RW_T5T_CONNECTION_RAW_WRITE_FAIL:
		break;
	default:
		break;
	}

	if(ui8Offset > 0)
	{
		TRF79x0_setType5OptionDelay(g_bT5TOptionDelay);

		TRF79x0_writeFIFO(g_pui8T5TBuffer,CRC_BIT_ENABLE,ui8Offset,0);
	}

}

//*****************************************************************************
//
//! NFC_RW_T5T_processReceivedData - Processes the Type V Data Received.
//!
//! \param pui8RxBuffer is the pointer to where T5T data can be received.
//! \param ui8Length is the length of the T5T data packet received.
//!
//! This function is used to handle the processing of all Type V data packets
//! that have been received from Type V tags. This function includes a timeout
//! monitor to ensure the software will not be locked up if the tag is
//! unexpectedly removed from the RF field.
//!
//! This function will wait for replies specific to the command that was issued.
//! If it receives an expected reply then it will indicated a successful response
//! and the application layer would then have to determine what command to issue
//! next. This gives developers full control over the sequencing of Type V
//! Reader commands. If an unexpected reply is received, it will be treated as
//! a protocol error and the Initiator state machine will reset.
//!
//! \return g_eNfcRwT5TCommStatus returns the current status of the Type V
//! Reader based on what response was received.
//
//*****************************************************************************
tNfcRwT5TConnectionStatus NFC_RW_T5T_processReceivedData(uint8_t * pui8RxBuffer, uint8_t ui8Length)
{
	uint8_t ui8TempCount;

	if(ui8Length == 0x00)
	{
		g_ui8T5TTimeOut++;
	}
	else
	{
		g_ui8T5TTimeOut = 0x00;
	}

	if(g_ui8T5TTimeOut == 5)
	{
		g_eNfcRwT5TCommStatus = NFC_RW_T5T_PROTOCOL_ERROR;
	}

	// Delay to meet specifications on how long between received reply and subsequent transmission
	MCU_delayMicrosecond(300);

	switch(g_eNfcRwT5TCommStatus)
	{
	case NFC_RW_T5T_CONNECTION_IDLE:
		g_ui8T5TBytesReceived = ui8Length;
		g_pui8T5TRxData = pui8RxBuffer;
		break;
	case NFC_RW_T5T_CONNECTION_INVENTORY:
		break;
	case NFC_RW_T5T_CONNECTION_INVENTORY_RSP:
		if (ui8Length == 0x0A)
		{
			if((pui8RxBuffer[0] == T5T_RESP_FLAG_NO_ERROR) || (pui8RxBuffer[0] == T5T_REQ_FLAG_EXTENDED))
			{
				// Fetch DSFID
				g_ui8T5TDSFID = pui8RxBuffer[1];
				// Fetch UID
				for (ui8TempCount = 0; ui8TempCount < 8; ui8TempCount++)
				{
					g_ui8T5TUID[ui8TempCount] = pui8RxBuffer[ui8TempCount+2];
				}

				g_eNfcRwT5TCommStatus = NFC_RW_T5T_CONNECTION_INVENTORY_SUCCESS;
			}
			else
			{
				g_eNfcRwT5TCommStatus = NFC_RW_T5T_CONNECTION_INVENTORY_FAIL;
			}
		}
		else
		{
			g_eNfcRwT5TCommStatus = NFC_RW_T5T_CONNECTION_INVENTORY_FAIL;
		}
		break;
	case NFC_RW_T5T_CONNECTION_INVENTORY_SUCCESS:
		break;
	case NFC_RW_T5T_CONNECTION_INVENTORY_FAIL:
		break;
	case NFC_RW_T5T_CONNECTION_STAY_QUIET:
		break;
	case NFC_RW_T5T_CONNECTION_READ_SINGLE_BLOCK:
		break;
	case NFC_RW_T5T_CONNECTION_WAIT_READ_SINGLE_RSP:
		g_ui8T5TBytesReceived = ui8Length;

		g_pui8T5TRxData = pui8RxBuffer;

		if(ui8Length == 0x02)
		{
			if ((pui8RxBuffer[0] == T5T_RESP_FLAG_NO_ERROR) || (pui8RxBuffer[0] == T5T_REQ_FLAG_EXTENDED))
			{
				g_eNfcRwT5TCommStatus = NFC_RW_T5T_CONNECTION_READ_SINGLE_SUCCESS;
			}
			else
			{
				g_ui8T5TErrorCode = pui8RxBuffer[1];
				g_eNfcRwT5TCommStatus = NFC_RW_T5T_CONNECTION_READ_SINGLE_FAIL;
			}
		}
		else if(ui8Length > 0x02)
		{
			if((pui8RxBuffer[0] == T5T_RESP_FLAG_NO_ERROR) || (pui8RxBuffer[0] == T5T_REQ_FLAG_EXTENDED))
			{
				g_eNfcRwT5TCommStatus = NFC_RW_T5T_CONNECTION_READ_SINGLE_SUCCESS;
			}
			else
			{
				g_eNfcRwT5TCommStatus = NFC_RW_T5T_CONNECTION_READ_SINGLE_FAIL;
			}
		}
		else
		{
			g_eNfcRwT5TCommStatus = NFC_RW_T5T_CONNECTION_READ_SINGLE_FAIL;
		}
		break;
	case NFC_RW_T5T_CONNECTION_READ_SINGLE_SUCCESS:
		break;
	case NFC_RW_T5T_CONNECTION_READ_SINGLE_FAIL:
		break;
	case NFC_RW_T5T_CONNECTION_WRITE_SINGLE_BLOCK:
		break;
	case NFC_RW_T5T_CONNECTION_WAIT_WRITE_SINGLE_RSP:
		if(ui8Length == 0x01)
		{
			if((pui8RxBuffer[0] == T5T_RESP_FLAG_NO_ERROR) || (pui8RxBuffer[0] == T5T_REQ_FLAG_EXTENDED))
			{
				g_eNfcRwT5TCommStatus = NFC_RW_T5T_CONNECTION_WRITE_SINGLE_SUCCESS;
			}
			else
			{
				g_eNfcRwT5TCommStatus = NFC_RW_T5T_CONNECTION_WRITE_SINGLE_FAIL;
			}
		}
		else if(ui8Length == 0x02)
		{
			g_ui8T5TErrorCode = pui8RxBuffer[1];
			g_eNfcRwT5TCommStatus = NFC_RW_T5T_CONNECTION_WRITE_SINGLE_FAIL;
		}
		else
		{
			g_eNfcRwT5TCommStatus = NFC_RW_T5T_CONNECTION_WRITE_SINGLE_FAIL;
		}
		break;
	case NFC_RW_T5T_CONNECTION_WRITE_SINGLE_SUCCESS:
		break;
	case NFC_RW_T5T_CONNECTION_WRITE_SINGLE_FAIL:
		break;
	case NFC_RW_T5T_CONNECTION_LOCK_BLOCK:
		break;
	case NFC_RW_T5T_CONNECTION_WAIT_LOCK_BLOCK_RSP:
		if(ui8Length == 0x01)
		{
			if((pui8RxBuffer[0] == T5T_RESP_FLAG_NO_ERROR) || (pui8RxBuffer[0] == T5T_REQ_FLAG_EXTENDED))
			{
				g_eNfcRwT5TCommStatus = NFC_RW_T5T_CONNECTION_LOCK_BLOCK_SUCCESS;
			}
			else
			{
				g_eNfcRwT5TCommStatus = NFC_RW_T5T_CONNECTION_LOCK_BLOCK_FAIL;
			}
		}
		else if(ui8Length == 0x02)
		{
			g_ui8T5TErrorCode = pui8RxBuffer[1];
			g_eNfcRwT5TCommStatus = NFC_RW_T5T_CONNECTION_LOCK_BLOCK_FAIL;
		}
		else
		{
			g_eNfcRwT5TCommStatus = NFC_RW_T5T_CONNECTION_LOCK_BLOCK_FAIL;
		}
		break;
	case NFC_RW_T5T_CONNECTION_LOCK_BLOCK_SUCCESS:
		break;
	case NFC_RW_T5T_CONNECTION_LOCK_BLOCK_FAIL:
		break;
	case NFC_RW_T5T_CONNECTION_SELECT:
		break;
	case NFC_RW_T5T_CONNECTION_WAIT_SELECT_RSP:
		if(ui8Length == 0x01)
		{
			if((pui8RxBuffer[0] == T5T_RESP_FLAG_NO_ERROR) || (pui8RxBuffer[0] == T5T_REQ_FLAG_EXTENDED))
			{
				g_eNfcRwT5TCommStatus = NFC_RW_T5T_CONNECTION_SELECT_SUCCESS;
			}
			else
			{
				g_eNfcRwT5TCommStatus = NFC_RW_T5T_CONNECTION_SELECT_FAIL;
			}
		}
		else if(ui8Length == 0x02)
		{
			g_ui8T5TErrorCode = pui8RxBuffer[1];
			g_eNfcRwT5TCommStatus = NFC_RW_T5T_CONNECTION_SELECT_FAIL;
		}
		else
		{
			g_eNfcRwT5TCommStatus = NFC_RW_T5T_CONNECTION_SELECT_FAIL;
		}
		break;
	case NFC_RW_T5T_CONNECTION_SELECT_SUCCESS:
		break;
	case NFC_RW_T5T_CONNECTION_SELECT_FAIL:
		break;
	case NFC_RW_T5T_CONNECTION_RESET_TO_READY:
		break;
	case NFC_RW_T5T_CONNECTION_WAIT_RESET_TO_READY_RSP:
		if(ui8Length == 0x01)
		{
			if((pui8RxBuffer[0] == T5T_RESP_FLAG_NO_ERROR) || (pui8RxBuffer[0] == T5T_REQ_FLAG_EXTENDED))
			{
				g_eNfcRwT5TCommStatus = NFC_RW_T5T_CONNECTION_RESET_TO_READY_SUCCESS;
			}
			else
			{
				g_eNfcRwT5TCommStatus = NFC_RW_T5T_CONNECTION_RESET_TO_READY_FAIL;
			}
		}
		else if(ui8Length == 0x02)
		{
			g_ui8T5TErrorCode = pui8RxBuffer[1];
			g_eNfcRwT5TCommStatus = NFC_RW_T5T_CONNECTION_RESET_TO_READY_FAIL;
		}
		else
		{
			g_eNfcRwT5TCommStatus = NFC_RW_T5T_CONNECTION_RESET_TO_READY_FAIL;
		}
		break;
	case NFC_RW_T5T_CONNECTION_RESET_TO_READY_SUCCESS:
		break;
	case NFC_RW_T5T_CONNECTION_RESET_TO_READY_FAIL:
		break;
	case NFC_RW_T5T_CONNECTION_READ_MULT_BLOCK:
		break;
	case NFC_RW_T5T_CONNECTION_WAIT_READ_MULT_RSP:
		g_ui8T5TBytesReceived = ui8Length;

		g_pui8T5TRxData = pui8RxBuffer;

		if(ui8Length == 0x02)
		{
			g_ui8T5TErrorCode = pui8RxBuffer[1];
			g_eNfcRwT5TCommStatus = NFC_RW_T5T_CONNECTION_READ_MULT_FAIL;
		}
		else if(ui8Length > 0x02)
		{
			if((pui8RxBuffer[0] == T5T_RESP_FLAG_NO_ERROR) || (pui8RxBuffer[0] == T5T_REQ_FLAG_EXTENDED))
			{
				g_eNfcRwT5TCommStatus = NFC_RW_T5T_CONNECTION_READ_MULT_SUCCESS;
			}
			else
			{
				g_eNfcRwT5TCommStatus = NFC_RW_T5T_CONNECTION_READ_MULT_FAIL;
			}
		}
		else
		{
			g_eNfcRwT5TCommStatus = NFC_RW_T5T_CONNECTION_READ_MULT_FAIL;
		}
		break;
	case NFC_RW_T5T_CONNECTION_READ_MULT_SUCCESS:
		break;
	case NFC_RW_T5T_CONNECTION_READ_MULT_FAIL:
		break;
	case NFC_RW_T5T_CONNECTION_WRITE_AFI_BLOCK:
		break;
	case NFC_RW_T5T_CONNECTION_WAIT_WRITE_AFI_RSP:
		if(ui8Length == 0x01)
		{
			if((pui8RxBuffer[0] == T5T_RESP_FLAG_NO_ERROR) || (pui8RxBuffer[0] == T5T_REQ_FLAG_EXTENDED))
			{
				g_eNfcRwT5TCommStatus = NFC_RW_T5T_CONNECTION_WRITE_AFI_SUCCESS;
			}
			else
			{
				g_eNfcRwT5TCommStatus = NFC_RW_T5T_CONNECTION_WRITE_AFI_FAIL;
			}
		}
		else if(ui8Length == 0x02)
		{
			g_ui8T5TErrorCode = pui8RxBuffer[1];
			g_eNfcRwT5TCommStatus = NFC_RW_T5T_CONNECTION_WRITE_AFI_FAIL;
		}
		else
		{
			g_eNfcRwT5TCommStatus = NFC_RW_T5T_CONNECTION_WRITE_AFI_FAIL;
		}
		break;
	case NFC_RW_T5T_CONNECTION_WRITE_AFI_SUCCESS:
		break;
	case NFC_RW_T5T_CONNECTION_WRITE_AFI_FAIL:
		break;
	case NFC_RW_T5T_CONNECTION_LOCK_AFI_BLOCK:
		break;
	case NFC_RW_T5T_CONNECTION_WAIT_LOCK_AFI_RSP:
		if(ui8Length == 0x01)
		{
			if((pui8RxBuffer[0] == T5T_RESP_FLAG_NO_ERROR) || (pui8RxBuffer[0] == T5T_REQ_FLAG_EXTENDED))
			{
				g_eNfcRwT5TCommStatus = NFC_RW_T5T_CONNECTION_LOCK_AFI_SUCCESS;
			}
			else
			{
				g_eNfcRwT5TCommStatus = NFC_RW_T5T_CONNECTION_LOCK_AFI_FAIL;
			}
		}
		else if(ui8Length == 0x02)
		{
			g_ui8T5TErrorCode = pui8RxBuffer[1];
			g_eNfcRwT5TCommStatus = NFC_RW_T5T_CONNECTION_LOCK_AFI_FAIL;
		}
		else
		{
			g_eNfcRwT5TCommStatus = NFC_RW_T5T_CONNECTION_LOCK_AFI_FAIL;
		}
		break;
	case NFC_RW_T5T_CONNECTION_LOCK_AFI_SUCCESS:
		break;
	case NFC_RW_T5T_CONNECTION_LOCK_AFI_FAIL:
		break;
	case NFC_RW_T5T_CONNECTION_GET_SYS_INFO:
		break;
	case NFC_RW_T5T_CONNECTION_GET_SYS_INFO_RSP:
		g_ui8T5TBytesReceived = ui8Length;

		g_pui8T5TRxData = pui8RxBuffer;

		if (ui8Length >= 0x0A)
		{
			if((pui8RxBuffer[0] == T5T_RESP_FLAG_NO_ERROR) || (pui8RxBuffer[0] == T5T_REQ_FLAG_EXTENDED))
			{
				// Fetch UID
				for (ui8TempCount = 0; ui8TempCount < 8; ui8TempCount++)
				{
					g_ui8T5TUID[ui8TempCount] = pui8RxBuffer[ui8TempCount+2];
				}
				// DSFID Supported
				if (pui8RxBuffer[1] & T5T_GET_SYS_RESP_DSFID)
				{
					// Fetch DSFID
					g_ui8T5TDSFID = pui8RxBuffer[0x0A];
					// AFI Supported
					if (pui8RxBuffer[1] & T5T_GET_SYS_RESP_AFI)
					{
						// Fetch AFI
						g_ui8T5TAFI = pui8RxBuffer[0x0B];

						if (pui8RxBuffer[1] & T5T_GET_SYS_RESP_MEM_SIZE)
						{
							// Fetch VICC Memory Size
							if (g_ui8T5TRequestFlag & T5T_REQ_FLAG_EXTENDED)
							{
								g_ui8T5TVICCBlockCountBuffer[0] = pui8RxBuffer[0x0C];
								g_ui8T5TVICCBlockCountBuffer[1] = pui8RxBuffer[0x0D];
								g_ui8T5TVICCBlockSize = (pui8RxBuffer[0x0E] & 0x1F);
							}
							else
							{
								g_ui8T5TVICCBlockCountBuffer[0] = pui8RxBuffer[0x0C];
								g_ui8T5TVICCBlockCountBuffer[1] = 0x00;
								g_ui8T5TVICCBlockSize = (pui8RxBuffer[0x0D] & 0x1F);
							}
						}

					}
					else if (pui8RxBuffer[1] & T5T_GET_SYS_RESP_MEM_SIZE)
					{
						// Fetch VICC Memory Size
						if (g_ui8T5TRequestFlag & T5T_REQ_FLAG_EXTENDED)
						{
							g_ui8T5TVICCBlockCountBuffer[0] = pui8RxBuffer[0x0B];
							g_ui8T5TVICCBlockCountBuffer[1] = pui8RxBuffer[0x0C];
							g_ui8T5TVICCBlockSize = (pui8RxBuffer[0x0D] & 0x1F);
						}
						else
						{
							g_ui8T5TVICCBlockCountBuffer[0] = pui8RxBuffer[0x0B];
							g_ui8T5TVICCBlockCountBuffer[1] = 0x00;
							g_ui8T5TVICCBlockSize = (pui8RxBuffer[0x0C] & 0x1F);
						}
					}
				}
				else if (pui8RxBuffer[1] & T5T_GET_SYS_RESP_AFI)
				{
					// Fetch AFI
					g_ui8T5TAFI = pui8RxBuffer[0x0A];
					if (pui8RxBuffer[1] & T5T_GET_SYS_RESP_MEM_SIZE)
					{
						// Fetch VICC Memory Size
						if (g_ui8T5TRequestFlag & T5T_REQ_FLAG_EXTENDED)
						{
							g_ui8T5TVICCBlockCountBuffer[0] = pui8RxBuffer[0x0B];
							g_ui8T5TVICCBlockCountBuffer[1] = pui8RxBuffer[0x0C];
							g_ui8T5TVICCBlockSize = (pui8RxBuffer[0x0D] & 0x1F);
						}
						else
						{
							g_ui8T5TVICCBlockCountBuffer[0] = pui8RxBuffer[0x0B];
							g_ui8T5TVICCBlockCountBuffer[1] = 0x00;
							g_ui8T5TVICCBlockSize = (pui8RxBuffer[0x0C] & 0x1F);
						}
					}
				}
				else if (pui8RxBuffer[1] & T5T_GET_SYS_RESP_MEM_SIZE)
				{
					// Fetch VICC Memory Size
					if (g_ui8T5TRequestFlag & T5T_REQ_FLAG_EXTENDED)
					{
						g_ui8T5TVICCBlockCountBuffer[0] = pui8RxBuffer[0x0A];
						g_ui8T5TVICCBlockCountBuffer[1] = pui8RxBuffer[0x0B];
						g_ui8T5TVICCBlockSize = (pui8RxBuffer[0x0C] & 0x1F);
					}
					else
					{
						g_ui8T5TVICCBlockCountBuffer[0] = pui8RxBuffer[0x0A];
						g_ui8T5TVICCBlockCountBuffer[1] = 0x00;
						g_ui8T5TVICCBlockSize = (pui8RxBuffer[0x0B] & 0x1F);
					}
				}
				else
				{
					// Default for TI HF-I Pro/Standard Tags which do not reply to Get System Info
					g_ui8T5TVICCBlockSize = 0x04;
					g_ui8T5TVICCBlockCountBuffer[0] = 0x0B;
					g_ui8T5TVICCBlockCountBuffer[1] = 0x00;
				}

				g_ui16T5TVICCBlockCount |= (g_ui8T5TVICCBlockCountBuffer[1] << 8) + (g_ui8T5TVICCBlockCountBuffer[0] & 0xFF);

				g_eNfcRwT5TCommStatus = NFC_RW_T5T_CONNECTION_GET_SYS_INFO_SUCCESS;
			}
			else
			{
				g_eNfcRwT5TCommStatus = NFC_RW_T5T_CONNECTION_GET_SYS_INFO_FAIL;
				g_ui8T5TTimeOut = 0x00;
			}
		}
		else if (ui8Length == 0x02)
		{
			g_ui8T5TErrorCode = pui8RxBuffer[1];
			g_eNfcRwT5TCommStatus = NFC_RW_T5T_CONNECTION_GET_SYS_INFO_FAIL;
			g_ui8T5TTimeOut = 0x00;
		}
		else
		{
			g_eNfcRwT5TCommStatus = NFC_RW_T5T_CONNECTION_GET_SYS_INFO_FAIL;
			g_ui8T5TTimeOut = 0x00;
		}
		break;
	case NFC_RW_T5T_CONNECTION_GET_SYS_INFO_SUCCESS:
		break;
	case NFC_RW_T5T_CONNECTION_GET_SYS_INFO_FAIL:
		break;
	case NFC_RW_T5T_CONNECTION_RAW_WRITE:
		break;
	case NFC_RW_T5T_CONNECTION_RAW_WRITE_RSP:
		g_ui8T5TBytesReceived = ui8Length;

		g_pui8T5TRxData = pui8RxBuffer;

		if (ui8Length > 0)
		{
			g_eNfcRwT5TCommStatus = NFC_RW_T5T_CONNECTION_RAW_WRITE_SUCCESS;
		}
		else
		{
			g_eNfcRwT5TCommStatus = NFC_RW_T5T_CONNECTION_RAW_WRITE_FAIL;
			g_ui8T5TTimeOut = 0x00;
		}
		break;
	case NFC_RW_T5T_CONNECTION_RAW_WRITE_SUCCESS:
		break;
	case NFC_RW_T5T_CONNECTION_RAW_WRITE_FAIL:
		break;
	default:
		break;
	}
	return g_eNfcRwT5TCommStatus;
}

//*****************************************************************************
//
//! NFC_RW_T5T_getInventoryStatus - Checks for an Inventory response
//!
//! This function checks if an Inventory response was received, and returns
//! the Type V Reader status of either a SUCCESS or a FAIL. If an Inventory
//! response has been received, the current status will be changed to IDLE
//! regardless of whether the Type V Reader status was a SUCCESS or FAIL.
//!
//! \return eStatusTemp is the Type V Reader status
//
//*****************************************************************************
tNfcRwT5TConnectionStatus NFC_RW_T5T_getInventoryStatus(void)
{
	tNfcRwT5TConnectionStatus eStatusTemp;

	if(g_eNfcRwT5TCommStatus == NFC_RW_T5T_CONNECTION_INVENTORY_SUCCESS)
	{
		eStatusTemp = g_eNfcRwT5TCommStatus;
		// Clear Current State
		g_eNfcRwT5TCommStatus = NFC_RW_T5T_CONNECTION_IDLE;
	}
	else if(g_eNfcRwT5TCommStatus == NFC_RW_T5T_CONNECTION_INVENTORY_FAIL)
	{
		eStatusTemp = g_eNfcRwT5TCommStatus;
		// Clear Current State
		g_eNfcRwT5TCommStatus = NFC_RW_T5T_CONNECTION_IDLE;
	}

	return eStatusTemp;
}

//*****************************************************************************
//
//! NFC_RW_T5T_getReadSingleStatus - Checks for a Read Single response.
//!
//! This function checks if a Read Single response was received, and returns
//! the Type V Reader status of either a SUCCESS or a FAIL. If a Read Single
//! response has been received, the current status will be changed to IDLE
//! regardless of whether the Type V Reader status was a SUCCESS or FAIL.
//!
//! \return eStatusTemp is the Type V Reader status.
//
//*****************************************************************************
tNfcRwT5TConnectionStatus NFC_RW_T5T_getReadSingleStatus(void)
{
	tNfcRwT5TConnectionStatus eStatusTemp;

	if(g_eNfcRwT5TCommStatus == NFC_RW_T5T_CONNECTION_READ_SINGLE_SUCCESS)
	{
		eStatusTemp = g_eNfcRwT5TCommStatus;
		// Clear Current State
		g_eNfcRwT5TCommStatus = NFC_RW_T5T_CONNECTION_IDLE;
	}
	else if(g_eNfcRwT5TCommStatus == NFC_RW_T5T_CONNECTION_READ_SINGLE_FAIL)
	{
		eStatusTemp = g_eNfcRwT5TCommStatus;
		// Clear Current State
		g_eNfcRwT5TCommStatus = NFC_RW_T5T_CONNECTION_IDLE;
	}

	return eStatusTemp;
}

//*****************************************************************************
//
//! NFC_RW_T5T_getWriteSingleStatus - Checks for a Write Single response.
//!
//! This function checks if a Write Single response was received, and returns
//! the Type V Reader status of either a SUCCESS or a FAIL. If a Write Single
//! response has been received, the current status will be changed to IDLE
//! regardless of whether the Type V Reader status was a SUCCESS or FAIL.
//!
//! \return eStatusTemp is the Type V Reader status.
//
//*****************************************************************************
tNfcRwT5TConnectionStatus NFC_RW_T5T_getWriteSingleStatus(void)
{
	tNfcRwT5TConnectionStatus eStatusTemp;

	if(g_eNfcRwT5TCommStatus == NFC_RW_T5T_CONNECTION_WRITE_SINGLE_SUCCESS)
	{
		eStatusTemp = g_eNfcRwT5TCommStatus;
		// Clear Current State
		g_eNfcRwT5TCommStatus = NFC_RW_T5T_CONNECTION_IDLE;
	}
	else if(g_eNfcRwT5TCommStatus == NFC_RW_T5T_CONNECTION_WRITE_SINGLE_FAIL)
	{
		eStatusTemp = g_eNfcRwT5TCommStatus;
		// Clear Current State
		g_eNfcRwT5TCommStatus = NFC_RW_T5T_CONNECTION_IDLE;
	}

	return eStatusTemp;
}

//*****************************************************************************
//
//! NFC_RW_T5T_getLockBlockStatus - Checks for a Lock Block response.
//!
//! This function checks if a Lock Block response was received, and returns
//! the Type V Reader status of either a SUCCESS or a FAIL. If a Lock Block
//! response has been received, the current status will be changed to IDLE
//! regardless of whether the Type V Reader status was a SUCCESS or FAIL.
//!
//! \return eStatusTemp is the Type V Reader status.
//
//*****************************************************************************
tNfcRwT5TConnectionStatus NFC_RW_T5T_getLockBlockStatus(void)
{
	tNfcRwT5TConnectionStatus eStatusTemp;

	if(g_eNfcRwT5TCommStatus == NFC_RW_T5T_CONNECTION_LOCK_BLOCK_SUCCESS)
	{
		eStatusTemp = g_eNfcRwT5TCommStatus;
		// Clear Current State
		g_eNfcRwT5TCommStatus = NFC_RW_T5T_CONNECTION_IDLE;
	}
	else if(g_eNfcRwT5TCommStatus == NFC_RW_T5T_CONNECTION_LOCK_BLOCK_FAIL)
	{
		eStatusTemp = g_eNfcRwT5TCommStatus;
		// Clear Current State
		g_eNfcRwT5TCommStatus = NFC_RW_T5T_CONNECTION_IDLE;
	}

	return eStatusTemp;
}

//*****************************************************************************
//
//! NFC_RW_T5T_getSelectStatus - Checks for a Select response.
//!
//! This function checks if a Select response was received, and returns
//! the Type V Reader status of either a SUCCESS or a FAIL. If a Select
//! response has been received, the current status will be changed to IDLE
//! regardless of whether the Type V Reader status was a SUCCESS or FAIL.
//!
//! \return eStatusTemp is the Type V Reader status.
//
//*****************************************************************************
tNfcRwT5TConnectionStatus NFC_RW_T5T_getSelectStatus(void)
{
	tNfcRwT5TConnectionStatus eStatusTemp;

	if(g_eNfcRwT5TCommStatus == NFC_RW_T5T_CONNECTION_SELECT_SUCCESS)
	{
		eStatusTemp = g_eNfcRwT5TCommStatus;
		// Clear Current State
		g_eNfcRwT5TCommStatus = NFC_RW_T5T_CONNECTION_IDLE;
	}
	else if(g_eNfcRwT5TCommStatus == NFC_RW_T5T_CONNECTION_SELECT_FAIL)
	{
		eStatusTemp = g_eNfcRwT5TCommStatus;
		// Clear Current State
		g_eNfcRwT5TCommStatus = NFC_RW_T5T_CONNECTION_IDLE;
	}

	return eStatusTemp;
}

//*****************************************************************************
//
//! NFC_RW_T5T_getResetToReadyStatus - Checks for a Reset to Ready response.
//!
//! This function checks if a Reset to Ready response was received, and returns
//! the Type V Reader status of either a SUCCESS or a FAIL. If a Reset to Ready
//! response has been received, the current status will be changed to IDLE
//! regardless of whether the Type V Reader status was a SUCCESS or FAIL.
//!
//! \return eStatusTemp is the Type V Reader status.
//
//*****************************************************************************
tNfcRwT5TConnectionStatus NFC_RW_T5T_getResetToReadyStatus(void)
{
	tNfcRwT5TConnectionStatus eStatusTemp;

	if(g_eNfcRwT5TCommStatus == NFC_RW_T5T_CONNECTION_RESET_TO_READY_SUCCESS)
	{
		eStatusTemp = g_eNfcRwT5TCommStatus;
		// Clear Current State
		g_eNfcRwT5TCommStatus = NFC_RW_T5T_CONNECTION_IDLE;
	}
	else if(g_eNfcRwT5TCommStatus == NFC_RW_T5T_CONNECTION_RESET_TO_READY_FAIL)
	{
		eStatusTemp = g_eNfcRwT5TCommStatus;
		// Clear Current State
		g_eNfcRwT5TCommStatus = NFC_RW_T5T_CONNECTION_IDLE;
	}

	return eStatusTemp;
}

//*****************************************************************************
//
//! NFC_RW_T5T_getReadMultipleStatus - Checks for a Read Multiple response.
//!
//! This function checks if a Read Multiple was received, and returns
//! the Type V Reader status of either a SUCCESS or a FAIL. If a Read Multiple
//! response has been received, the current status will be changed to IDLE
//! regardless of whether the Type V Reader status was a SUCCESS or FAIL.
//!
//! \return eStatusTemp is the Type V Reader status.
//
//*****************************************************************************
tNfcRwT5TConnectionStatus NFC_RW_T5T_getReadMultipleStatus(void)
{
	tNfcRwT5TConnectionStatus eStatusTemp;

	if(g_eNfcRwT5TCommStatus == NFC_RW_T5T_CONNECTION_READ_MULT_SUCCESS)
	{
		eStatusTemp = g_eNfcRwT5TCommStatus;
		// Clear Current State
		g_eNfcRwT5TCommStatus = NFC_RW_T5T_CONNECTION_IDLE;
	}
	else if(g_eNfcRwT5TCommStatus == NFC_RW_T5T_CONNECTION_READ_MULT_FAIL)
	{
		eStatusTemp = g_eNfcRwT5TCommStatus;
		// Clear Current State
		g_eNfcRwT5TCommStatus = NFC_RW_T5T_CONNECTION_IDLE;
	}

	return eStatusTemp;
}

//*****************************************************************************
//
//! NFC_RW_T5T_getWriteAFIStatus - Checks for a Write AFI response.
//!
//! This function checks if a Write AFI response was received, and returns
//! the Type V Reader status of either a SUCCESS or a FAIL. If a Write AFI
//! response has been received, the current status will be changed to IDLE
//! regardless of whether the Type V Reader status was a SUCCESS or FAIL.
//!
//! \return eStatusTemp is the Type V Reader status.
//
//*****************************************************************************
tNfcRwT5TConnectionStatus NFC_RW_T5T_getWriteAFIStatus(void)
{
	tNfcRwT5TConnectionStatus eStatusTemp;

	if(g_eNfcRwT5TCommStatus == NFC_RW_T5T_CONNECTION_WRITE_AFI_SUCCESS)
	{
		eStatusTemp = g_eNfcRwT5TCommStatus;
		// Clear Current State
		g_eNfcRwT5TCommStatus = NFC_RW_T5T_CONNECTION_IDLE;

		// Update our AFI with the new AFI if the writiing was a success.
		g_ui8T5TAFI = g_ui8T5TWriteAFI;
	}
	else if(g_eNfcRwT5TCommStatus == NFC_RW_T5T_CONNECTION_WRITE_AFI_FAIL)
	{
		eStatusTemp = g_eNfcRwT5TCommStatus;
		// Clear Current State
		g_eNfcRwT5TCommStatus = NFC_RW_T5T_CONNECTION_IDLE;
	}

	return eStatusTemp;
}

//*****************************************************************************
//
//! NFC_RW_T5T_getLockAFIStatus - Checks for a Lock AFI response.
//!
//! This function checks if a Lock AFI response was received, and returns
//! the Type V Reader status of either a SUCCESS or a FAIL. If a Lock AFI
//! response has been received, the current status will be changed to IDLE
//! regardless of whether the Type V Reader status was a SUCCESS or FAIL.
//!
//! \return eStatusTemp is the Type V Reader status.
//
//*****************************************************************************
tNfcRwT5TConnectionStatus NFC_RW_T5T_getLockAFIStatus(void)
{
	tNfcRwT5TConnectionStatus eStatusTemp;

	if(g_eNfcRwT5TCommStatus == NFC_RW_T5T_CONNECTION_LOCK_AFI_SUCCESS)
	{
		eStatusTemp = g_eNfcRwT5TCommStatus;
		// Clear Current State
		g_eNfcRwT5TCommStatus = NFC_RW_T5T_CONNECTION_IDLE;
	}
	else if(g_eNfcRwT5TCommStatus == NFC_RW_T5T_CONNECTION_LOCK_AFI_FAIL)
	{
		eStatusTemp = g_eNfcRwT5TCommStatus;
		// Clear Current State
		g_eNfcRwT5TCommStatus = NFC_RW_T5T_CONNECTION_IDLE;
	}

	return eStatusTemp;
}

//*****************************************************************************
//
//! NFC_RW_T5T_getGetSysInfoStatus - Checks for a Get System Info response.
//!
//! This function checks if a Get System Info response was received, and returns
//! the Type V Reader status of either a SUCCESS or a FAIL. If a Get System Info
//! response has been received, the current status will be changed to IDLE
//! regardless of whether the Type V Reader status was a SUCCESS or FAIL.
//!
//! \return eStatusTemp is the Type V Reader status.
//
//*****************************************************************************
tNfcRwT5TConnectionStatus NFC_RW_T5T_getGetSysInfoStatus(void)
{
	tNfcRwT5TConnectionStatus eStatusTemp;

	if(g_eNfcRwT5TCommStatus == NFC_RW_T5T_CONNECTION_GET_SYS_INFO_SUCCESS)
	{
		eStatusTemp = g_eNfcRwT5TCommStatus;
		// Clear Current State
		g_eNfcRwT5TCommStatus = NFC_RW_T5T_CONNECTION_IDLE;
	}
	else if(g_eNfcRwT5TCommStatus == NFC_RW_T5T_CONNECTION_GET_SYS_INFO_FAIL)
	{
		eStatusTemp = g_eNfcRwT5TCommStatus;
		// Clear Current State
		g_eNfcRwT5TCommStatus = NFC_RW_T5T_CONNECTION_IDLE;
	}

	return eStatusTemp;
}

//*****************************************************************************
//
//! NFC_RW_T5T_getRawWriteStatus - Checks for any Type V tag response.
//!
//! This function checks if a Type V tag response was received, and returns
//! the Type V Reader status of either a SUCCESS or a FAIL. If a Type V tag
//! response has been received, the current status will be changed to IDLE
//! regardless of whether the Type V Reader status was a SUCCESS or FAIL.
//!
//! \return eStatusTemp is the Type V Reader status.
//
//*****************************************************************************
tNfcRwT5TConnectionStatus NFC_RW_T5T_getRawWriteStatus(void)
{
	tNfcRwT5TConnectionStatus eStatusTemp;

	if(g_eNfcRwT5TCommStatus == NFC_RW_T5T_CONNECTION_RAW_WRITE_SUCCESS)
	{
		eStatusTemp = g_eNfcRwT5TCommStatus;
		// Clear Current State
		g_eNfcRwT5TCommStatus = NFC_RW_T5T_CONNECTION_IDLE;
	}
	else if(g_eNfcRwT5TCommStatus == NFC_RW_T5T_CONNECTION_RAW_WRITE_FAIL)
	{
		eStatusTemp = g_eNfcRwT5TCommStatus;
		// Clear Current State
		g_eNfcRwT5TCommStatus = NFC_RW_T5T_CONNECTION_IDLE;
	}

	return eStatusTemp;
}

//*****************************************************************************
//
//! NFC_RW_T5T_sendInventoryCmd - Sends the Inventory Command.
//!
//! \param ui8ReqFlag is the Request Flag to be sent with the Type V command
//! \param ui8AFI is the AFI that may be sent by with the Inventory command
//! \param bSendAFI is a boolean to determine whether the AFI is sent or not
//!
//! This function is used set the correct state for sending an Inventory
//! command, and also to process any application based variables that have
//! been passed into the Type V Reader layer.
//!
//! \return ui8Status indicates if the state was successfully changed or not.
//
//*****************************************************************************
uint8_t NFC_RW_T5T_sendInventoryCmd(uint8_t ui8ReqFlag, uint8_t ui8AFI, bool bSendAFI)
{
	uint8_t ui8Status = STATUS_SUCCESS;

	if(g_eNfcRwT5TCommStatus == NFC_RW_T5T_CONNECTION_IDLE)
	{
		g_eNfcRwT5TCommStatus = NFC_RW_T5T_CONNECTION_INVENTORY;

		g_ui8T5TRequestFlag = ui8ReqFlag;

		g_ui8T5TAFI = ui8AFI;

		g_bSendInvAFI = bSendAFI;
	}
	else
	{
		ui8Status = STATUS_FAIL;
	}

	return ui8Status;
}

//*****************************************************************************
//
//! NFC_RW_T5T_sendStayQuietCmd - Sends the Stay Quiet Command.
//!
//! \param ui8ReqFlag is the Request Flag to be sent with the Type V command
//!
//! This function is used set the correct state for sending a Stay Quiet
//! command, and also to process any application based variables that have
//! been passed into the Type V Reader layer.
//!
//! \return ui8Status indicates if the state was successfully changed or not.
//
//*****************************************************************************
uint8_t NFC_RW_T5T_sendStayQuietCmd(uint8_t ui8ReqFlag)
{
	uint8_t ui8Status = STATUS_SUCCESS;

	if(g_eNfcRwT5TCommStatus == NFC_RW_T5T_CONNECTION_IDLE)
	{
		g_eNfcRwT5TCommStatus = NFC_RW_T5T_CONNECTION_STAY_QUIET;

		g_ui8T5TRequestFlag = ui8ReqFlag;
	}
	else
	{
		ui8Status = STATUS_FAIL;
	}

	return ui8Status;
}

//*****************************************************************************
//
//! NFC_RW_T5T_sendReadSingleCmd - Sends the Read Single Command.
//!
//! \param ui8ReqFlag is the Request Flag to be sent with the Type V command
//! \param ui16BlockNumber is the Block Number to be read from the tag
//!
//! This function is used set the correct state for sending a Read Single
//! command, and also to process any application based variables that have
//! been passed into the Type V Reader layer.
//!
//! \return ui8Status indicates if the state was successfully changed or not.
//
//*****************************************************************************
uint8_t NFC_RW_T5T_sendReadSingleCmd(uint8_t ui8ReqFlag, uint16_t ui16BlockNumber)
{
	uint8_t ui8Status = STATUS_SUCCESS;

	if(g_eNfcRwT5TCommStatus == NFC_RW_T5T_CONNECTION_IDLE)
	{
		g_eNfcRwT5TCommStatus = NFC_RW_T5T_CONNECTION_READ_SINGLE_BLOCK;

		g_ui8T5TRequestFlag = ui8ReqFlag;

		g_ui16T5TBlockNumber = ui16BlockNumber;
	}
	else
	{
		ui8Status = STATUS_FAIL;
	}

	return ui8Status;
}

//*****************************************************************************
//
//! NFC_RW_T5T_sendWriteSingleCmd - Sends the Write Single Command.
//!
//! \param ui8ReqFlag is the Request Flag to be sent with the Type V command
//! \param ui16BlockNumber is the Block Number to be read from the tag
//!
//! This function is used set the correct state for sending a Write Single
//! command, and also to process any application based variables that have
//! been passed into the Type V Reader layer.
//!
//! \return ui8Status indicates if the state was successfully changed or not.
//
//*****************************************************************************
uint8_t NFC_RW_T5T_sendWriteSingleCmd(uint8_t ui8ReqFlag, uint16_t ui16BlockNumber, const uint8_t * pui8Buffer, uint8_t ui8Length)
{
	uint8_t ui8Status = STATUS_SUCCESS;

	if(g_eNfcRwT5TCommStatus == NFC_RW_T5T_CONNECTION_IDLE)
	{
		g_eNfcRwT5TCommStatus = NFC_RW_T5T_CONNECTION_WRITE_SINGLE_BLOCK;

		g_ui8T5TRequestFlag = ui8ReqFlag;

		g_ui16T5TBlockNumber = ui16BlockNumber;

		g_pui8T5TTxData = (uint8_t *)pui8Buffer;

		g_ui8T5TTxLength = ui8Length;
	}
	else
	{
		ui8Status = STATUS_FAIL;
	}

	return ui8Status;
}

//*****************************************************************************
//
//! NFC_RW_T5T_sendLockBlockCmd - Sends the Lock Block Command.
//!
//! \param ui8ReqFlag is the Request Flag to be sent with the Type V command
//! \param ui16BlockNumber is the Block Number to be read from the tag
//!
//! This function is used set the correct state for sending a Lock Block
//! command, and also to process any application based variables that have
//! been passed into the Type V Reader layer.
//!
//! \return ui8Status indicates if the state was successfully changed or not.
//
//*****************************************************************************
uint8_t NFC_RW_T5T_sendLockBlockCmd(uint8_t ui8ReqFlag, uint16_t ui16BlockNumber)
{
	uint8_t ui8Status = STATUS_SUCCESS;

	if(g_eNfcRwT5TCommStatus == NFC_RW_T5T_CONNECTION_IDLE)
	{
		g_eNfcRwT5TCommStatus = NFC_RW_T5T_CONNECTION_LOCK_BLOCK;

		g_ui8T5TRequestFlag = ui8ReqFlag;

		g_ui16T5TBlockNumber = ui16BlockNumber;
	}
	else
	{
		ui8Status = STATUS_FAIL;
	}

	return ui8Status;
}

//*****************************************************************************
//
//! NFC_RW_T5T_sendSelect - Sends the Select Command.
//!
//! \param ui8ReqFlag is the Request Flag to be sent with the Type V command
//!
//! This function is used set the correct state for sending a Select
//! command, and also to process any application based variables that have
//! been passed into the Type V Reader layer.
//!
//! \return ui8Status indicates if the state was successfully changed or not.
//
//*****************************************************************************
uint8_t NFC_RW_T5T_sendSelect(uint8_t ui8ReqFlag)
{
	uint8_t ui8Status = STATUS_SUCCESS;

	if(g_eNfcRwT5TCommStatus == NFC_RW_T5T_CONNECTION_IDLE)
	{
		g_eNfcRwT5TCommStatus = NFC_RW_T5T_CONNECTION_SELECT;

		g_ui8T5TRequestFlag = ui8ReqFlag;
	}
	else
	{
		ui8Status = STATUS_FAIL;
	}

	return ui8Status;
}

//*****************************************************************************
//
//! NFC_RW_T5T_sendResetToReady - Sends the Reset to Ready Command.
//!
//! \param ui8ReqFlag is the Request Flag to be sent with the Type V command
//!
//! This function is used set the correct state for sending a Reset to Ready
//! command, and also to process any application based variables that have
//! been passed into the Type V Reader layer.
//!
//! \return ui8Status indicates if the state was successfully changed or not.
//
//*****************************************************************************
uint8_t NFC_RW_T5T_sendResetToReady(uint8_t ui8ReqFlag)
{
	uint8_t ui8Status = STATUS_SUCCESS;

	if(g_eNfcRwT5TCommStatus == NFC_RW_T5T_CONNECTION_IDLE)
	{
		g_eNfcRwT5TCommStatus = NFC_RW_T5T_CONNECTION_RESET_TO_READY;

		g_ui8T5TRequestFlag = ui8ReqFlag;
	}
	else
	{
		ui8Status = STATUS_FAIL;
	}

	return ui8Status;
}

//*****************************************************************************
//
//! NFC_RW_T5T_sendReadMultipleCmd - Sends the Read Multiple Command.
//!
//! \param ui8ReqFlag is the Request Flag to be sent with the Type V command
//! \param ui16BlockNumber is the starting Block Number to be read from the tag
//! \param ui8BlockCount is the Total Number of Blocks to be read from the tag
//!
//! This function is used set the correct state for sending a Read Multiple
//! command, and also to process any application based variables that have
//! been passed into the Type V Reader layer.
//!
//! \return ui8Status indicates if the state was successfully changed or not.
//
//*****************************************************************************
uint8_t NFC_RW_T5T_sendReadMultipleCmd(uint8_t ui8ReqFlag, uint16_t ui16BlockNumber, uint8_t ui8BlockCount)
{
	uint8_t ui8Status = STATUS_SUCCESS;
	uint16_t ui16ReadBlockLength;

	if(g_eNfcRwT5TCommStatus == NFC_RW_T5T_CONNECTION_IDLE)
	{
		if (ui8BlockCount-1 <= g_ui16T5TVICCBlockCount)
		{
			if ((ui16BlockNumber+ui8BlockCount-1) <= g_ui16T5TVICCBlockCount)
			{
				// Calculate the length of the command based on:
				if (ui8ReqFlag & T5T_REQ_FLAG_OPTION)
				{
					// (Amount of blocks requested to be read) * (Block length+1 (since hex starts @ 0x00) + 1 (for the Block Security Status byte))
					ui16ReadBlockLength = ((uint16_t)ui8BlockCount) * ((uint16_t)g_ui8T5TVICCBlockSize+1 + 1);
				}
				else
				{
					// (Amount of blocks requested to be read) * (Block length+1 (since hex starts @ 0x00))
					ui16ReadBlockLength = ((uint16_t)ui8BlockCount) * ((uint16_t)g_ui8T5TVICCBlockSize+1);
				}

				if (ui16ReadBlockLength < 256)
				{
					g_eNfcRwT5TCommStatus = NFC_RW_T5T_CONNECTION_READ_MULT_BLOCK;

					g_ui8T5TRequestFlag = ui8ReqFlag;

					g_ui16T5TBlockNumber = ui16BlockNumber;

					g_ui8T5TBlockCount = ui8BlockCount - 1;
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
//! NFC_RW_T5T_sendWriteAFICmd - Sends the Write AFI Command.
//!
//! \param ui8ReqFlag is the Request Flag to be sent with the Type V command
//! \param ui8NewAFI is the new AFI value to be written to the tag
//!
//! This function is used set the correct state for sending a Write AFI
//! command, and also to process any application based variables that have
//! been passed into the Type V Reader layer.
//!
//! \return ui8Status indicates if the state was successfully changed or not.
//
//*****************************************************************************
uint8_t NFC_RW_T5T_sendWriteAFICmd(uint8_t ui8ReqFlag, uint8_t ui8NewAFI)
{
	uint8_t ui8Status = STATUS_SUCCESS;

	if(g_eNfcRwT5TCommStatus == NFC_RW_T5T_CONNECTION_IDLE)
	{
		g_eNfcRwT5TCommStatus = NFC_RW_T5T_CONNECTION_WRITE_AFI_BLOCK;

		g_ui8T5TRequestFlag = ui8ReqFlag;

		g_ui8T5TWriteAFI = ui8NewAFI;
	}
	else
	{
		ui8Status = STATUS_FAIL;
	}

	return ui8Status;
}

//*****************************************************************************
//
//! NFC_RW_T5T_sendLockAFICmd - Sends the Lock AFI Command.
//!
//! \param ui8ReqFlag is the Request Flag to be sent with the Type V command
//!
//! This function is used set the correct state for sending a Lock AFI
//! command, and also to process any application based variables that have
//! been passed into the Type V Reader layer.
//!
//! \return ui8Status indicates if the state was successfully changed or not.
//
//*****************************************************************************
uint8_t NFC_RW_T5T_sendLockAFICmd(uint8_t ui8ReqFlag)
{
	uint8_t ui8Status = STATUS_SUCCESS;

	if(g_eNfcRwT5TCommStatus == NFC_RW_T5T_CONNECTION_IDLE)
	{
		g_eNfcRwT5TCommStatus = NFC_RW_T5T_CONNECTION_LOCK_AFI_BLOCK;

		g_ui8T5TRequestFlag = ui8ReqFlag;
	}
	else
	{
		ui8Status = STATUS_FAIL;
	}

	return ui8Status;
}

//*****************************************************************************
//
//! NFC_RW_T5T_sendGetSysInfoCmd - Sends the Get System Information Command.
//!
//! \param ui8ReqFlag is the Request Flag to be sent with the Type V command
//! \param ui8CommandCode is the command code for the issued ISO15693 command
//!
//! This function is used set the correct state for sending a Get System Info
//! command, and also to process any application based variables that have
//! been passed into the Type V Reader layer.
//!
//! \return ui8Status indicates if the state was successfully changed or not.
//
//*****************************************************************************
uint8_t NFC_RW_T5T_sendGetSysInfoCmd(uint8_t ui8ReqFlag, uint8_t ui8CommandCode)
{
	uint8_t ui8Status = STATUS_SUCCESS;

	if(g_eNfcRwT5TCommStatus == NFC_RW_T5T_CONNECTION_IDLE)
	{
		g_eNfcRwT5TCommStatus = NFC_RW_T5T_CONNECTION_GET_SYS_INFO;

		g_ui8T5TRequestFlag = ui8ReqFlag;

		g_ui8T5TCommandCode = ui8CommandCode;
	}
	else
	{
		ui8Status = STATUS_FAIL;
	}

	return ui8Status;
}

//*****************************************************************************
//
//! NFC_RW_T5T_sendRawWriteCmd - Sends a raw ISO15693 formatted data packet.
//!
//! \param ui8ReqFlag is the Request Flag to be sent with the Type V command
//! \param ui8CommandCode is the command code for the issued ISO15693 command
//! \param pui8Buffer is a pointer to additional data for the ISO15693 command
//! \param ui8Length is the length of the data stored at pui8Buffer
//! \param bOptionDelay is used to indicate if a delay is required when the
//! option flag is set as described in the ISO15693 specifications
//!//! This function is used set the correct state for sending a Raw data packet
//! for ISO15693, and also to process any application based variables that have
//! been passed into the Type V Reader layer.
//!
//! \return ui8Status indicates if the state was successfully changed or not.
//
//*****************************************************************************
uint8_t NFC_RW_T5T_sendRawWriteCmd(uint8_t ui8ReqFlag, uint8_t ui8CommandCode, const uint8_t * pui8Buffer, uint8_t ui8Length, bool bOptionDelay)
{
	uint8_t ui8Status = STATUS_SUCCESS;

	if(g_eNfcRwT5TCommStatus == NFC_RW_T5T_CONNECTION_IDLE)
	{
		g_eNfcRwT5TCommStatus = NFC_RW_T5T_CONNECTION_RAW_WRITE;

		g_ui8T5TRequestFlag = ui8ReqFlag;

		g_ui8T5TCommandCode = ui8CommandCode;

		g_pui8T5TTxData = (uint8_t *)pui8Buffer;

		g_ui8T5TTxLength = ui8Length;

		g_bT5TOptionDelay = bOptionDelay;
	}
	else
	{
		ui8Status = STATUS_FAIL;
	}

	return ui8Status;
}

//*****************************************************************************
//
//! NFC_RW_T5T_getPacketStatus - Gets the last Type 5 data packet received.
//!
//! \param pui8RXData this is a double pointer to receive the current pointer
//! to the location of the Type V data bytes received buffer
//! \param pui8Length this is a pointer to get the length of the received Type
//! V data
//!
//! This function returns the packet length and the location of the packet
//! contents of the latest Type V data packet that was received.
//!
//! \return None.
//
//*****************************************************************************
void NFC_RW_T5T_getPacketStatus(uint8_t ** pui8RXData, uint8_t * pui8Length)
{
	*pui8Length = g_ui8T5TBytesReceived;

	*pui8RXData = g_pui8T5TRxData;
}

//*****************************************************************************
//
//! NFC_RW_T5T_getT5TErrorCode - Returns the current Type V error code.
//!
//! This function returns the current Type V error code. This is important for
//! for the application layer in order to handle errors in communication with
//! Type V tags.
//!
//! \return g_ui8T5TErrorCode is the current Type V error code.
//
//*****************************************************************************
uint8_t NFC_RW_T5T_getT5TErrorCode(void)
{
	return g_ui8T5TErrorCode;
}

//*****************************************************************************
//
//! NFC_RW_T5T_getT5TDSFID - Returns the DSFID read from the Type V tag.
//!
//! This function returns the DSFID from the Type V Tag. The DSFID is acquired
//! through the Inventory and Get System Information commands.
//!
//! \return g_ui8T5TDSFID is the DSFID of the Type V Tag.
//
//*****************************************************************************
uint8_t NFC_RW_T5T_getT5TDSFID(void)
{
	return g_ui8T5TDSFID;
}

//*****************************************************************************
//
//! NFC_RW_T5T_getT5TAFI - Returns the AFI read from the Type V tag.
//!
//! This function returns the AFI from the Type V Tag. The AFI is acquired
//! through the Get System Information command.
//!
//! \return g_ui8T5TAFI is the AFI of the Type V Tag.
//
//*****************************************************************************
uint8_t NFC_RW_T5T_getT5TAFI(void)
{
	return g_ui8T5TAFI;
}

//*****************************************************************************
//
//! NFC_RW_T5T_getT5TUID - Returns the UID read from the Type V tag.
//!
//! This function returns the UID from the Type V Tag. The UID is acquired
//! through the Inventory command, and must be used in order to issue any
//! addressed Type V commands.
//!
//! \return g_ui8T5TUID is the UID of the Type V Tag.
//
//*****************************************************************************
void NFC_RW_T5T_getT5TUID(uint8_t ** pui8NfcVId, uint8_t * ui8NfcVIdLength)
{
	*pui8NfcVId = g_ui8T5TUID;

	*ui8NfcVIdLength = 8;
}

//*****************************************************************************
//
//! NFC_RW_T5T_getVICCBlockCount - Returns number of blocks in the Type V tag.
//!
//! This function returns the number of memory blocks that are contained within
//! the Type V tag. This information is acquired through the Get System
//! Information command. Using the block count along with the VICC block size
//! it is possible to determine the total memory size of the Type V tag.
//!
//! \return g_ui16T5TVICCBlockCount is the number of blocks in the Type V tag.
//
//*****************************************************************************
uint16_t NFC_RW_T5T_getVICCBlockCount(void)
{
	return g_ui16T5TVICCBlockCount;
}

//*****************************************************************************
//
//! NFC_RW_T5T_getVICCBlockSize - Returns the block size for the Type V tag.
//!
//! This function returns the number of bytes per block for the Type V tag.
//! This information is acquired through the Get System Information command.
//! Using the size per block along with the VICC block count it is possible to
//! determine the total memory size of the Type V tag.
//! Note that the size returned by this function is the one less than the actual
//! size. Therefore a size returned as 0x03 would indicate 4 bytes per block.
//!
//! \return g_ui8T5TVICCBlockSize is the number of bytes per block in the
//! Type V tag.
//
//*****************************************************************************
uint8_t NFC_RW_T5T_getVICCBlockSize(void)
{
	return g_ui8T5TVICCBlockSize;
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************

