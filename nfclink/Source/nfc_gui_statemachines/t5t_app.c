//*****************************************************************************
//
// t5t_app.c - Reader/Writer Type 5 Tag State Machine
//
// Copyright (c) 2015 Texas Instruments Incorporated.  All rights reserved.
// TI Information - Selective Disclosure
//
//*****************************************************************************
//
// This file will test the following T5T API's:
//
//  NFC_RW_T5T_getInventoryStatus
//  NFC_RW_T5T_getReadSingleStatus
//  NFC_RW_T5T_sendInventoryCmd
//  NFC_RW_T5T_sendReadSingleCmd
//  NFC_RW_T5T_sendGetSysInfoCmd
//  NFC_RW_T5T_getPacketStatus
//  NFC_RW_T5T_getVICCBlockCount
//
// The flow of this test is as follows:
//	Inventory
//	Identify the tag via Get System Information sequence
//	Read Single Block for the entire tag
//
//******************************************************************************

//******************************************************************************
//	The Following NFC configuration were used for these tests:
//------------------------------------------------------------------------------
//	// Enable Reader Writer Supported Modes
//	sRWSupportedModes.bits.bNfcA = 0;
//	sRWSupportedModes.bits.bNfcB = 0;
//	sRWSupportedModes.bits.bNfcF = 0;
//	sRWSupportedModes.bits.bISO15693 = 1;
//
//	// NFC-A Bitrates
//	sRWSupportedBitrates.bits.bNfcA_106kbps = 1;
//	sRWSupportedBitrates.bits.bNfcA_212kbps = 0;
//	sRWSupportedBitrates.bits.bNfcA_424kbps = 0;
//	sRWSupportedBitrates.bits.bNfcA_848kbps = 1;
//	// NFC-B Bitrates
//	sRWSupportedBitrates.bits.bNfcB_106kbps = 1;
//	sRWSupportedBitrates.bits.bNfcB_212kbps = 0;
//	sRWSupportedBitrates.bits.bNfcB_424kbps = 1;
//	sRWSupportedBitrates.bits.bNfcB_848kbps = 0;
//	// NFC-F Bitrates
//	sRWSupportedBitrates.bits.bNfcF_212kbps = 1;
//	sRWSupportedBitrates.bits.bNfcF_424kbps = 0;
//	// ISO15693 Bitrates
//	sRWSupportedBitrates.bits.bISO15693_26_7kbps = 1;
//
//	// Default Max number of WTX 2
//	sRWSetupOptions.bits.ui3RWMaxWTX = 2;
//	// Default Max number of ACK 2
//	sRWSetupOptions.bits.ui3RWMaxACK = 2;
//	// Default Max number of NACK 2
//	sRWSetupOptions.bits.ui3RWMaxNACK = 2;
//	// Default Max number of DSL 2
//	sRWSetupOptions.bits.ui3RWMaxDSL = 2;
//
//	ui8IsoDepInitiatorDID = 0x00;
//******************************************************************************

#include "tag_header.h"

//*****************************************************************************

tT5TStateMachine g_eT5TState;

static uint16_t g_ui16T5TBlockNumber;
static uint16_t g_ui16T5TBlockCount;
static uint8_t g_pui8T5TRxBuffer[30];

bool g_bT5TWaitForRsp;

uint8_t * g_pui8T5TBuffer;

uint16_t g_ui16T5TNdefLen;

uint16_t g_ui16T5TMaxNdefLen;

uint16_t g_ui16T5TNdefIndex;

uint16_t g_ui16T5TTLVRemaining;

uint8_t g_ui8T5TCurrentTlv;

uint16_t g_ui16T5TSize;

bool g_bT5TTLVSelected;

bool g_bT5TTLVLengthKnown;

uint8_t g_ui8T5TTLVLengthRemainBytes;

uint16_t g_ui16T5TReadIndex;

bool g_bT5TFormatting;

void T5T_init(uint8_t * pui8Ndef, uint16_t ui16NdefMaxSize)
{
	g_eT5TState = T5T_INVENTORY;
	g_ui16T5TBlockNumber = 0;
	g_ui16T5TBlockCount = 0;
	g_bT5TWaitForRsp = false;

	g_pui8T5TBuffer = pui8Ndef;

	g_ui16T5TMaxNdefLen = ui16NdefMaxSize;

	g_ui16T5TNdefIndex = 0;

	g_ui16T5TNdefLen = 0;

	g_ui16T5TTLVRemaining = 0;

	g_ui8T5TCurrentTlv = 0;

	g_ui16T5TSize = 0;

	g_bT5TTLVSelected = false;

	g_bT5TTLVLengthKnown = false;

	g_ui8T5TTLVLengthRemainBytes = 0;

	g_ui16T5TReadIndex = 0;

	g_bT5TFormatting = false;
}

void T5T_writeNDEF(uint8_t * pui8Data, uint16_t ui16NdefLen)
{
	uint8_t ui8Offset;

	ui8Offset = 0;

	if(g_eT5TState == T5T_SELECTED_IDLE)
	{
		// Total Size of tag - 3 bytes (of TLV overhead Type, Length and Terminator TLV) - 4 bytes (Capability Container)
		if(g_ui16T5TSize > 7 && (g_ui16T5TSize-7) >= ui16NdefLen)
		{
			g_eT5TState = T5T_WRITE_NDEF;

			g_ui16T5TNdefIndex = 0;

			// Start Writing at Block 1
			g_ui16T5TBlockNumber = 0x01;

			// NDEF Msg TLV - Type
			g_pui8T5TBuffer[ui8Offset++] = TLV_NDEF_MSG;
			// NDEF Msg TLV - Length
			if(ui16NdefLen < 0xFF)
			{
				g_pui8T5TBuffer[ui8Offset++] = (uint8_t) ui16NdefLen;
			}

			g_ui16T5TTLVRemaining = (uint16_t) ui8Offset+ui16NdefLen+1;

			memcpy(&g_pui8T5TBuffer[ui8Offset],pui8Data,ui16NdefLen);

			g_pui8T5TBuffer[ui8Offset+ui16NdefLen] = TLV_TERMINATOR;
		}
		else
		{
			Serial_printf("T5T Write Fail: NDEF message size not supported by tag.\n",RW_STATUS_DATA);
		}

	}
	else
	{
		Serial_printf("T5T Write Fail: Busy.\n",RW_STATUS_DATA);
	}
}

void T5T_formatTag(void)
{
	uint8_t ui8TagBlockSize;

	Serial_printf("T5T Formatting...\n",RW_STATUS_DATA);

	ui8TagBlockSize = NFC_RW_T5T_getVICCBlockSize();

	if(g_eT5TState == T5T_SELECTED_IDLE)
	{
		g_eT5TState = T5T_WRITE_NDEF;

		g_bT5TFormatting = true;

		g_ui16T5TNdefIndex = 0;

		g_ui16T5TBlockNumber = 0;

		// Check for Extended Memory Tags
		if (g_ui16T5TBlockCount > 255)
		{
			g_ui16T5TTLVRemaining = 12;			// Need 2 Write Blocks to finish writing CC

			g_pui8T5TBuffer[0] = 0xE2;			// Tags with more than 256 blocks have Magic Number = 0xE2
			g_pui8T5TBuffer[1] = 0x40;			// NFC Major Version = 1, NFC Minor Version = 0, Read Access = Always, Write Access = Always
			g_pui8T5TBuffer[2] = 0x00;

			if (g_ui16T5TBlockCount == 0x0A)
			{
				g_pui8T5TBuffer[3] = 0x10;		// TI Tag-It Pro/Standard needs Option Flag
			}
			else
			{
				g_pui8T5TBuffer[3] = 0x00;
			}

			g_pui8T5TBuffer[4] = 0x00;			// RFU
			g_pui8T5TBuffer[5] = 0x00;			// RFU
			g_pui8T5TBuffer[6] = (uint8_t) ((g_ui16T5TBlockCount & 0xFF00) >> 8);
			g_pui8T5TBuffer[7] = (uint8_t) (g_ui16T5TBlockCount && 0x00FF);

			// Write an empty NDEF to tag
			g_pui8T5TBuffer[8] = 0x03;
			g_pui8T5TBuffer[9] = 0x00;
			g_pui8T5TBuffer[10] = 0xFE;
			g_pui8T5TBuffer[11] = 0x00;
		}
		else
		{
			g_ui16T5TTLVRemaining = 8;			// Need 1 Write Block for CC

			g_pui8T5TBuffer[0] = 0xE1;			// All other tags have Magic Number = 0xE1
			g_pui8T5TBuffer[1] = 0x40;			// NFC Major Version = 1, NFC Minor Version = 0, Read Access = Always, Write Access = Always

			if (g_ui16T5TBlockCount == 0x0A)
			{
				g_pui8T5TBuffer[2] = 0x04;
				g_pui8T5TBuffer[3] = 0x10;		// TI Tag-It Pro/Standard needs Option Flag
			}
			else
			{
				g_pui8T5TBuffer[2] = (uint8_t) (((g_ui16T5TBlockCount+1)*(ui8TagBlockSize+1))>>3);
				g_pui8T5TBuffer[3] = 0x00;
			}

			// Write an empty NDEF to tag
			g_pui8T5TBuffer[4] = 0x03;
			g_pui8T5TBuffer[5] = 0x00;
			g_pui8T5TBuffer[6] = 0xFE;
			g_pui8T5TBuffer[7] = 0x00;
		}
	}
	else
	{
		Serial_printf("T5T Write Fail: Busy./n",RW_STATUS_DATA);
	}
}

void T5T_stateMachine(void)
{
	uint8_t ui8RxLength;
	uint8_t * pui8RxData;
	tNfcRwT5TConnectionStatus eT5TStatus;
	char pui8LenBuffer[5];
	uint8_t ui8Temp;
	uint8_t ui8LocalReadIndex;

	// Waiting for a response
	if(g_bT5TWaitForRsp == true)
	{
		switch(g_eT5TState)
		{
		case T5T_INVENTORY:
			eT5TStatus = NFC_RW_T5T_getInventoryStatus();
			if(eT5TStatus == NFC_RW_T5T_CONNECTION_INVENTORY_SUCCESS)
			{
				g_eT5TState = T5T_GET_SYS_INFO_EXT;
			}
			else
			{
				g_eT5TState = T5T_SELECTED_IDLE;
			}
			g_bT5TWaitForRsp = false;
			break;
		case T5T_GET_SYS_INFO_EXT:
			eT5TStatus = NFC_RW_T5T_getGetSysInfoStatus();
			if(eT5TStatus == NFC_RW_T5T_CONNECTION_GET_SYS_INFO_SUCCESS)
			{
				NFC_RW_T5T_getPacketStatus(&pui8RxData,&ui8RxLength);

				memcpy(g_pui8T5TRxBuffer,pui8RxData,ui8RxLength);

				g_eT5TState = T5T_READ_CC;

				g_ui16T5TBlockCount = NFC_RW_T5T_getVICCBlockCount();
			}
			else if(eT5TStatus == NFC_RW_T5T_CONNECTION_GET_SYS_INFO_FAIL)
			{
				g_eT5TState = T5T_GET_SYS_INFO_1;
			}
			g_bT5TWaitForRsp = false;
			break;
		case T5T_GET_SYS_INFO_1:
			eT5TStatus = NFC_RW_T5T_getGetSysInfoStatus();
			if(eT5TStatus == NFC_RW_T5T_CONNECTION_GET_SYS_INFO_SUCCESS)
			{
				NFC_RW_T5T_getPacketStatus(&pui8RxData,&ui8RxLength);

				memcpy(g_pui8T5TRxBuffer,pui8RxData,ui8RxLength);

				g_eT5TState = T5T_READ_CC;

				g_ui16T5TBlockCount = NFC_RW_T5T_getVICCBlockCount();
			}
			else if(eT5TStatus == NFC_RW_T5T_CONNECTION_GET_SYS_INFO_FAIL)
			{
				g_eT5TState = T5T_GET_SYS_INFO_2;
			}
			g_bT5TWaitForRsp = false;
			break;
		case T5T_GET_SYS_INFO_2:
			eT5TStatus = NFC_RW_T5T_getGetSysInfoStatus();
			if(eT5TStatus == NFC_RW_T5T_CONNECTION_GET_SYS_INFO_SUCCESS)
			{
				g_eT5TState = T5T_READ_CC;

				g_ui16T5TBlockCount = NFC_RW_T5T_getVICCBlockCount();
			}
			else if(eT5TStatus == NFC_RW_T5T_CONNECTION_GET_SYS_INFO_FAIL)
			{
				g_eT5TState = T5T_READ_CC;

				g_ui16T5TBlockCount = 0x0A;
			}
			g_bT5TWaitForRsp = false;
			break;
		case T5T_READ_CC:
			eT5TStatus = NFC_RW_T5T_getReadSingleStatus();
			if(eT5TStatus == NFC_RW_T5T_CONNECTION_READ_SINGLE_SUCCESS)
			{
				NFC_RW_T5T_getPacketStatus(&pui8RxData,&ui8RxLength);

				memcpy(g_pui8T5TRxBuffer,pui8RxData+1,ui8RxLength-1);

				// Check if NDEF message formatted
				if(g_pui8T5TRxBuffer[0] == 0xE1)
				{
					// Switch to the read NDEF data
					g_eT5TState = T5T_READ_NDEF;

					g_ui16T5TSize = g_pui8T5TRxBuffer[2] << 3;
				}
				else
				{
					// Switch to the read NDEF data
					g_eT5TState = T5T_READ_DATA;

					Serial_printf("T5T Not NDEF Formatted",RW_STATUS_DATA);

					// Print New Line
					Serial_printf("\n",RW_STATUS_DATA);

					Serial_printf("Block ",RW_STATUS_DATA);

					// Print Block Number
					convertWordToAscii(g_ui16T5TBlockNumber,(uint8_t *) pui8LenBuffer);

					Serial_printf(pui8LenBuffer,RW_STATUS_DATA);

					Serial_printf(" ",RW_STATUS_DATA);

					for(ui8Temp = 0; ui8Temp < (ui8RxLength-1); ui8Temp++)
					{
						convertByteToAscii(g_pui8T5TRxBuffer[ui8Temp],(uint8_t *) pui8LenBuffer);

						Serial_printf("0x",RW_STATUS_DATA);

						Serial_printf(pui8LenBuffer,RW_STATUS_DATA);

						Serial_printf(" ",RW_STATUS_DATA);
					}

					// Print New Line
					Serial_printf("\n",RW_STATUS_DATA);

					g_ui16T5TSize = 0;
				}

				// Incrment Block Number
				g_ui16T5TBlockNumber++;
			}
			else if(eT5TStatus == NFC_RW_T5T_CONNECTION_READ_SINGLE_FAIL)
			{
				g_eT5TState = T5T_SELECTED_IDLE;
			}
			g_bT5TWaitForRsp = false;
			break;
		case T5T_READ_DATA:
			eT5TStatus = NFC_RW_T5T_getReadSingleStatus();
			if(eT5TStatus == NFC_RW_T5T_CONNECTION_READ_SINGLE_SUCCESS)
			{
				NFC_RW_T5T_getPacketStatus(&pui8RxData,&ui8RxLength);

				memcpy(g_pui8T5TRxBuffer,pui8RxData+1,ui8RxLength-1);

				Serial_printf("Block ",RW_STATUS_DATA);

				// Print Block Number
				convertWordToAscii(g_ui16T5TBlockNumber,(uint8_t *) pui8LenBuffer);

				Serial_printf(pui8LenBuffer,RW_STATUS_DATA);

				Serial_printf(" ",RW_STATUS_DATA);

				for(ui8Temp = 0; ui8Temp < (ui8RxLength-1); ui8Temp++)
				{
					convertByteToAscii(g_pui8T5TRxBuffer[ui8Temp],(uint8_t *) pui8LenBuffer);

					Serial_printf("0x",RW_STATUS_DATA);

					Serial_printf(pui8LenBuffer,RW_STATUS_DATA);

					Serial_printf(" ",RW_STATUS_DATA);
				}

				// Print New Line
				Serial_printf("\n",RW_STATUS_DATA);

				g_ui16T5TBlockNumber++;

				if(g_ui16T5TBlockNumber > g_ui16T5TBlockCount)
				{
					g_eT5TState = T5T_SELECTED_IDLE;
				}
			}
			else if(eT5TStatus == NFC_RW_T5T_CONNECTION_READ_SINGLE_FAIL)
			{
				g_eT5TState = T5T_SELECTED_IDLE;
			}
			g_bT5TWaitForRsp = false;
			break;
		case T5T_READ_NDEF:
			eT5TStatus = NFC_RW_T5T_getReadSingleStatus();
			if(eT5TStatus == NFC_RW_T5T_CONNECTION_READ_SINGLE_SUCCESS)
			{
				NFC_RW_T5T_getPacketStatus(&pui8RxData,&ui8RxLength);

				memcpy(g_pui8T5TRxBuffer,pui8RxData+1,ui8RxLength-1);

				ui8RxLength = ui8RxLength - 1;

				ui8LocalReadIndex = 0x00;

				// Processes all the bytes returned from read block #
				while(ui8LocalReadIndex < ui8RxLength && g_eT5TState == T5T_READ_NDEF)
				{

					// TLV handling

					//  No TLV is selected
					if(g_bT5TTLVSelected == false)
					{
						// Read TLV
						g_ui8T5TCurrentTlv =  g_pui8T5TRxBuffer[ui8LocalReadIndex++];

						// NULL TLV just increment the read index
						if(g_ui8T5TCurrentTlv == TLV_NULL)
						{
							// Do nothing
						}
						// TERMINATOR TLV we are done reading
						else if(g_ui8T5TCurrentTlv == TLV_TERMINATOR)
						{
							g_eT5TState = T5T_SELECTED_IDLE;
						}
						else
						{
							// Go read length
							g_bT5TTLVSelected = true;

							g_bT5TTLVLengthKnown = false;
							// Default
							g_ui8T5TTLVLengthRemainBytes = 0xFF;
						}

					}
					// TLV is selected and Length not known
					else if(g_bT5TTLVLengthKnown == false)
					{
						// No length bytes have been read - (0xFF is default value)
						if(g_ui8T5TTLVLengthRemainBytes == 0xFF)
						{
							// Large NDEF - NDEF length is stored in two subsequent bytes
							if(g_pui8T5TRxBuffer[ui8LocalReadIndex] == 0xFF)
							{
								g_ui8T5TTLVLengthRemainBytes = 2;
							}
							else
							{
								g_bT5TTLVLengthKnown = true;
								g_ui16T5TNdefLen = g_pui8T5TRxBuffer[ui8LocalReadIndex];
								g_ui16T5TTLVRemaining = g_ui16T5TNdefLen;
								g_ui16T5TReadIndex = 0;
							}
						}
						// Read 1st length byte
						else if(g_ui8T5TTLVLengthRemainBytes == 2)
						{
							g_ui16T5TNdefLen = (uint16_t) (g_pui8T5TRxBuffer[ui8LocalReadIndex] << 8);
							g_ui8T5TTLVLengthRemainBytes = 1;
						}
						else if(g_ui8T5TTLVLengthRemainBytes == 1)
						{
							g_bT5TTLVLengthKnown = true;
							g_ui16T5TNdefLen |= ((uint16_t) g_pui8T5TRxBuffer[ui8LocalReadIndex] & 0xFF);
							g_ui16T5TTLVRemaining = g_ui16T5TNdefLen;
							g_ui8T5TTLVLengthRemainBytes = 0;
							g_ui16T5TReadIndex = 0;
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
						if(g_ui8T5TCurrentTlv == TLV_NDEF_MSG)
						{
							// If the current TLV remaining bytes are more than the available bytes
							if(g_ui16T5TTLVRemaining > (ui8RxLength - ui8LocalReadIndex))
							{
//								Serial_printBuffer((char *)&g_pui8T5TRxBuffer[ui8ReadIndex],(15 - ui8ReadIndex),RW_PAYLOAD_DATA);

								// Store Incoming NDEF Message into g_pui8T5TBuffer
								if(g_ui16T5TMaxNdefLen > (g_ui16T5TReadIndex + (ui8RxLength - ui8LocalReadIndex)))
								{
									memcpy(&g_pui8T5TBuffer[g_ui16T5TReadIndex],&g_pui8T5TRxBuffer[ui8LocalReadIndex],(uint16_t) ui8RxLength - ui8LocalReadIndex);
								}
								else
								{
									// NDEF message is larger than the size of the g_pui8T5TBuffer buffer
								}

								g_ui16T5TReadIndex += ui8RxLength - ui8LocalReadIndex;


							}
							else
							{
								// Store Incoming NDEF Message into g_pui8T5TBuffer
								if(g_ui16T5TMaxNdefLen > (g_ui16T5TReadIndex + g_ui16T5TTLVRemaining))
								{
									memcpy(&g_pui8T5TBuffer[g_ui16T5TReadIndex],&g_pui8T5TRxBuffer[ui8LocalReadIndex],g_ui16T5TTLVRemaining);

									Serial_printBuffer((char *)&g_pui8T5TBuffer[0],(uint8_t) (g_ui16T5TReadIndex+g_ui16T5TTLVRemaining),RW_PAYLOAD_DATA);
								}
								else
								{
									// NDEF message is larger than the size of the g_pui8T5TBuffer buffer
								}

								g_ui16T5TReadIndex += g_ui16T5TTLVRemaining;
							}
						}

						// Incrementing Index

						// If the current TLV remaining bytes are more than the available bytes
						if(g_ui16T5TTLVRemaining > (ui8RxLength - ui8LocalReadIndex))
						{
							// Update Remaining
							g_ui16T5TTLVRemaining -= (uint16_t) ( ui8RxLength - ui8LocalReadIndex);
							// Increase Index to 16 to continue reading next block
							ui8LocalReadIndex = ui8RxLength;
						}
						else
						{
							// Increase read index by remaining number of bytes
							ui8LocalReadIndex += (uint8_t) g_ui16T5TTLVRemaining;
							g_ui16T5TTLVRemaining = 0;
							// Finished reading TLV
							g_bT5TTLVSelected = false;
						}
					}
				}

				if(g_eT5TState != T5T_SELECTED_IDLE)
				{
					// Incrment Block Number
					g_ui16T5TBlockNumber++;

					// EOF Not Found
					if(g_ui16T5TBlockNumber > g_ui16T5TBlockCount)
					{
						Serial_printf("Error: T5T Terminator TLV not found.",RW_STATUS_DATA);

						// Print New Line
						Serial_printf("\n",RW_STATUS_DATA);

						g_eT5TState = T5T_SELECTED_IDLE;
					}
				}
			}
			else if(eT5TStatus == NFC_RW_T5T_CONNECTION_READ_SINGLE_FAIL)
			{
				g_eT5TState = T5T_SELECTED_IDLE;
			}
			g_bT5TWaitForRsp = false;
			break;
		case T5T_WRITE_NDEF:
			eT5TStatus = NFC_RW_T5T_getWriteSingleStatus();

			if(eT5TStatus == NFC_RW_T5T_CONNECTION_WRITE_SINGLE_SUCCESS)
			{
				g_ui16T5TBlockNumber++;
				g_ui16T5TNdefIndex = g_ui16T5TNdefIndex + 4;
				if(g_ui16T5TTLVRemaining > 4)
				{
					g_ui16T5TTLVRemaining = g_ui16T5TTLVRemaining - 4;
				}
				else
				{
					g_ui16T5TTLVRemaining = 0;
					g_eT5TState = T5T_SELECTED_IDLE;

					Serial_printf("T5T Write Successful!",RW_STATUS_DATA);

					// Print New Line
					Serial_printf("\n",RW_STATUS_DATA);

					if (g_bT5TFormatting)
					{
						Serial_printf("Re-present the Tag to Write to it.",RW_STATUS_DATA);

						// Print New Line
						Serial_printf("\n",RW_STATUS_DATA);
					}
				}
			}
			else if(eT5TStatus == NFC_RW_T5T_CONNECTION_WRITE_SINGLE_FAIL)
			{
				if (NFC_RW_T5T_getT5TErrorCode() == 0x12)
				{
					Serial_printf("T5T Write Fail! Target Block Is Locked",RW_STATUS_DATA);
				}

				g_eT5TState = T5T_SELECTED_IDLE;
			}
			g_bT5TWaitForRsp = false;
			break;
		case T5T_SELECTED_IDLE:
			g_bT5TWaitForRsp = false;
			break;
		}
	}

	// Sending a command
	if(g_bT5TWaitForRsp == false)
	{
		switch(g_eT5TState)
		{
		case T5T_INVENTORY:
			if(NFC_RW_T5T_sendInventoryCmd(0x26,0x00,false) == STATUS_SUCCESS)
			{
				g_bT5TWaitForRsp = true;
			}
			break;
		case T5T_GET_SYS_INFO_EXT:
			if(NFC_RW_T5T_sendGetSysInfoCmd(0x02,0x3B) == STATUS_SUCCESS)
			{
				g_bT5TWaitForRsp = true;
			}
			break;
		case T5T_GET_SYS_INFO_1:
			if(NFC_RW_T5T_sendGetSysInfoCmd(0x0A,0x2B) == STATUS_SUCCESS)
			{
				g_bT5TWaitForRsp = true;
			}
			break;
		case T5T_GET_SYS_INFO_2:
			if(NFC_RW_T5T_sendGetSysInfoCmd(0x02,0x2B) == STATUS_SUCCESS)
			{
				g_bT5TWaitForRsp = true;
			}
			break;
		case T5T_READ_CC:
			if(NFC_RW_T5T_sendReadSingleCmd(0x22,g_ui16T5TBlockNumber) == STATUS_SUCCESS)
			{
				g_bT5TWaitForRsp = true;
			}
			break;
		case T5T_READ_DATA:
			if(NFC_RW_T5T_sendReadSingleCmd(0x22,g_ui16T5TBlockNumber) == STATUS_SUCCESS)
			{
				g_bT5TWaitForRsp = true;
			}
			break;
		case T5T_READ_NDEF:
			if(NFC_RW_T5T_sendReadSingleCmd(0x22,g_ui16T5TBlockNumber) == STATUS_SUCCESS)
			{
				g_bT5TWaitForRsp = true;
			}
			break;
		case T5T_WRITE_NDEF:
			if(g_ui16T5TTLVRemaining > 0)
			{
				if(NFC_RW_T5T_sendWriteSingleCmd(T5T_REQ_FLAG_HIGH_DATA|T5T_REQ_FLAG_OPTION,g_ui16T5TBlockNumber,&g_pui8T5TBuffer[g_ui16T5TNdefIndex],4) == STATUS_SUCCESS)
				{
					g_bT5TWaitForRsp = true;
				}
				else
				{
					g_eT5TState = T5T_SELECTED_IDLE;
				}
			}
			break;
		case T5T_SELECTED_IDLE:
			break;
		}
	}
}

