/* ovl_maxstk.c - default function returning maximum stack size
 *
 * Copyright (c) 2016-2017 Arm Limited (or its affiliates). All rights reserved.
 * Use, modification and redistribution of this file is subject to your possession of a
 * valid End User License Agreement for the Arm Product of which these examples are part of 
 * and your compliance with all applicable terms and conditions of such licence agreement.
 */

#include "ovl_api.h"

int __ARM_overlay_api_max_stack_size(void)
{
    return 256;
}
