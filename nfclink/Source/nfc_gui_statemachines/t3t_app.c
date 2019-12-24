//*****************************************************************************
//
// t3t_app.c - Reader/Writer Type 3 Tag State Machine
//
// Copyright (c) 2015 Texas Instruments Incorporated.  All rights reserved.
// TI Information - Selective Disclosure
//
//*****************************************************************************

#include "tag_header.h"

//*****************************************************************************

#define		T3T_DATA_BASE_INDEX			13

#define		T3T_WRITING_FLAG_ON			0x0F
#define		T3T_WRITING_FLAG_OFF		0x00

//*****************************************************************************

tT3TStateMachine g_eT3TState;

tT3TPacketData g_sTxT3TPacket;
uint8_t g_pui8Service[2] = {0x09, 0x00};
uint8_t g_pui8Block[2] = {0x80, 0x01};
uint8_t g_pui8T3TRxData[30];
uint8_t g_pui8T3TTxData[16];

uint8_t g_pui8Service_ext[14] = {0x09, 0x00,0x49,0x11,0x89,0x22,0xC9,0x33,0x09,0x44,0x49,0x55,0x89,0x66};
uint8_t g_pui8Block_ext[36] = {0x80, 0x00,0x80, 0x01,0x80, 0x02,
							 0x81, 0x00,0x81, 0x01,0x81, 0x02,
							 0x82, 0x00,0x82, 0x01,0x82, 0x02,
							 0x83, 0x00,0x83, 0x01,0x83, 0x02,
							 0x84, 0x00,0x84, 0x01,0x84, 0x02};
uint8_t * g_pui8T3TBuffer;

bool g_bT3TWaitForRsp;

uint16_t g_ui16T3TMaxNdefLen;

uint16_t g_ui16T3TBlockNumber;

uint16_t g_ui16T3TMaxBlocks;

uint16_t g_ui16T3TNdefLen;

uint16_t g_ui16T3TNdefIndex;

uint16_t g_ui16T3TSize;

void T3T_init(uint8_t * pui8Ndef, uint16_t ui16NdefMaxSize)
{
	g_ui16T3TBlockNumber = 0;
	g_eT3TState = T3T_SENSF_REQ;
	g_bT3TWaitForRsp = false;

	g_pui8T3TBuffer = pui8Ndef;
	g_ui16T3TMaxNdefLen = ui16NdefMaxSize;

	g_ui16T3TMaxBlocks = 0;
	g_ui16T3TNdefLen = 0;
	g_ui16T3TNdefIndex = 0;
	g_ui16T3TSize = 0;
}

void T3T_writeNDEF(uint8_t * pui8Data, uint16_t ui16NdefLen)
{
	uint8_t ui8Temp;
	uint16_t ui16BCC;

	// Make sure Tag is formatted and that the T3T statemachine is in idle state
	if(g_eT3TState == T3T_SELECTED_IDLE)
	{
		if(g_ui16T3TSize >= ui16NdefLen)
		{
			// Copy NDEF mesage
			memcpy(g_pui8T3TBuffer,pui8Data,ui16NdefLen);

			// Se the NDEF Index
			g_ui16T3TNdefIndex = 0;

			// Set Ndef Len
			g_ui16T3TNdefLen = ui16NdefLen;

			// Reset the Block Number
			g_ui16T3TBlockNumber = 0;

			// Write Flag
			g_pui8T3TTxData[9] = T3T_WRITING_FLAG_ON;

			// Length Bytes
			g_pui8T3TTxData[12] = (uint8_t) (ui16NdefLen >> 8);
			g_pui8T3TTxData[13] = (uint8_t) (ui16NdefLen & 0xFF);

			// Clear BCC
			ui16BCC = 0x00;

			// Calculate BCC
			for(ui8Temp = 0; ui8Temp < 14; ui8Temp++)
			{
				ui16BCC = ui16BCC + (uint16_t) g_pui8T3TTxData[ui8Temp];
			}

			// BCC Bytes
			g_pui8T3TTxData[14] = (uint8_t) (ui16BCC >> 8);
			g_pui8T3TTxData[15] = (uint8_t) (ui16BCC & 0xFF);

			g_eT3TState = T3T_WRITE_NDEF;
		}
		else
		{
			Serial_printf("T3T Write Fail: NDEF message size not supported by tag.",RW_STATUS_DATA);

			// Print New Line
			Serial_printf("\n",RW_STATUS_DATA);
		}
	}
	else
	{
		Serial_printf("T3T Write Fail: Busy.",RW_STATUS_DATA);

		// Print New Line
		Serial_printf("\n",RW_STATUS_DATA);
	}
}

void T3T_stateMachine(void)
{
	tNfcRwT3TConnectionStatus sT3TState;
	uint8_t ui8Count;
	uint8_t * pui8Data;
	char pui8LenBuffer[5];
	uint8_t ui8Temp;

	uint16_t ui16BCC;
	uint16_t ui16BCCTemp;


	// Waiting for a response
	if(g_bT3TWaitForRsp == true)
	{
		switch(g_eT3TState)
		{
		case T3T_SENSF_REQ:
			if(NFC_RW_T3T_getSensFStatus() == NFC_RW_T3T_CONNECTION_SENSF_REQ_SUCCESS)
			{
				g_ui16T3TBlockNumber = 0;
				g_eT3TState = T3T_READ_CC;
				g_bT3TWaitForRsp = false;
			}
			break;
		case T3T_READ_CC:
			sT3TState = NFC_RW_T3T_getCheckStatus();

			if(sT3TState == NFC_RW_T3T_CONNECTION_CHECK_SUCCESS)
			{
				NFC_RW_T3T_getPacketStatus(&pui8Data,&ui8Count);

				memcpy(g_pui8T3TTxData,&pui8Data[T3T_DATA_BASE_INDEX],16);
				memcpy(g_pui8T3TRxData,pui8Data,ui8Count);

				ui16BCC = ((uint16_t) (g_pui8T3TRxData[T3T_DATA_BASE_INDEX+14] << 8) + \
						  	   (uint16_t) (g_pui8T3TRxData[T3T_DATA_BASE_INDEX+15] & 0xFF));

				ui16BCCTemp = 0;

				for(ui8Temp = 0; ui8Temp < 14; ui8Temp++)
				{
					ui16BCCTemp = ui16BCCTemp + (uint16_t) g_pui8T3TRxData[T3T_DATA_BASE_INDEX+ui8Temp];
				}

				// Check if message is NDEF compliant

				// Make sure that the NDEF Version is not equal to 0x00 and that the checksum is correct
				if(g_pui8T3TRxData[T3T_DATA_BASE_INDEX] != 0x00 && ui16BCC == ui16BCCTemp)
				{
					g_ui16T3TMaxBlocks = (uint16_t) (g_pui8T3TRxData[T3T_DATA_BASE_INDEX+3] << 8) + \
										 (uint16_t) (g_pui8T3TRxData[T3T_DATA_BASE_INDEX+4] & 0xFF);

					g_ui16T3TNdefLen = (uint16_t) (g_pui8T3TRxData[T3T_DATA_BASE_INDEX+12] << 8) + \
							 	 	   (uint16_t) (g_pui8T3TRxData[T3T_DATA_BASE_INDEX+13] & 0xFF);

					// g_ui16T3TSize = Blocks x 16
					g_ui16T3TSize = g_ui16T3TMaxBlocks << 4;

					// Check if the len of the NDEF message is > 0
					if(g_ui16T3TNdefLen)
					{
						g_eT3TState = T3T_READ_NDEF;
					}
					else
					{
						g_eT3TState = T3T_SELECTED_IDLE;
					}
				}
				else
				{
					Serial_printf("T3T Not NDEF Formatted",RW_STATUS_DATA);

					// Print New Line
					Serial_printf("\n",RW_STATUS_DATA);

					Serial_printf("Block ",RW_STATUS_DATA);

					// Print Block Number
					convertWordToAscii(g_ui16T3TBlockNumber,(uint8_t *) pui8LenBuffer);

					Serial_printf(pui8LenBuffer,RW_STATUS_DATA);

					Serial_printf(" ",RW_STATUS_DATA);

					for(ui8Temp = 0; ui8Temp < 16; ui8Temp++)
					{
						convertByteToAscii(g_pui8T3TRxData[ui8Temp+T3T_DATA_BASE_INDEX], (uint8_t *) pui8LenBuffer);

						Serial_printf("0x",RW_STATUS_DATA);

						Serial_printf(pui8LenBuffer,RW_STATUS_DATA);

						Serial_printf(" ",RW_STATUS_DATA);
					}

					// Print New Line
					Serial_printf("\n",RW_STATUS_DATA);

					g_eT3TState = T3T_READ_DATA;

					g_ui16T3TSize = 0;
				}

				g_ui16T3TBlockNumber++;

				g_bT3TWaitForRsp = false;
			}
			else if(sT3TState == NFC_RW_T3T_CONNECTION_CHECK_FAIL)
			{
				g_eT3TState = T3T_SELECTED_IDLE;
				g_bT3TWaitForRsp = false;
			}
			break;
		case T3T_READ_DATA:
			sT3TState = NFC_RW_T3T_getCheckStatus();

			if(sT3TState == NFC_RW_T3T_CONNECTION_CHECK_SUCCESS)
			{
				NFC_RW_T3T_getPacketStatus(&pui8Data,&ui8Count);

				memcpy(g_pui8T3TRxData,pui8Data,ui8Count);

				Serial_printf("Block ",RW_STATUS_DATA);

				// Print Block Number
				convertWordToAscii(g_ui16T3TBlockNumber,(uint8_t *) pui8LenBuffer);

				Serial_printf(pui8LenBuffer,RW_STATUS_DATA);

				Serial_printf(" ",RW_STATUS_DATA);

				for(ui8Temp = 0; ui8Temp < 16; ui8Temp++)
				{
					convertByteToAscii(g_pui8T3TRxData[ui8Temp+T3T_DATA_BASE_INDEX], (uint8_t *) pui8LenBuffer);

					Serial_printf("0x",RW_STATUS_DATA);

					Serial_printf(pui8LenBuffer,RW_STATUS_DATA);

					Serial_printf(" ",RW_STATUS_DATA);
				}

				// Print New Line
				Serial_printf("\n",RW_STATUS_DATA);

				g_ui16T3TBlockNumber++;

				if(g_ui16T3TBlockNumber == 0x0D)
				{
					g_eT3TState = T3T_SELECTED_IDLE;
				}

				g_bT3TWaitForRsp = false;
			}
			else if(sT3TState == NFC_RW_T3T_CONNECTION_CHECK_FAIL)
			{
				g_eT3TState = T3T_SELECTED_IDLE;
				g_bT3TWaitForRsp = false;
			}
			break;
		case T3T_READ_NDEF:
			sT3TState = NFC_RW_T3T_getCheckStatus();

			if(sT3TState == NFC_RW_T3T_CONNECTION_CHECK_SUCCESS)
			{
				NFC_RW_T3T_getPacketStatus(&pui8Data,&ui8Count);

				memcpy(g_pui8T3TRxData,pui8Data,ui8Count);

				if ((g_ui16T3TNdefLen < g_ui16T3TMaxNdefLen) && (g_ui16T3TNdefIndex < g_ui16T3TMaxNdefLen))
				{
					// Print NDEF Value(s)
					if(g_ui16T3TNdefLen > 16)
					{
						memcpy(&g_pui8T3TBuffer[g_ui16T3TNdefIndex],&g_pui8T3TRxData[T3T_DATA_BASE_INDEX],16);
						g_ui16T3TNdefIndex = g_ui16T3TNdefIndex + 16;
						g_ui16T3TNdefLen = g_ui16T3TNdefLen - 16;
					}
					else
					{
						memcpy(&g_pui8T3TBuffer[g_ui16T3TNdefIndex],&g_pui8T3TRxData[T3T_DATA_BASE_INDEX],g_ui16T3TNdefLen);
						g_ui16T3TNdefIndex = g_ui16T3TNdefIndex + g_ui16T3TNdefLen;
						Serial_printBuffer((char *)g_pui8T3TBuffer,(uint8_t) g_ui16T3TNdefIndex,RW_PAYLOAD_DATA);
						g_ui16T3TNdefLen = 0;
						g_eT3TState = T3T_SELECTED_IDLE;
					}
				}
				else
				{
					// Print NDEF Value(s)
					if(g_ui16T3TNdefLen > 16)
					{
						memcpy(&g_pui8T3TBuffer[0],&g_pui8T3TRxData[T3T_DATA_BASE_INDEX],16);
						Serial_printBuffer((char *)g_pui8T3TBuffer,16,RW_PAYLOAD_DATA);
						g_ui16T3TNdefIndex = g_ui16T3TNdefIndex + 16;
						g_ui16T3TNdefLen = g_ui16T3TNdefLen - 16;
					}
					else
					{
						memcpy(&g_pui8T3TBuffer[0],&g_pui8T3TRxData[T3T_DATA_BASE_INDEX],16);
						Serial_printBuffer((char *)g_pui8T3TBuffer,g_ui16T3TNdefLen,RW_PAYLOAD_DATA);
						g_ui16T3TNdefIndex = g_ui16T3TNdefIndex + g_ui16T3TNdefLen;
						g_ui16T3TNdefLen = 0;
						g_eT3TState = T3T_SELECTED_IDLE;
					}
				}

				g_ui16T3TBlockNumber++;

				g_bT3TWaitForRsp = false;
			}
			else if(sT3TState == NFC_RW_T3T_CONNECTION_CHECK_FAIL)
			{
				g_eT3TState = T3T_SELECTED_IDLE;
				g_bT3TWaitForRsp = false;
			}
			break;
		case T3T_WRITE_NDEF:
			sT3TState = NFC_RW_T3T_getUpdateStatus();
			if(sT3TState == NFC_RW_T3T_CONNECTION_UPDATE_SUCCESS)
			{
				if(g_ui16T3TBlockNumber > 0)
				{
					if(g_ui16T3TNdefLen > 16)
					{
						g_ui16T3TNdefLen = g_ui16T3TNdefLen - 16;
						g_ui16T3TNdefIndex = g_ui16T3TNdefIndex + 16;
					}
					else
					{
						g_ui16T3TNdefIndex = g_ui16T3TNdefIndex + g_ui16T3TNdefLen;
						g_ui16T3TNdefLen = 0;
					}
				}
				else
				{
					// When BlockNumber and NdefLen are both 0, then the NDEF has been written
					if(g_ui16T3TNdefLen == 0)
					{
						g_eT3TState = T3T_SELECTED_IDLE;

						Serial_printf("Write Successful!",RW_STATUS_DATA);

						// Print New Line
						Serial_printf("\n",RW_STATUS_DATA);
					}
				}

				g_ui16T3TBlockNumber++;

				g_bT3TWaitForRsp = false;
			}
			else if(sT3TState == NFC_RW_T3T_CONNECTION_UPDATE_FAIL)
			{
				g_eT3TState = T3T_SELECTED_IDLE;
				g_bT3TWaitForRsp = false;
			}
			break;
		case T3T_SELECTED_IDLE:
			break;
		}
	}

	// Sending a command
	if(g_bT3TWaitForRsp == false)
	{
		switch(g_eT3TState)
		{
		case T3T_SENSF_REQ:
			NFC_RW_T3T_sendSensFReq(0xFFFF,0x01,0x0F);
			g_bT3TWaitForRsp = true;
			break;
		case T3T_READ_CC:
		case T3T_READ_DATA:
		case T3T_READ_NDEF:
			g_sTxT3TPacket.pui8Blocks = g_pui8Block_ext;
			g_sTxT3TPacket.pui8Services = g_pui8Service_ext;
			g_sTxT3TPacket.ui8BlockLen = 0x01;
			g_sTxT3TPacket.ui8ServiceCount = 0x01;
			g_sTxT3TPacket.bThreeByteBlock = false;

			g_pui8Block_ext[0] = 0x80;
			g_pui8Block_ext[1] = (uint8_t) g_ui16T3TBlockNumber;

			g_pui8Service_ext[0] = 0x0B;
			g_pui8Service_ext[1] = 0x00;

			NFC_RW_T3T_sendCheckCmd(g_sTxT3TPacket);
			g_bT3TWaitForRsp = true;
			break;
		case T3T_WRITE_NDEF:
			g_sTxT3TPacket.pui8Blocks = g_pui8Block_ext;
			g_sTxT3TPacket.pui8Services = g_pui8Service_ext;
			g_sTxT3TPacket.ui8BlockLen = 0x01;
			g_sTxT3TPacket.ui8ServiceCount = 0x01;
			g_sTxT3TPacket.bThreeByteBlock = false;

			if(g_ui16T3TBlockNumber == 0)
			{
				g_sTxT3TPacket.pui8Data = g_pui8T3TTxData;
				g_sTxT3TPacket.ui8DataLen = 16;
			}
			else if(g_ui16T3TNdefLen > 0)
			{
				g_sTxT3TPacket.pui8Data = &g_pui8T3TBuffer[g_ui16T3TNdefIndex];
				g_sTxT3TPacket.ui8DataLen = 16;
			}
			else if(g_ui16T3TNdefLen == 0)
			{
				g_sTxT3TPacket.pui8Data = g_pui8T3TTxData;
				g_sTxT3TPacket.ui8DataLen = 16;

				// Reset the Block Number
				g_ui16T3TBlockNumber = 0;

				// Clear Write Flag
				g_pui8T3TTxData[9] = T3T_WRITING_FLAG_OFF;

				// Clear BCC
				ui16BCC = 0x00;

				// Calculate BCC
				for(ui8Temp = 0; ui8Temp < 14; ui8Temp++)
				{
					ui16BCC = ui16BCC + (uint16_t) g_pui8T3TTxData[ui8Temp];
				}

				// BCC Bytes
				g_pui8T3TTxData[14] = (uint8_t) (ui16BCC >> 8);
				g_pui8T3TTxData[15] = (uint8_t) (ui16BCC & 0xFF);
			}

			g_pui8Block_ext[0] = 0x80;
			g_pui8Block_ext[1] = (uint8_t) g_ui16T3TBlockNumber;

			g_pui8Service_ext[0] = 0x09;
			g_pui8Service_ext[1] = 0x00;

			NFC_RW_T3T_sendUpdateCmd(g_sTxT3TPacket);
			g_bT3TWaitForRsp = true;
			break;
		case T3T_SELECTED_IDLE:
			break;
		}
	}
}

