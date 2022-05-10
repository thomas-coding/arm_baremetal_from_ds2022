/*
==================================================================
 Copyright (c) 2009-2017 Arm Limited (or its affiliates). All rights reserved.
 Use, modification and redistribution of this file is subject to your possession of a
 valid End User License Agreement for the Arm Product of which these examples are part of 
 and your compliance with all applicable terms and conditions of such licence agreement.

 Simple Secure World Application
==================================================================
*/

#include <stdio.h>
#include "bp147_tzpc.h"

extern void enableBranchPrediction(void);
extern void enableCaches(void);
extern void monitorInit(void);


int main(void)
{
  unsigned int i, tmp;

// MMU was enabled earlier and scatterloading has now finished, so
// it is now safe to enable caches and branch prediction
  enableBranchPrediction();
  enableCaches();

  // Configure TZPC
  tmp = BP147_TZPC_BIT_0 | BP147_TZPC_BIT_1 |
        BP147_TZPC_BIT_2 | BP147_TZPC_BIT_3 |
        BP147_TZPC_BIT_4 | BP147_TZPC_BIT_5 |
        BP147_TZPC_BIT_6 | BP147_TZPC_BIT_7;
  setDecodeRegionNS(0, tmp);
  setDecodeRegionNS(1, tmp);
  setDecodeRegionNS(2, tmp);

  // Install monitor
  monitorInit();

  for (i = 0; i < 10; i++)
  {
    printf("Hello from Secure world\n");
    __asm("SMC 0");
  }

  return 0;
}
