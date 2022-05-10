//----------------------------------------------------------------
// Cortex-R7 Embedded example - Startup Code
//
// Copyright (c) 2006-2021 Arm Limited (or its affiliates). All rights reserved.
// Use, modification and redistribution of this file is subject to your possession of a
// valid End User License Agreement for the Arm Product of which these examples are part of 
// and your compliance with all applicable terms and conditions of such licence agreement.
//----------------------------------------------------------------

// MPU region defines


// Region size <256 bytes is unpredictable

#define Region_256B 0b00111
#define Region_512B 0b01000
#define Region_1K   0b01001
#define Region_2K   0b01010
#define Region_4K   0b01011
#define Region_8K   0b01100
#define Region_16K  0b01101
#define Region_32K  0b01110
#define Region_64K  0b01111
#define Region_128K 0b10000
#define Region_256K 0b10001
#define Region_512K 0b10010
#define Region_1M   0b10011
#define Region_2M   0b10100
#define Region_4M   0b10101
#define Region_8M   0b10110
#define Region_16M  0b10111
#define Region_32M  0b11000
#define Region_64M  0b11001
#define Region_128M 0b11010
#define Region_256M 0b11011
#define Region_512M 0b11100
#define Region_1G   0b11101
#define Region_2G   0b11110
#define Region_4G   0b11111

#define Region_Enable 0b1

#define Execute_Never 0x1000 // Bit 12

#define Normal_nShared 0x0B // Outer and Inner Write-Back, Write-Allocate
#define Device_nShared 0x10

#define Full_Access 0b011
#define Read_Only   0b110

//----------------------------------------------------------------

    .eabi_attribute Tag_ABI_align8_preserved,1

    .section  VECTORS,"ax"
    .align 3
    .cfi_sections .debug_frame  // put stack frame info into .debug_frame instead of .eh_frame


//----------------------------------------------------------------
// Entry point for the Reset handler
//----------------------------------------------------------------

    .global Start

Start:

//----------------------------------------------------------------
// Exception Vector Table
//----------------------------------------------------------------
// Note: LDR PC instructions are used here, though branch (B) instructions
// could also be used, unless the exception handlers are >32MB away.

Vectors:
        LDR PC, Reset_Addr
        LDR PC, Undefined_Addr
        LDR PC, SVC_Addr
        LDR PC, Prefetch_Addr
        LDR PC, Abort_Addr
        B .                                // Reserved vector
        LDR PC, IRQ_Addr
        LDR PC, FIQ_Addr


    .balign 4
Reset_Addr:     .word Reset_Handler
Undefined_Addr: .word Undefined_Handler
SVC_Addr:       .word SVC_Handler
Prefetch_Addr:  .word Prefetch_Handler
Abort_Addr:     .word Abort_Handler
IRQ_Addr:       .word IRQ_Handler
FIQ_Addr:       .word FIQ_Handler


//----------------------------------------------------------------
// Exception Handlers
//----------------------------------------------------------------

Undefined_Handler:
        B   Undefined_Handler
SVC_Handler:
        B   SVC_Handler
Prefetch_Handler:
        B   Prefetch_Handler
Abort_Handler:
        B   Abort_Handler
IRQ_Handler:
        B   IRQ_Handler
FIQ_Handler:
        B   FIQ_Handler


//----------------------------------------------------------------
// Reset Handler
//----------------------------------------------------------------

    .global Reset_Handler
    .type Reset_Handler, "function"
Reset_Handler:

//----------------------------------------------------------------
// Disable MPU and caches
//----------------------------------------------------------------

// Disable MPU and cache in case it was left enabled from an earlier run
// This does not need to be done from a cold reset

        MRC p15, 0, r0, c1, c0, 0       // Read System Control Register
        BIC r0, r0, #0x05               // Disable MPU (M bit) and data cache (C bit)
        BIC r0, r0, #0x800              // Disable branch prediction (Z bit)
        BIC r0, r0, #0x1000             // Disable instruction cache (I bit)
        DSB                             // Ensure all previous loads/stores have completed
        MCR p15, 0, r0, c1, c0, 0       // Write System Control Register
        ISB                             // Ensure subsequent insts execute wrt new MPU settings


//----------------------------------------------------------------
// Initialize Supervisor Mode Stack using Linker symbol from scatter file.
// Stacks must be 8 byte aligned.
//----------------------------------------------------------------

        .global  Image$$ARM_LIB_STACKHEAP$$ZI$$Limit
        LDR SP, =Image$$ARM_LIB_STACKHEAP$$ZI$$Limit


//----------------------------------------------------------------
// Cache invalidation
//----------------------------------------------------------------

        DSB             // Complete all outstanding explicit memory operations

        MOV     r0, #0

        MCR     p15, 0, r0, c7, c5, 0       // Invalidate entire instruction cache

        // Invalidate Data Cache

        MRC     p15, 1, r0, c0, c0, 1      // Read CLIDR
        ANDS    r3, r0, #0x07000000        // Extract coherency level
        MOV     r3, r3, LSR #23            // Total cache levels << 1
        BEQ     Finished                   // If 0, no need to clean

        MOV     r10, #0                    // R10 holds current cache level << 1
Loop1:  ADD     r2, r10, r10, LSR #1       // R2 holds cache "Set" position 
        MOV     r1, r0, LSR r2             // Bottom 3 bits are the Cache-type for this level
        AND     r1, r1, #7                 // Isolate those lower 3 bits
        CMP     r1, #2
        BLT     Skip                       // No cache or only instruction cache at this level

        MCR     p15, 2, r10, c0, c0, 0     // Write the Cache Size selection register
        ISB                                // ISB to sync the change to the CacheSizeID reg
        MRC     p15, 1, r1, c0, c0, 0      // Reads current Cache Size ID register
        AND     r2, r1, #7                 // Extract the line length field
        ADD     r2, r2, #4                 // Add 4 for the line length offset (log2 16 bytes)
        LDR     r4, =0x3FF
        ANDS    r4, r4, r1, LSR #3         // R4 is the max number on the way size (right aligned)
        CLZ     r5, r4                     // R5 is the bit position of the way size increment
        LDR     r7, =0x7FFF
        ANDS    r7, r7, r1, LSR #13        // R7 is the max number of the index size (right aligned)

Loop2:  MOV     r9, r4                     // R9 working copy of the max way size (right aligned)

Loop3:  ORR     r11, r10, r9, LSL r5       // Factor in the Way number and cache number into R11
        ORR     r11, r11, r7, LSL r2       // Factor in the Set number
        MCR     p15, 0, r11, c7, c6, 2     // Invalidate by Set/Way
        SUBS    r9, r9, #1                 // Decrement the Way number
        BGE     Loop3
        SUBS    r7, r7, #1                 // Decrement the Set number
        BGE     Loop2
Skip:   ADD     r10, r10, #2               // Increment the cache number
        CMP     r3, r10
        BGT     Loop1

Finished:


//----------------------------------------------------------------
// TCM Configuration
//----------------------------------------------------------------

// Cortex-R7 optionally provides two Tightly-Coupled Memory (TCM) blocks (ITCM and DTCM) 
//    for fast access to code or data.
// ITCM typically holds interrupt or exception code that must be accessed at high speed,
//    without any potential delay resulting from a cache miss.
// DTCM typically holds a block of data for intensive processing, such as audio or video data.

// The following illustrates basic TCM configuration, as the basis for exploration by the user

#ifdef TCM
    .global  Image$$ITCM$$Base
    .global  Image$$DTCM0$$Base

        MRC     p15, 0, r0, c0, c0, 2       // Read TCM Type Register
        // r0 now contains ITCM & DTCM availability

        MRC     p15, 0, r0, c9, c1, 1       // Read ITCM Region Register
        // r0 now contains ITCM size in bits [6:2]

        MRC     p15, 0, r0, c9, c1, 0       // Read DTCM Region Register
        // r0 now contains DTCM size in bits [6:2]

        LDR     r0, =Image$$ITCM$$Base      // Set ITCM base address
        ORR     r0, r0, #1                  // Enable it
        MCR     p15, 0, r0, c9, c1, 1       // Write ITCM Region Register

        LDR     r0, =Image$$DTCM$$Base      // Set DTCM base address
        ORR     r0, r0, #1                  // Enable it
        MCR     p15, 0, r0, c9, c1, 0       // Write DTCM Region Register

#endif


//----------------------------------------------------------------
// MPU Configuration
//----------------------------------------------------------------

// Notes:
// * Regions apply to both instruction and data accesses.
// * Each region base address must be a multiple of its size
// * Any address range not covered by an enabled region will abort
// * The region at 0x0 over the Vector table is needed to support semihosting

// Region 0: Code          Base = 0x00000000  Size = 32KB   Normal  Non-shared  Read-only    Executable
// Region 1: Data          Base = 0x00008000  Size = 16KB   Normal  Non-shared  Full access  Not Executable
// Region 2: Stack/Heap    Base = 0x0000C000  Size = 16KB   Normal  Non-shared  Full access  Not Executable
// Region 3: Vectors       Base = 0x00000000  Size =  1KB   Normal  Non-shared  Full access  Executable

        // Import linker symbols to get region base addresses
    .global  Image$$CODE$$Base
    .global  Image$$DATA$$Base
    .global  Image$$ARM_LIB_STACKHEAP$$Base

        // Region 0 - Code
        MOV     r1, #0
        MCR     p15, 0, r1, c6, c2, 0       // Set memory region number register
        ISB                                 // Ensure subsequent insts execute wrt this region
        LDR     r2, =Image$$CODE$$Base
        MCR     p15, 0, r2, c6, c1, 0       // Set region base address register
        LDR     r2, =0x0  |  (Region_32K << 1)  |  Region_Enable
        MCR     p15, 0, r2, c6, c1, 2       // Set region size & enable register
        LDR     r2, =0x0  |  (Read_Only  << 8)  |  Normal_nShared
        MCR     p15, 0, r2, c6, c1, 4       // Set region access control register

        // Region 1 - Data
        ADD     r1, r1, #1
        MCR     p15, 0, r1, c6, c2, 0       // Set memory region number register
        ISB                                 // Ensure subsequent insts execute wrt this region
        LDR     r2, =Image$$DATA$$Base
        MCR     p15, 0, r2, c6, c1, 0       // Set region base address register
        LDR     r2, =0x0  |  (Region_16K << 1)  |  Region_Enable
        MCR     p15, 0, r2, c6, c1, 2       // Set region size & enable register
        LDR     r2, =0x0  |  (Full_Access << 8)  |  Normal_nShared  |  Execute_Never
        MCR     p15, 0, r2, c6, c1, 4       // Set region access control register

        // Region 2 - Stack/Heap
        ADD     r1, r1, #1
        MCR     p15, 0, r1, c6, c2, 0       // Set memory region number register
        ISB                                 // Ensure subsequent insts execute wrt this region
        LDR     r2, =Image$$ARM_LIB_STACKHEAP$$Base
        MCR     p15, 0, r2, c6, c1, 0       // Set region base address register
        LDR     r2, =0x0  |  (Region_16K << 1)  |  Region_Enable
        MCR     p15, 0, r2, c6, c1, 2       // Set region size & enable register
        LDR     r2, =0x0  |  (Full_Access << 8)  |  Normal_nShared  |  Execute_Never
        MCR     p15, 0, r2, c6, c1, 4       // Set region access control register

        // Region 3 - Vectors
        ADD     r1, r1, #1
        MCR     p15, 0, r1, c6, c2, 0       // Set memory region number register
        ISB                                 // Ensure subsequent insts execute wrt this region
        LDR     r2, =0
        MCR     p15, 0, r2, c6, c1, 0       // Set region base address register
        LDR     r2, =0x0  |  (Region_1K  << 1)  |  Region_Enable
        MCR     p15, 0, r2, c6, c1, 2       // Set region size & enable register
        LDR     r2, =0x0  |  (Full_Access << 8)  |  Normal_nShared
        MCR     p15, 0, r2, c6, c1, 4       // Set region access control register

        // Disable all higher priority regions (assumes unified regions, which is always true for Cortex-R7)
        MRC     p15, 0, r0, c0, c0, 4       // Read MPU Type register (MPUIR)
        LSR     r0, r0, #8
        AND     r0, r0, #0xff               // r0 = Number of MPU regions (12 or 16 for Cortex-R7)
        MOV     r2, #0                      // Value to write to disable region
region_loop:
        ADD     r1, r1, #1
        CMP     r0, r1
        BLS     regions_done
        MCR     p15, 0, r1, c6, c2, 0       // Set memory region number register (RGNR)
        MCR     p15, 0, r2, c6, c1, 2       // Set region size & enable register (DRSR)
        B       region_loop
regions_done:


#ifdef __ARM_FP
//----------------------------------------------------------------
// Enable access to VFP by enabling access to Coprocessors 10 and 11.
// Enables Full Access i.e. in both privileged and non privileged modes
//----------------------------------------------------------------

        MRC     p15, 0, r0, c1, c0, 2      // Read Coprocessor Access Control Register (CPACR)
        ORR     r0, r0, #(0xF << 20)       // Enable access to CP 10 & 11
        MCR     p15, 0, r0, c1, c0, 2      // Write Coprocessor Access Control Register (CPACR)
        ISB

//----------------------------------------------------------------
// Switch on the VFP hardware
//----------------------------------------------------------------

        MOV     r0, #0x40000000
        VMSR    FPEXC, r0                   // Write FPEXC register, EN bit set
#endif


//----------------------------------------------------------------
// Enable MPU and branch to C library init
// Leaving the caches disabled until after scatter loading.
//----------------------------------------------------------------

        MRC     p15, 0, r0, c1, c0, 0       // Read System Control Register
        ORR     r0, r0, #0x01               // Set M bit to enable MPU
        ORR     r0, r0, #0x800              // Set Z bit to enable branch prediction
        DSB                                 // Ensure all previous loads/stores have completed
        MCR     p15, 0, r0, c1, c0, 0       // Write System Control Register
        ISB                                 // Ensure subsequent insts execute wrt new MPU settings

    .global     __main
        B       __main

    .size Reset_Handler, . - Reset_Handler


//----------------------------------------------------------------
// Global Enable for Instruction and Data Caching
//----------------------------------------------------------------

    .global enable_caches

    .type enable_caches, "function"
    .cfi_startproc
enable_caches:

        MRC     p15, 0, r0, c1, c0, 0       // Read System Control Register
        ORR     r0, r0, #(0x1 << 12)        // enable I Cache
        ORR     r0, r0, #(0x1 << 2)         // enable D Cache
        MCR     p15, 0, r0, c1, c0, 0       // Write System Control Register
        ISB

        BX    lr
    .cfi_endproc

    .size enable_caches, . - enable_caches
