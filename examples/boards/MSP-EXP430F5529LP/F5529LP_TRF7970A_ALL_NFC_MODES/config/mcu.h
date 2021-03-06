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
#include "msp430.h"
#include "gpio.h"
#include <stdint.h>
#include <string.h>
#include "types.h"
#include "nfc_config.h"
#include "config.h"

// USB includes
#include "USB_config/descriptors.h"
#include "USB_API/USB_Common/device.h"
#include "USB_API/USB_Common/types.h"               //Basic Type declarations
#include "USB_API/USB_Common/usb.h"                 //USB-specific functions

#include "USB_API/USB_CDC_API/UsbCdc.h"
#include "usbConstructs.h"


// ******** Definitions ******************************************************//

#define MHZ									1000000
#define MCLK_FREQ							(25*MHZ)
#define MCLK_MS_COUNT						(MCLK_FREQ/1000)
#define UCS_MCLK_DESIRED_FREQUENCY_IN_KHZ   12000
#define UCS_MCLK_FLLREF_RATIO   366

#define PMM_STATUS_OK     0
#define PMM_STATUS_ERROR  1
#define st(x)      do { x } while (__LINE__ == -1)

#define XT1_XT2_PORT_DIR            P5DIR
#define XT1_XT2_PORT_OUT            P5OUT
#define XT1_XT2_PORT_SEL            P5SEL
#define XT1_ENABLE                  (BIT4 + BIT5)
#define XT2_ENABLE                  (BIT2 + BIT3)

/*******************************************************************************
 * Macros
 ******************************************************************************/

/* Select source for FLLREF  e.g. SELECT_FLLREF(SELREF__XT1CLK) */
#define SELECT_FLLREF(source) st(UCSCTL3 = (UCSCTL3 & ~(SELREF_7)) | (source);)
/* Select source for ACLK    e.g. SELECT_ACLK(SELA__XT1CLK) */
#define SELECT_ACLK(source)   st(UCSCTL4 = (UCSCTL4 & ~(SELA_7))   | (source);)
/* Select source for MCLK    e.g. SELECT_MCLK(SELM__XT2CLK) */
#define SELECT_MCLK(source)   st(UCSCTL4 = (UCSCTL4 & ~(SELM_7))   | (source);)
/* Select source for SMCLK   e.g. SELECT_SMCLK(SELS__XT2CLK) */
#define SELECT_SMCLK(source)  st(UCSCTL4 = (UCSCTL4 & ~(SELS_7))   | (source);)
/* Select source for MCLK and SMCLK e.g. SELECT_MCLK_SMCLK(SELM__DCOCLK + SELS__DCOCLK) */
#define SELECT_MCLK_SMCLK(sources) st(UCSCTL4 = (UCSCTL4 & ~(SELM_7 + SELS_7)) | (sources);)

/* set ACLK/x */
#define ACLK_DIV(x)         st(UCSCTL5 = (UCSCTL5 & ~(DIVA_7)) | (DIVA__##x);)
/* set MCLK/x */
#define MCLK_DIV(x)         st(UCSCTL5 = (UCSCTL5 & ~(DIVM_7)) | (DIVM__##x);)
/* set SMCLK/x */
#define SMCLK_DIV(x)        st(UCSCTL5 = (UCSCTL5 & ~(DIVS_7)) | (DIVS__##x);)
/* Select divider for FLLREF  e.g. SELECT_FLLREFDIV(2) */
#define SELECT_FLLREFDIV(x) st(UCSCTL3 = (UCSCTL3 & ~(FLLREFDIV_7))|(FLLREFDIV__##x);)


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
#define SLAVE_SELECT_HIGH       SPI_SS_POUT |= SPI_SS; //GPIO_setOutputHighOnPin(SPI_SS_PORT, SPI_SS);
#define SLAVE_SELECT_LOW        SPI_SS_POUT &= ~(SPI_SS); //GPIO_setOutputLowOnPin(SPI_SS_PORT, SPI_SS);

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

#endif
