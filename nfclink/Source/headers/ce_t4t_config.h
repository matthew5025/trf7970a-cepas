//*****************************************************************************
//
// ce_t4t_config.h - Define Card Emulation structure
//
// Copyright (c) 2015 Texas Instruments Incorporated.  All rights reserved.
// TI Information - Selective Disclosure
//
//*****************************************************************************

#ifndef __CE_T4T_CONFIG_H__
#define __CE_T4T_CONFIG_H__

#include "trf79x0.h"
#include "ndef_image.h"

typedef struct
{
	uint16_t ui16Type4FileId;
	uint8_t * pui8Type4File;
	uint16_t ui16Type4FileLen;
	bool bReadOnly;
}tType4File;

typedef struct
{
	uint8_t * pui8AppId;
	uint8_t ui8AppIdLen;
	tType4File * pui8Type4FileArray;
	uint8_t ui8Type4FileLen;
}tType4App;

typedef struct
{
	tType4App * sType4AppArray; 
	uint8_t ui8AppArrayLen;
}tType4AppDS;

typedef struct
{
  uint16_t ui16PacketLength;
  uint8_t * pui8NdefDataPtr;
}sNdefPacket;


extern void T4T_CE_init(void);

extern void T4T_CE_initNDEF(void);

extern void T4T_CE_switchRotatingNDEF(void);

extern void T4T_CE_switchImageNDEF(void);

extern void T4T_CE_updateNDEF(uint8_t * pui8Data, uint16_t ui16NdefLen);

#endif
