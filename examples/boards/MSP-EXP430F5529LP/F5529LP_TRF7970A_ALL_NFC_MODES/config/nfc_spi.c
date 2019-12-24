//*****************************************************************************
//
// nfc_spi.c - SPI Configuration and transmission APIs
//
// Copyright (c) 2015 Texas Instruments Incorporated.  All rights reserved.
// TI Information - Selective Disclosure
//
//*****************************************************************************
#include "nfc_spi.h"
#include "usci_b_spi.h"
#include "gpio.h"
#include "mcu.h"
#include "ucs.h"

//===============================================================

// Direct Mode disabled
uint8_t g_direct_mode = 0;

void SPI_SendByte(uint8_t data){

    UCB0TXBUF = data;

    while (UCB0STAT & UCBUSY);
}

uint8_t SPI_ReceiveByte(){

	UCB0TXBUF = 0x00;

	while (UCB0STAT & UCBUSY);

    return UCB0RXBUF;

}

void SPI_directCommand(uint8_t command)
{
	volatile uint8_t x;

	SLAVE_SELECT_LOW; 						// Start SPI Mode

	// set Address/Command Word Bit Distribution to command
	command = (0x80 | command);					// command
	command = (0x9f & command);					// command code

	SPI_SendByte( command );

	SLAVE_SELECT_HIGH; 						//Stop SPI Mode
}

//===============================================================

void SPI_rawWrite(uint8_t *pbuf, uint8_t length)
{	
	volatile uint8_t x;

	SLAVE_SELECT_LOW; 						//Start SPI Mode
	while(length-- > 0)	{
	    SPI_SendByte( *pbuf++ );
    }

	if(g_direct_mode == 0x00)
	{
		SLAVE_SELECT_HIGH; 						// Stop SPI Mode
	}
}

void SPI_readCont(uint8_t *pbuf, uint8_t reg, uint8_t length)
{	
	volatile int8_t x;

	SLAVE_SELECT_LOW; 						// Start SPI Mode

	// Address/Command Word Bit Distribution
	// address, write, single (fist 3 bits = 0)
	reg = (0x1f & reg);					// register address
	reg = (0x60 | reg); 					// address, read, continuous
	SPI_SendByte( reg);

	while(length > 0)
	{
		*pbuf = SPI_ReceiveByte();
		pbuf++;
		length--;
	}

	SLAVE_SELECT_HIGH; 						// Stop SPI Mode

}

void SPI_readSingle(uint8_t *pbuf, uint8_t reg)
{			
	volatile uint8_t x;

	SLAVE_SELECT_LOW; 						// Start SPI Mode

	// Address/Command Word Bit Distribution
	// address, write, single (fist 3 bits = 0)
	reg = (0x1f & reg);				// register address
	reg = (0x40 | reg); 			// address, read, single
	

	SPI_SendByte(reg);  			// Previous data to TX, RX

	*pbuf = SPI_ReceiveByte();

	if(g_direct_mode == 0x00)
	{
		SLAVE_SELECT_HIGH; 						// Stop SPI Mode
	}
}

void SPI_setup(void)
{	
   USCI_B_SPI_masterInit(SPI_MODULE_BASE_ADDR,
						USCI_B_SPI_CLOCKSOURCE_SMCLK,
						MCLK_FREQ,
						SPI_CLK_FREQ,
						USCI_B_SPI_MSB_FIRST,
						USCI_B_SPI_PHASE_DATA_CHANGED_ONFIRST_CAPTURED_ON_NEXT,
						USCI_B_SPI_CLOCKPOLARITY_INACTIVITY_LOW
						);

    GPIO_setAsPeripheralModuleFunctionOutputPin(SPI_PORT, SPI_CLK | SPI_MOSI | SPI_MISO);

    USCI_B_SPI_enable(SPI_MODULE_BASE_ADDR);
}

void SPI_writeSingle(uint8_t data, uint8_t reg)
{
	volatile int8_t x;

	SLAVE_SELECT_LOW; 						// Start SPI Mode

	// Address/Command Word Bit Distribution
	// address, write, single (fist 3 bits = 0)
	reg = (0x1f &reg);				// register address


	SPI_SendByte(reg);
	SPI_SendByte(data);

	
	if(g_direct_mode == 0x00)
	{
		SLAVE_SELECT_HIGH; 						// Stop SPI Mode
	}
}

void SPI_writeCont(uint8_t *pbuf, uint8_t reg, uint8_t length)
{
	volatile int8_t x;

	SLAVE_SELECT_LOW; 						// Start SPI Mode

	// Address/Command Word Bit Distribution
	// address, write, single (fist 3 bits = 0)
	reg = (0x20 | reg); 				// address, write, continuous
	reg = (0x3f & reg);					// register address

	SPI_SendByte(reg);


	while(length > 0)
	{
	    SPI_SendByte(*pbuf);

		pbuf++;
		length--;

	}

	SLAVE_SELECT_HIGH; 						// Stop SPI Mode
}

void SPI_writePacket(uint8_t *pbuf, uint8_t crc_bit, uint8_t total_length, uint8_t payload_length, bool header_enable, uint8_t broken_bits, bool bNFCAHeader)
{
	uint8_t ui8LenLowerNibble;
	uint8_t ui8LenHigherNibble;
	uint16_t ui16TotalLength;

	ui16TotalLength = total_length;

	// Add one to the total length sent to the TRF7970A
	if(bNFCAHeader == true)
	{
		ui16TotalLength++;
	}

	ui8LenLowerNibble = (ui16TotalLength & 0x0F) << 4;
	ui8LenHigherNibble = (uint8_t) ((ui16TotalLength & 0x0FF0) >> 4);

	SLAVE_SELECT_LOW; 						// Start SPI Mode

	if(header_enable == TRUE)
	{
		// RESET FIFO
		SPI_SendByte(0x8F);

		// CRC COMMAND
		SPI_SendByte( 0x90 | (crc_bit & 0x01));	// Previous data to TX, RX

		// WRITE TO LENGTH REG
		SPI_SendByte(0x3D);

		if(broken_bits == 0x00)
		{
			// LENGTH HIGH Nibble
			SPI_SendByte(ui8LenHigherNibble);	// Previous data to TX, RX

			// LENGTH LOW Nibble
			SPI_SendByte(ui8LenLowerNibble);	// Previous data to TX, RX
		}
		else
		{
			// Broken Bit
			SPI_SendByte(0x00);
			SPI_SendByte((broken_bits << 0x01) | 0x01);
		}

		if(bNFCAHeader == true)
		{
			// Type A Header Byte
			SPI_SendByte(0xF0);
		}
	}
	else
	{
		// Always do a continious write to the FIFO (even if there is only one byte)
		SPI_SendByte(0x3F);
	}

	while(payload_length > 0)
	{
	    SPI_SendByte(*pbuf);					// Previous data to TX, RX
		pbuf++;
		payload_length--;
	}
	SLAVE_SELECT_HIGH; 						// Stop SPI Mode
}
