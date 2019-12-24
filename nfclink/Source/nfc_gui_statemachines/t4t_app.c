//*****************************************************************************
//
// t4t_app.c - Reader/Writer Type 4 Tag State Machine
//
// Copyright (c) 2015 Texas Instruments Incorporated.  All rights reserved.
// TI Information - Selective Disclosure
//
//*****************************************************************************

#include "tag_header.h"

//*****************************************************************************

const uint8_t g_pui8DTAId[14] = {0x32, 0x4E, 0x46, 0x43, 0x2E, 0x53, 0x59, 0x53, 0x2E, 0x44, 0x44, 0x46, 0x30, 0x31};

const uint8_t g_pui8NDEFId[7] = {0xD2, 0x76, 0x00, 0x00, 0x85, 0x01, 0x01};

const uint8_t g_pui8CCFileId[2] = {0xE1, 0x03};

uint8_t g_pui8NDEFFileId[2];

uint16_t g_ui16NdefLength;
uint16_t g_ui16NdefRemainingBytes;
uint16_t g_ui16T4TNdefIndex;

bool g_bNdefReadOnly;
bool g_bNdefWriteComplete;

uint8_t g_ui8NdefMaxReadBytes;
uint8_t g_ui8NdefMaxWriteBytes;

uint8_t * g_pui8T4TBuffer;
uint8_t g_ui8TxLength;

uint16_t g_ui16T4TMaxNdefLen;

tNDEFStateMachine  g_eT4TState;

bool g_bT4WaitForRsp;

uint16_t g_ui16T4TSize;

void T4T_init(uint8_t * pui8Ndef, uint16_t ui16NdefMaxSize)
{
	// Initialize Global Variables
	g_ui16NdefLength = 0;
	g_ui16NdefRemainingBytes = 0;
	g_ui16T4TNdefIndex = 0;

	g_bNdefReadOnly = false;
	g_bNdefWriteComplete = false;

	g_ui8NdefMaxReadBytes = 0;
	g_ui8NdefMaxWriteBytes = 0;

	g_pui8T4TBuffer = pui8Ndef;
	g_ui16T4TMaxNdefLen = ui16NdefMaxSize;

	g_bT4WaitForRsp = false;

	g_ui8TxLength = 0;

	g_eT4TState = T4T_APP_SEL;

	g_ui16T4TSize = 0;
}

void T4T_writeNDEF(uint8_t * pui8Data, uint16_t ui16NdefLen)
{
	if(g_eT4TState == T4T_SELECTED_IDLE)
	{
		if(g_ui16T4TSize >= ui16NdefLen)
		{
			g_eT4TState = T4T_WRITE_NDEF;

			g_ui16NdefRemainingBytes = ui16NdefLen+2;

			g_ui16T4TNdefIndex = 0x00;

			g_pui8T4TBuffer[0] = (uint8_t) ((ui16NdefLen >> 8) & 0xFF);
			g_pui8T4TBuffer[1] = (uint8_t) (ui16NdefLen & 0xFF);

			memcpy(&g_pui8T4TBuffer[2],pui8Data,ui16NdefLen);
		}
		else
		{
			Serial_printf("T4T Write Fail: NDEF message size not supported by tag.\n",RW_STATUS_DATA);

			// Print New Line
			Serial_printf("\n",RW_STATUS_DATA);

		}
	}
	else
	{
		Serial_printf("T4T Write Fail: Busy.",RW_STATUS_DATA);

		// Print New Line
		Serial_printf("\n",RW_STATUS_DATA);
	}
}

void T4T_stateMachine(void)
{
	tISO7816ConnectionStatus sISO7816Status;
	uint16_t ui16RxLength;
	uint8_t * pui8RxData;

	// Waiting to get a response for one of the states
	if(g_bT4WaitForRsp)
	{
		switch(g_eT4TState)
		{
		case T4T_APP_SEL:
			sISO7816Status = ISO_7816_4_getSelectStatus();

			if(sISO7816Status == ISO_7816_4_CONNECTION_SELECT_SUCCESS)
			{
				g_bT4WaitForRsp = false;

				// Go Select the Capability Container
				g_eT4TState = T4T_SEL_CC;
			}
			else if(sISO7816Status == ISO_7816_4_CONNECTION_SELECT_FAIL)
			{
				g_bT4WaitForRsp = false;
				g_eT4TState = T4T_SELECTED_IDLE;

				Serial_printfLine("No NDEF Support");
			}
			break;
		case T4T_SEL_CC:
			sISO7816Status = ISO_7816_4_getSelectStatus();

			if(sISO7816Status == ISO_7816_4_CONNECTION_SELECT_SUCCESS)
			{
				g_bT4WaitForRsp = false;

				// Go Read the Capability Container
				g_eT4TState = T4T_READ_CC;
			}
			else if(sISO7816Status == ISO_7816_4_CONNECTION_SELECT_FAIL)
			{
				g_bT4WaitForRsp = false;
				g_eT4TState = T4T_SELECTED_IDLE;
			}
			break;
		case T4T_READ_CC:
			sISO7816Status = ISO_7816_4_getReadStatus();

			if(sISO7816Status == ISO_7816_4_CONNECTION_READ_SUCCESS)
			{
				ISO_7816_4_getPacketStatus(&pui8RxData,&ui16RxLength);

				// MLe
				if (pui8RxData[3] != 0x00)
				{
					g_ui8NdefMaxReadBytes = 0xFF;
				}
				else
				{
					g_ui8NdefMaxReadBytes = pui8RxData[4];
				}
				// MLc
				if (pui8RxData[5] != 0x00)
				{
					g_ui8NdefMaxWriteBytes = 0xFF;
				}
				else
				{
					g_ui8NdefMaxWriteBytes = pui8RxData[6];
				}

				// NDEF File ID
				g_pui8NDEFFileId[0] = pui8RxData[9];
				g_pui8NDEFFileId[1] = pui8RxData[10];

				// Check NDEF Read-only flag
				if(pui8RxData[14] > 0x00)
				{
					g_bNdefReadOnly = true;
				}
				else
				{
					g_bNdefReadOnly = false;
				}

				g_bT4WaitForRsp = false;

				// Check if version number is 3.0
				if(pui8RxData[2] == 0x30)
				{
					g_eT4TState = T4T_SELECTED_IDLE;
				}
				// Check if version number is 2.2
				else if(pui8RxData[2] == 0x20 || pui8RxData[2] == 0x22)
				{
					// Go Select the NDEF File
					g_eT4TState = T4T_SEL_NDEF_FILE;

					g_ui16T4TSize = ((uint16_t) pui8RxData[11] << 8) + pui8RxData[12];
				}
				else
				{
					g_eT4TState = T4T_SELECTED_IDLE;
				}
			}
			else if(sISO7816Status == ISO_7816_4_CONNECTION_READ_FAIL)
			{
				g_bT4WaitForRsp = false;
				g_eT4TState = T4T_SELECTED_IDLE;
			}
			break;
		case T4T_SEL_NDEF_FILE:
			sISO7816Status = ISO_7816_4_getSelectStatus();

			if(sISO7816Status == ISO_7816_4_CONNECTION_SELECT_SUCCESS)
			{
				g_bT4WaitForRsp = false;

				// Go Read the NDEF File length
				g_eT4TState = T4T_READ_NDEF_LEN;
			}
			else if(sISO7816Status == ISO_7816_4_CONNECTION_SELECT_FAIL)
			{
				g_bT4WaitForRsp = false;
				g_eT4TState = T4T_SELECTED_IDLE;
			}
			break;
		case T4T_READ_NDEF_LEN:
			sISO7816Status = ISO_7816_4_getReadStatus();

			if(sISO7816Status == ISO_7816_4_CONNECTION_READ_SUCCESS)
			{
				ISO_7816_4_getPacketStatus(&pui8RxData,&ui16RxLength);

				g_ui16NdefLength = (uint16_t) (pui8RxData[0] << 8) + (uint16_t) pui8RxData[1];

				g_ui16NdefRemainingBytes = g_ui16NdefLength;

				// Reset Index
				g_ui16T4TNdefIndex = 0x00;

				g_bT4WaitForRsp = false;

				// Depending on the test

				// Write Tests

				// Read Tests

				if(g_ui16NdefRemainingBytes > 0)
				{
					// Go Read the NDEF message
					g_eT4TState = T4T_READ_NDEF;
				}
				else
				{
					// When no remaining bytes we are done
					g_eT4TState = T4T_SELECTED_IDLE;
				}

			}
			else if(sISO7816Status == ISO_7816_4_CONNECTION_READ_FAIL)
			{
				g_bT4WaitForRsp = false;
				g_eT4TState = T4T_SELECTED_IDLE;
			}
			break;
		case T4T_READ_NDEF:
			sISO7816Status = ISO_7816_4_getReadStatus();

			if(sISO7816Status == ISO_7816_4_CONNECTION_READ_SUCCESS)
			{
				if (g_ui16T4TMaxNdefLen > g_ui16NdefLength)
				{

					ISO_7816_4_getPacketStatus(&pui8RxData,&ui16RxLength);

					ui16RxLength =  ui16RxLength - 2;

					if(g_ui16T4TMaxNdefLen > (g_ui16T4TNdefIndex+ui16RxLength))
					{
						// Store the NDEF in a dummy buffer
						memcpy(&g_pui8T4TBuffer[g_ui16T4TNdefIndex],pui8RxData,ui16RxLength);
					}

					// Update remaining bytes
					g_ui16NdefRemainingBytes = g_ui16NdefRemainingBytes - ui16RxLength;

					// Increase Store index
					g_ui16T4TNdefIndex = g_ui16T4TNdefIndex + ui16RxLength;

					g_bT4WaitForRsp = false;

					// When completed go to the idle state
					if(g_ui16NdefRemainingBytes == 0)
					{
						if(g_ui16T4TMaxNdefLen > g_ui16T4TNdefIndex)
						{
							Serial_printBuffer((char *)g_pui8T4TBuffer,g_ui16T4TNdefIndex,RW_PAYLOAD_DATA);
						}

						g_eT4TState = T4T_SELECTED_IDLE;
					}
				}
				else
				{
					ISO_7816_4_getPacketStatus(&pui8RxData,&ui16RxLength);

					ui16RxLength =  ui16RxLength - 2;

					// Store the NDEF in a dummy buffer
					memcpy(&g_pui8T4TBuffer[0],pui8RxData,ui16RxLength);

					// Update remaining bytes
					g_ui16NdefRemainingBytes = g_ui16NdefRemainingBytes - ui16RxLength;

					// Increase Store index
					g_ui16T4TNdefIndex = g_ui16T4TNdefIndex + ui16RxLength;

					// Dump the current NDEF Contents to Serial buffer
					Serial_printBuffer((char *)g_pui8T4TBuffer,ui16RxLength,RW_PAYLOAD_DATA);

					g_bT4WaitForRsp = false;

					// When completed go to the idle state
					if(g_ui16NdefRemainingBytes == 0)
					{
						g_eT4TState = T4T_SELECTED_IDLE;
					}
				}
			}
			else if(sISO7816Status == ISO_7816_4_CONNECTION_READ_FAIL)
			{
				g_bT4WaitForRsp = false;
				g_eT4TState = T4T_SELECTED_IDLE;
			}
			break;
		case T4T_SELECTED_IDLE:
			break;
		case T4T_WRITE_NDEF:
			sISO7816Status = ISO_7816_4_getWriteStatus();

			if(sISO7816Status == ISO_7816_4_CONNECTION_WRITE_SUCCESS)
			{
				if(g_ui16NdefRemainingBytes == 0x00)
				{
					g_eT4TState = T4T_SELECTED_IDLE;

					Serial_printf("Write Successful!",RW_STATUS_DATA);

					// Print New Line
					Serial_printf("\n",RW_STATUS_DATA);
				}
				// Done with the test
				g_bT4WaitForRsp = false;
			}
			else if(sISO7816Status == ISO_7816_4_CONNECTION_WRITE_FAIL)
			{
				g_bT4WaitForRsp = false;
				g_eT4TState = T4T_SELECTED_IDLE;
			}
			break;
		default:
			break;
		}
	}

	// Sending a command
	if(g_bT4WaitForRsp == false)
	{
		switch(g_eT4TState)
		{
		case T4T_APP_SEL:
			if(ISO_7816_4_sendSelectApplication(g_pui8NDEFId,7) == STATUS_SUCCESS)
			{
				g_bT4WaitForRsp = true;
			}
			break;
		case T4T_SEL_CC:
			if(ISO_7816_4_sendSelectFile(g_pui8CCFileId,2) == STATUS_SUCCESS)
			{
				g_bT4WaitForRsp = true;
			}
			break;
		case T4T_READ_CC:
			if(ISO_7816_4_sendReadFile(0,0x0F) == STATUS_SUCCESS)
			{
				g_bT4WaitForRsp = true;
			}
			break;
		case T4T_SEL_NDEF_FILE:
			if(ISO_7816_4_sendSelectFile(g_pui8NDEFFileId,2) == STATUS_SUCCESS)
			{
				g_bT4WaitForRsp = true;
			}
			break;
		case T4T_READ_NDEF_LEN:
			if(ISO_7816_4_sendReadFile(0,0x02) == STATUS_SUCCESS)
			{
				g_bT4WaitForRsp = true;
			}
			break;
		case T4T_READ_NDEF:
			if(g_ui16NdefRemainingBytes > g_ui8NdefMaxReadBytes)
			{
				if(ISO_7816_4_sendReadFile(g_ui16T4TNdefIndex+2,g_ui8NdefMaxReadBytes) == STATUS_SUCCESS)
				{
					g_bT4WaitForRsp = true;
				}
			}
			else
			{
				if(ISO_7816_4_sendReadFile(g_ui16T4TNdefIndex+2,(uint8_t) g_ui16NdefRemainingBytes) == STATUS_SUCCESS)
				{
					g_bT4WaitForRsp = true;
				}
			}
			break;
		case T4T_SELECTED_IDLE:
			break;
		case T4T_WRITE_NDEF:
			// If the number of bytes remaining to be written is less than the max number of bytes
			if(g_ui16NdefRemainingBytes > 0)
			{
				if(g_ui16NdefRemainingBytes > (uint16_t) g_ui8NdefMaxWriteBytes)
				{

					// Clear the NDEF Len for function purpose
					if(ISO_7816_4_sendWriteFile(g_ui16T4TNdefIndex, g_ui8NdefMaxWriteBytes,&g_pui8T4TBuffer[g_ui16T4TNdefIndex]) == STATUS_SUCCESS)
					{
						g_bT4WaitForRsp = true;
						g_ui16NdefRemainingBytes = g_ui16NdefRemainingBytes - (uint16_t) g_ui8NdefMaxWriteBytes;
						g_ui16T4TNdefIndex = g_ui16T4TNdefIndex + (uint16_t) g_ui8NdefMaxWriteBytes;
					}
				}
				else
				{
					// Clear the NDEF Len for function purpose
					if(ISO_7816_4_sendWriteFile(g_ui16T4TNdefIndex,(uint8_t) g_ui16NdefRemainingBytes,&g_pui8T4TBuffer[g_ui16T4TNdefIndex]) == STATUS_SUCCESS)
					{
						g_bT4WaitForRsp = true;
						g_ui16NdefRemainingBytes = 0;
					}
				}

			}
			else
			{
				g_eT4TState = T4T_SELECTED_IDLE;
			}


			break;
		default:
			break;
		}
	}

}
