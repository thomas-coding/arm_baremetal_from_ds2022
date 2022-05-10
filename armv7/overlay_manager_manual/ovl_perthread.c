/* ovl_perthread.c - overlay perthread function for the single-threaded case
 *
 * Copyright (c) 2016-2017 Arm Limited (or its affiliates). All rights reserved.
 * Use, modification and redistribution of this file is subject to your possession of a
 * valid End User License Agreement for the Arm Product of which these examples are part of 
 * and your compliance with all applicable terms and conditions of such licence agreement.
 */

#include <stdint.h>
#include <stddef.h>

#include "ovl_internal.h"
#include "ovl_api.h"

static struct __perthread *single_perthread_structure;

void *__ARM_overlay_api_perthread(void)
{
    /*
     * Default single-threaded implementation: just return the same
     * pointer variable always.
     */
    return single_perthread_structure;
}

int $Super$$main(int argc, char **argv);
int $Sub$$main(int argc, char **argv)
{
    /*
     * Wrap main() so that we set up the above pointer variable before
     * beginning the main program.
     */
    single_perthread_structure = __ARM_overlay_perthread_new();
    return $Super$$main(argc, argv);
}
