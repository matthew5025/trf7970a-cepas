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

#if (NFC_PEER_2_PEER_INITIATOR_ENABLED || NFC_READER_WRITER_ENABLED)

#if NFC_INITIATOR_DEBUG
	tNfcInitiatorState eTempNfcState = g_sCurrentPollState;
#endif

#if	(NFC_RW_T2T_ENABLED || NFC_RW_T4TA_ENABLED || NFC_P2P_PASSIVE_INIT_ENABLED || NFC_P2P_ACTIVE_INIT_ENABLED)
	NFC_A_init();
#endif
#if	(NFC_RW_T4TB_ENABLED)
	NFC_B_init();
#endif
#if	(NFC_RW_T3T_ENABLED || NFC_P2P_PASSIVE_INIT_ENABLED || NFC_P2P_ACTIVE_INIT_ENABLED)
	NFC_F_Init();
#endif

#if	(NFC_RW_T2T_ENABLED)
	NFC_RW_T2T_init();
#endif
#if	(NFC_RW_T3T_ENABLED)
	NFC_RW_T3T_init();
#endif
#if	(NFC_RW_T5T_ENABLED)
	NFC_RW_T5T_init();
#endif

#if	(NFC_P2P_PASSIVE_INIT_ENABLED || NFC_P2P_ACTIVE_INIT_ENABLED)
	NFCDEP_init();
#endif
#if	(NFC_RW_T4TA_ENABLED || NFC_RW_T4TB_ENABLED)
	ISODEP_init(true);
#endif

#if NFC_INITIATOR_DEBUG
	if(eTempNfcState != g_sCurrentPollState)
	{
		Serial_printf("Initiator In :",DEP_LAYER_DATA,NFC_INITIATOR_LAYER_DATA);
		NFC_Initiator_PrintMode(eTempNfcState);
		Serial_printf("Initiator Out :",DEP_LAYER_DATA,NFC_INITIATOR_LAYER_DATA);
		NFC_Initiator_PrintMode(g_sCurrentPollState);
	}
#endif

#endif
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
#if	(NFC_P2P_PASSIVE_INIT_ENABLED || NFC_P2P_ACTIVE_INIT_ENABLED)
	uint8_t * pui8NfcFId2;
	uint8_t ui8NfcFIdLength;
#endif

#if NFC_INITIATOR_DEBUG
	tNfcInitiatorState eTempNfcState = g_sCurrentPollState;
#endif
	switch(g_sCurrentPollState)
	{
	case NFC_INITIATOR_TECH_DETECTION:
		// Detect which technology is in proximity.

#if	(NFC_RW_T2T_ENABLED || NFC_RW_T4TA_ENABLED || NFC_P2P_PASSIVE_INIT_ENABLED)
		if(sInitiatorMode.bits.bPassiveTypeA == 1)
		{
			NFC_A_send_SENS_REQ();
			// 5 mS
			g_ui16PICCTimeOut = 5;
			break;
		}
#endif

#if (NFC_P2P_ACTIVE_INIT_ENABLED)
		if(sInitiatorMode.bits.bActiveTypeA == 1)
		{
			NFCDEP_stateMachine();
			// 20 mS
			g_ui16PICCTimeOut = 20;
			break;
		}
#endif

#if (NFC_RW_T4TB_ENABLED)
		if(sInitiatorMode.bits.bPassiveTypeB == 1)
		{
			NFC_B_stateMachine();
			// 5 mS
			g_ui16PICCTimeOut = 5;
			break;
		}
#endif

#if (NFC_RW_T3T_ENABLED || NFC_P2P_PASSIVE_INIT_ENABLED)
		if(sInitiatorMode.bits.bPassiveTypeF == 1)
		{
			// SC = 0xFFFF (wildcards), RC = 0x00, Slots = 0x00 (1 slots)
			NFC_F_send_SENSF_REQ(0xFFFF,0x00,0x03);
			// 20 mS
			g_ui16PICCTimeOut = 20;
			break;
		}
#endif

#if (NFC_P2P_ACTIVE_INIT_ENABLED)
		if(sInitiatorMode.bits.bActiveTypeF == 1)
		{
			NFCDEP_stateMachine();
			// 20 mS
			g_ui16PICCTimeOut = 20;
			break;
		}
#endif

#if (NFC_RW_T5T_ENABLED)
		if(sInitiatorMode.bits.bPassive15693 == 1)
		{
			NFC_RW_T5T_sendInventoryCmd(0x26,0x00,false);
			NFC_RW_T5T_stateMachine();
			// 10 mS
			g_ui16PICCTimeOut = 10;
			break;
		}
#endif

		break;
#if	(NFC_RW_T2T_ENABLED || NFC_RW_T4TA_ENABLED || NFC_P2P_PASSIVE_INIT_ENABLED)
	case NFC_INITIATOR_TECH_RESOLUTION_A_CL1:
		// Send 1st Cascade
		NFC_A_send_SDD_REQ(1);
		break;
	case NFC_INITIATOR_TECH_RESOLUTION_A_CL1_SEL:
		// Send 1st Cascade Select
		NFC_A_send_SEL_REQ(1);
		break;
	case NFC_INITIATOR_TECH_RESOLUTION_A_CL2:
		// Send 2nd Cascade
		NFC_A_send_SDD_REQ(2);
		break;
	case NFC_INITIATOR_TECH_RESOLUTION_A_CL2_SEL:
		// Send 2nd Cascade Select
		NFC_A_send_SEL_REQ(2);
		break;
	case NFC_INITIATOR_TECH_RESOLUTION_A_CL3:
		// Send 3rd Cascade
		NFC_A_send_SDD_REQ(3);
		break;
	case NFC_INITIATOR_TECH_RESOLUTION_A_CL3_SEL:
		// Send 3rd Cascade Select
		NFC_A_send_SEL_REQ(3);
		break;
#endif
	case NFC_INITIATOR_TECH_RESOLUTION_F:
		break;
#if	(NFC_P2P_PASSIVE_INIT_ENABLED || NFC_P2P_ACTIVE_INIT_ENABLED)
	case NFC_INITIATOR_DEVICE_ACTIVATION_A:
		// Select the Type A NFCID1 that was found in the Tech Resolution. If there is only one NFC_A device in the
		// field then the select stage is skipped.
		// Here the RATS / ATR_REQ + PSL_REQ is sent.
		if(sInitiatorMode.bits.bActiveTypeA == 1 || sInitiatorMode.bits.bPassiveTypeA == 1)
		{
			NFCDEP_stateMachine();
			g_ui16PICCTimeOut = NFCDEP_getLinkTimeOut();
		}
		break;
#endif
#if	(NFC_RW_T4TB_ENABLED)
	case NFC_INITIATOR_DEVICE_ACTIVATION_B:
		// The timeout is FWT_Activation (Listen) (4.833 mS) + T_POLL (20 mS)
		g_ui16PICCTimeOut = ISODEP_getLinkTimeOut();
		NFC_B_stateMachine();
		break;
#endif
#if	(NFC_RW_T4TA_ENABLED)
	case NFC_INITIATOR_DEVICE_ACTIVATION_A_TAG:
		// The timeout is FWT_Activation (Listen) (4.833 mS) + T_POLL (20 mS)
		g_ui16PICCTimeOut = 25;
		NFC_A_stateMachine();
		break;
#endif
#if	(NFC_P2P_PASSIVE_INIT_ENABLED || NFC_P2P_ACTIVE_INIT_ENABLED)
	case NFC_INITIATOR_DEVICE_ACTIVATION_F:
		// Required Delay to meet timing requirement of Tdelay+[TSN+1]*T(Timeslot)+FDT(F,POLL,MIN)
//		MCU_delayMillisecond(12); // This is calculated from the requirement but testing with MicroPross showed the delay was excessive.
		MCU_delayMillisecond(5);
		// Sends ATR_REQ + PSL (NFC DEP)
		NFC_F_getNFCID2(&pui8NfcFId2,&ui8NfcFIdLength);
		NFCDEP_setNFCID2i(pui8NfcFId2,8);
		NFCDEP_stateMachine();

		// RWTactivation + Delta RWT ==> ~1238 mS
		g_ui16PICCTimeOut = NFCDEP_getLinkTimeOut();
		break;
	case NFC_INITIATOR_NFC_DEP:
		NFCDEP_stateMachine();

		g_ui16PICCTimeOut = NFCDEP_getLinkTimeOut();
		break;
#endif
#if	(NFC_RW_T4TA_ENABLED || NFC_RW_T4TB_ENABLED)
	case NFC_INITIATOR_ISO_DEP:
		ISODEP_stateMachine();

		g_ui16PICCTimeOut = ISODEP_getLinkTimeOut();
		break;
#endif
#if (NFC_RW_T2T_ENABLED)
	case NFC_INITIATOR_TYPE_2_DEP:
		NFC_RW_T2T_stateMachine();

		// 10 mS (Max FDT for T2T)
		g_ui16PICCTimeOut = 10;
		break;
#endif
#if (NFC_RW_T3T_ENABLED)
	case NFC_INITIATOR_TYPE_3_DEP:
		NFC_RW_T3T_stateMachine();

		// 20.9 mS (Max FDT for T3T)
		g_ui16PICCTimeOut = 35;
		break;
#endif
#if (NFC_RW_T5T_ENABLED)
	case NFC_INITIATOR_TYPE_V_DEP:
		NFC_RW_T5T_stateMachine();

		g_ui16PICCTimeOut = 40;
		break;
#endif
#if (NFC_RW_T2T_ENABLED)
	case NFC_INITIATOR_TYPE_MFC_DEP:
		g_ui16PICCTimeOut = 30;

		break;
#endif
#if	(NFC_P2P_PASSIVE_INIT_ENABLED || NFC_P2P_ACTIVE_INIT_ENABLED)
	case NFC_INITIATOR_DEVICE_DEACTIVATION_NFC_DEP:
		// Call function to set the state of DEP in a deselect state
		NFCDEP_stateMachine();
		// 77 mS
		g_ui16PICCTimeOut = NFCDEP_getLinkTimeOut();
		break;
#endif
	}
#if NFC_INITIATOR_DEBUG
	if(eTempNfcState != g_sCurrentPollState)
	{
		Serial_printf("Initiator In :",NFC_INITIATOR_LAYER_DATA);
		NFC_Initiator_PrintMode(eTempNfcState);
		Serial_printf("Initiator Out :",NFC_INITIATOR_LAYER_DATA);
		NFC_Initiator_PrintMode(g_sCurrentPollState);
	}
#endif

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
#if (NFC_P2P_ACTIVE_INIT_ENABLED)
	tDepState eDepState;
#endif
#if	(NFC_RW_T4TA_ENABLED)
	tUNfcAState eNfcAState;
#endif
#if	(NFC_RW_T4TB_ENABLED)
	tNfcBPollState eNfcBState;
#endif
#if	(NFC_RW_T4TA_ENABLED || NFC_RW_T4TB_ENABLED)
	tIsoDepState eISODepState;
#endif
#if (NFC_P2P_PASSIVE_INIT_ENABLED || NFC_RW_T3T_ENABLED)
	uint8_t * pui8NFCID2;
	uint8_t ui8NfcFIdLength;
#endif

#if (NFC_PEER_2_PEER_INITIATOR_ENABLED)
	uint16_t ui16DepCommand;
#endif

#if NFC_INITIATOR_DEBUG
	tNfcInitiatorState eTempNfcState = g_sCurrentPollState;
#endif

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
#if (NFC_P2P_PASSIVE_INIT_ENABLED)
		ui16DepCommand = NFCDEP_getDEPCommand(pui8Payload,ui8Length);
#endif
	}
	else if(sInitiatorMode.bits.bActiveTypeF == 1
			&& (pui8Payload[0] == ui8Length))
	{
		ui8CommandLength = ui8Length;
#if (NFC_P2P_ACTIVE_INIT_ENABLED)
		ui16DepCommand = NFCDEP_getDEPCommand(pui8Payload,ui8Length);
#endif

	}
	else if(sInitiatorMode.bits.bActiveTypeA == 1
			&& ((pui8Payload[0]) == ui8Length))
	{
		ui8CommandLength = ui8Length;
#if (NFC_P2P_ACTIVE_INIT_ENABLED)
		ui16DepCommand = NFCDEP_getDEPCommand(pui8Payload,ui8Length);
#endif
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

#if	(NFC_RW_T2T_ENABLED || NFC_RW_T4TA_ENABLED || NFC_P2P_PASSIVE_INIT_ENABLED)
		if(sInitiatorMode.bits.bPassiveTypeA == 1)
		{
			if(NFC_A_processSENSRes(pui8Payload,ui8Length) == STATUS_SUCCESS)
			{
				MCU_delayMicrosecond(500);
				g_ui8TechnologyFound |= FOUND_TECHNOLOGY_A;
				g_sCurrentPollState = NFC_INITIATOR_TECH_RESOLUTION_A_CL1;
			}
			else
			{
				g_sCurrentPollState = NFC_INITIATOR_PROTOCOL_ERROR;
			}
			break;
		}
#endif

#if	(NFC_P2P_ACTIVE_INIT_ENABLED)
		if(sInitiatorMode.bits.bActiveTypeA == 1)
		{
			if(ui16DepCommand == ATR_RES_CMD)
			{
				eDepState = NFCDEP_processReceivedData(pui8Payload,ui8Length);
				if(eDepState == DEP_PSL_REQ_STATE)
				{
					g_ui8TechnologyFound |= FOUND_TECHNOLOGY_A;
					g_sCurrentPollState = NFC_INITIATOR_DEVICE_ACTIVATION_A;
				}
				else if(eDepState == DEP_ACTIVATED_STATE)
				{
					g_ui8TechnologyFound |= FOUND_TECHNOLOGY_A;
					g_sCurrentPollState = NFC_INITIATOR_NFC_DEP;
				}
			}
			break;
		}
#endif

#if (NFC_RW_T4TB_ENABLED)
		if(sInitiatorMode.bits.bPassiveTypeB == 1)
		{
			// Validate the response is valid
			if(NFC_B_processReceivedRsp(pui8Payload,ui8CommandLength) == NFC_B_POLL_ATTRIB_REQ)
			{
				// Delay to meet FDT_B_POLL_MIN
				MCU_delayMicrosecond(250);
				g_ui8TechnologyFound |= FOUND_TECHNOLOGY_B;
				g_sCurrentPollState = NFC_INITIATOR_DEVICE_ACTIVATION_B;
			}
			else
			{

			}
			break;
		}
#endif

#if (NFC_P2P_PASSIVE_INIT_ENABLED || NFC_RW_T3T_ENABLED)
		if(sInitiatorMode.bits.bPassiveTypeF == 1)
		{
			if(ui8CommandId == SENSF_RES_CMD)
			{
				NFC_F_processReceivedRequest(pui8Payload,ui8CommandLength);
				NFC_F_getNFCID2(&pui8NFCID2,&ui8NfcFIdLength);
				// Check if the Target supports NFCDEP
				if(pui8NFCID2[0] == 0x01 && pui8NFCID2[1] == 0xFE)
				{
					g_ui8TechnologyFound |= FOUND_TECHNOLOGY_F;
					g_sCurrentPollState = NFC_INITIATOR_DEVICE_ACTIVATION_F;
				}
				else
				{
					TRF79x0_setNfcMode(false);
					g_ui8TechnologyFound |= FOUND_TECHNOLOGY_F;
					g_sCurrentPollState = NFC_INITIATOR_TYPE_3_DEP;
				}
			}
			break;
		}
#endif

#if (NFC_P2P_ACTIVE_INIT_ENABLED)
		if(sInitiatorMode.bits.bActiveTypeF == 1)
		{
			if(ui16DepCommand == ATR_RES_CMD)
			{
				NFCDEP_processReceivedData(pui8Payload,ui8Length);
				g_ui8TechnologyFound |= FOUND_TECHNOLOGY_F;
				g_sCurrentPollState = NFC_INITIATOR_DEVICE_ACTIVATION_F;
			}
			break;
		}
#endif

#if (NFC_RW_T5T_ENABLED)
		if (sInitiatorMode.bits.bPassive15693 == 1)
		{
			if (NFC_RW_T5T_processReceivedData(pui8Payload,ui8Length) == NFC_RW_T5T_CONNECTION_INVENTORY_SUCCESS)
			{
				g_ui8TechnologyFound |= FOUND_TECHNOLOGY_V;
				g_sCurrentPollState = NFC_INITIATOR_TYPE_V_DEP;

				// Clear Inventory Status
				NFC_RW_T5T_getInventoryStatus();
			}
			break;
		}
#endif

		break;
#if	(NFC_RW_T2T_ENABLED || NFC_RW_T4TA_ENABLED || NFC_P2P_PASSIVE_INIT_ENABLED)
	case NFC_INITIATOR_TECH_RESOLUTION_A_CL1:
		if (NFC_A_processSDDResponse(pui8Payload,1) == STATUS_SUCCESS)
		{
			g_sCurrentPollState = NFC_INITIATOR_TECH_RESOLUTION_A_CL1_SEL;
		}
		else
		{
			g_sCurrentPollState = NFC_INITIATOR_PROTOCOL_ERROR;
		}
		break;
	case NFC_INITIATOR_TECH_RESOLUTION_A_CL1_SEL:
		NFC_A_processSELResponse(pui8Payload,ui8Length);
		if(NFC_A_isInitiatorSelected(1) == true)
		{
			// NFC DEP (P2P)
			if((NFC_A_isNfcDepSupported() == STATUS_SUCCESS) && (pui8Payload[0] & 0x40))
			{
				g_sCurrentPollState = NFC_INITIATOR_DEVICE_ACTIVATION_A;
			}
			// ISO DEP (T4TA)
			else if((NFC_A_isIsoDepSupported() == true) && (pui8Payload[0] & 0x20))
			{
				g_sCurrentPollState = NFC_INITIATOR_DEVICE_ACTIVATION_A_TAG;
			}
			// Type 2 Tag
			else
			{
				g_sCurrentPollState = NFC_INITIATOR_TYPE_2_DEP;
			}
		}
		else
		{
			g_sCurrentPollState = NFC_INITIATOR_TECH_RESOLUTION_A_CL2;
		}
		break;
	case NFC_INITIATOR_TECH_RESOLUTION_A_CL2:
		if (NFC_A_processSDDResponse(pui8Payload,2) == STATUS_SUCCESS)
		{
			g_sCurrentPollState = NFC_INITIATOR_TECH_RESOLUTION_A_CL2_SEL;
		}
		else
		{
			g_sCurrentPollState = NFC_INITIATOR_PROTOCOL_ERROR;
		}
		break;
	case NFC_INITIATOR_TECH_RESOLUTION_A_CL2_SEL:
		NFC_A_processSELResponse(pui8Payload,ui8Length);
		if(NFC_A_isInitiatorSelected(2) == true)
		{
			if((NFC_A_isNfcDepSupported() == true) && (pui8Payload[0] & 0x40))
			{
				g_sCurrentPollState = NFC_INITIATOR_DEVICE_ACTIVATION_A;
			}
			else if((NFC_A_isIsoDepSupported() == true) && (pui8Payload[0] & 0x20))
			{
				g_sCurrentPollState = NFC_INITIATOR_DEVICE_ACTIVATION_A_TAG;
			}
			else
			{
				g_sCurrentPollState = NFC_INITIATOR_TYPE_2_DEP;
			}
		}
		else
		{
			g_sCurrentPollState = NFC_INITIATOR_TECH_RESOLUTION_A_CL3;
		}
		break;
	case NFC_INITIATOR_TECH_RESOLUTION_A_CL3:
		if (NFC_A_processSDDResponse(pui8Payload,3) == STATUS_SUCCESS)
		{
			g_sCurrentPollState = NFC_INITIATOR_TECH_RESOLUTION_A_CL3_SEL;
		}
		else
		{
			g_sCurrentPollState = NFC_INITIATOR_PROTOCOL_ERROR;
		}
		break;
	case NFC_INITIATOR_TECH_RESOLUTION_A_CL3_SEL:
		NFC_A_processSELResponse(pui8Payload,ui8Length);
		if(NFC_A_isInitiatorSelected(3) == true)
		{
			if((NFC_A_isNfcDepSupported() == true) && (pui8Payload[0] & 0x40))
			{
				g_sCurrentPollState = NFC_INITIATOR_DEVICE_ACTIVATION_A;
			}
			else if((NFC_A_isIsoDepSupported() == true) && (pui8Payload[0] & 0x20))
			{
				g_sCurrentPollState = NFC_INITIATOR_DEVICE_ACTIVATION_A_TAG;
			}
			else
			{
				g_sCurrentPollState = NFC_INITIATOR_TYPE_2_DEP;
			}
		}
		else
		{
			g_sCurrentPollState = NFC_INITIATOR_TECH_DETECTION;
		}
		break;
#endif
#if	(NFC_P2P_PASSIVE_INIT_ENABLED || NFC_P2P_ACTIVE_INIT_ENABLED || NFC_RW_T3T_ENABLED)
	case NFC_INITIATOR_TECH_RESOLUTION_F:
		if(ui8CommandId == SENSF_RES_CMD)
		{
			// Store NFCID2
			NFC_F_processReceivedRequest(pui8Payload,ui8Length);
			g_sCurrentPollState = NFC_INITIATOR_DEVICE_ACTIVATION_F;
		}
		break;
#endif
#if	(NFC_P2P_PASSIVE_INIT_ENABLED || NFC_P2P_ACTIVE_INIT_ENABLED)
	case NFC_INITIATOR_DEVICE_ACTIVATION_A:
		ui16DepCommand = NFCDEP_getDEPCommand(pui8Payload,ui8Length);
		// Missing NFCID1 Handling
		if (ui16DepCommand == PSL_RES_CMD || ui16DepCommand == DEP_RES_CMD )
		{
			NFCDEP_processReceivedData(pui8Payload,ui8Length);
			g_sCurrentPollState = NFC_INITIATOR_NFC_DEP;
		}
		else if(ui16DepCommand == ATR_RES_CMD)
		{
			// Stay in same mode.
			NFCDEP_processReceivedData(pui8Payload,ui8Length);
		}
		break;
#endif
#if	(NFC_RW_T4TA_ENABLED)
	case NFC_INITIATOR_DEVICE_ACTIVATION_A_TAG:
		eNfcAState = NFC_A_processReceivedData(pui8Payload,ui8Length);

		if(eNfcAState == NFC_A_ACTIVATED)
		{
			g_sCurrentPollState = NFC_INITIATOR_ISO_DEP;
		}
		else if(eNfcAState == NFC_A_IDLE_STATE)
		{
			// Error Occurred
			g_sCurrentPollState = NFC_INITIATOR_PROTOCOL_ERROR;
		}
		else
		{
			// Do nothing
		}

		break;
#endif
#if	(NFC_RW_T4TB_ENABLED)
	case NFC_INITIATOR_DEVICE_ACTIVATION_B:
		eNfcBState = NFC_B_processReceivedRsp(pui8Payload,ui8Length);

		if(eNfcBState == NFC_B_POLL_ACTIVATED)
		{
			g_sCurrentPollState = NFC_INITIATOR_ISO_DEP;
		}
		else if(eNfcBState == NFC_B_PROTOCOL_ERROR)
		{
			g_sCurrentPollState = NFC_INITIATOR_PROTOCOL_ERROR;
		}

		break;
#endif
#if	(NFC_P2P_PASSIVE_INIT_ENABLED || NFC_P2P_ACTIVE_INIT_ENABLED)
	case NFC_INITIATOR_DEVICE_ACTIVATION_F:
		if(ui16DepCommand == ATR_RES_CMD)
		{
			NFCDEP_processReceivedData(pui8Payload,ui8Length);
		}
		else if(ui16DepCommand == PSL_RES_CMD || ui16DepCommand == DEP_RES_CMD )
		{
			NFCDEP_processReceivedData(pui8Payload,ui8Length);
			g_sCurrentPollState = NFC_INITIATOR_NFC_DEP;
		}
		else
		{
			g_sCurrentPollState = NFC_INITIATOR_TECH_DETECTION;
		}
		break;
	case NFC_INITIATOR_NFC_DEP:
		ui16DepCommand = NFCDEP_getDEPCommand(pui8Payload,ui8Length);
		if(ui16DepCommand == DEP_RES_CMD)
		{
			NFCDEP_processReceivedData(pui8Payload,ui8Length);
		}
		else
		{
			g_sCurrentPollState = NFC_INITIATOR_DEVICE_DEACTIVATION_NFC_DEP;
		}
		break;
#endif
#if	(NFC_RW_T4TA_ENABLED || NFC_RW_T4TB_ENABLED)
	case NFC_INITIATOR_ISO_DEP:
		eISODepState = ISODEP_processReceivedData(pui8Payload,ui8Length);

		if(eISODepState == ISO_DEP_PROTOCOL_ERROR)
		{
			g_sCurrentPollState = NFC_INITIATOR_PROTOCOL_ERROR;
		}

		break;
#endif
#if	(NFC_RW_T2T_ENABLED)
	case NFC_INITIATOR_TYPE_2_DEP:
		if(NFC_RW_T2T_processReceivedData(pui8Payload,ui8Length) == NFC_RW_T2T_PROTOCOL_ERROR)
		{
			g_sCurrentPollState = NFC_INITIATOR_PROTOCOL_ERROR;
		}
		break;
#endif
#if	(NFC_RW_T3T_ENABLED)
	case NFC_INITIATOR_TYPE_3_DEP:
		if(NFC_RW_T3T_processReceivedData(pui8Payload,ui8Length) == NFC_RW_T3T_PROTOCOL_ERROR)
		{
			g_sCurrentPollState = NFC_INITIATOR_PROTOCOL_ERROR;
		}
		break;
#endif
#if	(NFC_RW_T5T_ENABLED)
	case NFC_INITIATOR_TYPE_V_DEP:
		NFC_RW_T5T_processReceivedData(pui8Payload,ui8Length);
		break;
#endif
#if	(NFC_RW_T2T_ENABLED)
	case NFC_INITIATOR_TYPE_MFC_DEP:
		break;
#endif
#if	(NFC_P2P_PASSIVE_INIT_ENABLED || NFC_P2P_ACTIVE_INIT_ENABLED)
	case NFC_INITIATOR_DEVICE_DEACTIVATION_NFC_DEP:
		if(ui16DepCommand == RSL_RES_CMD || ui16DepCommand == DSL_RES_CMD)
		{
			NFCDEP_processReceivedData(pui8Payload,ui8Length);
			g_sCurrentPollState = NFC_INITIATOR_TECH_DETECTION;
		}
		else
		{
			g_sCurrentPollState = NFC_INITIATOR_TECH_DETECTION;
		}
		break;
#endif

	}
#if NFC_INITIATOR_DEBUG
	if(eTempNfcState != g_sCurrentPollState)
	{
		Serial_printf("Initiator In :",NFC_INITIATOR_LAYER_DATA);
		NFC_Initiator_PrintMode(eTempNfcState);
		Serial_printf("Initiator Out :",NFC_INITIATOR_LAYER_DATA);
		NFC_Initiator_PrintMode(g_sCurrentPollState);
	}
#endif

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
