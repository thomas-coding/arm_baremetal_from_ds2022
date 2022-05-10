/*
** Copyright (c) 2017-2022 Arm Limited (or its affiliates). All rights reserved.
** Use, modification and redistribution of this file is subject to your possession of a
** valid End User License Agreement for the Arm Product of which these examples are part of 
** and your compliance with all applicable terms and conditions of such licence agreement.
*/

/* Secure MPU initialization */

#include "mpu.h"                                     /* MPU Masks */

#include <arm_acle.h>                                /* Needed for isb/dsb intrinsics */
#include "IOTKit_ARMv8MBL.h"                         /* Device, processor and core peripherals */

void secure_MPU_init(void);


/* Linker symbols from scatter-file */
extern unsigned int Image$$SECURE_CODE$$Base;
extern unsigned int Image$$NSC_VENEERS$$Limit;
extern unsigned int Image$$SECURE_DATA$$Base;
extern unsigned int Image$$SECURE_DATA$$ZI$$Limit;


/* Setup MPU regions, then enable the MPU */
void secure_MPU_init(void)
{
    /* Configure the MAIR index 0 for Device and 1 for Cacheable */
    MPU->MAIR0 = (MAIR_DEVICE_NgNrNe << 0) | (MAIR_NORMAL_WB_NT << 8);
    /* The rest are 0 */
    MPU->MAIR1 = 0;
    
    /* Region for Secure code (Executable, Read-only) */
    MPU->RNR = 1;
    MPU->RBAR = BAR_VAL(&Image$$SECURE_CODE$$Base, NON_SHAREABLE | P_RO);
    MPU->RLAR = RLAR_VAL(&Image$$NSC_VENEERS$$Limit, 1, REGION_ENABLED);

    /* Region for Secure data (Not-Executable, Read-Write) */
    MPU->RNR = 2;
    MPU->RBAR = BAR_VAL(&Image$$SECURE_DATA$$Base, NON_SHAREABLE | P_RW | XN);
    MPU->RLAR = RLAR_VAL(&Image$$SECURE_DATA$$ZI$$Limit, 1, REGION_ENABLED);
    
    /* Region for Secure access to Non-secure vector table (Not-Executable, Read-Write) */
    MPU->RNR = 3;
    MPU->RBAR = BAR_VAL(0x00200000, NON_SHAREABLE | P_RW | XN);
    MPU->RLAR = RLAR_VAL(0x0020001F, 1, REGION_ENABLED);

    /* Enable the Secure MPU */
    MPU->CTRL |= 1;

    /* Force Memory Writes before continuing */
    __dsb(0xf);
    /* Flush and refill pipeline with updated permissions */
    __isb(0xf);
}
