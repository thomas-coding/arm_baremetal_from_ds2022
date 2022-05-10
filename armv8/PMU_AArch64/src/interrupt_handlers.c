/* Bare-metal example for Armv8-A */

/* Timer and interrupts */

/* Copyright (c) 2016 Arm Limited (or its affiliates). All rights reserved. */
/* Use, modification and redistribution of this file is subject to your possession of a     */
/* valid End User License Agreement for the Arm Product of which these examples are part of */
/* and your compliance with all applicable terms and conditions of such licence agreement.  */

#include <stdio.h>

#include "GICv3.h"
#include "GICv3_gicc.h"
#include "pmu_interrupt.h"
#include "sp804_timer.h"
#include "v8_aarch64.h"

#include "timer_interrupt.h"

// --------------------------------------------------------

void el1IrqHandler(void)
{
  unsigned int ID;

  ID = getICC_IAR1();

  // Check for reserved IDs
  if ((1020 <= ID) && (ID <= 1023))
  {
      printf("Reserved INTID %d\n\n", ID);
      return;
  }

  switch(ID)
  {
    case 34:
      // Dual-Timer 0 (SP804)
      timerInterruptHandler();
      break;

    case 23:
      // PMU counter overflow
      pmuOverflowHandler();
      break;

    case 14:
      // Sample the PMU
      pmuInterruptHandler();
      break;

    default:
      // Unexpected ID value
      printf("Unexpected INTID %d\n\n", ID);
      break;
  }


  // Write the End of Interrupt register to tell the GIC
  // we've finished handling the interrupt
  setICC_EOIR1(ID);
}
