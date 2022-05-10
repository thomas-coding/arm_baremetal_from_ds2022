// ------------------------------------------------------------
// Armv8-R64 AArch64 Startup Code
//
// Includes MPU configuration
//
// Starts in EL2S, exits in EL1S
//
// Copyright (c) 2014-2021 Arm Limited (or its affiliates). All rights reserved.
// Use, modification and redistribution of this file is subject to your possession of a
// valid End User License Agreement for the Arm Product of which these examples are part of 
// and your compliance with all applicable terms and conditions of such licence agreement.
// ------------------------------------------------------------

#include "v8_system.h"

// MPU region defines

// Protection Region Base Address Register
#define Execute_Never   0b1       // Bit 0
#define RW_this_EL_only      0b00 // AP[2:1]
#define RW_this_EL_and_lower 0b01
#define RO_this_EL_only      0b10
#define RO_this_EL_and_lower 0b11
#define Non_Shareable   0b00      // SH[1:0]
#define Outer_Shareable 0x10
#define Inner_Shareable 0b11

// Protection Region Limit Address Register
#define ENable 0b1                // Bit 0
#define AttrIndx0 0b000           // AttrIndx[2:0]
#define AttrIndx1 0b001
#define AttrIndx2 0b010
#define AttrIndx3 0b011
#define AttrIndx4 0b100
#define AttrIndx5 0b101
#define AttrIndx6 0b110
#define AttrIndx7 0b111

    .section StartUp, "ax"
    .balign 4
    .cfi_sections .debug_frame  // put stack frame info into .debug_frame instead of .eh_frame

    .global el1_vectors
    .global el2_vectors

    // use separate stack and heap, as anticipated by scatter.scat
    .global __use_two_region_memory


// ------------------------------------------------------------

    .global start64
    .type start64, "function"
start64:
    // Extract the core number from MPIDR_EL1 and store it in x19
    // (defined by the AAPCS as callee-saved), so we can re-use it later
    //
    bl GetCPUID
    mov x19, x0

    // If run on a multi-core system, put any secondary cores to sleep
    cbz x19, core0_only
loop_wfi:
    dsb SY      // Clear all pending data accesses
    wfi         // Go to sleep
    b loop_wfi

core0_only:

    //
    // program the VBARs
    //
    ldr x1, =el1_vectors
    msr VBAR_EL1, x1

    ldr x1, =el2_vectors
    msr VBAR_EL2, x1


    mov x0, #0
    msr SCTLR_EL2, x0
    msr SCTLR_EL1, x0


//----------------------------------------------------------------
// EL2 MPU Configuration
//----------------------------------------------------------------

    // Region 0 - Code
    ldr x1, =Image$$CODE$$Base
    ldr x2, =((Non_Shareable << 4) | (RO_this_EL_only << 2))
    orr x1, x1, x2
    msr PRBAR_EL2, x1

    ldr x1, =Image$$CODE$$Limit     // Could use Load$$DATA$$Limit to include RW data
    sub x1, x1, #1                  // Convert limit from exclusive to inclusive
    bfc x1, #0, #6                  // and clear the lower 6 bits
    ldr x2, =((AttrIndx0 << 1) | (ENable))
    orr x1, x1, x2
    msr PRLAR_EL2, x1

    // Region 1 - Data
    ldr x1, =Image$$DATA$$Base
    ldr x2, =((Non_Shareable << 4) | (RW_this_EL_only << 2))
    orr x1, x1, x2
    msr PRBAR1_EL2, x1

    ldr x1, =Image$$ARM_LIB_STACK$$ZI$$Limit
    sub x1, x1, #1
    bfc x1, #0, #6
    ldr x2, =((AttrIndx0 << 1) | (ENable))
    orr x1, x1, x2
    msr PRLAR1_EL2, x1

    // MAIR_EL2 configuration
    mrs x0, MAIR_EL2
    ldr x1, =0xBB                   // Normal inner/outer RW cacheable, write-through
    bfi x0, x1, #0, #8              // Update Attr0
    ldr x1, =0x04                   // Device nGnRnE
    bfi x0, x1, #8, #8              // Update Attr1
    msr MAIR_EL2, x0

//----------------------------------------------------------------
// Enable EL2 MPU and caches
//----------------------------------------------------------------

    mrs x1, SCTLR_EL2
    orr x1, x1, #SCTLR_ELx_M
    orr x1, x1, #SCTLR_ELx_C
    orr x1, x1, #SCTLR_ELx_I
    bic x1, x1, #SCTLR_ELx_A // Disable alignment fault checking.  To enable, change bic to orr
    msr SCTLR_EL2, x1


    //
    // Set up EL1 entry point and "dummy" exception return information,
    // then perform exception return to enter EL1
    //
    .global drop_to_el1
drop_to_el1:
    adr x1, el1_entry_aarch64
    msr ELR_EL2, x1
    mov x1, #(AARCH64_SPSR_EL1h | \
              AARCH64_SPSR_F  | \
              AARCH64_SPSR_I  | \
              AARCH64_SPSR_A)
    msr SPSR_EL2, x1
    eret



// ------------------------------------------------------------
// EL1 - Common start-up code
// ------------------------------------------------------------

    .global el1_entry_aarch64
    .type el1_entry_aarch64, "function"
el1_entry_aarch64:

    //
    // Now we're in EL1, setup the application stack
    // the scatter file allocates 2^14 bytes per app stack
    //
    ldr x0, =Image$$ARM_LIB_STACK$$ZI$$Limit
    sub x0, x0, x19, lsl #14
    mov sp, x0

    //
    // Enable floating point
    //
    mov x0, #CPACR_EL1_FPEN
    msr CPACR_EL1, x0

//----------------------------------------------------------------
// EL1 MPU Configuration
//----------------------------------------------------------------

    // Region 0 - Code
    ldr x1, =Image$$CODE$$Base
    ldr x2, =((Non_Shareable << 4) | (RO_this_EL_and_lower << 2))
    orr x1, x1, x2
    msr PRBAR_EL1, x1

    ldr x1, =Image$$CODE$$Limit
    sub x1, x1, #1                  // convert limit from exclusive to inclusive
    bfc x1, #0, #6                  // and clear the lower 6 bits
    ldr x2, =((AttrIndx0 << 1) | (ENable))
    orr x1, x1, x2
    msr PRLAR_EL1, x1

    // Region 1 - Data
    ldr x1, =Image$$DATA$$Base
    ldr x2, =((Non_Shareable << 4) | (RW_this_EL_and_lower << 2))
    orr x1, x1, x2
    msr PRBAR1_EL1, x1

    ldr x1, =Image$$ARM_LIB_STACK$$ZI$$Limit
    sub x1, x1, #1
    bfc x1, #0, #6
    ldr x2, =((AttrIndx0 << 1) | (ENable))
    orr x1, x1, x2
    msr PRLAR1_EL1, x1

    // MAIR_EL1 configuration
    mrs x0, MAIR_EL1
    ldr x1, =0xBB                   // Normal inner/outer RW cacheable, write-through
    bfi x0, x1, #0, #8              // Update Attr0
    ldr x1, =0x04                   // Device nGnRnE
    bfi x0, x1, #8, #8              // Update Attr1
    msr MAIR_EL1, x0

//----------------------------------------------------------------
// Branch to C library init
// Leaving the MPU and caches disabled until after scatter loading.
//----------------------------------------------------------------

    .global __main
    b       __main


//----------------------------------------------------------------
// Enable EL1 MPU and caches
//----------------------------------------------------------------

    .global enable_mpu_and_caches
    .type enable_mpu_and_caches, "function"
    .cfi_startproc
enable_mpu_and_caches:
    mrs x1, SCTLR_EL1          // Read System Control Register
    orr x1, x1, #SCTLR_ELx_M   // Set M bit to enable MPU
    orr x1, x1, #SCTLR_ELx_C   // Enable D-cache
    orr x1, x1, #SCTLR_ELx_I   // Enable I-cache
    bic x1, x1, #SCTLR_ELx_A   // Disable alignment fault checking.  To enable, change bic to orr
    msr SCTLR_EL1, x1          // Write System Control Register
    isb                        // Ensure subsequent insts execute wrt new MPU settings
    ret
    .cfi_endproc

