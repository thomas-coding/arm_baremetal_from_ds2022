/*
 * STM Example
 *
 * Copyright (c) 2015-2017 Arm Limited (or its affiliates). All rights reserved.
 * Use, modification and redistribution of this file is subject to your possession of a
 * valid End User License Agreement for the Arm Product of which these examples are part of 
 * and your compliance with all applicable terms and conditions of such licence agreement.
 *
 */

#include <stdio.h>
#include <stdlib.h>

#include "platform.h"
#include "stm.h"

#include "cambridge.h"
#include "primes.h"

struct STM *gSTM;

int main(int argc, char **argv)
{
    /* Initialize the STM */
    struct STM stm = { 0 };
    gSTM = (struct STM *) &stm;

    stmInit(
                gSTM,
                (struct stmAPB *) PLATFORM_STM_APB_ADDRESS,
                (struct stmAXI *) PLATFORM_STM_AXI_ADDRESS
            );

    stringExample();
    calculatePrimes();

    return 0;
}
