/*
 * Armv8-A - main
 *
 * Copyright (c) 2011-2019 Arm Limited (or its affiliates). All rights reserved.
 * Use, modification and redistribution of this file is subject to your possession of a
 * valid End User License Agreement for the Arm Product of which these examples are part of 
 * and your compliance with all applicable terms and conditions of such licence agreement.
 *
 */

#include <stdlib.h>
#include <stdio.h>

#ifdef USE_SERIAL_PORT
  #include "uart.h"
#endif

#ifdef USE_SERIAL_PORT
extern void UartInit(void);
#endif


// declaration of 'extern' functions
extern void setup_screen(void);  // in fireworks.c
extern void init_timer(void);    // in timer_interrupts.c
extern void fireworks(void);     // in fireworks.c


__attribute__((noreturn)) int main(void)
{
#ifdef USE_SERIAL_PORT
    UartInit();
#endif

    printf("\nArmv8-A single-core Fireworks Example, using Arm Compiler for Embedded 6\n\n");
    printf("Click on USERSW1 to stop/restart the Banner\n\n");
    setup_screen();    // in fireworks.c
    init_timer();

    //run the fireworks!!
    fireworks();

    for(;;) {} //loop forever
}
