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
#include "MP_SCU.h" // for getNumCPUs


/*
This application code uses mutexes.  The C library also calls-out to a mutex lock as part of malloc().
To avoid possible lock-up in systems with no global exclusive monitors in their memory system, use the
local monitor provided within MPCore instead.  The local monitor becomes active when the L1 D cache is enabled.
So enable the L1 cache after scatterloading has finished to avoid cache incoherency,
but before any mutex calls or C functions that make use of malloc (e.g. printf) are called.
*/

// Mutex to regulate access by multiple CPUs to a resource, aligned to cache line size
mutex_t malloc_lock    __attribute__ ((aligned (32)));

// declaration of 'extern' functions
extern void enableCaches(void);  // in v7.s
extern void setup_screen(void);  // in fireworks.c
extern void uart_init(void);     // in uart.c
extern void init_timer(void);    // in timer_interrupts.c
extern void fireworks(int id);   // in fireworks.c

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

  if (getNumCPUs() > 1)
  {
    if (id == 1)
    {
      printf("Running Banner on CORE1\n");
      printf("Other secondary cores sleep\n");
    }
    if (id > 1 )
    {
      while (1) { __asm("DSB"); __asm("WFI"); }
    }
  }

  //run the fireworks!!
  fireworks(id);

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

  uart_init();      // in uart.c

  printf("\nArmv7-A Fireworks Example, using GCC for bare-metal targets\n\n");
  printf("Click on USERSW1 to stop/restart the Banner\n\n");
  setup_screen();   // in fireworks.c
  init_timer();     // in timer_interrupts.c
  printf("Running Fireworks on CORE0\n");
  if (getNumCPUs() == 1)
    printf("Running Banner on CORE0\n");
  if (getNumCPUs() > 1)
    sendSGI(0x0, 0x0F, 0x01); // Wake the secondary CPUs by sending SGI (ID 0)

  asm("CPSIE i");   // Enable IRQ

  main_app();       // Start the main application with the other CPUs
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
