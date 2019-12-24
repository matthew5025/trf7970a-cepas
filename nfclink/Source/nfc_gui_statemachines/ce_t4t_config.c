//*****************************************************************************
//
// ce_t4t_config.c - Emulated Type 4 Tag Information
//
// Copyright (c) 2015 Texas Instruments Incorporated.  All rights reserved.
// TI Information - Selective Disclosure
//
//*****************************************************************************

#include "ce_t4t_config.h"
#include "iso_7816_4.h"

//*****************************************************************************

uint8_t g_ui8NdefPacketIndex;

// Capability Container File - fixed
uint8_t pui8CCBuffer[23] =
{
//	0x00, 0x17,		// CC Len - 7 (fixed) + #Files * 8 (i.e. 2 files, 7+2*8=23) 2 Files (1 NDEF 1 Proprietary)
	0x00, 0x0F,		// CC Len - 7 (fixed) + #Files * 8 (i.e. 2 files, 7+2*8=23) 1 Files (1 NDEF)
	0x20,			// MAP Ver 2.0
	0x00, 0xFB,		// MLe
	0x00, 0xF9,		// MLc

	0x04,			// T (NDEF File)
	0x06,			// L
	0xE1, 0x04,		// File ID
	0x01, 0xF4,		// Max NDEF - 500 bytes
	0x00,			// R
	0x00,			// W - 0x00 (write capability available), 0xFF (read-only)

	0x05,			// T (Proprietary File)
	0x06,			// L
	0xE1, 0x05,		// File ID
	0x00, 0xFF,		// Max NDEF
	0x00,			// R
	0x00			// W - 0x00 (write capability available), 0xFF (read-only)
};

// NDEF File (read-only) NFC Powered by Texas Instruments
uint8_t pui8NDefBuffer[500] =
{
	    0x00, 0x2E,    // These two bytes are excluded from the File Length count
	    // File Header
	    0xD1,          // NDEF Header
	    0x01,          // Length of the record name
	    0x2A,          // Length of the payload data
	    0x54,          // Binary Encoding of record name - 0x54 (Text RTD)
	    //Payload
	    0x02,          // Status Byte - UTF-8, two byte language code
	    0x65, 0x6E,    // Language Code - English
		0x4E, 0x46, 0x43, 0x20, 0x2D, 0x20, 0x50, 0x6F, 0x77, 0x65,
		0x72, 0x65, 0x64, 0x20, 0x62, 0x79, 0x20, 0x54, 0x65, 0x78,
		0x61, 0x73, 0x20, 0x49, 0x6E, 0x73, 0x74, 0x72, 0x75, 0x6D,
		0x65, 0x6E, 0x74, 0x73, 0x20, 0x49, 0x6E, 0x63, 0x2E
};

// Proprietary File # 1
uint8_t pui8PropBuffer[82] =
{
		0x00, 0x50,
		0xD1, 0x02, 0x4B, 0x53, 0x70, 0x91, 0x01, 0x29, 0x54, 0x02, 0x65, 0x6E,
		0x54, 0x68, 0x65, 0x20, 0x64, 0x65, 0x76, 0x69, 0x63, 0x65, 0x20, 0x72,
		0x65, 0x71, 0x75, 0x69, 0x72, 0x65, 0x73, 0x20, 0x6D, 0x61, 0x69, 0x6E,
		0x74, 0x65, 0x6E, 0x61, 0x6E, 0x63, 0x65, 0x20, 0x23, 0x20, 0x31, 0x32,
		0x38, 0x30, 0x11, 0x01, 0x13, 0x55, 0x01, 0x73, 0x61, 0x6C, 0x65, 0x61,
		0x65, 0x2E, 0x63, 0x6F, 0x6D, 0x2F, 0x73, 0x75, 0x70, 0x70, 0x6F, 0x72,
		0x74, 0x51, 0x03, 0x01, 0x61, 0x63, 0x74, 0x00
};

// NDEF Application ID
uint8_t pui8NdefAppId[7] = {0xD2, 0x76, 0x00, 0x00, 0x85, 0x01, 0x01};

// Test Application # 1 - Example
uint8_t pui8TestAppId[9] = {0xA0, 0x00, 0x00, 0x03, 0x08, 0x00, 0x00, 0x10, 0x00};

// Capability Container File (0xE103)
tType4File sCCFile = {0xE103, pui8CCBuffer, 23,false};

// NDEF File (0xE104)
tType4File sNdefFile = {0xE104, (uint8_t *) pui8NDefBuffer, 500, false};

// Proprietary File # 1 (0xE105)
tType4File sProp1File = {0xE105, pui8PropBuffer, 82,false};

// Buffer including all the Type 4 Files
tType4File psType4Buffer[3]; //= {sCCFile, sNdefFile, sProp1File};

//NDEF application
tType4App sNdefApp = {pui8NdefAppId, 7, psType4Buffer, 3};

tType4App sTestApp = {pui8TestAppId, 9, 0, 0};

tType4App psAppBuffer[2]; //= {sNdefApp, sTestApp};

// Type 4 Application Data Structure including all the applications
tType4AppDS sType4AppDS = {psAppBuffer, 1};  // One application example
//tType4AppDS sType4AppDS = {psAppBuffer, 2};  // Two application example

// NDEF Packet Structure
sNdefPacket g_sNdefPackets[5];

void T4T_CE_init(void)
{
	// Initialize NDEF's files
	psType4Buffer[0] = sCCFile;			// Capability Container
	psType4Buffer[1] = sNdefFile; 		// NDEF File
	psType4Buffer[2] = sProp1File; 		// Proprietary File

	// Initialize Application Buffer
	psAppBuffer[0] = sNdefApp;
	psAppBuffer[1] = sTestApp;

	ISO_7816_4_setAppDS(sType4AppDS);
}

void T4T_CE_initNDEF(void)
{
	g_ui8NdefPacketIndex = 0x00;

	// Text
	g_sNdefPackets[0].ui16PacketLength = 48;
	g_sNdefPackets[0].pui8NdefDataPtr = (uint8_t *) pui8NfcPoweredByTexasInstruments;

	// URI
	g_sNdefPackets[1].ui16PacketLength = 17;
	g_sNdefPackets[1].pui8NdefDataPtr = (uint8_t *) pui8TexasInstrumentsUrl;

	// Smart Poster
	g_sNdefPackets[2].ui16PacketLength = 58;
	g_sNdefPackets[2].pui8NdefDataPtr = (uint8_t *) pui8ExampleSmartPoster;

	// Vcard
	g_sNdefPackets[3].ui16PacketLength = 178;
	g_sNdefPackets[3].pui8NdefDataPtr = (uint8_t*) pui8ExampleVCard;

	// Beats Handover
	g_sNdefPackets[4].ui16PacketLength = 47;
	g_sNdefPackets[4].pui8NdefDataPtr = (uint8_t *) pui8BluetoothHandover;
}

void T4T_CE_switchRotatingNDEF(void)
{
	if(g_ui8NdefPacketIndex == 4)
	{
		g_ui8NdefPacketIndex = 0;
	}
	else
	{
		g_ui8NdefPacketIndex++;
	}

	// Modifiy the File Len and File pointer inside the NDEF file
	psType4Buffer[1].ui16Type4FileLen = g_sNdefPackets[g_ui8NdefPacketIndex].ui16PacketLength;
	psType4Buffer[1].pui8Type4File = g_sNdefPackets[g_ui8NdefPacketIndex].pui8NdefDataPtr;

	// Modify the Capability Container Max NLEN
	psType4Buffer[0].pui8Type4File[11] = (uint8_t) (g_sNdefPackets[g_ui8NdefPacketIndex].ui16PacketLength >> 8);
	psType4Buffer[0].pui8Type4File[12] = (uint8_t) (g_sNdefPackets[g_ui8NdefPacketIndex].ui16PacketLength & 0xFF);
}

void T4T_CE_switchImageNDEF(void)
{
	// Modifiy the File Len and File pointer inside the NDEF file
	psType4Buffer[1].ui16Type4FileLen = 3599;
	psType4Buffer[1].pui8Type4File = (uint8_t *) pui8TiLogo;

	// Modify the Capability Container Max NLEN
	psType4Buffer[0].pui8Type4File[11] = (uint8_t) (3599 >> 8);
	psType4Buffer[0].pui8Type4File[12] = (uint8_t) (3599 & 0xFF);
}

void T4T_CE_updateNDEF(uint8_t * pui8Data, uint16_t ui16NdefLen)
{
	pui8NDefBuffer[0] = (uint8_t) ((ui16NdefLen >> 8) & 0xFF);
	pui8NDefBuffer[1] = (uint8_t) (ui16NdefLen & 0xFF);

	memcpy(&pui8NDefBuffer[2],pui8Data,ui16NdefLen);

	// Modifiy the File Len and File pointer inside the NDEF file
	psType4Buffer[1].ui16Type4FileLen = ui16NdefLen+2;
	psType4Buffer[1].pui8Type4File = (uint8_t *) pui8NDefBuffer;

	// Modify the Capability Container Max NLEN = 500
	psType4Buffer[0].pui8Type4File[11] = 0x01;
	psType4Buffer[0].pui8Type4File[12] = 0xF4;
}
