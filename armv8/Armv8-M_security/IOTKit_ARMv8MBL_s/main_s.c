/*
** Copyright (c) 2017 Arm Limited (or its affiliates). All rights reserved.
** Use, modification and redistribution of this file is subject to your possession of a
** valid End User License Agreement for the Arm Product of which these examples are part of 
** and your compliance with all applicable terms and conditions of such licence agreement.
*/

#include <stdio.h>
#include <arm_cmse.h>
#include "IOTKit_ARMv8MBL.h"                         /* Device, processor and core peripherals */
#include "main_s.h"

/* Start address of non-secure application */
#define NONSECURE_START (0x00200000u)

extern void secure_MPU_init(void);
extern int stdout_init(void);

/* Secure data is intended to be used in the Secure world only */
const int secure_data __attribute__((section(".ARM__at_0x1FFFDC"))) = (int)0xACCE55ED;

#if defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
__asm(" .global __ARM_use_no_argv\n");
#endif
 

/* ======================================= */
/*          Secure-Only Functions          */
/* ======================================= */

int secure_fn1(int x)
{
  printf("Hello from Secure Function 1!\n\r");
  return x;
}

/* NOTE: Non-secure image must already be present in memory before running this function */

void nonsecure_init(void)
{
  /* SCB_NS.VTOR points to the Non-secure vector table base address */
  SCB_NS->VTOR=NONSECURE_START;

  /* 1st entry in the vector table is the Non-secure Main Stack Pointer */
  __TZ_set_MSP_NS(*((uint32_t *)NONSECURE_START));      /* Set up MSP in Non-secure code */

  /* 2nd entry contains the address of the Non-secure Reset_Handler */
  nsfunc *ns_reset = ((nsfunc*)(*(((uint32_t *)NONSECURE_START)+1)));

  ns_reset();       /* Call the Non-secure Reset_Handler */
}

/* ====================================== */
/*     Non-secure callable functions      */
/* ====================================== */

int __attribute__((cmse_nonsecure_entry)) ns_callable_fn1(int x)
{
  printf("Hello from Non-secure Callable Function 1!\n\r");

  return secure_fn1(x);
}

int __attribute__((cmse_nonsecure_entry)) ns_callable_fn2(int x)
{
  /* cmse_nonsecure_caller is an CMSE intrinsic that returns the security state of the caller */
  if (cmse_nonsecure_caller())
  {
    printf("Non-secure Callable Function 2 called from Non-secure World!\n\r");
    return ns_callable_fn1(x);
  }
  else
  {
    printf("Non-secure Callable Function 2 called from Secure World!\n\r");
    return secure_fn1(x);
  }
}

/* ============= */
/* Secure main() */
/* ============= */

int main(void)
{
  secure_MPU_init();

  stdout_init();

  printf("Hello from Secure World!\n\r");

  secure_fn1(1);          /* Secure function call */

  ns_callable_fn2(2);     /* Secure function call */

  nonsecure_init();       /* Initialize and switch to the Non-secure world */
  
  return 0;
}
