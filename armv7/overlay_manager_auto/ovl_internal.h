/* ovl_internal.h - shared definitions between the overlay manager/API files
 *
 * Copyright (c) 2016-2017 Arm Limited (or its affiliates). All rights reserved.
 * Use, modification and redistribution of this file is subject to your possession of a
 * valid End User License Agreement for the Arm Product of which these examples are part of 
 * and your compliance with all applicable terms and conditions of such licence agreement.
 */

/*
 * An entry in the overlay return stack.
 */
struct stack_entry {
    uintptr_t overlay_id;
    uintptr_t return_address;
};

/*
 * The root structure returned by __ARM_overlay_api_perthread, holding
 * the pointers to all other pieces of per-thread overlay state.
 */
struct __perthread {
    /*
     * Points to the first empty slot in the thread's overlay return
     * stack. (The stack grows upwards, so sp is incremented after a
     * push and decremented before a pop.)
     */
    struct stack_entry *sp;

    /*
     * Points to a table of the overlay regions which indicates which
     * overlay _this thread_ last requested to be loaded into each
     * region. Used to decide what overlay number to write into a
     * return stack entry.
     *
     * (Using CurrLoad$$Table$$AutoOverlay[], the process-global
     * linker-allocated table of what overlay *is* loaded in each
     * region, would have a subtle race condition, if another thread
     * pre-empts this one while it's in the prologue of
     * __ARM_overlay_entry.)
     */
    int *last_requested;
};

/*
 * Linker-provided data tables.
 */
extern const struct linker_region_record {
    uintptr_t start, end;
} Region$$Table$$AutoOverlay[];
extern const unsigned short Region$$Count$$AutoOverlay;

extern const struct linker_overlay_record {
    unsigned short region;
} Overlay$$Map$$AutoOverlay[];

extern const size_t Size$$Table$$AutoOverlay[];

extern unsigned short CurrLoad$$Table$$AutoOverlay[];

/*
 * Special value used in CurrLoad$$Table$$AutoOverlay and as a return
 * value from __ARM_overlay_which_region.
 */
#define NOT_AN_OVERLAY (-1)

/*
 * Function for the debugger to put a breakpoint on so that it can be
 * notified of overlay loads.
 */
void __ARM_notify_overlay_loaded(int overlay);
