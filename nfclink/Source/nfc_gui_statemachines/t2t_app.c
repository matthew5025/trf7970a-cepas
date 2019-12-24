//*****************************************************************************
//
// t2t_app.c - Reader/Writer Type 2 Tag State Machine
//
// Copyright (c) 2015 Texas Instruments Incorporated.  All rights reserved.
// TI Information - Selective Disclosure
//
//*****************************************************************************

#include "tag_header.h"

//*****************************************************************************

uint16_t g_ui16T2TTLVRemaining;

uint8_t g_ui8T2TCurrentTlv;

tT2TStateMachine g_eT2TState;

uint8_t g_pui8T2TRxBuffer[16];

bool g_bT2TWaitForRsp;

uint8_t g_ui8T2TBlockNumber;

uint16_t g_ui16T2TNDEFBlockRemaining;

uint8_t * g_pui8T2TBuffer;

uint16_t g_ui16T2TNdefLen;

uint16_t g_ui16T2TMaxNdefLen;

uint16_t g_ui16T2TNdefIndex;

uint16_t g_ui16T2TSize;

bool g_bT2TTLVSelected;

bool g_bT2TTLVLengthKnown;

uint8_t g_ui8T2TTLVLengthRemainBytes;

uint16_t g_ui16T2TReadIndex;


void T2T_init(uint8_t * pui8Ndef, uint16_t ui16NdefMaxSize)
{
	g_eT2TState = T2T_READ_CC;
	g_ui8T2TCurrentTlv = 0;
	g_ui16T2TTLVRemaining = 0;
	g_bT2TWaitForRsp = false;

	g_pui8T2TBuffer = pui8Ndef;
	g_ui16T2TMaxNdefLen = ui16NdefMaxSize;

	g_ui8T2TBlockNumber = 0;
	g_ui16T2TNDEFBlockRemaining = 0;

	g_ui16T2TNdefIndex = 0;
	g_ui16T2TSize = 0;

	g_bT2TTLVSelected = false;
	g_bT2TTLVLengthKnown = false;
	g_ui8T2TTLVLengthRemainBytes = 0;
	g_ui16T2TReadIndex = 0;
}

void T2T_writeNDEF(uint8_t * pui8Data, uint16_t ui16NdefLen)
{
	uint8_t ui8Offset;
	ui8Offset = 0;

	if(g_eT2TState == T2T_SELECTED_IDLE )
	{
		if(g_ui16T2TSize > 8 && (g_ui16T2TSize-8) >= ui16NdefLen)
		{
			g_eT2TState = T2T_WRITE_NDEF;
			g_ui16T2TNdefIndex = 0;

			// Start Writing at Block 4
			g_ui8T2TBlockNumber = 0x04;

			// Lock Control TLV - Type
			g_pui8T2TBuffer[ui8Offset++] = TLV_LOCK_CONTROL;
			// Lock Control TLV - Length
			g_pui8T2TBuffer[ui8Offset++] = 0x03;
			// Lock Control TLV - Values
			g_pui8T2TBuffer[ui8Offset++] = 0xA0;
			g_pui8T2TBuffer[ui8Offset++] = 0x10;
			g_pui8T2TBuffer[ui8Offset++] = 0x44;
			// NDEF Msg TLV - Type
			g_pui8T2TBuffer[ui8Offset++] = TLV_NDEF_MSG;
			// NDEF Msg TLV - Length
			if(ui16NdefLen < 0xFF)
			{
				g_pui8T2TBuffer[ui8Offset++] = (uint8_t) ui16NdefLen;
			}

			g_ui16T2TTLVRemaining = (uint16_t) ui8Offset+ui16NdefLen+1;

			memcpy(&g_pui8T2TBuffer[ui8Offset],pui8Data,ui16NdefLen);

			g_pui8T2TBuffer[ui8Offset+ui16NdefLen] = TLV_TERMINATOR;
		}
		else
		{
			Serial_printf("T2T Write Fail: NDEF message size not supported by tag.",RW_STATUS_DATA);

			// Print New Line
			Serial_printf("\n",RW_STATUS_DATA);
		}

	}
	else
	{
		Serial_printf("T2T Write Fail: Busy.",RW_STATUS_DATA);

		// Print New Line
		Serial_printf("\n",RW_STATUS_DATA);
	}
}

void T2T_stateMachine(void)
{
	uint8_t ui8RxLength;
	uint8_t * pui8RxData;
	uint8_t ui8TempLength;
	char pui8LenBuffer[3];

	tNfcRwT2TConnectionStatus eT2TStatus;

	volatile uint8_t ui8LocalReadIndex;

	if(g_bT2TWaitForRsp)
	{
		switch(g_eT2TState)
		{
		case T2T_READ_CC:
			eT2TStatus = NFC_RW_T2T_getReadStatus();

			if(eT2TStatus == NFC_RW_T2T_CONNECTION_READ_SUCCESS)
			{
				NFC_RW_T2T_getPacketStatus(&pui8RxData,&ui8RxLength);

				// Store the NDEF in a dummy buffer
				memcpy(g_pui8T2TRxBuffer,pui8RxData,ui8RxLength);

				// Check if NDEF compliant
				if(pui8RxData[0] == 0xE1)
				{
					// Switch to the read NDEF data
					g_eT2TState = T2T_READ_NDEF;

					g_ui16T2TNDEFBlockRemaining = ((uint16_t) pui8RxData[2]) << 1;

					// Size is the 2nd byte * 8
					g_ui16T2TSize = pui8RxData[2] << 3;
				}
				else
				{
					// Switch to the read tag data
					g_eT2TState = T2T_READ_DATA;

					Serial_printf("T2T Not NDEF Formatted",RW_STATUS_DATA);

					// Print New Line
					Serial_printf("\n",RW_STATUS_DATA);

					Serial_printf("Block ",RW_STATUS_DATA);

					// Print Block Number
					convertByteToAscii(g_ui8T2TBlockNumber,(uint8_t *) pui8LenBuffer);

					Serial_printf(pui8LenBuffer,RW_STATUS_DATA);

					Serial_printf(" ",RW_STATUS_DATA);

					for(ui8TempLength = 0; ui8TempLength < 4; ui8TempLength++)
					{
						convertByteToAscii(pui8RxData[ui8TempLength],(uint8_t *) pui8LenBuffer);

						Serial_printf("0x",RW_STATUS_DATA);

						Serial_printf(pui8LenBuffer,RW_STATUS_DATA);

						Serial_printf(" ",RW_STATUS_DATA);
					}

					// Print New Line
					Serial_printf("\n",RW_STATUS_DATA);

					// Fixed amount
					g_ui16T2TNDEFBlockRemaining = 12;

					g_ui16T2TSize = 0;
				}

				g_bT2TWaitForRsp = false;

				// Incrment Block Number to # 4
				g_ui8T2TBlockNumber++;

			}
			else if(eT2TStatus == NFC_RW_T2T_CONNECTION_READ_FAIL)
			{
				g_bT2TWaitForRsp = false;
				g_eT2TState = T2T_SELECTED_IDLE;
			}
			break;
		case T2T_READ_DATA:
			eT2TStatus = NFC_RW_T2T_getReadStatus();

			if(eT2TStatus == NFC_RW_T2T_CONNECTION_READ_SUCCESS)
			{
				Serial_printf("Block ",RW_STATUS_DATA);

				// Print Block Number
				convertByteToAscii(g_ui8T2TBlockNumber,(uint8_t *) pui8LenBuffer);

				Serial_printf(pui8LenBuffer,RW_STATUS_DATA);

				Serial_printf(" ",RW_STATUS_DATA);

				NFC_RW_T2T_getPacketStatus(&pui8RxData,&ui8RxLength);

				// Store the NDEF in a dummy buffer
				memcpy(g_pui8T2TRxBuffer,pui8RxData,ui8RxLength);

				for(ui8TempLength = 0; ui8TempLength < 4; ui8TempLength++)
				{
					convertByteToAscii(pui8RxData[ui8TempLength], (uint8_t *) pui8LenBuffer);

					Serial_printf("0x",RW_STATUS_DATA);

					Serial_printf(pui8LenBuffer,RW_STATUS_DATA);

					Serial_printf(" ",RW_STATUS_DATA);
				}

				// Print New Line
				Serial_printf("\n",RW_STATUS_DATA);

				g_bT2TWaitForRsp = false;

				// Incrment Block Number
				g_ui8T2TBlockNumber++;

				g_ui16T2TNDEFBlockRemaining--;

				if(g_ui16T2TNDEFBlockRemaining == 0x00)
				{
					g_eT2TState = T2T_SELECTED_IDLE;
				}
			}
			else if(eT2TStatus == NFC_RW_T2T_CONNECTION_READ_FAIL)
			{
				g_bT2TWaitForRsp = false;
				g_eT2TState = T2T_SELECTED_IDLE;
			}
			break;
		case T2T_READ_NDEF:
			eT2TStatus = NFC_RW_T2T_getReadStatus();

			if(eT2TStatus == NFC_RW_T2T_CONNECTION_READ_SUCCESS)
			{
				NFC_RW_T2T_getPacketStatus(&pui8RxData,&ui8RxLength);

				// Store the NDEF in a dummy buffer
				memcpy(g_pui8T2TRxBuffer,pui8RxData,ui8RxLength);

				ui8LocalReadIndex = 0x00;

				// Processes 16 bytes returned from read block #
				while(ui8LocalReadIndex < 16 && g_eT2TState == T2T_READ_NDEF)
				{

					// TLV handling

					//  No TLV is selected
					if(g_bT2TTLVSelected == false)
					{
						// Read TLV
						g_ui8T2TCurrentTlv =  g_pui8T2TRxBuffer[ui8LocalReadIndex++];

						// NULL TLV just increment the read index
						if(g_ui8T2TCurrentTlv == TLV_NULL)
						{
							// Do nothing
						}
						// TERMINATOR TLV we are done reading
						else if(g_ui8T2TCurrentTlv == TLV_TERMINATOR)
						{
							g_eT2TState = T2T_SELECTED_IDLE;
						}
						else
						{
							// Go read length
							g_bT2TTLVSelected = true;

							g_bT2TTLVLengthKnown = false;
							// Default
							g_ui8T2TTLVLengthRemainBytes = 0xFF;
						}

					}
					// TLV is selected and Length not known
					else if(g_bT2TTLVLengthKnown == false)
					{
						// No length bytes have been read - (0xFF is default value)
						if(g_ui8T2TTLVLengthRemainBytes == 0xFF)
						{
							// Large NDEF - NDEF length is stored in two subsequent bytes
							if(g_pui8T2TRxBuffer[ui8LocalReadIndex] == 0xFF)
							{
								g_ui8T2TTLVLengthRemainBytes = 2;
							}
							else
							{
								g_bT2TTLVLengthKnown = true;
								g_ui16T2TNdefLen = g_pui8T2TRxBuffer[ui8LocalReadIndex];
								g_ui16T2TTLVRemaining = g_ui16T2TNdefLen;
								g_ui16T2TReadIndex = 0;
							}
						}
						// Read 1st length byte
						else if(g_ui8T2TTLVLengthRemainBytes == 2)
						{
							g_ui16T2TNdefLen = (uint16_t) (g_pui8T2TRxBuffer[ui8LocalReadIndex] << 8);
							g_ui8T2TTLVLengthRemainBytes = 1;
						}
						else if(g_ui8T2TTLVLengthRemainBytes == 1)
						{
							g_bT2TTLVLengthKnown = true;
							g_ui16T2TNdefLen |= ((uint16_t) g_pui8T2TRxBuffer[ui8LocalReadIndex] & 0xFF);
							g_ui16T2TTLVRemaining = g_ui16T2TNdefLen;
							g_ui8T2TTLVLengthRemainBytes = 0;
							g_ui16T2TReadIndex = 0;
						}
						else
						{
							// Do nothing
						}
						// Increment read index
						ui8LocalReadIndex++;
					}
					// Parse the selected TLV data
					else
					{
						if(g_ui8T2TCurrentTlv == TLV_NDEF_MSG)
						{
							// If the current TLV remaining bytes are more than the available bytes
							if(g_ui16T2TTLVRemaining > (16 - ui8LocalReadIndex))
							{
//								Serial_printBuffer((char *)&g_pui8T2TRxBuffer[ui8ReadIndex],(15 - ui8ReadIndex),RW_PAYLOAD_DATA);

								// Store Incoming NDEF Message into g_pui8T2TBuffer
								if(g_ui16T2TMaxNdefLen > (g_ui16T2TReadIndex + (16 - ui8LocalReadIndex)))
								{
									memcpy(&g_pui8T2TBuffer[g_ui16T2TReadIndex],&g_pui8T2TRxBuffer[ui8LocalReadIndex],(uint16_t) 16 - ui8LocalReadIndex);
								}
								else
								{
									// NDEF message is larger than the size of the g_pui8T2TBuffer buffer
								}

								g_ui16T2TReadIndex += 16 - ui8LocalReadIndex;


							}
							else
							{
								// Store Incoming NDEF Message into g_pui8T2TBuffer
								if(g_ui16T2TMaxNdefLen > (g_ui16T2TReadIndex + g_ui16T2TTLVRemaining))
								{
									memcpy(&g_pui8T2TBuffer[g_ui16T2TReadIndex],&g_pui8T2TRxBuffer[ui8LocalReadIndex],g_ui16T2TTLVRemaining);

									Serial_printBuffer((char *)&g_pui8T2TBuffer[0],(uint8_t) (g_ui16T2TReadIndex+g_ui16T2TTLVRemaining),RW_PAYLOAD_DATA);
								}
								else
								{
									// NDEF message is larger than the size of the g_pui8T2TBuffer buffer
								}

								g_ui16T2TReadIndex += g_ui16T2TTLVRemaining;
							}
						}

						// Incrementing Index

						// If the current TLV remaining bytes are more than the available bytes
						if(g_ui16T2TTLVRemaining > (16 - ui8LocalReadIndex))
						{
							// Update Remaining
							g_ui16T2TTLVRemaining -= (uint16_t) ( 16 - ui8LocalReadIndex);
							// Increase Index to 16 to continue reading next block
							ui8LocalReadIndex = 16;
						}
						else
						{
							// Increase read index by remaining number of bytes
							ui8LocalReadIndex += (uint8_t) g_ui16T2TTLVRemaining;
							g_ui16T2TTLVRemaining = 0;
							// Finished reading TLV
							g_bT2TTLVSelected = false;
						}
					}
				}

				g_bT2TWaitForRsp = false;

				// To ensure that Block number doesn't overflow
				if(g_ui8T2TBlockNumber == 0xFC)
				{
					g_eT2TState = T2T_SELECTED_IDLE;
				}

				if(g_eT2TState != T2T_SELECTED_IDLE)
				{
					// Incrment Block Number
					g_ui8T2TBlockNumber = g_ui8T2TBlockNumber + 4;

					g_ui16T2TNDEFBlockRemaining = g_ui16T2TNDEFBlockRemaining - 4;

					// EOF Not Found
					if(g_ui16T2TNDEFBlockRemaining == 0)
					{
						Serial_printf("Error: T2T Terminator TLV not found.",RW_STATUS_DATA);

						// Print New Line
						Serial_printf("\n",RW_STATUS_DATA);

						g_eT2TState = T2T_SELECTED_IDLE;
					}
				}
			}
			else if(eT2TStatus == NFC_RW_T2T_CONNECTION_READ_FAIL)
			{
				g_bT2TWaitForRsp = false;
				g_eT2TState = T2T_SELECTED_IDLE;
			}
			break;
		case T2T_WRITE_NDEF:
			eT2TStatus = NFC_RW_T2T_getWriteStatus();

			if(eT2TStatus == NFC_RW_T2T_CONNECTION_WRITE_SUCCESS)
			{
				g_ui8T2TBlockNumber++;
				g_ui16T2TNdefIndex = g_ui16T2TNdefIndex + 4;
				if(g_ui16T2TTLVRemaining > 4)
				{
					g_ui16T2TTLVRemaining = g_ui16T2TTLVRemaining - 4;
				}
				else
				{
					g_ui16T2TTLVRemaining = 0;
					g_eT2TState = T2T_SELECTED_IDLE;

					Serial_printf("Write Successful!",RW_STATUS_DATA);

					// Print New Line
					Serial_printf("\n",RW_STATUS_DATA);
				}
				g_bT2TWaitForRsp = false;
			}
			else if(eT2TStatus == NFC_RW_T2T_CONNECTION_WRITE_FAIL)
			{
				g_bT2TWaitForRsp = false;
				g_eT2TState = T2T_SELECTED_IDLE;
			}
			break;
		case T2T_SELECTED_IDLE:
			break;
		case T2T_SECTOR_SELECT:
			eT2TStatus = NFC_RW_T2T_getSectorSelStatus();

			if(eT2TStatus == NFC_RW_T2T_CONNECTION_SECTOR_SELECT_SUCCESS)
			{
				g_bT2TWaitForRsp = false;
				g_eT2TState = T2T_READ_NDEF;
			}
			else if(eT2TStatus == NFC_RW_T2T_CONNECTION_SECTOR_SELECT_FAIL)
			{
				g_bT2TWaitForRsp = false;
				g_eT2TState = T2T_SELECTED_IDLE;
			}
			break;
		}
	}

	// Sending a command
	if(g_bT2TWaitForRsp == false)
	{
		switch(g_eT2TState)
		{
		case T2T_READ_CC:
			g_ui8T2TBlockNumber = 0x03;

			if(NFC_RW_T2T_sendReadCmd(g_ui8T2TBlockNumber) == STATUS_SUCCESS)
			{
				g_bT2TWaitForRsp = true;
			}

			break;
		case T2T_READ_DATA:
			if(NFC_RW_T2T_sendReadCmd(g_ui8T2TBlockNumber) == STATUS_SUCCESS)
			{
				g_bT2TWaitForRsp = true;
			}
			break;
		case T2T_READ_NDEF:
			if(NFC_RW_T2T_sendReadCmd(g_ui8T2TBlockNumber) == STATUS_SUCCESS)
			{
				g_bT2TWaitForRsp = true;
			}
			break;
		case T2T_WRITE_NDEF:
			if(g_ui16T2TTLVRemaining > 0)
			{
				// Clear the NDEF Len for function purpose
				if(NFC_RW_T2T_sendWriteCmd(g_ui8T2TBlockNumber, &g_pui8T2TBuffer[g_ui16T2TNdefIndex],4) == STATUS_SUCCESS)
				{
					g_bT2TWaitForRsp = true;
				}
				else
				{
					g_eT2TState = T2T_SELECTED_IDLE;
				}
			}
			break;
		case T2T_SELECTED_IDLE:
			break;
		case T2T_SECTOR_SELECT:
			if(NFC_RW_T2T_sendSectorSel(1) == STATUS_SUCCESS)
			{
				g_bT2TWaitForRsp = true;
			}
			break;
		}
	}
}

