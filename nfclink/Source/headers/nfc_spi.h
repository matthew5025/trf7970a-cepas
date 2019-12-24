//*****************************************************************************
//
// nfc_spi.h - SPI Driver
//
// Copyright (c) 2015 Texas Instruments Incorporated.  All rights reserved.
// TI Information - Selective Disclosure
//
//*****************************************************************************
#ifndef _NFC_SPI_H_
#define _NFC_SPI_H_

#include "mcu.h"
#include "types.h"
#include "trf79x0.h"

//
// Macro definitions
//
/*! SPI Bit Rate */
#define SPI_CLK_FREQ				(4*MHZ)

//
// Function Prototypes
//
void SPI_directCommand(uint8_t command);
void SPI_usciSet(void);
void SPI_rawWrite(uint8_t *pbuf, uint8_t length);
void SPI_readSingle(uint8_t *pbuf, uint8_t reg);
void SPI_readCont(uint8_t *pbuf, uint8_t reg, uint8_t length);
void SPI_setup(void);
void SPI_writeSingle(uint8_t data, uint8_t reg);
void SPI_writeCont(uint8_t *pbuf, uint8_t reg, uint8_t length);
void SPI_writePacket(uint8_t *pbuf, uint8_t crc_bit, uint8_t total_length, uint8_t payload_length, bool header_enable, uint8_t broken_bits, bool bNFCAHeader);

#endif
