//*****************************************************************************
//
// nfc_initiator.c - Implementation of the polling mode.
//
// Copyright (c) 2015 Texas Instruments Incorporated.  All rights reserved.
// TI Information - Selective Disclosure
//
//*****************************************************************************

#include "nfc_initiator.h"

#include "nfc_config.h"

//*****************************************************************************
//
//! \addtogroup nfc_initiator_api NFC Initiator API Functions
//! @{
//!
//! NFC Initiator Statemachine based on the NFC Forum Activity Spec Ver 1.0.
//
//*****************************************************************************

//*****************************************************************************
//
// Stores the technologies that were discovered.
//
//*****************************************************************************
static uint8_t g_ui8TechnologyFound;

//*****************************************************************************
//
// Stores the current initiator state.
//
//*****************************************************************************
tNfcInitiatorState g_sCurrentPollState;

//*****************************************************************************
//
// Stores the initiator PICC timoeout. (time the upper layer should wait for a
// PICC cmd)
//
//*****************************************************************************
uint16_t g_ui16PICCTimeOut;

//*****************************************************************************
//
//! NFC_Initiator_Init - Initializes the variables for the initiator
//! statemachine.
//!
//! This function must be called prior to any other NFC_Initiator function in
//! in order to properly initialize all variables which have been declared.
//! Failure to do so may cause the software to enter unknown states. It will
//! initialize the NFC_A, NFC_B, NFC_F, T2T, T3T, T5T, Mifare, NFC_DEP, and
//! ISO_DEP layers.
//!
//! \return None.
//
//*****************************************************************************
void NFC_Initiator_Init(void)
{
	// Initialize variables
	g_sCurrentPollState = NFC_INITIATOR_TECH_DETECTION;
	g_ui8TechnologyFound = FOUND_TECHNOLOGY_NONE;
	g_ui16PICCTimeOut = 0x0000;
}

//*****************************************************************************
//
//! NFC_Initiator_GetPICCTimeOut - returns current PICC timeout.
//!
//! This function returns the current PICC timeout.
//!
//! \return g_ui16PCDTimeOut the current PICC timeout.
//
//*****************************************************************************
uint16_t NFC_Initiator_GetPICCTimeOut(void)
{
	return g_ui16PICCTimeOut;
}

//*****************************************************************************
//
//! NFC_Initiator_SetState - sets the current initiator state.
//!
//! \param eState the initiator state
//!
//! This function sets g_eCurrentTargetState with a new initiator state (eState),
//! used inside the NFC_Initiator_StateMachine() function.
//!
//! \return None.
//
//*****************************************************************************
void NFC_Initiator_SetState(tNfcInitiatorState eState)
{
	g_sCurrentPollState = eState;
}

//*****************************************************************************
//
//! NFC_Initiator_StateMachine - Handles incoming commands from PCD, and sets
//! the PCD timeout based on the NFC technologies that get activated.
//! \param sInitiatorMode is the initiator mode enabled in the transceiver.
//!
//! This function handles the initiator statemachine for the application where the
//! transceiver is in polling mode. It handles the anti-collision /
//! activation of NFC-A, NFC-B and NFC-F. Once the technologies are activated
//! it calls the NFCDEP_stateMachine() function for P2P mode,
//! NFC_RW_T2T_stateMachine() function for T2T tags,
//! NFC_RW_T3T_stateMachine() function for T3T tags,
//! ISODEP_stateMachine() function for T4T tags,
//! MIFARE_stateMachine() function for Mifare Classic tags,
//! and NFC_RW_T5T_stateMachine() function T5T tags.
//!
//! \return g_sCurrentPollState the current initiator mode.
//
//*****************************************************************************
tNfcInitiatorState NFC_Initiator_StateMachine(t_sTRF79x0_InitiatorMode sInitiatorMode)
{
	switch(g_sCurrentPollState)
	{
	case NFC_INITIATOR_TECH_DETECTION:
		// Detect which technology is in proximity.
		break;
	case NFC_INITIATOR_TECH_RESOLUTION_F:
		break;
	}
	return g_sCurrentPollState;
}

//*****************************************************************************
//
//! NFC_Initiator_ProccessResponse - Handles incoming commands from PICC, and sets
//! the PICC timeout based on the NFC technologies that get activated.
//! \param pui8Payload is the pointer to data received from the PICC.
//! \param ui8Length is the number of bytes received from the PICC.
//! \param sInitiatorMode is the initiator mode enabled in the transceiver.
//!
//! This function processes the response from PICC and sets the state for the next
//! command to be sent from the PCD in the NFC_Initiator_StateMachine() function.
//!
//! \return g_sCurrentPollState the current initiator state.
//
//*****************************************************************************
tNfcInitiatorState NFC_Initiator_ProccessResponse(uint8_t * pui8Payload, uint8_t ui8Length, t_sTRF79x0_InitiatorMode sInitiatorMode)
{
	uint8_t ui8CommandId;
	uint8_t ui8CommandLength;
	// Command Length Validation
	if(sInitiatorMode.bits.bPassiveTypeA == 1
	   || sInitiatorMode.bits.bPassiveTypeB == 1)
	{
		ui8CommandLength = ui8Length;
		ui8CommandId = pui8Payload[0];
	}
	else if(sInitiatorMode.bits.bPassiveTypeF == 1
			&& (pui8Payload[0] == ui8Length))
	{
		ui8CommandLength = ui8Length;
		ui8CommandId = pui8Payload[1];
	}
	else if(sInitiatorMode.bits.bActiveTypeF == 1
			&& (pui8Payload[0] == ui8Length))
	{
		ui8CommandLength = ui8Length;

	}
	else if(sInitiatorMode.bits.bActiveTypeA == 1
			&& ((pui8Payload[0]) == ui8Length))
	{
		ui8CommandLength = ui8Length;
	}
	else if(sInitiatorMode.bits.bPassive15693 == 1)
	{
		// Do Nothing
	}
	else
	{
		// Error
		ui8CommandId = 0xFF;
		return g_sCurrentPollState;
	}

	switch(g_sCurrentPollState)
	{
	case NFC_INITIATOR_TECH_DETECTION:
		break;

	}

	return g_sCurrentPollState;
}

//*****************************************************************************
//
//! NFC_Initiator_GetState - returns current initator state.
//!
//! This function returns the current initiator state.
//!
//! \return g_sCurrentPollState the current initiator state.
//
//*****************************************************************************
tNfcInitiatorState NFC_Initiator_GetState(void)
{
	return g_sCurrentPollState;
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
