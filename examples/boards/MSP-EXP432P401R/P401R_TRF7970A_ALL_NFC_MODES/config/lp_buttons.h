//*****************************************************************************
//
// lp_buttons.h - P401R Launchpad Buttons
//
// Copyright (c) 2014 Texas Instruments Incorporated.  All rights reserved.
// TI Information - Selective Disclosure
//
//*****************************************************************************

#ifndef LP_BUTTONS_H
#define LP_BUTTONS_H

#include <stdint.h>

#define BUTTON_PORT_DIR   P1DIR
#define BUTTON_PORT_OUT   P1OUT
#define BUTTON_PORT_SEL	  P1SEL0
#define BUTTON_PORT_REN	  P1REN

#define BUTTON_PORT_IE    P1IE
#define BUTTON_PORT_IES   P1IES
#define BUTTON_PORT_IFG   P1IFG
#define BUTTON_PORT_IN    P1IN

#define BUTTON1_PIN       BIT1
#define BUTTON2_PIN       BIT4

#define BUTTON1_IFG       P1IFG
#define BUTTON2_IFG       P1IFG

#define BUTTON_S2       0x02
#define BUTTON_S1       0x10
#define BUTTON_ALL      0x12

extern volatile uint16_t buttonsPressed;
extern volatile  uint8_t buttonDebounce;

extern void Buttons_init(uint16_t buttonsMask);
extern void Buttons_interruptEnable(uint16_t buttonsMask);
extern void Buttons_interruptDisable(uint16_t buttonsMask);

#endif /* HAL_BUTTONS_H */
