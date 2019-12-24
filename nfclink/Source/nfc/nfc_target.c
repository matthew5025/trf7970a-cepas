//*****************************************************************************
//
// nfc_target.c - Implementation of the target/listen mode.
//
// Copyright (c) 2015 Texas Instruments Incorporated.  All rights reserved.
// TI Information - Selective Disclosure
//
//*****************************************************************************

#include "nfc_target.h"

//*****************************************************************************
//
//! \addtogroup nfc_target_api NFC Target API Functions
//! @{
//!
//! NFC Target Statemachine based on the NFC Forum Activity Spec Ver 1.0.
//
//*****************************************************************************

//*****************************************************************************
//
// Stores the current target state.
//
//*****************************************************************************
static tNfcTargetState g_eCurrentTargetState;

//*****************************************************************************
//
// Stores the target PCD timoeout. (time the upper layer should wait for a
// PCD cmd)
//
//*****************************************************************************
static uint16_t g_ui16PCDTimeOut;

//*****************************************************************************
//
//! NFC_Target_Init - Initializes the variables for the target statemachine.
//!
//! This function must be called prior to any other NFC_Target function in
//! in order to properly initialize all variables which have been declared.
//! Failure to do so may cause the software to enter unknown states. It will
//! initialize the NFC_A, NFC_B, NFC_F, NFC_DEP, and ISO_DEP layers.
//!
//! \return None.
//!
//
//*****************************************************************************
void NFC_Target_Init()
{
	g_eCurrentTargetState = NFC_IDLE_STATE;
	g_ui16PCDTimeOut = 0x0000;

#if NFC_TARGET_DEBUG
	tNfcTargetState eTempNfcState = g_eCurrentTargetState;
#endif

#if (NFC_P2P_PASSIVE_TARG_ENABLED || NFC_P2P_ACTIVE_TARG_ENABLED || NFC_CE_T4TA_ENABLED)
	NFC_A_init();
#endif
#if (NFC_P2P_PASSIVE_TARG_ENABLED || NFC_P2P_ACTIVE_TARG_ENABLED)
	NFC_F_Init();
#endif

#if NFC_CE_T4TB_ENABLED
	NFC_B_init();
#endif

#if (NFC_P2P_PASSIVE_TARG_ENABLED || NFC_P2P_ACTIVE_TARG_ENABLED)
	NFCDEP_init();
#endif

#if (NFC_CE_T4TA_ENABLED || NFC_CE_T4TB_ENABLED)
	ISODEP_init(false);
#endif

#if NFC_TARGET_DEBUG
	if(eTempNfcState != g_eCurrentTargetState)
	{
		Serial_printf("Target In :",NFC_TARGET_LAYER_DATA);
		NFC_Target_PrintMode(eTempNfcState);
		Serial_printf("Target Out :",NFC_TARGET_LAYER_DATA);
		NFC_Target_PrintMode(g_eCurrentTargetState);
	}
#endif
}

//*****************************************************************************
//
//! NFC_Target_GetPCDTimeOut - returns current PCD timeout.
//!
//! This function returns the current PCD timeout.
//!
//! \return g_ui16PCDTimeOut the current PCD timeout.
//
//*****************************************************************************
uint16_t NFC_Target_GetPCDTimeOut(void)
{
	return g_ui16PCDTimeOut;
}

//*****************************************************************************
//
//! NFC_Target_SetState - sets the current target state.
//!
//! \param eState the target state
//!
//! This function sets g_eCurrentTargetState with a new target state (eState),
//! used inside the NFC_Target_StateMachine() function.
//!
//! \return None.
//
//*****************************************************************************
void NFC_Target_SetState(tNfcTargetState eState)
{
	g_eCurrentTargetState = eState;
}

//*****************************************************************************
//
//! NFC_Target_StateMachine - Handles incoming commands from PCD, and sets
//! the PCD timeout based on the NFC technologies that get activated.
//! \param pui8Payload is the pointer to data received from the PCD.
//! \param ui8Length is the number of bytes received from the PCD.
//! \param sTargetMode is the Target mode enabled in the transceiver.
//!
//! This function handles the target statemachine for the application where the
//! transceiver is in target / listen mode. It handles the anti-collision /
//! activation of NFC-A, NFC-B and NFC-F. Once the technologies are activated
//! it calls the NFCDEP_processReceivedRequest() function for P2P mode,
//! and ISODEP_processReceivedRequest() function for T4T tags.
//!
//!
//! \return None.
//
//*****************************************************************************
tNfcTargetState NFC_Target_StateMachine(uint8_t * pui8Payload,
										uint8_t ui8Length,
										t_sTRF79x0_TargetMode sTargetMode)
{
#if (NFC_P2P_PASSIVE_TARG_ENABLED || NFC_P2P_ACTIVE_TARG_ENABLED || NFC_CE_T4TA_ENABLED || NFC_CE_T4TB_ENABLED)
	uint8_t ui8CommandLength;
	uint8_t ui8CommandId;

#if (NFC_P2P_PASSIVE_TARG_ENABLED || NFC_P2P_ACTIVE_TARG_ENABLED)
	uint8_t * pui8NfcFId2;
	uint8_t ui8NfcFIdLength;
	t_sTRF79x0_Bitrate sTRFBitrate;
#endif
	uint16_t ui16DepCommand;
#if (NFC_CE_T4TA_ENABLED || NFC_CE_T4TB_ENABLED)
	tIsoDepState sIsoDepState;
#endif

#if NFC_TARGET_DEBUG
	tNfcTargetState eTempNfcState = g_eCurrentTargetState;
#endif

	// Set Command and DepCommand to default values
	ui8CommandId = 0xFF;
	ui16DepCommand = 0xFFFF;
	
	// Command Length Validation
#if	(NFC_P2P_PASSIVE_TARG_ENABLED || NFC_CE_T4TA_ENABLED || NFC_CE_T4TB_ENABLED)
	if(sTargetMode.bits.bPassiveTypeA == 1
	   || sTargetMode.bits.bPassiveTypeB == 1)
	{
		ui8CommandLength = ui8Length;
		ui8CommandId = pui8Payload[0];
	}
	else if(sTargetMode.bits.bPassiveTypeF == 1
			&& (pui8Payload[0] == ui8Length))
	{
		ui8CommandLength = ui8Length;
		ui8CommandId = pui8Payload[1];
#if NFC_P2P_PASSIVE_TARG_ENABLED
		ui16DepCommand = NFCDEP_getDEPCommand(pui8Payload,ui8Length);
#endif
	}
#endif
#if (NFC_P2P_ACTIVE_TARG_ENABLED)
	if(sTargetMode.bits.bActiveTypeF == 1
			&& (pui8Payload[0] == ui8Length))
	{
		ui8CommandLength = ui8Length;
		ui16DepCommand = NFCDEP_getDEPCommand(pui8Payload,ui8Length);
	}
	else if(sTargetMode.bits.bActiveTypeA == 1
			&& ((pui8Payload[0]) == ui8Length))
	{
		ui8CommandLength = ui8Length;
		ui16DepCommand = NFCDEP_getDEPCommand(pui8Payload,ui8Length);
	}
#endif

	// Check for error condition
	if(	ui8CommandId == 0xFF && ui16DepCommand == 0xFFFF)
	{
		return g_eCurrentTargetState;
	}
	
	// End Command Length Validation

	switch(g_eCurrentTargetState)
	{
	case NFC_IDLE_STATE:
#if (NFC_P2P_PASSIVE_TARG_ENABLED || NFC_CE_T4TA_ENABLED)
		if(sTargetMode.bits.bPassiveTypeA == 1)
		{
			// Check for SENS_REQ or ALL_REQ
			if(ui8CommandId == SENS_REQ_CMD
			   || ui8CommandId == ALL_REQ_CMD)
			{
				NFC_A_send_SENS_RES();
				g_eCurrentTargetState = NFC_READY_A_CL1_STATE;
				// 5 mS
				g_ui16PCDTimeOut = 10;
			}
			else
			{
				// Added in accordance with TC_LIS_NFCA_UND_BI_18 (xy=12)
				TRF79x0_muteTimeMillisecond(MUTE_TIME_MS);
			}
		}
#endif
#if NFC_CE_T4TB_ENABLED
		if(sTargetMode.bits.bPassiveTypeB == 1)
		{
			if(ui8CommandId == SENSB_REQ_CMD)
			{
				if(NFC_B_getSENSBN(pui8Payload,ui8Length) == 0
					&& NFC_B_checkSENSBAfi(pui8Payload,ui8Length) == STATUS_SUCCESS)
				{
					g_eCurrentTargetState = NFC_READY_B_DECL_STATE;
					NFC_B_send_SENSB_RES();
					// 1000 mS
					g_ui16PCDTimeOut = 20;
				}
				else
				{
					g_eCurrentTargetState = NFC_READY_B_REQU_STATE;
				}
			}
		}
#endif
#if NFC_P2P_PASSIVE_TARG_ENABLED
		if(sTargetMode.bits.bPassiveTypeF == 1 && ui8CommandId == SENSF_REQ_CMD)
		{
			if(NFC_F_processReceivedRequest(pui8Payload,ui8Length) == STATUS_SUCCESS)
			{
				g_eCurrentTargetState = NFC_READY_F_STATE;
				// 1237 mS
				g_ui16PCDTimeOut = 1237;
				// Set NFCID3t
				NFC_F_getNFCID2(&pui8NfcFId2,&ui8NfcFIdLength);
				NFCDEP_setNFCID3t(pui8NfcFId2,8);
			}
		}
#endif
#if NFC_P2P_ACTIVE_TARG_ENABLED
		if(sTargetMode.bits.bActiveTypeF == 1
				&& (pui8Payload[0] == ui8Length))
		{
			// Only receive ATR_REQ_CMD
			if(ui16DepCommand == ATR_REQ_CMD)
			{
				if(NFCDEP_processReceivedRequest(pui8Payload,ui8Length) == STATUS_SUCCESS)
				{
					g_eCurrentTargetState = NFC_ATR_READY_F_STATE;

					g_ui16PCDTimeOut = NFCDEP_getLinkTimeOut();
				}
			}
		}
		else if(sTargetMode.bits.bActiveTypeA == 1
				&& ((pui8Payload[0]) == ui8Length))
		{
			// Only receive ATR_REQ_CMD
			if(ui16DepCommand == ATR_REQ_CMD)
			{
				if(NFCDEP_processReceivedRequest(pui8Payload,ui8Length) == STATUS_SUCCESS)
				{
					g_eCurrentTargetState = NFC_ATR_READY_A_STATE;

					g_ui16PCDTimeOut = NFCDEP_getLinkTimeOut();
				}
			}
		}
#endif
		break;
#if NFC_CE_T4TA_ENABLED
	case NFC_READY_CE_A_CL1_STATE:
#endif
#if (NFC_P2P_PASSIVE_TARG_ENABLED || NFC_CE_T4TA_ENABLED)
	case NFC_READY_A_CL1_STATE:
		if(ui8CommandId == SDD_REQ_CMD_CL1 && ui8Length == 0x02)
		{
			NFC_A_send_SDD_RES(0x01);
		}
		// SEL Request
		else if(ui8CommandId == SDD_REQ_CMD_CL1)
		{
			if(NFC_A_check_SEL_REQ(0x01,pui8Payload,ui8Length) == STATUS_SUCCESS)
			{
				NFC_A_send_SEL_RES(0x01);

				if(NFC_A_isTargetSelected(0x01) == STATUS_SUCCESS)
				{
					// 100 mS
					g_ui16PCDTimeOut = 100;
					g_eCurrentTargetState = NFC_ACTIVE_A_STATE;
				}
				else
				{
					g_eCurrentTargetState = NFC_READY_A_CL2_STATE;
				}
			}
			else
			{
				// Added in accordance with TC_LIS_NFCA_UND_BI_118_xy_14
				TRF79x0_muteTimeMillisecond(MUTE_TIME_MS);
				g_eCurrentTargetState = NFC_IDLE_STATE;
			}
		}
		else if(ui8CommandId == SLP_REQ_CMD)
		{
			g_ui16PCDTimeOut = 100;
			g_eCurrentTargetState = NFC_SLEEP_A_STATE;
			TRF79x0_setTargetSelect(false);
		}
		else
		{
			if(g_eCurrentTargetState == NFC_READY_CE_A_CL1_STATE)
			{
				g_eCurrentTargetState = NFC_SLEEP_A_STATE;
				TRF79x0_setTargetSelect(false);
			}
			else
			{
				// Added in accordance with TC_LIS_NFCA_UND_BI_18 (xy=12)
				TRF79x0_muteTimeMillisecond(MUTE_TIME_MS);
				g_eCurrentTargetState = NFC_IDLE_STATE;
			}
		}
		break;
#endif
#if NFC_CE_T4TA_ENABLED
	case NFC_READY_CE_A_CL2_STATE:
#endif
#if (NFC_P2P_PASSIVE_TARG_ENABLED || NFC_CE_T4TA_ENABLED)
	case NFC_READY_A_CL2_STATE:
		if(ui8CommandId == SDD_REQ_CMD_CL2 && ui8Length == 0x02)
		{
			NFC_A_send_SDD_RES(0x02);
		}
		// SEL Request
		else if(ui8CommandId == SDD_REQ_CMD_CL2)
		{
			if(NFC_A_check_SEL_REQ(0x02,pui8Payload,ui8Length) == STATUS_SUCCESS)
			{
				NFC_A_send_SEL_RES(0x02);

				if(NFC_A_isTargetSelected(0x02) == STATUS_SUCCESS)
				{
					// 100 mS
					g_ui16PCDTimeOut = 100;
					g_eCurrentTargetState = NFC_ACTIVE_A_STATE;
				}
				else
				{
					g_eCurrentTargetState = NFC_READY_A_CL3_STATE;
				}
			}
			else
			{
				// Added in accordance with TC_LIS_NFCA_UND_BI_118_xy_14
				TRF79x0_muteTimeMillisecond(MUTE_TIME_MS);
				g_eCurrentTargetState = NFC_IDLE_STATE;
			}
		}
		else if(ui8CommandId == SLP_REQ_CMD)
		{
			g_ui16PCDTimeOut = 100;
			g_eCurrentTargetState = NFC_SLEEP_A_STATE;
			TRF79x0_setTargetSelect(false);
		}
		else
		{
			if(g_eCurrentTargetState == NFC_READY_CE_A_CL2_STATE)
			{
				g_eCurrentTargetState = NFC_SLEEP_A_STATE;
				TRF79x0_setTargetSelect(false);
			}
			else
			{
				g_eCurrentTargetState = NFC_IDLE_STATE;
			}
		}
		break;
#endif
#if NFC_CE_T4TA_ENABLED
	case NFC_READY_CE_A_CL3_STATE:
#endif
#if (NFC_P2P_PASSIVE_TARG_ENABLED || NFC_CE_T4TA_ENABLED)
	case NFC_READY_A_CL3_STATE:
		if(ui8CommandId == SDD_REQ_CMD_CL3 && ui8Length == 0x02)
		{
			NFC_A_send_SDD_RES(0x03);
		}
		// SEL Request
		else if(ui8CommandId == SDD_REQ_CMD_CL3)
		{
			if(NFC_A_check_SEL_REQ(0x03,pui8Payload,ui8Length) == STATUS_SUCCESS)
			{
				NFC_A_send_SEL_RES(0x03);

				if(NFC_A_isTargetSelected(0x03) == STATUS_SUCCESS)
				{
					// 100 mS
					g_ui16PCDTimeOut = 100;
					g_eCurrentTargetState = NFC_ACTIVE_A_STATE;
				}
			}
			else
			{
				// Added in accordance with TC_LIS_NFCA_UND_BI_118_xy_14
				TRF79x0_muteTimeMillisecond(MUTE_TIME_MS);
				g_eCurrentTargetState = NFC_IDLE_STATE;
			}
		}
		else if(ui8CommandId == SLP_REQ_CMD)
		{
			g_ui16PCDTimeOut = 100;
			g_eCurrentTargetState = NFC_SLEEP_A_STATE;
			TRF79x0_setTargetSelect(false);
		}
		else
		{
			if(g_eCurrentTargetState == NFC_READY_CE_A_CL3_STATE)
			{
				g_eCurrentTargetState = NFC_SLEEP_A_STATE;
				TRF79x0_setTargetSelect(false);
			}
			else
				g_eCurrentTargetState = NFC_IDLE_STATE;
		}
		break;
#endif
#if (NFC_P2P_PASSIVE_TARG_ENABLED || NFC_P2P_ACTIVE_TARG_ENABLED || NFC_CE_T4TA_ENABLED)
	case NFC_ACTIVE_A_STATE:

#if (NFC_P2P_PASSIVE_TARG_ENABLED || NFC_P2P_ACTIVE_TARG_ENABLED)
		// Have to check if for NFC-DEP Support and check command received
		ui16DepCommand = NFCDEP_getDEPCommand(pui8Payload,ui8Length);

		// ATR_REQ CMD
		if(ui16DepCommand == ATR_REQ_CMD
			&& NFC_A_isNfcDepSupported() == STATUS_SUCCESS)
		{
			if(NFCDEP_processReceivedRequest(pui8Payload,ui8Length) == STATUS_SUCCESS)
				g_eCurrentTargetState = NFC_ATR_READY_A_STATE;
			else
			{
				g_eCurrentTargetState = NFC_IDLE_STATE;
			}
			g_ui16PCDTimeOut = NFCDEP_getLinkTimeOut();
			break;
		}
		// Otherwise go back to IDLE
		else
		{
			g_eCurrentTargetState = NFC_IDLE_STATE;
		}
#endif
#if NFC_CE_T4TA_ENABLED
		// Have to check if for ISO-DEP Support and check command received
		// RATS_CMD
		if(NFC_A_isIsoDepSupported() == STATUS_SUCCESS
			   && ui8Length == 0x02
			   && ui8CommandId == RATS_CMD)
		{
			// Check why ISODEP must be called twice
			ISODEP_init(false);
			ISODEP_setNFCType(ISO_DEP_A);
			ISODEP_processReceivedRequest(pui8Payload,ui8Length);
			g_ui16PCDTimeOut = ISODEP_getLinkTimeOut();
			g_eCurrentTargetState = NFC_CARD_EMULATOR_4A_STATE;
		}
		// SLP_REQ
		else if(ui8Length == 0x02
			   && ui8CommandId == SLP_REQ_CMD)
		{
			g_eCurrentTargetState = NFC_SLEEP_A_STATE;
			TRF79x0_setTargetSelect(false);
		}
		// Other go back to IDLE
		else
		{
			g_eCurrentTargetState = NFC_IDLE_STATE;
		}
#endif
		break;
#endif
#if (NFC_P2P_PASSIVE_TARG_ENABLED || NFC_CE_T4TA_ENABLED)
	case NFC_SLEEP_A_STATE:
		if(ui8Length == 0x01
			   && ui8CommandId == ALL_REQ_CMD)
		{
			g_eCurrentTargetState = NFC_READY_CE_A_CL1_STATE;
			NFC_A_send_SENS_RES();
		}
		break;

#endif
#if NFC_CE_T4TA_ENABLED
	case NFC_ACTIVE_CE_A_STATE:
		break;
	case NFC_CARD_EMULATOR_4A_STATE:
		sIsoDepState = ISODEP_processReceivedRequest(pui8Payload,ui8Length);
		switch(sIsoDepState)
		{
		case ISO_DEP_PARAM_SEL_STATE:
			g_eCurrentTargetState = NFC_SLEEP_A_STATE;
			TRF79x0_setTargetSelect(false);
			break;
		case ISO_DEP_PPS_SEL_STATE:
			break;
		case ISO_DEP_ACTIVATED_STATE:
			break;
		case ISO_DEP_DEACTIVATED_STATE:
			g_eCurrentTargetState = NFC_IDLE_STATE;
			break;
		case ISO_DEP_PROTOCOL_ERROR:
			g_eCurrentTargetState = NFC_IDLE_STATE;
			break;
		}

		break;
#endif
#if (NFC_P2P_PASSIVE_TARG_ENABLED || NFC_P2P_ACTIVE_TARG_ENABLED)
	case NFC_ATR_READY_A_STATE:
		// Have to check if for ISO-DEP / NFC-DEP Support and check command received

		ui16DepCommand = NFCDEP_getDEPCommand(pui8Payload,ui8Length);

		// RSL_REQ_CMD
		if(ui16DepCommand == RSL_REQ_CMD)
		{
			if(NFCDEP_processReceivedRequest(pui8Payload,ui8Length) == STATUS_SUCCESS)
			{
				g_eCurrentTargetState = NFC_IDLE_STATE;
			}
		}
		// PSL_REQ_CMD
		else if(ui16DepCommand == PSL_REQ_CMD)
		{
			if(NFCDEP_processReceivedRequest(pui8Payload,ui8Length) == STATUS_SUCCESS)
			{
				sTRFBitrate = NFCDEP_getBitrate();
				if(sTRFBitrate.bits.b106kpbs == 1)
				{
					g_eCurrentTargetState = NFC_TARGET_A_STATE;
				}
				else if(sTRFBitrate.bits.b212kpbs == 1
						|| sTRFBitrate.bits.b424kpbs == 1)
				{
					g_eCurrentTargetState = NFC_TARGET_F_STATE;
				}
				else
				{
					g_eCurrentTargetState = NFC_ATR_READY_A_STATE;
				}
			}
		}
		// DEP_REQ_CMD
		else if(ui16DepCommand == DEP_REQ_CMD)
		{
			if(NFCDEP_processReceivedRequest(pui8Payload,ui8Length) == STATUS_SUCCESS)
			{
				g_eCurrentTargetState = NFC_TARGET_A_STATE;
			}
			else
			{
				g_eCurrentTargetState = NFC_ATR_READY_A_STATE;
			}
		}
		// DSL_REQ_CMD
		else if(ui16DepCommand == DSL_REQ_CMD)
		{
			if(NFCDEP_processReceivedRequest(pui8Payload,ui8Length) == STATUS_SUCCESS)
			{
				TRF79x0_setTargetSelect(false);
				g_eCurrentTargetState = NFC_SLEEP_AF_STATE;
			}
			else
			{
				g_eCurrentTargetState = NFC_ATR_READY_A_STATE;
			}
		}
		else
		{
			g_eCurrentTargetState = NFC_ATR_READY_A_STATE;
		}

		g_ui16PCDTimeOut = NFCDEP_getLinkTimeOut();

		break;
	case NFC_TARGET_A_STATE:
		ui16DepCommand = NFCDEP_getDEPCommand(pui8Payload,ui8Length);

		// RSL_REQ_CMD
		if(ui16DepCommand == RSL_REQ_CMD)
		{
			if(NFCDEP_processReceivedRequest(pui8Payload,ui8Length) == STATUS_SUCCESS)
			{
				g_eCurrentTargetState = NFC_IDLE_STATE;
			}
		}
		// DSL_REQ_CMD
		else if(ui16DepCommand == DSL_REQ_CMD)
		{
			if(NFCDEP_processReceivedRequest(pui8Payload,ui8Length) == STATUS_SUCCESS)
			{
				TRF79x0_setTargetSelect(false);
				g_eCurrentTargetState = NFC_SLEEP_AF_STATE;
			}
			else
			{
				g_eCurrentTargetState = NFC_TARGET_A_STATE;
			}
		}
		// DEP_REQ_CMD
		else if(ui16DepCommand == DEP_REQ_CMD)
		{
			if(NFCDEP_processReceivedRequest(pui8Payload,ui8Length) == STATUS_SUCCESS)
			{
				g_eCurrentTargetState = NFC_TARGET_A_STATE;
			}
			else
			{
				g_eCurrentTargetState = NFC_IDLE_STATE;
			}
		}
		else
		{
			g_eCurrentTargetState = NFC_TARGET_A_STATE;
		}

		g_ui16PCDTimeOut = NFCDEP_getLinkTimeOut();

		break;
	case NFC_READY_F_STATE:
		// ATR_REQ CMD
		if(ui16DepCommand == ATR_REQ_CMD
			&& NFC_F_isNfcDepSupported() == STATUS_SUCCESS)
		{
			// Check NFCID3i compared to NFCID2
			NFC_F_getNFCID2(&pui8NfcFId2,&ui8NfcFIdLength);
			if(NFC_Target_CompareBuffers(&pui8Payload[3],pui8NfcFId2,8) == true)
			{
				if(NFCDEP_processReceivedRequest(pui8Payload,ui8Length) == STATUS_SUCCESS)
				{
					g_eCurrentTargetState = NFC_ATR_READY_F_STATE;
				}
			}
			g_ui16PCDTimeOut = NFCDEP_getLinkTimeOut();
		}
		else if(ui8CommandId == SENSF_REQ_CMD && ui8CommandLength == 0x06)
		{
			if(NFC_F_processReceivedRequest(pui8Payload,ui8Length) == STATUS_SUCCESS)
			{
				g_eCurrentTargetState = NFC_READY_F_STATE;
			}
		}
		else
		{
			g_eCurrentTargetState = NFC_READY_F_STATE;
		}
		break;
	case NFC_ATR_READY_F_STATE:
		// RSL_REQ_CMD
		if(ui16DepCommand == RSL_REQ_CMD)
		{
			if(NFCDEP_processReceivedRequest(pui8Payload,ui8Length) == STATUS_SUCCESS)
			{
				g_eCurrentTargetState = NFC_IDLE_STATE;
			}
		}
		// PSL_REQ_CMD
		else if(ui16DepCommand == PSL_REQ_CMD)
		{
			if(NFCDEP_processReceivedRequest(pui8Payload,ui8Length) == STATUS_SUCCESS)
			{
				sTRFBitrate = NFCDEP_getBitrate();
				if(sTRFBitrate.bits.b106kpbs == 1)
				{
					g_eCurrentTargetState = NFC_TARGET_A_STATE;
				}
				else if(sTRFBitrate.bits.b212kpbs == 1
						|| sTRFBitrate.bits.b424kpbs == 1)
				{
					g_eCurrentTargetState = NFC_TARGET_F_STATE;
				}
				else
				{
					g_eCurrentTargetState = NFC_ATR_READY_F_STATE;
				}
			}
		}
		// DEP_REQ_CMD
		else if(ui16DepCommand == DEP_REQ_CMD)
		{
			if(NFCDEP_processReceivedRequest(pui8Payload,ui8Length) == STATUS_SUCCESS)
			{
				g_eCurrentTargetState = NFC_TARGET_F_STATE;
			}
			else
			{
				g_eCurrentTargetState = NFC_ATR_READY_F_STATE;
			}
		}
		// DSL_REQ_CMD
		else if(ui16DepCommand == DSL_REQ_CMD)
		{
			if(NFCDEP_processReceivedRequest(pui8Payload,ui8Length) == STATUS_SUCCESS)
			{
				TRF79x0_setTargetSelect(false);
				g_eCurrentTargetState = NFC_SLEEP_AF_STATE;
			}
			else
			{
				g_eCurrentTargetState = NFC_ATR_READY_F_STATE;
			}
		}
		else
		{
			g_eCurrentTargetState = NFC_ATR_READY_F_STATE;
		}

		g_ui16PCDTimeOut = NFCDEP_getLinkTimeOut();

		break;
	case NFC_TARGET_F_STATE:
		// RSL_REQ_CMD
		if(ui16DepCommand == RSL_REQ_CMD)
		{
			if(NFCDEP_processReceivedRequest(pui8Payload,ui8Length) == STATUS_SUCCESS)
			{
				g_eCurrentTargetState = NFC_IDLE_STATE;
			}
		}
		// DSL_REQ_CMD
		else if(ui16DepCommand == DSL_REQ_CMD)
		{
			if(NFCDEP_processReceivedRequest(pui8Payload,ui8Length) == STATUS_SUCCESS)
			{
				TRF79x0_setTargetSelect(false);
				g_eCurrentTargetState = NFC_SLEEP_AF_STATE;
			}
			else 
			{
				g_eCurrentTargetState = NFC_TARGET_F_STATE;
			}
		}
		// DEP_REQ_CMD
		else if(ui16DepCommand == DEP_REQ_CMD)
		{
			if(NFCDEP_processReceivedRequest(pui8Payload,ui8Length) == STATUS_SUCCESS)
			{
				g_eCurrentTargetState = NFC_TARGET_F_STATE;
			}
			else
			{
				g_eCurrentTargetState = NFC_TARGET_F_STATE;
			}
		}
		g_ui16PCDTimeOut = NFCDEP_getLinkTimeOut();

		break;
#endif
#if (NFC_P2P_PASSIVE_TARG_ENABLED || NFC_P2P_ACTIVE_TARG_ENABLED || NFC_CE_T4TA_ENABLED)
	case NFC_SLEEP_AF_STATE:
#if (NFC_P2P_PASSIVE_TARG_ENABLED)
		if(ui8CommandId == SENSF_REQ_CMD && ui8CommandLength == 0x06)
		{		
			if(NFC_F_processReceivedRequest(pui8Payload,ui8Length) == STATUS_SUCCESS)
			{
				g_eCurrentTargetState = NFC_READY_F_STATE;
			}
			g_ui16PCDTimeOut = NFCDEP_getLinkTimeOut();
		}
#endif
#if (NFC_CE_T4TA_ENABLED)
		if(ui8CommandLength == 0x01
			   && ui8CommandId == ALL_REQ_CMD)
		{
			g_eCurrentTargetState = NFC_READY_A_CL1_STATE;
			NFC_A_send_SENS_RES();
			g_ui16PCDTimeOut = ISODEP_getLinkTimeOut();
		}
#endif
		break;
#endif
#if NFC_CE_T4TB_ENABLED
	case NFC_READY_B_REQU_STATE:
		if(ui8CommandId == SENSB_REQ_CMD && ui8CommandLength == 0x03)
		{
			if(NFC_B_getSENSBN(pui8Payload,ui8Length) == 0
				&& NFC_B_checkSENSBAfi(pui8Payload,ui8Length) == STATUS_SUCCESS)
			{
				g_eCurrentTargetState = NFC_READY_B_DECL_STATE;
				NFC_B_send_SENSB_RES();
			}
			else if(NFC_B_checkSENSBAfi(pui8Payload,ui8Length) == STATUS_FAIL)
			{
				g_eCurrentTargetState = NFC_IDLE_STATE;
			}
			else
			{
				g_eCurrentTargetState = NFC_READY_B_REQU_STATE;
			}
		}
		else if(ui8CommandId == SLOT_MARKER)
		{
			g_eCurrentTargetState = NFC_READY_B_DECL_STATE;
			NFC_B_send_SENSB_RES();
		}

		break;
	case NFC_READY_B_DECL_STATE:
		if(ui8CommandId == SENSB_REQ_CMD && ui8CommandLength == 0x03)
		{
			if(NFC_B_getSENSBN(pui8Payload,ui8Length) == 0
				&& NFC_B_checkSENSBAfi(pui8Payload,ui8Length) == STATUS_SUCCESS)
			{
				g_eCurrentTargetState = NFC_READY_B_DECL_STATE;
				NFC_B_send_SENSB_RES();
			}
			else if(NFC_B_getSENSBN(pui8Payload,ui8Length) > 0)
			{
				g_eCurrentTargetState = NFC_READY_B_REQU_STATE;
			}
			else if(NFC_B_checkSENSBAfi(pui8Payload,ui8Length) == STATUS_FAIL)
			{
				g_eCurrentTargetState = NFC_IDLE_STATE;
			}
		}
		else if(ui8CommandId == SLPB_REQ_CMD)
		{
			// Need to check for the correct NFCID
			if(NFC_B_checkSLPBId(pui8Payload,ui8Length) == STATUS_SUCCESS)
			{
				g_eCurrentTargetState = NFC_SLEEP_B_STATE;
				NFC_B_send_SLPB_RES();
			}
			// If not correct, then go into Mute_Time.
			else
			{
				TRF79x0_muteTimeMillisecond(MUTE_TIME_MS);
			}

		}
		else if(ui8CommandId == ATTRIB_CMD)
		{
			if(NFC_B_checkAttribId(pui8Payload,ui8Length) == STATUS_SUCCESS &&
					NFC_B_checkAttribParam(pui8Payload,ui8Length) == STATUS_SUCCESS)
			{
				// Check why ISODEP must be called twice
				ISODEP_init(false);
				ISODEP_setNFCType(ISO_DEP_B);
				ISODEP_processReceivedRequest(pui8Payload,ui8Length);
				g_ui16PCDTimeOut = ISODEP_getLinkTimeOut();
				g_eCurrentTargetState = NFC_CARD_EMULATOR_4B_STATE;
			}
			// If NFCID is not correct, then go into Mute_Time.
			else {
				TRF79x0_muteTimeMillisecond(MUTE_TIME_MS);
			}
		}
		// Added to handle BI_08_07
		else
		{
			TRF79x0_muteTimeMillisecond(MUTE_TIME_MS);
		}
		break;
	case NFC_SLEEP_B_STATE:
		// Included mask in order to allow Extended SENSB_RES support bit to be excluded?
		if((ui8CommandId == SENSB_REQ_CMD)
		 && ((pui8Payload[2] & 0xEF) == ALLB_REQ_PARAM_MASK) )
		{
			if(NFC_B_getSENSBN(pui8Payload,ui8Length) == 0
				&& NFC_B_checkSENSBAfi(pui8Payload,ui8Length) == STATUS_SUCCESS)
			{
				g_eCurrentTargetState = NFC_READY_B_DECL_STATE;
				NFC_B_send_SENSB_RES();
			}
			else if(NFC_B_getSENSBN(pui8Payload,ui8Length) > 0)
			{
				g_eCurrentTargetState = NFC_READY_B_REQU_STATE;
			}
			else if(NFC_B_checkSENSBAfi(pui8Payload,ui8Length) == STATUS_FAIL)
			{
				g_eCurrentTargetState = NFC_IDLE_STATE;
			}
		}
		else
		{
			TRF79x0_resetMode();
		}
		break;
	case NFC_CARD_EMULATOR_4B_STATE:
		sIsoDepState = ISODEP_processReceivedRequest(pui8Payload,ui8Length);
		switch(sIsoDepState)
		{
		case ISO_DEP_PARAM_SEL_STATE:
			g_eCurrentTargetState = NFC_SLEEP_B_STATE;
			break;
		case ISO_DEP_PPS_SEL_STATE:
			break;
		case ISO_DEP_ACTIVATED_STATE:
			break;
		case ISO_DEP_DEACTIVATED_STATE:
			g_eCurrentTargetState = NFC_IDLE_STATE;
			break;
		case ISO_DEP_PROTOCOL_ERROR:
			g_eCurrentTargetState = NFC_IDLE_STATE;
			break;
		}
		break;
#endif
	}

#if NFC_TARGET_DEBUG
	if(eTempNfcState != g_eCurrentTargetState)
	{
		Serial_printf("Target In :");
		NFC_Target_PrintMode(eTempNfcState);
		Serial_printf("Target Out :");
		NFC_Target_PrintMode(g_eCurrentTargetState);
	}
#endif

#endif
	return g_eCurrentTargetState;
}

//*****************************************************************************
//
//! NFC_Target_getState - returns the current target state.
//!
//! This function returns  g_eCurrentTargetState.
//!
//! \return g_eCurrentTargetState the current target state.
//
//*****************************************************************************
tNfcTargetState NFC_Target_getState(void)
{
	return g_eCurrentTargetState;
}

//*****************************************************************************
//
//! NFC_Target_CompareBuffers - compares if two buffers are the same.
//!
//! \param puiBuffer1 is the pointer for source buffer 1.
//! \param puiBuffer2 is the pointer for source buffer 2.
//! \param ui8Length is the number of bytes to compare between buffer 1 and 2.
//!
//! This function checks if two buffers have the same values for ui8Length values.
//!
//! \return bRet is true if both buffers have the same value, false otherwise.
//
//*****************************************************************************
bool NFC_Target_CompareBuffers(uint8_t * puiBuffer1, uint8_t * puiBuffer2, uint8_t ui8Length)
{
	uint8_t ui8Temp;
	bool bRet = true;
	for(ui8Temp = 0; ui8Temp < ui8Length; ui8Temp++)
	{
		if(puiBuffer1[ui8Temp] != puiBuffer2[ui8Temp])
		{
			bRet = false;
			break;
		}
	}
	return bRet;
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
