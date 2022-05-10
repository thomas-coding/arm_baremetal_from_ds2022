/* ovl_alloc.c - implementation of overlay alloc for the single-threaded case
 *
 * Copyright (c) 2016-2017 Arm Limited (or its affiliates). All rights reserved.
 * Use, modification and redistribution of this file is subject to your possession of a
 * valid End User License Agreement for the Arm Product of which these examples are part of 
 * and your compliance with all applicable terms and conditions of such licence agreement.
 */

#include <stdlib.h>

#include "ovl_api.h"

void *__ARM_overlay_api_alloc(size_t size)
{
    /*
     * Default implementation if the user doesn't provide a
     * replacement: just call malloc, and make no effort to free the
     * memory.
     */
    return malloc(size);
}

