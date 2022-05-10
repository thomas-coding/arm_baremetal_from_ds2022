/*
 * STM Example
 *
 * Copyright (c) 2015-2017 Arm Limited (or its affiliates). All rights reserved.
 * Use, modification and redistribution of this file is subject to your possession of a
 * valid End User License Agreement for the Arm Product of which these examples are part of 
 * and your compliance with all applicable terms and conditions of such licence agreement.
 *
 */

#include "stm.h"

void stringExample(void)
{
    stmSendString(gSTM, 0, "Cambridge\nCambridge\nCambridge\nCambridge");
}
