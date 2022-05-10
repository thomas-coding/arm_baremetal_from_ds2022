// ------------------------------------------------------------
// Cortex-A single-core Fireworks - Main
//
// Copyright (c) 2011-2018 Arm Limited (or its affiliates). All rights reserved.
// Use, modification and redistribution of this file is subject to your possession of a
// valid End User License Agreement for the Arm Product of which these examples are part of 
// and your compliance with all applicable terms and conditions of such licence agreement.
// ------------------------------------------------------------


#include <stdio.h>
#include <arm_compat.h> // provides (AC5) compiler intrinsics such as __enable_irq()
#include "v7.h"


// declaration of 'extern' functions
extern void enableCaches(void);  // in v7.s
extern void setup_screen(void);  // in fireworks.c
extern void uart_init(void);     // in uart.c
extern void init_timer(void);    // in timer_interrupts.c
extern void fireworks(void);     // in fireworks.c


__attribute__((noreturn)) int main(void)
{
// MMU was enabled earlier and scatterloading has now finished, so
// it is now safe to enable caches and branch prediction for each core
  enableBranchPrediction();
  enableCaches();

  uart_init();      // in uart.c

  printf("\nArmv7-A Fireworks Example, using Arm Compiler for Embedded 6\n\n");
  printf("Click on USERSW1 to stop/restart the Banner\n\n");
  setup_screen();   // in fireworks.c
  init_timer();     // in timer_interrupts.c
  __enable_irq();   // Arm Compiler intrinsic provided by arm_compat.h

  //run the fireworks!!
  fireworks();

  for(;;) {} //loop forever
}


/*
The C library startup routine __rt_entry() is called after scatterloading has been performed by __scatterload().
__rt_entry() attempts a mutex lock call-out as part of its startup.  To avoid possible lock-up in systems with
no global exclusive monitors in their memory system, rely on the local monitor provided within MPCore instead.
The local monitor becomes active when the L1 D cache is enabled.  So the L1 cache should be enabled after
scatterloading has finished to avoid cache incoherency, but before __rt_entry() executes.  This can be achieved
by using the 'wrapper' function which enables the caches then calls the original __rt_entry().
*/

extern void $Super$$__rt_entry(void);

void $Sub$$__rt_entry(void)
{
  enableBranchPrediction();
  enableCaches();

  $Super$$__rt_entry();
}


// ------------------------------------------------------------
// End of main.c
// ------------------------------------------------------------
