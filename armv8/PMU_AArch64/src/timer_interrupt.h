/* Bare-metal example for Armv8-A */

/* Timer and interrupts */

/* Copyright (c) 2016 Arm Limited (or its affiliates). All rights reserved. */
/* Use, modification and redistribution of this file is subject to your possession of a     */
/* valid End User License Agreement for the Arm Product of which these examples are part of */
/* and your compliance with all applicable terms and conditions of such licence agreement.  */

/**
 * Initialize and enable Timer0 interrupt on core 0
 */
void initTimerInterrupt(void);

/**
 * Handle the interrupt
 */
void timerInterruptHandler(void);

/**
 * The number of 35MHz ticks from the timer
 */
unsigned long getTimerTicks(void);
