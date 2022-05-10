// ------------------------------------------------------------
// Cortex-A MPCore - Main
//
// Copyright (c) 2011-2018 Arm Limited (or its affiliates). All rights reserved.
// Use, modification and redistribution of this file is subject to your possession of a
// valid End User License Agreement for the Arm Product of which these examples are part of 
// and your compliance with all applicable terms and conditions of such licence agreement.
// ------------------------------------------------------------


#include <stdio.h>

#include "v7.h"
#include "MP_GIC.h"
#include "MP_Mutexes.h"
#include "primes.h"


/*
This application code uses mutexes.  The C library also calls-out to a mutex lock as part of malloc().
To avoid possible lock-up in systems with no global exclusive monitors in their memory system, use the
local monitor provided within MPCore instead.  The local monitor becomes active when the L1 D cache is enabled.
So enable the L1 cache after scatterloading has finished to avoid cache incoherency,
but before any mutex calls or C functions that make use of malloc (e.g. printf) are called.
*/

// Mutex to regulate access by multiple CPUs to a resource, aligned to cache line size
mutex_t malloc_lock    __attribute__ ((aligned (32)));

// ------------------------------------------------------------


// this is executed by all CPUs
__attribute__((noreturn)) void main_app(void)
{
  unsigned int id;

// MMU was enabled earlier and scatterloading has now finished, so
// it is now safe to enable caches and branch prediction for each core
  enableBranchPrediction();
  enableCaches();

  id = getCPUID();

  lockMutex(&print_lock);
  printf("CPU %d: Starting calculation\n", id);
  unlockMutex(&print_lock);

  calculatePrimes(id);

  lockMutex(&print_lock);
  printf("CPU %d: Finished\n", id);
  unlockMutex(&print_lock);

  for(;;) {} //loop forever
}


// this main() is only executed by CPU 0
__attribute__((noreturn)) void main(void)
{
// MMU was enabled earlier and scatterloading has now finished, so
// it is now safe to enable caches and branch prediction for each core
  enableBranchPrediction();
  enableCaches();

  initMutex(&malloc_lock);

  // init_uart();

  printf("\nArmv7-A SMP Prime Number Generator Example, using GCC for bare-metal targets\n\n");

  initPrimes();             // Initialize the primes just once
  asm("dsb ISHST");         // Ensure the side effects of initPrimes are seen before waking the CPUs


  sendSGI(0x0, 0x0F, 0x01); // Wake the secondary CPUs by sending SGI (ID 0)

  main_app();               // Start the main application with the other CPUs
}


// Called by the C library to obtain a lock on the mutex.
void __malloc_lock(void)
{
  lockMutex(&malloc_lock);
}

// Called by the C library to release the lock on the mutex previously acquired
void __malloc_unlock(void)
{
  unlockMutex(&malloc_lock);
}


// ------------------------------------------------------------
// End of main.c
// ------------------------------------------------------------
