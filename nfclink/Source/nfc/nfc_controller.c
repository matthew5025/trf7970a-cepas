//*****************************************************************************
//
// nfc_controller.c - Implementation of NFC Controller APIs.
//
// Copyright (c) 2015 Texas Instruments Incorporated.  All rights reserved.
// TI Information - Selective Disclosure
//
//*****************************************************************************

#include "nfc_controller.h"

//*****************************************************************************
//
// Stores the NFC R/W, CE, and P2P mode and bitrates.
//
//*****************************************************************************
static struct _NFCRegisters {
	tNfcState eNfcState;
	tNfcMode sNfcModeSupport;
	tNfcMode sNfcModeCurrent;

	t_sNfcP2PMode sP2PModeCurrent;
	t_sNfcP2PMode sP2PModeSupport;
	t_sNfcP2PCommBitrate sP2PCommCurrent;
	t_sNfcP2PCommBitrate sP2PTargetCommSupport;
	t_sNfcP2PCommBitrate sP2PInitiatorCommSupport;

	t_sNfcCEMode sCEModeSupport;
	t_sNfcCEMode sCEModeCurrent;

	t_sNfcRWMode sRWModeSupport;
	t_sNfcRWMode sRWModeCurrent;
	t_sNfcRWCommBitrate sRWCommSupport;
	t_sNfcRWCommBitrate sRWCommCurrent;

}g_sNfcStatus;

//*****************************************************************************
//
// Stores the certification enabled boolean flag. Must be false for normal
// operation
//
//*****************************************************************************
static bool g_bSupportCertification;

//*****************************************************************************
//
// Stores the pointer to buffer where data is received from the transceiver.
//
//*****************************************************************************
static uint8_t * g_pui8RxData;

//*****************************************************************************
//
// Stores the time the listen statemachine will wait for incoming PCD command.
//
//*****************************************************************************
static uint16_t g_ui16LoopDelay;

//*****************************************************************************
//
//! \addtogroup nfc_controller_api NFC Controller APIs Functions
//! @{
//!
//! NFC Controller APIs.
//
//*****************************************************************************

//*****************************************************************************
//
//! NFC_init - initializes the NFC controller variables.
//!
//! This function intializes the NFC controller variables required for
//! valid operation when using the NFC_run() function. This function must
//! be executed before any of the other NFC controller functions.
//!
//! \return None.
//
//*****************************************************************************
void NFC_init(void)
{
	// NFC Modes Configuration

	// Initial Mode
	g_sNfcStatus.eNfcState = NFC_PROTOCOL_ACTIVATION;

	g_sNfcStatus.sNfcModeCurrent.ui8Byte = 0x00;
	g_sNfcStatus.sNfcModeSupport.ui8Byte = 0x00;


#if NFC_CARD_EMULATION_ENABLED
	g_sNfcStatus.sCEModeSupport.ui8byte = 0x00;
	g_sNfcStatus.sRWModeCurrent.ui8byte = 0x00;
#endif

	// Certification Mode Config
	g_bSupportCertification = 0;

	// Default value
	g_ui16LoopDelay = 500;

	// Store the nfc_buffer ptr in g_pui8RxData
	g_pui8RxData = TRF79x0_getNfcBuffer();
}

//*****************************************************************************
//
//! NFC_setListenTime - initializes the time the NFC stack will wait for
//! an incoming PCD command.
//!
//! \param ui16ListenTime the time the NFC_listenStateMachine() function will
//! wait to receive an incoming reader command.
//!
//! This function sets the time the NFC stack will wait for an incoming PCD
//! command.The default time for the delay time is 500 mS.
//!
//! \return None.
//
//*****************************************************************************
void NFC_setListenTime(uint16_t ui16ListenTime)
{
	g_ui16LoopDelay = ui16ListenTime;
}

//*****************************************************************************
//
//! NFC_run - executes the polling and listening statemachines.
//!
//! This function checks what modes were enabled for Card Emulation, Peer to
//! Peer, and Reader / Writer to configure the supporting NFC mode variables.
//! The supported NFC variables are used to decide which NFC technologies the
//! NFC_listenStateMachine() function will listen for, and which NFC
//! technologies the NFC_pollStateMachine() will try to activate. Once
//! a NFC technology is activated, this function will continue to activate
//! the necessary layers. Once the statemachine reaches the
//! NFC_DATA_EXCHANGE_PROTOCOL state, the application can then execute commands
//! based on NFC mode and technology that gets activated. The application must
//! use the NFC_P2P_getModeStatus(), NFC_RW_getModeStatus(), and
//! NFC_CE_getModeStatus() function to identify which technology is activated.
//!
//! \return sReturnNfcState returns the current state of the NFC stack.
//
//*****************************************************************************
tNfcState NFC_run(void)
{
	tNfcState sReturnNfcState;
	uint8_t ui8PrevState;

	ui8PrevState = g_sNfcStatus.sNfcModeSupport.ui8Byte;
	g_sNfcStatus.sNfcModeSupport.ui8Byte = 0x00;

#if NFC_CARD_EMULATION_ENABLED
	if (g_sNfcStatus.sCEModeSupport.ui8byte != 0x00)
	{
		g_sNfcStatus.sNfcModeSupport.bits.bNfcModeListen = 1;
	}
#endif

	// Check that the current mode was not activated
	if(g_sNfcStatus.eNfcState == NFC_PROTOCOL_ACTIVATION
			&& g_sNfcStatus.sNfcModeSupport.ui8Byte != 0x00)
	{
		// Check if no mode is selected
		if(g_sNfcStatus.sNfcModeCurrent.ui8Byte == 0)
		{
			// If poll is supported then switch to poll
			if(g_sNfcStatus.sNfcModeSupport.bits.bNfcModePoll == 1)
			{
				// Start by Polling
				g_sNfcStatus.sNfcModeCurrent.bits.bNfcModePoll = 1;
			}
			// If listen is supported then switch to listen
			else if(g_sNfcStatus.sNfcModeSupport.bits.bNfcModeListen == 1)
			{
				// Start by Listening
				g_sNfcStatus.sNfcModeCurrent.bits.bNfcModeListen = 1;
			}
			else
			{
				// Do nothing
			}
		}
		else
		{
			if(g_sNfcStatus.sNfcModeCurrent.bits.bNfcModeListen == 1
				&& g_sNfcStatus.sNfcModeSupport.bits.bNfcModePoll == 1)
			{
				// Set current Mode to Poll mode
				g_sNfcStatus.sNfcModeCurrent.ui8Byte = 0x00;
				g_sNfcStatus.sNfcModeCurrent.bits.bNfcModePoll = 1;
			}
			// Check if current mode is initiator mode and target mode is supported
			else if(g_sNfcStatus.sNfcModeCurrent.bits.bNfcModePoll == 1
				&& g_sNfcStatus.sNfcModeSupport.bits.bNfcModeListen == 1)
			{
				// Set current Mode to listen mode
				g_sNfcStatus.sNfcModeCurrent.ui8Byte = 0x00;
				g_sNfcStatus.sNfcModeCurrent.bits.bNfcModeListen = 1;
			}
			// If initiator is enabled and target is disabled
			else if(g_sNfcStatus.sNfcModeCurrent.bits.bNfcModePoll == 1
					&& g_sNfcStatus.sNfcModeSupport.bits.bNfcModeListen == 0)
			{
				MCU_delayMillisecond((uint32_t) g_ui16LoopDelay);
			}
		}
	}
	// If nothing is supported
	else if(g_sNfcStatus.sNfcModeSupport.ui8Byte == 0x00)
	{
		g_sNfcStatus.sNfcModeCurrent.ui8Byte = 0x00;
	}

	if(g_sNfcStatus.sNfcModeCurrent.bits.bNfcModeListen == 1)
	{
		sReturnNfcState = NFC_listenStateMachine();
	}
	else if(g_sNfcStatus.sNfcModeCurrent.bits.bNfcModePoll == 1)
	{
		sReturnNfcState = NFC_pollStateMachine();
	}
	else if(ui8PrevState != 0)
	{
		TRF79x0_idleMode();
	}

	return sReturnNfcState;
}

//*****************************************************************************
//
//! NFC_pollStateMachine - executes the polling statemachines.
//!
//! This function is called from the NFC_run() function when the NFC stack
//! enables the polling functionality. This function will try to activate
//! different NFC technologies based on the polling configurations.
//! Once a technology sends a successfull response, the NFC stack will
//! activate that technology without continuing to scan for other technologies.
//!
//! \return sReturnNfcState returns the current state of the NFC stack.
//
//*****************************************************************************
tNfcState NFC_pollStateMachine(void)
{
	return g_sNfcStatus.eNfcState;
}

//*****************************************************************************
//
//! NFC_listenStateMachine - executes the listen statemachines.
//!
//! This function is called from the NFC_run() function when the NFC stack
//! enables the listening functionality. Initially this function will wait to
//! receive a command from a reader for a 500 mS (default) or g_ui16LoopDelay
//! which can be modified with the NFC_setListenTime() function. Once a reader
//! sends a polling command for a technology we are waiting for, this function
//! will reply with a valid response.
//!
//! \return sReturnNfcState returns the current state of the NFC stack.
//
//*****************************************************************************
tNfcState NFC_listenStateMachine(void)
{
#if (NFC_P2P_PASSIVE_TARG_ENABLED || NFC_P2P_ACTIVE_TARG_ENABLED || NFC_CE_T4TA_ENABLED || NFC_CE_T4TB_ENABLED)
	tTRF79x0_IRQFlag eTrfIrqStatus = IRQ_STATUS_IDLE;
	tTRF79x0_Status sTRF79x0Status;
	tNfcTargetState eTargetState;
	uint16_t ui16PCDTimeOut = 0;
	t_sTRF79x0_TargetMode sTargetMode;
	t_sTRF79x0_Bitrate ui8TargetBitR;

	switch(g_sNfcStatus.eNfcState)
	{
	case NFC_TARGET_WAIT_FOR_ACTIVATION:
	case NFC_PROTOCOL_ACTIVATION:
		sTRF79x0Status = TRF79x0_getStatus();

		if(sTRF79x0Status.eCurrentMode == TARGET_MODE
			&& sTRF79x0Status.sTargetMode.ui8byte == 0x00)
		{
			// Do nothing
		}
		else
		{
			sTargetMode.ui8byte = 0x00;
			ui8TargetBitR.ui16word = 0x0000;

			g_sNfcStatus.eNfcState = NFC_TARGET_WAIT_FOR_ACTIVATION;
#if NFC_CE_T4TB_ENABLED
			if(g_sNfcStatus.sCEModeSupport.bits.bT4TBEnabled)
			{
				sTargetMode.bits.bPassiveTypeB = 1;
				ui8TargetBitR.bits.b106kpbs = 1;
			}
#endif

			if (g_bSupportCertification)
			{
				// Initialize TRF7970 Sniffer mode
				if(sTRF79x0Status.bRfFieldOn == false)
				{
					TRF79x0_targetModeSetup(sTargetMode, ui8TargetBitR);
				}
				else
				{
					// Do nothing
				}
			}
			else
			{
				TRF79x0_targetModeSetup(sTargetMode, ui8TargetBitR);
			}
		}

		// Initialize Target Variables
		NFC_Target_Init();

		//
		// Poll the IRQ flag for g_ui16LoopDelay mS.
		//
		eTrfIrqStatus = NFC_waitForCommand(g_ui16LoopDelay);

		if(eTrfIrqStatus == IRQ_STATUS_RX_COMPLETE)
		{
			sTRF79x0Status = TRF79x0_getStatus();

			eTargetState = NFC_Target_StateMachine(g_pui8RxData,sTRF79x0Status.ui8FifoBytesReceived,sTRF79x0Status.sTargetMode);

			if(eTargetState != NFC_IDLE_STATE)
			{
				g_sNfcStatus.eNfcState = NFC_PARAMETER_SELECTION;
			}
		}
		else if(eTrfIrqStatus == IRQ_STATUS_PROTOCOL_ERROR)
		{
			if (g_sNfcStatus.sCEModeSupport.bits.bT4TAEnabled == 1
					|| g_sNfcStatus.sCEModeSupport.bits.bT4TBEnabled == 1)
			{
				TRF79x0_muteTimeMillisecond(MUTE_TIME_MS);
			}
			else
			{
				TRF79x0_muteTimeMillisecond(MUTE_TIME_MS);
			}
		}
		else if(eTrfIrqStatus == IRQ_STATUS_TIME_OUT)
		{
			g_sNfcStatus.eNfcState = NFC_PROTOCOL_ACTIVATION;
		}

		break;
	case NFC_PARAMETER_SELECTION:
		ui16PCDTimeOut = NFC_Target_GetPCDTimeOut();

		eTrfIrqStatus = NFC_waitForCommand(ui16PCDTimeOut);

		if(eTrfIrqStatus == IRQ_STATUS_RX_COMPLETE)
		{
			sTRF79x0Status = TRF79x0_getStatus();

			if((g_pui8RxData[0] != 0xF0)
					|| (g_pui8RxData[1] != (sTRF79x0Status.ui8FifoBytesReceived-1)))
			{
				eTargetState = NFC_Target_StateMachine(g_pui8RxData,sTRF79x0Status.ui8FifoBytesReceived,sTRF79x0Status.sTargetMode);
			}

			if(eTargetState == NFC_IDLE_STATE)
			{
				g_sNfcStatus.eNfcState = NFC_PROTOCOL_ACTIVATION;
			}
#if NFC_CE_T4TB_ENABLED
			else if(eTargetState == NFC_CARD_EMULATOR_4B_STATE)
			{
				g_sNfcStatus.sCEModeCurrent.bits.bT4TBEnabled = 1;
				TRF79x0_setTargetSelect(true);
				g_sNfcStatus.eNfcState = NFC_DATA_EXCHANGE_PROTOCOL;
			}
#endif
		}
		else if(eTrfIrqStatus == IRQ_STATUS_TIME_OUT
				|| eTrfIrqStatus == IRQ_STATUS_PROTOCOL_ERROR)
		{
			sTRF79x0Status = TRF79x0_getStatus();
			// If a protocol error occurred, but the field was not disabled the state should stay the same
			if(sTRF79x0Status.bRfFieldOn == false)
			{
				g_sNfcStatus.eNfcState = NFC_PROTOCOL_ACTIVATION;
			}
			else if(eTrfIrqStatus == IRQ_STATUS_PROTOCOL_ERROR)
			{
				eTargetState = NFC_Target_getState();

				switch(eTargetState)
				{
				case NFC_SLEEP_AF_STATE:
				case NFC_SLEEP_A_STATE:
					// Stay in the same state
					break;
				case NFC_SLEEP_B_STATE:
					// Stay in the same state
					TRF79x0_muteTimeMillisecond(MUTE_TIME_MS);
					break;
				case NFC_ACTIVE_A_STATE:
					g_sNfcStatus.eNfcState = NFC_PROTOCOL_ACTIVATION;
					break;
				case NFC_READY_CE_A_CL1_STATE:
				case NFC_READY_CE_A_CL2_STATE:
				case NFC_READY_CE_A_CL3_STATE:
				case NFC_ACTIVE_CE_A_STATE:
					// This handles setting the correct NFC State for Card Emulation versus Peer 2 Peer due to ceritification specs
					NFC_Target_SetState(NFC_SLEEP_A_STATE);
					TRF79x0_muteTimeMillisecond(MUTE_TIME_MS);
					break;
				case NFC_READY_B_DECL_STATE:
				case NFC_READY_B_REQU_STATE:
					TRF79x0_resetMode();
					break;
				default:
					g_sNfcStatus.eNfcState = NFC_PARAMETER_SELECTION;
					break;
				}

#if NFC_CE_T4TB_ENABLED
				if (sTRF79x0Status.sTargetMode.bits.bPassiveTypeB == 1
						&& sTRF79x0Status.sTargetPayloadFrequency.bits.b106kpbs == 1)
				{
					TRF79x0_muteTimeMillisecond(MUTE_TIME_MS);
				}
#endif
			}
		}
		break;
	case NFC_DATA_EXCHANGE_PROTOCOL:
		TRF79x0_setTargetSelect(true);

		ui16PCDTimeOut = NFC_Target_GetPCDTimeOut();

		eTrfIrqStatus = NFC_waitForCommand(ui16PCDTimeOut);

		if(eTrfIrqStatus == IRQ_STATUS_RX_COMPLETE)
		{
			sTRF79x0Status = TRF79x0_getStatus();

			eTargetState = NFC_Target_StateMachine(g_pui8RxData,sTRF79x0Status.ui8FifoBytesReceived,sTRF79x0Status.sTargetMode);
			if(eTargetState == NFC_IDLE_STATE)
			{
				g_sNfcStatus.eNfcState = NFC_PROTOCOL_ACTIVATION;
			}
		}
		else if(eTrfIrqStatus == IRQ_STATUS_TIME_OUT
				|| eTrfIrqStatus == IRQ_STATUS_PROTOCOL_ERROR)
		{
			sTRF79x0Status = TRF79x0_getStatus();
			// If a protocol error occurred, but the field was not disabled the state should stay the same
			if(sTRF79x0Status.bRfFieldOn == false)
			{
				g_sNfcStatus.eNfcState = NFC_PROTOCOL_ACTIVATION;
			}
			else if(eTrfIrqStatus == IRQ_STATUS_PROTOCOL_ERROR)
			{
				eTargetState = NFC_Target_getState();
#if (NFC_CE_T4TA_ENABLED || NFC_CE_T4TB_ENABLED)
				if (eTargetState == NFC_SLEEP_A_STATE ||
						eTargetState == NFC_SLEEP_B_STATE ||
						eTargetState == NFC_CARD_EMULATOR_4A_STATE ||
						eTargetState == NFC_CARD_EMULATOR_4B_STATE)
				{
					g_sNfcStatus.eNfcState = NFC_DATA_EXCHANGE_PROTOCOL;
					TRF79x0_muteTimeMillisecond(MUTE_TIME_MS);
				}
#endif
			}
		}
		break;
	}

#endif
	return g_sNfcStatus.eNfcState;
}

//*****************************************************************************
//
//! NFC_disable - disables the NFC stack.
//!
//! This function disables the polling/listening statemachines.
//!
//! \return None.
//
//*****************************************************************************
void NFC_disable(void)
{
#if NFC_CARD_EMULATION_ENABLED
	// Disable Card Emulation
	NFC_CE_disable();
#endif

	TRF79x0_idleMode();

	g_sNfcStatus.eNfcState = NFC_DISABLED;
}

//*****************************************************************************
//
//! NFC_setSupportCertification - sets the NFC certification flag.
//!
//! \param bSupportCertification is a boolean flag indicating NFC certification
//! mode enabled.
//!
//! This function sets the value for g_bSupportCertification (NFC Certifcation mode
//! enabled). This function must be executed when performing tests with the
//! NFC Wave 1 Certification Test Suite.For normal NFC operation this function must
//! be called with bSupportCertification equal to false.
//!
//! \return None.
//
//*****************************************************************************
void NFC_setSupportCertification(bool bSupportCertification)
{
	g_bSupportCertification = bSupportCertification;
}

//*****************************************************************************
//
//! NFC_waitForCommand - this function waits to receive a command from
//! the PICC / PCD.
//!
//! \param ui16TimeOut is the time the function will wait to receive a command.
//!
//! This function waits to receive a command from the PICC / PCD. If there is
//! a protocol error, packet overflow, SDD complete event, or a valid packet is
//! received the function will return to the NFC_run() function.
//!
//! \return eTrfIrqStatus is the transceiver flag indicating the communication
//! status.
//
//*****************************************************************************
tTRF79x0_IRQFlag NFC_waitForCommand(uint16_t ui16TimeOut)
{
	tTRF79x0_IRQFlag eTrfIrqStatus = IRQ_STATUS_IDLE;

	tTRF79x0_Status sTRF79x0Status;

//	if(g_sNfcStatus.sNfcModeCurrent.bits.bNfcModeListen == 1)
//	{
//		SDM_TXENABLE_OFF;
//	}

	while(eTrfIrqStatus != IRQ_STATUS_TIME_OUT)
	{
		eTrfIrqStatus = TRF79x0_irqHandler(ui16TimeOut);

		if(eTrfIrqStatus == IRQ_STATUS_RX_COMPLETE
				|| eTrfIrqStatus == IRQ_STATUS_PROTOCOL_ERROR
				|| eTrfIrqStatus == IRQ_STATUS_NFC_SDD_COMPLETE
				|| eTrfIrqStatus == IRQ_STATUS_TIME_OUT
				|| eTrfIrqStatus == IRQ_STATUS_OVERFLOW)
		{
			break;
		}

		sTRF79x0Status = TRF79x0_getStatus();

		if(sTRF79x0Status.bRfFieldOn == false)
		{
			if (g_sNfcStatus.sP2PModeCurrent.bits.bInitiatorEnabled == 1)
			{
				// Do Nothing
			}
			else if( sTRF79x0Status.eCurrentMode == TARGET_MODE
					&& (sTRF79x0Status.sTargetMode.bits.bActiveTypeA == 1
						|| sTRF79x0Status.sTargetMode.bits.bActiveTypeF == 1))
			{
				// Do Nothing
			}
			else if( sTRF79x0Status.eCurrentMode == INITIATOR_MODE
					&& (sTRF79x0Status.sInitiatorMode.bits.bActiveTypeA == 1
						|| sTRF79x0Status.sInitiatorMode.bits.bActiveTypeF == 1))
			{
				// Do Nothing
			}
			else
			{
				eTrfIrqStatus = IRQ_STATUS_PROTOCOL_ERROR;
				break;
			}
		}
		else
		{
			// Do nothing
		}
	}

//	if(g_sNfcStatus.sNfcModeCurrent.bits.bNfcModeListen == 1)
//	{
//		SDM_TXENABLE_ON;
//	}

	return eTrfIrqStatus;
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************

//*****************************************************************************
//
//! \addtogroup nfc_p2p_api NFC P2P APIs Functions
//! @{
//!
//! NFC Peer to Peer APIs.
//
//*****************************************************************************

//*****************************************************************************
//
//! NFC_P2P_getModeStatus - checks if the P2P mode has been activated.
//!
//! \param psP2PMode is the pointer where the current P2P mode is stored if P2P
//! is actived.
//! \param psP2PBitrate is the pointer where the current P2P bitrate is stored
//! if P2P is actived.
//!
//! This function checks if P2P mode is activated. If it is, it stores the
//! current P2P mode and bitrates in the respectives variables.
//!
//! \return bP2PEnableStatus is a boolean indicating if P2P is activated.
//
//*****************************************************************************
bool NFC_P2P_getModeStatus(t_sNfcP2PMode * psP2PMode, t_sNfcP2PCommBitrate * psP2PBitrate)
{
	bool bP2PEnableStatus = true;

	if(g_sNfcStatus.sP2PModeCurrent.ui8byte)
	{
		*psP2PMode = g_sNfcStatus.sP2PModeCurrent;
		*psP2PBitrate = g_sNfcStatus.sP2PCommCurrent;
	}
	else
	{
		bP2PEnableStatus = false;
	}

	return bP2PEnableStatus;
}

//*****************************************************************************
//
//! NFC_P2P_getReceiveState - returns current SNEP receive statet.
//!
//! This function returns the current SNEP receive state when a SNEP connection
//! has been established over P2P.
//!
//! \return sReceiveStatus the current SNEP receive status.
//
//*****************************************************************************
tNfcP2PRxStatus NFC_P2P_getReceiveState(void)
{
	tNfcP2PRxStatus sReceiveStatus;

	 SNEP_getReceiveStatus(&sReceiveStatus.sDataReceivedStatus,
			 	 	 	   &sReceiveStatus.ui16DataReceivedLength,
			 	 	 	   &sReceiveStatus.pui8RxDataPtr,
			 	 	 	   &sReceiveStatus.ui32PacketSize);

	return sReceiveStatus;
}

//*****************************************************************************
//
//! NFC_P2P_sendNdefPacket - this function sends an NDEF packet using the Simple
//! NDEF Exchange Protocol layer.
//!
//! \param pui8Ndef is the pointer to the current fragment to be transmitted.
//! \param bPacketStart is the boolean flag indicating the first fragment of
//! a packet.
//! \param ui8FragmentLength is the fragment length
//! \param ui32PacketSize is the complete packet size
//!
//! This function checks if there is available space in the SNEP buffer, and queues
//! the data to be sent.
//!
//! \return ui8TransmittedBytes the number of bytes queued in the SNEP layer.
//
//*****************************************************************************
uint8_t NFC_P2P_sendNdefPacket(uint8_t * pui8Ndef,
								bool bPacketStart,
								uint8_t ui8FragmentLength,
								uint32_t ui32PacketSize)
{
	uint8_t ui8AvailableFragmentLength;
	uint8_t ui8TransmittedBytes;

	// Get fragment buffer availability
	ui8AvailableFragmentLength = SNEP_getTxBufferStatus();

	// Check if the size of the fragment is larger than the available frag size.
	// Else send a fragment with the largest size available.
	if(ui8FragmentLength > ui8AvailableFragmentLength)
	{
		ui8FragmentLength = ui8AvailableFragmentLength;
	}

	// Check if the first fragment of the packet
	if(bPacketStart == true && ui8FragmentLength > 0 && ui32PacketSize > 0)
	{
		// Setup the SNEP packet.
		if( SNEP_setupPacket( pui8Ndef, ui32PacketSize,ui8FragmentLength) != STATUS_SUCCESS)
		{
			// Check if the data was setup properly
			ui8FragmentLength = 0;
		}
	}
	else if(ui8FragmentLength > 0 && ui32PacketSize > 0)
	{
		// Queue Data
		if (SNEP_enqueue(pui8Ndef, ui8FragmentLength) != STATUS_SUCCESS)
		{
			// Check if the data was queued properly
			ui8FragmentLength = 0;
		}
	}
	else
	{
		// Do nothing
	}

	ui8TransmittedBytes = ui8FragmentLength;

	return ui8TransmittedBytes;
}


//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************


//*****************************************************************************
//
//! \addtogroup nfc_rw_api NFC RW APIs Functions
//! @{
//!
//! NFC Reader/Writer APIs.
//
//*****************************************************************************

//*****************************************************************************
//
//! NFC_RW_getModeStatus - checks if the RW mode has been activated.
//!
//! \param psRWMode is the pointer where the current RW mode is stored if RW
//! is actived.
//! \param psRWBitrate is the pointer where the current RW bitrate is stored
//! if RW is actived.
//!
//! This function checks if RW mode is activated. If it is, it stores the
//! current RW mode and bitrates in the respectives variables.
//!
//! \return bRwEnableStatus is a boolean indicating if RW is activated.
//
//*****************************************************************************
bool NFC_RW_getModeStatus(t_sNfcRWMode * psRWMode, t_sNfcRWCommBitrate * psRWBitrate)
{
	bool bRwEnableStatus = true;

	if(g_sNfcStatus.sRWModeCurrent.ui8byte)
	{
		*psRWMode = g_sNfcStatus.sRWModeCurrent;
		*psRWBitrate = g_sNfcStatus.sRWCommCurrent;
	}
	else
	{
		bRwEnableStatus = false;
	}

	return bRwEnableStatus;
}

//*****************************************************************************
//
//! NFC_RW_getRWSuportedBitrates - returns the supported initiator NFC RW
//! bitrates.
//!
//! This function returns the supported RW bitrates.
//!
//! \return sRWCommSupport the supported RW bitrates.
//
//*****************************************************************************
t_sNfcRWCommBitrate NFC_RW_getRWSuportedBitrates(void)
{
	return g_sNfcStatus.sRWCommSupport;
}

//*****************************************************************************
//
//! NFC_RW_triggerRWProtocolError - disables the RF field and resets the polling
//! statemachine.
//!
//! This function triggers a protocol error disabling the RF field. It should
//! be used when the NFC stack is in polling mode.
//!
//! \return None.
//
//*****************************************************************************
void NFC_RW_triggerRWProtocolError(void)
{
	g_sNfcStatus.eNfcState = NFC_PROTOCOL_ACTIVATION;
	TRF79x0_setField(EXT_FIELD_OFF);
}

//*****************************************************************************
//
//! NFC_RW_enableMifareMode - Enables the Mifare Data Exchange Protocol State.
//!
//! This function switches the current initiator state to
//! NFC_MIFARE_DATA_EXCHANGE_PROTOCOL. This is required for Mifare reading and
//! writing functionality.
//!
//! \return None.
//
//*****************************************************************************
void NFC_RW_enableMifareMode(void)
{
	NFC_Initiator_SetState(NFC_INITIATOR_TYPE_MFC_DEP);

	g_sNfcStatus.eNfcState = NFC_MIFARE_DATA_EXCHANGE_PROTOCOL;
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************

//*****************************************************************************
//
//! \addtogroup nfc_ce_api NFC CE APIs Functions
//! @{
//!
//! NFC Card Emulation APIs.
//
//*****************************************************************************

//*****************************************************************************
//
//! NFC_CE_getModeStatus - checks if the CE mode has been activated.
//!
//! \param psCEMode is the pointer where the current CE mode is stored if CE
//! is actived.
//!
//! This function checks if CE mode is activated. If it is, it stores the
//! current CE mode in psCEMode.
//!
//! \return bCEEnableStatus is a boolean indicating if CE is activated.
//
//*****************************************************************************
bool NFC_CE_getModeStatus(t_sNfcCEMode * psCEMode)
{
	bool bCEEnableStatus = true;

	if(g_sNfcStatus.sCEModeCurrent.ui8byte)
	{
		*psCEMode = g_sNfcStatus.sCEModeCurrent;
	}
	else
	{
		bCEEnableStatus = false;
	}

	return bCEEnableStatus;
}

#if NFC_CARD_EMULATION_ENABLED
//*****************************************************************************
//
//! NFC_CE_configure - configures the RW modes and bitrates.
//!
//! \param sCEModes is the CE modes supported.
//!
//! This function enables the modes and corresponding bitrates for CE mode.
//! It must be executed during the NFC configuration function to enable the CE
//! support.
//!
//! \return None.
//
//*****************************************************************************
void NFC_CE_configure(t_sNfcCEMode sCEModes)
{
	g_sNfcStatus.sCEModeSupport = sCEModes;

	g_sNfcStatus.eNfcState = NFC_PROTOCOL_ACTIVATION;
}
#endif

#if NFC_CARD_EMULATION_ENABLED
//*****************************************************************************
//
//! NFC_CE_disable - disables the CE mode functionality.
//!
//! This function disables the CE listening functionality.
//!
//! \return None.
//
//*****************************************************************************
void NFC_CE_disable(void)
{
	// Disable Card Emulation
	g_sNfcStatus.sCEModeSupport.ui8byte = 0x00;
	g_sNfcStatus.sCEModeCurrent.ui8byte = 0x00;

	// Turn off the RF field
	TRF79x0_idleMode();
}
#endif


//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
