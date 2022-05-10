/*
** Copyright (c) 2016-2022 Arm Limited (or its affiliates). All rights reserved.
** Use, modification and redistribution of this file is subject to your possession of a
** valid End User License Agreement for the Arm Product of which these examples are part of 
** and your compliance with all applicable terms and conditions of such licence agreement.
*/

/* This file contains the main() program that displays a welcome message, enables the caches,
   performs a float calculation to demonstrate floating point, then runs the main application (sorts) */

#include <stdio.h>

extern void enable_mpu(void);
extern void enable_caches(void);
extern void compare_sorts(void);
float calculate( float a, float b );


int main(void)
{
    asm(".global __use_hlt_semihosting");

    /* Enable the MPU */
    enable_mpu();

    /* Display a welcome message via semihosting */
    printf("Cortex-R52 bare-metal startup example\n");

    /* Enable the caches */
    enable_caches();

    /* Perform a float calculation to demonstrate floating point (using the FPU, if compiled appropriately) */
#ifdef __ARM_FP
    printf("Floating point calculation using the FPU...\n");
#else
    printf("Floating point calculation using the software floating point library (no FPU)...\n");
#endif
    printf("Float result is        %f\n", calculate(1.5f, 2.5f));
    printf("Float result should be 0.937500\n");

    /* Run the main application (sorts) */
    compare_sorts();

    return 0;
}

/* Float calculation to demonstrate the FPU, not inlined for clarity */
__attribute__((noinline)) float calculate( float a, float b )
{
    float temp1, temp2;

    temp1 = a + b;
    temp2 = a * b;
    return temp2 / temp1;
}
