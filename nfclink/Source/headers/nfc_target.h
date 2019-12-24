//*****************************************************************************
//
// nfc_target.h - Implementation of target/listen mode.
//
// Copyright (c) 2015 Texas Instruments Incorporated.  All rights reserved.
// TI Information - Selective Disclosure
//
//*****************************************************************************
#ifndef __NFC_TARGET_H__
#define __NFC_TARGET_H__

#include "nfc_a.h"
#include "nfc_b.h"
#include "nfc_f.h"

#include "nfc_dep.h"
#include "iso_dep.h"

#include "nfc_config.h"

typedef enum
{
	NFC_IDLE_STATE = 0x00,
	NFC_READY_A_CL1_STATE,
	NFC_READY_A_CL2_STATE,
	NFC_READY_A_CL3_STATE,
	NFC_ACTIVE_A_STATE,

	NFC_SLEEP_A_STATE,
	NFC_READY_CE_A_CL1_STATE,
	NFC_READY_CE_A_CL2_STATE,
	NFC_READY_CE_A_CL3_STATE,
	NFC_ACTIVE_CE_A_STATE,
	NFC_CARD_EMULATOR_4A_STATE,

	NFC_ATR_READY_A_STATE,
	NFC_TARGET_A_STATE,

	NFC_READY_F_STATE,
	NFC_ATR_READY_F_STATE,
	NFC_CARD_EMULATOR_3_STATE,
	NFC_TARGET_F_STATE,
	NFC_SLEEP_AF_STATE,

	NFC_READY_B_REQU_STATE,
	NFC_READY_B_DECL_STATE,
	NFC_SLEEP_B_STATE,
	NFC_CARD_EMULATOR_4B_STATE
}tNfcTargetState;

//*****************************************************************************
//
// Function Prototypes
//
//*****************************************************************************
extern void NFC_Target_Init();

extern uint16_t NFC_Target_GetPCDTimeOut(void);

extern void NFC_Target_SetState(tNfcTargetState eState);

extern tNfcTargetState NFC_Target_StateMachine(uint8_t * pui8Payload,
											   uint8_t ui8Length,
											   t_sTRF79x0_TargetMode sTargetMode);

extern tNfcTargetState NFC_Target_getState(void);

extern bool NFC_Target_CompareBuffers(uint8_t * puiBuffer1,
									  uint8_t * puiBuffer2,
									  uint8_t ui8Length);

#endif
