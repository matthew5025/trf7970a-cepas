//*****************************************************************************
//
// config.h - MCU Pin Configuration to interface with the TRF7970A
//
// Copyright (c) 2015 Texas Instruments Incorporated.  All rights reserved.
// TI Information - Selective Disclosure
//
//*****************************************************************************
#ifndef __CONFIG_H__
#define __CONFIG_H__

#ifdef		MSP432P401R_LAUNCHPAD_ENABLED

//
// TRF79790A Hardware Configuration
//
#define SPI_MODULE_BASE_ADDR 	EUSCI_B0_MODULE
#define SPI_PORT 				GPIO_PORT_P1
#define SPI_CLK 				GPIO_PIN5
#define SPI_MOSI 				GPIO_PIN6
#define SPI_MISO 				GPIO_PIN7
#define SPI_SS_PORT				GPIO_PORT_P6
#define SPI_SS_POUT				P6OUT
#define SPI_SS 					GPIO_PIN5
#define TRF_EN_PORT				GPIO_PORT_P6
#define TRF_EN 					GPIO_PIN4

#define TRF_IRQ_PORT 			GPIO_PORT_P3
#define TRF_IRQ_PIN				P3IN
#define TRF_IRQ 				GPIO_PIN0

//
// P2.0 - Displays if the external RF field is enabled.
//
#define	NFC_RF_FIELD_LED_BIT			BIT0
#define	NFC_RF_FIELD_LED_POUT			P2OUT
#define	NFC_RF_FIELD_LED_DIR			P2DIR

//
// P1.0 - Displays if the NFC host is connected to the controller.
//
#define	NFC_HOST_LED_BIT				BIT0
#define	NFC_HOST_LED_POUT				P1OUT
#define	NFC_HOST_LED_DIR				P1DIR

//
// P - Toggles during the RX of a NDEF packet via P2P.
//
#define	NFC_RX_LED_BIT
#define	NFC_RX_LED_POUT
#define	NFC_RX_LED_PDIR

//
// P - Toggles during the TX of a NDEF packet via P2P.
//
#define	NFC_TX_LED_BIT
#define	NFC_TX_LED_POUT
#define	NFC_TX_LED_PDIR

//
// LED1 P3.6 - Displays if the RW link is established.
//
#define	NFC_RW_LED_BIT					BIT6
#define	NFC_RW_LED_POUT					P3OUT
#define	NFC_RW_LED_PDIR					P3DIR

//
// LED2 P5.2 - Displays if the CE link is established.
//
#define	NFC_P2P_LED_BIT					BIT2
#define	NFC_P2P_LED_POUT				P5OUT
#define	NFC_P2P_LED_PDIR				P5DIR

//
// LED3 P5.0 - Displays if the P2P link is established.
//
#define	NFC_CE_LED_BIT					BIT0
#define	NFC_CE_LED_POUT					P5OUT
#define	NFC_CE_LED_PDIR					P5DIR

#endif

#endif
