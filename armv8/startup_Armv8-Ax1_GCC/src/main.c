/*
 * Armv8-A - main
 *
 * Copyright (c) 2011-2018 Arm Limited (or its affiliates). All rights reserved.
 * Use, modification and redistribution of this file is subject to your possession of a
 * valid End User License Agreement for the Arm Product of which these examples are part of 
 * and your compliance with all applicable terms and conditions of such licence agreement.
 *
 */

#include <stdlib.h>
#include <stdio.h>

// declaration of 'extern' functions
extern void init_timer(void);    // in timer_interrupts.c


__attribute__((noreturn)) int main(void)
{
    printf("\nArmv8-A single-core startup code example, using GCC for bare-metal targets\n\n");
    init_timer();

    for(;;) {} //loop forever
}
