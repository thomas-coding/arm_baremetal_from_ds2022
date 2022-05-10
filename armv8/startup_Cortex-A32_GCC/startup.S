//----------------------------------------------------------------
// Cortex-A32 Embedded example - Startup Code
//
// Copyright (c) 2016-2018 Arm Limited (or its affiliates). All rights reserved.
// Use, modification and redistribution of this file is subject to your possession of a
// valid End User License Agreement for the Arm Product of which these examples are part of 
// and your compliance with all applicable terms and conditions of such licence agreement.
//----------------------------------------------------------------


// Standard definitions of mode bits and interrupt (I & F) flags in PSRs

#define Mode_USR 0x10
#define Mode_FIQ 0x11
#define Mode_IRQ 0x12
#define Mode_SVC 0x13
#define Mode_ABT 0x17
#define Mode_UND 0x1B
#define Mode_SYS 0x1F

#define I_Bit 0x80 // When I bit is set, IRQ is disabled
#define F_Bit 0x40 // When F bit is set, FIQ is disabled


    .section  VECTORS, "ax"
    .align 3
    .cfi_sections .debug_frame  // put stack frame info into .debug_frame instead of .eh_frame

 
//----------------------------------------------------------------
// Entry point for the Reset handler
//----------------------------------------------------------------

    .global Vectors

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
    LDR PC, Hypervisor_Addr
    LDR PC, IRQ_Addr
    LDR PC, FIQ_Addr


    .balign 4
Reset_Addr:
    .word Reset_Handler
Undefined_Addr:
    .word Undefined_Handler
SVC_Addr:
    .word SVC_Handler
Prefetch_Addr:
    .word Prefetch_Handler
Abort_Addr:
    .word Abort_Handler
Hypervisor_Addr:
    .word Hypervisor_Handler
IRQ_Addr:
    .word IRQ_Handler
FIQ_Addr:
    .word FIQ_Handler


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
Hypervisor_Handler:
    B   Hypervisor_Handler
IRQ_Handler:
    B   IRQ_Handler
FIQ_Handler:
    B   FIQ_Handler


//----------------------------------------------------------------
// Reset Handler
//----------------------------------------------------------------
Reset_Handler:

//----------------------------------------------------------------
// Disable caches and MMU in case they were left enabled from an earlier run
// This does not need to be done from a cold reset
//----------------------------------------------------------------

    MRC     p15, 0, r0, c1, c0, 0       // Read System Control Register
    BIC     r0, r0, #(0x1 << 12)        // Clear I bit 12 to disable I Cache
    BIC     r0, r0, #(0x1 <<  2)        // Clear C bit  2 to disable D Cache
    BIC     r0, r0, #0x1                // Clear M bit  0 to disable MMU
    MCR     p15, 0, r0, c1, c0, 0       // Write System Control Register
    ISB

// The MMU is enabled later, before calling main().  Caches are enabled inside main(),
// after the MMU has been enabled and scatterloading has been performed.

//----------------------------------------------------------------
// CPUECTLR.SMPEN bit must be set before the caches and MMU are enabled,
// or any cache and TLB maintenance operations are performed, even for "AMP" CPUs.
//----------------------------------------------------------------
        MRRC    p15, 1, r0, r1, c15        // Read CPUECTLR
        ORR     r0, r0, #(1 << 6)          // Set CPUECTLR.SMPEN bit
        MCRR    p15, 1, r0, r1, c15        // Write CPUECTLR

//----------------------------------------------------------------
// Initialize Supervisor Mode Stack
// Note stack must be 8 byte aligned.
//----------------------------------------------------------------

        LDR     SP, =__stack

//----------------------------------------------------------------
// Invalidate Data and Instruction TLBs and branch predictor in case they were left enabled from an earlier run
// This does not need to be done from a cold reset
//----------------------------------------------------------------

        MOV     r0,#0
        MCR     p15, 0, r0, c8, c7, 0      // I-TLB and D-TLB invalidation
        MCR     p15, 0, r0, c7, c5, 6      // BPIALL - Invalidate entire branch predictor array

//----------------------------------------------------------------
// Set Vector Base Address Register (VBAR) to point to this application's vector table
//----------------------------------------------------------------

        LDR     r0, =Vectors
        MCR     p15, 0, r0, c12, c0, 0

//----------------------------------------------------------------
// Cache Invalidation
// The processor automatically invalidates caches on reset unless suppressed with the DBGL1RSTDISABLE or L2RSTDISABLE pins.
// It is therefore not necessary for software to invalidate the caches on start-up.
//----------------------------------------------------------------

        // Invalidate L1 Instruction Cache

        MRC     p15, 1, r0, c0, c0, 1      // Read Cache Level ID Register (CLIDR)
        TST     r0, #0x3                   // Harvard Cache?
        MOV     r0, #0                     // SBZ
        MCRNE   p15, 0, r0, c7, c5, 0      // ICIALLU - Invalidate instruction cache and flush branch target cache

        // Invalidate Data Cache

        MRC     p15, 1, r0, c0, c0, 1      // Read CLIDR
        ANDS    r3, r0, #0x07000000        // Extract coherency level
        MOV     r3, r3, LSR #23            // Total cache levels << 1
        BEQ     Finished                   // If 0, no need to clean

        MOV     r10, #0                    // R10 holds current cache level << 1
Loop1:
        ADD     r2, r10, r10, LSR #1       // R2 holds cache "Set" position
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

Loop2:
        MOV     r9, r4                     // R9 working copy of the max way size (right aligned)

Loop3:
        ORR     r11, r10, r9, LSL r5       // Factor in the Way number and cache number into R11
        ORR     r11, r11, r7, LSL r2       // Factor in the Set number
        MCR     p15, 0, r11, c7, c6, 2     // Invalidate by Set/Way
        SUBS    r9, r9, #1                 // Decrement the Way number
        BGE     Loop3
        SUBS    r7, r7, #1                 // Decrement the Set number
        BGE     Loop2
Skip:
        ADD     r10, r10, #2               // Increment the cache number
        CMP     r3, r10
        BGT     Loop1

Finished:


//----------------------------------------------------------------
// MMU Configuration
// Set translation table base
//----------------------------------------------------------------

        // write the address of our page table base to TTBR0 register 0
        LDR     r0,=__pagetable_start
        MCR     p15, 0, r0, c2, c0, 0      // TTBR0

        //
        // Set up some memory attributes
        //
        // These equate to:
        //
        // 0 -> 0b01000100 = 0x00000044 = Normal, Inner/Outer Non-Cacheable
        // 1 -> 0b11111111 = 0x0000ff00 = Normal, Inner/Outer WriteBack Read/Write Allocate
        // 2 -> 0b00000100 = 0x00040000 = Device-nGnRE
        //
        LDR     r0, =0x04ff44
        MCR     p15, 0, r0, c10, c2, 0     // MAIR0

        // Set up TTBCR for the Long-descriptor translation table format (EAE = 1)
        //
        // We're using only TTBR0 (EPD1 = 1), and the page table entries:
        //  - are using an 8-bit ASID from TTBR0
        //  - are outer-shareable (SH0 = 0b10)
        //  - are using Inner & Outer WBWA Normal memory ([IO]RGN0 = 0b01)
        //  - map
        //      + 32 bits of VA space (T0SZ = 0x00)
        //      + into a 40-bit PA space
        //
        //  32   28   24   20   16   12    8    4    0
        //    +----+----+----+----+----+----+----+----+
        //    |    |OOII|    |    |    |OOII|    |    |
        //    |    |RRRR|E   |  TT|    |RRRR|E T |   T|
        //    |E SS|GGGG|P   |  11|  SS|GGGG|P 0 |   0|
        //    |A HH|NNNN|DA  |  SS|  HH|NNNN|D S |   S|
        //    |E 11|1111|11  |  ZZ|  00|0000|0 Z-|---Z|
        //
        //     1000 0000 1000 0000 0010 0101 0000 0000
        //
        //   0x    8    0    8    0    2    5    0    0
        //
        // Note: the ISB is needed to ensure the changes to system
        //       context are before the write of SCTLR_EL1.M to enable
        //       the MMU. It is likely on a "real" implementation that
        //       this setup would work without an ISB, due to the
        //       amount of code that gets executed before enabling the
        //       MMU, but that would not be architecturally correct.
        //
        LDR     r0, =0x80802500
        MCR     p15, 0, r0, c2, c0, 2      // TTBCR
        ISB


//----------------------------------------------------------------
// PAGE TABLE generation
//
// There are 4 level one table entries addressing 1GB each
// The first will be a Block entry marked as Device memory for peripherals (if any)
// The next invalid
// The next a Block entry marked as Normal for RAM where the code/stack/heap is
// The next invalid
//
//         |63                                                                             1|0|
//         +--------------------------------------------------------------------------------+-+
// Invalid |................................................................................|0|
//
//
//         |63                  52|51     39|40           n|n-1  12|11                   2|1|0|
//         +----------------------+---------+--------------+-------+----------------------+-+-+
// Block   |Upper block attributes|  res0   |Output address| res0  |Lower block attributes|0|1|
//
//
//         |     63|62   61|     60|      59|58 52|51 40|39                        12|11 2|1|0|
//         +-------+-------+-------+--------+-----+-----+----------------------------+----+-+-+
// Table   |NSTable|APTable|XNTable|PXNTable|.....|res0 |Next level table addr[39:12]|....|1|1|
//
//
//                Upper attributes                                    Lower attributes
//         |63 59|58 55|54| 53|        52|            |11|10|9     8|7     6| 5|4            2|
//         +-----+-----+--+---+----------+            +--+--+-------+-------+--+--------------+
//         |.....|.....|XN|PXN|Contiguous|            |nG|AF|SH[1:0]|AP[2:1]|NS|AttrIndex[2:0]|
//
//  where ... is ignored, n is 30 for level 1 or 21 for level 2
//----------------------------------------------------------------

        LDR     r0,=__pagetable_start
        MOV     r2, #0

        // Device memory
        LDR     r3,=((0 << 30) | (0b0111010010 << 2) | 0b01) // Output address + Lower attributes + Block tag
        STR     r3, [r0, #0]
        LDR     r4,=(0b100 << 20)                // Upper attributes
        STR     r4, [r0, #4]

        // Mark entry invalid by setting bit 0 to 0
        STR     r2, [r0, #8]
        // Upper word ignored

        // RAM
        LDR     r3,=((2 << 30) | (0b0111010001 << 2) | 0b01) // Output address + Lower attributes + block tag
        STR     r3, [r0, #16]
        LDR     r4,=(0b000 << 20)                // Upper attributes
        STR     r4, [r0, #20]

        // Mark entry invalid by setting bit 0 to 0
        STR     r2, [r0, #24]
        // Upper word ignored

        // Ensure all the entry writes are complete before enabling the MMU
        DSB     ISH


//----------------------------------------------------------------
// Setup domain control register - Enable all domains to client mode
//----------------------------------------------------------------

        MRC     p15, 0, r0, c3, c0, 0      // Read Domain Access Control Register
        LDR     r0, =0x55555555            // Initialize every domain entry to b01 (client)
        MCR     p15, 0, r0, c3, c0, 0      // Write Domain Access Control Register

#if defined(__ARM_NEON) || defined(__ARM_FP)
//----------------------------------------------------------------
// Enable access to NEON/VFP by enabling access to Coprocessors 10 and 11.
// Enables Full Access i.e. in both privileged and non privileged modes
//----------------------------------------------------------------

        MRC     p15, 0, r0, c1, c0, 2      // Read Coprocessor Access Control Register (CPACR)
        ORR     r0, r0, #(0xF << 20)       // Enable access to CP 10 & 11
        MCR     p15, 0, r0, c1, c0, 2      // Write Coprocessor Access Control Register (CPACR)
        ISB

//----------------------------------------------------------------
// Switch on the VFP and NEON hardware
//----------------------------------------------------------------

        MOV     r0, #0x40000000
        VMSR    FPEXC, r0                   // Write FPEXC register, EN bit set
#endif


//----------------------------------------------------------------
// Enable MMU and branch to _start
// Leaving the caches disabled until after scatter loading.
//----------------------------------------------------------------

        LDR     r12,=_start

        MRC     p15, 0, r0, c1, c0, 0      // Read System Control Register
        BIC     r0, r0, #(0x1 << 12)       // Clear I bit 12 to disable I Cache
        BIC     r0, r0, #(0x1 <<  2)       // Clear C bit  2 to disable D Cache
        BIC     r0, r0, #0x2               // Clear A bit  1 to disable strict alignment fault checking
        ORR     r0, r0, #0x1               // Set M bit 0 to enable MMU before scatter loading
        MCR     p15, 0, r0, c1, c0, 0      // Write System Control Register
        ISB

// Now the MMU is enabled, virtual to physical address translations will occur. This will affect the next
// instruction fetch.
//
// The two instructions currently in the pipeline will have been fetched before the MMU was enabled.
// The branch to _start is safe because the Virtual Address (VA) is the same as the Physical Address (PA)
// (flat mapping) of this code that enables the MMU and performs the branch

        BX      r12                        // Branch to _start C library entry point



//----------------------------------------------------------------
// Enable caches
// This code must be run from a privileged mode
//----------------------------------------------------------------

    .section  ENABLECACHES,"ax"
    .align 3

    .global enable_caches
    .type enable_caches, "function"
    .cfi_startproc
enable_caches:

        MRC     p15, 0, r0, c1, c0, 0      // Read System Control Register
        ORR     r0, r0, #(0x1 << 12)       // Set I bit 12 to enable I Cache
        ORR     r0, r0, #(0x1 << 2)        // Set C bit  2 to enable D Cache
        MCR     p15, 0, r0, c1, c0, 0      // Write System Control Register
        ISB

        BX      lr
    .cfi_endproc

