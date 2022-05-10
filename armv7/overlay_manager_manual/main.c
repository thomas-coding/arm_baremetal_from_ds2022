/* main.c - demo of manual overlay manager
 *
 * Copyright (c) 2016-2021 Arm Limited (or its affiliates). All rights reserved.
 * Use, modification and redistribution of this file is subject to your possession of a
 * valid End User License Agreement for the Arm Product of which these examples are part of
 * and your compliance with all applicable terms and conditions of such licence agreement.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "modules.h"

/*
 * Perform cache maintenance after an overlay is loaded
 */
__attribute__((used)) void __ARM_overlay_api_loaded(void *overlay_start, void *overlay_end)
{
    /* FIX ME: Add cache maintenance code for your processor here
     * This function is called after loading an overlay into memory, and
     * before branching to the newly written memory. It should perform
     * any necessary cache invalidations or instruction memory barrier
     * operations to ensure that instruction fetches from the memory
     * will return the newly loaded code and not be polluted by its
     * previous contents.
     */
}

/*
 * This value represents the number of different `OVERLAY` regions
 * you specify in the .scat file
 */
#define NUM_OVERLAY_REGIONS 6

/*
 * For each `OVERLAY` region you specify you must accompany it with
 * a block like shown below containing:
 *
 * Load$$<OVERLAY_NAME>$$R0$$Base
 * Image$$<OVERLAY_NAME>$$R0$$Base
 * Load$$<OVERLAY_NAME>$$R0$$Length
 *
 * These symbols are then used to create the variables below
 */
extern unsigned int Load$$OVERLAY_RAM0_0$$RO$$Base;
extern unsigned int Image$$OVERLAY_RAM0_0$$RO$$Base;
extern unsigned int Load$$OVERLAY_RAM0_0$$RO$$Length;

extern unsigned int Load$$OVERLAY_RAM0_1$$RO$$Base;
extern unsigned int Image$$OVERLAY_RAM0_1$$RO$$Base;
extern unsigned int Load$$OVERLAY_RAM0_1$$RO$$Length;

extern unsigned int Load$$OVERLAY_RAM1_0$$RO$$Base;
extern unsigned int Image$$OVERLAY_RAM1_0$$RO$$Base;
extern unsigned int Load$$OVERLAY_RAM1_0$$RO$$Length;

extern unsigned int Load$$OVERLAY_RAM1_1$$RO$$Base;
extern unsigned int Image$$OVERLAY_RAM1_1$$RO$$Base;
extern unsigned int Load$$OVERLAY_RAM1_1$$RO$$Length;

extern unsigned int Load$$OVERLAY_RAM2_0$$RO$$Base;
extern unsigned int Image$$OVERLAY_RAM2_0$$RO$$Base;
extern unsigned int Load$$OVERLAY_RAM2_0$$RO$$Length;

extern unsigned int Load$$OVERLAY_RAM2_1$$RO$$Base;
extern unsigned int Image$$OVERLAY_RAM2_1$$RO$$Base;
extern unsigned int Load$$OVERLAY_RAM2_1$$RO$$Length;

/*
 * This struct defines where the overlay regions exist in memory.
 * The start and end are used by the overlay manager to determine if
 * an overlay needs be evicted.
 */
const struct linker_region_record {
    uintptr_t start, end;
} Region$$Table$$AutoOverlay[NUM_OVERLAY_REGIONS] = {
		(unsigned int) &Image$$OVERLAY_RAM0_0$$RO$$Base,
		(unsigned int) &Image$$OVERLAY_RAM0_1$$RO$$Base,
		(unsigned int) &Image$$OVERLAY_RAM1_0$$RO$$Base,
		(unsigned int) &Image$$OVERLAY_RAM1_1$$RO$$Base,
		(unsigned int) &Image$$OVERLAY_RAM2_0$$RO$$Base,
		(unsigned int) &Image$$OVERLAY_RAM2_1$$RO$$Base
};

// Defines the number of Overlay regions used
unsigned short Region$$Count$$AutoOverlay = NUM_OVERLAY_REGIONS;

/*
 * This maps the id of the overlay region to the overlay. For example, overlay id 0
 * will be mapped to the first 2 overlay regions (defined in scatter file) and overlay
 * id 1 will be mapped to the next 2 overlay regions. When defining which overlay region
 * each function belongs to (see modules.h) you will need to use the correct overlay id.
 */
unsigned short Overlay$$Map$$AutoOverlay[NUM_OVERLAY_REGIONS] = {0x0, 0x0, 0x1, 0x1, 0x2, 0x2};

/*
 * This table defines the size of each overlay region by using the length from above.
 * For each overlay region you add you will also need to update this table.
 */
unsigned int Size$$Table$$AutoOverlay[NUM_OVERLAY_REGIONS] = {
		(unsigned int) &Load$$OVERLAY_RAM0_0$$RO$$Length,
		(unsigned int) &Load$$OVERLAY_RAM0_1$$RO$$Length,
		(unsigned int) &Load$$OVERLAY_RAM1_0$$RO$$Length,
		(unsigned int) &Load$$OVERLAY_RAM1_1$$RO$$Length,
		(unsigned int) &Load$$OVERLAY_RAM2_0$$RO$$Length,
		(unsigned int) &Load$$OVERLAY_RAM2_1$$RO$$Length
};

/*
 * This table shows which overlay is currently loaded. We initialise the table to be
 * "full" as zeroing it can lead to an overlay of id 0 to be used.
 */
unsigned short CurrLoad$$Table$$AutoOverlay[NUM_OVERLAY_REGIONS] = {0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF};

/*
 * This table defines the base load address of each region so when the region is called
 * it will do a lookup in this table to find the address to populate the overlay.
 */
unsigned int LoadAddr$$Table$$AutoOverlay[NUM_OVERLAY_REGIONS] = {
		(unsigned int) &Load$$OVERLAY_RAM0_0$$RO$$Base,
		(unsigned int) &Load$$OVERLAY_RAM0_1$$RO$$Base,
		(unsigned int) &Load$$OVERLAY_RAM1_0$$RO$$Base,
		(unsigned int) &Load$$OVERLAY_RAM1_1$$RO$$Base,
		(unsigned int) &Load$$OVERLAY_RAM2_0$$RO$$Base,
		(unsigned int) &Load$$OVERLAY_RAM2_1$$RO$$Base
};


/*
 * Use $Sub and $Super to wrap the overlay-loading function so that it
 * prints out what overlay is being loaded.
 */
void $Super$$__ARM_overlay_api_load(int overlay_id, void *destination);
void $Sub$$__ARM_overlay_api_load(int overlay_id, void *destination)
{
    printf("[loading overlay %d -> %p]\n", overlay_id, destination);
    $Super$$__ARM_overlay_api_load(overlay_id, destination);
}

void indirect_chain_start() {
    printf("Starting indirect chain...\n");
    indirect_chain_1();
}

void indirect_chain_1_to_2() {
    printf("Moving from indirect chain 1 to indirect chain 2\n");
    indirect_chain_2();
}

void indirect_chain_2_to_3() {
    printf("Moving from indirect chain 1 to indirect chain 2\n");
    indirect_chain_3();
}

void fp_call_non_overlaid(demo_function_pointer fp) {
    printf("Calling from a non-overlaid function with arg 999\n");
    fp(999);
}

void fp_target_non_overlaid(int arg) {
    printf("hello and goodbye from fp_target_non_overlaid(%d)\n", arg);
}

int main(void) {
    printf("Hello from main\n");

    /* Simplest use case - call of a manually overlaid function. */
    standalone_1();
    standalone_2();
    standalone_3();

    /* Direct chain calls an overlaid function from an overlaid function. */
    direct_chain_1();

    /*
     * Starts an indirect chain. This is a series of functions which don't
     * directly call an overlaid function but do call an overlaid function
     * from a non-overlaid function
     */
    indirect_chain_start();

    /* Call each fp_call function with each fp_target. */
    fp_call_non_overlaid(fp_target_non_overlaid);
    fp_call_non_overlaid(fp_target_0);
    fp_call_non_overlaid(fp_target_1);
    fp_call_non_overlaid(fp_target_2);
    fp_call_non_overlaid(fp_target_3);
    fp_call_0(fp_target_non_overlaid);
    fp_call_0(fp_target_0);
    fp_call_0(fp_target_1);
    fp_call_0(fp_target_2);
    fp_call_0(fp_target_3);
    fp_call_1(fp_target_non_overlaid);
    fp_call_1(fp_target_0);
    fp_call_1(fp_target_1);
    fp_call_1(fp_target_2);
    fp_call_1(fp_target_3);
    fp_call_2(fp_target_non_overlaid);
    fp_call_2(fp_target_0);
    fp_call_2(fp_target_1);
    fp_call_2(fp_target_2);
    fp_call_2(fp_target_3);
    fp_call_3(fp_target_non_overlaid);
    fp_call_3(fp_target_0);
    fp_call_3(fp_target_1);
    fp_call_3(fp_target_2);
    fp_call_3(fp_target_3);

    /*
     * An interworking chain alternating between Arm and Thumb.
     * Each function is overlaid
     */
    interworking_chain_0a();

    printf("Goodbye from main\n");
    return 0;
}
