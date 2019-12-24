//*****************************************************************************
//
// mcu.h - MCU Configuration and host interface APIs definitions
//
// Copyright (c) 2015 Texas Instruments Incorporated.  All rights reserved.
// TI Information - Selective Disclosure
//
//*****************************************************************************
#ifndef __MCU_H__
#define __MCU_H__

// Include Header Files
#include "driverlib.h"
#include <stdint.h>
#include <string.h>
#include "types.h"
#include "nfc_config.h"
#include "config.h"


// ******** Definitions ******************************************************//

#define MHZ									1000000
#define MCLK_FREQ							(24*MHZ)
#define SMCLK_FREQ							(12*MHZ)
#define MCLK_MS_COUNT						(MCLK_FREQ/1000)
#define UCS_MCLK_DESIRED_FREQUENCY_IN_KHZ   12000
#define UCS_MCLK_FLLREF_RATIO   366

 //=====MCU constants=============================================
#define MOD_DIR_OUT	P2DIR |= BIT1
#define MOD_OFF		P2OUT &= ~BIT1
#define MOD_ON		P2OUT |= BIT1

#define ENABLE_PORT_SET      	GPIO_setAsOutputPin(TRF_EN_PORT, TRF_EN);
#define	TRF_ENABLE				GPIO_setOutputHighOnPin(TRF_EN_PORT, TRF_EN);		// EN pin on the TRF796x
#define TRF_DISABLE				GPIO_setOutputLowOnPin(TRF_EN_PORT, TRF_EN);

#define ENABLE2_PORT_SET      	GPIO_setAsOutputPin(TRF_EN2_PORT, TRF_EN2);
#define	TRF_ENABLE2				GPIO_setOutputHighOnPin(TRF_EN2_PORT, TRF_EN2);		// EN pin on the TRF796x
#define TRF_DISABLE2			GPIO_setOutputLowOnPin(TRF_EN2_PORT, TRF_EN2);

#define IRQ_PIN_SET     		GPIO_setAsInputPin(TRF_IRQ_PORT, TRF_IRQ);

#define IRQ_INT_ON				GPIO_enableInterrupt(TRF_IRQ_PORT, TRF_IRQ);

#define IRQ_INT_OFF				GPIO_disableInterrupt(TRF_IRQ_PORT, TRF_IRQ);
#define IRQ_EDGE_SET    		GPIO_interruptEdgeSelect(TRF_IRQ_PORT, TRF_IRQ, GPIO_LOW_TO_HIGH_TRANSITION);
#define IRQ_CLR					GPIO_clearInterruptFlag(TRF_IRQ_PORT, TRF_IRQ);

#define SLAVE_SELECT_PORT_SET	GPIO_setAsOutputPin(SPI_SS_PORT, SPI_SS);
#define SLAVE_SELECT_HIGH       SPI_SS_POUT |= SPI_SS;
#define SLAVE_SELECT_LOW        SPI_SS_POUT &= ~(SPI_SS);

#define IRQ_IS_SET()           	(TRF_IRQ_PIN & TRF_IRQ) // GPIO_getInputPinValue(TRF_IRQ_PORT, TRF_IRQ)

extern bool g_bSerialConnectionEstablished;
extern volatile uint8_t g_ui8BytesReceived;

//
// Function Prototypes
//
void MCU_init();
void MCU_delayMillisecond(uint32_t n_ms);
void MCU_delayMicrosecond(uint32_t n_us);
void MCU_clockInit();
void MCU_portInit();
void MCU_timerInit(uint16_t timeout_ms, uint8_t * timeout_flag);
void MCU_timerDisable();


void Serial_init(void);
void Serial_printf(char * pui8String,tNFCHostCommands eCmdType);
void Serial_printBuffer(char * pui8String, uint8_t ui8Length, tNFCHostCommands eCmdType);
void Serial_printfLine(char * pui8String);

extern
uint8_t convertNibbleToAscii(uint8_t ui8Nibble);

extern
void convertByteToAscii(uint8_t ui8byte, uint8_t pui8Buffer[3]);

extern
void convertWordToAscii(uint16_t ui16Word, uint8_t pui8Buffer[5]);

extern
void EusciA0_ISR(void);

#endif
