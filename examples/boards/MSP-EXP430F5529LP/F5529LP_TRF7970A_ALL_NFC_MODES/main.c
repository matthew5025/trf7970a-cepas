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

#if NFC_CARD_EMULATION_ENABLED
	t_sNfcCEMode g_sCESupportedModes;
#endif

void NFC_configuration(void);
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

	while(1)
	{
		eTempNFCState = NFC_run();

		if(eTempNFCState == NFC_DATA_EXCHANGE_PROTOCOL)
		{
			if(NFC_RW_getModeStatus(&sRWMode,&sRWBitrate))
			{
			}
			else if(NFC_P2P_getModeStatus(&sP2PMode,&sP2PBitrate))
			{
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

#if (NFC_CARD_EMULATION_ENABLED || NFC_PEER_2_PEER_TARGET_ENABLED || NFC_PEER_2_PEER_INITIATOR_ENABLED)
				// Initialize IDs for NFC-A, NFC-B and NFC-F
				NFC_initIDs();
#endif
				// Clear RW, P2P and CE LEDs
				NFC_RW_LED_POUT &= ~NFC_RW_LED_BIT;
				NFC_P2P_LED_POUT &= ~NFC_P2P_LED_BIT;
				NFC_CE_LED_POUT &= ~NFC_CE_LED_BIT;

				buttonDebounce = 1;
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

#if NFC_CARD_EMULATION_ENABLED
	g_sCESupportedModes.ui8byte = 0x00;
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

#if NFC_CARD_EMULATION_ENABLED

	// Enable Card Emulation Supported Modes
	g_sCESupportedModes.bits.bT4TAEnabled = 1;
	g_sCESupportedModes.bits.bT4TBEnabled = 1;
	
#endif

#if NFC_CARD_EMULATION_ENABLED
	NFC_CE_configure(g_sCESupportedModes);

	T4T_CE_init();
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
