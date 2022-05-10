/*
 * Armv8-A - main
 *
 * Copyright (c) 2011-2021 Arm Limited (or its affiliates). All rights reserved.
 * Use, modification and redistribution of this file is subject to your possession of a
 * valid End User License Agreement for the Arm Product of which these examples are part of 
 * and your compliance with all applicable terms and conditions of such licence agreement.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <arm_acle.h>

#include "v8_aarch64.h"
#include "MP_Mutexes.h"
#include "GICv3.h"
#include "GICv3_gicc.h"

#ifdef USE_SERIAL_PORT
  #include "uart.h"
#endif

extern void setup_screen(void);  // in fireworks.c
extern void init_timer(void);    // in timer_interrupts.c
extern void fireworks(int id);   // in fireworks.c


// compile-time control for the max number of CPUs
#define nCPUs 8

static unsigned int cpu_active_count = 0;
static unsigned int cpu_finished_count = 0;

// Mutex to regulate access by multiple CPUs to a resource, aligned to cache line size
// mutex print_lock __attribute__ ((aligned (64)));  // not needed for Arm-supplied C library that handles printf locking itself

/*
 * per-thread space for use by the Arm C libraries
 *
 * 24 is a magic number from the Arm C library documentation (the
 * docs actually say 96 (AArch32) or 192 (AArch64) bytes, but
 * the requirement is 24 pointers-worth of aligned space)
 */
static void *user_perthread_libspace[nCPUs][24];

// Called by the C library to get the address of the per-thread libspace
// Return a separate space for each CPU
void *__user_perthread_libspace(void)
{
    return user_perthread_libspace[GetCPUID()];
}


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
//      _mutex_acquire(&print_lock);  // not needed for Arm-supplied C library that handles printf locking itself
      printf("Error: More CPU cores are running than the maximum this program expects\n");
      printf("Change the value of nCPUs.  Exiting...\n");
//      _mutex_release(&print_lock);
      exit(1);
    }

    __atomic_add_fetch(&cpu_active_count, 1, __ATOMIC_RELAXED);

#ifdef SMP
    if (id == 1)
    {
//      _mutex_acquire(&print_lock);  // not needed for Arm-supplied C library that handles printf locking itself
      printf("Running Banner on CORE1\n");
      printf("Other secondary cores sleep\n");
//      _mutex_release(&print_lock);
    }

    if (id > 1)
#else
    if (id > 0)
#endif
    {
      while (1) { __wfi(); }
    }

    fireworks(id);

//    _mutex_acquire(&print_lock);  // not needed for Arm-supplied C library that handles printf locking itself
    printf("CPU %d: finished\n", id);
//    _mutex_release(&print_lock);

    if (__atomic_add_fetch(&cpu_finished_count, 1, __ATOMIC_RELAXED) < cpu_active_count)
    {
      /*
       * CPUs that finish early wait for termination
       */
      while(1)
        __wfi();
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

//    _mutex_initialize(&print_lock);  // not needed for Arm-supplied C library that handles printf locking itself
//    _mutex_acquire(&print_lock);
    printf("\nCortex-X2 SMP Fireworks Example, using Arm Compiler for Embedded 6\n\n");
    printf("Click on USERSW1 to stop/restart the Banner\n\n");
    printf("Running Fireworks on CORE0\n");
#ifndef SMP
    printf("Running Banner on CORE0\n");
#endif
//    _mutex_release(&print_lock);

    setup_screen();    // in fireworks.c

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
