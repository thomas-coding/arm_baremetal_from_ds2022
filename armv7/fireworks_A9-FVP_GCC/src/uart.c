/*
** Copyright (c) 2011-2017 Arm Limited (or its affiliates). All rights reserved.
** Use, modification and redistribution of this file is subject to your possession of a
** valid End User License Agreement for the Arm Product of which these examples are part of 
** and your compliance with all applicable terms and conditions of such licence agreement.
*/

/* Simple polled UART driver for Cortex-A9 VE FVP model */

// Ensure uart_init() is called before any other functions in this file.

#define UART0_BASE 0x1C090000
#define UART0_IO  (volatile unsigned char *)(UART0_BASE)
#define UART0_LSR (volatile unsigned char *)(UART0_BASE + 0x3)
#define UART0_FR  (volatile unsigned int *) (UART0_BASE + 0x18)
#define UART0_CR  (volatile unsigned int *) (UART0_BASE + 0x30)


void uart_init(void)
{
	*UART0_CR = *UART0_CR | 0x301;	// Enable UART and enable TX/RX
}

void uart_putc_polled(char c)
{
	while (*UART0_FR & 0x08)
		;	// Wait for UART TX to become free. Note that FIFOs are not being used here
	*UART0_IO = c;
}

char uart_getchar_polled(void)
{
	while (*UART0_FR & 0x08)
		;	// Retrieve characters from UART0
	return *UART0_IO;
}
