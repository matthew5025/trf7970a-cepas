//*****************************************************************************
//
// iso7816_4.c - Implementation of ISO7816-4 APIs.
//
// Copyright (c) 2015 Texas Instruments Incorporated.  All rights reserved.
// TI Information - Selective Disclosure
//
//*****************************************************************************

#include "iso_7816_4.h"

//*****************************************************************************
//
//! \addtogroup iso7816_4_api ISO7816-4 API Functions
//! @{
//!
//! ISO7816-4 is the layer used to send/receive APDUs for T4T tags that are
//! activated to level 4.
//
//*****************************************************************************


uint16_t g_ui16SelectedFile;
uint8_t g_ui8SelectedFileIndex;
bool g_bApplicationSelected;
uint8_t g_ui8SelectedAppIndex;

tType4AppDS g_pType4DS;

uint16_t g_ui16CommandResponse;

uint8_t * g_pui8FileData;
uint8_t g_ui8BytestoSend;

uint8_t g_bNDEFRead;

// The maximum len for an application ID is 16 bytes
uint8_t g_ui8AppId[16];
uint8_t g_ui8AppLen;

// File ID of the file being selected
uint16_t g_ui16FileReadID;

// Start Index of data to be read
uint16_t g_ui16StartDataIndex;

// Number of datas requested to be read/written to/from the tag
uint8_t g_ui8BytesRequested;

// Data ptr to data to be sent
uint8_t * g_pui8DataBuffer;

// Data written / read index
uint8_t g_ui8ISO_7816_4_4DataIndex;

// Data Received
uint8_t * g_pui8ISO_7816_4_4RxData;

uint16_t g_ui16ISO_7816_4_BytesReceived;

tISO7816ConnectionStatus g_eISO7816CommStatus;

tISO7816ApplicationSelect g_e7816Select;

uint8_t g_ui8CEStatus;

uint8_t g_ui8TransferBytes;

uint16_t g_ui16StartIndex;

void ISO_7816_4_init(void)
{
	// Set Selected File to null
	g_ui16SelectedFile = 0;
	g_ui16CommandResponse = 0x0000;
	g_ui8BytestoSend = 0x00;
	g_pui8FileData = 0x00;
	g_ui8SelectedAppIndex = 0x00;
	g_ui8SelectedFileIndex = 0x00;
	g_eISO7816CommStatus = ISO_7816_4_CONNECTION_IDLE;
	g_e7816Select = ISO_7816_4_SELECT_IDLE;
	g_ui16FileReadID = 0x0000;
	g_ui16StartDataIndex = 0x00;
	g_ui8BytesRequested = 0x00;
	g_pui8DataBuffer = 0x00;
	g_ui8ISO_7816_4_4DataIndex = 0x00;
	g_ui16ISO_7816_4_BytesReceived = 0;
	g_bApplicationSelected = 0;
	g_ui8CEStatus = 0x00;
	g_ui8TransferBytes = 0x00;
	g_ui16StartIndex = 0x00;

}

void ISO_7816_4_setAppDS(tType4AppDS pType4DS)
{
	g_pType4DS = pType4DS;
}

uint8_t ISO_7816_4_stateMachine(uint8_t * pui8PduBufferPtr, uint8_t ui8AvailableSpace)
{
	uint8_t ui8Counter;
	uint8_t ui8Offset = 0;

	switch(g_eISO7816CommStatus)
	{
	case ISO_7816_4_CONNECTION_IDLE:
		// CLA
		pui8PduBufferPtr[ui8Offset++] = 0x00;
		// Do a dummy read of 1 byte
		pui8PduBufferPtr[ui8Offset++] = (uint8_t) INS_READ_BINARY;

		pui8PduBufferPtr[ui8Offset++] = (uint8_t) 0x00;
		pui8PduBufferPtr[ui8Offset++] = (uint8_t) 0x00;
		pui8PduBufferPtr[ui8Offset++] = 0x01;

		break;
	case ISO_7816_4_CONNECTION_SELECT:

		// Check the Application ID is less than or equal to 16 ( or less than 17)
		if(g_e7816Select == ISO_7816_4_SELECT_APP_ID && g_ui8AppLen < 17)
		{
			// CLA
			pui8PduBufferPtr[ui8Offset++] = 0x00;

			// Select Command
			pui8PduBufferPtr[ui8Offset++] = 0xA4;

			// Select by ID (P1 = 0x04, P2 = 0x00)
			pui8PduBufferPtr[ui8Offset++] = 0x04;
			pui8PduBufferPtr[ui8Offset++] = 0x00;

			// Length of UID
			pui8PduBufferPtr[ui8Offset++] = g_ui8AppLen;

			for(ui8Counter = 0; ui8Counter < g_ui8AppLen; ui8Counter++)
			{
				pui8PduBufferPtr[ui8Offset++] = g_ui8AppId[ui8Counter];
			}

			// MLe
			pui8PduBufferPtr[ui8Offset++] = 0x00;

			g_eISO7816CommStatus = ISO_7816_4_CONNECTION_WAIT_SELECT_RSP;
		}
		else if(g_e7816Select == ISO_7816_4_SELECT_FILE_ID)
		{
			// CLA
			pui8PduBufferPtr[ui8Offset++] = 0x00;

			// Select Command
			pui8PduBufferPtr[ui8Offset++] = 0xA4;

			// Select by ID (P1 = 0x04, P2 = 0x00)
			pui8PduBufferPtr[ui8Offset++] = 0x00;
			pui8PduBufferPtr[ui8Offset++] = 0x0C;

			// Length of File
			pui8PduBufferPtr[ui8Offset++] = 0x02;

			pui8PduBufferPtr[ui8Offset++] = (uint8_t) (g_ui16FileReadID & 0xFF);
			pui8PduBufferPtr[ui8Offset++] = (uint8_t) (g_ui16FileReadID >> 8);


			g_eISO7816CommStatus = ISO_7816_4_CONNECTION_WAIT_SELECT_RSP;
		}
		else
		{
			// Error occurred
			g_eISO7816CommStatus = ISO_7816_4_CONNECTION_IDLE;
		}

		break;

	case ISO_7816_4_CONNECTION_SELECT_FAIL:
		// Wait until the user clears the state by using the check Response AP
		break;
	case ISO_7816_4_CONNECTION_SELECT_SUCCESS:
		// Wait until the user clears the state by using the check Response AP
		break;
	case ISO_7816_4_CONNECTION_WAIT_SELECT_RSP:
		break;
	case ISO_7816_4_CONNECTION_WRITE:

		// CLA
		pui8PduBufferPtr[ui8Offset++] = 0x00;

		pui8PduBufferPtr[ui8Offset++] = (uint8_t) INS_UPDATE_BINARY;

		pui8PduBufferPtr[ui8Offset++] = (uint8_t) (g_ui16StartDataIndex >> 8);
		pui8PduBufferPtr[ui8Offset++] = (uint8_t) (g_ui16StartDataIndex & 0xFF);
		pui8PduBufferPtr[ui8Offset++] = g_ui8BytesRequested;

		// Subtract 4 bytes from the total available bytes to ensure the packet fits
		ui8AvailableSpace = ui8AvailableSpace - 4;

		for(ui8Counter = 0; ui8Counter < g_ui8BytesRequested; ui8Counter++)
		{
			pui8PduBufferPtr[ui8Offset++] = g_pui8DataBuffer[g_ui8ISO_7816_4_4DataIndex++];
		}

		g_eISO7816CommStatus = ISO_7816_4_CONNECTION_WAIT_WRITE_RSP;

		break;
	case ISO_7816_4_CONNECTION_WAIT_WRITE_RSP:
		break;
	case ISO_7816_4_CONNECTION_READ:

		// CLA
		pui8PduBufferPtr[ui8Offset++] = 0x00;

		pui8PduBufferPtr[ui8Offset++] = (uint8_t) INS_READ_BINARY;

		pui8PduBufferPtr[ui8Offset++] = (uint8_t) (g_ui16StartDataIndex >> 8);
		pui8PduBufferPtr[ui8Offset++] = (uint8_t) (g_ui16StartDataIndex & 0xFF);
		pui8PduBufferPtr[ui8Offset++] = g_ui8BytesRequested;

		g_eISO7816CommStatus = ISO_7816_4_CONNECTION_WAIT_READ_RSP;

		break;
	case ISO_7816_4_CONNECTION_WAIT_READ_RSP:
		break;
	case ISO_7816_4_CONNECTION_READ_SUCCESS:
		break;
	case ISO_7816_4_CONNECTION_READ_FAIL:
		break;
	case ISO_7816_4_CONNECTION_RAW_WRITE:
		for(ui8Counter = 0; ui8Counter < g_ui8BytesRequested; ui8Counter++)
		{
			pui8PduBufferPtr[ui8Offset++] = g_pui8DataBuffer[ui8Counter];
		}

		g_eISO7816CommStatus = ISO_7816_4_CONNECTION_WAIT_RAW_WRITE_RSP;

		break;
	case ISO_7816_4_CONNECTION_WAIT_RAW_WRITE_RSP:
		break;
	case ISO_7816_4_CONNECTION_WRITE_RAW_SUCCESS:
		break;
	case ISO_7816_4_CONNECTION_WRITE_RAW_FAIL:
		break;
	case ISO_7816_4_TAG_MODE:
		// Copy the bytes to be sent into the TX buffer
		for(ui8Counter=0;ui8Counter<g_ui8BytestoSend;ui8Counter++)
		{
			pui8PduBufferPtr[ui8Offset++] = g_pui8FileData[ui8Counter];
		}

		// Append SW1-SW2
		pui8PduBufferPtr[ui8Offset++] = (uint8_t) (g_ui16CommandResponse >> 8);
		pui8PduBufferPtr[ui8Offset++] = (uint8_t) (0xFF & g_ui16CommandResponse);
		break;
	}



	return ui8Offset;
}

uint8_t ISO_7816_4_processReceivedRequest(uint8_t * pui8RxPayload , uint8_t ui8CmdLength)
{
	uint8_t ui8Status = STATUS_SUCCESS;

	tISO_7816_4_INS eInsValue;
	uint8_t ui8CLAValue;
	uint8_t ui8P1Value;
	uint8_t ui8P2Value;

	uint8_t ui8Index;
	uint8_t ui8AppIDIdx;
	uint16_t ui16FileTemp;
	bool bAppIdValid;
	bool bFileValid;

	ui8CLAValue = pui8RxPayload[CLA_INDEX];

	if(ui8CmdLength < 4)
	{
		g_ui16CommandResponse = SW1_SW2_APP_NOT_FOUND;
		g_ui8BytestoSend = 0x00;
	}
	else if(ui8CLAValue == 0x00)
	{
		eInsValue = (tISO_7816_4_INS) pui8RxPayload[INS_INDEX];
		ui8P1Value = pui8RxPayload[P1_INDEX];
		ui8P2Value = pui8RxPayload[P2_INDEX];

		switch(eInsValue)
		{
		case INS_SELECT:

			// Select By Name (0x04) - First or only occurrence (0x00)
			if(ui8P1Value == 0x04 && ui8P2Value == 0x00)
			{
				if(g_pType4DS.ui8AppArrayLen == 0x00)
				{
					g_ui16CommandResponse = SW1_SW2_APP_FOUND;
				}
				else
				{
					// Check if application exists
					for(ui8Index=0;ui8Index<g_pType4DS.ui8AppArrayLen;ui8Index++)
					{
						// ui8Index contains the number of the application

						// Check if the length of the application is correct
						if(g_pType4DS.sType4AppArray[ui8Index].ui8AppIdLen == pui8RxPayload[LC_INDEX])
						{
							for(ui8AppIDIdx=0;ui8AppIDIdx<pui8RxPayload[LC_INDEX];ui8AppIDIdx++)
							{
								// Check if the Application ID is correct
								if(g_pType4DS.sType4AppArray[ui8Index].pui8AppId[ui8AppIDIdx] == pui8RxPayload[DATA_INDEX+ui8AppIDIdx])
								{
									bAppIdValid = true;
								}
								else
								{
									bAppIdValid = false;
									break;
								}
							}
						}
						else
						{
							bAppIdValid = false;
						}

						if(bAppIdValid == true)
						{
							g_ui16CommandResponse = SW1_SW2_APP_FOUND;
							g_ui8BytestoSend = 0x00;
							g_ui8SelectedAppIndex = ui8Index;
							g_bApplicationSelected = true;
							break;
						}
						else
						{
							g_ui16CommandResponse = SW1_SW2_APP_NOT_FOUND;
							g_ui8BytestoSend = 0x00;
							g_bApplicationSelected = false;
						}
					}
				}

			}
			// Select By File Identifier (x00) - First or only occurrence (0x0C)
			else if(ui8P1Value == 0x00 && ui8P2Value == 0x0C && pui8RxPayload[LC_INDEX] == 0x02 && g_bApplicationSelected == true)
			{
				// Ui8Index contains the index of the file

				for(ui8Index=0;ui8Index<g_pType4DS.sType4AppArray[g_ui8SelectedAppIndex].ui8Type4FileLen;ui8Index++)
				{
					ui16FileTemp = (uint16_t) ((pui8RxPayload[DATA_INDEX] << 8) + pui8RxPayload[DATA_INDEX+1]);
					if(ui16FileTemp == g_pType4DS.sType4AppArray[g_ui8SelectedAppIndex].pui8Type4FileArray[ui8Index].ui16Type4FileId)
					{
						bFileValid = true;
					}
					else
					{
						bFileValid = false;
					}

					if(bFileValid == true)
					{
						g_ui8SelectedFileIndex = ui8Index;
						g_ui16CommandResponse = SW1_SW2_APP_FOUND;
						g_ui8BytestoSend = 0x00;
						g_ui16SelectedFile = ui16FileTemp;
						break;
					}
					else
					{
						g_ui16CommandResponse = SW1_SW2_APP_NOT_FOUND;
						g_ui8BytestoSend = 0x00;
					}
				}
			}
			else
			{
				g_ui16CommandResponse = SW1_SW2_APP_NOT_FOUND;
				g_ui8BytestoSend = 0x00;
			}
			break;
		case INS_READ_BINARY:

			// ui16FileTemp holds the last address being requested
			if(g_ui16SelectedFile != 0)
			{
				// Set the Read Flag
				g_ui8CEStatus = CE_READ_FLAG;

				g_ui16StartIndex = (uint16_t) ((ui8P1Value << 8) + ui8P2Value);

				g_ui8TransferBytes = pui8RxPayload[LC_INDEX];

				// P1 and P2 hold the last address being requested
				ui16FileTemp = (uint16_t) ((ui8P1Value << 8) + ui8P2Value + pui8RxPayload[LC_INDEX]);

				// Check if data requested is inside the bounds of the file
				if(g_pType4DS.sType4AppArray[g_ui8SelectedAppIndex].pui8Type4FileArray[g_ui8SelectedFileIndex].ui16Type4FileLen >= ui16FileTemp)
				{
					// Subtract the Len of the file being requested
					ui16FileTemp -= pui8RxPayload[LC_INDEX];

					// if the file is inbounds
					g_ui16CommandResponse = SW1_SW2_APP_FOUND;
					g_ui8BytestoSend = pui8RxPayload[LC_INDEX];
					g_pui8FileData = g_pType4DS.sType4AppArray[g_ui8SelectedAppIndex].pui8Type4FileArray[g_ui8SelectedFileIndex].pui8Type4File + ui16FileTemp;
				}
				else
				{
					// Data being read is outside of the file's bound
					g_ui16CommandResponse = SW1_SW2_INCORRECT_P1_P2;
					g_ui8BytestoSend = 0x00;
				}
			}
			else
			{
				// A file Must be selected before allowing the device to read
				g_ui16CommandResponse = SW1_SW2_CMD_NOT_ALLOWED;
				g_ui8BytestoSend = 0x00;
			}


			break;
		case INS_UPDATE_BINARY:

			if(g_ui16SelectedFile != 0)
			{
				// Set the Write Flag
				g_ui8CEStatus = CE_WRITE_FLAG;

				g_ui16StartIndex = (uint16_t) ((ui8P1Value << 8) + ui8P2Value);

				g_ui8TransferBytes = pui8RxPayload[LC_INDEX];

				// Check if CC (0xE103) is being updated
				if(g_ui16SelectedFile == 0xE103 && pui8RxPayload[LC_INDEX] == 0x01)
				{
					// Check what file's property is being updated
					ui16FileTemp = (g_pType4DS.sType4AppArray[g_ui8SelectedAppIndex].pui8Type4FileArray[g_ui8SelectedFileIndex].pui8Type4File[ui8P2Value-5] << 8) +
							 (g_pType4DS.sType4AppArray[g_ui8SelectedAppIndex].pui8Type4FileArray[g_ui8SelectedFileIndex].pui8Type4File[ui8P2Value-4]);

					// Find the file being updated in the data structure
					for(ui8Index=0;ui8Index<g_pType4DS.sType4AppArray[g_ui8SelectedAppIndex].ui8Type4FileLen;ui8Index++)
					{
						// Check if the file ID of the data structure matches the ui16FileTemp
						if(ui16FileTemp == g_pType4DS.sType4AppArray[g_ui8SelectedAppIndex].pui8Type4FileArray[ui8Index].ui16Type4FileId)
						{
							// Check if the permission is write only - if it is then set to 0xFF
							if(pui8RxPayload[DATA_INDEX] == 0xFF)
							{
								g_pType4DS.sType4AppArray[g_ui8SelectedAppIndex].pui8Type4FileArray[ui8Index].bReadOnly = true;
							}
							break;
						}
					}
				}

				// ui16FileTemp holds the last address being written
				// P1 and P2 hold the last address being requested
				ui16FileTemp = (uint16_t) ((ui8P1Value << 8) + ui8P2Value + pui8RxPayload[LC_INDEX]);

				// Check the files is not read only and that the address being written is inside the bounds of the file
				if(g_pType4DS.sType4AppArray[g_ui8SelectedAppIndex].pui8Type4FileArray[g_ui8SelectedFileIndex].bReadOnly == false &&
						g_pType4DS.sType4AppArray[g_ui8SelectedAppIndex].pui8Type4FileArray[g_ui8SelectedFileIndex].ui16Type4FileLen >= ui16FileTemp)
				{
					// Subtract the Len of the file being requested
					ui16FileTemp -= pui8RxPayload[LC_INDEX];

					g_pui8FileData = g_pType4DS.sType4AppArray[g_ui8SelectedAppIndex].pui8Type4FileArray[g_ui8SelectedFileIndex].pui8Type4File + ui16FileTemp;

					for(ui8Index=0;ui8Index<pui8RxPayload[LC_INDEX];ui8Index++)
					{
						g_pui8FileData[ui8Index] = pui8RxPayload[DATA_INDEX+ui8Index];
					}

					g_ui16CommandResponse = SW1_SW2_APP_FOUND;
					g_ui8BytestoSend = 0x00;
				}
				else
				{
					// Data being written is outside of the file's bound
					g_ui16CommandResponse = SW1_SW2_INCORRECT_P1_P2;
					g_ui8BytestoSend = 0x00;
				}
			}
			else
			{
				// A file Must be selected before allowing the device to write it
				g_ui16CommandResponse = SW1_SW2_CMD_NOT_ALLOWED;
				g_ui8BytestoSend = 0x00;
			}


			break;
		default:
			g_ui16CommandResponse = SW1_Sw2_FUNCTION_NOT_SUPPORTED;
			g_ui8BytestoSend = 0x00;
			break;

		}
	}
	else
	{
		g_ui16CommandResponse = SW1_Sw2_FUNCTION_NOT_SUPPORTED;
		g_ui8BytestoSend = 0x00;
	}

	return ui8Status;

}

uint8_t ISO_7816_4_processReceivedData(uint8_t * pui8RxBuffer, uint16_t ui16Length)
{
	uint16_t ui16Sw1Sw2Temp;

	switch(g_eISO7816CommStatus)
	{
	case ISO_7816_4_CONNECTION_IDLE:
		// Do nothing

		break;
	case ISO_7816_4_CONNECTION_SELECT:

		// Do nothing

		break;
	case ISO_7816_4_CONNECTION_WAIT_SELECT_RSP:

		ui16Sw1Sw2Temp = (uint16_t) (pui8RxBuffer[ui16Length-2] << 8) | (pui8RxBuffer[ui16Length-1]);

		// Check if the the select command type was FILE ID and the response was successful
		if(g_e7816Select == ISO_7816_4_SELECT_FILE_ID && ui16Sw1Sw2Temp == SW1_SW2_APP_FOUND)
		{
			g_ui16SelectedFile = g_ui16FileReadID;
		}

		g_pui8ISO_7816_4_4RxData = pui8RxBuffer;

		g_ui16ISO_7816_4_BytesReceived = ui16Length;

		// If the response was successful
		if(ui16Sw1Sw2Temp == SW1_SW2_APP_FOUND)
		{
			g_eISO7816CommStatus = ISO_7816_4_CONNECTION_SELECT_SUCCESS;
		}
		else
		{
			g_eISO7816CommStatus = ISO_7816_4_CONNECTION_SELECT_FAIL;
		}

		break;
	case ISO_7816_4_CONNECTION_SELECT_FAIL:
		// Wait until the user clears the state by using the check Response AP
		break;
	case ISO_7816_4_CONNECTION_SELECT_SUCCESS:
		// Wait until the user clears the state by using the check Response AP
		break;
	case ISO_7816_4_CONNECTION_WRITE:

		// Do nothing

		break;
	case ISO_7816_4_CONNECTION_WAIT_WRITE_RSP:

		if(ui16Length == 0x02)
		{
			ui16Sw1Sw2Temp = (uint16_t) (pui8RxBuffer[0] << 8) | (pui8RxBuffer[1]);
		}
		else
		{
			g_ui16SelectedFile = SW1_SW2_APP_NOT_FOUND;
		}

		// If the response was successful
		if(ui16Sw1Sw2Temp == SW1_SW2_APP_FOUND)
		{
			g_eISO7816CommStatus = ISO_7816_4_CONNECTION_WRITE_SUCCESS;
		}
		else
		{
			g_eISO7816CommStatus = ISO_7816_4_CONNECTION_WRITE_FAIL;
		}
		break;
	case ISO_7816_4_CONNECTION_WRITE_SUCCESS:
		// Wait until the user clears the state by using the check Response AP
		break;
	case ISO_7816_4_CONNECTION_WRITE_FAIL:
		// Wait until the user clears the state by using the check Response AP
		break;
	case ISO_7816_4_CONNECTION_READ:

		// Do nothing

		break;
	case ISO_7816_4_CONNECTION_WAIT_READ_RSP:

		if(ui16Length > 0x02)
		{
			// Get the response SW 1 and SW 2 (the last two bytes of the response)
			ui16Sw1Sw2Temp = (uint16_t) (pui8RxBuffer[ui16Length-2] << 8) | (pui8RxBuffer[ui16Length-1]);
		}
		else
		{
			g_ui16SelectedFile = SW1_SW2_APP_NOT_FOUND;
		}

		// If the response was successful
		if(ui16Sw1Sw2Temp == SW1_SW2_APP_FOUND)
		{
			g_eISO7816CommStatus = ISO_7816_4_CONNECTION_READ_SUCCESS;

			g_pui8ISO_7816_4_4RxData = pui8RxBuffer;

			g_ui16ISO_7816_4_BytesReceived = ui16Length;
		}
		else
		{
			g_eISO7816CommStatus = ISO_7816_4_CONNECTION_READ_FAIL;

			g_ui16ISO_7816_4_BytesReceived = 0x00;
		}
	case ISO_7816_4_CONNECTION_READ_SUCCESS:
		// Wait until the user clears the state by using the check Response AP
		break;
	case ISO_7816_4_CONNECTION_READ_FAIL:
		// Wait until the user clears the state by using the check Response AP
		break;

	case ISO_7816_4_CONNECTION_RAW_WRITE:
		break;
	case ISO_7816_4_CONNECTION_WAIT_RAW_WRITE_RSP:

		if(ui16Length > 0x02)
		{
			ui16Sw1Sw2Temp = (uint16_t) (pui8RxBuffer[ui16Length-2] << 8) | (pui8RxBuffer[ui16Length-1]);
		}
		else
		{
			g_ui16SelectedFile = SW1_SW2_APP_NOT_FOUND;
		}

		g_pui8ISO_7816_4_4RxData = pui8RxBuffer;

		g_ui16ISO_7816_4_BytesReceived = ui16Length;

		// If the response was successful
		if(ui16Sw1Sw2Temp == SW1_SW2_APP_FOUND)
		{
			g_eISO7816CommStatus = ISO_7816_4_CONNECTION_WRITE_RAW_SUCCESS;
		}
		else
		{
			g_eISO7816CommStatus = ISO_7816_4_CONNECTION_WRITE_RAW_FAIL;
		}
		break;
	case ISO_7816_4_CONNECTION_WRITE_RAW_SUCCESS:
		break;
	case ISO_7816_4_CONNECTION_WRITE_RAW_FAIL:
		break;
    //! When send RAW APDU to a tag
	case ISO_7816_4_TAG_MODE:
		break;
	}
	return 0;
}

void ISO_7816_4_setCommMode(tISO7816ConnectionStatus eConnectionStatus)
{
	g_eISO7816CommStatus = eConnectionStatus;
}

tISO7816ConnectionStatus ISO_7816_4_getCommStatus(void)
{
	return g_eISO7816CommStatus;
}

tISO7816ConnectionStatus ISO_7816_4_getSelectStatus(void)
{
	tISO7816ConnectionStatus eStatusTemp;

	if(g_eISO7816CommStatus == ISO_7816_4_CONNECTION_SELECT_SUCCESS)
	{
		eStatusTemp = g_eISO7816CommStatus;
		// Clear Current State
		g_eISO7816CommStatus = ISO_7816_4_CONNECTION_IDLE;
	}
	else if(g_eISO7816CommStatus == ISO_7816_4_CONNECTION_SELECT_FAIL)
	{
		eStatusTemp = g_eISO7816CommStatus;
		// Clear Current State
		g_eISO7816CommStatus = ISO_7816_4_CONNECTION_IDLE;
	}

	return eStatusTemp;
}

tISO7816ConnectionStatus ISO_7816_4_getWriteStatus(void)
{
	tISO7816ConnectionStatus eStatusTemp;

	if(g_eISO7816CommStatus == ISO_7816_4_CONNECTION_WRITE_SUCCESS)
	{
		eStatusTemp = g_eISO7816CommStatus;
		// Clear Current State
		g_eISO7816CommStatus = ISO_7816_4_CONNECTION_IDLE;
	}
	else if(g_eISO7816CommStatus == ISO_7816_4_CONNECTION_WRITE_FAIL)
	{
		eStatusTemp = g_eISO7816CommStatus;
		// Clear Current State
		g_eISO7816CommStatus = ISO_7816_4_CONNECTION_IDLE;
	}

	return eStatusTemp;
}

tISO7816ConnectionStatus ISO_7816_4_getReadStatus(void)
{
	tISO7816ConnectionStatus eStatusTemp;

	if(g_eISO7816CommStatus == ISO_7816_4_CONNECTION_READ_SUCCESS)
	{
		eStatusTemp = g_eISO7816CommStatus;
		// Clear Current State
		g_eISO7816CommStatus = ISO_7816_4_CONNECTION_IDLE;
	}
	else if(g_eISO7816CommStatus == ISO_7816_4_CONNECTION_READ_FAIL)
	{
		eStatusTemp = g_eISO7816CommStatus;
		// Clear Current State
		g_eISO7816CommStatus = ISO_7816_4_CONNECTION_IDLE;
	}

	return eStatusTemp;
}

tISO7816ConnectionStatus ISO_7816_4_getWriteRawStatus(void)
{
	tISO7816ConnectionStatus eStatusTemp;

	if(g_eISO7816CommStatus == ISO_7816_4_CONNECTION_WRITE_RAW_SUCCESS)
	{
		eStatusTemp = g_eISO7816CommStatus;
		// Clear Current State
		g_eISO7816CommStatus = ISO_7816_4_CONNECTION_IDLE;
	}
	else if(g_eISO7816CommStatus == ISO_7816_4_CONNECTION_WRITE_RAW_FAIL)
	{
		eStatusTemp = g_eISO7816CommStatus;
		// Clear Current State
		g_eISO7816CommStatus = ISO_7816_4_CONNECTION_IDLE;
	}

	return eStatusTemp;
}

uint8_t ISO_7816_4_sendSelectApplication(const uint8_t * pui8Buffer, uint8_t ui8Length)
{
	uint8_t ui8Status = STATUS_SUCCESS;
	uint8_t ui8Offset;

	if(g_eISO7816CommStatus == ISO_7816_4_CONNECTION_IDLE && ui8Length < 17)
	{
		g_eISO7816CommStatus = ISO_7816_4_CONNECTION_SELECT;

		g_ui8AppLen = ui8Length;

		g_e7816Select = ISO_7816_4_SELECT_APP_ID;

		// Copy UID
		for(ui8Offset=0;ui8Offset<g_ui8AppLen;ui8Offset++)
		{
			g_ui8AppId[ui8Offset] = pui8Buffer[ui8Offset];
		}
	}
	else
	{
		ui8Status = STATUS_FAIL;
	}

	return ui8Status;
}

uint8_t ISO_7816_4_sendSelectFile(const uint8_t * pui8Buffer, uint8_t ui8Length)
{
	uint8_t ui8Status = STATUS_SUCCESS;

	if(g_eISO7816CommStatus == ISO_7816_4_CONNECTION_IDLE && ui8Length == 0x02)
	{
		g_eISO7816CommStatus = ISO_7816_4_CONNECTION_SELECT;

		g_e7816Select = ISO_7816_4_SELECT_FILE_ID;

		g_ui16FileReadID =  (uint16_t) (pui8Buffer[1] << 8) + (uint16_t) pui8Buffer[0];
	}
	else
	{
		ui8Status = STATUS_FAIL;
	}

	return ui8Status;
}

uint8_t ISO_7816_4_sendReadFile(uint16_t ui16StartIndex, uint8_t ui8BytestoRead)
{
	uint8_t ui8Status = STATUS_SUCCESS;

	if(g_eISO7816CommStatus == ISO_7816_4_CONNECTION_IDLE)
	{
		g_eISO7816CommStatus = ISO_7816_4_CONNECTION_READ;

		g_ui16StartDataIndex = ui16StartIndex;

		g_ui8BytesRequested = ui8BytestoRead;
	}
	else
	{
		ui8Status = STATUS_FAIL;
	}

	return ui8Status;
}

uint8_t ISO_7816_4_sendWriteFile(uint16_t ui16StartIndex, uint8_t ui8BytestoWrite, uint8_t * pui8Data)
{
	uint8_t ui8Status = STATUS_SUCCESS;

	if(g_eISO7816CommStatus == ISO_7816_4_CONNECTION_IDLE)
	{
		g_eISO7816CommStatus = ISO_7816_4_CONNECTION_WRITE;

		g_pui8DataBuffer = pui8Data;

		g_ui16StartDataIndex = ui16StartIndex;

		g_ui8BytesRequested = ui8BytestoWrite;

		g_ui8ISO_7816_4_4DataIndex = 0;
	}
	else
	{
		ui8Status = STATUS_FAIL;
	}

	return ui8Status;
}

uint8_t ISO_7816_4_sendAPDU(uint8_t * pui8Data, uint8_t ui8Length)
{
	uint8_t ui8Status = STATUS_SUCCESS;

	if(g_eISO7816CommStatus == ISO_7816_4_CONNECTION_IDLE)
	{
		g_eISO7816CommStatus = ISO_7816_4_CONNECTION_RAW_WRITE;

		g_pui8DataBuffer = pui8Data;

		g_ui8BytesRequested = ui8Length;
	}
	else
	{
		ui8Status = STATUS_FAIL;
	}

	return ui8Status;
}

tISO7816ConnectionStatus ISO_7816_4_getWriteAPDUStatus(void)
{
	tISO7816ConnectionStatus eStatusTemp;

	if(g_eISO7816CommStatus == ISO_7816_4_CONNECTION_WRITE_RAW_SUCCESS)
	{
		eStatusTemp = g_eISO7816CommStatus;
		// Clear Current State
		g_eISO7816CommStatus = ISO_7816_4_CONNECTION_IDLE;
	}
	else if(g_eISO7816CommStatus == ISO_7816_4_CONNECTION_WRITE_RAW_FAIL)
	{
		eStatusTemp = g_eISO7816CommStatus;
		// Clear Current State
		g_eISO7816CommStatus = ISO_7816_4_CONNECTION_IDLE;
	}

	return eStatusTemp;
}

void ISO_7816_4_getPacketStatus(uint8_t ** pui8RXData, uint16_t * pui16Length)
{
	*pui8RXData = g_pui8ISO_7816_4_4RxData;
	*pui16Length = g_ui16ISO_7816_4_BytesReceived;
}

uint8_t ISO_7816_4_getCEStatus(uint16_t * pui16FileId, uint16_t * pui16StartIdx, uint8_t * pui8BufferLength)
{
	uint8_t ui8TempStatus;

	ui8TempStatus = g_ui8CEStatus;

	if(ui8TempStatus != 0x00)
	{
		*pui16FileId = g_ui16SelectedFile;

		*pui16StartIdx = g_ui16StartIndex;

		*pui8BufferLength = g_ui8TransferBytes;
	}

	g_ui8CEStatus = 0;

	return ui8TempStatus;
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
