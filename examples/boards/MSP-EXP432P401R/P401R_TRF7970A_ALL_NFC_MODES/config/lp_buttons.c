//*****************************************************************************
//
// lp_buttons.c - Push button initialization and interrupts for F5529 LaunchPad
//
// Copyright (c) 2014 Texas Instruments Incorporated.  All rights reserved.
// TI Information - Selective Disclosure
//
//*****************************************************************************

#include "mcu.h"
#include "lp_buttons.h"

volatile uint8_t buttonDebounce = 1;
volatile uint16_t buttonsPressed = 0;

// Forward declared functions
void Buttons_startWDT(void);
void WDT_ISR(void);
void Port1_ISR(void);

/***************************************************************************//**
 * @brief  Initialize ports for buttons as active low inputs
 * @param  buttonsMask   Use values defined in lp_buttons.h for the buttons to
 *                       initialize
 * @return none
 ******************************************************************************/

void Buttons_init(uint16_t buttonsMask)
{
    // Configure button ports
	BUTTON_PORT_DIR &= ~BUTTON_ALL;  // Buttons on P1.1/P1.4 are inputs

    BUTTON_PORT_OUT |= buttonsMask;  //buttons are active low
    BUTTON_PORT_REN |= buttonsMask;  //pullup resistor
    BUTTON_PORT_SEL &= ~buttonsMask;

    Interrupt_registerInterrupt(INT_WDT_A, WDT_ISR);
    GPIO_registerInterrupt(GPIO_PORT_P1, Port1_ISR);
}

/***************************************************************************//**
 * @brief  Enable button interrupts for selected buttons
 * @param  buttonsMask   Use values defined in lp_buttons.h for the buttons to
 *                       enable
 * @return none
 ******************************************************************************/

void Buttons_interruptEnable(uint16_t buttonsMask)
{
    BUTTON_PORT_IES &= ~buttonsMask; //select falling edge trigger
    BUTTON_PORT_IFG &= ~buttonsMask; //clear flags
    BUTTON_PORT_IE |= buttonsMask;   //enable interrupts
}

/***************************************************************************//**
 * @brief  Disable button interrupts for selected buttons
 * @param  buttonsMask   Use values defined in lp_buttons.h for the buttons to
 *                       disable
 * @return none
 ******************************************************************************/

void Buttons_interruptDisable(uint16_t buttonsMask)
{
    BUTTON_PORT_IE &= ~buttonsMask;
}

/***************************************************************************//**
 * @brief  Sets up the WDT as a button debouncer, only activated once a
 *         button interrupt has occurred.
 * @param  none
 * @return none
 ******************************************************************************/

void Buttons_startWDT()
{
    // WDT as 250ms interval counter
//    SFRIFG1 &= ~WDTIFG;
    WDTCTL = WDTPW + WDTSSEL_1 + WDTTMSEL + WDTCNTCL + WDTIS_5;
    Interrupt_enableInterrupt(INT_WDT_A);
}

/***************************************************************************//**
 * @brief  Handles Watchdog Timer interrupts.
 *
 *         Global variables are used to determine the module triggering the
 *         interrupt, and therefore, how to handle it.
 * @param  none
 * @return none
 ******************************************************************************/

void WDT_ISR(void)
{
    if (buttonDebounce == 2)
    {
        buttonDebounce = 1;

//        SFRIFG1 &= ~WDTIFG;
        Interrupt_disableInterrupt(INT_WDT_A);
        WDTCTL = WDTPW + WDTHOLD;
    }
}

/***************************************************************************//**
 * @brief  Handles Port 1 interrupts - performs button debouncing and registers
 *         button presses.
 * @param  none
 * @return none
 ******************************************************************************/

void Port1_ISR(void)
{
    uint32_t status;

    status = GPIO_getEnabledInterruptStatus(GPIO_PORT_P1);
    GPIO_clearInterruptFlag(GPIO_PORT_P1, status);

	if (status & BIT1)
	{
		if (buttonDebounce == 1)
		{
			//
			// Context save interrupt flag before calling interrupt vector.
			// Reading interrupt vector generator will automatically clear IFG flag
			//
			buttonsPressed = BUTTON_S1;
			buttonDebounce = 2;
			Buttons_startWDT();
		}
		else if (buttonDebounce == 0)
		{
			// Do Nothing
		}
	}

	if (status & BIT4)
	{
		if (buttonDebounce == 1)
		{
			//
			// Context save interrupt flag before calling interrupt vector.
			// Reading interrupt vector generator will automatically clear IFG flag
			//
			buttonsPressed = BUTTON_S2;
			buttonDebounce = 2;
			Buttons_startWDT();
		}
		else if (buttonDebounce == 0)
		{
			// Do Nothing
		}

	}
}

/***************************************************************************//**
 * @}
 ******************************************************************************/
