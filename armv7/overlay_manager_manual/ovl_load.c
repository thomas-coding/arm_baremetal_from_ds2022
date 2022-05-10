/* ovl_load.c - default overlay load function that works by memcpy
 *
 * Copyright (c) 2016-2017 Arm Limited (or its affiliates). All rights reserved.
 * Use, modification and redistribution of this file is subject to your possession of a
 * valid End User License Agreement for the Arm Product of which these examples are part of 
 * and your compliance with all applicable terms and conditions of such licence agreement.
 */

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "ovl_api.h"
#include "ovl_internal.h"

extern void *LoadAddr$$Table$$AutoOverlay[];

void __ARM_overlay_api_load(int overlay, void *destination)
{
    memcpy(destination, LoadAddr$$Table$$AutoOverlay[overlay],
           Size$$Table$$AutoOverlay[overlay]);
}
