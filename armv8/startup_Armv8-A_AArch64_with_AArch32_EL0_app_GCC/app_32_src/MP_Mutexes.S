//
// Armv8-A AArch32 - Basic Mutex Example
//
// Copyright (c) 2012-2017 Arm Limited (or its affiliates). All rights reserved.
// Use, modification and redistribution of this file is subject to your possession of a
// valid End User License Agreement for the Arm Product of which these examples are part of 
// and your compliance with all applicable terms and conditions of such licence agreement.
//


    .text
    .cfi_sections .debug_frame  // put stack frame info into .debug_frame instead of .eh_frame


    .global _mutex_initialize
    .global _mutex_acquire
    .global _mutex_release

//
// These routines implement the mutex management functions required for running
// the Arm C library in a multi-threaded environment.
//
// They use a value of 0 to represent an unlocked mutex, and 1 for a locked mutex
//
// **********************************************************************
//

    .type _mutex_initialize, "function"
    .cfi_startproc
_mutex_initialize:

    //
    // mark the mutex as unlocked
    //
    mov     r1, #0
    str     r1, [r0]

    //
    // we are running multi-threaded, so set a non-zero return
    // value (function prototype says use 1)
    //
    mov     r0, #1
    bx lr
    .cfi_endproc


    .type _mutex_acquire, "function"
    .cfi_startproc
_mutex_acquire:

    //
    // send ourselves an event, so we don't stick on the wfe at the
    // top of the loop
    //
    sevl

    //
    // wait until the mutex is available
    //
loop:
    wfe
    ldrex   r1, [r0]
    cmp     r1, #0
    bne     loop

    //
    // mutex is (at least, it was) available - try to claim it
    //
    mov     r1, #1
    strex   r2, r1, [r0]
    cmp     r2, #0
    bne     loop

    dmb                 // Ensure that accesses to shared resource have completed

    //
    // OK, we have the mutex, our work is done here
    //
    bx lr
    .cfi_endproc


    .type _mutex_release, "function"
    .cfi_startproc
_mutex_release:

    dmb                 // Ensure that accesses to shared resource have completed

    mov     r1, #0
    str     r1, [r0]

    dsb                 // Ensure that no instructions following the barrier execute until
                        // all memory accesses prior to the barrier have completed.

    sev                 // Send event to other CPUs, wakes anyone waiting on a mutex (using WFE)

    bx lr
    .cfi_endproc
