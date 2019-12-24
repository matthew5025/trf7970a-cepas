//*****************************************************************************
//
// trf7970.c - TRF7970A Firmware Driver.
//
// Copyright (c) 2015 Texas Instruments Incorporated.  All rights reserved.
// TI Information - Selective Disclosure
//
//*****************************************************************************

#include "trf79x0.h"

#ifdef MSP432P401R_LAUNCHPAD_ENABLED
	void TRF79x0_isrHandler(void);
#endif
#define NFC_FIFO_SIZE 		255

uint8_t g_ui8FifoBuffer[NFC_FIFO_SIZE];

t_sTRF79x0_Bitrate g_sTargetSupportedBitrates;
t_sTRF79x0_TargetMode g_sTargetSupportedTechnologies;
tTRF79x0_Status g_sTrf7970Status;
tTRF79x0_Version g_eTrf7970Version;

volatile uint8_t g_ui8IrqFlag;
volatile uint8_t g_bBitclockFlag;
volatile uint8_t g_ui8TimeOutFlag;

uint8_t g_ui8TargetId[10];
uint8_t g_ui8IdLength;
bool g_bNfcDepSupport;
bool g_bIsoDepSuport;
bool g_bTestEnabled;
bool g_bType5OptionDelay;
bool g_bTRFPowerSupply5V;
bool g_bTRFExtAmpEnable;

//*****************************************************************************
//
// This function initializes the TRF97x0 Enable, Slave Select, and SPI module.
//
//*****************************************************************************
uint8_t TRF79x0_init(void)
{
	g_ui8IrqFlag = 0x00;
	g_ui8TimeOutFlag = 0x00;

#ifdef MSP430F5529_EXP_BOARD_ENABLED
	g_bTRFPowerSupply5V = true;
#endif

#ifdef MSP430F5529_LAUNCHPAD_ENABLED
	g_bTRFPowerSupply5V = false;
#endif

#ifdef MSP432P401R_LAUNCHPAD_ENABLED
	g_bTRFPowerSupply5V = false;
#endif

	g_bTRFExtAmpEnable = false;

	MOD_DIR_OUT;
	MOD_OFF;

	// Set Slave Select to output direction
	SLAVE_SELECT_PORT_SET;

	// De-assert the Slave Select
	SLAVE_SELECT_HIGH;

	//
	// The TRF79x0 requires a low to high transition on the Enable Line.
	//
	// Set EN = 0 on the TRF79x0
	TRF_DISABLE;
	// Set EN to output direction
	ENABLE_PORT_SET;
	// wait until system clock started
	MCU_delayMillisecond(10);
	// Set EN = 1 on the TRF 79xx
	TRF_ENABLE;

	SPI_setup();
	//
	// Enable an interrupt Low to High transition for the IRQ pin on the TRF7970A
	//
	IRQ_PIN_SET;
	IRQ_EDGE_SET;
	IRQ_INT_ON;
	IRQ_CLR;

#ifdef MSP432P401R_LAUNCHPAD_ENABLED
	GPIO_registerInterrupt(TRF_IRQ_PORT,TRF79x0_isrHandler);
#endif

	MCU_delayMillisecond(10);

	g_eTrf7970Version = TRF7970_A;

	return STATUS_SUCCESS;
}

/*
 * Enable TRF79x0 Interrupt.
 */
inline void TRF79x0_interruptEnable(void)
{
	IRQ_INT_ON;
}

/*
 * Disable TRF79x0 Interrupt.
 */
inline void TRF79x0_interruptDisable(void)
{
	IRQ_INT_OFF;
}

/*
 * Raw Write to the TRF79x0.
 */
void TRF79x0_rawWrite(
	uint8_t * pui8Payload,
	uint8_t ui8Length
	)
{
	SPI_rawWrite(pui8Payload,ui8Length);
}

/*
 * Direct Command Write.
 */
void TRF79x0_directCommand(uint8_t ui8Command)
{
	SPI_directCommand(ui8Command);
}

/*
 * Single Write to the ui8Register register.
 */
void TRF79x0_writeSingle(
	uint8_t ui8Value,
	uint8_t ui8Register
	)
{
	SPI_writeSingle(ui8Value,ui8Register);
}

/*
 * Continuous Write of ui8Length bytes starting at register ui8Register.
 */
void TRF79x0_writeCont(
	uint8_t * pui8Payload,
	uint8_t ui8Register,
	uint8_t ui8Length
	)
{
	SPI_writeCont(pui8Payload,ui8Register,ui8Length);
}

/*
 * Single Read from the ui8Register register.
 */
void TRF79x0_readSingle(
	uint8_t * pui8Value,
	uint8_t ui8Register
	)
{
	SPI_readSingle(pui8Value,ui8Register);
}

/*
 * Continuous Read of ui8Length bytes starting at register ui8Register.
 */
void TRF79x0_readCont(
	uint8_t * pui8Payload,
	uint8_t ui8Register,
	uint8_t ui8Length
	)
{
	SPI_readCont(pui8Payload,ui8Register,ui8Length);
//	if(ui8Length == 0x01)
//	{
//		SPI_readSingle(pui8Payload,ui8Register);
//	}
//	else
//	{
//		SPI_readCont(pui8Payload,ui8Register,ui8Length);
//	}
}

/*
 * Writes payload to the FIFO register.
 */
uint8_t TRF79x0_writeFIFO(
	uint8_t *pui8Payload,
    tTRF79x0_CRC eCRCBit,
	uint8_t ui8Length,
	uint8_t ui8BrokenBits
	)
{
	uint8_t ui8Status = STATUS_SUCCESS;
	tTRF79x0_IRQFlag sIrqStatusFlag = IRQ_STATUS_IDLE;
	uint8_t ui8RemainingBytes = 0;
	uint8_t ui8FiFoStatusLength = 0;
	uint8_t ui8PayloadLength = 0;
    uint8_t ui8TimeOutCounter = 0;
    uint8_t ui8IrqBuffer[2];

	// Ensure the IRQ line is low before sending out a packet
	if (IRQ_IS_SET())
	{
		TRF79x0_readSingle(ui8IrqBuffer, TRF79X0_IRQ_STATUS_REG);
	}

	if(g_sTrf7970Status.eCurrentMode == TARGET_MODE)
	{
		// Enable the field for active communication
		if(g_sTrf7970Status.sTargetMode.bits.bActiveTypeA == 1 ||
			g_sTrf7970Status.sTargetMode.bits.bActiveTypeF == 1)
		{
			// Check for external RSSI
//			if(TRF79x0_checkField(EXTERNAL_RSSI) == 0x00)
//			{
//				TRF79x0_switchTargetBaudRate(g_sTrf7970Status.sTargetPayloadFrequency,true);
//				TRF79x0_setField(EXT_FIELD_ON);
//				MCU_delayMillisecond(1);
			MCU_delayMicrosecond(200);
//			}
//			else
//			{
//				ui8Status = STATUS_FAIL;
//			}
		}
		else if(g_sTrf7970Status.sTargetMode.bits.bPassiveTypeF == 1)
		{
			// Need to meet the TRF0,LISTEN,MIN (197uS)
			MCU_delayMicrosecond(75);
		}
		else
		{
			// Do nothing
		}
	}
	else if(g_sTrf7970Status.eCurrentMode == INITIATOR_MODE)
	{
		// Enable the field for active communication
		if(g_sTrf7970Status.sInitiatorMode.bits.bActiveTypeA == 1 ||
			g_sTrf7970Status.sInitiatorMode.bits.bActiveTypeF == 1)
		{
			TRF79x0_switchInitiatorBaudRate(g_sTrf7970Status.sInitiatorMode,g_sTrf7970Status.sInitiatorPayloadFrequency,true);
//			MCU_delayMillisecond(1);
			MCU_delayMicrosecond(200);
		}
		else if(g_sTrf7970Status.sInitiatorMode.bits.bPassiveTypeB == 1)
		{
			// Minimum TR2
			MCU_delayMicrosecond(300);
		}
	}

	if(ui8Status != STATUS_FAIL && ui8Length > 0)
	{
		// Check if the Total Packet Length is greater than 127, if so we have to fragment
		// the packet with the first fragment including 127 bytes.
		if (ui8Length > 127)
		{
			ui8PayloadLength = 127;
		}
		else
		{
			ui8PayloadLength = ui8Length;
		}

		// Calculate remaining bytes to be sent.
		ui8RemainingBytes = ui8Length - ui8PayloadLength;

		if( g_sTrf7970Status.eCurrentMode == TARGET_MODE
			&& g_sTrf7970Status.sTargetMode.bits.bActiveTypeA == 1)
		{
			// Send 1st Fragment of the packet.
			SPI_writePacket(pui8Payload, (uint8_t) eCRCBit, ui8Length, ui8PayloadLength, true,ui8BrokenBits,true);
		}
		else if(g_sTrf7970Status.bNfcModeEnabled == true
			&& g_sTrf7970Status.bTargetSelected == true
			&& g_sTrf7970Status.eCurrentMode == TARGET_MODE
			&& g_sTrf7970Status.sTargetMode.bits.bPassiveTypeA == 1)
		{
			// Send 1st Fragment of the packet.
			SPI_writePacket(pui8Payload, (uint8_t) eCRCBit, ui8Length, ui8PayloadLength, true,ui8BrokenBits,true);
		}
		else if(g_sTrf7970Status.bNfcModeEnabled == true
			&& g_sTrf7970Status.eCurrentMode == INITIATOR_MODE
			&& (g_sTrf7970Status.sInitiatorMode.bits.bPassiveTypeA == 1
					|| g_sTrf7970Status.sInitiatorMode.bits.bActiveTypeA == 1))
		{
			// Send 1st Fragment of the packet.
			SPI_writePacket(pui8Payload, (uint8_t) eCRCBit, ui8Length, ui8PayloadLength, true,ui8BrokenBits,true);
		}
		else
		{
			// Send 1st Fragment of the packet.
			SPI_writePacket(pui8Payload, (uint8_t) eCRCBit, ui8Length, ui8PayloadLength, true,ui8BrokenBits,false);
		}

		// Wait until TX is complete
		while (sIrqStatusFlag != IRQ_STATUS_TX_COMPLETE)
		{
			// Workaround for Type A commands - check the IRQ within 7 mS to refill FIFO
			if (g_sTrf7970Status.eCurrentMode == TARGET_MODE
					&& (g_sTrf7970Status.sTargetMode.bits.bPassiveTypeA == 1)
					&& ui8Length > 127)
			{
				sIrqStatusFlag = TRF79x0_irqHandler(7);
                                ui8TimeOutCounter = ui8TimeOutCounter + 7;
			}
			// Workaround for Type F commands - check the IRQ within 1 mS to refill FIFO
			else if (g_sTrf7970Status.eCurrentMode == TARGET_MODE
					&& (g_sTrf7970Status.sTargetMode.bits.bPassiveTypeF == 1
							|| g_sTrf7970Status.sTargetMode.bits.bActiveTypeF == 1)
					&& ui8Length > 127)
			{
				sIrqStatusFlag = TRF79x0_irqHandler(1);
                                ui8TimeOutCounter++;
			}
			else
			{
				sIrqStatusFlag = TRF79x0_irqHandler(1);
                                ui8TimeOutCounter++;
			}

                        // Check if there was a Protocol_Error or if 10 mSec ( 128 bytes @ 26kb worst case scenario)
			if (sIrqStatusFlag == IRQ_STATUS_PROTOCOL_ERROR || sIrqStatusFlag == IRQ_STATUS_NO_RESPONSE 
                              || ui8TimeOutCounter > 40)
			{
				ui8Status = STATUS_FAIL;
				break;
			}
			else if (sIrqStatusFlag == IRQ_STATUS_TX_COMPLETE)
			{
				ui8Status = STATUS_SUCCESS;
			}
			else if ((sIrqStatusFlag == IRQ_STATUS_FIFO_HIGH_OR_LOW || sIrqStatusFlag == IRQ_STATUS_TIME_OUT) && ui8RemainingBytes > 0)
			{
                ui8TimeOutCounter = 0;

				// Modify the pointer to point to the next address of data for payload larger than 127 bytes
				pui8Payload = pui8Payload + ui8PayloadLength;

				// Check how many bytes are inside the FIFO.
				TRF79x0_readSingle(&ui8FiFoStatusLength, TRF79X0_FIFO_STATUS_REG);

				// Check if there are more remaining bytes than available spots on the TRF7970
				if (ui8RemainingBytes > (127 - ui8FiFoStatusLength))
				{
					// If there are more bytes than available then payload length is the (127 - ui8FiFoStatusLength)
					ui8PayloadLength = (127 - ui8FiFoStatusLength);
				}
				else
				{
					ui8PayloadLength = ui8RemainingBytes;
				}

				// Update Remaining Bytes.
				ui8RemainingBytes = ui8RemainingBytes - ui8PayloadLength;

				if(g_sTrf7970Status.bNfcModeEnabled == true
					&& g_sTrf7970Status.bTargetSelected == true
					&& g_sTrf7970Status.eCurrentMode == TARGET_MODE
					&& (g_sTrf7970Status.sTargetMode.bits.bPassiveTypeA == 1
							|| g_sTrf7970Status.sTargetMode.bits.bActiveTypeA == 1))
				{
					// Send 1st Fragment of the packet.
					SPI_writePacket(pui8Payload, (uint8_t) eCRCBit, ui8Length, ui8PayloadLength, false,0x00,true);
				}
				else if(g_sTrf7970Status.bNfcModeEnabled == true
					&& g_sTrf7970Status.eCurrentMode == INITIATOR_MODE
					&& (g_sTrf7970Status.sInitiatorMode.bits.bPassiveTypeA == 1
							|| g_sTrf7970Status.sInitiatorMode.bits.bActiveTypeA == 1))
				{
					// Send 1st Fragment of the packet.
					SPI_writePacket(pui8Payload, (uint8_t) eCRCBit, ui8Length, ui8PayloadLength, false,0x00,true);
				}
				else
				{
					// Send 1st Fragment of the packet.
					SPI_writePacket(pui8Payload, (uint8_t) eCRCBit, ui8Length, ui8PayloadLength, false,0x00,false);
				}
			}
			else
			{
				// Do nothing
			}
		}

		// Target Active Communication Handling.
		if(g_sTrf7970Status.eCurrentMode == TARGET_MODE
				&& g_sTrf7970Status.sTargetMode.ui8byte != 0x00)
		{
			// Disable the field for active communication
			if(g_sTrf7970Status.sTargetMode.bits.bActiveTypeA == 1)
			{
				// Handle the RF Field turned on while exiting the send cmd.
				TRF79x0_irqHandler(0);

				MCU_delayMicrosecond(1);

				// Setup for passive target mode.
				TRF79x0_writeSingle(0x21,TRF79X0_ISO_CONTROL_REG);

				g_sTrf7970Status.bRfFieldOn = 0;
			}
			else if(g_sTrf7970Status.sTargetMode.bits.bActiveTypeF == 1)
			{
				// Handle the RF Field turned on while exiting the send cmd.
				TRF79x0_irqHandler(0);

				MCU_delayMicrosecond(1);

				// Setup for passive target mode.
				TRF79x0_writeSingle(0x21,TRF79X0_ISO_CONTROL_REG);

				g_sTrf7970Status.bRfFieldOn = 0;
			}
			else
			{
				// Do nothing
			}
		}
		// Initiator Active Communication Handling.
		else if(g_sTrf7970Status.eCurrentMode == INITIATOR_MODE)
		{
			// Disable the field for active communication
			if(g_sTrf7970Status.sInitiatorMode.bits.bActiveTypeA == 1)
			{
				// Handle the RF Field turned on while exiting the send cmd.
				TRF79x0_irqHandler(0);

				MCU_delayMicrosecond(100);

				// Setup for passive target mode.
				TRF79x0_writeSingle(0x21,TRF79X0_ISO_CONTROL_REG);

				g_sTrf7970Status.bRfFieldOn = 0;
			}
			else if(g_sTrf7970Status.sInitiatorMode.bits.bActiveTypeF == 1)
			{
				// Handle the RF Field turned on while exiting the send cmd.
				TRF79x0_irqHandler(0);

				MCU_delayMicrosecond(100);

				// Setup for passive target mode.
				TRF79x0_writeSingle(0x21,TRF79X0_ISO_CONTROL_REG);

				g_sTrf7970Status.bRfFieldOn = 0;
			}
			// 15693 Option Flag Handling
			else if(g_sTrf7970Status.sInitiatorMode.bits.bPassive15693 == 1)
			{
				// If the command is not Read Multiple and Single Blocks
				if (g_bType5OptionDelay)
				{
					MCU_delayMillisecond(13);
					TRF79x0_directCommand(TRF79X0_TRANSMIT_NEXT_SLOT_CMD);
					g_bType5OptionDelay = false;
				}
			}
			else
			{
				// Do nothing
			}
		}
		else
		{
			// Do nothing
		}
	}

	return ui8Status;
}

/*
 * IRQ Handler.
 */
tTRF79x0_IRQFlag TRF79x0_irqHandler(uint16_t ui16TimeOut)
{
	tTRF79x0_IRQFlag eIrqStatusFlag = IRQ_STATUS_IDLE;

	uint8_t ui8IrqBuffer[2];
	uint8_t ui8TargetProtocol[2];
	uint8_t ui8FifoStatusLength;
	uint8_t ui8FifoIndex = 0;
	uint16_t ui16TempLen = 0;

	bool bRfidModeEnabled = false;
	bool bNfcModeEnabled = false;

	volatile uint8_t ui8TempX;

	if (IRQ_IS_SET())
	{
		g_ui8IrqFlag = 1;
	}
	else
	{
		g_ui8IrqFlag = 0;
		if (ui16TimeOut == 0)
		{
			return IRQ_STATUS_TIME_OUT;
		}
		//
		// Initialize a timeout_ms timeout
		//
		MCU_timerInit(ui16TimeOut, (uint8_t*) &g_ui8TimeOutFlag);
	}

	//
	// Check if the IRQ flag has been set
	//
	while (g_ui8IrqFlag == 0x00 && g_ui8TimeOutFlag == 0x00)
	{
		// Do Nothing
	}

	//
	// Disable Timer
	//
	MCU_timerDisable();

	// Check for RFID Mode
	if ((g_sTrf7970Status.eCurrentMode == INITIATOR_MODE
			&& (g_sTrf7970Status.sInitiatorMode.bits.bPassive15693 == 1
				|| g_sTrf7970Status.sInitiatorMode.bits.bPassiveTypeA == 1
				|| g_sTrf7970Status.sInitiatorMode.bits.bPassiveTypeB == 1
				|| g_sTrf7970Status.sInitiatorMode.bits.bPassiveTypeF == 1)))
	{
		bRfidModeEnabled = true;
	}
	else if(g_sTrf7970Status.eCurrentMode != IDLE_MODE)
	{
		bNfcModeEnabled = true;
	}
	else
	{
		// Do nothing
	}


	if (g_ui8TimeOutFlag == 0x01)
	{
		eIrqStatusFlag = IRQ_STATUS_TIME_OUT;
	}
	else
	{
		if(g_sTrf7970Status.eCurrentMode == TARGET_MODE)
		{
			//
			// Read Target Protocol
			//
//			TRF79x0_readCont(ui8TargetProtocol, TRF79X0_NFC_TARGET_PROTOCOL_REG, 2);
			TRF79x0_readSingle(ui8TargetProtocol,TRF79X0_NFC_TARGET_PROTOCOL_REG);
		}

		//
		// Read IRQ Register
		//
		TRF79x0_readSingle(ui8IrqBuffer,TRF79X0_IRQ_STATUS_REG);

		// RF Field for Target Modes
		if (g_sTrf7970Status.eCurrentMode == TARGET_MODE)
		{
			if ((ui8TargetProtocol[0] & 0x80) == 0x80)
			{
				g_sTrf7970Status.bRfFieldOn = true;
			}
			else if(g_sTrf7970Status.bRfFieldOn)
			{
				g_sTrf7970Status.bRfFieldOn = false;
			}
			else
			{
				// Do nothing
			}
		}
		else if (g_sTrf7970Status.eCurrentMode != TARGET_MODE)
		{
			g_sTrf7970Status.bRfFieldOn = false;
		}
		else
		{
			// Do nothing
		}
		TRF79x0_setExtFieldLed(g_sTrf7970Status.bRfFieldOn);

		//
		// FIFO_IRQ_Handler
		//
		if (ui8IrqBuffer[0] & IRQ_STATUS_FIFO_HIGH_OR_LOW) {
			if (ui8IrqBuffer[0] & IRQ_STATUS_RX_COMPLETE) {

				g_sTrf7970Status.ui8FifoBytesReceived = 0;
				//
				// Read the FIFO status and FIFO into g_nfc_buffer
				//
				TRF79x0_readSingle(&ui8FifoStatusLength,TRF79X0_FIFO_STATUS_REG);

				if(g_sTrf7970Status.bNfcModeEnabled == true
					&& g_sTrf7970Status.bTargetSelected == true
					&& g_sTrf7970Status.eCurrentMode == TARGET_MODE
					&& (g_sTrf7970Status.sTargetMode.bits.bPassiveTypeA == 1
							|| g_sTrf7970Status.sTargetMode.bits.bActiveTypeA == 1))
				{
					// Dummy read
					TRF79x0_readCont(g_ui8FifoBuffer,TRF79X0_FIFO_REG, 1);
					// Decrement the Total by one
					ui8FifoStatusLength--;
				}
				else if(g_sTrf7970Status.bNfcModeEnabled == true
					&& g_sTrf7970Status.eCurrentMode == INITIATOR_MODE
					&& (g_sTrf7970Status.sInitiatorMode.bits.bPassiveTypeA == 1
							|| g_sTrf7970Status.sInitiatorMode.bits.bActiveTypeA == 1))
				{
					// Dummy read
					TRF79x0_readCont(g_ui8FifoBuffer,TRF79X0_FIFO_REG, 1);
					// Decrement the Total by one
					ui8FifoStatusLength--;
				}

				ui8FifoIndex = 0;

				// Wait until RX complete, timeout or overflow occurs
				while (eIrqStatusFlag == IRQ_STATUS_IDLE)
				{
					// Update the received bytes
					g_sTrf7970Status.ui8FifoBytesReceived += ui8FifoStatusLength;

					if(ui8FifoStatusLength > 0)
					{
					  //
					  // Read the FIFO Data
					  //
					  TRF79x0_readCont(&g_ui8FifoBuffer[ui8FifoIndex],TRF79X0_FIFO_REG, ui8FifoStatusLength);

					  // Update fifo_index
					  ui8FifoIndex = ui8FifoIndex + ui8FifoStatusLength;
					}

					// Work around for 15693 receiving over 96 bytes
					if(g_sTrf7970Status.sInitiatorMode.bits.bPassive15693 == 1 && ui8IrqBuffer[0] == 0x40 && ui8FifoStatusLength == 1)
					{
						eIrqStatusFlag = IRQ_STATUS_RX_COMPLETE;
						break;
					}

					//
					// Re-Read the FIFO status to check if we are still in the process of receiving
					//
					TRF79x0_readSingle(&ui8FifoStatusLength,TRF79X0_FIFO_STATUS_REG);

					// Workaround for TC_LIS_NFCF_P2P_BV_13_x_3
					if(g_sTrf7970Status.ui8FifoBytesReceived == 255 &&
							g_sTrf7970Status.eCurrentMode == TARGET_MODE &&
							g_sTrf7970Status.sTargetMode.bits.bPassiveTypeF == 1)
					{
						eIrqStatusFlag = IRQ_STATUS_RX_COMPLETE;
					}
					else if((ui8FifoStatusLength & 0x7F) == 0x00)
					{
						eIrqStatusFlag = IRQ_STATUS_RX_COMPLETE;
					}
					else
					{
						//
						// Initialize a 1 mS timeout
						//
						if(g_sTrf7970Status.eCurrentMode == INITIATOR_MODE &&
							(g_sTrf7970Status.sInitiatorMode.bits.bPassiveTypeA == 1 ||
							 g_sTrf7970Status.sInitiatorMode.bits.bActiveTypeA == 1))
						{
//							// If the IRQ is set, do not read out the data yet (workaround for Passive Type A)
//							if (IRQ_IS_SET())
//							{
//								TRF79x0_readCont(ui8IrqBuffer, TRF79X0_IRQ_STATUS_REG, 2);
//							}
							ui16TimeOut = 7;
						}
						else if(g_sTrf7970Status.eCurrentMode == TARGET_MODE &&
								(g_sTrf7970Status.sTargetMode.bits.bPassiveTypeA == 1 ||
								 g_sTrf7970Status.sTargetMode.bits.bActiveTypeA == 1))
						{
							ui16TimeOut = 7;
						}
						else if(g_sTrf7970Status.sInitiatorMode.bits.bPassive15693 == 1)
						{
							ui16TimeOut = 50;
						}
						else if(g_sTrf7970Status.eCurrentMode == TARGET_MODE &&
								g_sTrf7970Status.sTargetMode.bits.bPassiveTypeF == 1)
						{
							ui16TimeOut = 3;
						}
						else
						{

							ui16TimeOut = 7;
						}

						if (!IRQ_IS_SET())
						{
							g_ui8IrqFlag = 0;
						}
						else
						{
							g_ui8IrqFlag = 1;
						}

						MCU_timerInit(ui16TimeOut, (uint8_t*) &g_ui8TimeOutFlag);

						while(g_ui8IrqFlag == 0x00 && g_ui8TimeOutFlag == 0x00)
						{

						}

						//
						// Disable Timer
						//
						MCU_timerDisable();
					}


					TRF79x0_readCont(ui8IrqBuffer, TRF79X0_IRQ_STATUS_REG, 2);

					//
					// Read the FIFO status and FIFO into g_nfc_buffer
					//
					TRF79x0_readSingle(&ui8FifoStatusLength,TRF79X0_FIFO_STATUS_REG);

					// Mask off bit 7.
					ui8FifoStatusLength &= 0x7F;

					ui16TempLen = (uint16_t) ui8FifoStatusLength + (uint16_t) g_sTrf7970Status.ui8FifoBytesReceived;

					if(ui16TempLen > 255)
					{
						// Workaround for TC_LIS_NFCF_P2P_BV_13_x_3
						if(ui16TempLen == 256 &&
								g_sTrf7970Status.eCurrentMode == TARGET_MODE &&
								g_sTrf7970Status.sTargetMode.bits.bPassiveTypeF == 1)
						{
							// Decrement the length received by 1
							ui8FifoStatusLength--;
						}
						else
						{
							eIrqStatusFlag = IRQ_STATUS_OVERFLOW;
						}
					}
					else if(ui8FifoStatusLength == 0)
					{
						// Done receiving
						eIrqStatusFlag = IRQ_STATUS_RX_COMPLETE;
					}


					// Check for protocol error
					if(ui8IrqBuffer[0] & 0x10)
					{
						eIrqStatusFlag = IRQ_STATUS_PROTOCOL_ERROR;
					}

					// Workaround for Test Case LIS_NFCAB_UND_BV_01 Case x=6
					if(eIrqStatusFlag != IRQ_STATUS_OVERFLOW &&
							g_ui8IrqFlag == 0x01 &&
							ui8IrqBuffer[0] == 0x40 &&
							(g_sTrf7970Status.sTargetMode.bits.bPassiveTypeB == 1 ||
									g_sTrf7970Status.sTargetMode.bits.bPassiveTypeA == 1 ||
									g_sTrf7970Status.sInitiatorMode.bits.bPassiveTypeB == 1))
					{
						TRF79x0_readCont(&g_ui8FifoBuffer[ui8FifoIndex],TRF79X0_FIFO_REG, ui8FifoStatusLength);
						// Update fifo_index
						ui8FifoIndex = ui8FifoIndex + ui8FifoStatusLength;

						// Update the received bytes
						g_sTrf7970Status.ui8FifoBytesReceived += ui8FifoStatusLength;

						eIrqStatusFlag = IRQ_STATUS_RX_COMPLETE;
					}

				}

				if(eIrqStatusFlag == IRQ_STATUS_OVERFLOW)
				{
					eIrqStatusFlag = IRQ_STATUS_PROTOCOL_ERROR;
				}
				else if(eIrqStatusFlag != IRQ_STATUS_RX_COMPLETE)
				{
					eIrqStatusFlag = IRQ_STATUS_PROTOCOL_ERROR;
				}
				else if(IRQ_IS_SET())
				{
					TRF79x0_readCont(ui8IrqBuffer, TRF79X0_IRQ_STATUS_REG, 2);
				}
				else
				{
					// Do nothing
				}

				// Handle Active communication TADT Required Delay
				if(g_sTrf7970Status.eCurrentMode == TARGET_MODE &&
					 (g_sTrf7970Status.sTargetMode.bits.bActiveTypeF == 1 ||
					  g_sTrf7970Status.sTargetMode.bits.bActiveTypeA == 1))
				{
					while(TRF79x0_checkField(EXTERNAL_RSSI) != 0x00);
					TRF79x0_switchTargetBaudRate(g_sTrf7970Status.sTargetPayloadFrequency,true);
					TRF79x0_setField(EXT_FIELD_ON);
				}
				else if (g_sTrf7970Status.eCurrentMode == INITIATOR_MODE &&
						 (g_sTrf7970Status.sInitiatorMode.bits.bActiveTypeF == 1 ||
						  g_sTrf7970Status.sInitiatorMode.bits.bActiveTypeA == 1))
				{
					while(TRF79x0_checkField(EXTERNAL_RSSI) != 0x00);
					TRF79x0_switchInitiatorBaudRate(g_sTrf7970Status.sInitiatorMode,g_sTrf7970Status.sInitiatorPayloadFrequency,true);
					TRF79x0_setField(EXT_FIELD_ON);
				}

			}
			else if (ui8IrqBuffer[0] & IRQ_STATUS_TX_COMPLETE)
			{
				//
				// Read the FIFO status and FIFO into g_nfc_buffer
				//
				TRF79x0_readSingle(&ui8FifoStatusLength,TRF79X0_FIFO_STATUS_REG);

				if(ui8FifoStatusLength == 0x00)
				{
					eIrqStatusFlag = IRQ_STATUS_TX_COMPLETE;
				}
				else
				{
					eIrqStatusFlag = IRQ_STATUS_FIFO_HIGH_OR_LOW;
				}

			}
			else
			{
				// Do nothing
			}
		}
		else if (ui8IrqBuffer[0] & IRQ_STATUS_TX_COMPLETE)
		{
			if(ui8IrqBuffer[0] == IRQ_STATUS_TX_COMPLETE)
			{
				g_sTrf7970Status.bTxComplete = true;
				// Reset FIFO CMD + Dummy byte
				TRF79x0_resetFifo();

				eIrqStatusFlag = IRQ_STATUS_TX_COMPLETE;
			}
			else if(ui8IrqBuffer[0] == (IRQ_STATUS_TX_COMPLETE | IRQ_STATUS_RX_COMPLETE))
			{
				eIrqStatusFlag = IRQ_STATUS_TX_COMPLETE;
			}
			else
			{
				// Read the FIFO status
				TRF79x0_readSingle(&ui8FifoStatusLength, TRF79X0_FIFO_STATUS_REG);

				if(ui8FifoStatusLength == 0)
					eIrqStatusFlag = IRQ_STATUS_TX_COMPLETE;
			}
		}
		// RX Complete
		else if (ui8IrqBuffer[0] & IRQ_STATUS_RX_COMPLETE)
		{
//			g_sTrf7970Status.bRfFieldOn = true;

			if(g_sTrf7970Status.eCurrentMode == TARGET_MODE)
			{
				if(g_sTrf7970Status.bTargetSelected == true)
				{
					//
					// Register 0Fh. RSSI Register
					//
					TRF79x0_readSingle(&g_sTrf7970Status.ui8InternalRssi,TRF79X0_RSSI_LEVEL_REG);
				}
			}
			// Initiator Mode
			else
			{
				//
				// Register 0Fh. RSSI Register
				//
				TRF79x0_readSingle(&g_sTrf7970Status.ui8InternalRssi,TRF79X0_RSSI_LEVEL_REG);
			}


			//
			// Read the FIFO status and FIFO into g_nfc_buffer
			//
			TRF79x0_readSingle(&ui8FifoStatusLength, TRF79X0_FIFO_STATUS_REG);

			// Handle Active communication TADT Required Delay for Target Mode
			if(g_sTrf7970Status.eCurrentMode == TARGET_MODE &&
				 (g_sTrf7970Status.sTargetMode.bits.bActiveTypeF == 1 ||
				  g_sTrf7970Status.sTargetMode.bits.bActiveTypeA == 1 ||
				  // Check if first polling command is greater than 20 bytes
				  (g_sTrf7970Status.sTargetMode.ui8byte == 0x00 &&
						  ui8FifoStatusLength > 20 &&
						  (g_sTargetSupportedTechnologies.bits.bActiveTypeA == 1 ||
						  g_sTargetSupportedTechnologies.bits.bActiveTypeF == 1) )))
			{
				// Wait until RF Field is disabled.
				while(TRF79x0_checkField(EXTERNAL_RSSI) != 0x00);
				if(g_sTrf7970Status.sTargetMode.ui8byte == 0x00)
				{
					// Change the ISO Control Register to RFID Mode to ensure RF Field will be enabled.
					TRF79x0_writeSingle(0x08,TRF79X0_ISO_CONTROL_REG);
				}
				else
				{
					TRF79x0_switchTargetBaudRate(g_sTrf7970Status.sTargetPayloadFrequency,true);
				}

				TRF79x0_setField(EXT_FIELD_ON);

				// If no Target has been selected and the received command is active A (Reg 0x19 = 0xC0)
				if(g_sTrf7970Status.sTargetMode.ui8byte == 0x00
//						&& g_sTrf7970Status.bTargetSelected == true
						&& ui8TargetProtocol[0] == 0xC0)
				{
					// Dummy read
					TRF79x0_readCont(g_ui8FifoBuffer,TRF79X0_FIFO_REG, 1);

					if(g_ui8FifoBuffer[0] == 0xF0)
					{
						// Decrement the Total by one
						ui8FifoStatusLength--;
						ui8FifoIndex = 0;
					}
					else
					{
						ui8FifoIndex = 1;
					}
				}
			}
			// Handle Active communication TADT Required Delay for Initiator Mode
			else if (g_sTrf7970Status.eCurrentMode == INITIATOR_MODE &&
					 (g_sTrf7970Status.sInitiatorMode.bits.bActiveTypeF == 1 ||
					  g_sTrf7970Status.sInitiatorMode.bits.bActiveTypeA == 1))
			{
				while(TRF79x0_checkField(EXTERNAL_RSSI) != 0x00);
				TRF79x0_switchInitiatorBaudRate(g_sTrf7970Status.sInitiatorMode,g_sTrf7970Status.sInitiatorPayloadFrequency,true);
				TRF79x0_setField(EXT_FIELD_ON);

			}
			else
			{
				// Do nothing
			}


			// Check if the selected_mode corresponds to the command read in the command
			if ((g_sTrf7970Status.eCurrentMode == TARGET_MODE &&
					(g_sTrf7970Status.sTargetMode.ui8byte == 0x00
					 || (g_sTrf7970Status.sTargetMode.bits.bPassiveTypeF == 1 &&
							 (ui8TargetProtocol[0] & TRF79X0_NFC_TARGET_PROTOCOL_FELICA))
					 || (g_sTrf7970Status.sTargetMode.bits.bActiveTypeF == 1 &&
							 (ui8TargetProtocol[0] & TRF79X0_NFC_TARGET_PROTOCOL_FELICA))
					 || (g_sTrf7970Status.sTargetMode.bits.bPassiveTypeA == 1)
					 || (g_sTrf7970Status.sTargetMode.bits.bPassiveTypeB == 1 &&
							 (ui8TargetProtocol[0] & TRF79X0_NFC_TARGET_PROTOCOL_ISO14443B))
					 || (g_sTrf7970Status.sTargetMode.bits.bActiveTypeA == 1)
				    ))
					|| g_sTrf7970Status.eCurrentMode == INITIATOR_MODE
			   	  )

			{
				// Check if P2P mode is enabled
				if(g_sTrf7970Status.bNfcModeEnabled == true)
				{
					if(g_sTrf7970Status.bTargetSelected == true
						&& g_sTrf7970Status.eCurrentMode == TARGET_MODE
						&& (g_sTrf7970Status.sTargetMode.bits.bPassiveTypeA == 1
								|| g_sTrf7970Status.sTargetMode.bits.bActiveTypeA == 1))
					{
						// Dummy read
						TRF79x0_readCont(g_ui8FifoBuffer,TRF79X0_FIFO_REG, 1);

						if(g_ui8FifoBuffer[0] == 0xF0)
						{
							// Decrement the Total by one
							ui8FifoStatusLength--;
							ui8FifoIndex = 0;
						}
						else
						{
							ui8FifoIndex = 1;
						}
					}
					else if(g_sTrf7970Status.eCurrentMode == INITIATOR_MODE
						&& (g_sTrf7970Status.sInitiatorMode.bits.bPassiveTypeA == 1
								|| g_sTrf7970Status.sInitiatorMode.bits.bActiveTypeA == 1))
					{
						// Dummy read
						TRF79x0_readCont(g_ui8FifoBuffer,TRF79X0_FIFO_REG, 1);

						if(g_ui8FifoBuffer[0] == 0xF0)
						{
							// Decrement the Total length by one
							ui8FifoStatusLength--;
							ui8FifoIndex = 0;
						}
						else
						{
							ui8FifoIndex = 1;
						}
					}
				}

				// Read the data
				if (ui8FifoStatusLength > 0
					&& ((ui8FifoStatusLength & TRF79X0_FIFO_OVERFLOW) != TRF79X0_FIFO_OVERFLOW))
				{
					//
					// Read the FIFO Data
					//
					TRF79x0_readCont(&g_ui8FifoBuffer[ui8FifoIndex], TRF79X0_FIFO_REG,ui8FifoStatusLength);

					g_sTrf7970Status.ui8FifoBytesReceived = ui8FifoStatusLength;
				}
				else
				{
					// Check if the protocol error bit was set
					if(ui8IrqBuffer[0] & IRQ_STATUS_PROTOCOL_ERROR)
					{
						eIrqStatusFlag = IRQ_STATUS_PROTOCOL_ERROR;

						// If the RF field is disabled
						if(ui8TargetProtocol[0] == 0x00)
						{
							TRF79x0_resetMode();
						}
					}
					else
					{
						TRF79x0_resetMode();
						return IRQ_STATUS_IDLE;
					}
				}

				// Check if the protocol error bit was set
				if(ui8IrqBuffer[0] & IRQ_STATUS_PROTOCOL_ERROR)
				{
					eIrqStatusFlag = IRQ_STATUS_PROTOCOL_ERROR;
				}
				else if (ui8IrqBuffer[0] & IRQ_STATUS_RFID_COLLISION_ERROR ||
					ui8IrqBuffer[0] & IRQ_STATUS_RFID_FRAMING_OR_EOF_ERROR ||
					ui8IrqBuffer[0] & IRQ_STATUS_RFID_PARITY_ERROR)
				{
					if(bRfidModeEnabled)
					{
						// NOTE: Code does not always reset immediately unless using the resetMode function below.
						// But when used, then it resets twice. Behaviors which occur are:
						// 1) Single reset after 500 ms
						// 2) Double reset with one happening immediately
//						TRF79x0_resetMode();
						eIrqStatusFlag = IRQ_STATUS_PROTOCOL_ERROR;
					}
				}
				else
				{
					// Do nothing
				}

				// Target Mode Listening for multiple protocols
				if(g_sTrf7970Status.eCurrentMode == TARGET_MODE
				   && g_sTrf7970Status.sTargetPayloadFrequency.ui16word == 0x0000
				   && eIrqStatusFlag != IRQ_STATUS_PROTOCOL_ERROR)
				{
					switch(ui8TargetProtocol[0] & TRF79X0_NFC_TARGET_PROTOCOL_KBPS_MASK)
					{
					case TRF79X0_NFC_TARGET_PROTOCOL_106KBPS:
						g_sTrf7970Status.sTargetPayloadFrequency.bits.b106kpbs = 1;
						break;
					case TRF79X0_NFC_TARGET_PROTOCOL_212KBPS:
						g_sTrf7970Status.sTargetPayloadFrequency.bits.b212kpbs = 1;
						break;
					case TRF79X0_NFC_TARGET_PROTOCOL_424KBPS:
						g_sTrf7970Status.sTargetPayloadFrequency.bits.b424kpbs = 1;
						break;
					default:
						// Handle Type A Case
						// Check for  SENS_REQ or  ATR_REQ (NFC-A)
						if(ui8FifoStatusLength == 1 || (((ui8FifoStatusLength) - 2) == (g_ui8FifoBuffer[0])))
						{
							g_sTrf7970Status.sTargetPayloadFrequency.bits.b106kpbs = 1;
						}

						break;
					}

					if( TRF79x0_targetInitialize() == STATUS_SUCCESS)
					{
						eIrqStatusFlag = IRQ_STATUS_RX_COMPLETE;
					}
					else
					{
						TRF79x0_resetMode();
					}
				}
				else if(g_sTrf7970Status.eCurrentMode == TARGET_MODE
						&& eIrqStatusFlag != IRQ_STATUS_PROTOCOL_ERROR)
				{
					switch(ui8TargetProtocol[0] & TRF79X0_NFC_TARGET_PROTOCOL_KBPS_MASK)
					{
					case TRF79X0_NFC_TARGET_PROTOCOL_106KBPS:
						if(g_sTrf7970Status.sTargetPayloadFrequency.bits.b106kpbs != 1)
							eIrqStatusFlag = IRQ_STATUS_PROTOCOL_ERROR;
						break;
					case TRF79X0_NFC_TARGET_PROTOCOL_212KBPS:
						if(g_sTrf7970Status.sTargetPayloadFrequency.bits.b212kpbs != 1)
							eIrqStatusFlag = IRQ_STATUS_PROTOCOL_ERROR;
						break;
					case TRF79X0_NFC_TARGET_PROTOCOL_424KBPS:
						if(g_sTrf7970Status.sTargetPayloadFrequency.bits.b424kpbs != 1)
							eIrqStatusFlag = IRQ_STATUS_PROTOCOL_ERROR;
						break;
					default:
						// Handle Type A Case
						if(ui8TargetProtocol[0] == 0xC0)
						{
							if(g_sTrf7970Status.sTargetPayloadFrequency.bits.b106kpbs != 1)
								eIrqStatusFlag = IRQ_STATUS_PROTOCOL_ERROR;
						}
						break;
					}

					if(eIrqStatusFlag != IRQ_STATUS_PROTOCOL_ERROR)
					{
						eIrqStatusFlag = IRQ_STATUS_RX_COMPLETE;
					}

				}
				// Initiator Mode
				else if(g_sTrf7970Status.eCurrentMode == INITIATOR_MODE
					    && eIrqStatusFlag != IRQ_STATUS_PROTOCOL_ERROR)
				{
					eIrqStatusFlag = IRQ_STATUS_RX_COMPLETE;
				}
				else
				{
					// Do nothing
				}
			}
			else
			{
				if(ui8FifoStatusLength == 0)
				{
					eIrqStatusFlag = IRQ_STATUS_PROTOCOL_ERROR;
					TRF79x0_resetMode();
				}
			}
		}
		// Handle NFC remaining cases
		else if(bNfcModeEnabled)
		{
			if ((ui8IrqBuffer[0] & IRQ_STATUS_NFC_COLLISION_ERROR)
				|| (ui8IrqBuffer[0] & IRQ_STATUS_NFC_PROTOCOL_ERROR))
			{
				if(g_sTrf7970Status.bTargetSelected)
				{
					g_sTrf7970Status.bProtocolError = true;
				}
				TRF79x0_resetMode();
				eIrqStatusFlag = IRQ_STATUS_PROTOCOL_ERROR;
			}
			else if (ui8IrqBuffer[0] & IRQ_STATUS_NFC_RF_FIELD_CHANGE)
			{
				eIrqStatusFlag = IRQ_STATUS_RF_FIELD_CHANGE;
				if(g_sTrf7970Status.sTargetMode.bits.bPassiveTypeA == 1
					|| g_sTrf7970Status.sTargetMode.bits.bPassiveTypeB == 1
					|| g_sTrf7970Status.sTargetMode.bits.bPassiveTypeF == 1)
				{
					if((ui8TargetProtocol[0] & 0x80) != 0x80)
					{
						TRF79x0_resetMode();
						eIrqStatusFlag = IRQ_STATUS_PROTOCOL_ERROR;
					}
				}
				// Check if field was disabled - allows for Auto_SDD to work properly
				else if(g_sTrf7970Status.eCurrentMode == TARGET_MODE
						&& ((ui8TargetProtocol[0] & 0x80) == 0x00)
						&& g_sTrf7970Status.sTargetMode.ui8byte == 0x00)
				{
					if(g_sTrf7970Status.sTargetMode.bits.bActiveTypeA == 1
							|| g_sTrf7970Status.sTargetMode.bits.bActiveTypeF == 1)
					{
						// Do not reset
					}
					else
					{
						TRF79x0_resetMode();
					}
				}
				else
				{
					// Do nothing
				}


				// Check RF Field when Active Initiator is enabled
				if (g_sTrf7970Status.eCurrentMode == INITIATOR_MODE &&
					 (g_sTrf7970Status.sInitiatorMode.bits.bActiveTypeF == 1 ||
					  g_sTrf7970Status.sInitiatorMode.bits.bActiveTypeA == 1))
				{
					TRF79x0_readCont(ui8TargetProtocol, TRF79X0_NFC_TARGET_PROTOCOL_REG, 2);
				}

				if (((ui8TargetProtocol[0] & 0x40) != 0x40 && ui8TargetProtocol[0] != 0x00)
						|| (g_sTrf7970Status.eCurrentMode == TARGET_MODE
						   && g_sTrf7970Status.sTargetPayloadFrequency.ui16word == 0x0000))
				{
					// In the case we get this in the middle of a transfer
//					TRF79x0_resetMode();
				}
			}
			else if(ui8IrqBuffer[0] & IRQ_STATUS_NFC_SDD_COMPLETE)
			{
				g_sTrf7970Status.sTargetMode.bits.bPassiveTypeA = 1;
				g_sTrf7970Status.sTargetPayloadFrequency.bits.b106kpbs = 1;
				eIrqStatusFlag = IRQ_STATUS_SDD_COMPLETE;
				TRF79x0_resetFifo();

				// Disable Anti-collision Frames for 14443A.
				TRF79x0_writeSingle(0x02, TRF79X0_SPECIAL_FUNC_1_REG);

				// Delay to allow for Auto-SDD to complete
				MCU_delayMicrosecond(200);

				TRF79x0_writeSingle(0x07, TRF79X0_NFC_TARGET_LEVEL_REG);

				// Dummy Read - for debugging purpose
				TRF79x0_readSingle(&ui8FifoStatusLength, TRF79X0_ISO_CONTROL_REG);

				// Ensure we are receiving with CRC (CRC not present in FIFO)
				TRF79x0_writeSingle(0x21, TRF79X0_ISO_CONTROL_REG);

				// Disable Anti-collision Frames for 14443A.
				TRF79x0_directCommand(TRF79X0_RESET_FIFO_CMD);
			}
			else
			{
				// Do nothing
			}
		// Handle RFID remaining cases
		}
		else if(bRfidModeEnabled)
		{
			if (ui8IrqBuffer[0] & IRQ_STATUS_RFID_COLLISION_ERROR ||
				ui8IrqBuffer[0] & IRQ_STATUS_RFID_FRAMING_OR_EOF_ERROR ||
				ui8IrqBuffer[0] & IRQ_STATUS_RFID_PARITY_ERROR ||
				ui8IrqBuffer[0] & IRQ_STATUS_RFID_CRC_ERROR)
			{
				TRF79x0_resetMode();
				eIrqStatusFlag = IRQ_STATUS_PROTOCOL_ERROR;
			}
			else if (ui8IrqBuffer[0] == IRQ_STATUS_RFID_NO_RESPONSE_INT)
			{
				eIrqStatusFlag = IRQ_STATUS_NO_RESPONSE;
			}
			else
			{
				// Do nothing
			}
		}
		else
		{
			// Do nothing
		}
	}

	if(eIrqStatusFlag == IRQ_STATUS_PROTOCOL_ERROR
			&& g_sTrf7970Status.eCurrentMode == TARGET_MODE &&
				 (g_sTrf7970Status.sTargetMode.bits.bActiveTypeF == 1 ||
				  g_sTrf7970Status.sTargetMode.bits.bActiveTypeA == 1))
	{
		TRF79x0_setField(EXT_FIELD_OFF);
	}

	// Reset Global Flags
	if (IRQ_IS_SET())
	{
		g_ui8IrqFlag = 1;
	}
	else
	{
		g_ui8IrqFlag = 0;
	}
	g_ui8TimeOutFlag = 0x00;

	return eIrqStatusFlag;
}

/*
 * Switch Target BaudRate.
 */
uint8_t TRF79x0_switchTargetBaudRate(
	t_sTRF79x0_Bitrate sTargetFrequency,
	bool bModifyRegister
	)
{
	uint8_t ui8IsoControlReg;
	uint8_t ui8RetValue = STATUS_SUCCESS;
	//
	// Register 01h. ISO Control Reg
	//
	if(g_sTrf7970Status.sTargetMode.bits.bPassiveTypeA == 1)
	{
		if(sTargetFrequency.bits.b106kpbs == 1)
		{
			ui8IsoControlReg = 0x24;
		}
		else if(sTargetFrequency.bits.b212kpbs == 1)
		{
			// Clear Target Mode
			g_sTrf7970Status.sTargetMode.ui8byte = 0x00;
			// Set Active Type F
			g_sTrf7970Status.sTargetMode.bits.bPassiveTypeF = 1;
			ui8IsoControlReg = 0x22;
			g_sTrf7970Status.sTargetPayloadFrequency = sTargetFrequency;
		}
		else if(sTargetFrequency.bits.b424kpbs == 1)
		{
			// Clear Target Mode
			g_sTrf7970Status.sTargetMode.ui8byte = 0x00;
			// Set Active Type F
			g_sTrf7970Status.sTargetMode.bits.bPassiveTypeF = 1;
			ui8IsoControlReg = 0x23;
			g_sTrf7970Status.sTargetPayloadFrequency = sTargetFrequency;
		}
		else
		{
			ui8RetValue = STATUS_FAIL;
		}
	}
	else if(g_sTrf7970Status.sTargetMode.bits.bPassiveTypeB == 1)
	{
		if(sTargetFrequency.bits.b106kpbs == 1)
		{
			ui8IsoControlReg = 0x25;
		}
		else
		{
			ui8RetValue = STATUS_FAIL;
		}
	}
	else if(g_sTrf7970Status.sTargetMode.bits.bPassiveTypeF == 1)
	{
		if(sTargetFrequency.bits.b212kpbs == 1)
		{
			ui8IsoControlReg = 0x22;
		}
		else if(sTargetFrequency.bits.b424kpbs == 1)
		{
			ui8IsoControlReg = 0x23;
		}
		else
		{
			ui8RetValue = STATUS_FAIL;
		}
	}
	else if(g_sTrf7970Status.sTargetMode.bits.bActiveTypeA == 1)
	{
		if(sTargetFrequency.bits.b106kpbs == 1)
		{
			ui8IsoControlReg = 0x08;
		}
		else if(sTargetFrequency.bits.b212kpbs == 1)
		{
			// Clear Target Mode
			g_sTrf7970Status.sTargetMode.ui8byte = 0x00;
			// Set Active Type F
			g_sTrf7970Status.sTargetMode.bits.bActiveTypeF = 1;
			ui8IsoControlReg = 0x32;
			g_sTrf7970Status.sTargetPayloadFrequency = sTargetFrequency;
		}
		else if(sTargetFrequency.bits.b424kpbs == 1)
		{
			// Clear Target Mode
			g_sTrf7970Status.sTargetMode.ui8byte = 0x00;
			// Set Active Type F
			g_sTrf7970Status.sTargetMode.bits.bActiveTypeF = 1;
			ui8IsoControlReg = 0x33;
			g_sTrf7970Status.sTargetPayloadFrequency = sTargetFrequency;
		}
		else
		{
			ui8RetValue = STATUS_FAIL;
		}
	}
	else if(g_sTrf7970Status.sTargetMode.bits.bActiveTypeF == 1)
	{
		if(sTargetFrequency.bits.b212kpbs == 1)
		{
			// Must be this mode however the SOF is too short.
			ui8IsoControlReg = 0x32;
		}
		else if(sTargetFrequency.bits.b424kpbs == 1)
		{
			ui8IsoControlReg = 0x33;
		}
		else
		{
			ui8RetValue = STATUS_FAIL;
		}
	}
	else
	{
		ui8RetValue = STATUS_FAIL;
	}

	if(ui8RetValue == STATUS_SUCCESS)
	{
		g_sTrf7970Status.sTargetPayloadFrequency = sTargetFrequency;

		if(bModifyRegister == true)
		{
			TRF79x0_writeSingle(ui8IsoControlReg,TRF79X0_ISO_CONTROL_REG);
		}
	}

	return ui8RetValue;
}

uint8_t TRF79x0_incrementTargetBaudRate(
	t_sTRF79x0_Bitrate sTargetFrequency
	)
{
	uint8_t ui8RetValue = STATUS_SUCCESS;

	if(g_sTrf7970Status.sTargetMode.bits.bPassiveTypeA == 1)
	{
		ui8RetValue = TRF79x0_switchTargetBaudRate(sTargetFrequency,true);
	}
	else if(g_sTrf7970Status.sTargetMode.bits.bPassiveTypeB == 1)
	{
		ui8RetValue = TRF79x0_switchTargetBaudRate(sTargetFrequency,true);
	}
	else if(g_sTrf7970Status.sTargetMode.bits.bPassiveTypeF == 1)
	{
		ui8RetValue = TRF79x0_switchTargetBaudRate(sTargetFrequency,true);
	}
	else if(g_sTrf7970Status.sTargetMode.bits.bActiveTypeA == 1)
	{
		ui8RetValue = TRF79x0_switchTargetBaudRate(sTargetFrequency,false);
	}
	else if(g_sTrf7970Status.sTargetMode.bits.bActiveTypeF == 1)
	{
		ui8RetValue = TRF79x0_switchTargetBaudRate(sTargetFrequency,false);
	}
	else
	{
		ui8RetValue = STATUS_FAIL;
	}

	return ui8RetValue;
}

/*
 * Switch Initiator BaudRate.
 */
uint8_t TRF79x0_switchInitiatorBaudRate(
	t_sTRF79x0_InitiatorMode sInitiatorMode,
	t_sTRF79x0_Bitrate sInitiatorFrequency,
	bool bModifyRegister
	)
{
	uint8_t ui8IsoControlReg;
	uint8_t ui8RetValue = STATUS_SUCCESS;
	g_sTrf7970Status.sInitiatorPayloadFrequency = sInitiatorFrequency;
	g_sTrf7970Status.sInitiatorMode = sInitiatorMode;

	//
	// Register 01h. ISO Control Reg
	//
	if( g_sTrf7970Status.sInitiatorMode.bits.bPassive15693 == 1)
	{
		if(sInitiatorFrequency.bits.b6_62_kbps_1_out_4 == 1)
		{
			ui8IsoControlReg = 0x00;
		}
		else if(sInitiatorFrequency.bits.b6_62_kbps_1_out_256 == 1)
		{
			ui8IsoControlReg = 0x01;
		}
		else if(sInitiatorFrequency.bits.b26_48_kbps_1_out_4 == 1)
		{
			ui8IsoControlReg = 0x02;
		}
		else if(sInitiatorFrequency.bits.b26_48_kbps_1_out_256 == 1)
		{
			ui8IsoControlReg = 0x03;
		}
		else if(sInitiatorFrequency.bits.b6_67_kbps_1_out_4 == 1)
		{
			ui8IsoControlReg = 0x04;
		}
		else if(sInitiatorFrequency.bits.b6_67_kbps_1_out_256 == 1)
		{
			ui8IsoControlReg = 0x05;
		}
		else if(sInitiatorFrequency.bits.b26_69_kbps_1_out_4 == 1)
		{
			ui8IsoControlReg = 0x06;
		}
		else if(sInitiatorFrequency.bits.b26_69_kbps_1_out_256 == 1)
		{
			ui8IsoControlReg = 0x07;
		}
		else
		{
			ui8RetValue = STATUS_FAIL;
		}
	}
	else if(g_sTrf7970Status.sInitiatorMode.bits.bPassiveTypeA == 1)
	{
		if(sInitiatorFrequency.bits.b106kpbs == 1)
		{
			ui8IsoControlReg = 0x08;
		}
		else if(sInitiatorFrequency.bits.b212kpbs == 1)
		{

			if(g_sTrf7970Status.bNfcModeEnabled == false)
			{
				ui8IsoControlReg = 0x09;
			}
			else
			{
				// Clear Initiator Mode
				g_sTrf7970Status.sInitiatorMode.ui8byte = 0x00;
				// Set Passive Type F
				g_sTrf7970Status.sInitiatorMode.bits.bPassiveTypeF = 1;
				ui8IsoControlReg = 0x32;
			}
		}
		else if(sInitiatorFrequency.bits.b424kpbs == 1)
		{
			if(g_sTrf7970Status.bNfcModeEnabled == false)
			{
				ui8IsoControlReg = 0x0A;
			}
			else
			{
				// Clear Initiator Mode
				g_sTrf7970Status.sInitiatorMode.ui8byte = 0x00;
				// Set Passive Type F
				g_sTrf7970Status.sInitiatorMode.bits.bPassiveTypeF = 1;
				ui8IsoControlReg = 0x33;
			}
		}
		else if(sInitiatorFrequency.bits.b848kpbs == 1)
		{
			ui8IsoControlReg = 0x0B;
		}
		else
		{
			// Do nothing
		}
	}
	else if(g_sTrf7970Status.sInitiatorMode.bits.bActiveTypeA == 1)
	{
		if(sInitiatorFrequency.bits.b106kpbs == 1)
		{
			ui8IsoControlReg = 0x08;
		}
		else if(sInitiatorFrequency.bits.b212kpbs == 1)
		{

			if(g_sTrf7970Status.bNfcModeEnabled == false)
			{
				ui8IsoControlReg = 0x09;
			}
			else
			{
				// Clear Initiator Mode
				g_sTrf7970Status.sInitiatorMode.ui8byte = 0x00;
				// Set Passive Type F
				g_sTrf7970Status.sInitiatorMode.bits.bActiveTypeF = 1;
				ui8IsoControlReg = 0x32;
			}
		}
		else if(sInitiatorFrequency.bits.b424kpbs == 1)
		{
			if(g_sTrf7970Status.bNfcModeEnabled == false)
			{
				ui8IsoControlReg = 0x0A;
			}
			else
			{
				// Clear Initiator Mode
				g_sTrf7970Status.sInitiatorMode.ui8byte = 0x00;
				// Set Passive Type F
				g_sTrf7970Status.sInitiatorMode.bits.bActiveTypeF = 1;
				ui8IsoControlReg = 0x33;
			}
		}
		else
		{
			// Do nothing
		}
	}

	else if(g_sTrf7970Status.sInitiatorMode.bits.bPassiveTypeB == 1)
	{
		if(sInitiatorFrequency.bits.b106kpbs == 1)
		{
			ui8IsoControlReg = 0x0C;
		}
		else if(sInitiatorFrequency.bits.b212kpbs == 1)
		{
			ui8IsoControlReg = 0x0D;
		}
		else if(sInitiatorFrequency.bits.b424kpbs == 1)
		{
			ui8IsoControlReg = 0x0E;
		}
		else if(sInitiatorFrequency.bits.b848kpbs == 1)
		{
			ui8IsoControlReg = 0x0F;
		}
		else
		{
			// Do nothing
		}
	}
	else if(g_sTrf7970Status.sInitiatorMode.bits.bActiveTypeF == 1 ||
			g_sTrf7970Status.sInitiatorMode.bits.bPassiveTypeF == 1)
	{
		if(sInitiatorFrequency.bits.b212kpbs == 1)
		{
			// Recommended setting for Interoperability
			ui8IsoControlReg = 0x32;
			// Special Setting to work with select Sony FeliCa Tags, but effects P2P performance
//			ui8IsoControlReg = 0x1A;
		}
		else if(sInitiatorFrequency.bits.b424kpbs == 1)
		{
			// Recommended setting for Interoperability
			ui8IsoControlReg = 0x33;
			// Special Setting to work with select Sony FeliCa Tags, but effects P2P performance
//			ui8IsoControlReg = 0x1B;
		}
		else
		{
			// Do nothing
		}
	}
	else
	{
		ui8RetValue = STATUS_FAIL;
	}

	if(ui8RetValue == STATUS_SUCCESS && bModifyRegister == true)
	{
		TRF79x0_writeSingle(ui8IsoControlReg,TRF79X0_ISO_CONTROL_REG);
	}

	return ui8RetValue;
}

uint8_t TRF79x0_incrementInitiatorBaudRate(
	t_sTRF79x0_Bitrate sInitiatorFrequency
	)
{
	uint8_t ui8RetValue = STATUS_SUCCESS;
	//
	// Register 01h. ISO Control Reg
	//
	if(g_sTrf7970Status.sInitiatorMode.bits.bPassive15693 == 1)
	{
		ui8RetValue = TRF79x0_switchInitiatorBaudRate(g_sTrf7970Status.sInitiatorMode,sInitiatorFrequency,true);
	}
	else if(g_sTrf7970Status.sInitiatorMode.bits.bActiveTypeA == 1)
	{
		ui8RetValue = TRF79x0_switchInitiatorBaudRate(g_sTrf7970Status.sInitiatorMode,sInitiatorFrequency,true);
	}
	else if(g_sTrf7970Status.sInitiatorMode.bits.bPassiveTypeA == 1)
	{
		ui8RetValue = TRF79x0_switchInitiatorBaudRate(g_sTrf7970Status.sInitiatorMode,sInitiatorFrequency,true);
	}
	else if(g_sTrf7970Status.sInitiatorMode.bits.bPassiveTypeB == 1)
	{
		ui8RetValue = TRF79x0_switchInitiatorBaudRate(g_sTrf7970Status.sInitiatorMode,sInitiatorFrequency,true);
	}
	else if(g_sTrf7970Status.sInitiatorMode.bits.bActiveTypeF == 1)
	{
		ui8RetValue = TRF79x0_switchInitiatorBaudRate(g_sTrf7970Status.sInitiatorMode,sInitiatorFrequency,true);
	}
	else if(g_sTrf7970Status.sInitiatorMode.bits.bPassiveTypeF == 1)
	{
		ui8RetValue = TRF79x0_switchInitiatorBaudRate(g_sTrf7970Status.sInitiatorMode,sInitiatorFrequency,true);
	}
	else
	{
		ui8RetValue = STATUS_FAIL;
	}

	return ui8RetValue;
}

/*
 * Get Collision Position if there was a collision.
 */
uint16_t TRF79x0_getCollisionStatus(void)
{
	uint16_t ui16CollisionStatus = 0x00;
	uint8_t ui8CollisionStatus = 0x00;

	TRF79x0_readSingle(&ui8CollisionStatus,TRF79X0_COLLISION_POSITION_REG);

	ui16CollisionStatus = ui8CollisionStatus;

	TRF79x0_readSingle(&ui8CollisionStatus,TRF79X0_IRQ_MASK_REG);

	// Mask Bit 7 and 6 ( Col 9 & Col 8)
	ui8CollisionStatus = ui8CollisionStatus & 0xC0;

	ui16CollisionStatus = (ui16CollisionStatus | ( (uint16_t) ui8CollisionStatus << 8));

	return ui16CollisionStatus;
}

/*
 * Enable/Disable RF Field.
 */
uint8_t TRF79x0_setField(tExtField eRfField)
{
	if(eRfField == EXT_FIELD_ON)
	{
		//
		// Register 00h. Chip Status Control
		//
		TRF79x0_writeSingle(TRF79X0_STATUS_CTRL_RF_ON | g_bTRFPowerSupply5V,
						   TRF79X0_CHIP_STATUS_CTRL_REG);
		g_sTrf7970Status.bRfFieldOn = 1;
	}
	else if(eRfField == EXT_FIELD_OFF)
	{
		//
		// Register 00h. Chip Status Control
		//
		TRF79x0_writeSingle((~TRF79X0_STATUS_CTRL_RF_ON) & g_bTRFPowerSupply5V,
							TRF79X0_CHIP_STATUS_CTRL_REG);
		g_sTrf7970Status.bRfFieldOn = 0;
	}
	else
	{
		return STATUS_FAIL;
	}

	return STATUS_SUCCESS;
}

/*
 * Checks the external field RSSI.
 */
uint8_t TRF79x0_checkField(tRssiStatus eField)
{
	uint8_t ui8RssiLevel;

	if(eField == INTERNAL_RSSI)
	{
		//
		// Transmitter On
		//
		TRF79x0_setField(EXT_FIELD_ON);

		//
		// Register 00h. Chip Status Control
		//
		TRF79x0_writeSingle((TRF79X0_STATUS_CTRL_RF_ON |
						   TRF79X0_STATUS_CTRL_RECEIVER_ON |
						   g_bTRFPowerSupply5V),
						   TRF79X0_CHIP_STATUS_CTRL_REG);

		//
		// Register 01h. ISO Control Reg
		//
		TRF79x0_writeSingle(0x00, TRF79X0_ISO_CONTROL_REG);

		//
		// Test Internal RF Field
		//
		TRF79x0_directCommand(TRF79X0_TEST_INTERNAL_RF_CMD);
	}
	else if(eField == EXTERNAL_RSSI)
	{
		//
		// Transmitter Off
		//
		TRF79x0_setField(EXT_FIELD_OFF);

		//
		// Register 00h. Chip Status Control
		//
		TRF79x0_writeSingle((TRF79X0_STATUS_CTRL_RECEIVER_ON |
							g_bTRFPowerSupply5V),
						   TRF79X0_CHIP_STATUS_CTRL_REG);

		//
		// Test External RF Field
		//
		TRF79x0_directCommand(TRF79X0_TEST_EXTERNAL_RF_CMD);
	}

	MCU_delayMicrosecond(50);

	//
	// Register 0Fh. RSSI Register
	//
	TRF79x0_readSingle(&ui8RssiLevel,TRF79X0_RSSI_LEVEL_REG);

	return (ui8RssiLevel&0x3F);
}

/*
 * Initialize Idle Mode.
 */
uint8_t TRF79x0_idleMode(void)
{
	uint8_t ui8RxBuffer[2];
	uint8_t ui8RetryCounter = 0;

	// Reset Status Values
	g_sTrf7970Status.ui8FifoBytesReceived = 0x00;
	g_sTrf7970Status.bTxComplete = false;
	g_sTrf7970Status.bTargetSelected = false;
	g_sTrf7970Status.bRfFieldOn = false;
	g_sTrf7970Status.eCurrentMode = IDLE_MODE;
	g_sTrf7970Status.sTargetPayloadFrequency.ui16word = 0x0000;
	g_sTrf7970Status.sTargetMode.ui8byte = 0x00;
	g_sTrf7970Status.sInitiatorPayloadFrequency.ui16word = 0x0000;
	g_sTrf7970Status.sInitiatorMode.ui8byte = 0x00;
	g_sTrf7970Status.eIrqFlags = IRQ_STATUS_IDLE;
	g_sTrf7970Status.ui8InternalRssi = 0;
	g_sTrf7970Status.bNfcModeEnabled = false;

	TRF79x0_setExtFieldLed(g_sTrf7970Status.bRfFieldOn);

	// Reset Auto-SDD parameters
	g_ui8IdLength = 0x00;
	g_bNfcDepSupport = false;
	g_bIsoDepSuport = false;
	g_bType5OptionDelay = false;

	do {
		//
		// Soft Init Command
		//
		TRF79x0_directCommand(TRF79X0_SOFT_INIT_CMD);

		//
		// Idle Command
		//
		TRF79x0_directCommand(TRF79X0_IDLE_CMD);

		MCU_delayMillisecond(1);

		//
		// Register 09h. Modulator Control
		//
		TRF79x0_readSingle(ui8RxBuffer, TRF79X0_MODULATOR_CONTROL_REG);

	} while (ui8RxBuffer[0] != 0x91 && ui8RetryCounter++ < 5);

	if(ui8RetryCounter == 0x05 )
	{
		return STATUS_FAIL;
	}

	//
	// Register 09h. Modulator Control
	//
	// SYS_CLK (in this case 13.56 MHz) out optional, based on system req.
	TRF79x0_writeSingle(0x00, TRF79X0_MODULATOR_CONTROL_REG);

	//
	// Register 01h. ISO Control Register
	//
	TRF79x0_writeSingle(0x21, TRF79X0_ISO_CONTROL_REG);

	//
	// Register 00h. Chip Status Control
	//
	// +5 V operation
	TRF79x0_writeSingle(0x00 | g_bTRFPowerSupply5V, TRF79X0_CHIP_STATUS_CTRL_REG);


	//
	// Register 0Bh. Regulator Control
	//
	if(g_bTRFExtAmpEnable)
	{
		TRF79x0_writeSingle(0x40, TRF79X0_REGULATOR_CONTROL_REG);
	}
	else
	{
		TRF79x0_writeSingle(0x00, TRF79X0_REGULATOR_CONTROL_REG);
	}

	//
	// Reset FIFO CMD + Dummy byte
	//
	TRF79x0_resetFifo();

	//
	// Register 0Dh. Interrupt Mask Register
	//
	TRF79x0_writeSingle(0x3E, TRF79X0_IRQ_MASK_REG);

	//
	// Register 14h. FIFO IRQ Level
	//
	// RX High = 96 bytes , TX Low = 32 bytes
	TRF79x0_writeSingle(0x0F, TRF79X0_FIFO_IRQ_LEVEL);

	TRF79x0_readSingle(ui8RxBuffer,TRF79X0_IRQ_STATUS_REG);

	return STATUS_SUCCESS;
}

/*
 * Initialize TRF7970A after the receipt of a SENS_REQ (Type A), SENSB_REQ (Type B), SENSF_REQ (Type F),
 * ATR_REQ @ 106 (Active Type A), ATR_REQ @ 212/424 (Active Type F). This function must be execute only when
 * the first command is received from the polling device.
 */
uint8_t TRF79x0_targetInitialize(void)
{
	uint8_t ui8Status = STATUS_SUCCESS;
	t_sTRF79x0_Bitrate sTargetBitrate;

	// Initialize Target Bitrate
	sTargetBitrate.ui16word = 0x0000;

	// Check if one the supported bitrates corresponds to the current bitrate.
	if((g_sTargetSupportedBitrates.ui16word & g_sTrf7970Status.sTargetPayloadFrequency.ui16word) == 0)
	{
		ui8Status = STATUS_FAIL;
	}
	else if(g_sTrf7970Status.sTargetPayloadFrequency.bits.b106kpbs == 1)
	{
		// Check for SENS_REQ (Type A)
		if(g_sTrf7970Status.ui8FifoBytesReceived == 1
				&& (g_ui8FifoBuffer[0] == 0x26 || g_ui8FifoBuffer[0] == 0x52))
		{
			if(g_sTargetSupportedTechnologies.bits.bPassiveTypeA == 1)
			{
				g_sTrf7970Status.sTargetMode.bits.bPassiveTypeA = 1;
			}
			else
			{
				ui8Status = STATUS_FAIL;
			}
		}
		// Check for SENSB_REQ or ATTRIB or SLPB_REQ (Type B)
		else if((g_sTrf7970Status.ui8FifoBytesReceived == 0x03 && g_ui8FifoBuffer[0] == 0x05)
				|| (g_ui8FifoBuffer[0] == 0x1D)
				|| (g_ui8FifoBuffer[0] == 0x50))
		{
			if(g_sTargetSupportedTechnologies.bits.bPassiveTypeB == 1)
			{
				g_sTrf7970Status.sTargetMode.bits.bPassiveTypeB = 1;

				sTargetBitrate.bits.b106kpbs = 1;
				TRF79x0_switchTargetBaudRate(sTargetBitrate,true);

				// Register 09h. Modulator Control Register - ASK (10 %)
				TRF79x0_writeSingle(TRF79X0_MOD_CTRL_MOD_ASK_10, TRF79X0_MODULATOR_CONTROL_REG);
			}
			else
			{
				ui8Status = STATUS_FAIL;
			}
		}
		// Check for ATR_REQ (Active Type A)
		else if(((g_sTrf7970Status.ui8FifoBytesReceived) - 2)== (g_ui8FifoBuffer[0]))
		{
			if(g_sTargetSupportedTechnologies.bits.bActiveTypeA == 1)
			{
				g_sTrf7970Status.sTargetMode.bits.bActiveTypeA = 1;
				TRF79x0_switchTargetBaudRate(g_sTrf7970Status.sTargetPayloadFrequency,true);
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
	else if(g_sTrf7970Status.sTargetPayloadFrequency.bits.b212kpbs == 1)
	{
		// Check for Proper NFC-F Command (P2P only)
		if(g_sTrf7970Status.ui8FifoBytesReceived == g_ui8FifoBuffer[0])
		{
			if(g_sTargetSupportedTechnologies.bits.bPassiveTypeF == 1)
			{
				g_sTrf7970Status.sTargetMode.bits.bPassiveTypeF = 1;

				sTargetBitrate.bits.b212kpbs = 1;
				TRF79x0_switchTargetBaudRate(sTargetBitrate,true);

				// Register 09h. Modulator Control Register - ASK (10 %)
				TRF79x0_writeSingle(TRF79X0_MOD_CTRL_MOD_ASK_10, TRF79X0_MODULATOR_CONTROL_REG);
			}
			else if(g_sTargetSupportedTechnologies.bits.bActiveTypeF == 1)
			{
				g_sTrf7970Status.sTargetMode.bits.bActiveTypeF = 1;
				TRF79x0_switchTargetBaudRate(g_sTrf7970Status.sTargetPayloadFrequency,true);

				// Register 09h. Modulator Control Register - ASK (10 %)
				TRF79x0_writeSingle(TRF79X0_MOD_CTRL_MOD_ASK_10, TRF79X0_MODULATOR_CONTROL_REG);
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
	else if(g_sTrf7970Status.sTargetPayloadFrequency.bits.b424kpbs == 1)
	{
		// Check for SENSF_REQ (Type F)
		if(g_sTrf7970Status.ui8FifoBytesReceived == 0x06 && g_ui8FifoBuffer[0] == 0x06)
		{
			if(g_sTargetSupportedTechnologies.bits.bPassiveTypeF == 1)
			{
				g_sTrf7970Status.sTargetMode.bits.bPassiveTypeF = 1;

				sTargetBitrate.bits.b424kpbs = 1;
				TRF79x0_switchTargetBaudRate(sTargetBitrate,true);

				// Register 09h. Modulator Control Register - ASK (10 %)
				TRF79x0_writeSingle(TRF79X0_MOD_CTRL_MOD_ASK_10, TRF79X0_MODULATOR_CONTROL_REG);
			}
			else
			{
				ui8Status = STATUS_FAIL;
			}
		}
		// Check for ATR_REQ (Active Type F)
		else if(g_sTrf7970Status.ui8FifoBytesReceived == g_ui8FifoBuffer[0])
		{
			if(g_sTargetSupportedTechnologies.bits.bActiveTypeF == 1)
			{
				g_sTrf7970Status.sTargetMode.bits.bActiveTypeF = 1;
				TRF79x0_switchTargetBaudRate(g_sTrf7970Status.sTargetPayloadFrequency,true);
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

/*
 * Setup Target Mode and supported frequencies.
 */
uint8_t TRF79x0_targetModeSetup(
	t_sTRF79x0_TargetMode sSupportedModes,
	t_sTRF79x0_Bitrate sTargetSupportedFrequency)
{
	uint8_t ui8RxBuffer[2];

	// Reset Status Values
	g_sTrf7970Status.ui8FifoBytesReceived = 0x00;
	g_sTrf7970Status.bTxComplete = false;
	g_sTrf7970Status.bTargetSelected = false;
	g_sTrf7970Status.bRfFieldOn = false;
	g_sTrf7970Status.eCurrentMode = TARGET_MODE;
	g_sTrf7970Status.sTargetPayloadFrequency.ui16word = 0x0000;
	g_sTrf7970Status.sTargetMode.ui8byte = 0x00;
	g_sTrf7970Status.sInitiatorPayloadFrequency.ui16word = 0x0000;
	g_sTrf7970Status.sInitiatorMode.ui8byte = 0x00;
	g_sTrf7970Status.eIrqFlags = IRQ_STATUS_IDLE;
	g_sTrf7970Status.ui8InternalRssi = 0;
	g_sTrf7970Status.bNfcModeEnabled = false;

	g_sTargetSupportedTechnologies = sSupportedModes;
	g_sTargetSupportedBitrates = sTargetSupportedFrequency;
	g_bType5OptionDelay = false;

	//
	// Soft Init. Command
	//
	TRF79x0_directCommand(TRF79X0_SOFT_INIT_CMD);

	//
	// Idle Command
	//
	TRF79x0_directCommand(TRF79X0_IDLE_CMD);

	//
	// Register 09h. Modulator Control Register - OOK (100 %) (This helps reduce the time to activate Type A)
	//
	TRF79x0_writeSingle(TRF79X0_MOD_CTRL_MOD_OOK_100, TRF79X0_MODULATOR_CONTROL_REG);

	//
	// Register 00h. Chip Status Control
	//
	// RF output active
	TRF79x0_writeSingle(TRF79X0_STATUS_CTRL_RF_ON | g_bTRFPowerSupply5V,
			TRF79X0_CHIP_STATUS_CTRL_REG);

	//
	// Register 01h. ISO Control Register
	//
	// Handle all incoming Technologies (A/B/F) and frequencies (106/212/424)
	TRF79x0_writeSingle(0xA1, TRF79X0_ISO_CONTROL_REG);

	//
	// Register 0Ah. RX Special Settings
	//
	TRF79x0_writeSingle(0xF0, TRF79X0_RX_SPECIAL_SETTINGS_REG);

	//
	// Register 0Bh. Regulator Control
	//
	if(g_bTRFExtAmpEnable)
	{
		TRF79x0_writeSingle(0x40, TRF79X0_REGULATOR_CONTROL_REG);
	}
	else
	{
		TRF79x0_writeSingle(0x00, TRF79X0_REGULATOR_CONTROL_REG);
	}
	
	//
	// Register 14h. FIFO IRQ Level
	//
	// RX High = 96 bytes , TX Low = 32 bytes
	TRF79x0_writeSingle(0x0F, TRF79X0_FIFO_IRQ_LEVEL);

	//
	// Register 16h. NFC Low Detection Level
	//
	TRF79x0_writeSingle(0x03, TRF79X0_NFC_LOW_DETECTION_LEVEL);

	//
	// Register 18h. NFC Target level
	//
	TRF79x0_writeSingle(0x07, TRF79X0_NFC_TARGET_LEVEL_REG);

	//
	// Read IRQ Register
	//
	TRF79x0_readCont(ui8RxBuffer, TRF79X0_IRQ_STATUS_REG, 2);

	//
	// Read Target NFC Protocol Register
	//
	TRF79x0_readSingle(ui8RxBuffer,TRF79X0_NFC_TARGET_PROTOCOL_REG);

	if(ui8RxBuffer[0] & 0x80)
	{
		g_sTrf7970Status.bRfFieldOn = true;
	}
	TRF79x0_setExtFieldLed(g_sTrf7970Status.bRfFieldOn);

	//	if(g_sTrf7970Status.bRfFieldOn == true)
	//	{
	//		// Setup AutoSdd Register
			TRF79x0_initAutoSdd();
	//	}

	//
	// Enable Decoder Command
	//
	TRF79x0_directCommand(TRF79X0_RUN_DECODERS_CMD);

	return STATUS_SUCCESS;
}

/*
 * Setup Initiator Mode and Initiator frequency.
 */
uint8_t TRF79x0_initiatorModeSetup(
	t_sTRF79x0_InitiatorMode sInitiatorMode,
	t_sTRF79x0_Bitrate sFrequency)
{
	uint8_t ui8TempBuffer[2];

	// Reset Status Values
	g_sTrf7970Status.ui8FifoBytesReceived = 0x00;
	g_sTrf7970Status.bTxComplete = false;
	g_sTrf7970Status.bTargetSelected = false;
	g_sTrf7970Status.bRfFieldOn = false;
	g_sTrf7970Status.eCurrentMode = INITIATOR_MODE;
	g_sTrf7970Status.sTargetPayloadFrequency.ui16word = 0x0000;
	g_sTrf7970Status.sTargetMode.ui8byte = 0x00;
	g_sTrf7970Status.sInitiatorPayloadFrequency = sFrequency;
	g_sTrf7970Status.sInitiatorMode = sInitiatorMode;
	g_sTrf7970Status.eIrqFlags = IRQ_STATUS_IDLE;
	g_sTrf7970Status.ui8InternalRssi = 0;
	g_sTrf7970Status.bNfcModeEnabled = false;

	g_bType5OptionDelay = false;

	//
	// Soft Init Command
	//
	TRF79x0_directCommand(TRF79X0_SOFT_INIT_CMD);

	//
	// Idle Command
	//
	TRF79x0_directCommand(TRF79X0_IDLE_CMD);

	//
	// Register 09h. Modulator Control
	//
	// SYS_CLK (in this case 13.56 MHz) out optional, based on system req.
	TRF79x0_writeSingle(0x00, TRF79X0_MODULATOR_CONTROL_REG);

	if(g_sTrf7970Status.sInitiatorMode.bits.bPassiveTypeA == 1 ||
		g_sTrf7970Status.sInitiatorMode.bits.bActiveTypeA == 1)
	{
		//
		// Register 09h. Modulator Control
		//
		// SYS_CLK (in this case 13.56 MHz) out optional, based on system req.
		TRF79x0_writeSingle(0x01, TRF79X0_MODULATOR_CONTROL_REG);
	}
	else if(g_sTrf7970Status.sInitiatorMode.bits.bPassiveTypeF == 1 ||
			g_sTrf7970Status.sInitiatorMode.bits.bActiveTypeF == 1)
	{
		//
		// Register 09h. Modulator Control
		//
		// SYS_CLK (in this case 13.56 MHz) out optional, based on system req.
		TRF79x0_writeSingle(0x00, TRF79X0_MODULATOR_CONTROL_REG);
	}

	// Check if there an external RF Field
	TRF79x0_directCommand(TRF79X0_TEST_EXTERNAL_RF_CMD);

	MCU_delayMicrosecond(50);

	TRF79x0_readSingle(ui8TempBuffer, TRF79X0_RSSI_LEVEL_REG);

	// If the External RF Field is 0x00, we continue else we return fail
	if ((ui8TempBuffer[0] & 0x3F) != 0x00) {
		// Register 00h. Chip Status Control
		// RF output de-activated, +5 V operation
		TRF79x0_writeSingle(0x00 | g_bTRFPowerSupply5V, TRF79X0_CHIP_STATUS_CTRL_REG);
		return STATUS_FAIL;
	}

	//
	// Register 00h. Chip Status Control
	//
	// Turn off transmitter
	TRF79x0_writeSingle(TRF79X0_STATUS_CTRL_RF_ON | g_bTRFPowerSupply5V, TRF79X0_CHIP_STATUS_CTRL_REG);

	TRF79x0_switchInitiatorBaudRate(sInitiatorMode,sFrequency,true);

	//
	// Register 0Ah. RX Special Settings
	//
	// Turn off transmitter, +5 V operation
	if(g_sTrf7970Status.sInitiatorMode.bits.bPassive15693)
	{
		TRF79x0_writeSingle(0x40, TRF79X0_RX_SPECIAL_SETTINGS_REG);
	}
	else
	{
		TRF79x0_writeSingle(0xF0, TRF79X0_RX_SPECIAL_SETTINGS_REG);
	}

	//
	// Register 0Bh. Regulator Control
	//
	if(g_bTRFExtAmpEnable)
	{
		TRF79x0_writeSingle(0x40, TRF79X0_REGULATOR_CONTROL_REG);
	}
	else
	{
		TRF79x0_writeSingle(0x00, TRF79X0_REGULATOR_CONTROL_REG);
	}

	//
	// Register 14h. FIFO IRQ Level
	//
	// RX High = 96 bytes , TX Low = 32 bytes
	TRF79x0_writeSingle(0x0F, TRF79X0_FIFO_IRQ_LEVEL);

	//
	// Register 18h. NFC Target level
	//
	TRF79x0_writeSingle(0x00, TRF79X0_NFC_TARGET_LEVEL_REG);

	TRF79x0_readSingle(ui8TempBuffer,TRF79X0_IRQ_STATUS_REG);

	if (g_bTestEnabled)
	{
		//
		// Register 02h. ISO14443B Options Reg
		//
		// TX EGT = 1 ETU, EOF = 10 etu
		TRF79x0_writeSingle(0x20,TRF79X0_ISO14443B_OPTIONS_REG);
	}

	return STATUS_SUCCESS;
}

/*
 * Reset TRF79x0 in current mode. (Needed for Target Mode to start polling for
 * supported modes and frequencies.)
 */
uint8_t TRF79x0_resetMode(void)
{
	if(g_sTrf7970Status.eCurrentMode == IDLE_MODE)
	{
		TRF79x0_idleMode();
	}
	else if(g_sTrf7970Status.eCurrentMode == INITIATOR_MODE)
	{
		TRF79x0_initiatorModeSetup(g_sTrf7970Status.sInitiatorMode,
								  g_sTrf7970Status.sInitiatorPayloadFrequency);
	}
	else if(g_sTrf7970Status.eCurrentMode == TARGET_MODE)
	{
		TRF79x0_targetModeSetup(g_sTargetSupportedTechnologies,
				               g_sTargetSupportedBitrates);
	}
	else
	{
		return STATUS_FAIL;
	}

	return STATUS_SUCCESS;
}

/*
 * * Reset FIFO.
 */
void TRF79x0_resetFifo(void)
{
	TRF79x0_directCommand(TRF79X0_RESET_FIFO_CMD);
}

uint8_t * TRF79x0_getNfcBuffer(void)
{
	return g_ui8FifoBuffer;
}

tTRF79x0_Status TRF79x0_getStatus(void)
{
	return g_sTrf7970Status;
}

void TRF79x0_setAutoSdd(uint8_t * pui8TargetId, uint8_t ui8IdLength, bool bNfcDep, bool bIsoDep)
{
	uint8_t ui8Temp;

	// UID Length Must be 4,7,or 10
	if(ui8IdLength == 0x04 || ui8IdLength == 0x07 || ui8IdLength == 0x0A)
	{
		// Check if is different than current DEP support
		if(	g_bNfcDepSupport != bNfcDep
			|| g_bIsoDepSuport != bIsoDep
			|| g_ui8IdLength != ui8IdLength)
		{
			g_ui8IdLength = ui8IdLength;
			// Copy the UID
			for(ui8Temp = 0; ui8Temp < ui8IdLength; ui8Temp++)
			{
				g_ui8TargetId[ui8Temp] = pui8TargetId[ui8Temp];
			}
			g_bNfcDepSupport = bNfcDep;
			g_bIsoDepSuport = bIsoDep;

			TRF79x0_initAutoSdd();
		}
	}


}

void TRF79x0_initAutoSdd(void)
{
	if(g_ui8IdLength > 0)
	{
		if(g_bIsoDepSuport == true)
		{
			// Indicate Layer 4 Compliance
			TRF79x0_writeSingle(0x01,TRF79X0_ISO14443B_OPTIONS_REG);
		}
		if(g_bNfcDepSupport == true)
		{
			// Not supported

			// Indicate NFC DEP Compliance
			TRF79x0_writeSingle(0x80,TRF79X0_SPECIAL_FUNC_2_REG);
		}

		if(g_ui8IdLength == 0x04)
		{
			// Auto SDD - Single UID
			TRF79x0_writeSingle(0x27,TRF79X0_NFC_TARGET_LEVEL_REG);
			TRF79x0_writeCont(g_ui8TargetId,TRF79X0_NFC_ID_REG,0x04);
		}
		else if(g_ui8IdLength == 0x07)
		{
			// Auto SDD - Double UID
			TRF79x0_writeSingle(0x67,TRF79X0_NFC_TARGET_LEVEL_REG);
			TRF79x0_writeCont(g_ui8TargetId,TRF79X0_NFC_ID_REG,0x07);
		}
		else if(g_ui8IdLength == 0x0A)
		{
			// Auto SDD - Tripple UID
			TRF79x0_writeSingle(0xA7,TRF79X0_NFC_TARGET_LEVEL_REG);
			TRF79x0_writeCont(g_ui8TargetId,TRF79X0_NFC_ID_REG,0x0A);
		}
		else
		{
			// Do Nothing
		}
	}
}

void TRF79x0_disableAutoSdd(void)
{
	g_bIsoDepSuport = false;
	g_bIsoDepSuport = false;
	g_ui8IdLength = 0;
}

void TRF79x0_setTargetSelect(bool bTargetStatus)
{
	g_sTrf7970Status.bTargetSelected = bTargetStatus;
}

void TRF79x0_setNfcMode(bool bNfcMode)
{
	g_sTrf7970Status.bNfcModeEnabled = bNfcMode;
}

void TRF79x0_setExtFieldLed(bool bExtField)
{
	if(bExtField == true)
	{
		NFC_RF_FIELD_LED_POUT |= NFC_RF_FIELD_LED_BIT;
	}
	else
	{
		NFC_RF_FIELD_LED_POUT &= ~NFC_RF_FIELD_LED_BIT;
	}
}

void TRF79x0_muteTimeMillisecond(uint32_t delay)
{
	if (g_bTestEnabled)
	{
		TRF79x0_directCommand(TRF79X0_STOP_DECODERS_CMD);
		MCU_delayMillisecond(delay);
		TRF79x0_directCommand(TRF79X0_RUN_DECODERS_CMD);
	}
	else
	{
		// Do nothing
	}
}

void TRF79x0_muteTimeMicrosecond(uint32_t delay)
{
	if (g_bTestEnabled)
	{
		TRF79x0_directCommand(TRF79X0_STOP_DECODERS_CMD);
		MCU_delayMicrosecond(delay);
		TRF79x0_directCommand(TRF79X0_RUN_DECODERS_CMD);
	}
	else
	{
		// Do nothing
	}
}

void TRF79x0_ignoreCmd(void)
{
	TRF79x0_directCommand(TRF79X0_STOP_DECODERS_CMD);
	TRF79x0_directCommand(TRF79X0_RUN_DECODERS_CMD);
}

void TRF79x0_testFlag(bool bSupportCertification)
{
	g_bTestEnabled = bSupportCertification;
}

void TRF79x0_setVersion(tTRF79x0_Version eTRFVersion)
{
	g_eTrf7970Version = eTRFVersion;
}

void TRF79x0_setType5OptionDelay(bool bT5TOptionDelay)
{
	g_bType5OptionDelay = bT5TOptionDelay & 0x01;
}

void TRF79x0_setExtAmplifer(bool bExtAmplifer)
{
	g_bTRFExtAmpEnable = bExtAmplifer & 0x01;
}

void TRF79x0_setPowerSupply(bool bPowerSupply5V)
{
	g_bTRFPowerSupply5V = bPowerSupply5V & 0x01;
}

#ifdef MSP432P401R_LAUNCHPAD_ENABLED
void TRF79x0_isrHandler(void)
{
    uint32_t status;

    status = GPIO_getEnabledInterruptStatus(TRF_IRQ_PORT);
    GPIO_clearInterruptFlag(TRF_IRQ_PORT, status);

    if(status & TRF_IRQ)
    {
        g_ui8IrqFlag = 0x01;
    }
}
#endif
