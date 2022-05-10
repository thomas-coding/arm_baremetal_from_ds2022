/* ovl_api.h - definitions of user-replaceable overlay API functions
 *
 * Copyright (c) 2016-2017 Arm Limited (or its affiliates). All rights reserved.
 * Use, modification and redistribution of this file is subject to your possession of a
 * valid End User License Agreement for the Arm Product of which these examples are part of 
 * and your compliance with all applicable terms and conditions of such licence agreement.
 */

#include <stddef.h>

#ifdef __cplusplus
#error Overlays are not supported in C++
#endif

/*
 * Macro that tags a function declaration as living in a particular
 * overlay.
 */
#define __ARM_OVERLAY(n) __attribute__((section(".ARM.overlay" #n)))

/* ----------------------------------------------------------------------
 * API functions that the user can reimplement to customize the
 * functionality of this overlay manager.
 */

/*
 * Function: __ARM_overlay_api_load
 *
 * Purpose:  Load an overlay into memory
 *
 * Arguments:
 *
 *   overlay_id     The integer id of the overlay to be loaded.
 *
 *   destination    The address that the overlay contents must be copied
 *                  to. (This will be the start address of one of the
 *                  regions marked as AUTO_OVERLAY in your scatter
 *                  load description.)
 *
 * Return value: none.
 *
 * Remarks:
 *
 *   This function has no way to report failure, because the overlay
 *   manager would have no way to recover if it does fail.
 *
 *   This function is assumed to have its own method of finding out
 *   the size of each overlay.
 */
void __ARM_overlay_api_load(int overlay_id, void *destination);

/*
 * Function: __ARM_overlay_api_loaded
 *
 * Purpose:  Perform cache maintenance after an overlay is loaded
 *
 * Arguments:
 *
 *   overlay_start  The start address of the memory that the overlay has
 *                  just been loaded into. Will equal the start
 *                  address of that overlay's region.
 *
 *   overlay_end    The end address of the memory that the overlay has
 *                  just been loaded into. May not equal the end
 *                  address of that overlay's region, if this
 *                  particular overlay is shorter than the full size
 *                  of the region.
 *
 * Return value: none.
 *
 * Remarks:
 *
 *   This function is called after loading an overlay into memory, and
 *   before branching to the newly written memory. It should perform
 *   any necessary cache invalidations or instruction memory barrier
 *   operations to ensure that instruction fetches from the memory
 *   will return the newly loaded code and not be polluted by its
 *   previous contents.
 */
void __ARM_overlay_api_loaded(void *overlay_start, void *overlay_end);

/*
 * Function: __ARM_overlay_api_max_stack_size
 *
 * Purpose: Specify the maximum stack size.
 *
 * Arguments: none.
 *
 * Return value: an integer giving the maximum number of nested
 * function calls between overlays that the stack must permit.
 *
 * Remarks:
 *
 *   In a multithreaded environment, this function is called from
 *   within the context of each new thread, when that thread's
 *   overlay state is initialized. So if you need different threads
 *   to have different maximum nesting depths, you can arrange that
 *   by checking the thread id within this function.
 */
int __ARM_overlay_api_max_stack_size(void);

/*
 * Function: __ARM_overlay_api_alloc
 *
 * Purpose: Allocate memory for a new thread's overlay state
 *
 * Arguments:
 *
 *   size            Size of memory to allocate, in bytes.
 *
 * Return value: a pointer to the allocated memory.
 *
 * Remarks:
 *
 *   In a multithreaded environment, this function will be called
 *   from within the context of each new thread to allocate the state
 *   for that thread. So if you want the memory to be cleaned up when
 *   the thread terminates, then your implementation of this function
 *   should have the side effect of recording the pointer it returns
 *   in the OS's data structure for the thread, so that the thread
 *   cleanup code can free it.
 */
void *__ARM_overlay_api_alloc(size_t size);

/*
 * Function: __ARM_overlay_api_enter_critical_section
 *
 * Purpose: Signal that an overlay manager operation is about to
 * begin, and must not be interrupted part way through
 *
 * Remarks:
 *
 *   The default behavior, if this function is not implemented, is to
 *   not disable interrupts before an overlay manager operation. If
 *   the overlay manager state cannot be concurrently modified by any
 *   other CPU or thread or interrupt handler, it is safe not to
 *   implement this function.
 */
void __ARM_overlay_api_enter_critical_section(void);

/*
 * Function: __ARM_overlay_api_leave_critical_section_and_branch
 *
 * Purpose: Signal that an overlay manager operation has concluded
 * and interrupts can now be re-enabled; additionally, transfer
 * control to a target address in the newly loaded overlay.
 *
 * Prototype:
 *
 *   None. This function's API is unsuitable for implementation in C.
 *
 *   (It is included in this header file nonetheless, so that it can
 *   be address-taken if necessary; the dummy incomplete struct in its
 *   prototype is intended to prevent accidental calls to it from C
 *   code.)
 *
 * Semantics:
 *
 *   This function is invoked by a non-linking branch (B rather than
 *   BL). On entry, the stack contains six register values. The
 *   function must pop all six of them, store the first five in
 *   r0,r1,r2,r3,r12, and branch to the address in the final one
 *   (with BX semantics, i.e. selecting Arm or Thumb state based on
 *   the low bit).
 *
 *   In other words, branching to this function must have the same
 *   effect on the core registers and PC as executing the instruction
 *   'POP {r0,r1,r2,r3,r12,pc}' would have.
 *
 *   In addition to that effect, this function must also undo
 *   whatever disabling of interrupts was done by
 *   __ARM_overlay_api_enter_critical_section.
 */
struct __opaque;
void __ARM_overlay_api_leave_critical_section_and_branch(struct __opaque);

/*
 * Function: __ARM_overlay_api_perthread
 *
 * Purpose: Return pointer to the current thread's overlay state
 *
 * Arguments: none.
 *
 * Return value: a pointer to the overlay state.
 *
 * Remarks:
 *
 *   The default behavior, if this function is not implemented, is to
 *   return the same pointer value every time, which is an adequate
 *   policy for use in a single-threaded environment. The object file
 *   declaring the default version of this function also uses $Sub and
 *   $Super to wrap main(), so as to allocate the memory for that
 *   pointer just before the main program starts up.
 *
 *   In a multithreaded environment, you would need to replace this
 *   function with one that returned a different value for each
 *   thread, e.g. by a TLS mechanism or by storing the value in the
 *   OS's control block for the thread. Also, on creation of a new
 *   thread, its overlay state would have to be created by calling
 *   __ARM_overlay_perthread_new().
 */
void *__ARM_overlay_api_perthread(void);

/*
 * Function: __ARM_overlay_api_evicted
 *
 * Purpose: Signal to the OS that an overlay is being evicted
 *
 * Arguments:
 *
 *   region_start    Start address of the affected overlay region.
 *
 *   region_end      End address of the affected overlay region.
 *
 *   overlay_id      Integer id of the overlay being evicted.
 *
 * Remarks:
 *
 *   The expected work done by this function would be something along
 *   the lines of: iterate over all the current non-executing threads
 *   known to the OS. If any thread's PC is within the range between
 *   'region_start' and 'region_end' (including the former, and
 *   excluding the latter), then the OS's data structure for that
 *   suspended thread must be adjusted so that the overlay numbered
 *   'overlay_id' is automatically reloaded (if necessary) when that
 *   thread is resumed.
 *
 *   The adjustment should take the following form: alter the
 *   thread's saved pc to point at the symbol '__ARM_overlay_resume',
 *   and push two extra words on the thread's stack, of which the one
 *   pushed first (i.e. at higher address) is the old value of the
 *   thread's saved pc, and the second (at lower address) is the
 *   value 'overlay_id' passed to this function.
 *
 *   That adjustment is sufficient to ensure that, when the thread
 *   resumes, it will begin executing from __ARM_overlay_resume,
 *   which will know what overlay to reload and where to branch to
 *   once it has done so.
 *
 *   Also, the OS will almost certainly need to give special
 *   treatment to threads in the middle of executing the API function
 *   __ARM_overlay_api_leave_critical_section_and_branch. It is
 *   expected that a typical implementation of that function will
 *   issue an instruction to re-enable interrupts, and then a POP
 *   instruction writing to pc, to unstack the saved registers and
 *   branch to the target address in the loaded overlay. Between
 *   those two instructions there will be at least one observable
 *   instant in which thread pre-emption could occur, in which the
 *   thread's pc will _not_ be in the target overlay (it will instead
 *   be in __ARM_overlay_api_leave_critical_section_and_branch) and
 *   yet the thread will crash if the target overlay is not reloaded
 *   before resumption. Therefore, the OS will need to identify
 *   threads whose pc is in that at-risk range; check their stack to
 *   retrieve the return address from the 6-register stack frame used
 *   by __ARM_overlay_api_leave_critical_section_and_branch; and if
 *   _that_ is in the evicted range, treat the thread as effectively
 *   having its pc in the target overlay, and replace that 6-register
 *   stack frame with the 2-register frame of __ARM_overlay_resume.
 */
void __ARM_overlay_api_evicted(void *region_start, void *region_end,
                               int overlay_id);

/* ----------------------------------------------------------------------
 * Functions provided by the overlay manager that the above user
 * functions can call.
 */

/*
 * Function: __ARM_overlay_perthread_new
 *
 * Purpose: Allocate overlay state for a new thread.
 *
 * Arguments: none.
 *
 * Return value: a pointer to a newly allocated overlay state.
 *
 * Remarks:
 *
 *   This function works out how much memory to allocate for the
 *   overlay state of a new thread, by calling the API function
 *   __ARM_overlay_api_max_stack_size (see above) and adding the
 *   result to the other storage requirements. Then it calls
 *   __ARM_overlay_api_alloc (see above) to allocate the memory,
 *   initializes fields inside the returned block as necessary, and
 *   returns the resulting pointer.
 *
 *   You will need to call this function if you are working in a
 *   multithreaded context, in order to create each new thread's
 *   overlay state. The resulting pointer should be stored somewhere
 *   that your implementation of __ARM_overlay_api_perthread (see
 *   above) can find it.
 */
struct __perthread;
struct __perthread *__ARM_overlay_perthread_new(void);
