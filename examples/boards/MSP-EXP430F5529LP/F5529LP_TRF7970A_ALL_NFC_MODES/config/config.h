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

// MSP430F5529 Experimenters Board

#ifdef		MSP430F5529_EXP_BOARD_ENABLED

#define		MSP430F5529_EXP_LCD_ENABLED

//
// TRF79790A Hardware Configuration
//
#define SPI_MODULE_BASE_ADDR 	USCI_B0_BASE
#define SPI_PORT 				GPIO_PORT_P3
#define SPI_CLK 				GPIO_PIN2
#define SPI_MOSI 				GPIO_PIN0
#define SPI_MISO 				GPIO_PIN1
#define SPI_SS_PORT				GPIO_PORT_P2
#define SPI_SS_POUT				P2OUT
#define SPI_SS 					GPIO_PIN6
#define TRF_EN_PORT				GPIO_PORT_P2
#define TRF_EN 					GPIO_PIN3
#define TRF_EN2_PORT			GPIO_PORT_P2
#define TRF_EN2					GPIO_PIN4

#define TRF_IRQ_PORT 			GPIO_PORT_P2
#define TRF_IRQ_PIN				P2IN
#define TRF_IRQ 				GPIO_PIN0

//
// P1.1 - Displays if the external RF field is enabled.
//
#define	NFC_RF_FIELD_LED_BIT			BIT1
#define	NFC_RF_FIELD_LED_POUT			P1OUT
#define	NFC_RF_FIELD_LED_DIR			P1DIR

//
// P1.3 - Displays if the NFC host is connected to the controller.
//
#define	NFC_HOST_LED_BIT				BIT3
#define	NFC_HOST_LED_POUT				P1OUT
#define	NFC_HOST_LED_DIR				P1DIR

//
// P1.4 - Toggles during the RX of a NDEF packet via P2P.
//
#define	NFC_RX_LED_BIT					BIT4
#define	NFC_RX_LED_POUT					P1OUT
#define	NFC_RX_LED_PDIR					P1DIR

//
// P1.5 - Toggles during the TX of a NDEF packet via P2P.
//
#define	NFC_TX_LED_BIT					BIT5
#define	NFC_TX_LED_POUT					P1OUT
#define	NFC_TX_LED_PDIR					P1DIR

//
// LED1 P1.0 - Displays if the RW link is established.
//
#define	NFC_RW_LED_BIT					BIT0
#define	NFC_RW_LED_POUT					P1OUT
#define	NFC_RW_LED_PDIR					P1DIR

//
// LED2 P8.1 - Displays if the CE link is established.
//
#define	NFC_P2P_LED_BIT					BIT1
#define	NFC_P2P_LED_POUT				P8OUT
#define	NFC_P2P_LED_PDIR				P8DIR

//
// LED3 P8.2 - Displays if the P2P link is established.
//
#define	NFC_CE_LED_BIT					BIT2
#define	NFC_CE_LED_POUT					P8OUT
#define	NFC_CE_LED_PDIR					P8DIR

#endif


#ifdef		MSP430F5529_LAUNCHPAD_ENABLED

// MSP430F5529 LaunchPad

//
// TRF79790A Hardware Configuration
//
#define SPI_MODULE_BASE_ADDR 	USCI_B0_BASE
#define SPI_PORT 				GPIO_PORT_P3
#define SPI_CLK 				GPIO_PIN2
#define SPI_MOSI 				GPIO_PIN0
#define SPI_MISO 				GPIO_PIN1
#define SPI_SS_PORT				GPIO_PORT_P4
#define SPI_SS_POUT				P4OUT
#define SPI_SS 					GPIO_PIN2
#define TRF_EN_PORT				GPIO_PORT_P4
#define TRF_EN 					GPIO_PIN1

#define TRF_IRQ_PORT 			GPIO_PORT_P2
#define TRF_IRQ_PIN				P2IN
#define TRF_IRQ 				GPIO_PIN2

//
// P4.7 - Displays if the external RF field is enabled.
//
#define	NFC_RF_FIELD_LED_BIT			BIT7
#define	NFC_RF_FIELD_LED_POUT			P4OUT
#define	NFC_RF_FIELD_LED_DIR			P4DIR

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
// LED1 P1.0 - Displays if the RW link is established.
//
#define	NFC_RW_LED_BIT					BIT1
#define	NFC_RW_LED_POUT					P8OUT
#define	NFC_RW_LED_PDIR					P8DIR

//
// LED2 P8.3 - Displays if the CE link is established.
//
#define	NFC_P2P_LED_BIT					BIT3
#define	NFC_P2P_LED_POUT				P2OUT
#define	NFC_P2P_LED_PDIR				P2DIR

//
// LED3 P8.1 - Displays if the P2P link is established.
//
#define	NFC_CE_LED_BIT					BIT6
#define	NFC_CE_LED_POUT					P2OUT
#define	NFC_CE_LED_PDIR					P2DIR

//
// P - Displays if there has been a partial packet received from the
// NFC host.
//
#define	NFC_HOST_CMD_ERROR_LED_BIT
#define	NFC_HOST_CMD_ERROR_LED_POUT
#define	NFC_HOST_CMD_ERROR_LED_PDIR

#endif

#endif
