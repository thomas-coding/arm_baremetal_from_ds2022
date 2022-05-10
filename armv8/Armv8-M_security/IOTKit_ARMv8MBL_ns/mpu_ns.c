/*
** Copyright (c) 2017 Arm Limited (or its affiliates). All rights reserved.
** Use, modification and redistribution of this file is subject to your possession of a
** valid End User License Agreement for the Arm Product of which these examples are part of 
** and your compliance with all applicable terms and conditions of such licence agreement.
*/

/* Non-secure MPU initialization */

#include "mpu.h"                                     /* MPU Masks */

#include <arm_acle.h>                                /* Needed for isb/dsb intrinsics */
#include "IOTKit_ARMv8MBL.h"                         /* Device, processor and core peripherals */

void nonsecure_MPU_init(void);


/* Linker symbols from scatter-file */
extern unsigned int Image$$NONSECURE_CODE$$Base;
extern unsigned int Image$$NONSECURE_CODE$$Limit;
extern unsigned int Image$$NONSECURE_DATA$$Base;
extern unsigned int Image$$NONSECURE_DATA$$ZI$$Limit;


/* Setup MPU regions, then enable the MPU */
void nonsecure_MPU_init(void)
{
    /* Configure the MAIR index 0 for Device and 1 for Cacheable */
    MPU->MAIR0 = (MAIR_DEVICE_NgNrNe << 0) | (MAIR_NORMAL_WB_NT << 8);
    /* The rest are 0 */
    MPU->MAIR1 = 0;
    
    /* Region for Non-secure code (Executable, Read-only) */
    MPU->RNR = 1;
    MPU->RBAR = BAR_VAL(&Image$$NONSECURE_CODE$$Base, NON_SHAREABLE | P_RO);
    MPU->RLAR = RLAR_VAL(&Image$$NONSECURE_CODE$$Limit, 1, REGION_ENABLED);

    /* Region for Non-secure data (Not-Executable, Read-Write) */
    MPU->RNR = 2;
    MPU->RBAR = BAR_VAL(&Image$$NONSECURE_DATA$$Base, NON_SHAREABLE | P_RW | XN);
    MPU->RLAR = RLAR_VAL(&Image$$NONSECURE_DATA$$ZI$$Limit, 1, REGION_ENABLED);
    
    /* Enable the Non-secure MPU */
    MPU->CTRL |= 1;

    /* Force Memory Writes before continuing */
    __dsb(0xf);
    /* Flush and refill pipeline with updated permissions */
    __isb(0xf);
}
