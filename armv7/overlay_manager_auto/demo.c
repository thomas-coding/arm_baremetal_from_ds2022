/* demo.c - demo of overlay manager
 *
 * Copyright (c) 2016-2021 Arm Limited (or its affiliates). All rights reserved.
 * Use, modification and redistribution of this file is subject to your possession of a
 * valid End User License Agreement for the Arm Product of which these examples are part of 
 * and your compliance with all applicable terms and conditions of such licence agreement.
 */

#include <stdio.h>
#include <stdlib.h>

#include "ovl_api.h"

#include "demo.h"

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
 * Use $Sub and $Super to wrap the overlay-loading function so that it
 * prints out what overlay is being loaded.
 */
void $Super$$__ARM_overlay_api_load(int overlay_id, void *destination);
void $Sub$$__ARM_overlay_api_load(int overlay_id, void *destination)
{
    printf("[loading overlay %d -> %p]\n", overlay_id, destination);
    $Super$$__ARM_overlay_api_load(overlay_id, destination);
}

/*
 * Initial simple test: a stand-alone function in each overlay, called
 * in turn from main().
 */
void standalone_0(void)
{
    printf("hello and goodbye from standalone_0\n");
}

void standalone_1(void)
{
    printf("hello and goodbye from standalone_1\n");
}

void standalone_2(void)
{
    printf("hello and goodbye from standalone_2\n");
}

void standalone_3(void)
{
    printf("hello and goodbye from standalone_3\n");
}

/*
 * Demonstrate calling directly between overlays. Since this chain of
 * function calls goes through one more overlay than we have regions,
 * somewhere in the chain an overlay will have to be evicted, and
 * reloaded again when the evicted function is returned to.
 */
void direct_chain_0(void)
{
    printf("hello from direct_chain_0\n");
    direct_chain_1();
    printf("goodbye from direct_chain_0\n");
}

void direct_chain_1(void)
{
    printf("hello from direct_chain_1\n");
    direct_chain_2();
    printf("goodbye from direct_chain_1\n");
}

void direct_chain_2(void)
{
    printf("hello from direct_chain_2\n");
    direct_chain_3();
    printf("goodbye from direct_chain_2\n");
}

void direct_chain_3(void)
{
    printf("hello and goodbye from direct_chain_3\n");
}

/*
 * A second direct chain of function calls. Between this chain and the
 * previous one, we include a call between every possible pair of our
 * 4 test overlays, meaning that _somewhere_ in this demo we must have
 * called directly between two overlays that actually conflict in
 * memory (occupy the same region).
 */
void direct_chain_0b(void)
{
    printf("hello from direct_chain_0b\n");
    direct_chain_1b();
    printf("goodbye from direct_chain_0b\n");
}

void direct_chain_1b(void)
{
    printf("hello from direct_chain_1b\n");
    direct_chain_2b();
    printf("goodbye from direct_chain_1b\n");
}

void direct_chain_2b(void)
{
    printf("hello from direct_chain_2b\n");
    direct_chain_3b();
    printf("goodbye from direct_chain_2b\n");
}

void direct_chain_3b(void)
{
    printf("hello and goodbye from direct_chain_3b\n");
}

/*
 * Demonstrate calling between overlays with a non-overlaid function
 * in between each pair. Like the direct chain above, this chain of
 * calls will also have to evict and reload an overlay, but this time
 * it will have to do it on the return from a non-overlaid function,
 * which means that the call _to_ that non-overlaid function had to go
 * through a call veneer that inserted a return thunk.
 */

void indirect_chain_0(void)
{
    printf("hello from indirect_chain_0\n");
    non_overlaid_wrapper_0_to_1();
    printf("goodbye from indirect_chain_0\n");
}

void non_overlaid_wrapper_0_to_1(void)
{
    printf("hello from non_overlaid_wrapper_0_to_1\n");
    indirect_chain_1();
    printf("goodbye from non_overlaid_wrapper_0_to_1\n");
}

void indirect_chain_1(void)
{
    printf("hello from indirect_chain_1\n");
    non_overlaid_wrapper_1_to_2();
    printf("goodbye from indirect_chain_1\n");
}

void non_overlaid_wrapper_1_to_2(void)
{
    printf("hello from non_overlaid_wrapper_1_to_2\n");
    indirect_chain_2();
    printf("goodbye from non_overlaid_wrapper_1_to_2\n");
}

void indirect_chain_2(void)
{
    printf("hello from indirect_chain_2\n");
    non_overlaid_wrapper_2_to_3();
    printf("goodbye from indirect_chain_2\n");
}

void non_overlaid_wrapper_2_to_3(void)
{
    printf("hello from non_overlaid_wrapper_2_to_3\n");
    indirect_chain_3();
    printf("goodbye from non_overlaid_wrapper_2_to_3\n");
}

void indirect_chain_3(void)
{
    printf("hello and goodbye from indirect_chain_3\n");
}

/*
 * A set of functions in each overlay that will be address-taken to
 * create function pointers, and a set of functions in each overlay
 * that will call them.
 */
void fp_target_non_overlaid(int arg)
{
    printf("hello and goodbye from fp_target_non_overlaid(%d)\n", arg);
}

void fp_target_0(int arg)
{
    printf("hello and goodbye from fp_target_0(%d)\n", arg);
}

void fp_target_1(int arg)
{
    printf("hello and goodbye from fp_target_1(%d)\n", arg);
}

void fp_target_2(int arg)
{
    printf("hello and goodbye from fp_target_2(%d)\n", arg);
}

void fp_target_3(int arg)
{
    printf("hello and goodbye from fp_target_3(%d)\n", arg);
}

void fp_call_non_overlaid(demo_function_pointer fp)
{
    printf("hello from fp_call_non_overlaid\n");
    fp(999);
    printf("goodbye from fp_call_non_overlaid\n");
}

void fp_call_0(demo_function_pointer fp)
{
    printf("hello from fp_call_0\n");
    fp(1000);
    printf("goodbye from fp_call_0\n");
}

void fp_call_1(demo_function_pointer fp)
{
    printf("hello from fp_call_1\n");
    fp(1001);
    printf("goodbye from fp_call_1\n");
}

void fp_call_2(demo_function_pointer fp)
{
    printf("hello from fp_call_2\n");
    fp(1002);
    printf("goodbye from fp_call_2\n");
}

void fp_call_3(demo_function_pointer fp)
{
    printf("hello from fp_call_3\n");
    fp(1003);
    printf("goodbye from fp_call_3\n");
}

/*
 * A set of functions that each return a pointer to a function in its
 * own overlay, demonstrating that even in that situation the linker
 * substitutes a veneer for the function address, and doesn't optimise
 * the veneer away on the basis that the reference is intra-overlay.
 */
demo_function_pointer fp_return_non_overlaid(void)
{
    return fp_target_non_overlaid;
}

demo_function_pointer fp_return_0(void)
{
    return fp_target_0;
}

demo_function_pointer fp_return_1(void)
{
    return fp_target_1;
}

demo_function_pointer fp_return_2(void)
{
    return fp_target_2;
}

demo_function_pointer fp_return_3(void)
{
    return fp_target_3;
}

/*
 * A chain of calls that alternate between Arm state and Thumb state
 * functions, demonstrating that Arm/Thumb interworking also works
 * correctly.
 *
 * There are two chains of calls here, each of which includes a
 * transition between every pair of overlays. Both chains alternate
 * Arm and Thumb, so by starting each chain once, we guarantee to have
 * tested every possibility.
 */
void interworking_chain_0a(void)
{
    printf("hello from interworking_chain_0a\n");
    interworking_chain_1t();
    printf("goodbye from interworking_chain_0a\n");
}

void interworking_chain_1a(void)
{
    printf("hello from interworking_chain_1a\n");
    interworking_chain_2t();
    printf("goodbye from interworking_chain_1a\n");
}

void interworking_chain_2a(void)
{
    printf("hello from interworking_chain_2a\n");
    interworking_chain_3t();
    printf("goodbye from interworking_chain_2a\n");
}

void interworking_chain_3a(void)
{
    printf("hello from interworking_chain_3a\n");
    interworking_chain_4t();
    printf("goodbye from interworking_chain_3a\n");
}

void interworking_chain_4a(void)
{
    printf("hello from interworking_chain_4a\n");
    interworking_chain_5t();
    printf("goodbye from interworking_chain_4a\n");
}

void interworking_chain_5a(void)
{
    printf("hello from interworking_chain_5a\n");
    interworking_chain_6t();
    printf("goodbye from interworking_chain_5a\n");
}

void interworking_chain_6a(void)
{
    printf("hello from interworking_chain_6a\n");
    interworking_chain_7t();
    printf("goodbye from interworking_chain_6a\n");
}

void interworking_chain_7a(void)
{
    printf("hello from interworking_chain_7a\n");
    interworking_chain_8t();
    printf("goodbye from interworking_chain_7a\n");
}

void interworking_chain_8a(void)
{
    printf("hello from interworking_chain_8a\n");
    interworking_chain_9t();
    printf("goodbye from interworking_chain_8a\n");
}

void interworking_chain_9a(void)
{
    printf("hello from interworking_chain_9a\n");
    interworking_chain_10t();
    printf("goodbye from interworking_chain_9a\n");
}

void interworking_chain_10a(void)
{
    printf("hello from interworking_chain_10a\n");
    interworking_chain_11t();
    printf("goodbye from interworking_chain_10a\n");
}

void interworking_chain_11a(void)
{
    printf("hello from interworking_chain_11a\n");
    interworking_chain_12t();
    printf("goodbye from interworking_chain_11a\n");
}

void interworking_chain_12a(void)
{
    printf("hello and goodbye from interworking_chain_12a\n");
}

/*
 * Tests of functions with long parameter lists, meaning that some
 * parameters are passed on the stack. This demonstrates that the
 * overlay veneer mechanism does not interfere with the stack.
 */
void many_params_0(int a, int b, int c, int d, int e, int f, int g, int h, int i, int j, int k)
{
    if (a != 'a' || b != 'b' || c != 'c' || d != 'd' || e != 'e' || f != 'f' ||
        g != 'g' || h != 'h' || i != 'i' || j != 'j' || k != 'k') {
        printf("FAILURE!");
        abort();
    } else {
        printf("hello and goodbye from many_params_0, everything OK\n");
    }
}

void many_params_1(int a, int b, int c, int d, int e, int f, int g, int h, int i, int j, int k)
{
    if (a != 'a' || b != 'b' || c != 'c' || d != 'd' || e != 'e' || f != 'f' ||
        g != 'g' || h != 'h' || i != 'i' || j != 'j' || k != 'k') {
        printf("FAILURE!");
        abort();
    } else {
        printf("hello and goodbye from many_params_1, everything OK\n");
    }
}

void many_params_2(int a, int b, int c, int d, int e, int f, int g, int h, int i, int j, int k)
{
    if (a != 'a' || b != 'b' || c != 'c' || d != 'd' || e != 'e' || f != 'f' ||
        g != 'g' || h != 'h' || i != 'i' || j != 'j' || k != 'k') {
        printf("FAILURE!");
        abort();
    } else {
        printf("hello and goodbye from many_params_2, everything OK\n");
    }
}

void many_params_3(int a, int b, int c, int d, int e, int f, int g, int h, int i, int j, int k)
{
    if (a != 'a' || b != 'b' || c != 'c' || d != 'd' || e != 'e' || f != 'f' ||
        g != 'g' || h != 'h' || i != 'i' || j != 'j' || k != 'k') {
        printf("FAILURE!");
        abort();
    } else {
        printf("hello and goodbye from many_params_3, everything OK\n");
    }
}


/*
 * Main program that calls all of the above tests in turn.
 */

int main(void)
{
    printf("hello from main\n");

    /* Call each standalone function. */
    standalone_0();
    standalone_1();
    standalone_2();
    standalone_3();

    /* Start each of the two direct chains of calls. */
    direct_chain_0();
    direct_chain_0b();

    /* Start the indirect call chain. */
    indirect_chain_0();

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

    /* Call each fp_call function with the return value of each fp_return. */
    fp_call_non_overlaid(fp_return_non_overlaid());
    fp_call_non_overlaid(fp_return_0());
    fp_call_non_overlaid(fp_return_1());
    fp_call_non_overlaid(fp_return_2());
    fp_call_non_overlaid(fp_return_3());
    fp_call_0(fp_return_non_overlaid());
    fp_call_0(fp_return_0());
    fp_call_0(fp_return_1());
    fp_call_0(fp_return_2());
    fp_call_0(fp_return_3());
    fp_call_1(fp_return_non_overlaid());
    fp_call_1(fp_return_0());
    fp_call_1(fp_return_1());
    fp_call_1(fp_return_2());
    fp_call_1(fp_return_3());
    fp_call_2(fp_return_non_overlaid());
    fp_call_2(fp_return_0());
    fp_call_2(fp_return_1());
    fp_call_2(fp_return_2());
    fp_call_2(fp_return_3());
    fp_call_3(fp_return_non_overlaid());
    fp_call_3(fp_return_0());
    fp_call_3(fp_return_1());
    fp_call_3(fp_return_2());
    fp_call_3(fp_return_3());

    /* Start the two interworking call chains. */
    interworking_chain_0a();
    interworking_chain_0t();

    /* Test parameter passing. */
    many_params_0('a','b','c','d','e','f','g','h','i','j','k');
    many_params_1('a','b','c','d','e','f','g','h','i','j','k');
    many_params_2('a','b','c','d','e','f','g','h','i','j','k');
    many_params_3('a','b','c','d','e','f','g','h','i','j','k');

    printf("goodbye from main\n");
    return 0;
}
