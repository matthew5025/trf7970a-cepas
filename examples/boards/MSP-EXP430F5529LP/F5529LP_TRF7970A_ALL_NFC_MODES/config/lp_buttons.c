//*****************************************************************************
//
// lp_buttons.c - Push button initialization and interrupts for F5529 LaunchPad
//
// Copyright (c) 2014 Texas Instruments Incorporated.  All rights reserved.
// TI Information - Selective Disclosure
//
//*****************************************************************************


#include "msp430.h"
#include "lp_buttons.h"
#include "trf79x0.h"

#define BUTTON_PORT_DIR   PADIR
#define BUTTON_PORT_OUT   PAOUT
#define BUTTON_PORT_SEL   PASEL
#define BUTTON_PORT_REN   PAREN

#define BUTTON_PORT_IE    PAIE
#define BUTTON_PORT_IES   PAIES
#define BUTTON_PORT_IFG   PAIFG
#define BUTTON_PORT_IN    PAIN

#define BUTTON1_PIN       BIT1       //P1.1
#define BUTTON2_PIN       BIT1       //P2.1

#define BUTTON1_IFG       P1IFG      //P1.7
#define BUTTON2_IFG       P2IFG      //P1.7

volatile uint8_t buttonDebounce = 1;
volatile uint16_t buttonsPressed = 0;

// Forward declared functions
void Buttons_startWDT(void);

/***************************************************************************//**
 * @brief  Initialize ports for buttons as active low inputs
 * @param  buttonsMask   Use values defined in HAL_buttons.h for the buttons to
 *                       initialize
 * @return none
 ******************************************************************************/

void Buttons_init(uint16_t buttonsMask)
{
    // Configure button ports
    PADIR &= ~0x0202;               // Buttons on P1.1/P2.1 are inputs

    BUTTON_PORT_OUT |= buttonsMask;  //buttons are active low
    BUTTON_PORT_REN |= buttonsMask;  //pullup resistor
    BUTTON_PORT_SEL &= ~buttonsMask;
}

/***************************************************************************//**
 * @brief  Enable button interrupts for selected buttons
 * @param  buttonsMask   Use values defined in HAL_buttons.h for the buttons to
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
 * @param  buttonsMask   Use values defined in HAL_buttons.h for the buttons to
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
    SFRIFG1 &= ~WDTIFG;
    WDTCTL = WDTPW + WDTSSEL_1 + WDTTMSEL + WDTCNTCL + WDTIS_5;
    SFRIE1 |= WDTIE;
}

/***************************************************************************//**
 * @brief  Handles Watchdog Timer interrupts.
 *
 *         Global variables are used to determine the module triggering the
 *         interrupt, and therefore, how to handle it.
 * @param  none
 * @return none
 ******************************************************************************/

#pragma vector=WDT_VECTOR
__interrupt void WDT_ISR(void)
{
    if (buttonDebounce == 2)
    {
        buttonDebounce = 1;

        SFRIFG1 &= ~WDTIFG;
        SFRIE1 &= ~WDTIE;
        WDTCTL = WDTPW + WDTHOLD;
    }
}

/***************************************************************************//**
 * @brief  Handles Port 2 interrupts - performs button debouncing and registers
 *         button presses.
 * @param  none
 * @return none
 ******************************************************************************/

#pragma vector=PORT2_VECTOR
__interrupt void Port2_ISR(void)
{

    switch (__even_in_range(P2IV, P2IV_P2IFG7))
    {
        // Vector  P2IV_NONE:  No Interrupt pending
        case  P2IV_NONE:
            break;

        // Vector  P2IV_P2IFG0:  P2IV P2IFG.0
        case  P2IV_P2IFG0:
            break;

        // Vector  P2IV_P2IFG1:  P2IV P2IFG.1
        case  P2IV_P2IFG1:

            if (buttonDebounce == 1)
            {
                //
                // Context save interrupt flag before calling interrupt vector.
                // Reading interrupt vector generator will automatically clear IFG flag
                //
                buttonsPressed = BUTTON_S1;
                buttonDebounce = 2;
                Buttons_startWDT();
                __bic_SR_register_on_exit(LPM3_bits);
            }
            else if (buttonDebounce == 0)
            {
                __bic_SR_register_on_exit(LPM4_bits);
            }

        	break;

        // Vector  P2IV_P2IFG2:  P2IV P2IFG.2
        case  P2IV_P2IFG2:

        	g_ui8IrqFlag = 0x01;
    		__bic_SR_register_on_exit(LPM0_bits);

            break;

        // Vector  P2IV_P2IFG3:  P2IV P2IFG.3
        case  P2IV_P2IFG3:
            break;

        // Vector  P2IV_P2IFG4:  P2IV P2IFG.4
        case  P2IV_P2IFG4:
            break;

        // Vector  P2IV_P2IFG5:  P2IV P2IFG.5
        case  P2IV_P2IFG5:
            break;

        // Vector  P2IV_P2IFG1:  P2IV P2IFG.6
        case  P2IV_P2IFG6:
            break;

        // Vector  P2IV_P2IFG7:  P2IV P2IFG.7
        case  P2IV_P2IFG7:
            break;

        // Default case
        default:
            break;
    }
}

/***************************************************************************//**
 * @brief  Handles Port 1 interrupts - performs button debouncing and registers
 *         button presses.
 * @param  none
 * @return none
 ******************************************************************************/

#pragma vector=PORT1_VECTOR
__interrupt void Port1_ISR(void)
{

    switch (__even_in_range(P1IV, P1IV_P1IFG7))
    {
        // Vector  P1IV_NONE:  No Interrupt pending
        case  P1IV_NONE:
            break;

        // Vector  P1IV_P1IFG0:  P1IV P1IFG.0
        case  P1IV_P1IFG0:
            break;

        // Vector  P1IV_P1IFG1:  P1IV P1IFG.1
        case  P1IV_P1IFG1:
            if (buttonDebounce == 1)
            {
                //
                // Context save interrupt flag before calling interrupt vector.
                // Reading interrupt vector generator will automatically clear IFG flag
                //
            	buttonsPressed = BUTTON_S2;
                buttonDebounce = 2;
                Buttons_startWDT();
                __bic_SR_register_on_exit(LPM3_bits);
            }
            else if (buttonDebounce == 0)
            {
                __bic_SR_register_on_exit(LPM4_bits);
            }

            break;

        // Vector  P1IV_P1IFG2:  P1IV P1IFG.2
        case  P1IV_P1IFG2:
            break;

        // Vector  P1IV_P1IFG3:  P1IV P1IFG.3
        case  P1IV_P1IFG3:
            break;

        // Vector  P1IV_P1IFG4:  P1IV P1IFG.4
        case  P1IV_P1IFG4:
            break;

        // Vector  P1IV_P1IFG5:  P1IV P1IFG.5
        case  P1IV_P1IFG5:
            break;

        // Vector  P1IV_P1IFG1:  P1IV P1IFG.6
        case  P1IV_P1IFG6:
            break;

        // Vector  P1IV_P1IFG7:  P1IV P1IFG.7
        case  P1IV_P1IFG7:
            break;

        // Default case
        default:
            break;
    }
}

/***************************************************************************//**
 * @}
 ******************************************************************************/
