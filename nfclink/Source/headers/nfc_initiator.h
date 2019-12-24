//*****************************************************************************
//
// nfc_initiator.h - Implementation of polling mode.
//
// Copyright (c) 2015 Texas Instruments Incorporated.  All rights reserved.
// TI Information - Selective Disclosure
//
//*****************************************************************************
#ifndef __NFC_INITIATOR_H__
#define __NFC_INITIATOR_H__

#include "nfc_a.h"
#include "nfc_b.h"
#include "nfc_f.h"

#include "iso_dep.h"
#include "nfc_dep.h"

#include "nfc_rw_t2t.h"
#include "nfc_rw_t3t.h"
#include "nfc_rw_t5t.h"

#define FOUND_TECHNOLOGY_NONE	0x00
#define FOUND_TECHNOLOGY_A		0x01
#define FOUND_TECHNOLOGY_B		0x02
#define FOUND_TECHNOLOGY_F		0x04
#define FOUND_TECHNOLOGY_V		0x08

#define CON_BITR_NONE			0x00
#define CON_BITR_212_KBPS		0x02
#define CON_BITR_424_KBPS		0x03

typedef enum
{
	NFC_INITIATOR_TECH_DETECTION = 0x00,
	NFC_INITIATOR_TECH_RESOLUTION_A_CL1,
	NFC_INITIATOR_TECH_RESOLUTION_A_CL1_SEL,
	NFC_INITIATOR_TECH_RESOLUTION_A_CL2,
	NFC_INITIATOR_TECH_RESOLUTION_A_CL2_SEL,
	NFC_INITIATOR_TECH_RESOLUTION_A_CL3,
	NFC_INITIATOR_TECH_RESOLUTION_A_CL3_SEL,
	NFC_INITIATOR_TECH_RESOLUTION_B,
	NFC_INITIATOR_TECH_RESOLUTION_F,
	NFC_INITIATOR_DEVICE_ACTIVATION_A,
	NFC_INITIATOR_DEVICE_ACTIVATION_A_TAG,
	NFC_INITIATOR_DEVICE_ACTIVATION_B,
	NFC_INITIATOR_DEVICE_ACTIVATION_F,
	NFC_INITIATOR_NFC_DEP,
	NFC_INITIATOR_ISO_DEP,
	NFC_INITIATOR_PROTOCOL_ERROR,
	NFC_INITIATOR_TYPE_2_DEP,
	NFC_INITIATOR_TYPE_3_DEP,
	NFC_INITIATOR_TYPE_V_DEP,
	NFC_INITIATOR_TYPE_MFC_DEP,
	NFC_INITIATOR_DEVICE_DEACTIVATION_NFC_DEP
//	NFC_INITIATOR_DEVICE_DEACTIVATION_ISO_DEP,
//	NFC_INITIATOR_DEVICE_DEACTIVATION_TYPE_1_DEP,
//	NFC_INITIATOR_DEVICE_DEACTIVATION_TYPE_2_DEP,
//	NFC_INITIATOR_DEVICE_DEACTIVATION_TYPE_3_DEP
}tNfcInitiatorState;

//*****************************************************************************
//
// Function Prototypes
//
//*****************************************************************************
extern void NFC_Initiator_Init(void);

extern uint16_t NFC_Initiator_GetPICCTimeOut(void);

extern void NFC_Initiator_SetState(tNfcInitiatorState eState);

extern tNfcInitiatorState NFC_Initiator_StateMachine(t_sTRF79x0_InitiatorMode sInitiatorMode);

extern tNfcInitiatorState NFC_Initiator_ProccessResponse(uint8_t * pui8Payload,
														uint8_t ui8Length,
														t_sTRF79x0_InitiatorMode sInitiatorMode);

extern tNfcInitiatorState NFC_Initiator_GetState(void);

#endif
