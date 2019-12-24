//*****************************************************************************
//
// lp_buttons.h - F5529 Launchpad Buttons
//
// Copyright (c) 2014 Texas Instruments Incorporated.  All rights reserved.
// TI Information - Selective Disclosure
//
//*****************************************************************************

#ifndef LP_BUTTONS_H
#define LP_BUTTONS_H

#include <stdint.h>

#ifdef MSP430F5529_LAUNCHPAD_ENABLED
#define BUTTON_S2       0x0002
#define BUTTON_S1       0x0200
#define BUTTON_ALL      0x0202

extern volatile uint16_t buttonsPressed;
extern volatile  uint8_t buttonDebounce;

extern void Buttons_init(uint16_t buttonsMask);
extern void Buttons_interruptEnable(uint16_t buttonsMask);
extern void Buttons_interruptDisable(uint16_t buttonsMask);
#endif

#endif /* HAL_BUTTONS_H */
