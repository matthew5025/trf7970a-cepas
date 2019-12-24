//*****************************************************************************
//
// nfc_config.h - NFC Command Definitions
//
// Copyright (c) 2015 Texas Instruments Incorporated.  All rights reserved.
// TI Information - Selective Disclosure
//
//*****************************************************************************

#ifndef __NFC_CONFIG_H__
#define __NFC_CONFIG_H__

//*****************************************************************************
//
// Defines to compile different NFC modes.
//
// Set to 0x01 to enable the NFC operating mode.
// Set to 0x00 to disable the NFC operating mode.
//
// This allows for customized configurations of the NFC stack to reduce the
// memory footprint by not compiling any unused NFC operating modes.
//
//*****************************************************************************

#define NFC_P2P_ACTIVE_INIT_ENABLED			0x01
#define NFC_P2P_ACTIVE_TARG_ENABLED			0x01
#define NFC_P2P_PASSIVE_INIT_ENABLED		0x01
#define NFC_P2P_PASSIVE_TARG_ENABLED		0x01

#define	NFC_CE_T4TA_ENABLED					0x01
#define	NFC_CE_T4TB_ENABLED					0x01

#define	NFC_RW_T2T_ENABLED					0x01
#define	NFC_RW_T3T_ENABLED					0x01
#define	NFC_RW_T4TA_ENABLED					0x01
#define	NFC_RW_T4TB_ENABLED					0x01
#define	NFC_RW_T5T_ENABLED					0x01

#define NFC_PEER_2_PEER_INITIATOR_ENABLED	(NFC_P2P_ACTIVE_INIT_ENABLED || NFC_P2P_PASSIVE_INIT_ENABLED)
#define NFC_PEER_2_PEER_TARGET_ENABLED		(NFC_P2P_ACTIVE_TARG_ENABLED || NFC_P2P_PASSIVE_TARG_ENABLED)
#define NFC_CARD_EMULATION_ENABLED			(NFC_CE_T4TA_ENABLED || NFC_CE_T4TB_ENABLED)
#define NFC_READER_WRITER_ENABLED			(NFC_RW_T2T_ENABLED || NFC_RW_T3T_ENABLED || NFC_RW_T4TA_ENABLED || NFC_RW_T4TB_ENABLED || NFC_RW_T5T_ENABLED)

//*****************************************************************************
//
// Defines for Debug Portions of Code
//
//*****************************************************************************

//
// NFC Layers - Set to 0x01 to enable debug statements, otherwise set to 0x00 to disable.
//
#define	NFC_A_DEBUG				0x00
#define	NFC_B_DEBUG				0x00
#define	NFC_F_DEBUG				0x00
#define NFC_INITIATOR_DEBUG		0x00
#define NFC_TARGET_DEBUG		0x00
#define	NFC_DEP_DEBUG			0x00
#define	NFC_LLCP_DEBUG			0x00
#define	NFC_SNEP_DEBUG			0x00
#define NFC_P2P_DEBUG			0x00
#define NFC_CE_DEBUG			0x00
#define TRF7970A_DEBUG			0x00

#define P2P_TARGET_FW_ENABLED  		0x01
#define P2P_INITIATOR_FW_ENABLED 	0x02
#define CARD_EMULATION_FW_ENABLED 	0x04
#define READER_WRITER_FW_ENABLED 	0x08

// Commands NFC Host <-- NFC Controller
typedef enum
{
	// General NFC commands
	IDLE_CMD = 0x00,
	FW_VERSION_CMD 		= 0x01,
	NFC_CONFIGURATION 	= 0x02,
	NFC_MODE_LOST 		= 0x03,

	// P2P Commands
	P2P_TECH_DATA 		= 0x20,
	P2P_BITR_DATA 		= 0x21,
	P2P_RSSI_DATA 		= 0x22,
	P2P_MODE_DATA 		= 0x23,
	P2P_RX_SIZE_DATA 	= 0x24,
	P2P_NDEF_TOTAL_SIZE = 0x25,
	P2P_PAYLOAD_DATA 	= 0x26,

	// CE Commands
	CE_TECH_DATA 		= 0x40,
	CE_BITR_DATA 		= 0x41,
	CE_RSSI_DATA 		= 0x42,
	CE_TYPE_DATA 		= 0x43,
	CE_UID_DATA 		= 0x44,
	CE_FILE_STATUS 		= 0x45,

	// RW Commands
	RW_TECH_DATA 		= 0x60,
	RW_BITR_DATA 		= 0x61,
	RW_RSSI_DATA 		= 0x62,
	RW_TYPE_DATA 		= 0x63,
	RW_UID_DATA 		= 0x64,
	RW_PAYLOAD_DATA 	= 0x65,
	RW_STATUS_DATA		= 0x66
}tNFCHostCommands;

#endif
