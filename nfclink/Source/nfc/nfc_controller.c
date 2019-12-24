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

#if (NFC_PEER_2_PEER_INITIATOR_ENABLED || NFC_PEER_2_PEER_TARGET_ENABLED)
	g_sNfcStatus.sP2PModeCurrent.ui8byte = 0x00;
	g_sNfcStatus.sP2PModeSupport.ui8byte = 0x00;
	g_sNfcStatus.sP2PCommCurrent.ui8byte = 0x00;
	g_sNfcStatus.sP2PTargetCommSupport.ui8byte = 0x00;
	g_sNfcStatus.sP2PInitiatorCommSupport.ui8byte = 0x00;
#endif

#if NFC_CARD_EMULATION_ENABLED
	g_sNfcStatus.sCEModeSupport.ui8byte = 0x00;
	g_sNfcStatus.sRWModeCurrent.ui8byte = 0x00;
#endif

#if NFC_READER_WRITER_ENABLED
	g_sNfcStatus.sRWModeSupport.ui8byte = 0x00;
	g_sNfcStatus.sRWModeCurrent.ui8byte = 0x00;
	g_sNfcStatus.sRWCommSupport.ui16byte = 0x0000;
	g_sNfcStatus.sRWCommCurrent.ui16byte = 0x0000;
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

#if (NFC_PEER_2_PEER_INITIATOR_ENABLED || NFC_PEER_2_PEER_TARGET_ENABLED)
	if (g_sNfcStatus.sP2PModeSupport.ui8byte != 0x00)
	{
		// Check if Initiator is Supported
		if(g_sNfcStatus.sP2PModeSupport.bits.bInitiatorEnabled == 1)
		{
			g_sNfcStatus.sNfcModeSupport.bits.bNfcModePoll = 1;
		}
		// Check if Target is Supported
		if(g_sNfcStatus.sP2PModeSupport.bits.bTargetEnabled == 1)
		{
			g_sNfcStatus.sNfcModeSupport.bits.bNfcModeListen = 1;
		}
	}
#endif

#if NFC_CARD_EMULATION_ENABLED
	if (g_sNfcStatus.sCEModeSupport.ui8byte != 0x00)
	{
		g_sNfcStatus.sNfcModeSupport.bits.bNfcModeListen = 1;
	}
#endif

#if NFC_READER_WRITER_ENABLED
	if(g_sNfcStatus.sRWModeSupport.ui8byte != 0x00)
	{
		g_sNfcStatus.sNfcModeSupport.bits.bNfcModePoll = 1;
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
#if (NFC_PEER_2_PEER_INITIATOR_ENABLED || NFC_READER_WRITER_ENABLED)
	tTRF79x0_IRQFlag eTrfIrqStatus = IRQ_STATUS_IDLE;
	tTRF79x0_Status sTRF79x0Status;
	tNfcInitiatorState eInitiatorState;
	uint16_t ui16PICCTimeOut = 0;

	t_sTRF79x0_InitiatorMode sTRFInitMode;
	t_sTRF79x0_Bitrate sTRFBitrate;

#if (NFC_P2P_PASSIVE_INIT_ENABLED || NFC_READER_WRITER_ENABLED)
	bool bPassiveRfField = false;
	uint8_t ui8Status;
#endif

	bool bISODep = false;
	bool bNFCADep = false;
	bool bNFCFDep = false;

	switch(g_sNfcStatus.eNfcState)
	{
	case NFC_PROTOCOL_ACTIVATION:
		// Initialize all current mode variables
		g_sNfcStatus.sRWModeCurrent.ui8byte = 0x00;
		g_sNfcStatus.sRWCommCurrent.ui16byte = 0x0000;
		g_sNfcStatus.sP2PModeCurrent.ui8byte = 0x00;
		g_sNfcStatus.sP2PCommCurrent.ui8byte = 0x00;

		// Initialize all NFC layers to default values
		NFC_Initiator_Init();

		// Set DEP Support for ISO DEP and NFC DEP
#if (NFC_READER_WRITER_ENABLED)
		if (g_sNfcStatus.sRWModeSupport.bits.bNfcA == 1
				|| g_sNfcStatus.sRWModeSupport.bits.bNfcB == 1)
		{
			bISODep = true;
		}
#endif
#if (NFC_PEER_2_PEER_INITIATOR_ENABLED)

		if(g_sNfcStatus.sP2PModeSupport.bits.bInitiatorEnabled == 1)
		{
			if (g_sNfcStatus.sP2PInitiatorCommSupport.bits.bPassive106kbps
					|| g_sNfcStatus.sP2PInitiatorCommSupport.bits.bActive106kbps)
			{
				bNFCADep = true;
			}
			if (g_sNfcStatus.sP2PInitiatorCommSupport.bits.bPassive212kbps
					|| g_sNfcStatus.sP2PInitiatorCommSupport.bits.bActive212kbps
					|| g_sNfcStatus.sP2PInitiatorCommSupport.bits.bPassive424kbps
					|| g_sNfcStatus.sP2PInitiatorCommSupport.bits.bActive424kbps)
			{
				bNFCFDep = true;
			}
		}

#endif
		NFC_A_setDepSupport(bISODep,bNFCADep);
		NFC_F_setNfcDepSupport(bNFCFDep);

#if (NFC_P2P_ACTIVE_INIT_ENABLED)
		// Check for Active 106 kbps Technology Activation
		if(g_sNfcStatus.eNfcState == NFC_PROTOCOL_ACTIVATION &&
			g_sNfcStatus.sP2PModeSupport.bits.bInitiatorEnabled == 1 &&
			g_sNfcStatus.sP2PInitiatorCommSupport.bits.bActive106kbps == 1)
		{
			// Reset Modes
			sTRFInitMode.ui8byte = 0X00;
			sTRFBitrate.ui16word = 0X0000;
			sTRFInitMode.bits.bActiveTypeA = 1;
			sTRFBitrate.bits.b106kpbs = 1;

			if(TRF79x0_initiatorModeSetup(sTRFInitMode,sTRFBitrate) == STATUS_SUCCESS)
			{
				g_sNfcStatus.sP2PCommCurrent.bits.bActive106kbps = 1;

				TRF79x0_setNfcMode(true);
				// Delay Guard Time
				MCU_delayMillisecond(5);
				NFC_Initiator_StateMachine(sTRFInitMode);

				ui16PICCTimeOut = NFC_Initiator_GetPICCTimeOut();

				eTrfIrqStatus = NFC_waitForCommand(ui16PICCTimeOut);

				if(eTrfIrqStatus == IRQ_STATUS_RX_COMPLETE)
				{
					sTRF79x0Status = TRF79x0_getStatus();

					eInitiatorState = NFC_Initiator_ProccessResponse(g_pui8RxData,sTRF79x0Status.ui8FifoBytesReceived,sTRF79x0Status.sInitiatorMode);

					if(eInitiatorState == NFC_INITIATOR_DEVICE_ACTIVATION_A)
					{
						g_sNfcStatus.eNfcState = NFC_PARAMETER_SELECTION;

					}
					else if( eInitiatorState == NFC_INITIATOR_NFC_DEP)
					{
						g_sNfcStatus.eNfcState = NFC_DATA_EXCHANGE_PROTOCOL;
						g_sNfcStatus.sP2PModeCurrent.bits.bInitiatorEnabled = 1;
					}
				}
			}
		}

		// Check for Active 212 kbps Technology Activation
		if(g_sNfcStatus.eNfcState == NFC_PROTOCOL_ACTIVATION &&
			g_sNfcStatus.sP2PModeSupport.bits.bInitiatorEnabled == 1 &&
			g_sNfcStatus.sP2PInitiatorCommSupport.bits.bActive212kbps == 1)
		{
			// Reset Modes
			sTRFInitMode.ui8byte = 0X00;
			sTRFBitrate.ui16word = 0X0000;
			sTRFInitMode.bits.bActiveTypeF = 1;
			sTRFBitrate.bits.b212kpbs = 1;
			if(TRF79x0_initiatorModeSetup(sTRFInitMode,sTRFBitrate) == STATUS_SUCCESS)
			{
				g_sNfcStatus.sP2PCommCurrent.bits.bActive212kbps = 1;

				// Delay Guard Time
				MCU_delayMillisecond(20);

				NFC_Initiator_StateMachine(sTRFInitMode);

				ui16PICCTimeOut = NFC_Initiator_GetPICCTimeOut();

				eTrfIrqStatus = NFC_waitForCommand(ui16PICCTimeOut);

				if(eTrfIrqStatus == IRQ_STATUS_RX_COMPLETE)
				{
					sTRF79x0Status = TRF79x0_getStatus();

					eInitiatorState = NFC_Initiator_ProccessResponse(g_pui8RxData,sTRF79x0Status.ui8FifoBytesReceived,sTRF79x0Status.sInitiatorMode);

					if(eInitiatorState == NFC_INITIATOR_DEVICE_ACTIVATION_F)
					{
						g_sNfcStatus.eNfcState = NFC_PARAMETER_SELECTION;
					}
					else if( eInitiatorState == NFC_INITIATOR_NFC_DEP)
					{
						g_sNfcStatus.eNfcState = NFC_DATA_EXCHANGE_PROTOCOL;
						g_sNfcStatus.sP2PModeCurrent.bits.bInitiatorEnabled = 1;
					}
				}
			}
		}
#endif

		// Passive Communication Polling
#if	(NFC_RW_T2T_ENABLED || NFC_RW_T4TA_ENABLED || NFC_P2P_PASSIVE_INIT_ENABLED)
		// Check for NFC-A Passive 106 kbpzs Technology Activation
		if(g_sNfcStatus.eNfcState == NFC_PROTOCOL_ACTIVATION &&
			( (g_sNfcStatus.sP2PInitiatorCommSupport.bits.bPassive106kbps == 1 &&
					g_sNfcStatus.sP2PModeSupport.bits.bInitiatorEnabled == 1) ||
				(g_sNfcStatus.sRWModeSupport.bits.bNfcA == 1 &&
					 (g_sNfcStatus.sRWCommSupport.bits.bNfcA_106kbps == 1
						|| g_sNfcStatus.sRWCommSupport.bits.bNfcA_212kbps == 1
						|| g_sNfcStatus.sRWCommSupport.bits.bNfcA_424kbps == 1
						|| g_sNfcStatus.sRWCommSupport.bits.bNfcA_848kbps == 1
					 )
				)
			)
		)
		{
			// Reset Modes
			sTRFInitMode.ui8byte = 0X00;
			sTRFBitrate.ui16word = 0X0000;
			sTRFInitMode.bits.bPassiveTypeA = 1;
			sTRFBitrate.bits.b106kpbs = 1;

			// Check if the RF field has been already enabled
			if(bPassiveRfField == false)
			{
				ui8Status = TRF79x0_initiatorModeSetup(sTRFInitMode,sTRFBitrate);
				bPassiveRfField = true;
			}
			else
			{
				ui8Status = TRF79x0_switchInitiatorBaudRate(sTRFInitMode,sTRFBitrate,true);
			}

			// Setup TRF as NFC-A Passive Initiator @ 106kbps
			if(ui8Status  == STATUS_SUCCESS)
			{
				// Delay Guard Time - 5 mSec
				MCU_delayMillisecond(5);

				TRF79x0_setNfcMode(false);

				NFC_Initiator_StateMachine(sTRFInitMode);

				ui16PICCTimeOut = NFC_Initiator_GetPICCTimeOut();

				eTrfIrqStatus = NFC_waitForCommand(ui16PICCTimeOut);

				if(eTrfIrqStatus == IRQ_STATUS_RX_COMPLETE)
				{
					g_sNfcStatus.sP2PCommCurrent.bits.bPassive106kbps = 1;

					sTRF79x0Status = TRF79x0_getStatus();

					eInitiatorState = NFC_Initiator_ProccessResponse(g_pui8RxData,sTRF79x0Status.ui8FifoBytesReceived,sTRF79x0Status.sInitiatorMode);

					if(eInitiatorState == NFC_INITIATOR_TECH_RESOLUTION_A_CL1)
					{
						g_sNfcStatus.eNfcState = NFC_PARAMETER_SELECTION;
					}
					else if (eInitiatorState == NFC_INITIATOR_PROTOCOL_ERROR)
					{
						g_sNfcStatus.eNfcState = NFC_PROTOCOL_ACTIVATION;
					}
					else
					{
						g_sNfcStatus.sP2PCommCurrent.bits.bPassive106kbps = 0;
					}
				}
			}
		}
#endif

#if	(NFC_RW_T4TB_ENABLED)
		// Check for NFC-B Passive 106 kbps Technology Activation
		if(g_sNfcStatus.eNfcState == NFC_PROTOCOL_ACTIVATION
				&& g_sNfcStatus.sRWModeSupport.bits.bNfcB == 1
				&& (g_sNfcStatus.sRWCommSupport.bits.bNfcB_106kbps == 1
					|| g_sNfcStatus.sRWCommSupport.bits.bNfcB_212kbps == 1
					|| g_sNfcStatus.sRWCommSupport.bits.bNfcB_424kbps == 1
					|| g_sNfcStatus.sRWCommSupport.bits.bNfcB_848kbps == 1))
		{
			// Reset Modes
			sTRFInitMode.ui8byte = 0X00;
			sTRFBitrate.ui16word = 0X0000;
			sTRFInitMode.bits.bPassiveTypeB = 1;
			sTRFBitrate.bits.b106kpbs = 1;

			// Check if the RF field has been already enabled
			if(bPassiveRfField == false)
			{
				ui8Status = TRF79x0_initiatorModeSetup(sTRFInitMode,sTRFBitrate);
				bPassiveRfField = true;
			}
			else
			{
				ui8Status = TRF79x0_switchInitiatorBaudRate(sTRFInitMode,sTRFBitrate,true);
			}

			// Setup TRF as NFC-B Passive Initiator @ 106kbps
			if( ui8Status == STATUS_SUCCESS)
			{
				if(g_bSupportCertification == true)
				{
					// Delay Guard Time - 5 mSec ( spec requires 5 milliseconds)
					MCU_delayMillisecond(5);
				}
				else
				{
					// Delay Guard Time - 10 mSec to power RF430CL330 RF Powered Reference Design
					MCU_delayMillisecond(10);
				}

				TRF79x0_setNfcMode(false);

				NFC_Initiator_StateMachine(sTRFInitMode);

				ui16PICCTimeOut = NFC_Initiator_GetPICCTimeOut();

				eTrfIrqStatus = NFC_waitForCommand(ui16PICCTimeOut);

				if(eTrfIrqStatus == IRQ_STATUS_RX_COMPLETE)
				{
					sTRF79x0Status = TRF79x0_getStatus();

					eInitiatorState = NFC_Initiator_ProccessResponse(g_pui8RxData,sTRF79x0Status.ui8FifoBytesReceived,sTRF79x0Status.sInitiatorMode);

					if(eInitiatorState == NFC_INITIATOR_DEVICE_ACTIVATION_B)
					{
						g_sNfcStatus.eNfcState = NFC_PARAMETER_SELECTION;
					}
				}
			}
		}
#endif

#if (NFC_RW_T3T_ENABLED || NFC_P2P_PASSIVE_INIT_ENABLED)
		// Check for NFC-F Passive 212 kbps Technology Activation
		if(g_sNfcStatus.eNfcState == NFC_PROTOCOL_ACTIVATION
			&& ( (g_sNfcStatus.sP2PInitiatorCommSupport.bits.bPassive212kbps == 1 &&
					g_sNfcStatus.sP2PModeSupport.bits.bInitiatorEnabled == 1) ||
						(g_sNfcStatus.sRWModeSupport.bits.bNfcF == 1
						&& (g_sNfcStatus.sRWCommSupport.bits.bNfcF_212kbps == 1)
						)
				)
			)
		{
			// Reset Modes
			sTRFInitMode.ui8byte = 0X00;
			sTRFBitrate.ui16word = 0X0000;
			sTRFInitMode.bits.bPassiveTypeF = 1;
			sTRFBitrate.bits.b212kpbs = 1;

			TRF79x0_setNfcMode(true);

			// Needed due to NFC-F Tests where the TRF7970 is triggering a protocol error when the DTA is sending the NFC-F response.
			// The protocol error is triggered when the TRF7970A sends a SENS_REQ, and does not turn off the RF fiedl. However, if the
			// TRF7970 sends a ALL_REQ() the decoder does not trigger an error.
			// Solution: Turn off the RF field in between each polling command.
			if(g_bSupportCertification == true)
			{
				ui8Status = TRF79x0_initiatorModeSetup(sTRFInitMode,sTRFBitrate);
			}
			else
			{
				// Check if the RF field has been already enabled
				if(bPassiveRfField == false)
				{
					ui8Status = TRF79x0_initiatorModeSetup(sTRFInitMode,sTRFBitrate);
					bPassiveRfField = true;
				}
				else
				{
					ui8Status = TRF79x0_switchInitiatorBaudRate(sTRFInitMode,sTRFBitrate,true);
				}
			}

			if(ui8Status  == STATUS_SUCCESS)
			{
				// Delay Guard Time - 20 mSec
				MCU_delayMillisecond(20);

				TRF79x0_incrementInitiatorBaudRate(sTRFBitrate);

				NFC_Initiator_StateMachine(sTRFInitMode);

				ui16PICCTimeOut = NFC_Initiator_GetPICCTimeOut();

				eTrfIrqStatus = NFC_waitForCommand(ui16PICCTimeOut);

				if(eTrfIrqStatus == IRQ_STATUS_RX_COMPLETE)
				{
					sTRF79x0Status = TRF79x0_getStatus();

					eInitiatorState = NFC_Initiator_ProccessResponse(g_pui8RxData,sTRF79x0Status.ui8FifoBytesReceived,sTRF79x0Status.sInitiatorMode);
#if (NFC_P2P_PASSIVE_INIT_ENABLED)
					if(eInitiatorState == NFC_INITIATOR_DEVICE_ACTIVATION_F
							&& g_sNfcStatus.sP2PModeSupport.bits.bInitiatorEnabled == 1
							&& g_sNfcStatus.sP2PInitiatorCommSupport.bits.bPassive212kbps == 1)
					{
						g_sNfcStatus.eNfcState = NFC_PARAMETER_SELECTION;

						g_sNfcStatus.sP2PModeCurrent.bits.bInitiatorEnabled = 1;

						g_sNfcStatus.sP2PCommCurrent.bits.bPassive212kbps = 1;
					}
#endif
#if (NFC_RW_T3T_ENABLED)
					if(eInitiatorState == NFC_INITIATOR_TYPE_3_DEP && g_sNfcStatus.sRWModeSupport.bits.bNfcF == 1)
					{
						g_sNfcStatus.eNfcState = NFC_DATA_EXCHANGE_PROTOCOL;

						g_sNfcStatus.sRWModeCurrent.bits.bNfcF = 1;

						g_sNfcStatus.sRWCommCurrent.bits.bNfcF_212kbps = 1;

					}
#endif
				}
			}
		}

		// Check for NFC-F Passive 424 kbps Technology Activation (P2P)
		if(g_sNfcStatus.eNfcState == NFC_PROTOCOL_ACTIVATION &&

			((g_sNfcStatus.sP2PInitiatorCommSupport.bits.bPassive424kbps == 1 &&
			g_sNfcStatus.sP2PModeSupport.bits.bInitiatorEnabled == 1) ||
			(g_sNfcStatus.sRWModeSupport.bits.bNfcF == 1
			&& g_sNfcStatus.sRWCommSupport.bits.bNfcF_424kbps == 1)) )
		{
			// Reset Modes
			sTRFInitMode.ui8byte = 0X00;
			sTRFBitrate.ui16word = 0X0000;
			sTRFInitMode.bits.bPassiveTypeF = 1;
			sTRFBitrate.bits.b424kpbs = 1;

			TRF79x0_setNfcMode(true);

			// Needed due to NFC-F Tests where the TRF7970 is triggering a protocol error when the DTA is sending the NFC-F response.
			// The protocol error is triggered when the TRF7970A sends a SENS_REQ, and does not turn off the RF fiedl. However, if the
			// TRF7970 sends a ALL_REQ() the decoder does not trigger an error.
			// Solution: Turn off the RF field in between each polling command.
			if(g_bSupportCertification == true)
			{
				ui8Status = TRF79x0_initiatorModeSetup(sTRFInitMode,sTRFBitrate);
			}
			else
			{
				// Check if the RF field has been already enabled
				if(bPassiveRfField == false)
				{
					ui8Status = TRF79x0_initiatorModeSetup(sTRFInitMode,sTRFBitrate);
					bPassiveRfField = true;
				}
				else
				{
					ui8Status = TRF79x0_switchInitiatorBaudRate(sTRFInitMode,sTRFBitrate,true);
				}
			}

			if(ui8Status  == STATUS_SUCCESS)
			{
				// Delay Guard Time - 20 mSec
				MCU_delayMillisecond(20);

				TRF79x0_incrementInitiatorBaudRate(sTRFBitrate);

				NFC_Initiator_StateMachine(sTRFInitMode);

				ui16PICCTimeOut = NFC_Initiator_GetPICCTimeOut();

				eTrfIrqStatus = NFC_waitForCommand(ui16PICCTimeOut);

				if(eTrfIrqStatus == IRQ_STATUS_RX_COMPLETE)
				{
					sTRF79x0Status = TRF79x0_getStatus();

					eInitiatorState = NFC_Initiator_ProccessResponse(g_pui8RxData,sTRF79x0Status.ui8FifoBytesReceived,sTRF79x0Status.sInitiatorMode);
#if (NFC_P2P_PASSIVE_INIT_ENABLED)
					if(eInitiatorState == NFC_INITIATOR_DEVICE_ACTIVATION_F
							&& g_sNfcStatus.sP2PModeSupport.bits.bInitiatorEnabled == 1
							&& g_sNfcStatus.sP2PInitiatorCommSupport.bits.bPassive424kbps == 1)
					{
						g_sNfcStatus.eNfcState = NFC_PARAMETER_SELECTION;

						g_sNfcStatus.sP2PModeCurrent.bits.bInitiatorEnabled = 1;

						g_sNfcStatus.sP2PCommCurrent.bits.bPassive424kbps = 1;
					}
#endif
#if (NFC_RW_T3T_ENABLED)
					if(eInitiatorState == NFC_INITIATOR_TYPE_3_DEP && g_sNfcStatus.sRWModeSupport.bits.bNfcF == 1)
					{
						g_sNfcStatus.eNfcState = NFC_DATA_EXCHANGE_PROTOCOL;

						g_sNfcStatus.sRWModeCurrent.bits.bNfcF = 1;

						g_sNfcStatus.sRWCommCurrent.bits.bNfcF_424kbps = 1;
					}
#endif
				}
			}
		}
#endif

#if (NFC_RW_T5T_ENABLED)
		// Check for NFC-V Technology Activation
		if( (g_sNfcStatus.eNfcState == NFC_PROTOCOL_ACTIVATION) &&
			(g_sNfcStatus.sRWModeSupport.bits.bISO15693 == 1) &&
			(g_sNfcStatus.sRWCommSupport.bits.bISO15693_26_48kbps == 1))
		{
			// Reset Modes
			sTRFInitMode.ui8byte = 0X00;
			sTRFBitrate.ui16word = 0X0000;
			sTRFInitMode.bits.bPassive15693 = 1;

			TRF79x0_setNfcMode(false);

			if(g_sNfcStatus.sRWCommSupport.bits.bISO15693_26_48kbps == 1)
			{
				sTRFBitrate.bits.b26_48_kbps_1_out_4 = 1;
			}
			else
			{
				// For other possible 15693 bitrates
			}

			// Check if the RF field has been already enabled
			if(bPassiveRfField == false)
			{
				ui8Status = TRF79x0_initiatorModeSetup(sTRFInitMode,sTRFBitrate);
				bPassiveRfField = true;
			}
			else
			{
				ui8Status = TRF79x0_switchInitiatorBaudRate(sTRFInitMode,sTRFBitrate,true);
			}

			if(ui8Status  == STATUS_SUCCESS)
			{
				if(g_bSupportCertification == true)
				{
					// Delay Guard Time - 5 mSec ( spec requires 5 milliseconds)
					MCU_delayMillisecond(5);
				}
				else
				{
					// Delay Guard Time - 10 mSec to power RF430FRL RF Powered Reference Design
					MCU_delayMillisecond(10);
				}

				NFC_Initiator_StateMachine(sTRFInitMode);

				ui16PICCTimeOut = NFC_Initiator_GetPICCTimeOut();

				eTrfIrqStatus = NFC_waitForCommand(ui16PICCTimeOut);

				if(eTrfIrqStatus == IRQ_STATUS_RX_COMPLETE)
				{
					sTRF79x0Status = TRF79x0_getStatus();

					eInitiatorState = NFC_Initiator_ProccessResponse(g_pui8RxData,sTRF79x0Status.ui8FifoBytesReceived,sTRF79x0Status.sInitiatorMode);

					if(eInitiatorState == NFC_INITIATOR_TYPE_V_DEP)
					{
						g_sNfcStatus.eNfcState = NFC_DATA_EXCHANGE_PROTOCOL;

						g_sNfcStatus.sRWModeCurrent.bits.bISO15693 = 1;

						g_sNfcStatus.sRWCommCurrent.bits.bISO15693_26_48kbps = 1;
					}
				}
			}

		}
#endif
		break;
	case NFC_PARAMETER_SELECTION:
		sTRF79x0Status = TRF79x0_getStatus();

		eInitiatorState = NFC_Initiator_StateMachine(sTRF79x0Status.sInitiatorMode);

		ui16PICCTimeOut = NFC_Initiator_GetPICCTimeOut();

		eTrfIrqStatus = NFC_waitForCommand(ui16PICCTimeOut);

		if(eTrfIrqStatus == IRQ_STATUS_RX_COMPLETE)
		{
			sTRF79x0Status = TRF79x0_getStatus();

			eInitiatorState = NFC_Initiator_ProccessResponse(g_pui8RxData,sTRF79x0Status.ui8FifoBytesReceived,sTRF79x0Status.sInitiatorMode);
#if	(NFC_P2P_PASSIVE_INIT_ENABLED || NFC_P2P_ACTIVE_INIT_ENABLED)
			if(eInitiatorState == NFC_INITIATOR_DEVICE_ACTIVATION_A)
			{
				TRF79x0_setNfcMode(true);
			}

			// Check the Initiator State and for Passive Check when
			if(eInitiatorState == NFC_INITIATOR_NFC_DEP)
			{
				g_sNfcStatus.sP2PModeCurrent.bits.bInitiatorEnabled = 1;

				// Clear the current com bit rate to ensure only one bitrate set
				g_sNfcStatus.sP2PCommCurrent.ui8byte = 0x00;

				TRF79x0_setNfcMode(true);

				if(g_sNfcStatus.sP2PModeSupport.bits.bInitiatorEnabled != 1)
				{
					g_sNfcStatus.eNfcState = NFC_PROTOCOL_ACTIVATION;
					break;
				}
#if	(NFC_P2P_PASSIVE_INIT_ENABLED)
				if(sTRF79x0Status.sInitiatorMode.bits.bPassiveTypeF == 1)
				{
					if(sTRF79x0Status.sInitiatorPayloadFrequency.bits.b212kpbs)
					{
						g_sNfcStatus.sP2PCommCurrent.bits.bPassive212kbps = 1;
					}
					else if(sTRF79x0Status.sInitiatorPayloadFrequency.bits.b212kpbs)
					{
						g_sNfcStatus.sP2PCommCurrent.bits.bPassive424kbps = 1;
					}
				}
				else if(sTRF79x0Status.sInitiatorMode.bits.bPassiveTypeA == 1)
				{
					g_sNfcStatus.sP2PCommCurrent.bits.bPassive106kbps = 1;
				}
#endif
#if	(NFC_P2P_ACTIVE_INIT_ENABLED)
				if(sTRF79x0Status.sInitiatorMode.bits.bActiveTypeF == 1)
				{
					if(sTRF79x0Status.sInitiatorPayloadFrequency.bits.b212kpbs)
					{
						g_sNfcStatus.sP2PCommCurrent.bits.bActive212kbps = 1;
					}
					else if(sTRF79x0Status.sInitiatorPayloadFrequency.bits.b212kpbs)
					{
						g_sNfcStatus.sP2PCommCurrent.bits.bActive424kbps = 1;
					}
				}
				else if(sTRF79x0Status.sInitiatorMode.bits.bActiveTypeA == 1)
				{
					g_sNfcStatus.sP2PCommCurrent.bits.bActive106kbps = 1;
				}
#endif
				g_sNfcStatus.eNfcState = NFC_DATA_EXCHANGE_PROTOCOL;
			}
#endif
#if	(NFC_RW_T4TA_ENABLED || NFC_RW_T4TB_ENABLED)
			if(eInitiatorState == NFC_INITIATOR_ISO_DEP)
			{
#if	(NFC_RW_T4TA_ENABLED)
				// NFC-A RW Mode (106/212/424/848kbps)
				if(sTRF79x0Status.sInitiatorMode.bits.bPassiveTypeA == 1)
				{
					g_sNfcStatus.sRWModeCurrent.bits.bNfcA = 1;

					if(sTRF79x0Status.sInitiatorPayloadFrequency.bits.b106kpbs)
					{
						g_sNfcStatus.sRWCommCurrent.bits.bNfcA_106kbps = 1;
					}
					else if(sTRF79x0Status.sInitiatorPayloadFrequency.bits.b212kpbs)
					{
						g_sNfcStatus.sRWCommCurrent.bits.bNfcA_212kbps = 1;
					}
					else if(sTRF79x0Status.sInitiatorPayloadFrequency.bits.b424kpbs)
					{
						g_sNfcStatus.sRWCommCurrent.bits.bNfcA_424kbps = 1;
					}
					else if(sTRF79x0Status.sInitiatorPayloadFrequency.bits.b848kpbs)
					{
						g_sNfcStatus.sRWCommCurrent.bits.bNfcA_848kbps = 1;
					}
				}
#endif
#if	(NFC_RW_T4TB_ENABLED)
				// NFC-B RW Mode (106/212/424/848kbps)
				if(sTRF79x0Status.sInitiatorMode.bits.bPassiveTypeB == 1)
				{
					g_sNfcStatus.sRWModeCurrent.bits.bNfcB = 1;

					if(sTRF79x0Status.sInitiatorPayloadFrequency.bits.b106kpbs)
					{
						g_sNfcStatus.sRWCommCurrent.bits.bNfcB_106kbps = 1;
					}
					else if(sTRF79x0Status.sInitiatorPayloadFrequency.bits.b212kpbs)
					{
						g_sNfcStatus.sRWCommCurrent.bits.bNfcB_212kbps = 1;
					}
					else if(sTRF79x0Status.sInitiatorPayloadFrequency.bits.b424kpbs)
					{
						g_sNfcStatus.sRWCommCurrent.bits.bNfcB_424kbps = 1;
					}
					else if(sTRF79x0Status.sInitiatorPayloadFrequency.bits.b848kpbs)
					{
						g_sNfcStatus.sRWCommCurrent.bits.bNfcB_848kbps = 1;
					}
				}
#endif
				g_sNfcStatus.eNfcState = NFC_DATA_EXCHANGE_PROTOCOL;
			}
#endif
#if (NFC_RW_T2T_ENABLED)
			if(eInitiatorState == NFC_INITIATOR_TYPE_2_DEP)
			{
				g_sNfcStatus.sRWModeCurrent.bits.bNfcA = 1;

				// NFC-A RW Mode (106/212/424/848kbps)
				if(sTRF79x0Status.sInitiatorMode.bits.bPassiveTypeA == 1)
				{
					if(sTRF79x0Status.sInitiatorPayloadFrequency.bits.b106kpbs)
					{
						g_sNfcStatus.sRWCommCurrent.bits.bNfcA_106kbps = 1;
					}
					else if(sTRF79x0Status.sInitiatorPayloadFrequency.bits.b212kpbs)
					{
						g_sNfcStatus.sRWCommCurrent.bits.bNfcA_212kbps = 1;
					}
					else if(sTRF79x0Status.sInitiatorPayloadFrequency.bits.b424kpbs)
					{
						g_sNfcStatus.sRWCommCurrent.bits.bNfcA_424kbps = 1;
					}
					else if(sTRF79x0Status.sInitiatorPayloadFrequency.bits.b848kpbs)
					{
						g_sNfcStatus.sRWCommCurrent.bits.bNfcA_848kbps = 1;
					}
				}

				g_sNfcStatus.eNfcState = NFC_DATA_EXCHANGE_PROTOCOL;
			}
#endif
			if(eInitiatorState == NFC_INITIATOR_TECH_DETECTION
					|| eInitiatorState == NFC_INITIATOR_PROTOCOL_ERROR)
			{
				g_sNfcStatus.eNfcState = NFC_PROTOCOL_ACTIVATION;
			}
		}
		else if (eTrfIrqStatus == IRQ_STATUS_TIME_OUT)
		{
			eInitiatorState = NFC_Initiator_GetState();
			if ((eInitiatorState == NFC_INITIATOR_DEVICE_ACTIVATION_A
					|| eInitiatorState == NFC_INITIATOR_DEVICE_ACTIVATION_F))
			{
#if	(NFC_P2P_PASSIVE_INIT_ENABLED || NFC_P2P_ACTIVE_INIT_ENABLED)
				// Check for timeouts and if the maximum has been hit, go to Protocol activation mode.
				if (NFCDEP_triggerTimeout() == STATUS_SUCCESS)
				{
					g_sNfcStatus.eNfcState = NFC_PARAMETER_SELECTION;
				}
				else
				{
					g_sNfcStatus.eNfcState = NFC_PROTOCOL_ACTIVATION;
				}
#endif
			}
			else
			{
				g_sNfcStatus.eNfcState = NFC_PROTOCOL_ACTIVATION;
			}
		}
		else if(eTrfIrqStatus == IRQ_STATUS_OVERFLOW)
		{
			g_sNfcStatus.eNfcState = NFC_PROTOCOL_ACTIVATION;
		}
		else
		{
			g_sNfcStatus.eNfcState = NFC_PROTOCOL_ACTIVATION;
		}

		break;
	case NFC_DATA_EXCHANGE_PROTOCOL:
		sTRF79x0Status = TRF79x0_getStatus();

		eInitiatorState = NFC_Initiator_StateMachine(sTRF79x0Status.sInitiatorMode);

		ui16PICCTimeOut = NFC_Initiator_GetPICCTimeOut();

		eTrfIrqStatus = NFC_waitForCommand(ui16PICCTimeOut);

		if (eTrfIrqStatus == IRQ_STATUS_RX_COMPLETE)
		{
			sTRF79x0Status = TRF79x0_getStatus();

			eInitiatorState = NFC_Initiator_ProccessResponse(g_pui8RxData,sTRF79x0Status.ui8FifoBytesReceived,sTRF79x0Status.sInitiatorMode);

			if(eInitiatorState != NFC_INITIATOR_NFC_DEP
				&& eInitiatorState != NFC_INITIATOR_ISO_DEP
				&& eInitiatorState != NFC_INITIATOR_TYPE_2_DEP
				&& eInitiatorState != NFC_INITIATOR_TYPE_3_DEP
				&& eInitiatorState != NFC_INITIATOR_TYPE_V_DEP)
			{
				g_sNfcStatus.eNfcState = NFC_PROTOCOL_ACTIVATION;
			}
		}
		else if (eTrfIrqStatus == IRQ_STATUS_TIME_OUT)
		{
#if	(NFC_P2P_PASSIVE_INIT_ENABLED || NFC_P2P_ACTIVE_INIT_ENABLED)
			if(g_sNfcStatus.sP2PModeCurrent.bits.bInitiatorEnabled == 1)
			{
				// Check for timeouts and if the maximum has been hit, go to Protocol activation mode.
				if (NFCDEP_triggerTimeout() == STATUS_SUCCESS)
				{
					g_sNfcStatus.eNfcState = NFC_DATA_EXCHANGE_PROTOCOL;
				}
				else
				{
					g_sNfcStatus.eNfcState = NFC_PROTOCOL_ACTIVATION;
				}
				break;
			}
#endif
#if	(NFC_RW_T4TA_ENABLED || NFC_RW_T4TB_ENABLED)
			if(eInitiatorState == NFC_INITIATOR_ISO_DEP)
			{
				// Check for timeouts and if the maximum has been hit, go to protocol activation mode
				if(ISODEP_triggerProtocolError() == STATUS_SUCCESS)
				{
					g_sNfcStatus.eNfcState = NFC_DATA_EXCHANGE_PROTOCOL;
				}
				else
				{
					g_sNfcStatus.eNfcState = NFC_PROTOCOL_ACTIVATION;
				}
				break;
			}
#endif
#if (NFC_RW_T2T_ENABLED)
			if(eInitiatorState == NFC_INITIATOR_TYPE_2_DEP)
			{
				// Trigger fail responses
				if(NFC_RW_T2T_processReceivedData(0,0) == NFC_RW_T2T_PROTOCOL_ERROR)
				{
					g_sNfcStatus.eNfcState = NFC_PROTOCOL_ACTIVATION;
				}
				break;
			}
#endif
#if (NFC_RW_T3T_ENABLED)
			if(eInitiatorState == NFC_INITIATOR_TYPE_3_DEP)
			{
				// Trigger fail responses
				if(NFC_RW_T3T_processReceivedData(0,0) == NFC_RW_T3T_PROTOCOL_ERROR)
				{
					g_sNfcStatus.eNfcState = NFC_PROTOCOL_ACTIVATION;
				}
				break;
			}
#endif
#if (NFC_RW_T5T_ENABLED)
			if(eInitiatorState == NFC_INITIATOR_TYPE_V_DEP)
			{
				// Trigger fail responses
				if(NFC_RW_T5T_processReceivedData(0,0) == NFC_RW_T5T_PROTOCOL_ERROR)
				{
					g_sNfcStatus.eNfcState = NFC_PROTOCOL_ACTIVATION;
				}
				break;
			}
#endif
			g_sNfcStatus.eNfcState = NFC_PROTOCOL_ACTIVATION;

		}
		else if (eTrfIrqStatus == IRQ_STATUS_PROTOCOL_ERROR)
		{
#if	(NFC_P2P_PASSIVE_INIT_ENABLED || NFC_P2P_ACTIVE_INIT_ENABLED)
			if(g_sNfcStatus.sP2PModeCurrent.bits.bInitiatorEnabled == 1)
			{
				if(NFCDEP_triggerProtocolError() == STATUS_SUCCESS)
				{
					// Stay in the same state
					g_sNfcStatus.eNfcState = NFC_DATA_EXCHANGE_PROTOCOL;
				}
				else
				{
					g_sNfcStatus.eNfcState = NFC_PROTOCOL_ACTIVATION;
				}
				break;
			}
#endif
#if	(NFC_RW_T4TA_ENABLED || NFC_RW_T4TB_ENABLED)
			if(eInitiatorState == NFC_INITIATOR_ISO_DEP)
			{
				// Check for timoeuts and if the maximum has been hit, got to protocol activation mode
				if(ISODEP_triggerProtocolError() == STATUS_SUCCESS)
				{
					g_sNfcStatus.eNfcState = NFC_DATA_EXCHANGE_PROTOCOL;
				}
				else
				{
					g_sNfcStatus.eNfcState = NFC_PROTOCOL_ACTIVATION;
				}
				break;
			}
#endif
			g_sNfcStatus.eNfcState = NFC_PROTOCOL_ACTIVATION;
		}
		else
		{
			g_sNfcStatus.eNfcState = NFC_PROTOCOL_ACTIVATION;
		}

		break;
#if	(NFC_RW_T2T_ENABLED)
	case NFC_MIFARE_DATA_EXCHANGE_PROTOCOL:

		sTRF79x0Status = TRF79x0_getStatus();

		eInitiatorState = NFC_Initiator_StateMachine(sTRF79x0Status.sInitiatorMode);

		// Receive the response inside the Mifare Statemachine
		NFC_Initiator_ProccessResponse(g_pui8RxData,0x00,sTRF79x0Status.sInitiatorMode);

		break;
#endif
	}

	if(g_sNfcStatus.eNfcState == NFC_PROTOCOL_ACTIVATION)
	{
		TRF79x0_setField(EXT_FIELD_OFF);
	}

#endif
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

#if	(NFC_P2P_PASSIVE_TARG_ENABLED || NFC_P2P_ACTIVE_TARG_ENABLED || NFC_CE_T4TA_ENABLED)
	bool bISODep = false;
	bool bNFCADep = false;
#endif
#if	NFC_PEER_2_PEER_TARGET_ENABLED
	bool bNFCFDep = false;
#endif

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

#if (NFC_P2P_PASSIVE_TARG_ENABLED || NFC_P2P_ACTIVE_TARG_ENABLED)
			if(g_sNfcStatus.sP2PModeSupport.bits.bTargetEnabled == 1)
			{

#if NFC_P2P_ACTIVE_TARG_ENABLED
				if(g_sNfcStatus.sP2PTargetCommSupport.bits.bActive106kbps)
				{
					sTargetMode.bits.bActiveTypeA = 1;
					ui8TargetBitR.bits.b106kpbs = 1;
				}

				if(g_sNfcStatus.sP2PTargetCommSupport.bits.bActive212kbps)
				{
					sTargetMode.bits.bActiveTypeF = 1;
					ui8TargetBitR.bits.b212kpbs = 1;
				}

				if(g_sNfcStatus.sP2PTargetCommSupport.bits.bActive424kbps)
				{
					sTargetMode.bits.bActiveTypeF = 1;
					ui8TargetBitR.bits.b424kpbs = 1;
				}
#endif

#if NFC_P2P_PASSIVE_TARG_ENABLED
				if(g_sNfcStatus.sP2PTargetCommSupport.bits.bPassive106kbps)
				{
					sTargetMode.bits.bPassiveTypeA = 1;
					ui8TargetBitR.bits.b106kpbs = 1;
				}

				if(g_sNfcStatus.sP2PTargetCommSupport.bits.bPassive212kbps)
				{
					sTargetMode.bits.bPassiveTypeF = 1;
					ui8TargetBitR.bits.b212kpbs = 1;
				}

				if(g_sNfcStatus.sP2PTargetCommSupport.bits.bPassive424kbps)
				{
					sTargetMode.bits.bPassiveTypeF = 1;
					ui8TargetBitR.bits.b424kpbs = 1;
				}
#endif
			}
#endif
#if NFC_CE_T4TA_ENABLED
			if(g_sNfcStatus.sCEModeSupport.bits.bT4TAEnabled)
			{
				sTargetMode.bits.bPassiveTypeA = 1;
				ui8TargetBitR.bits.b106kpbs = 1;
			}
#endif
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
#if (NFC_P2P_PASSIVE_TARG_ENABLED || NFC_CE_T4TA_ENABLED)
				// If the field is enabled, and the current mode is passive A
				else if(sTRF79x0Status.sTargetMode.bits.bPassiveTypeA == 1)
				{
					// Enable Anti-collision Frames for 14443A.
					TRF79x0_writeSingle(0x00, TRF79X0_SPECIAL_FUNC_1_REG);
					// Commented out to improve consistency for test TC_LIS_NFCA_UND_BI_118
//					TRF79x0_targetModeSetup(sTargetMode, ui8TargetBitR);
				}
#endif
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

#if (NFC_CE_T4TA_ENABLED)
		if (g_sNfcStatus.sCEModeSupport.ui8byte != 0x00)
		{
			bISODep = true;
		}
#endif
#if (NFC_P2P_PASSIVE_TARG_ENABLED || NFC_P2P_ACTIVE_TARG_ENABLED)
		if(g_sNfcStatus.sP2PModeSupport.bits.bTargetEnabled)
		{
			if (g_sNfcStatus.sP2PTargetCommSupport.bits.bPassive106kbps
					|| g_sNfcStatus.sP2PTargetCommSupport.bits.bActive106kbps)
			{
				bNFCADep = true;
			}
			if (g_sNfcStatus.sP2PTargetCommSupport.bits.bPassive212kbps
					|| g_sNfcStatus.sP2PTargetCommSupport.bits.bActive212kbps
					|| g_sNfcStatus.sP2PTargetCommSupport.bits.bPassive424kbps
					|| g_sNfcStatus.sP2PTargetCommSupport.bits.bActive424kbps)
			{
				bNFCFDep = true;
			}
		}

#endif
#if (NFC_P2P_PASSIVE_TARG_ENABLED || NFC_P2P_ACTIVE_TARG_ENABLED || NFC_CE_T4TA_ENABLED)
		NFC_A_setDepSupport(bISODep,bNFCADep);
#endif
#if (NFC_P2P_PASSIVE_TARG_ENABLED || NFC_P2P_ACTIVE_TARG_ENABLED)
		NFC_F_setNfcDepSupport(bNFCFDep);
#endif

		//
		// Poll the IRQ flag for g_ui16LoopDelay mS.
		//
		eTrfIrqStatus = NFC_waitForCommand(g_ui16LoopDelay);

		if(eTrfIrqStatus == IRQ_STATUS_RX_COMPLETE)
		{
			sTRF79x0Status = TRF79x0_getStatus();

#if NFC_P2P_ACTIVE_TARG_ENABLED
			if(g_sNfcStatus.sP2PTargetCommSupport.bits.bActive106kbps)
			{
				// CRC bytes were received but are not accounted for within the NFC Target stack, so the received length must be modified.
				sTRF79x0Status.ui8FifoBytesReceived = sTRF79x0Status.ui8FifoBytesReceived - 2;
			}
#endif

			eTargetState = NFC_Target_StateMachine(g_pui8RxData,sTRF79x0Status.ui8FifoBytesReceived,sTRF79x0Status.sTargetMode);

			if(eTargetState != NFC_IDLE_STATE)
			{
				g_sNfcStatus.eNfcState = NFC_PARAMETER_SELECTION;
			}

#if (NFC_P2P_PASSIVE_TARG_ENABLED || NFC_P2P_ACTIVE_TARG_ENABLED)
			if(eTargetState == NFC_ATR_READY_A_STATE)
			{
				TRF79x0_setTargetSelect(true);
				TRF79x0_setNfcMode(true);
			}
#endif
		}
#if (NFC_P2P_PASSIVE_TARG_ENABLED || NFC_CE_T4TA_ENABLED)
		else if(eTrfIrqStatus == IRQ_STATUS_SDD_COMPLETE)
		{
			sTRF79x0Status = TRF79x0_getStatus();

			NFC_Target_SetState(NFC_ACTIVE_A_STATE);

			g_sNfcStatus.eNfcState = NFC_PARAMETER_SELECTION;
		}
#endif
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

#if (NFC_P2P_PASSIVE_TARG_ENABLED || NFC_P2P_ACTIVE_TARG_ENABLED)
			// Check for NFC-A command
			if((g_pui8RxData[0] == 0xF0)
					&& (g_pui8RxData[1] == (sTRF79x0Status.ui8FifoBytesReceived-1)))
			{
				TRF79x0_setNfcMode(true);
				TRF79x0_setTargetSelect(true);
				eTargetState = NFC_Target_StateMachine((g_pui8RxData+1),(sTRF79x0Status.ui8FifoBytesReceived-1),sTRF79x0Status.sTargetMode);
				// This would occur if you received a ATR_REQ
				if(eTargetState != NFC_ATR_READY_A_STATE)
				{
					TRF79x0_setNfcMode(false);
					TRF79x0_setTargetSelect(false);
				}
			}
#endif
			if((g_pui8RxData[0] != 0xF0)
					|| (g_pui8RxData[1] != (sTRF79x0Status.ui8FifoBytesReceived-1)))
			{
				eTargetState = NFC_Target_StateMachine(g_pui8RxData,sTRF79x0Status.ui8FifoBytesReceived,sTRF79x0Status.sTargetMode);
			}

			if(eTargetState == NFC_IDLE_STATE)
			{
				g_sNfcStatus.eNfcState = NFC_PROTOCOL_ACTIVATION;
			}
#if (NFC_P2P_PASSIVE_TARG_ENABLED || NFC_P2P_ACTIVE_TARG_ENABLED)
			else if(eTargetState == NFC_TARGET_F_STATE
					|| eTargetState == NFC_TARGET_A_STATE)
			{
				g_sNfcStatus.sP2PModeCurrent.bits.bTargetEnabled = 1;

				if(sTRF79x0Status.sTargetMode.bits.bPassiveTypeF == 1)
				{
					if(sTRF79x0Status.sTargetPayloadFrequency.bits.b212kpbs == 1)
					{
						g_sNfcStatus.sP2PCommCurrent.bits.bPassive212kbps = 1;
					}
					else if(sTRF79x0Status.sTargetPayloadFrequency.bits.b424kpbs == 1)
					{
						g_sNfcStatus.sP2PCommCurrent.bits.bPassive424kbps = 1;
					}
				}
				else if(sTRF79x0Status.sTargetMode.bits.bActiveTypeF == 1)
				{
					if(sTRF79x0Status.sTargetPayloadFrequency.bits.b212kpbs == 1)
					{
						g_sNfcStatus.sP2PCommCurrent.bits.bActive212kbps = 1;
					}
					else if(sTRF79x0Status.sTargetPayloadFrequency.bits.b424kpbs == 1)
					{
						g_sNfcStatus.sP2PCommCurrent.bits.bActive424kbps = 1;
					}
				}
				else if(sTRF79x0Status.sTargetMode.bits.bPassiveTypeA == 1)
				{
					if(sTRF79x0Status.sTargetPayloadFrequency.bits.b106kpbs == 1)
					{
						g_sNfcStatus.sP2PCommCurrent.bits.bPassive106kbps = 1;
					}
				}
				else if(sTRF79x0Status.sTargetMode.bits.bActiveTypeA == 1)
				{
					if(sTRF79x0Status.sTargetPayloadFrequency.bits.b106kpbs == 1)
					{
						g_sNfcStatus.sP2PCommCurrent.bits.bActive106kbps = 1;
					}
				}
				TRF79x0_setTargetSelect(true);
				TRF79x0_setNfcMode(true);
				g_sNfcStatus.eNfcState = NFC_DATA_EXCHANGE_PROTOCOL;
			}
#endif
#if NFC_CE_T4TA_ENABLED
			else if(eTargetState == NFC_CARD_EMULATOR_4A_STATE)
			{
				g_sNfcStatus.sCEModeCurrent.bits.bT4TAEnabled = 1;
				TRF79x0_setTargetSelect(true);
				g_sNfcStatus.eNfcState = NFC_DATA_EXCHANGE_PROTOCOL;
			}
#endif
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

#if (NFC_P2P_PASSIVE_TARG_ENABLED || NFC_CE_T4TA_ENABLED)
				if (sTRF79x0Status.sTargetMode.bits.bPassiveTypeA == 1
					&& sTRF79x0Status.sTargetPayloadFrequency.bits.b106kpbs == 1)
				{
					if (eTargetState == NFC_ATR_READY_A_STATE)
					{
						TRF79x0_ignoreCmd();
					}
					else if(eTargetState == NFC_READY_A_CL1_STATE ||
							eTargetState == NFC_READY_A_CL2_STATE ||
							eTargetState == NFC_READY_A_CL3_STATE ||
							eTargetState == NFC_ACTIVE_A_STATE)
					{
						TRF79x0_muteTimeMillisecond(MUTE_TIME_MS);
						// Based on TC_LIS_NFCA_UND_BI_18 - must return to IDLE
						g_sNfcStatus.eNfcState = NFC_PROTOCOL_ACTIVATION;
					}
					else
					{
						TRF79x0_muteTimeMillisecond(MUTE_TIME_MS);
						g_sNfcStatus.eNfcState = NFC_PARAMETER_SELECTION;
					}
				}
#endif
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
#if NFC_CE_T4TA_ENABLED
				else if(eTargetState == NFC_READY_CE_A_CL1_STATE ||
						eTargetState == NFC_READY_CE_A_CL2_STATE ||
						eTargetState == NFC_READY_CE_A_CL3_STATE ||
						eTargetState ==	NFC_ACTIVE_CE_A_STATE)
				{
					NFC_Target_SetState(NFC_SLEEP_A_STATE);
					TRF79x0_muteTimeMillisecond(MUTE_TIME_MS);
				}
#endif
			}
		}
		break;
	}

#if NFC_P2P_ACTIVE_TARG_ENABLED
	if(eTrfIrqStatus == IRQ_STATUS_TIME_OUT)
	{
		// Ensure RF Field is disabled for Active Target A/F
		sTRF79x0Status = TRF79x0_getStatus();

		if(sTRF79x0Status.bRfFieldOn == 1 &&
		(g_sNfcStatus.sP2PCommCurrent.bits.bActive106kbps == 1
		|| g_sNfcStatus.sP2PCommCurrent.bits.bActive212kbps == 1
		|| g_sNfcStatus.sP2PCommCurrent.bits.bActive424kbps == 1))
		{
			g_sNfcStatus.eNfcState = NFC_PROTOCOL_ACTIVATION;

			TRF79x0_resetMode();
		}
	}
#endif

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
#if (NFC_PEER_2_PEER_INITIATOR_ENABLED || NFC_PEER_2_PEER_TARGET_ENABLED)
	// Disable Peer 2 Peer
	NFC_P2P_disable();
#endif

#if NFC_CARD_EMULATION_ENABLED
	// Disable Card Emulation
	NFC_CE_disable();
#endif

#if (NFC_READER_WRITER_ENABLED)
	// Disable Reader/Writer
	NFC_RW_disable();
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

#if (NFC_PEER_2_PEER_INITIATOR_ENABLED || NFC_PEER_2_PEER_TARGET_ENABLED)
//*****************************************************************************
//
//! NFC_P2P_configure - configures the P2P modes and bitrates.
//!
//! \param sP2PModes is the target/initiator modes supported.
//! \param sP2PTargetBitrates is the target bitrates supported.
//! \param sP2PInitiatorBitrates is the initiator bitrates supported.
//!
//! This function enables the modes and corresponding bitrates for P2P mode.
//! It must be executed during the NFC configuration function to enable the P2P
//! support.
//!
//! \return None.
//
//*****************************************************************************
void NFC_P2P_configure(t_sNfcP2PMode sP2PModes,
					   t_sNfcP2PCommBitrate sP2PTargetBitrates,
					   t_sNfcP2PCommBitrate sP2PInitiatorBitrates)
{
	g_sNfcStatus.sP2PModeSupport = sP2PModes;
	g_sNfcStatus.sP2PTargetCommSupport = sP2PTargetBitrates;
	g_sNfcStatus.sP2PInitiatorCommSupport = sP2PInitiatorBitrates;

	g_sNfcStatus.eNfcState = NFC_PROTOCOL_ACTIVATION;
}
#endif

#if (NFC_PEER_2_PEER_INITIATOR_ENABLED || NFC_PEER_2_PEER_TARGET_ENABLED)
//*****************************************************************************
//
//! NFC_P2P_disable - disables the P2P mode functionality.
//!
//! This function disables the polling/listening for P2P functionality.
//!
//! \return None.
//
//*****************************************************************************
void NFC_P2P_disable(void)
{
	// Disable Peer 2 Peer
	g_sNfcStatus.sP2PModeSupport.ui8byte = 0x00;
	g_sNfcStatus.sP2PModeCurrent.ui8byte = 0x00;
	g_sNfcStatus.sP2PCommCurrent.ui8byte = 0x00;
	g_sNfcStatus.sP2PTargetCommSupport.ui8byte = 0x00;
	g_sNfcStatus.sP2PInitiatorCommSupport.ui8byte = 0x00;

	TRF79x0_idleMode();
}
#endif

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

#if (NFC_PEER_2_PEER_INITIATOR_ENABLED || NFC_PEER_2_PEER_TARGET_ENABLED)
//*****************************************************************************
//
//! NFC_P2P_setModes - sets NFC P2P mode.
//!
//! \param sP2PModes indicates the P2P modes (Initiator/Target) enabled.
//!
//! This function enables the P2P mode indicated by sP2PModes.
//!
//! \return None.
//
//*****************************************************************************
void NFC_P2P_setModes(t_sNfcP2PMode sP2PModes)
{
	g_sNfcStatus.sP2PModeSupport = sP2PModes;
}
#endif

#if NFC_PEER_2_PEER_TARGET_ENABLED
//*****************************************************************************
//
//! NFC_P2P_setTargetBitrates - sets the target NFC P2P bitrates.
//!
//! \param sP2PTargetBitrates indicates the target P2P bitrates (Active/Passive
//! 106kbps,212kbps, or 424kbps) enabled.
//!
//! This function enables the target P2P bitrates indicated by sP2PTargetBitrates.
//!
//! \return None.
//
//*****************************************************************************
void NFC_P2P_setTargetBitrates(t_sNfcP2PCommBitrate sP2PTargetBitrates)
{
	g_sNfcStatus.sP2PTargetCommSupport = sP2PTargetBitrates;
}
#endif

#if NFC_PEER_2_PEER_INITIATOR_ENABLED
//*****************************************************************************
//
//! NFC_P2P_setInitiatorBitrates - sets the initiator NFC P2P bitrates.
//!
//! \param sP2PInitiatorBitrates indicates the initiator P2P bitrates
//! (Active/Passive 106kbps,212kbps, or 424kbps) enabled.
//!
//! This function enables the initiator P2P bitrates indicated by
//! sP2PInitiatorBitrates.
//!
//! \return None.
//
//*****************************************************************************
void NFC_P2P_setInitiatorBitrates(t_sNfcP2PCommBitrate sP2PInitiatorBitrates)
{
	g_sNfcStatus.sP2PInitiatorCommSupport = sP2PInitiatorBitrates;
}
#endif

#if NFC_PEER_2_PEER_INITIATOR_ENABLED
//*****************************************************************************
//
//! NFC_P2P_getInitiatorSupportedBitrates - returns the supported initiator NFC P2P
//! bitrates.
//!
//! This function returns the supported initiator P2P bitrates.
//!
//! \return sP2PInitiatorCommSupport the supported Initiator bitrates.
//
//*****************************************************************************
t_sNfcP2PCommBitrate NFC_P2P_getInitiatorSupportedBitrates(void)
{
	return g_sNfcStatus.sP2PInitiatorCommSupport;
}

//*****************************************************************************
//
//! NFC_P2P_getInitiatorBitrate - returns the current initiator NFC P2P
//! bitrates.
//!
//! This function returns the current initiator P2P bitrate.
//!
//! \return sP2PCommCurrent the current Initiator bitrate.
//
//*****************************************************************************
t_sNfcP2PCommBitrate NFC_P2P_getInitiatorBitrate(void)
{
	return g_sNfcStatus.sP2PCommCurrent;
}
#endif


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

#if NFC_READER_WRITER_ENABLED
//*****************************************************************************
//
//! NFC_RW_configure - configures the RW modes and bitrates.
//!
//! \param sRWModes is the RW modes supported.
//! \param t_sNfcRWCommBitrate is the RW bitrates supported.
//!
//! This function enables the modes and corresponding bitrates for RW mode.
//! It must be executed during the NFC configuration function to enable the RW
//! support.
//!
//! \return None.
//
//*****************************************************************************
void NFC_RW_configure(t_sNfcRWMode sRWModes, t_sNfcRWCommBitrate sRWBitrates)
{
	g_sNfcStatus.sRWModeSupport = sRWModes;
	g_sNfcStatus.sRWCommSupport = sRWBitrates;

	g_sNfcStatus.eNfcState = NFC_PROTOCOL_ACTIVATION;
}
#endif

#if NFC_READER_WRITER_ENABLED
//*****************************************************************************
//
//! NFC_RW_disable - disables the RW mode functionality.
//!
//! This function disables the RW polling functionality.
//!
//! \return None.
//
//*****************************************************************************
void NFC_RW_disable(void)
{
	// Disable Reader/Writer
	g_sNfcStatus.sRWModeSupport.ui8byte = 0x00;
	g_sNfcStatus.sRWModeCurrent.ui8byte = 0x00;
	g_sNfcStatus.sRWCommSupport.ui16byte = 0x0000;
	g_sNfcStatus.sRWCommCurrent.ui16byte = 0x0000;

	TRF79x0_idleMode();
}
#endif

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
