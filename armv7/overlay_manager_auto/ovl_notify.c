/* ovl_notify.c - overlay modifier notify function for debuggers, 
 *                separate from ovl_manager.c to avoid getting optimised 
 *                away
 *
 * Copyright (c) 2016-2017 Arm Limited (or its affiliates). All rights reserved.
 * Use, modification and redistribution of this file is subject to your possession of a
 * valid End User License Agreement for the Arm Product of which these examples are part of 
 * and your compliance with all applicable terms and conditions of such licence agreement.
 */

#include <stddef.h>
#include <stdint.h>

#include "ovl_internal.h"

void __ARM_notify_overlay_loaded(int overlay) {}
