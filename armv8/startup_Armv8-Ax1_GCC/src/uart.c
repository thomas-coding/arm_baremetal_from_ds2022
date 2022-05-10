/*
 * PL011 UART driver
 *
 * Copyright (c) 2005-2016 Arm Limited (or its affiliates). All rights reserved.
 * Use, modification and redistribution of this file is subject to your possession of a
 * valid End User License Agreement for the Arm Product of which these examples are part of 
 * and your compliance with all applicable terms and conditions of such licence agreement.
 */

#include <stdio.h>

#include "uart.h"

/*
 * UART instance: will be placed using the scatter file
 */
static struct pl011_uart uart __attribute__((section (".pl011")));

void UartInit(void)
{
    /*
     * ensure the UART is disabled
     */
    uart.UARTCR  = 0x0;

    /*
     * OK, now program this thing up
     */
    uart.UARTECR   = 0x0;  // Clear the receive status (i.e. error) register
    uart.UARTLCR_H = 0x0 | PL011_LCR_WORD_LENGTH_8 | PL011_LCR_FIFO_DISABLE | \
        PL011_LCR_ONE_STOP_BIT | PL011_LCR_PARITY_DISABLE | PL011_LCR_BREAK_DISABLE;

    uart.UARTIBRD = PL011_IBRD_DIV_38400;
    uart.UARTFBRD = PL011_FBRD_DIV_38400;

    /*
     * mask and clear all interrupts
     */
    uart.UARTIMSC = 0x0;
    uart.UARTICR  = PL011_ICR_CLR_ALL_IRQS;

    uart.UARTCR  = 0x0 | PL011_CR_UART_ENABLE | PL011_CR_TX_ENABLE | PL011_CR_RX_ENABLE;

    return;
}

void uart_putc_polled(char c)
{
    /* Wait for UART to become free */
    /* Note that FIFOs are not being used here */
    while (uart.UARTFR & PL011_FR_BUSY_FLAG);

    /* Write character and send it */
    uart.UARTDR = c;
}

char uart_getchar_polled(void)
{
    /* Wait for UART to become free */
    /* Note that FIFOs are not being used here */
    while (uart.UARTFR & PL011_FR_BUSY_FLAG);
    /* Read character received */
    return uart.UARTDR;
}
