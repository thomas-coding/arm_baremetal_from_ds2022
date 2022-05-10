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

#ifdef USE_SERIAL_PORT
  #include "uart.h"
#endif

extern void init_timer(void);    // in timer_interrupts.c


// compile-time control for the max number of CPUs
#define nCPUs 8

static unsigned int cpu_active_count = 0;
static unsigned int cpu_finished_count = 0;

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
    unsigned int id;

    id = GetCPUID();

    if (id >= nCPUs)
    {
      _mutex_acquire(&print_lock);
      printf("Error: More CPU cores are running than the maximum this program expects\n");
      printf("Change the value of nCPUs.  Exiting...\n");
      _mutex_release(&print_lock);
      exit(1);
    }

    __atomic_add_fetch(&cpu_active_count, 1, __ATOMIC_RELAXED);

    _mutex_acquire(&print_lock);
    printf("CPU %d: Starting calculation\n", id);
    _mutex_release(&print_lock);

    calculatePrimes(id);

    _mutex_acquire(&print_lock);
    printf("CPU %d: finished\n", id);
    _mutex_release(&print_lock);

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
__attribute__((noreturn)) int main(void)
{
#ifdef USE_SERIAL_PORT
    UartInit();
#endif

    printf("\r\nArmv8-A SMP Prime Number Generator Example, using GCC for bare-metal targets\r\n\r\n");

    initPrimes(); // Initialize the primes just once, including print_lock

    asm("dsb ISHST"); // Ensure the side effects of initPrimes are seen before waking the CPUs

    // Give secondary cores time to reach their holding pen
    for( int i=0; i <= 100000; i++) { asm("nop"); }

    /*
     * send SGI15 to the other (i.e. secondary) CPUs to
     * wake them up
     *
     * these should be group1 ("non-secure") interrupts
     */
    setICC_SGI1R(0, 0, 0, sgirIRMAll, 0, 15);

    init_timer();

    /*
     * start the main application with the other CPUs
     *
     * note: MainApp() does not return
     */
    MainApp();
}
