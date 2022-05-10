/*
==================================================================
 Copyright (c) 2009-2017 Arm Limited (or its affiliates). All rights reserved.
 Use, modification and redistribution of this file is subject to your possession of a
 valid End User License Agreement for the Arm Product of which these examples are part of 
 and your compliance with all applicable terms and conditions of such licence agreement.

 Simple Normal World Application
==================================================================
*/

#include <stdio.h>

extern void enableBranchPrediction(void);
extern void enableCaches(void);


int main(void)
{
  unsigned int i;

// MMU was enabled earlier and scatterloading has now finished, so
// it is now safe to enable caches and branch prediction for each core
  enableBranchPrediction();
  enableCaches();

  for (i = 0; i < 10; i++)
  {
    printf("Hello from Normal world\n");
    __asm("SMC 0");
  } 

  return 0;
}
