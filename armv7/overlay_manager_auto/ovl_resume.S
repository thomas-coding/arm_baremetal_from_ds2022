// ovl_resume.s - assembly code for overlay reload on thread resume
//
// Copyright (c) 2016-2017 Arm Limited (or its affiliates). All rights reserved.
// Use, modification and redistribution of this file is subject to your possession of a
// valid End User License Agreement for the Arm Product of which these examples are part of 
// and your compliance with all applicable terms and conditions of such licence agreement.

	.text
	.p2align 3

	.weak __ARM_overlay_api_enter_critical_section
	.weak __ARM_overlay_api_leave_critical_section_and_branch
	.global __ARM_overlay_which_region
	.global __ARM_overlay_last_requested
	.global __ARM_overlay_push_return_stack
	.global __ARM_overlay_pop_return_stack
	.global __ARM_overlay_load

// This entry point is intended for multithreaded users of the overlay
// manager, in situations where an overlay has been evicted while a
// suspended thread was executing code from it.
//
// In that situation, you can recover by adjusting the suspended
// thread's state as follows. First, push two additional words on the
// thread's stack:
//
//  - (at the higher address) the PC of the suspended thread (in BX
//    style, i.e. with low bit set for Thumb or clear for Arm state)
//
//  - (at the lower address) the index of the overlay that has been
//    evicted. (This is passed as an argument to the API function
//    __ARM_overlay_api_evicted(), which is probably where this
//    adjustment will be happening.)
//
// Then, reset the thread's PC to point at __ARM_overlay_resume,
// defined below. When the thread resumes, this function will take care
// of reloading the overlay it needs, reloading all the register state,
// and returning control to whatever the thread was in the middle of
// doing.

	.global __ARM_overlay_resume
	.type __ARM_overlay_resume,%function

__ARM_overlay_resume:
        // On entry here, all registers are those of the thread's
        // state before the overlay it was executing in was evicted,
        // except for pc (which points _here_, obviously), and sp,
        // which is 8 bytes lower than it should be, because two extra
        // words have been pushed (as described above).

        // Push local registers. r12,lr are pushed last, because we'll
        // save a little effort later by popping them first.
        PUSH    {r0,r1,r2,r3}
        PUSH    {r12,lr}

        // Disable interrupts.
        BL      __ARM_overlay_api_enter_critical_section

        // Load the target overlay.
        LDR     r0, [sp, #6*4]          // get overlay number
        BL      __ARM_overlay_load      // and load it

        // Set up the 6-register state for return.
        POP     {r12,lr}
        STR     r12, [sp, #4*4]         // overwrite the overlay-id field

        // Re-enable interrupts, unstack all original registers, and
        // go to the target return address.
        B       __ARM_overlay_api_leave_critical_section_and_branch
        // And fall through in case the user didn't implement that
        // function. (Which would be odd, in this particular function!
        // But it only costs one instruction to allow for it anyway.)
        POP     {r0,r1,r2,r3,r12,pc}

        .size	__ARM_overlay_resume, . - __ARM_overlay_resume
