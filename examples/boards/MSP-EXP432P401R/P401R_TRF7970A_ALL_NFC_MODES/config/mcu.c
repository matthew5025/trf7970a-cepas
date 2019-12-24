//*****************************************************************************
//
// mcu.c - MCU Configuration and host interface APIs
//
// Copyright (c) 2015 Texas Instruments Incorporated.  All rights reserved.
// TI Information - Selective Disclosure
//
//*****************************************************************************
// Include Header Files
#include "mcu.h"

volatile uint8_t * g_timeout_ptr;
uint32_t g_ui32RemainingTime;
volatile uint8_t g_ui8TimeoutFlag;
// Defines whether the timer is used for milliseconds (TIMER_COUNT_MS) or microseconds (TIME_COUNT_US)
volatile uint8_t g_ui8TimerResolution;
bool g_bSerialConnectionEstablished = false;

#define 	TIMER_COUNT_MS		0x01
#define		TIMER_COUNT_US		0x02

/* UART Configuration Parameter. These are the configuration parameters to
 * make the eUSCI A UART module to operate with a 115200 baud rate. These
 * values were calculated using the online calculator that TI provides
 * at:
 *http://processors.wiki.ti.com/index.php/USCI_UART_Baud_Rate_Gen_Mode_Selection
 */
const eUSCI_UART_Config uartConfig =
{
        EUSCI_A_UART_CLOCKSOURCE_SMCLK,          // SMCLK Clock Source
        0x68,                                    // BRDIV = 208
        0,                                       // UCxBRF = 0
        2,                                       // UCxBRS = 0
        EUSCI_A_UART_NO_PARITY,                  // No Parity
        EUSCI_A_UART_LSB_FIRST,                  // MSB First
        EUSCI_A_UART_ONE_STOP_BIT,               // One stop bit
        EUSCI_A_UART_MODE,                       // UART mode
        EUSCI_A_UART_LOW_FREQUENCY_BAUDRATE_GENERATION  // Low Frequency Mode
};

/* Application Defines  */
#define TIMER_PERIOD_1_MS    0x0177			// SMCLK = 12 MHz, Timer A Divider = 32, Timer Clk = 375kHz ==> 1 mS = 375 counts
#define TIMER_PERIOD_1_US    0x000C			// SMCLK = 12 MHz, Timer A Divider =  1, Timer Clk = 12Mhz  ==> 1 uS = 12 counts

/* Timer_A UpMode Configuration Parameter */
Timer_A_UpModeConfig upConfig =
{
        TIMER_A_CLOCKSOURCE_SMCLK,              // SMCLK Clock Source
		TIMER_A_CLOCKSOURCE_DIVIDER_4,          // SMCLK/16 = 750KHz
		TIMER_PERIOD_1_MS,                      // 5000 tick period
        TIMER_A_TAIE_INTERRUPT_DISABLE,         // Disable Timer interrupt
        TIMER_A_CCIE_CCR0_INTERRUPT_ENABLE ,    // Enable CCR0 interrupt
        TIMER_A_DO_CLEAR                        // Clear value
};

void timer_a_0_isr(void);

/**
 * @brief <b>Function Name</b>: MCU_init
 * @brief <b>Description</b>: Initialize MSP430, such as Clock, Port, MPU and
 * System Time.
 * @param Input value: None
 * @return Return value:None
 **/
void MCU_init(void)
{
    uint32_t currentPowerState;

    WDT_A_holdTimer();
    Interrupt_disableMaster();

//    /* Starting HFXT in non-bypass mode without a timeout. Before we start
//     * we have to change VCORE to 1 to support the 48MHz frequency */
//    PCM_setCoreVoltageLevel(PCM_VCORE1);
//    FlashCtl_setWaitState(FLASH_BANK0, 2);
//    FlashCtl_setWaitState(FLASH_BANK1, 2);
//    FLCTL_BANK0_RDCTL |= (FLCTL_BANK0_RDCTL_BUFI | FLCTL_BANK0_RDCTL_BUFD );
//    FLCTL_BANK1_RDCTL |= (FLCTL_BANK1_RDCTL_BUFI | FLCTL_BANK1_RDCTL_BUFD );
//    PCM_setPowerState(PCM_AM_DCDC_VCORE1);

    /* Get current power state */
    currentPowerState = PCM->CTL0 & PCM_CTL0_CPM_MASK;
    /* Transition to VCORE Level 1 from current power state properly */
    switch (currentPowerState)
    {
        case PCM_CTL0_CPM_MASK:                // AM0_LDO, need to switch to AM1_LDO
            while ((PCM->CTL1 & PCM_CTL1_PMR_BUSY));
            PCM->CTL0 = PCM_CTL0_KEY_VAL | PCM_CTL0_AMR_1;
            while ((PCM->CTL1 & PCM_CTL1_PMR_BUSY));
            if (PCM->IFG & PCM_IFG_AM_INVALID_TR_IFG)
//                error();                    // Error if transition was not successful
            break;
        case PCM_CTL0_CPM_4 :                // AM0_DCDC, need to switch to AM1_DCDC
            while ((PCM->CTL1 & PCM_CTL1_PMR_BUSY));
            PCM->CTL0 = PCM_CTL0_KEY_VAL | PCM_CTL0_AMR_5;
            while ((PCM->CTL1 & PCM_CTL1_PMR_BUSY));
            if (PCM->IFG & PCM_IFG_AM_INVALID_TR_IFG)
//                error();                    // Error if transition was not successful
            break;
        default:                            // Device is in some other state, which is unexpected
//            error();
        	 break;
    }

    /* Initialize main clock to 24MHz */
    CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_24);

    CS_initClockSignal(CS_MCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1 );
    CS_initClockSignal(CS_HSMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1 );
    /* Initialize SMCLK to 12MHz */
    CS_initClockSignal(CS_SMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_2 );

	Timer_A_registerInterrupt(TIMER_A0_BASE,TIMER_A_CCR0_INTERRUPT,timer_a_0_isr);

	Interrupt_enableMaster();
}

//===============================================================
// NAME: void McuDelayMillisecond (uint8_t n_ms)
//
// BRIEF: Is used to create delays.
//
// INPUTS:
//    Parameters:
//        uint8_t        n_ms        delay time in ms.
//
// OUTPUTS:
//
// PROCESS:    [1] do loop of 1 ms duration as often as required
//
// CHANGE:
// DATE          WHO    DETAIL
// 23Nov2010    RP    Original Code
//===============================================================

void MCU_delayMillisecond(uint32_t n_ms)
{
	g_timeout_ptr = (uint8_t *) &g_ui8TimeoutFlag;

	// Clear Flag
	*g_timeout_ptr = 0x00;

	g_ui8TimerResolution = TIMER_COUNT_MS;

	if(n_ms > 174)
	{
		g_ui32RemainingTime = n_ms - 174;
		n_ms = 174;
	}
	else
	{
		g_ui32RemainingTime = 0x00;
	}

	// Clear Flag
	*g_timeout_ptr = 0x00;

	upConfig.clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_32;
	upConfig.timerPeriod = TIMER_PERIOD_1_MS * n_ms;

	/* Configuring Timer_A1 for Up Mode */
	Timer_A_configureUpMode(TIMER_A0_BASE, &upConfig);

	Interrupt_enableInterrupt(INT_TA0_0);

	Timer_A_startCounter(TIMER_A0_BASE, TIMER_A_UP_MODE);

	// Wait until Flag is set
	while(*g_timeout_ptr != 0x01)
	{
		//
		// Enable Low Power Mode 0
		//
	}
	MCU_timerDisable();
}


void MCU_delayMicrosecond(uint32_t n_us)
{
	g_timeout_ptr = (uint8_t *) &g_ui8TimeoutFlag;

	// Clear Flag
	*g_timeout_ptr = 0x00;

	g_ui8TimerResolution = TIMER_COUNT_US;

	if(n_us > 5400)
	{
		g_ui32RemainingTime = n_us - 5400;
		n_us = 5400;
	}
	else
	{
		g_ui32RemainingTime = 0x00;
	}

	// Clear Flag
	*g_timeout_ptr = 0x00;

	upConfig.clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_1;
	upConfig.timerPeriod = TIMER_PERIOD_1_US * n_us;

	/* Configuring Timer_A1 for Up Mode */
	Timer_A_configureUpMode(TIMER_A0_BASE, &upConfig);

	Interrupt_enableInterrupt(INT_TA0_0);

	Timer_A_startCounter(TIMER_A0_BASE, TIMER_A_UP_MODE);

	// Wait until Flag is set
	while(*g_timeout_ptr != 0x01)
	{
		//
		// Enable Low Power Mode 0
		//
	}
	MCU_timerDisable();
}

/**
 * @brief <b>Function Name</b>: Clock_init
 * @brief <b>Description</b>: This function to initialize MSP430 clock, to set
 * MCLK and SMCLK to 1MHz.For MSP430FR5969, set ACLK = LFXT1 = 32768Hz, so
 * External watch crystal installed on XIN XOUT is required for ACLK.
 * @param Input Parameters: None
 * @return Return Values: None
 * @note External watch crystal installed on XIN XOUT is required for ACLK,By
 * default, the MSP430 uses XT1 to source ACLK.
 **/
void MCU_clockInit()
{

}


/**
 * @brief <b>Function Name</b>: Port_init
 * @brief <b>Description</b>: This function to initialize MSP430 Port Pins to I/O.
 * Initializes the unused pins as outputs for low power consumption.
 * @param Input Parameters: None
 * @return Return Values: None
 **/
void MCU_portInit()
{

}

/**
 * @brief <b>Function Name</b>: Timer_init
 * @brief <b>Description</b>:
 * This function to initialize MSP430 Timer. Set SMCLK as Timer clock source,
 * and then set CCR0 as upmode, enable CCr0 interrupt and it will go to related
 * interrupt every 0.5s.
 * @param Input Parameters:
 * uint16_t timeout_ms has a maximum value of 8191 mS = 8.1 seconds
 * uint8_t * timeout_flag is the flag to set if the time is elapsed.
 * @return Return Values: None
 **/
void MCU_timerInit(uint16_t timeout_ms, uint8_t * timeout_flag)
{
	g_timeout_ptr = (uint8_t *) timeout_flag;

	// Clear Flag
	*g_timeout_ptr = 0x00;

	/* Enabling interrupts and starting the timer */

	g_ui8TimerResolution = TIMER_COUNT_MS;

	if(timeout_ms == 0x00)
	{
		*timeout_flag = 0x01;
	}
	else
	{
		if(timeout_ms > 174)
		{
			g_ui32RemainingTime = timeout_ms - 174;
			timeout_ms = 174;
		}
		else
		{
			g_ui32RemainingTime = 0x00;
		}

		upConfig.clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_32;
		upConfig.timerPeriod = TIMER_PERIOD_1_MS * timeout_ms;

		Timer_A_clearInterruptFlag(TIMER_A0_BASE);

		/* Configuring Timer_A1 for Up Mode */
		Timer_A_configureUpMode(TIMER_A0_BASE, &upConfig);

		Interrupt_enableInterrupt(INT_TA0_0);

		Timer_A_startCounter(TIMER_A0_BASE, TIMER_A_UP_MODE);
	}

}

void MCU_timerDisable(void)
{
	Timer_A_disableInterrupt(INT_TA0_0);
	TA0CTL &= ~0x30;
	Timer_A_stopTimer(INT_TA0_0);
	Timer_A_clearTimer(INT_TA0_0);
}

void Serial_sendPacket(uint8_t * pui8Data, uint8_t ui8Length)
{
	uint8_t ui8Index;

	for(ui8Index=0;ui8Index<ui8Length;ui8Index++)
	{
		// Send Data
		UART_transmitData(EUSCI_A0_BASE,pui8Data[ui8Index]);
	}
}

void Serial_init(void)
{

	/* Selecting P1.2 and P1.3 in UART mode and P1.0 as output (LED) */
	GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1,
			GPIO_PIN2 | GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);

	/* Configuring UART Module */
	UART_initModule(EUSCI_A0_BASE, &uartConfig);

	/* Enable UART module */
	UART_enableModule(EUSCI_A0_BASE);

	// Set P1.0 to output direction
	GPIO_setAsOutputPin(
			GPIO_PORT_P1,
			GPIO_PIN0
	);

	UART_enableInterrupt(EUSCI_A0_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT);
	Interrupt_enableInterrupt(INT_EUSCIA0);

	UART_registerInterrupt(EUSCI_A0_BASE,EusciA0_ISR);

}

void Serial_printf(char * pui8String, tNFCHostCommands eCmdType)
{
	uint8_t pui8Packet[3];

	if(g_bSerialConnectionEstablished == true)
	{
		// Send Packet Header
		pui8Packet[0] = 0xFE;
		pui8Packet[1] = (uint8_t) eCmdType;
		pui8Packet[2] = (uint8_t) strlen(pui8String);
		Serial_sendPacket(pui8Packet,3);

		//send the chars from buf
		Serial_sendPacket((uint8_t *) pui8String,strlen(pui8String));
	}
}

void Serial_printBuffer(char * pui8String, uint8_t ui8Length, tNFCHostCommands eCmdType)
{
	uint8_t pui8Packet[3];

	if(g_bSerialConnectionEstablished == true && eCmdType != IDLE_CMD)
	{
		// Send Packet Header
		pui8Packet[0] = 0xFE;
		pui8Packet[1] = (uint8_t) eCmdType;
		pui8Packet[2] = (uint8_t) ui8Length;
		Serial_sendPacket(pui8Packet,3);

		//send the chars from buf
		Serial_sendPacket((uint8_t *) pui8String,ui8Length);
	}
	else if(eCmdType == IDLE_CMD)
	{
		//send the chars from buf
		Serial_sendPacket((uint8_t *) pui8String,ui8Length);
	}
	else
	{
		//send the chars from buf
		Serial_sendPacket((uint8_t *) pui8String,ui8Length);
	}
}

void Serial_printfLine(char * pui8String)
{
	//send the chars from buf
	Serial_sendPacket((uint8_t *) pui8String,strlen(pui8String));
}

uint8_t convertNibbleToAscii(uint8_t ui8Nibble)
{
	uint8_t	ui8AsciiOut = ui8Nibble;

	if(ui8Nibble > 9)	// If req ASCII A-F then add 7(hex)
	{
		ui8AsciiOut = ui8AsciiOut + 0x07;
	}

	// Add offset to convert to Ascii
	ui8AsciiOut = ui8AsciiOut + 0x30;

	return(ui8AsciiOut);
}

void
convertByteToAscii(uint8_t ui8byte, uint8_t pui8Buffer[3])
{
	uint8_t	ui8Temp1 = 0;

	// Convert High Nibble
	ui8Temp1 = (ui8byte >> 4) & 0x0F;			// get high nibble
	pui8Buffer[0] = convertNibbleToAscii(ui8Temp1);		// convert to ASCII

	// Convert Low Nibble
	ui8Temp1 = ui8byte & 0x0F;					// get low nibble
	pui8Buffer[1] = convertNibbleToAscii(ui8Temp1);		// convert to ASCII

	pui8Buffer[2] = 0x00;
}

void
convertWordToAscii(uint16_t ui16Word, uint8_t pui8Buffer[5])
{
	uint8_t	ui8Temp1 = 0;

	// Convert High Nibble
	ui8Temp1 = (ui16Word >> 12) & 0x0F;			// get high nibble
	pui8Buffer[0] = convertNibbleToAscii(ui8Temp1);		// convert to ASCII

	ui8Temp1 = (ui16Word >> 8) & 0x0F;
	pui8Buffer[1] = convertNibbleToAscii(ui8Temp1);		// convert to ASCII

	ui8Temp1 = (ui16Word >> 4) & 0x0F;
	pui8Buffer[2] = convertNibbleToAscii(ui8Temp1);		// convert to ASCII

	// Convert Low Nibble
	ui8Temp1 = ui16Word & 0x0F;					// get low nibble
	pui8Buffer[3] = convertNibbleToAscii(ui8Temp1);		// convert to ASCII

	pui8Buffer[4] = 0x00;
}

void timer_a_0_isr(void)
{
	P3OUT ^= BIT2;
	if(g_ui8TimerResolution == TIMER_COUNT_MS)
	{
		if(g_ui32RemainingTime > 174)
		{
			g_ui32RemainingTime = g_ui32RemainingTime - 174;
			upConfig.timerPeriod = TIMER_PERIOD_1_MS * 174;

			Timer_A_configureUpMode(TIMER_A0_BASE, &upConfig);

			Timer_A_startCounter(TIMER_A0_BASE, TIMER_A_UP_MODE);
		}
		else if(g_ui32RemainingTime > 0)
		{
			upConfig.timerPeriod = TIMER_PERIOD_1_MS * g_ui32RemainingTime;
			g_ui32RemainingTime = 0x00;

			Timer_A_configureUpMode(TIMER_A0_BASE, &upConfig);

			Timer_A_startCounter(TIMER_A0_BASE, TIMER_A_UP_MODE);
		}
		else
		{
			*g_timeout_ptr = 0x01;
		}
	}
	else if(g_ui8TimerResolution == TIMER_COUNT_US)
	{
		if(g_ui32RemainingTime > 5400)
		{
			g_ui32RemainingTime = g_ui32RemainingTime - 5400;
			upConfig.timerPeriod = TIMER_PERIOD_1_US * 5400;

			Timer_A_configureUpMode(TIMER_A0_BASE, &upConfig);

			Timer_A_startCounter(TIMER_A0_BASE, TIMER_A_UP_MODE);
		}
		else if(g_ui32RemainingTime > 0)
		{
			upConfig.timerPeriod = TIMER_PERIOD_1_US * g_ui32RemainingTime;
			g_ui32RemainingTime = 0x00;

			Timer_A_configureUpMode(TIMER_A0_BASE, &upConfig);

			Timer_A_startCounter(TIMER_A0_BASE, TIMER_A_UP_MODE);
		}
		else
		{
			*g_timeout_ptr = 0x01;
		}
	}
    Timer_A_clearCaptureCompareInterrupt(TIMER_A0_BASE,
            TIMER_A_CAPTURECOMPARE_REGISTER_0);
}

