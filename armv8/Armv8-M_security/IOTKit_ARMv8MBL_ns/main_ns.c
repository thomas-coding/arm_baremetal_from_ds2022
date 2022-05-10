/*
** Copyright (c) 2017 Arm Limited (or its affiliates). All rights reserved.
** Use, modification and redistribution of this file is subject to your possession of a
** valid End User License Agreement for the Arm Product of which these examples are part of 
** and your compliance with all applicable terms and conditions of such licence agreement.
*/

#include <stdio.h>
#include <arm_cmse.h>
#include "IOTKit_ARMv8MBL.h"                    /* Device, processor and core peripherals */
#include "main_ns.h"

extern void nonsecure_MPU_init(void);

#if defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
__asm(" .global __ARM_use_no_argv\n");
#endif
 
void read_secure_data(unsigned int address)
{
  unsigned int value = *((unsigned int*) (address));
  printf("Secure data value at address 0x%x is %d \n\r", address, value);
}

void write_data(unsigned int address, unsigned int value)
{
  *((unsigned int*) (address)) = value;
}

/* ================= */
/* Non-secure main() */
/* ================= */

int main(void)
{
  int val1, val2;
  int x = 0;

  nonsecure_MPU_init();

  printf("\n\rHello from Non-secure World!\n\r");

  val1 = ns_callable_fn1(x);
  val2 = ns_callable_fn2(x);

  if (val1 == val2)
  {
    printf("val2 is equal to val1\n\r");
  }
  else
  {
    printf("val2 is different from val1\n\r");
  }

  printf("Attempting a Non-secure access to secure memory (will generate a hard fault)\n\r");
  read_secure_data(0x001FFFDC);

  // __set_CONTROL(CONTROL_nPRIV_Msk);
  // printf("Attempting an unprivileged write to the MPU Control register (will generate a hard fault)\n\r");
  // write_data(0xE002ED94, 0x1);

  return 0;
}
