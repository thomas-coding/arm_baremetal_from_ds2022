/*
** Copyright (c) 2011-2019 Arm Limited (or its affiliates). All rights reserved.
** Use, modification and redistribution of this file is subject to your possession of a
** valid End User License Agreement for the Arm Product of which these examples are part of 
** and your compliance with all applicable terms and conditions of such licence agreement.
*/

/* Simple polled UART driver */

// This code assumes baud rate, etc has already been setup by Beagle's X-Loader or U-Boot
// Beagle's UART3 is connected to the Serial port

#define UART3_BASE 0x49020000
#define UART_IO  (volatile unsigned char *)(UART3_BASE)
#define UART_LSR (volatile unsigned char *)(UART3_BASE + 0x14)

#define TX_FIFO_E 0x20
#define RX_FIFO_E 0x01


void uart_putc_polled(char c)
{
    while ( !(*UART_LSR & TX_FIFO_E));
    *UART_IO = c;
}

char uart_getchar_polled(void)
{
    while ( !(*UART_LSR & RX_FIFO_E));
    return *UART_IO;
}
