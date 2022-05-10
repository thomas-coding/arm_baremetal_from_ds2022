/* Bare-metal example  */

/* SVC handler interrupts */

/* Copyright (c) 2020 Arm Limited (or its affiliates). All rights reserved. */
/* Use, modification and redistribution of this file is subject to your possession of a     */
/* valid End User License Agreement for the Arm Product of which these examples are part of */
/* and your compliance with all applicable terms and conditions of such licence agreement.  */

#include <stdio.h>

#include "GICv3.h"
#include "GICv3_gicc.h"
#include "primes.h"
#include "v8_aarch64.h"


// stringify version of setICC_SGI1R() is in GICV3_gicc.h


static inline void cache_enable(void)
{
 asm volatile (
  "dsb ish \n"     // ensure all previous stores have completed before invalidating
  "ic  ialluis \n" // I cache invalidate all inner shareable to PoU (which includes secondary cores)
  "dsb ish \n"     // ensure completion on inner shareable domain   (which includes secondary cores)
  "isb \n"

  // Scatter-loading is complete, so enable the caches here, so that the C-library's mutex initialization later will work
  "mrs x1, SCTLR_EL1 \n"
  "orr x1, x1, #(1 << 2) \n"  //enable data cache
  "orr x1, x1, #(1 << 12) \n"  //enable instruction cache
  "msr SCTLR_EL1, x1 \n"
  "isb  \n"
  "ret  \n"
  :::"x1"
  );
}


unsigned int svcHandler(unsigned int svc_num, unsigned int parameter)
{
    // EL0 asks EL1 to send SGI
    if (svc_num==2)    // sending SGI syscall
        setICC_SGI1R(0, 0, 0, 1ULL<<40, 0, parameter);  //send SGI #15, parameter should be 15
    else if (svc_num==1) // cache clean and enable syscall
        cache_enable();
  
  return 0;
}


