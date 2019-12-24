//*****************************************************************************
//
// trf79x0.h - TRF7970A APIs and structure definitons
//
// Copyright (c) 2015 Texas Instruments Incorporated.  All rights reserved.
// TI Information - Selective Disclosure
//
//*****************************************************************************
#ifndef __TRF79X0_H__
#define __TRF79X0_H__

#include "nfc_spi.h"
#include "mcu.h"
#include "nfc_config.h"

#ifdef __cplusplus
extern "C"
{
#endif

//*****************************************************************************
//
// TRF79X0 Register Definitions.
//
//*****************************************************************************
#define TRF79X0_CHIP_STATUS_CTRL_REG        0x00
#define TRF79X0_ISO_CONTROL_REG             0x01
#define TRF79X0_ISO14443B_OPTIONS_REG       0x02
#define TRF79X0_ISO14443A_OPTIONS_REG       0x03
#define TRF79X0_TX_TIMER_EPC_HIGH           0x04
#define TRF79X0_TX_TIMER_EPC_LOW            0x05
#define TRF79X0_TX_PULSE_LENGTH_CTRL_REG    0x06
#define TRF79X0_RX_NO_RESPONSE_WAIT_REG     0x07
#define TRF79X0_RX_WAIT_TIME_REG            0x08
#define TRF79X0_MODULATOR_CONTROL_REG       0x09
#define TRF79X0_RX_SPECIAL_SETTINGS_REG     0x0A
#define TRF79X0_REGULATOR_CONTROL_REG       0x0B
#define TRF79X0_IRQ_STATUS_REG              0x0C
#define TRF79X0_IRQ_MASK_REG                0x0D
#define TRF79X0_COLLISION_POSITION_REG      0x0E
#define TRF79X0_RSSI_LEVEL_REG              0x0F
#define TRF79X0_SPECIAL_FUNC_1_REG	        0x10
#define TRF79X0_SPECIAL_FUNC_2_REG	        0x11
#define TRF79X0_FIFO_IRQ_LEVEL				0x14
#define TRF79X0_NFC_LOW_DETECTION_LEVEL		0x16
#define TRF79X0_NFC_ID_REG                  0x17
#define TRF79X0_NFC_TARGET_LEVEL_REG        0x18
#define TRF79X0_NFC_TARGET_PROTOCOL_REG     0x19
#define TRF79X0_TEST_SETTING1_REG           0x1A
#define TRF79X0_TEST_SETTING2_REG           0x1B
#define TRF79X0_FIFO_STATUS_REG             0x1C
#define TRF79X0_TX_LENGTH_BYTE1_REG         0x1D
#define TRF79X0_TX_LENGTH_BYTE2_REG         0x1E
#define TRF79X0_FIFO_REG                    0x1F

//*****************************************************************************
//
// TRF79X0 TRF79X0_CHIP_STATUS_CTRL_REG Register Definitions.
//
//*****************************************************************************
#define TRF79X0_STATUS_CTRL_STANDBY_MODE	0x80
#define TRF79X0_STATUS_CTRL_ACTIVE_MODE		0x00
#define TRF79X0_STATUS_CTRL_DIRECT          0x40
#define TRF79X0_STATUS_CTRL_RF_ON           0x20
#define TRF79X0_STATUS_CTRL_RF_PWR_HALF     0x10
#define TRF79X0_STATUS_CTRL_RF_PWR_FULL     0x00
#define TRF79X0_STATUS_CTRL_AGC_ON			0x04
#define TRF79X0_STATUS_CTRL_RECEIVER_ON		0x02
#define TRF79X0_STATUS_CTRL_5V_SYSTEM		0x01
#define TRF79X0_STATUS_CTRL_3V_SYSTEM		0x00

//*****************************************************************************
//
// TRF79X0 TRF79X0_ISO_CONTROL Register Definitions.
//
//*****************************************************************************
#define TRF79X0_ISO_CONTROL_RX_CRC_N        0x80
#define TRF79X0_ISO_CONTROL_DIR_MODE        0x40
#define TRF79X0_ISO_NFC_TARGET				0x00
#define TRF79X0_ISO_NFC_INITIATOR			0x10
#define TRF79X0_NFC_PASSIVE_MODE			0x00
#define TRF79X0_NFC_ACTIVE_MODE				0x08
#define TRF79X0_NFC_NORMAL_MODE				0x00
#define TRF79X0_NFC_CARD_EMULATION_MODE		0x40
#define TRF79X0_ISO_CONTROL_14443A_106K     0x08

//*****************************************************************************
//
// TRF79X0 TRF79X0_MODULATOR_CONTROL_REG Register Definitions.
//
//*****************************************************************************
#define TRF79X0_MOD_CTRL_SYS_CLK_13_56MHZ   0x30
#define TRF79X0_MOD_CTRL_SYS_CLK_6_78MHZ    0x20
#define TRF79X0_MOD_CTRL_SYS_CLK_3_3MHZ     0x10
#define TRF79X0_MOD_CTRL_SYS_CLK_DISABLE    0x00
#define TRF79X0_MOD_CTRL_MOD_ASK_30         0x07
#define TRF79X0_MOD_CTRL_MOD_ASK_22         0x06
#define TRF79X0_MOD_CTRL_MOD_ASK_16         0x05
#define TRF79X0_MOD_CTRL_MOD_ASK_13         0x04
#define TRF79X0_MOD_CTRL_MOD_ASK_8_5        0x03
#define TRF79X0_MOD_CTRL_MOD_ASK_7          0x02
#define TRF79X0_MOD_CTRL_MOD_OOK_100        0x01
#define TRF79X0_MOD_CTRL_MOD_ASK_10         0x00

//*****************************************************************************
//
// TRF79X0 TRF79X0_REGULATOR_CONTROL_REG Register Definitions.
//
//*****************************************************************************
#define TRF79X0_REGULATOR_CTRL_AUTO_REG     0x80
#define TRF79X0_REGULATOR_CTRL_VRS_2_7V     0x00
#define TRF79X0_REGULATOR_CTRL_VRS_2_8V     0x01
#define TRF79X0_REGULATOR_CTRL_VRS_2_9V     0x02
#define TRF79X0_REGULATOR_CTRL_VRS_3_0V     0x03
#define TRF79X0_REGULATOR_CTRL_VRS_3_1V     0x04
#define TRF79X0_REGULATOR_CTRL_VRS_3_2V     0x05
#define TRF79X0_REGULATOR_CTRL_VRS_3_3V     0x06
#define TRF79X0_REGULATOR_CTRL_VRS_3_4V     0x07

//*****************************************************************************
//
// TRF79x0 Command Definitions.
//
//*****************************************************************************
#define TRF79X0_IDLE_CMD                    			0x00
#define TRF79X0_SOFT_INIT_CMD               			0x03
#define TRF79X0_INITIAL_RF_COLLISION_AVOID_CMD			0x04
#define TRF79X0_PERFORM_RES_RF_COLLISION_AVOID_CMD		0x05
#define TRF79X0_PERFORM_RES_RF_COLLISION_AVOID_N0_CMD	0x06
#define TRF79X0_RESET_FIFO_CMD              			0x0F
#define TRF79X0_TRANSMIT_NO_CRC_CMD         			0x10
#define TRF79X0_TRANSMIT_CRC_CMD            			0x11
#define TRF79X0_DELAY_TRANSMIT_NO_CRC_CMD   			0x12
#define TRF79X0_DELAY_TRANSMIT_CRC_CMD      			0x13
#define TRF79X0_TRANSMIT_NEXT_SLOT_CMD      			0x14
#define TRF79X0_CLOSE_SLOT_SEQUENCE_CMD     			0x15
#define TRF79X0_STOP_DECODERS_CMD           			0x16
#define TRF79X0_RUN_DECODERS_CMD            			0x17
#define TRF79X0_TEST_INTERNAL_RF_CMD        			0x18
#define TRF79X0_TEST_EXTERNAL_RF_CMD        			0x19
#define TRF79X0_RX_ADJUST_GAIN_CMD          			0x1A

//*****************************************************************************
//
// TRF79x0 Command/Address mode definitions.
//
//*****************************************************************************
#define TRF79X0_ADDRESS_MASK                			0x1F
#define TRF79X0_CONTROL_CMD                 			0x80
#define TRF79X0_CONTROL_REG_READ            			0x40
#define TRF79X0_CONTROL_REG_WRITE           			0x00
#define TRF79X0_REG_MODE_SINGLE             			0x00
#define TRF79X0_REG_MODE_CONTINUOUS         			0x20

//*****************************************************************************
//
// TRF79X0 TRF79X0_RX_SPECIAL_SETTINGS_REG Register Definitions.
//
//*****************************************************************************
#define TRF79X0_RX_SP_SET_M848              0x20

//*****************************************************************************
//
// TRF79X0 TRF79X0_FIFO_STATUS_REG Register Definitions.
//
//*****************************************************************************
#define TRF79X0_FIFO_OVERFLOW							0x80

//*****************************************************************************
//
// TRF79X0 TRF79X0_NFC_TARGET_PROTOCOL_REG Register Definitions.
//
//*****************************************************************************
#define TRF79X0_NFC_TARGET_PROTOCOL_RF_WAKE_UP			0x80
#define TRF79X0_NFC_TARGET_PROTOCOL_RF_COLLISION_LEVEL  0x40
#define TRF79X0_NFC_TARGET_PROTOCOL_FELICA				0x10
#define TRF79X0_NFC_TARGET_PROTOCOL_ISO14443A			0x00
#define TRF79X0_NFC_TARGET_PROTOCOL_ISO14443B			0x40
#define TRF79X0_NFC_TARGET_PROTOCOL_KBPS_MASK			0x03
#define TRF79X0_NFC_TARGET_PROTOCOL_106KBPS				0x01
#define TRF79X0_NFC_TARGET_PROTOCOL_212KBPS				0x02
#define TRF79X0_NFC_TARGET_PROTOCOL_424KBPS				0x03

//
// Used for the NFC-Validation
//
#define MUTE_TIME_MS									90

//
// TRF797x0 Modes Settings
//
typedef enum
{
   IDLE_MODE = 0,
   INITIATOR_MODE,
   TARGET_MODE
} tTRF79x0_Mode;

typedef union
{
    struct {
    	uint8_t bPassive15693  	: 1;
    	uint8_t bPassiveTypeA  	: 1;
    	uint8_t bPassiveTypeB  	: 1;
    	uint8_t bPassiveTypeF  	: 1;
    	uint8_t bActiveTypeA  	: 1;
    	uint8_t bActiveTypeF  	: 1;
        /*! Reserved bits         */
    	uint8_t reserved   		: 2;
    }bits;
    /*! Whole status byte access */
    uint8_t     ui8byte;
}t_sTRF79x0_InitiatorMode;

typedef union
{
    struct {
    	uint8_t bPassiveTypeA  	: 1;
    	uint8_t bPassiveTypeB  	: 1;
    	uint8_t bPassiveTypeF  	: 1;
    	uint8_t bActiveTypeA  	: 1;
    	uint8_t bActiveTypeF  	: 1;

        /*! Reserved bits         */
    	uint8_t reserved   : 5;
    }bits;
    /*! Whole status byte access */
    uint8_t     ui8byte;
}t_sTRF79x0_TargetMode;

typedef union
{
    struct {
    	// Target and Initiator Bitrates
    	// ISO1443A / ISO1443B
    	uint16_t b106kpbs  				: 1;
    	// ISO1443A / ISO1443B / FeliCa
        uint16_t b212kpbs 				: 1;
        // ISO1443A / ISO1443B / FeliCa
        uint16_t b424kpbs 				: 1;
        // Initiator Bitrates Only
        // ISO1443A / ISO1443B
        uint16_t b848kpbs 				: 1;
        // 15693 Bit-Rates
        uint16_t b6_62_kbps_1_out_4 	: 1;
        uint16_t b6_62_kbps_1_out_256 	: 1;
        uint16_t b26_48_kbps_1_out_4 	: 1;
        uint16_t b26_48_kbps_1_out_256 	: 1;
        uint16_t b6_67_kbps_1_out_4 	: 1;
        uint16_t b6_67_kbps_1_out_256 	: 1;
        uint16_t b26_69_kbps_1_out_4 	: 1;
        uint16_t b26_69_kbps_1_out_256 	: 1;
        /*! Reserved bits         */
        uint16_t reserved     			: 4;
    }bits;
    /*! Whole status word access */
    uint16_t     ui16word;
}t_sTRF79x0_Bitrate;

//
// CRC Settings for TRF79x0_
//
typedef enum
{
	CRC_BIT_DISABLE = 0,						// TX CRC Disabled
	CRC_BIT_ENABLE								// TX CRC Enabled
} tTRF79x0_CRC;

//
// External Field Settings for TRF79x0_
//
typedef enum
{
	EXT_FIELD_OFF = 0,						// External Field Off
	EXT_FIELD_ON							// External Field On
} tExtField;

//
// RSSI Settings for testing internal/external RF strengths.
//
typedef enum
{
	INTERNAL_RSSI = 0,						// Internal RSSI
	EXTERNAL_RSSI							// External RSSI
} tRssiStatus;

//
// IRQ Flag definitions. 
//
typedef enum
{
    IRQ_STATUS_IDLE = 0x00,
    IRQ_STATUS_COLLISION_ERROR = 0x01,
    IRQ_STATUS_COLLISION_AVOID_FINISHED = 0x02,
    IRQ_STATUS_RF_FIELD_CHANGE = 0x04,
    IRQ_STATUS_SDD_COMPLETE = 0x08,
    IRQ_STATUS_PROTOCOL_ERROR = 0x10,
    IRQ_STATUS_FIFO_HIGH_OR_LOW  = 0x20,
    IRQ_STATUS_RX_COMPLETE = 0x40,
    IRQ_STATUS_TX_COMPLETE = 0x80,
    IRQ_STATUS_OVERFLOW = 0xFC,
    IRQ_STATUS_NO_RESPONSE = 0xFE,
    IRQ_STATUS_TIME_OUT = 0xFF
} tTRF79x0_IRQFlag;

//
// IRQ NFC Flag definitions.
//
typedef enum {
	IRQ_STATUS_NFC_COLLISION_ERROR = 0x01,
	IRQ_STATUS_NFC_COLLISION_AVOID_FINISHED = 0x02,
	IRQ_STATUS_NFC_RF_FIELD_CHANGE = 0x04,
	IRQ_STATUS_NFC_SDD_COMPLETE = 0x08,
	IRQ_STATUS_NFC_PROTOCOL_ERROR = 0x10
}tTRF79x0_NFCFlag;

//
// IRQ RFID Flag definitions.
//
typedef enum {
	IRQ_STATUS_RFID_NO_RESPONSE_INT = 0x01,
	IRQ_STATUS_RFID_COLLISION_ERROR = 0x02,
	IRQ_STATUS_RFID_FRAMING_OR_EOF_ERROR = 0x04,
	IRQ_STATUS_RFID_PARITY_ERROR = 0x08,
	IRQ_STATUS_RFID_CRC_ERROR = 0x10
}tTRF79x0_RFIDFlag;

typedef enum {
	TRF7970_A = 0x01
}tTRF79x0_Version;

//
// TRF7970A Status Register
//
typedef struct
{
	uint8_t ui8FifoBytesReceived;		// Bytes Received
	bool bTxComplete;					// Transmission Complete
	bool bTargetSelected;				// Target Selected
	bool bRfFieldOn;					// RF Field Status
	bool bProtocolError;				// Protocol Error
	bool bNfcModeEnabled;				// Use to differentiate between Reader/NFC Modes
	tTRF79x0_Mode eCurrentMode;			// Target / Initiator / Idle
	// Target Bitrate
	t_sTRF79x0_Bitrate sTargetPayloadFrequency;
	// Passive / Active Types A / B (not active) / F
	t_sTRF79x0_TargetMode sTargetMode;
	// Initiator Bitrate
	t_sTRF79x0_Bitrate sInitiatorPayloadFrequency;
	// Passive / Active Types A / B (not active) / F / 15693 (not active)
	t_sTRF79x0_InitiatorMode sInitiatorMode;
	// IRQ Status
	tTRF79x0_IRQFlag eIrqFlags;
	// Internal RSSI
	uint8_t ui8InternalRssi;
}tTRF79x0_Status;

extern volatile uint8_t g_ui8IrqFlag;
extern volatile uint8_t g_bBitclockFlag;
/*
 * Initialize the TRF79x0_.
 */
extern uint8_t TRF79x0_init(void);

/*
 * Enable TRF79x0_ Interrupt.
 */
extern void TRF79x0_interruptEnable(void);

/*
 * Disable TRF79x0_ Interrupt.
 */
extern void TRF79x0_interruptDisable(void);

/*
 * Raw Write to the TRF79x0_.
 */
extern void TRF79x0_rawWrite(
	uint8_t * pui8Payload,
	uint8_t ui8Length
	);

/*
 * Direct Command Write.
 */
extern void TRF79x0_directCommand(uint8_t ui8Command);

/*
 * Single Write to the ui8Register register.
 */
extern void TRF79x0_writeSingle(
	uint8_t ui8Value,
	uint8_t ui8Register
	);

/*
 * Continuous Write of ui8Length bytes starting at register ui8Register.
 */
extern void TRF79x0_writeCont(
	uint8_t * pui8Payload,
	uint8_t ui8Register,
	uint8_t ui8Length
	);

/*
 * Single Read from the ui8Register register.
 */
extern void TRF79x0_readSingle(
	uint8_t * pui8Value,
	uint8_t ui8Register
	);

/*
 * Continuous Read of ui8Length bytes starting at register ui8Register.
 */
extern void TRF79x0_readCont(
	uint8_t * pui8Payload,
	uint8_t ui8Register,
	uint8_t ui8Length
	);

/*
 * Writes payload to the FIFO register. 
 */
extern uint8_t TRF79x0_writeFIFO(
	uint8_t *pui8Payload,
    tTRF79x0_CRC eCRCBit,
	uint8_t ui8Length,
	uint8_t ui8BrokenBits
	);

/*
 * IRQ Handler.
 */
extern tTRF79x0_IRQFlag TRF79x0_irqHandler(uint16_t ui16TimeOut);
 
/*
 * Switch Target BaudRate.
 */
extern uint8_t TRF79x0_switchTargetBaudRate(
	t_sTRF79x0_Bitrate sTargetFrequency,
	bool bModifyRegister
	);

extern uint8_t TRF79x0_incrementTargetBaudRate(
	t_sTRF79x0_Bitrate sTargetFrequency
	);

/*
 * Switch Initiator BaudRate.
 */
extern uint8_t TRF79x0_switchInitiatorBaudRate(
	t_sTRF79x0_InitiatorMode sInitiatorMode,
	t_sTRF79x0_Bitrate sInitiatorFrequency,
	bool bModifyRegister
	);

extern uint8_t TRF79x0_incrementInitiatorBaudRate(
	t_sTRF79x0_Bitrate sInitiatorFrequency
	);

/*
 * Get Collision Position if there was a collision.
 */
extern uint16_t TRF79x0_getCollisionStatus(void);

/*
 * Enable/Disable RF Field.
 */
extern uint8_t TRF79x0_setField(tExtField eRfField);
 
/*
 * Checks the internal/external field RSSI.
 */
extern uint8_t TRF79x0_checkField(tRssiStatus eField);
 
/*
 * Initialize Initiator/Target/Idle Mode.
 */
extern uint8_t TRF79x0_idleMode(void);

/*
 * Initialize TRF7970A in correct mode
 */
extern uint8_t TRF79x0_targetInitialize(void);

/*
 * Setup Target Mode and supported frequencies.
 */
extern uint8_t TRF79x0_targetModeSetup(
	t_sTRF79x0_TargetMode sSupportedModes,
	t_sTRF79x0_Bitrate sTargetSupportedFrequency);

/*
 * Setup Initiator Mode and Initiator frequency.
 */
extern uint8_t TRF79x0_initiatorModeSetup(
	t_sTRF79x0_InitiatorMode sInitiatorMode,
	t_sTRF79x0_Bitrate sFrequency);

	
/*
 * Reset TRF79x0_ in current mode. (Needed for Target Mode to start polling for
 * supported modes and frequencies.)
 */
extern uint8_t TRF79x0_resetMode(void);

/*
 * Reset FIFO.
 */
extern void TRF79x0_resetFifo(void);


/*
 * Reset FIFO.
 */
extern uint8_t * TRF79x0_getNfcBuffer(void);

/*
 * Get TRF7970A Status
 */
extern tTRF79x0_Status TRF79x0_getStatus(void);

extern void TRF79x0_setAutoSdd(uint8_t * pui8TargetId,
							   uint8_t ui8IdLength,
							   bool bNfcDep,
							   bool bIsoDep);

extern void TRF79x0_initAutoSdd(void);

extern void TRF79x0_disableAutoSdd(void);

extern void TRF79x0_setTargetSelect(bool bTargetStatus);

extern void TRF79x0_setNfcMode(bool bNfcMode);

extern void TRF79x0_setExtFieldLed(bool bExtField);

extern void TRF79x0_muteTimeMillisecond(uint32_t delay);

extern void TRF79x0_muteTimeMicrosecond(uint32_t delay);

extern void TRF79x0_ignoreCmd(void);

extern void TRF79x0_testFlag(bool bSupportCertification);

extern void TRF79x0_setVersion(tTRF79x0_Version eTRFVersion);

extern void TRF79x0_setType5OptionDelay(bool bT5TOptionDelay);

extern void TRF79x0_setExtAmplifer(bool bExtAmplifer);

extern void TRF79x0_setPowerSupply(bool bPowerSupply5V);

#endif
