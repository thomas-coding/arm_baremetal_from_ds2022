/*
 * Armv8-A - main for AArch32
 *
 * Copyright (c) 2011-2021 Arm Limited (or its affiliates). All rights reserved.
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
#include "aarch32_pmu.h"

// compile-time control for the max number of CPUs
#define nCPUs 8

static unsigned int cpu_active_count = 0;
static unsigned int cpu_finished_count = 0;


/* Stacks for secondary CPUs, since it runs in EL0, EL0 stack is required
 * For primary CPU, the stack is set up by C runtime library with stack information
 * in scatter loading file. The stacks must be set up at begin of MainApp.
 */
#define SECONDARY_STACK_SIZE (2*1024)
__attribute__((aligned(16)))  char secondary_cpu_stack[nCPUs-1][SECONDARY_STACK_SIZE];




/* EL1 startup code store CPUID in User read-only Thread and Process ID Register */
static inline unsigned int EL0GetCPUID(void)
{
	unsigned int cpuid;
	asm volatile("MRC p15, 0, %0, c13, c0, 3\n; isb" :"+r" (cpuid)::);
	return cpuid;
}

/* EL0 cannot access ICC_SGI1R_EL1 register, use syscall call #2
 * to call EL1 SVC service to send SGI #15
 */
static void EL0SendSGI(unsigned int sgi_num)
{
 
	asm volatile ("mov r0, %0 \n"
			      "svc #2 \n"   //system call
			      :"+r"(sgi_num)::"cc");
		
}

#ifdef USE_SERIAL_PORT
  #include "uart.h"
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
__attribute__((noreturn)) void RunApp(void)
{
    unsigned int id;

    id = EL0GetCPUID();

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

    exit(0);
}


__attribute__((noinline, noreturn)) void MainApp(void)
{
    //set up stack for secondary CPUs, must be written with inline assembly
	//to avoid using stack in this function

	unsigned int cpuid;

    asm volatile(
    	 ".global RunApp \n"
    	 ".global secondary_cpu_stack \n"
    	 "MRC p15, 0, %0, c13, c0, 3 \n"   //get CPUID from TPIDRRO_EL0
    	 "cmp r0, #0 \n"
    	 "beq 1f \n"   //nothing to do for primary CPU
    	 //set up stack for secondary CPU
    	 "ldr r1, =secondary_cpu_stack \n"
    	 "mov r2, %1 \n"
    	 "mla sp, %0, r2, r1 \n"  //sp=&secondary_cpu_stack[cpuid-1]+SECONDARY_STACK_SIZE
    	 "1: \n"
    	 "b RunApp \n"

    	 :"+r"(cpuid)
		 :"I"(SECONDARY_STACK_SIZE)
		 :"r1","r2","cc");

    while(1) {} // should never reach here, only to avoid return warning
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

    /*
     * send SGI15 to the other (i.e. secondary) CPUs to
     * wake them up
     *
     * these should be group1 ("non-secure") interrupts
     */
     EL0SendSGI(15);

    /*
     * start the main application with the other CPUs
     *
     * note: MainApp() does not return
     */
    MainApp();
}
