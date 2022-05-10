/* Bare-metal example  */

/* sgi interrupts */

/* Copyright (c) 2016-2018 Arm Limited (or its affiliates). All rights reserved. */
/* Use, modification and redistribution of this file is subject to your possession of a     */
/* valid End User License Agreement for the Arm Product of which these examples are part of */
/* and your compliance with all applicable terms and conditions of such licence agreement.  */

#include <stdio.h>

#include "GICv3.h"
#include "GICv3_gicc.h"



// --------------------------------------------------------

void irqHandler(void)
{
  unsigned int ID;

  ID = getICC_IAR1(); // readIntAck();

  // Check for reserved IDs
  if ((1020 <= ID) && (ID <= 1023))
  {
      //printf("irqHandler() - Reserved INTID %d\n\n", ID);
      return;
  }

   if(ID <16)
   {
	 // this is a sgi interrupt 
	 //printf ("this is a sgi %d interrupt\n", ID);
   }

  // Write the End of Interrupt register to tell the GIC
  // we've finished handling the interrupt
  setICC_EOIR1(ID); // writeAliasedEOI(ID);
}


