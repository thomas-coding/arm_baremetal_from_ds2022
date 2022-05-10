// ------------------------------------------------------------
// Cortex-A single-core Fireworks - Main
//
// Copyright (c) 2011-2018 Arm Limited (or its affiliates). All rights reserved.
// Use, modification and redistribution of this file is subject to your possession of a
// valid End User License Agreement for the Arm Product of which these examples are part of 
// and your compliance with all applicable terms and conditions of such licence agreement.
// ------------------------------------------------------------


#include <stdio.h>

#include "v7.h"


// declaration of 'extern' functions
extern void enableCaches(void);  // in v7.s
extern void setup_screen(void);  // in fireworks.c
extern void uart_init(void);     // in uart.c
extern void init_timer(void);    // in timer_interrupts.c
extern void fireworks(void);     // in fireworks.c


__attribute__((noreturn)) void main(void)
{
// MMU was enabled earlier and scatterloading has now finished, so
// it is now safe to enable caches and branch prediction for each core
  enableBranchPrediction();
  enableCaches();

  uart_init();      // in uart.c

  printf("\nArmv7-A Fireworks Example, using GCC for bare-metal targets\n\n");
  printf("Click on USERSW1 to stop/restart the Banner\n\n");
  setup_screen();   // in fireworks.c
  init_timer();     // in timer_interrupts.c
  asm("CPSIE i");   // Enable IRQ

  //run the fireworks!!
  fireworks();

  for(;;) {} //loop forever
}


// ------------------------------------------------------------
// End of main.c
// ------------------------------------------------------------
