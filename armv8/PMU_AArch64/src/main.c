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

#include "primes.h"
#include "v8_aarch64.h"
#include "MP_Mutexes.h"
#include "GICv3.h"
#include "GICv3_gicc.h"
#include "pmu_interrupt.h"
#include "timer_interrupt.h"

// compile-time control for the max number of CPUs
#define nCPUs 8

static unsigned int cpu_active_count = 0;
static unsigned int cpu_finished_count = 0;

// printf lock to regulate CPU access to an output device
#if defined ( __GNUC__ ) /* GCC */
mutex print_lock __attribute__ ((aligned (64)));  // not needed for Arm-supplied C library that handles printf locking itself
#endif

/*
 * void MainApp(void)
 *
 *    called by all CPUs to run SMP applications, this is the
 *    application start point for all secondary CPUs, and is called
 *    after other startup code has executed on the primary CPU
 *
 * Inputs
 *   <none>
 *
 * Returns
 *   <function does not return>
 */
__attribute__((noreturn)) void MainApp(void)
{
    unsigned long core;

    core = GetCoreNumber();

    if (core >= nCPUs)
    {
#if defined ( __GNUC__ ) /* GCC */
      _mutex_acquire(&print_lock);  // not needed for Arm-supplied C library that handles printf locking itself
#endif
      printf("Error: More CPU cores are running than the maximum this program expects\n");
      printf("Change the value of nCPUs.  Exiting...\n");
#if defined ( __GNUC__ ) /* GCC */
      _mutex_release(&print_lock);
#endif
      exit(1);
    }

    __atomic_add_fetch(&cpu_active_count, 1, __ATOMIC_RELAXED);

    initPmuInterrupt();

    // Enable interrupts
    asm ("msr DAIFClr, #0xF");
    setICC_IGRPEN1_EL1(igrpEnable);

#if defined ( __GNUC__ ) /* GCC */
      _mutex_acquire(&print_lock);  // not needed for Arm-supplied C library that handles printf locking itself
#endif
    printf("CPU %lu: Starting calculation\n", core);
#if defined ( __GNUC__ ) /* GCC */
    _mutex_release(&print_lock);
#endif

    calculatePrimes(core);

#if defined ( __GNUC__ ) /* GCC */
      _mutex_acquire(&print_lock);  // not needed for Arm-supplied C library that handles printf locking itself
#endif
    printf("CPU %lu: finished\n", core);
#if defined ( __GNUC__ ) /* GCC */
    _mutex_release(&print_lock);
#endif

    if (__atomic_add_fetch(&cpu_finished_count, 1, __ATOMIC_RELAXED) < cpu_active_count)
    {
      /*
       * CPUs that finish early wait for termination
       */
      while(1)
        asm("wfi");
    }
    else
    {
      /*
       * The last CPU to finish terminates the program
       */
      printf("All CPUs finished\n");
      exit(0);
    }
}

/*
 * int main(void)
 *    the application start point for the primary CPU - bring up the
 *    secondary CPUs and then call MainApp
 *
 *  Inputs
 *    <none>
 *
 *  Returns
 *    subroutine does not return
 */
int main(void)
{
    printf("\r\nPMUv3 Example, based on Armv8-A SMP Prime Number Generator Example\r\n\r\n");

    initTimerInterrupt();

    initPrimes(); // Initialize the primes just once, including print_lock

    asm("dsb OSHST"); // Ensure the side effects of initPrimes are seen before waking the CPUs

    /*
     * send SGI15 to the other (i.e. secondary) CPUs to
     * wake them up
     *
     * these should be group1 ("non-secure") interrupts
     */
    setICC_SGI1R(0, 0, 0, sgirIRMAll, 0, 15);

    /*
     * start the main application with the other CPUs
     *
     * note: MainApp() does not return
     */
    MainApp();
}
