/*-------------------------------------------------------------------*/
/* Performance Monitoring Unit (PMU) Example Code for Armv7 Cortex-A */
/*-------------------------------------------------------------------*/

/* Copyright (c) 2010-2019 Arm Limited (or its affiliates). All rights reserved. */
/* Use, modification and redistribution of this file is subject to your possession of a     */
/* valid End User License Agreement for the Arm Product of which these examples are part of */
/* and your compliance with all applicable terms and conditions of such licence agreement.  */


#include <stdio.h>
#include "v7_pmu.h"

extern void enable_caches(void); // in startup.S

void start_perfmon(void)
{
	enable_pmu_user_access();  // Allow access to PMU from User Mode
    enable_pmu();              // Enable the PMU

    pmn_config(0, 0x08);       // Configure counter 0 to count Instruction architecturally executed
    pmn_config(1, 0x03);       // Configure counter 1 to count D-Cache Misses, this has a performance impact

    enable_pmn(0);			   // Enable counter 0
    enable_pmn(1);             // Enable counter 1

    enable_caches();           // Enable cache, in startup.S

    enable_ccnt();             // Enable CCNT
    reset_ccnt();              // Reset the CCNT (cycle counter)
    reset_pmn();               // Reset the configurable counters
}

void stop_perfmon(void)
{
	disable_pmn(0);            // Stop counter 0
	disable_pmn(1);            // Stop counter 1
    disable_ccnt();            // Stop CCNT
    printf("\nPerformance monitor results");
    printf("\n---------------------------\n");
	printf("Instruction architecturally executed = %u\n", read_pmn(0) );
	printf("D-Cache Misses = %u\n", read_pmn(1) );
    printf("Cycle Count (CCNT) = %u\n",    read_ccnt() );

}
