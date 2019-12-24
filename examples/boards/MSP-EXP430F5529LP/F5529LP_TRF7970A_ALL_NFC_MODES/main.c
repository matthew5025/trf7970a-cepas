//*****************************************************************************
//
// main.c
//
// Copyright (c) 2015 Texas Instruments Incorporated.  All rights reserved.
// TI Information - Selective Disclosure
//
//*****************************************************************************
#include "msp430.h"
#include "nfc_controller.h"

#ifdef MSP430F5529_LAUNCHPAD_ENABLED
#include "lp_buttons.h"
#endif

#ifdef MSP432P401R_LAUNCHPAD_ENABLED
#include "lp_buttons.h"
#endif

#ifdef MSP430F5529_EXP_BOARD_ENABLED
#include "HAL_Buttons.h"
#include "HAL_Dogs102x6.h"
#include "lcd_images.h"
#else
#define DOGS102x6_DRAW_NORMAL 0x00
#define DOGS102x6_DRAW_INVERT 0x01
#endif

#include "tag_header.h"

//
// Buffer to store incoming packets from NFC host
//
uint8_t g_ui8SerialBuffer[265];

//
// Number of bytes received from the host
//
volatile uint16_t g_ui16BytesReceived = 0x00;

bool g_bEnableAutoSDD;
bool g_bExtAmplifier;
bool g_bTRF5VSupply;
tTRF79x0_Version g_eTRFVersion;
bool g_bSupportCertification;
uint16_t g_ui16ListenTime;

#if (NFC_PEER_2_PEER_INITIATOR_ENABLED || NFC_PEER_2_PEER_TARGET_ENABLED)
	t_sNfcP2PMode g_sP2PSupportedModes;
	t_sNfcP2PCommBitrate g_sP2PSupportedTargetBitrates;
	t_sNfcP2PCommBitrate g_sP2PSupportedInitiatorBitrates;
	t_sNfcDEP_P2PSetup g_sP2PSetupOptions;
	uint8_t g_ui8NfcDepInitiatorDID;
#endif

#if NFC_CARD_EMULATION_ENABLED
	t_sNfcCEMode g_sCESupportedModes;
#endif

#if NFC_READER_WRITER_ENABLED
	t_sNfcRWMode g_sRWSupportedModes;
	t_sNfcRWCommBitrate g_sRWSupportedBitrates;
	t_sIsoDEP_RWSetup g_sRWSetupOptions;
	uint8_t g_ui8IsoDepInitiatorDID;
#endif


void NFC_configuration(void);
void Serial_processCommand(void);
void LCD_init(void);
void updateLcdfcStatus(bool bUpdateRssiOnly);

uint8_t g_ui8TxBuffer[256];
uint8_t g_ui8TxLength;

void LCD_stringDraw(uint8_t row, uint8_t col, char *word, uint8_t style)
{
#ifdef MSP430F5529_EXP_LCD_ENABLED
	Dogs102x6_stringDraw(row,col,word,style);
#endif
}

void NFC_emulatedT4TSniffer(void)
{
	uint16_t ui16FileID;
	uint16_t ui16StartIdx;
	uint8_t ui8BufferLength;
	uint8_t ui8T4TStatus;

	char pui8ByteBuffer[3];
	char pui8WordBuffer[5];

	ui8T4TStatus = ISO_7816_4_getCEStatus(&ui16FileID, &ui16StartIdx, &ui8BufferLength);

	if ((ui8T4TStatus == CE_READ_FLAG) && (ui8BufferLength > 1))
	{
		Serial_printf("Read File 0x",CE_FILE_STATUS);

		// Print File ID
		convertWordToAscii(ui16FileID,(uint8_t *)pui8WordBuffer);

		Serial_printf(pui8WordBuffer,CE_FILE_STATUS);

		Serial_printf(" , Idx 0x",CE_FILE_STATUS);

		// Print Index
		convertWordToAscii(ui16StartIdx,(uint8_t *) pui8WordBuffer);

		Serial_printf(pui8WordBuffer,CE_FILE_STATUS);

		Serial_printf(" , Len 0x",CE_FILE_STATUS);

		// Print Len
		convertByteToAscii(ui8BufferLength,(uint8_t *) pui8ByteBuffer);

		Serial_printf(pui8ByteBuffer,CE_FILE_STATUS);

		pui8ByteBuffer[0] = 0x0D;
		pui8ByteBuffer[1] = 0x0A;
		pui8ByteBuffer[2] = 0x00;

		// Send New Line
		Serial_printf(pui8ByteBuffer,CE_FILE_STATUS);
	}
	else if(ui8T4TStatus == CE_WRITE_FLAG)
	{
		Serial_printf("Write  File 0x",CE_FILE_STATUS);

		// Print File ID
		convertWordToAscii(ui16FileID,(uint8_t *)pui8WordBuffer);

		Serial_printf(pui8WordBuffer,CE_FILE_STATUS);

		Serial_printf(" , Idx 0x",CE_FILE_STATUS);

		// Print Index
		convertWordToAscii(ui16StartIdx,(uint8_t *) pui8WordBuffer);

		Serial_printf(pui8WordBuffer,CE_FILE_STATUS);

		Serial_printf(" , Len 0x",CE_FILE_STATUS);

		// Print Len
		convertByteToAscii(ui8BufferLength,(uint8_t *) pui8ByteBuffer);

		Serial_printf(pui8ByteBuffer,CE_FILE_STATUS);

		pui8ByteBuffer[0] = 0x0D;
		pui8ByteBuffer[1] = 0x0A;
		pui8ByteBuffer[2] = 0x00;

		// Send New Line
		Serial_printf(pui8ByteBuffer,CE_FILE_STATUS);
	}
	else
	{
		// Do nothing
	}
}

void NFC_initIDs(void)
{
	// NFC ID's
	uint8_t pui8NfcAId[10] = {0x08,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09};	// Generic ISO14443 T4TA Tag
	uint8_t pui8NfcBId[4] = {0x08,0x0A, 0xBE,0xEF};	// Generic ISO14443 T4TB Tag
	uint8_t pui8NfcFId[8] = {0x01,0xFE,0x88,0x77,0x66,0x55,0x44,0x33};	// Type F ID for P2P

	// Set the NFC Id's for Type A, Type B, and Type F
	NFC_A_setNfcAId(pui8NfcAId,4);
	NFC_B_setNfcBId(pui8NfcBId,4);
	NFC_F_setNfcId2(pui8NfcFId,8);
}

void main(void)
{
	tNfcState eTempNFCState;
	tNfcState eCurrentNFCState;

	// CE Variables
	t_sNfcCEMode sCEMode;

#if (NFC_PEER_2_PEER_INITIATOR_ENABLED || NFC_PEER_2_PEER_TARGET_ENABLED)
	// Peer to Peer TX Variables
	uint32_t ui32PacketRemaining;
	uint8_t ui8TXBytes;
	uint16_t ui16TxIndex;
	uint32_t ui32PacketLength;
	uint8_t * pui8NdefPointer;
	uint8_t ui8FragmentSize;
	char pcBytesReceivedString[5];

	// Bytes Received from Peer to Peer
	uint16_t ui16BytesReceived = 0x00;

	// Peer to peer RX Status
	tNfcP2PRxStatus sP2PRxStatus;
#endif
	t_sNfcP2PMode sP2PMode;
	t_sNfcP2PCommBitrate sP2PBitrate;

	// Reader/Writer RX Status
	t_sNfcRWMode sRWMode;
	t_sNfcRWCommBitrate sRWBitrate;

	// Initialize MCU
	MCU_init();

    // Initialize Debug Pins as output
	NFC_RF_FIELD_LED_DIR |= NFC_RF_FIELD_LED_BIT;
	NFC_HOST_LED_DIR |= NFC_HOST_LED_BIT;
#ifdef MSP430F5529_EXP_BOARD_ENABLED
	NFC_RX_LED_PDIR |= NFC_RX_LED_BIT;
	NFC_TX_LED_PDIR |= NFC_TX_LED_BIT;
#endif
	NFC_RW_LED_PDIR |= NFC_RW_LED_BIT;
	NFC_P2P_LED_PDIR |= NFC_P2P_LED_BIT;
	NFC_CE_LED_PDIR |= NFC_CE_LED_BIT;

	// Clear NFC pins
	NFC_RF_FIELD_LED_POUT &= ~NFC_RF_FIELD_LED_BIT;
	NFC_HOST_LED_POUT &= ~NFC_HOST_LED_BIT;
#ifdef MSP430F5529_EXP_BOARD_ENABLED
	NFC_RX_LED_POUT &= ~NFC_RX_LED_BIT;
	NFC_TX_LED_POUT &= ~NFC_TX_LED_BIT;
#endif
	NFC_RW_LED_POUT &= ~NFC_RW_LED_BIT;
	NFC_P2P_LED_POUT &= ~NFC_P2P_LED_BIT;
	NFC_CE_LED_POUT &= ~NFC_CE_LED_BIT;

	//Enable interrupts globally
	__enable_interrupt();

	// Initialize USB Communication
	Serial_init();

	// Initialize TRF7970
	TRF79x0_init();

	Buttons_init(BUTTON_ALL);
	Buttons_interruptEnable(BUTTON_ALL);

	TRF79x0_idleMode();

	// Initialize the NFC Controller
	NFC_init();

	// This function will configure all the settings for each protocol
	NFC_configuration();

	// Initialize Type 4 Tag RTD Message
	T4T_CE_initNDEF();

	// Initialize IDs for NFC-A, NFC-B and NFC-F
	NFC_initIDs();

	// Enable Card Emulation for NFC-B
    g_sCESupportedModes.bits.bT4TAEnabled = 0;
    g_sCESupportedModes.bits.bT4TBEnabled = 1;
	NFC_CE_configure(g_sCESupportedModes);

#ifdef MSP430F5529_EXP_LCD_ENABLED
	// Initi8alize the LCD and Display default menu
	LCD_init();
#endif

#if NFC_READER_WRITER_ENABLED
	// Initialize the RW T2T, T3T, T4T and T5 state machines
	T2T_init(g_ui8TxBuffer,256);
	T3T_init(g_ui8TxBuffer,256);
	T4T_init(g_ui8TxBuffer,256);
	T5T_init(g_ui8TxBuffer,256);
#endif

	while(1)
	{
		eTempNFCState = NFC_run();

		if(eTempNFCState == NFC_DATA_EXCHANGE_PROTOCOL)
		{
			if(NFC_RW_getModeStatus(&sRWMode,&sRWBitrate))
			{
#if NFC_READER_WRITER_ENABLED
				NFC_RW_LED_POUT |= NFC_RW_LED_BIT;

				if( sRWMode.bits.bNfcA == 1)
				{
					if(NFC_A_getSAK() == 0x00)
					{
						// T2T Tag State Machine
						T2T_stateMachine();
					}
					else if(NFC_A_getSAK() & 0x20)
					{
						// T4T Tag State Machine
						T4T_stateMachine();
					}
				}
				else if(sRWMode.bits.bNfcB == 1)
				{
					if(NFC_B_isISOCompliant())
					{
						// T4T Tag State Machine
						T4T_stateMachine();
					}
				}
				else if(sRWMode.bits.bNfcF == 1)
				{
					// T3T Tag State Machine
					T3T_stateMachine();
				}
				else if(sRWMode.bits.bISO15693 == 1)
				{
					// T5T Tag State Machine
					T5T_stateMachine();
				}
#endif
			}
			else if(NFC_P2P_getModeStatus(&sP2PMode,&sP2PBitrate))
			{
#if (NFC_PEER_2_PEER_INITIATOR_ENABLED || NFC_PEER_2_PEER_TARGET_ENABLED)
				NFC_P2P_LED_POUT |= NFC_P2P_LED_BIT;

				//
				// Read the receive status structure - check if there is a received packet from the Target
				//
				sP2PRxStatus = NFC_P2P_getReceiveState();
				if(sP2PRxStatus.sDataReceivedStatus != RECEIVED_NO_FRAGMENT)
				{

					if(ui16BytesReceived == 0x00)
					{
						// Send NDEF Total Size
						Serial_printBuffer((char *)&sP2PRxStatus.ui32PacketSize,4,P2P_NDEF_TOTAL_SIZE);
					}

					ui16BytesReceived = sP2PRxStatus.ui16DataReceivedLength + ui16BytesReceived;

					convertWordToAscii(ui16BytesReceived,(uint8_t *)pcBytesReceivedString);

					Serial_printBuffer((char *)sP2PRxStatus.pui8RxDataPtr,sP2PRxStatus.ui16DataReceivedLength,P2P_PAYLOAD_DATA);

					Serial_printf(pcBytesReceivedString,P2P_RX_SIZE_DATA);
				#ifdef MSP430F5529_EXP_LCD_ENABLED
					LCD_stringDraw(6,11*6,pcBytesReceivedString,DOGS102x6_DRAW_NORMAL);
				#endif

					// Check if the last packet was received completely
					if((uint16_t) sP2PRxStatus.ui32PacketSize == ui16BytesReceived)
					{
						// Reset Bytes received
						ui16BytesReceived = 0;
					}
				}

				// Check for communication with Host
				if(g_bSerialConnectionEstablished == false)
				{
					// Check for button input
					if ((buttonsPressed & BUTTON_S1) && (buttonDebounce == 2))
					{
						ui16TxIndex = 0x00;
						buttonDebounce = 0x00;
						// Total Length of the packet.
						ui32PacketLength = 46;
						ui32PacketRemaining = ui32PacketLength;
						// Send Text String
						pui8NdefPointer = (uint8_t *) (pui8NfcPoweredByTexasInstruments+2);
						if(ui32PacketRemaining < LLCP_MIU)
						{
							ui8FragmentSize = (uint8_t) ui32PacketRemaining;
						}
						else
						{
							ui8FragmentSize = LLCP_MIU;
						}
						ui8TXBytes = NFC_P2P_sendNdefPacket(pui8NdefPointer,true,ui8FragmentSize,ui32PacketLength);

						if(ui8TXBytes)
						{
							ui32PacketRemaining = ui32PacketRemaining - (uint16_t) (ui8TXBytes);
							ui16TxIndex = ui16TxIndex + (uint16_t) ui8TXBytes;
#ifdef MSP430F5529_EXP_BOARD_ENABLED
							// Toggle TX LED
							NFC_TX_LED_POUT ^= NFC_TX_LED_BIT;
#endif
						}
					}
					else if((buttonsPressed & BUTTON_S2) && (buttonDebounce == 2))
					{
						ui16TxIndex = 0x00;
						buttonDebounce = 0x00;
						// Total Length of the packet.
						ui32PacketLength = 3597;
						ui32PacketRemaining = ui32PacketLength;
						// Send TI Logo
						pui8NdefPointer = (uint8_t *) (pui8TiLogo + 2);
						if(ui32PacketRemaining < LLCP_MIU)
						{
							ui8FragmentSize = (uint8_t) ui32PacketRemaining;
						}
						else
						{
							ui8FragmentSize = LLCP_MIU;
						}
						ui8TXBytes = NFC_P2P_sendNdefPacket(pui8NdefPointer,true,ui8FragmentSize,ui32PacketLength);

						if(ui8TXBytes)
						{
							ui32PacketRemaining = ui32PacketRemaining - (uint16_t) (ui8TXBytes);
							ui16TxIndex = ui16TxIndex + (uint16_t) ui8TXBytes;

#ifdef MSP430F5529_EXP_BOARD_ENABLED
							// Toggle TX LED
							NFC_TX_LED_POUT ^= NFC_TX_LED_BIT;
#endif
						}
					}
					else if(ui32PacketRemaining > 0)
					{
						if(ui32PacketRemaining < LLCP_MIU)
						{
							ui8FragmentSize = (uint8_t) ui32PacketRemaining;
						}
						else
						{
							ui8FragmentSize = LLCP_MIU;
						}
						ui8TXBytes = NFC_P2P_sendNdefPacket((uint8_t *) (pui8NdefPointer+ui16TxIndex),false,ui8FragmentSize,(uint32_t) ui32PacketLength);

						if(ui8TXBytes)
						{
							ui32PacketRemaining = ui32PacketRemaining - (uint16_t) (ui8TXBytes);
							ui16TxIndex = ui16TxIndex + (uint16_t) ui8TXBytes;

#ifdef MSP430F5529_EXP_BOARD_ENABLED
							// Toggle TX LED
							NFC_TX_LED_POUT ^= NFC_TX_LED_BIT;
#endif
						}
					}
					else if(buttonDebounce == 0x00)
					{
						// Enable the button debounce.
						buttonDebounce = 0x01;
					}
					else if (ui32PacketRemaining == 0)
					{
#ifdef MSP430F5529_EXP_BOARD_ENABLED
						// Clear TX LED
						NFC_TX_LED_POUT &= ~NFC_TX_LED_BIT;
#endif
					}
				}
#endif
			}
			else if(NFC_CE_getModeStatus(&sCEMode))
			{
#if NFC_CARD_EMULATION_ENABLED
				NFC_CE_LED_POUT |= NFC_CE_LED_BIT;

				NFC_emulatedT4TSniffer();
#endif
			}

			// Update only RSSI
			updateLcdfcStatus(true);
		}
		else
		{
			// Clear LEDs (RX & TX)
#ifdef MSP430F5529_EXP_BOARD_ENABLED
			NFC_TX_LED_POUT &= ~NFC_TX_LED_BIT;
			NFC_RX_LED_POUT &= ~NFC_RX_LED_BIT;
#endif
		}

		// Update Current State if it has changed.
		if(eCurrentNFCState != eTempNFCState)
		{
			__no_operation();

			if(eCurrentNFCState != NFC_TARGET_WAIT_FOR_ACTIVATION
				&& eCurrentNFCState != NFC_STATE_IDLE
				&& (eTempNFCState == NFC_PROTOCOL_ACTIVATION
					|| eTempNFCState == NFC_DISABLED))
 			{
				eCurrentNFCState = eTempNFCState;

#if NFC_READER_WRITER_ENABLED
				// Initialize the RW T2T, T3T, T4T and T5 state machines
				T2T_init(g_ui8TxBuffer,256);
				T3T_init(g_ui8TxBuffer,256);
				T4T_init(g_ui8TxBuffer,256);
				T5T_init(g_ui8TxBuffer,256);
#endif

#if (NFC_CARD_EMULATION_ENABLED || NFC_PEER_2_PEER_TARGET_ENABLED || NFC_PEER_2_PEER_INITIATOR_ENABLED)
				// Initialize IDs for NFC-A, NFC-B and NFC-F
				NFC_initIDs();
#endif
				// Clear RW, P2P and CE LEDs
				NFC_RW_LED_POUT &= ~NFC_RW_LED_BIT;
				NFC_P2P_LED_POUT &= ~NFC_P2P_LED_BIT;
				NFC_CE_LED_POUT &= ~NFC_CE_LED_BIT;

				buttonDebounce = 1;
#if (NFC_PEER_2_PEER_INITIATOR_ENABLED || NFC_PEER_2_PEER_TARGET_ENABLED)
				ui16BytesReceived = 0;
				ui32PacketRemaining = 0;
#endif
#ifdef MSP430F5529_EXP_LCD_ENABLED
				LCD_stringDraw(3,9*6,"TBD     ",DOGS102x6_DRAW_NORMAL);
				LCD_stringDraw(4,9*6,"        ",DOGS102x6_DRAW_NORMAL);
				LCD_stringDraw(5,9*6,"        ",DOGS102x6_DRAW_NORMAL);
				LCD_stringDraw(6,9*6,"        ",DOGS102x6_DRAW_NORMAL);
				LCD_stringDraw(7,9*6,"        ",DOGS102x6_DRAW_NORMAL);
#endif
				Serial_printf("DC",NFC_MODE_LOST);
			}
			else
			{
				eCurrentNFCState = eTempNFCState;
			}

#if (NFC_PEER_2_PEER_INITIATOR_ENABLED || NFC_PEER_2_PEER_TARGET_ENABLED)
			if(eCurrentNFCState == NFC_DATA_EXCHANGE_PROTOCOL)
			{
				updateLcdfcStatus(false);
#ifdef MSP430F5529_EXP_LCD_ENABLED
				convertWordToAscii(ui16BytesReceived,(uint8_t *)pcBytesReceivedString);
				LCD_stringDraw(6,11*6,pcBytesReceivedString,DOGS102x6_DRAW_NORMAL);
#endif
			}
#endif
		}


#if NFC_CARD_EMULATION_ENABLED
		if(eCurrentNFCState == NFC_PROTOCOL_ACTIVATION)
		{
			// Check for button input
			if ((buttonsPressed & BUTTON_S1) && (buttonDebounce == 2))
			{
				buttonDebounce = 0x00;
				T4T_CE_switchRotatingNDEF();
			}
			else if((buttonsPressed & BUTTON_S2) && (buttonDebounce == 2))
			{
				buttonDebounce = 0x00;
				T4T_CE_switchImageNDEF();
			}
			else if(buttonDebounce == 0x00)
			{
				// Enable the button debounce.
				buttonDebounce = 0x01;
			}
		}
#endif

		// Check if any packets have been received from the NFC host.
		if(g_ui16BytesReceived > 0)
		{
			Serial_processCommand();
		}
	}
}

//*****************************************************************************
//
//! NFC_configuration - Handles the initial NFC configuration.
//!
//!	Setup all NFC Mode Parameters.
//!
//! Current modes enabled: Card Emulation
//! Current modes supported: Card Emulationa and Peer 2 Peer
//! Reader/Writer is NOT supported yet.
//!
//*****************************************************************************

void NFC_configuration(void)
{
#if (NFC_PEER_2_PEER_INITIATOR_ENABLED || NFC_PEER_2_PEER_TARGET_ENABLED)
	g_sP2PSupportedModes.ui8byte = 0x00;
	g_sP2PSupportedTargetBitrates.ui8byte = 0x00;
	g_sP2PSupportedInitiatorBitrates.ui8byte = 0x00;
	g_sP2PSetupOptions.ui8byte = 0x00;
#endif

#if NFC_CARD_EMULATION_ENABLED
	g_sCESupportedModes.ui8byte = 0x00;
#endif

#if NFC_READER_WRITER_ENABLED
	g_sRWSupportedModes.ui8byte = 0x00;
	g_sRWSupportedBitrates.ui16byte = 0x0000;
	g_sRWSetupOptions.ui16byte = 0x0000;
#endif

	// Set the TRF7970 Version being used
	g_eTRFVersion = TRF7970_A;

	// External Amplifer (disconnected by default)
	g_bExtAmplifier = false;

	// Configure TRF External Amplifier for the transceiver
	TRF79x0_setExtAmplifer(g_bExtAmplifier);

	// Configure TRF Power Supply (5V = true, 3V = false)
#ifdef MSP430F5529_EXP_BOARD_ENABLED
	g_bTRF5VSupply = true;
#else
	g_bTRF5VSupply = false;
#endif

	// Configure TRF Power Supply
	TRF79x0_setPowerSupply(g_bTRF5VSupply);

	// Milliseconds the NFC stack will be in listen mode
	g_ui16ListenTime = 500;

	// Set the time the NFC stack will be with the RF field disabled (listen mode)
	NFC_setListenTime(g_ui16ListenTime);

	// Enable (1) or disable (0) the Auto SDD Anti-collision function of the TRF7970A
	g_bEnableAutoSDD = 0;

#if (NFC_PEER_2_PEER_INITIATOR_ENABLED || NFC_PEER_2_PEER_TARGET_ENABLED)

	// Enable Peer 2 Peer Supported Modes
	g_sP2PSupportedModes.bits.bTargetEnabled = 0;
	g_sP2PSupportedModes.bits.bInitiatorEnabled = 0;

	// Set P2P Supported Bit Rates - Target mode
	g_sP2PSupportedTargetBitrates.bits.bPassive106kbps = 0;
	g_sP2PSupportedTargetBitrates.bits.bPassive212kbps = 0;
	g_sP2PSupportedTargetBitrates.bits.bPassive424kbps = 0;
	g_sP2PSupportedTargetBitrates.bits.bActive106kbps = 0;
	g_sP2PSupportedTargetBitrates.bits.bActive212kbps = 0;
	g_sP2PSupportedTargetBitrates.bits.bActive424kbps = 0;

	// Set P2P Supported Bit Rates - Initiator mode
	g_sP2PSupportedInitiatorBitrates.bits.bPassive106kbps = 0;
	g_sP2PSupportedInitiatorBitrates.bits.bPassive212kbps = 0;
	g_sP2PSupportedInitiatorBitrates.bits.bPassive424kbps = 0;
	g_sP2PSupportedInitiatorBitrates.bits.bActive106kbps = 0;
	g_sP2PSupportedInitiatorBitrates.bits.bActive212kbps = 0;
	g_sP2PSupportedInitiatorBitrates.bits.bActive424kbps = 0;

	// Certification Config Start //

	// Enable (1) or disable (0) Wave 1 NFC Forum Certification functionality
	// Note: Enabling this feature can affect interoperability with NFC Devices that are not certified.
	g_bSupportCertification = 0;

	// Allows for Customization of the DID (Device Identification) number when in initiator mode
	g_ui8NfcDepInitiatorDID = 0x00;

	// Enable LLCP
	g_sP2PSetupOptions.bits.bP2PSupportLLCP = 1;

	// Enable Loopback
	g_sP2PSetupOptions.bits.bP2PSupportLoopback = 0;

	// Specify maximum number of timeouts and protocol errors allowed before resetting
	g_sP2PSetupOptions.bits.ui3P2PMaxTimeouts = 2;
	g_sP2PSetupOptions.bits.ui3P2PMaxProtocolErrors = 2;
#endif

#if NFC_CARD_EMULATION_ENABLED

	// Enable Card Emulation Supported Modes
	g_sCESupportedModes.bits.bT4TAEnabled = 1;
	g_sCESupportedModes.bits.bT4TBEnabled = 1;
	
#endif

#if NFC_READER_WRITER_ENABLED
	// Enable Reader Writer Supported Modes
	g_sRWSupportedModes.bits.bNfcA = 0;
	g_sRWSupportedModes.bits.bNfcB = 0;
	g_sRWSupportedModes.bits.bNfcF = 0;
	g_sRWSupportedModes.bits.bISO15693 = 0;

	// NFC-A Bitrates
	g_sRWSupportedBitrates.bits.bNfcA_106kbps = 0;		// Must be enabled if bNfcA is set
	g_sRWSupportedBitrates.bits.bNfcA_212kbps = 0;
	g_sRWSupportedBitrates.bits.bNfcA_424kbps = 0;
	g_sRWSupportedBitrates.bits.bNfcA_848kbps = 0;
	// NFC-B Bitrates
	g_sRWSupportedBitrates.bits.bNfcB_106kbps = 0; 		// Must be enabled if bNfcB is set
	g_sRWSupportedBitrates.bits.bNfcB_212kbps = 0;
	g_sRWSupportedBitrates.bits.bNfcB_424kbps = 0;
	g_sRWSupportedBitrates.bits.bNfcB_848kbps = 0;
	// NFC-F Bitrates
	g_sRWSupportedBitrates.bits.bNfcF_212kbps = 0;
	g_sRWSupportedBitrates.bits.bNfcF_424kbps = 0;
	// ISO15693 Bitrates
	g_sRWSupportedBitrates.bits.bISO15693_26_48kbps = 0;

	// Default Max number of WTX 2
	g_sRWSetupOptions.bits.ui3RWMaxWTX = 2;
	// Default Max number of ACK 2
	g_sRWSetupOptions.bits.ui3RWMaxACK = 2;
	// Default Max number of NACK 2
	g_sRWSetupOptions.bits.ui3RWMaxNACK = 2;
	// Default Max number of DSL 2
	g_sRWSetupOptions.bits.ui3RWMaxDSL = 2;

	g_ui8IsoDepInitiatorDID = 0x00;
#endif

#if (NFC_PEER_2_PEER_INITIATOR_ENABLED || NFC_PEER_2_PEER_TARGET_ENABLED)
	// Configure Peer 2 Peer functions for the correct modes and communication bitrates
	NFC_P2P_configure(g_sP2PSupportedModes,g_sP2PSupportedTargetBitrates,g_sP2PSupportedInitiatorBitrates);

	// Configure NFC DEP functions including passing the DID
	NFCDEP_configure_P2P(g_sP2PSetupOptions,g_bSupportCertification,g_ui8NfcDepInitiatorDID);
#endif

#if NFC_CARD_EMULATION_ENABLED
	NFC_CE_configure(g_sCESupportedModes);

	T4T_CE_init();
#endif


#if NFC_READER_WRITER_ENABLED
	if (g_sRWSupportedModes.ui8byte != 0x00)
	{
		// To be made shortly
		NFC_RW_configure(g_sRWSupportedModes,g_sRWSupportedBitrates);
	}
	ISODEP_configure_RW(g_sRWSetupOptions,g_ui8IsoDepInitiatorDID);
#endif

	// Set the Auto SDD flag within nfc_a.c
	NFC_A_setAutoSDD(g_bEnableAutoSDD);

	// Set the current TRF version within trf79x0.c
	TRF79x0_setVersion(g_eTRFVersion);

	// Set Certification Support for all Protocols - Required for NFC Forum Certification
	NFC_setSupportCertification(g_bSupportCertification);

	// Set Test Enable flag within trf79x0.c - Required for NFC Forum Certification
	TRF79x0_testFlag(g_bSupportCertification);

}

//*****************************************************************************
//
//! Serial_processCommand - Process incoming commands from NFC host.
//!
//! Checks for the SOF (0xFE) from the host, and processes the commands. The
//! possible commands are:
//! 	COMMUNICATION_START - NFC Host connected
//! 	START_P2P_CMD - Enable the P2P modes included in the command
//! 	STOP_P2P_CMD - Disable the P2P stack.
//! 	ACK_CMD - Not currently used
//! 	NDEF_PAYLOAD - NDEF data to send via the P2P stack.
//!		COMMUNICATION_END - Disconnect form the NFC Host.
//!
//! \return None.
//
//*****************************************************************************
void Serial_processCommand(void)
{

	tNFCControllerCommands eHostCommand;
#if (NFC_PEER_2_PEER_INITIATOR_ENABLED || NFC_PEER_2_PEER_TARGET_ENABLED)
	uint32_t ui32NdefLength;
	uint8_t ui8FragmentLength;
	uint16_t ui32PacketIndex;
#endif
#if NFC_READER_WRITER_ENABLED
	t_sNfcRWMode sRWMode;
	t_sNfcRWCommBitrate sRWBitrate;
#endif
	uint8_t ui8CurrentConfiguration[19];

	// When SOF and length are received, but still missing data
	if(g_ui16BytesReceived > 2 && ((g_ui8SerialBuffer[2] + 3) > g_ui16BytesReceived))
	{
		// Wait until full packet has been received
	}
	// Waiting for Length Byte
	else if(g_ui16BytesReceived < 3)
	{
		// Wait until Length Byte received
	}
	else if(g_ui8SerialBuffer[2] + 3 == g_ui16BytesReceived)	// Length
	{
		eHostCommand = (tNFCControllerCommands) g_ui8SerialBuffer[1];
		if(eHostCommand == COMM_START && g_ui16BytesReceived == 3)
		{
			// Initialize the Current Configuration variables to zero
			//
			memset(ui8CurrentConfiguration,0x00,19);

			// Turn on LED
			g_bSerialConnectionEstablished = true;
			NFC_HOST_LED_POUT |= NFC_HOST_LED_BIT;
			Serial_printf(NFC_FW_VERSION,FW_VERSION_CMD);

#if NFC_PEER_2_PEER_TARGET_ENABLED
			ui8CurrentConfiguration[0] |= P2P_TARGET_FW_ENABLED;
			ui8CurrentConfiguration[1] |= g_sP2PSupportedModes.bits.bTargetEnabled;
			ui8CurrentConfiguration[1] |= (g_sP2PSupportedTargetBitrates.ui8byte << 1);
#endif

#if NFC_PEER_2_PEER_INITIATOR_ENABLED
			ui8CurrentConfiguration[0] |= P2P_INITIATOR_FW_ENABLED;
			ui8CurrentConfiguration[2] |= g_sP2PSupportedModes.bits.bInitiatorEnabled;
			ui8CurrentConfiguration[2] |= (g_sP2PSupportedInitiatorBitrates.ui8byte << 1);
#endif

#if NFC_CARD_EMULATION_ENABLED
			ui8CurrentConfiguration[0] |= CARD_EMULATION_FW_ENABLED;
			ui8CurrentConfiguration[3] |= g_sCESupportedModes.ui8byte;
#endif

#if NFC_READER_WRITER_ENABLED
			ui8CurrentConfiguration[0] |= READER_WRITER_FW_ENABLED;
			ui8CurrentConfiguration[4] |= g_sRWSupportedModes.ui8byte;
			ui8CurrentConfiguration[5] |= (uint8_t) (g_sRWSupportedBitrates.ui16byte & 0xFF);
			ui8CurrentConfiguration[6] |= (uint8_t) ((g_sRWSupportedBitrates.ui16byte >> 8) & 0xFF);
#endif

			ui8CurrentConfiguration[7] = (uint8_t) (g_ui16ListenTime & 0xFF);
			ui8CurrentConfiguration[8] = (uint8_t) ((g_ui16ListenTime >> 8) & 0xFF);

			ui8CurrentConfiguration[9] = g_bSupportCertification;

#if (NFC_PEER_2_PEER_INITIATOR_ENABLED || NFC_PEER_2_PEER_TARGET_ENABLED)
			ui8CurrentConfiguration[10] = g_sP2PSetupOptions.ui8byte;

			ui8CurrentConfiguration[11] = g_ui8NfcDepInitiatorDID;
#endif

#if NFC_READER_WRITER_ENABLED
			ui8CurrentConfiguration[12] = (uint8_t) (g_sRWSetupOptions.ui16byte & 0xFF);

			ui8CurrentConfiguration[13] = (uint8_t) ((g_sRWSetupOptions.ui16byte >> 8) & 0xFF);

			ui8CurrentConfiguration[14] = g_ui8IsoDepInitiatorDID;
#endif

			ui8CurrentConfiguration[15] = g_bEnableAutoSDD;

			ui8CurrentConfiguration[16] = g_bExtAmplifier;

			ui8CurrentConfiguration[17] = g_bTRF5VSupply;

			ui8CurrentConfiguration[18] = (uint8_t) g_eTRFVersion;

			Serial_printBuffer((char *)ui8CurrentConfiguration,19,NFC_CONFIGURATION);

		}
		else if(eHostCommand == COMM_END && g_ui16BytesReceived == 3)
		{
			// Turn off LED
			NFC_HOST_LED_POUT &= ~NFC_HOST_LED_BIT;
			g_bSerialConnectionEstablished = false;
		}
		else if(eHostCommand == NFC_TEST_CONFIG && g_ui16BytesReceived == 9)
		{
			// NFC Certification
			g_bSupportCertification = g_ui8SerialBuffer[3];

			// Set Certification Support for all Protocols - Required for NFC Forum Certification
			NFC_setSupportCertification(g_bSupportCertification);

			// Set Test Enable flag within trf79x0.c - Required for NFC Forum Certification
			TRF79x0_testFlag(g_bSupportCertification);

#if (NFC_PEER_2_PEER_INITIATOR_ENABLED || NFC_PEER_2_PEER_TARGET_ENABLED)
			// P2P Options
			g_sP2PSetupOptions.ui8byte = g_ui8SerialBuffer[4];

			// NFC DEP DID
			g_ui8NfcDepInitiatorDID = g_ui8SerialBuffer[5];

			// Configure NFC DEP functions including passing the DID
			NFCDEP_configure_P2P(g_sP2PSetupOptions,g_bSupportCertification,g_ui8NfcDepInitiatorDID);
#endif

 #if NFC_READER_WRITER_ENABLED
			// RW Options
			g_sRWSetupOptions.ui16byte =  (uint16_t) (g_ui8SerialBuffer[6]) + ((uint16_t) g_ui8SerialBuffer[7] << 8);

			// ISO DEP DID
			g_ui8IsoDepInitiatorDID = g_ui8SerialBuffer[8];

			ISODEP_configure_RW(g_sRWSetupOptions,g_ui8IsoDepInitiatorDID);
#endif
		}
		else if(eHostCommand == TRF_SETTINGS && g_ui16BytesReceived == 9)
		{
			// TRF Version Number
			g_eTRFVersion = (tTRF79x0_Version) g_ui8SerialBuffer[3];

			// Set the current TRF version within trf79x0.c
			TRF79x0_setVersion(g_eTRFVersion);

			// Listen Time
			g_ui16ListenTime = (uint16_t) (g_ui8SerialBuffer[4]) + ((uint16_t) g_ui8SerialBuffer[5] << 8);

			// Set the time the NFC stack will be with the RF field disabled (listen mode)
			NFC_setListenTime(g_ui16ListenTime);

			// 5V Power Supply
			g_bTRF5VSupply = g_ui8SerialBuffer[6];

			// Configure TRF Power Supply
			TRF79x0_setPowerSupply(g_bTRF5VSupply);

			// External Amplifier
			g_bExtAmplifier = g_ui8SerialBuffer[7];

			// Configure TRF External Amplifier for the transceiver
			TRF79x0_setExtAmplifer(g_bExtAmplifier);

			// Auto-SDD Setting
			g_bEnableAutoSDD = g_ui8SerialBuffer[8];

			// Set the Auto SDD flag within nfc_a.c
			NFC_A_setAutoSDD(g_bEnableAutoSDD);
		}
#if (NFC_PEER_2_PEER_INITIATOR_ENABLED || NFC_PEER_2_PEER_TARGET_ENABLED)
		else if(eHostCommand == P2P_START_CMD && g_ui16BytesReceived == 6)
		{
			g_sP2PSupportedTargetBitrates.ui8byte = g_ui8SerialBuffer[3];
			g_sP2PSupportedInitiatorBitrates.ui8byte = g_ui8SerialBuffer[4];
			g_sP2PSupportedModes.ui8byte = g_ui8SerialBuffer[5];

			NFC_P2P_disable();
			NFC_P2P_configure(g_sP2PSupportedModes,g_sP2PSupportedTargetBitrates,g_sP2PSupportedInitiatorBitrates);
		}
		else if(eHostCommand == P2P_STOP_CMD && g_ui16BytesReceived == 3)
		{
			NFC_P2P_disable();
			g_sP2PSupportedModes.ui8byte = 0x00;
		}
		else if(eHostCommand == P2P_PUSH_PAYLOAD)
		{
			// Check if is a short record
			if(g_ui8SerialBuffer[3] & 0x10)
			{
				ui32NdefLength = (uint32_t) g_ui8SerialBuffer[2];
				ui32PacketIndex = 0;
				ui8FragmentLength = SNEP_getTxBufferStatus();

				if((uint32_t) ui8FragmentLength > ui32NdefLength)
				{
					ui8FragmentLength = (uint8_t) ui32NdefLength;
				}

				SNEP_setupPacket(&g_ui8SerialBuffer[3], ui32NdefLength,ui8FragmentLength);
#ifdef MSP430F5529_EXP_BOARD_ENABLED
				// Toggle Heart Beat LED (TX)
				NFC_TX_LED_POUT ^= NFC_TX_LED_BIT;
#endif
			}

			ui32PacketIndex = ui32PacketIndex + ui8FragmentLength;
			ui32NdefLength =  ui32NdefLength - ui8FragmentLength;
		}
#endif
#if (NFC_CARD_EMULATION_ENABLED)
		else if(eHostCommand == CE_START_CMD && g_ui16BytesReceived == 4)
		{
			g_sCESupportedModes.ui8byte = g_ui8SerialBuffer[3];
			NFC_CE_configure(g_sCESupportedModes);
		}
		else if(eHostCommand == CE_STOP_CMD && g_ui16BytesReceived == 3)
		{
			NFC_CE_disable();
			g_sCESupportedModes.ui8byte = 0x00;
		}
		else if(eHostCommand == CE_NDEF_CONFIG)
		{
			T4T_CE_updateNDEF(&g_ui8SerialBuffer[3],(uint16_t) (g_ui16BytesReceived-3));
		}
#endif
#if (NFC_READER_WRITER_ENABLED)
		else if(eHostCommand == RW_START_CMD && g_ui16BytesReceived == 6)
		{
			g_sRWSupportedBitrates.ui16byte = (g_ui8SerialBuffer[4] << 8) + g_ui8SerialBuffer[3];
			g_sRWSupportedModes.ui8byte = g_ui8SerialBuffer[5];

			NFC_RW_configure(g_sRWSupportedModes,g_sRWSupportedBitrates);
		}
		else if(eHostCommand == RW_STOP_CMD && g_ui16BytesReceived == 3)
		{
			NFC_RW_disable();
			g_sRWSupportedModes.ui8byte = 0x00;
		}
		else if(eHostCommand == RW_WRITE_TAG)
		{
			if(NFC_RW_getModeStatus(&sRWMode,&sRWBitrate))
			{
				if( sRWMode.bits.bNfcA == 1)
				{
					if(NFC_A_getSAK() == 0x00)
					{
						// T2T Tag Write State Machine
						T2T_writeNDEF(&g_ui8SerialBuffer[3],(uint16_t) (g_ui16BytesReceived-3));
					}
					else if(NFC_A_getSAK() & 0x20)
					{
						// T4T Tag Write State Machine
						T4T_writeNDEF(&g_ui8SerialBuffer[3],(uint16_t) (g_ui16BytesReceived-3));
					}
				}
				else if(sRWMode.bits.bNfcB == 1)
				{
					if(NFC_B_isISOCompliant())
					{
						// T4T Tag Write State Machine
						T4T_writeNDEF(&g_ui8SerialBuffer[3],(uint16_t) (g_ui16BytesReceived-3));
					}
				}
				else if(sRWMode.bits.bNfcF == 1)
				{
					// T3T Tag State Machine
					T3T_writeNDEF(&g_ui8SerialBuffer[3],(uint16_t) (g_ui16BytesReceived-3));
				}
				else if(sRWMode.bits.bISO15693 == 1)
				{
					// T5T Tag State Machine
					T5T_writeNDEF(&g_ui8SerialBuffer[3],(uint16_t) (g_ui16BytesReceived-3));
				}
			}
		}
		else if (eHostCommand == RW_FORMAT_TAG)
		{
			if(NFC_RW_getModeStatus(&sRWMode,&sRWBitrate))
			{
				if(sRWMode.bits.bISO15693 == 1)
				{
					T5T_formatTag();
				}
				else
				{
					Serial_printf("Cannot Format non-T5T Tags! \n",RW_STATUS_DATA);
				}
			}

		}
#endif
		g_ui16BytesReceived = 0;
	}
	else
	{
		// Partial Command Received
		g_ui16BytesReceived = 0x00;
	}
}


//*****************************************************************************
//
//! LCD_init - Initialize the LCD drivers and display CE code status.
//!
//! Initialize LCD - Initialize the LCD drivers, display TI logo for 1 second,
//! then display the NFC logo for 1 second, and display the version of the CE
//! code.
//!
//! \return None.
//
//*****************************************************************************
void LCD_init(void)
{
#ifdef MSP430F5529_EXP_LCD_ENABLED
	// Set up LCD
	Dogs102x6_init();
	Dogs102x6_backlightInit();

	Dogs102x6_setBacklight(3);
	Dogs102x6_setContrast(11);
	Dogs102x6_clearScreen();

	// Display TI logo
	Dogs102x6_imageDraw(tiBug, 0, 16);

	MCU_delayMillisecond(1000);

	// NFC logo
	Dogs102x6_imageDraw(nfc66x66, 0, 16);

	MCU_delayMillisecond(1000);

	Dogs102x6_clearScreen();

	// NFC Logo
	Dogs102x6_imageDraw(nfc25x25, 0, 76);

	// Display NFC Demo Info
	LCD_stringDraw(0,0,"TRF7970A NFC",DOGS102x6_DRAW_INVERT);

	LCD_stringDraw(1,0,"P2P CE RW   ",DOGS102x6_DRAW_INVERT);

	LCD_stringDraw(2,0,"Ver: ",DOGS102x6_DRAW_INVERT);
	LCD_stringDraw(2,4*6,NFC_FW_VERSION,DOGS102x6_DRAW_INVERT);

	LCD_stringDraw(3,0,"Mode   :         ",DOGS102x6_DRAW_NORMAL);

	LCD_stringDraw(4,0,"Tech   :         ",DOGS102x6_DRAW_NORMAL);
	LCD_stringDraw(5,0,"BITR   :         ",DOGS102x6_DRAW_NORMAL);
	LCD_stringDraw(6,0,"RX     :         ",DOGS102x6_DRAW_NORMAL);
	LCD_stringDraw(7,0,"RSSI   :         ",DOGS102x6_DRAW_NORMAL);
#endif
}


//*****************************************************************************
//
//! updateLcdfcStatus - Update the current P2P mode information.
//!
//! \param bUpdateRssiOnly defines if the function should update the RSSI value
//! only. When it is set to false, the P2P mode, bitrate, and number of bytes
//! received are also updated on the LCD.
//!
//! Update the activated P2P mode, bitrate, and number of bytes received.
//!
//! \return None.
//
//*****************************************************************************
void updateLcdfcStatus(bool bUpdateRssiOnly)
{
	static uint8_t ui8RssiValue = 0x00;
	char pui8Buffer[3];
	char pcUIDString[30];
	tTRF79x0_Status eTrfStatus;

	uint8_t * pcDataBuffer;
	uint8_t ui8UIDLen;
	uint8_t ui8Temp;

	eTrfStatus = TRF79x0_getStatus();

	if(bUpdateRssiOnly == false)
	{
		// Reset RSSI Value when the other parameters are updated
		ui8RssiValue = 0x00;

		if(eTrfStatus.eCurrentMode == TARGET_MODE )
		{
			if(eTrfStatus.sTargetMode.bits.bPassiveTypeA == 1)
			{
				if(eTrfStatus.bNfcModeEnabled == true)
				{
#ifdef MSP430F5529_EXP_LCD_ENABLED
					LCD_stringDraw(3,9*6,"Target  ",DOGS102x6_DRAW_NORMAL);
#endif
					Serial_printf("Target",P2P_MODE_DATA);
					Serial_printf("Passive A",P2P_TECH_DATA);;
				}
				else
				{
#ifdef MSP430F5529_EXP_LCD_ENABLED
					LCD_stringDraw(3,9*6,"CE      ",DOGS102x6_DRAW_NORMAL);
#endif

					Serial_printf("T4T",CE_TYPE_DATA);
					Serial_printf("NFC-A",CE_TECH_DATA);
					Serial_printf("106 kbps",CE_BITR_DATA);

					// Get NFC A ID Length
					NFC_A_getNfcAId(&pcDataBuffer,&ui8UIDLen);

					for(ui8Temp=0;ui8Temp<ui8UIDLen;ui8Temp++)
					{
						// Convert hex values to ascii
						convertByteToAscii(pcDataBuffer[ui8Temp],(uint8_t *) pui8Buffer);

						pcUIDString[ui8Temp*2] = pui8Buffer[0];
						pcUIDString[(ui8Temp*2)+1] = pui8Buffer[1];
					}
					// Set last byte has null character
					pcUIDString[ui8UIDLen*2] = 0x00;

					Serial_printf(pcUIDString,CE_UID_DATA);
				}

#ifdef MSP430F5529_EXP_LCD_ENABLED
				LCD_stringDraw(4,9*6,"PASS A  ",DOGS102x6_DRAW_NORMAL);
#endif
			}
			else if(eTrfStatus.sTargetMode.bits.bPassiveTypeB == 1)
			{
#ifdef MSP430F5529_EXP_LCD_ENABLED
				LCD_stringDraw(3,9*6,"CE      ",DOGS102x6_DRAW_NORMAL);
				LCD_stringDraw(4,9*6,"PASS B  ",DOGS102x6_DRAW_NORMAL);
#endif

				Serial_printf("T4T",CE_TYPE_DATA);
				Serial_printf("NFC-B",CE_TECH_DATA);
				Serial_printf("106 kbps",CE_BITR_DATA);

				// Get NFC B ID Length
				NFC_B_getNfcBId(&pcDataBuffer, &ui8UIDLen);

				for(ui8Temp=0;ui8Temp<ui8UIDLen;ui8Temp++)
				{
					// Convert hex values to ascii
					convertByteToAscii(pcDataBuffer[ui8Temp],(uint8_t *) pui8Buffer);

					pcUIDString[ui8Temp*2] = pui8Buffer[0];
					pcUIDString[(ui8Temp*2)+1] = pui8Buffer[1];
				}
				// Set last byte has null character
				pcUIDString[ui8UIDLen*2] = 0x00;

				Serial_printf(pcUIDString,CE_UID_DATA);
			}
			else if(eTrfStatus.sTargetMode.bits.bPassiveTypeF == 1 && eTrfStatus.bNfcModeEnabled == true)
			{
#ifdef MSP430F5529_EXP_LCD_ENABLED
				LCD_stringDraw(3,9*6,"Target  ",DOGS102x6_DRAW_NORMAL);
				LCD_stringDraw(4,9*6,"PASS F  ",DOGS102x6_DRAW_NORMAL);
#endif

				Serial_printf("Target",P2P_MODE_DATA);
				Serial_printf("Passive F",P2P_TECH_DATA);;
			}
			else if(eTrfStatus.sTargetMode.bits.bActiveTypeA == 1 && eTrfStatus.bNfcModeEnabled == true)
			{
#ifdef MSP430F5529_EXP_LCD_ENABLED
				LCD_stringDraw(3,9*6,"Target  ",DOGS102x6_DRAW_NORMAL);
				LCD_stringDraw(4,9*6,"ACTIVE A",DOGS102x6_DRAW_NORMAL);
#endif

				Serial_printf("Target",P2P_MODE_DATA);
				Serial_printf("Active A",P2P_TECH_DATA);;
			}
			else if(eTrfStatus.sTargetMode.bits.bActiveTypeF == 1 && eTrfStatus.bNfcModeEnabled == true)
			{
#ifdef MSP430F5529_EXP_LCD_ENABLED
				LCD_stringDraw(3,9*6,"Target  ",DOGS102x6_DRAW_NORMAL);
				LCD_stringDraw(4,9*6,"ACTIVE F",DOGS102x6_DRAW_NORMAL);
#endif

				Serial_printf("Target",P2P_MODE_DATA);
				Serial_printf("Active F",P2P_TECH_DATA);;
			}

			if(eTrfStatus.sTargetPayloadFrequency.bits.b106kpbs == 1)
			{
				if(eTrfStatus.bNfcModeEnabled == true)
				{
					Serial_printf("106 kbps",P2P_BITR_DATA);
				}
#ifdef MSP430F5529_EXP_LCD_ENABLED
				LCD_stringDraw(5,9*6,"106 kbps",DOGS102x6_DRAW_NORMAL);
#endif
			}
			else if(eTrfStatus.sTargetPayloadFrequency.bits.b212kpbs == 1)
			{
				if(eTrfStatus.bNfcModeEnabled == true)
				{
					Serial_printf("212 kbps",P2P_BITR_DATA);
				}
#ifdef MSP430F5529_EXP_LCD_ENABLED
				LCD_stringDraw(5,9*6,"212 kbps",DOGS102x6_DRAW_NORMAL);
#endif
			}
			else if(eTrfStatus.sTargetPayloadFrequency.bits.b424kpbs == 1)
			{
				if(eTrfStatus.bNfcModeEnabled == true)
				{
					Serial_printf("424 kbps",P2P_BITR_DATA);
				}
#ifdef MSP430F5529_EXP_LCD_ENABLED
				LCD_stringDraw(5,9*6,"424 kbps",DOGS102x6_DRAW_NORMAL);
#endif
			}
		}
		else if(eTrfStatus.eCurrentMode  == INITIATOR_MODE)
		{
			if(eTrfStatus.sInitiatorMode.bits.bPassiveTypeA == 1)
			{

				if(eTrfStatus.bNfcModeEnabled == true)
				{
#ifdef MSP430F5529_EXP_LCD_ENABLED
					LCD_stringDraw(3,9*6,"Init.   ",DOGS102x6_DRAW_NORMAL);
					LCD_stringDraw(4,9*6,"PASS A  ",DOGS102x6_DRAW_NORMAL);
#endif

					Serial_printf("Initiator",P2P_MODE_DATA);
					Serial_printf("Passive A",P2P_TECH_DATA);;
				}
				else
				{
					Serial_printf("NFC-A",RW_TECH_DATA);

					// Get NFC A ID Length
					NFC_A_getNfcAId(&pcDataBuffer,&ui8UIDLen);

					for(ui8Temp=0;ui8Temp<ui8UIDLen;ui8Temp++)
					{
						// Convert hex values to ascii
						convertByteToAscii(pcDataBuffer[ui8Temp],(uint8_t *) pui8Buffer);

						pcUIDString[ui8Temp*2] = pui8Buffer[0];
						pcUIDString[(ui8Temp*2)+1] = pui8Buffer[1];
					}
					// Set last byte has null character
					pcUIDString[ui8UIDLen*2] = 0x00;

					// Read SAK Value
					ui8Temp = NFC_A_getSAK();

					if(ui8Temp & 0x20)
					{
						Serial_printf("T4T",RW_TYPE_DATA);
					}
					else
					{
						Serial_printf("T2T",RW_TYPE_DATA);
					}

					Serial_printf(pcUIDString,RW_UID_DATA);

#ifdef MSP430F5529_EXP_LCD_ENABLED
					LCD_stringDraw(3,9*6,"R/W     ",DOGS102x6_DRAW_NORMAL);
					LCD_stringDraw(4,9*6,"PASS A  ",DOGS102x6_DRAW_NORMAL);
#endif
				}
			}
			else if(eTrfStatus.sInitiatorMode.bits.bPassiveTypeB == 1)
			{
				Serial_printf("NFC-B",RW_TECH_DATA);

				// Get NFC B ID Length
				NFC_B_getNfcBId(&pcDataBuffer, &ui8UIDLen);

				for(ui8Temp=0;ui8Temp<ui8UIDLen;ui8Temp++)
				{
					// Convert hex values to ascii
					convertByteToAscii(pcDataBuffer[ui8Temp],(uint8_t *) pui8Buffer);

					pcUIDString[ui8Temp*2] = pui8Buffer[0];
					pcUIDString[(ui8Temp*2)+1] = pui8Buffer[1];
				}
				// Set last byte has null character
				pcUIDString[ui8UIDLen*2] = 0x00;

				Serial_printf("T4T",RW_TYPE_DATA);

				Serial_printf(pcUIDString,RW_UID_DATA);

#ifdef MSP430F5529_EXP_LCD_ENABLED
				LCD_stringDraw(3,9*6,"R/W     ",DOGS102x6_DRAW_NORMAL);
				LCD_stringDraw(4,9*6,"PASS B  ",DOGS102x6_DRAW_NORMAL);
#endif
			}
			else if(eTrfStatus.sInitiatorMode.bits.bPassiveTypeF == 1)
			{
				if(eTrfStatus.bNfcModeEnabled == true)
				{
#ifdef MSP430F5529_EXP_LCD_ENABLED
					LCD_stringDraw(3,9*6,"Init.   ",DOGS102x6_DRAW_NORMAL);
					LCD_stringDraw(4,9*6,"PASS F  ",DOGS102x6_DRAW_NORMAL);
#endif

					Serial_printf("Initiator",P2P_MODE_DATA);
					Serial_printf("Passive F",P2P_TECH_DATA);;
				}
				else
				{
					Serial_printf("NFC-F",RW_TECH_DATA);

					// Get NFC F ID Length
					NFC_F_getNFCID2(&pcDataBuffer, &ui8UIDLen);

					for(ui8Temp=0;ui8Temp<ui8UIDLen;ui8Temp++)
					{
						// Convert hex values to ascii
						convertByteToAscii(pcDataBuffer[ui8Temp],(uint8_t *) pui8Buffer);

						pcUIDString[ui8Temp*2] = pui8Buffer[0];
						pcUIDString[(ui8Temp*2)+1] = pui8Buffer[1];
					}
					// Set last byte has null character
					pcUIDString[ui8UIDLen*2] = 0x00;

					Serial_printf("T3T",RW_TYPE_DATA);

					Serial_printf(pcUIDString,RW_UID_DATA);

#ifdef MSP430F5529_EXP_LCD_ENABLED
					LCD_stringDraw(3,9*6,"R/W     ",DOGS102x6_DRAW_NORMAL);
					LCD_stringDraw(4,9*6,"PASS F  ",DOGS102x6_DRAW_NORMAL);
#endif
				}
			}
			else if(eTrfStatus.sInitiatorMode.bits.bActiveTypeA == 1 && eTrfStatus.bNfcModeEnabled == true)
			{
#ifdef MSP430F5529_EXP_LCD_ENABLED
				LCD_stringDraw(3,9*6,"Init.   ",DOGS102x6_DRAW_NORMAL);
				LCD_stringDraw(4,9*6,"ACTIVE A",DOGS102x6_DRAW_NORMAL);
#endif

				Serial_printf("Initiator",P2P_MODE_DATA);
				Serial_printf("Active A",P2P_TECH_DATA);;
			}
			else if(eTrfStatus.sInitiatorMode.bits.bActiveTypeF == 1 && eTrfStatus.bNfcModeEnabled == true)
			{
#ifdef MSP430F5529_EXP_LCD_ENABLED
				LCD_stringDraw(3,9*6,"Init.   ",DOGS102x6_DRAW_NORMAL);
				LCD_stringDraw(4,9*6,"ACTIVE F",DOGS102x6_DRAW_NORMAL);
#endif

				Serial_printf("Initiator",P2P_MODE_DATA);
				Serial_printf("Active F",P2P_TECH_DATA);;
			}
			else if(eTrfStatus.sInitiatorMode.bits.bPassive15693 == 1)
			{
				Serial_printf("NFC-V",RW_TECH_DATA);

				// Get NFC V ID Length
				NFC_RW_T5T_getT5TUID(&pcDataBuffer,&ui8UIDLen);

				for(ui8Temp=ui8UIDLen;ui8Temp>0;ui8Temp--)
				{
					// Convert hex values to ascii
					convertByteToAscii(pcDataBuffer[ui8Temp-1],(uint8_t *) pui8Buffer);

					pcUIDString[(ui8UIDLen-ui8Temp)*2] = pui8Buffer[0];
					pcUIDString[((ui8UIDLen-ui8Temp)*2)+1] = pui8Buffer[1];
				}
				// Set last byte has null character
				pcUIDString[ui8UIDLen*2] = 0x00;

				Serial_printf("T5T",RW_TYPE_DATA);

				Serial_printf(pcUIDString,RW_UID_DATA);

#ifdef MSP430F5529_EXP_LCD_ENABLED
				LCD_stringDraw(3,9*6,"R/W     ",DOGS102x6_DRAW_NORMAL);
				LCD_stringDraw(4,9*6,"15693   ",DOGS102x6_DRAW_NORMAL);
#endif
			}

			if(eTrfStatus.sInitiatorPayloadFrequency.bits.b26_48_kbps_1_out_4 == 1)
			{
#ifdef MSP430F5529_EXP_LCD_ENABLED
				LCD_stringDraw(5,9*6,"26.5kbps",DOGS102x6_DRAW_NORMAL);
#endif

				Serial_printf("26.48 kbps",RW_BITR_DATA);
			}
			else if(eTrfStatus.sInitiatorPayloadFrequency.bits.b106kpbs == 1)
			{
				if(eTrfStatus.bNfcModeEnabled == true)
				{
					Serial_printf("106 kbps",P2P_BITR_DATA);
				}
				else
				{
					Serial_printf("106 kbps",RW_BITR_DATA);
				}
#ifdef MSP430F5529_EXP_LCD_ENABLED
				LCD_stringDraw(5,9*6,"106 kbps",DOGS102x6_DRAW_NORMAL);
#endif
			}
			else if(eTrfStatus.sInitiatorPayloadFrequency.bits.b212kpbs == 1)
			{
				if(eTrfStatus.bNfcModeEnabled == true)
				{
					Serial_printf("212 kbps",P2P_BITR_DATA);
				}
				else
				{
					Serial_printf("212 kbps",RW_BITR_DATA);
				}
#ifdef MSP430F5529_EXP_LCD_ENABLED
				LCD_stringDraw(5,9*6,"212 kbps",DOGS102x6_DRAW_NORMAL);
#endif
			}
			else if(eTrfStatus.sInitiatorPayloadFrequency.bits.b424kpbs == 1)
			{
				if(eTrfStatus.bNfcModeEnabled == true)
				{
					Serial_printf("424 kbps",P2P_BITR_DATA);
				}
				else
				{
					Serial_printf("424 kbps",RW_BITR_DATA);
				}
#ifdef MSP430F5529_EXP_LCD_ENABLED
				LCD_stringDraw(5,9*6,"424 kbps",DOGS102x6_DRAW_NORMAL);
#endif
			}
			else if(eTrfStatus.sInitiatorPayloadFrequency.bits.b848kpbs == 1)
			{
#ifdef MSP430F5529_EXP_LCD_ENABLED
				LCD_stringDraw(5,9*6,"848 kbps",DOGS102x6_DRAW_NORMAL);
#endif

				Serial_printf("848 kbps",RW_BITR_DATA);
			}

		}
#ifdef MSP430F5529_EXP_LCD_ENABLED
		LCD_stringDraw(6,9*6,"0x",DOGS102x6_DRAW_NORMAL);
		LCD_stringDraw(7,9*6,"0x",DOGS102x6_DRAW_NORMAL);
#endif

		ui8RssiValue = eTrfStatus.ui8InternalRssi;
		convertByteToAscii(eTrfStatus.ui8InternalRssi,(uint8_t *)pui8Buffer);
		#ifdef MSP430F5529_EXP_LCD_ENABLED
		LCD_stringDraw(7,11*6,pui8Buffer,DOGS102x6_DRAW_NORMAL);
		#endif
		if(eTrfStatus.bNfcModeEnabled == true)
		{
			Serial_printf(pui8Buffer,P2P_RSSI_DATA);
		}
		else
		{
			Serial_printf(pui8Buffer,RW_RSSI_DATA);
			Serial_printf(pui8Buffer,CE_RSSI_DATA);
		}
	}

	if(eTrfStatus.ui8InternalRssi != ui8RssiValue)
	{
		ui8RssiValue = eTrfStatus.ui8InternalRssi;
		convertByteToAscii(eTrfStatus.ui8InternalRssi,(uint8_t *)pui8Buffer);
#ifdef MSP430F5529_EXP_LCD_ENABLED
		LCD_stringDraw(7,11*6,pui8Buffer,DOGS102x6_DRAW_NORMAL);
#endif
		if(eTrfStatus.bNfcModeEnabled == true)
		{
			Serial_printf(pui8Buffer,P2P_RSSI_DATA);
		}
		else
		{
			Serial_printf(pui8Buffer,RW_RSSI_DATA);
			Serial_printf(pui8Buffer,CE_RSSI_DATA);
		}
	}
}

//*****************************************************************************
//
//! USBCDC_handleDataReceived - Receives the incoming data from the NFC host.
//!
//! Receives the data incoming from the NFC host, and stores it inside
//! g_ui8SerialBuffer.
//!
//! \return None.
//
//*****************************************************************************
BYTE USBCDC_handleDataReceived (BYTE intfNum)
{
    //TO DO: You can place your code here

	// Flush the buffer to ensure we have enough memory available
	g_ui16BytesReceived = cdcReceiveDataInBuffer (g_ui8SerialBuffer, 265, CDC0_INTFNUM);

    return (TRUE);                              //return FALSE to go asleep after interrupt (in the case the CPU slept before
                                                //interrupt)
}
