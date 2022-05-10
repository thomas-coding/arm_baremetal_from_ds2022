// ovl_veneer.s - assembly code for overlay load veneer and return thunk
//
// Copyright (c) 2016-2017 Arm Limited (or its affiliates). All rights reserved.
// Use, modification and redistribution of this file is subject to your possession of a
// valid End User License Agreement for the Arm Product of which these examples are part of 
// and your compliance with all applicable terms and conditions of such licence agreement.

	.text
	.p2align 3
	.eabi_attribute Tag_ABI_align8_preserved,1

	.weak __ARM_overlay_api_enter_critical_section
	.weak __ARM_overlay_api_leave_critical_section_and_branch
	.global __ARM_overlay_which_region
	.global __ARM_overlay_last_requested
	.global __ARM_overlay_push_return_stack
	.global __ARM_overlay_pop_return_stack
	.global __ARM_overlay_load

        .cfi_sections .debug_frame

.equ NOT_AN_OVERLAY, 0xFFFFFFFF

// This is the main entry point called by linker-generated veneers,
// when calling into or out of an overlay.
//
// It expects to be called with:
//
//  - r0 = the target overlay number, or -1 for NOT_AN_OVERLAY as defined
//    above
//
//  - r1 = the address of the target function, to transfer control to
//    once the overlay has been loaded.
//
//  - the original values of {r0,r1,r2,r3,r12,lr} pushed on the stack.
//    These will be popped before transferring control to the callee.
//
// (NOT_AN_OVERLAY is used in the case where the called function is not
// overlaid at all, so that the only purpose of going via this veneer
// is to insert a thunk in the return sequence to reload the _caller_'s
// overlay when the callee returns.)

	.global __ARM_overlay_entry
	.type __ARM_overlay_entry, %function
__ARM_overlay_entry:
        .fnstart
        .cantunwind
        .cfi_startproc
        .cfi_def_cfa_offset 6*4
        .cfi_rel_offset lr, 5*4

        // Start by storing the target branch address over the top of
        // the stacked lr, so that when we match it with a POP pc
        // later we'll branch to the right place. The value we're
        // overwriting is still available in lr itself.
        STR     r1, [sp, #5*4]

        PUSH    {r0,r4,r12,lr}
        .cfi_remember_state
        .cfi_adjust_cfa_offset 4*4
        .cfi_rel_offset r4, 1*4
        .cfi_rel_offset lr, 3*4

        BL      __ARM_overlay_api_enter_critical_section

        // Is our return address lr itself in an overlay region?
        LDR     r0, [sp, #3*4]
        BL      __ARM_overlay_which_region
        CMP     r0, #NOT_AN_OVERLAY
        BEQ     0f

        // If so, we need to insert a return veneer, so that when we
        // come back to that address, the overlay containing our
        // caller will be loaded there.

        // Determine which actual overlay number needs reloading into
        // this region by the return veneer.
        BL      __ARM_overlay_last_requested

        LDR     r1, [sp, #3*4]          // reload the target return address

        // Pass r0 = overlay number and r1 = target address to the API
        // function that will push them as a record on the overlay
        // return stack.
        BL      __ARM_overlay_push_return_stack

        // And now we must also rewrite the value of lr the callee
        // will receive, to point at the return thunk that will pop
        // that entry again.
        LDR     r0, =__ARM_overlay_return_thunk
        STR     r0, [sp, #3*4]          // overwrite stacked lr

0:

        // Now actually load the target overlay (if any).
        LDR     r0, [sp]
        CMP     r0, #NOT_AN_OVERLAY
        BLNE    __ARM_overlay_load

        // Restore the registers we pushed above.
        POP     {r0,r4,r12,lr}
        .cfi_restore_state

        // Re-enable interrupts, pop the registers pushed by the
        // linker veneer, and go to the target function.
        B       __ARM_overlay_api_leave_critical_section_and_branch
        // And fall through in case the user didn't implement that
        // function.
        POP     {r0,r1,r2,r3,r12,pc}

        .cfi_endproc
        .fnend
	.size	__ARM_overlay_entry, . - __ARM_overlay_entry

// Return thunk intentionally has no directives to generate DWARF FDE,
// so that debuggers won't attempt to backtrace through it and get
// confused by unloaded overlays.
	.type __ARM_overlay_return_thunk, %function
__ARM_overlay_return_thunk:
        PUSH    {r0,r1,r2,r3,r12,lr}

        BL      __ARM_overlay_api_enter_critical_section

        // Pop an (overlay number, return address) pair from the
        // overlay return stack.
        PUSH    {r0,r1}
        MOV     r0, sp
        BL      __ARM_overlay_pop_return_stack
        POP     {r0,r1}

        STR     r1, [sp,#5*4]           // overwrite stacked lr

        // Load the target overlay.
        BL      __ARM_overlay_load

        // Re-enable interrupts, unstack all original registers, and
        // go to the target return address.
        B       __ARM_overlay_api_leave_critical_section_and_branch
        // And fall through in case the user didn't implement that
        // function.
        POP     {r0,r1,r2,r3,r12,pc}

	.size	__ARM_overlay_return_thunk, . - __ARM_overlay_return_thunk
