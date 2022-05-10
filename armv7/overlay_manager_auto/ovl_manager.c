/* ovl_manager.c - main overlay manager, not user-replaceable
 *
 * Copyright (c) 2016-2017 Arm Limited (or its affiliates). All rights reserved.
 * Use, modification and redistribution of this file is subject to your possession of a
 * valid End User License Agreement for the Arm Product of which these examples are part of 
 * and your compliance with all applicable terms and conditions of such licence agreement.
 */

#include <stddef.h>
#include <stdint.h>

#include "ovl_internal.h"
#include "ovl_api.h"

/*
 * Declare some API functions as weakly referenced by this module, so
 * that the default behavior will be to do nothing if the user does
 * not implement them.
 */
__attribute((weak)) void __ARM_overlay_api_evicted
(void *startaddr, void *endaddr, int overlay_id);
__attribute((weak)) void __ARM_overlay_api_loaded
(void *startaddr, void *endaddr);

/*
 * Determine which overlay region, if any, contains a given pc value.
 * Returns a region id, or NOT_AN_OVERLAY if the pc value is not in
 * any of the regions.
 */
int __ARM_overlay_which_region(uintptr_t pc)
{
    /*
     * Binary search, maintaining the invariant that the array index
     * we want (if any) is always strictly greater than 'min' and
     * strictly less than 'max'. Hence, when min and max are adjacent
     * integers, the search has terminated without finding an answer.
     */
    int min = -1, max = Region$$Count$$AutoOverlay;
    while (max - min > 1) {
        int mid = min + ((max-min)>>1);
        const struct linker_region_record *midrec =
            Region$$Table$$AutoOverlay + mid;

        if (pc < midrec->start)
            max = mid;
        else if (pc >= midrec->end)
            min = mid;
        else
            return mid;
    }

    return NOT_AN_OVERLAY;
}

/*
 * Determine which actual overlay was last requested in the given
 * overlay region, by a call to __ARM_overlay_load, from this thread.
 * Takes a region id as input, and returns an overlay id as output.
 */
int __ARM_overlay_last_requested(int region)
{
    struct __perthread *pt = __ARM_overlay_api_perthread();
    return pt->last_requested[region];
}

/*
 * Push a new entry on this thread's overlay return stack.
 */
void __ARM_overlay_push_return_stack(struct stack_entry ent)
{
    struct __perthread *pt = __ARM_overlay_api_perthread();
    /*
     * We perform no stack bounds checking here. If the return stack
     * needed to be dynamically allocated, either by periodic resizing
     * or structured as a linked list, this would be the place to
     * change that behavior.
     */
    *pt->sp++ = ent;
}

/*
 * Pop an entry from this thread's overlay return stack and return it.
 */
struct stack_entry __ARM_overlay_pop_return_stack(void)
{
    struct __perthread *pt = __ARM_overlay_api_perthread();
    return *--pt->sp;
}

/*
 * Load an overlay, given just its id.
 */
void __ARM_overlay_load(int overlay)
{
    /*
     * Identify the target overlay region, and look up its start address.
     */
    int region = Overlay$$Map$$AutoOverlay[overlay].region;
    void *region_addr = (void *)Region$$Table$$AutoOverlay[region].start;

    /*
     * Record that this overlay was the most recently requested one in
     * this region. (Even if we then find that it's already loaded -
     * it might have been loaded by _another_ thread, in which case we
     * still need to record that _this_ thread had a use for it.)
     */
    struct __perthread *pt = __ARM_overlay_api_perthread();
    pt->last_requested[region] = overlay;

    /*
     * If the overlay isn't the one loaded in this region, load it.
     */
    if (CurrLoad$$Table$$AutoOverlay[region] != overlay) {
        /*
         * First, notify the OS that the previously loaded overlay
         * here (if any) is going away.
         */
        if (CurrLoad$$Table$$AutoOverlay[region] !=
            (unsigned short)NOT_AN_OVERLAY)
            __ARM_overlay_api_evicted
                (region_addr, (void *)Region$$Table$$AutoOverlay[region].end,
                 CurrLoad$$Table$$AutoOverlay[region]);

        /*
         * Actually load the overlay data.
         */
        __ARM_overlay_api_load(overlay, region_addr);

        /*
         * Record this as the currently loaded overlay.
         */
        CurrLoad$$Table$$AutoOverlay[region] = overlay;

        /*
         * Notify the API to do cache invalidation.
         */
        __ARM_overlay_api_loaded
            (region_addr,
             (char *)region_addr + Size$$Table$$AutoOverlay[overlay]);

        /*
         * Notify the debugger.
         */
        __ARM_notify_overlay_loaded(overlay);
    }
}

/*
 * Set up a new perthread structure.
 */

#define alignof(T) offsetof(struct {char c; T t;}, t)

struct __perthread *__ARM_overlay_perthread_new(void)
{
    struct __perthread *pt;

    /*
     * Find out how much stack to allocate.
     */
    int stacksize = __ARM_overlay_api_max_stack_size();

    /*
     * Work out the total size of data we need, including the
     * top-level __perthread structure, the last_requested table, and
     * an overlay return stack of the size we just found out.
     */
    size_t lroffset, stackoffset, totalsize;
    lroffset = sizeof(struct __perthread);
    lroffset = (lroffset + alignof(int) - 1) & (-alignof(int));
    stackoffset = lroffset + Region$$Count$$AutoOverlay * sizeof(int);
    stackoffset = ((stackoffset + alignof(struct stack_entry) - 1) &
                   (-alignof(struct stack_entry)));
    totalsize = stackoffset + stacksize * sizeof(struct stack_entry);

    /*
     * Allocate the memory.
     */
    pt = __ARM_overlay_api_alloc(totalsize);

    /*
     * Fill in the top-level fields.
     */
    pt->last_requested = (void *)((char *)pt + lroffset);
    pt->sp = (void *)((char *)pt + stackoffset);

    return pt;
}
