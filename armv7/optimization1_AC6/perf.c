/*-------------------------------------------------------------------*/
/* Performance Monitoring Unit (PMU) Example Code for Armv7 Cortex-A */
/*-------------------------------------------------------------------*/

/* Copyright (c) 2010-2012 Arm Limited (or its affiliates). All rights reserved. */
/* Use, modification and redistribution of this file is subject to your possession of a     */
/* valid End User License Agreement for the Arm Product of which these examples are part of */
/* and your compliance with all applicable terms and conditions of such licence agreement.  */


#include <stdio.h>
#include "v7_pmu.h"


extern int $Super$$main(void);   // in sorts.c
extern void enable_caches(void); // in startup.s


/* Dummy IRQ handler */
void C_interrupt_handler(void)
{
    // does nothing in this example
}


/* main() function entered after normal C run-time environment & library start-up has completed */
/* This is a performance counting "wrapper" around the primary application. */
/* This wrapper is named $Sub$$main() to ensure it is called before $Super$$main() */
/* $Super$$main() is the application's main() function to be called after the PMU has been setup and cache enabled */

int $Sub$$main(void)
{
    printf("Performance Monitoring Unit (PMU) Example Code for Cortex-A* family\n");
    printf("===================================================================\n\n");

    enable_pmu_user_access();  // Allow access to PMU from User Mode

    enable_pmu();              // Enable the PMU

   pmn_config(0, 0x01);       // Configure counter 0 to count I-Cache Misses
    pmn_config(1, 0x03);       // Configure counter 1 to count D-Cache Misses
// Alternatively...
//    pmn_config(0, 0x06);       // Configure counter 0 to count Data Reads
//    pmn_config(1, 0x07);       // Configure counter 1 to count Data Writes
    pmn_config(2, 0x08);       // Configure counter 2 to count Instructions Executed
    pmn_config(3, 0x0C);       // Configure counter 3 to count PC Changes

    enable_pmn(0);             // Enable counter 0
    enable_pmn(1);             // Enable counter 1
    enable_pmn(2);             // Enable counter 2
    enable_pmn(3);             // Enable counter 3
    enable_ccnt();             // Enable CCNT

    enable_caches();           // in startup.s

    reset_ccnt();              // Reset the CCNT (cycle counter)
    reset_pmn();               // Reset the configurable counters

    // Start of measured test run
    $Super$$main();            // External application to be measured, with a main() function
    // End of measured test run

    disable_ccnt();            // Stop CCNT
    disable_pmn(0);            // Stop counter 0
    disable_pmn(1);            // Stop counter 1
    disable_pmn(2);            // Stop counter 2
    disable_pmn(3);            // Stop counter 3

    printf("\nPerformance monitor results");
    printf("\n---------------------------\n");
    printf("I-Cache Misses = %u\n",        read_pmn(0) );
    printf("D-Cache Misses = %u\n",        read_pmn(1) );
    printf("Instructions Executed = %u\n", read_pmn(2) );
    printf("PC Changes = %u\n",            read_pmn(3) );
    printf("Cycle Count (CCNT) = %u\n",    read_ccnt() );

    return 0;
}
