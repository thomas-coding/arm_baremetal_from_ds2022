// ------------------------------------------------------------
// Cortex-A9 MPCore SMP Prime Number Generator Example
//
// Copyright (c) 2011-2018 Arm Limited (or its affiliates). All rights reserved.
// Use, modification and redistribution of this file is subject to your possession of a
// valid End User License Agreement for the Arm Product of which these examples are part of 
// and your compliance with all applicable terms and conditions of such licence agreement.
// ------------------------------------------------------------


  .section StartUp, "ax"
  .balign 0x20


// Standard definitions of mode bits and interrupt (I&F) flags in PSRs

#define Mode_USR   0x10
#define Mode_FIQ   0x11
#define Mode_IRQ   0x12
#define Mode_SVC   0x13
#define Mode_ABT   0x17
#define Mode_UND   0x1B
#define Mode_SYS   0x1F

#define I_Bit 0x80 // when I bit is set, IRQ is disabled
#define F_Bit 0x40 // when F bit is set, FIQ is disabled


// ------------------------------------------------------------
// Porting defines
// ------------------------------------------------------------

#define L1_COHERENT    0x00014c06 // Template descriptor for coherent memory
#define L1_NONCOHERENT 0x00000c1e // Template descriptor for non-coherent memory
#define L1_DEVICE      0x00000c16 // Template descriptor for device memory

// ------------------------------------------------------------


  .global Vectors

Vectors:
  B      Reset_Handler
  B      Undefined_Handler
  B      SVC_Handler
  B      Prefetch_Handler
  B      Abort_Handler
  B .    //Reserved vector
  B      IRQ_Handler
  B      FIQ_Handler

// ------------------------------------------------------------
// Handlers for unused exceptions
// ------------------------------------------------------------

Undefined_Handler:
  B       Undefined_Handler
SVC_Handler:
  B       SVC_Handler
Prefetch_Handler:
  B       Prefetch_Handler
Abort_Handler:
  B       Abort_Handler
FIQ_Handler:
  B       FIQ_Handler

// ------------------------------------------------------------
// Imports
// ------------------------------------------------------------
  .global readIntAck
  .global writeEOI
  .global enableGIC
  .global enableGICProcessorInterface
  .global setPriorityMask
  .global enableIntID
  .global setIntPriority
  .global enableSCU
  .global joinSMP
  .global secureSCUInvalidate
  .global enableMaintenanceBroadcast
  .global invalidateCaches
  .global disableHighVecs
  .global _start
  .global main_app


// ------------------------------------------------------------
// Interrupt Handler
// ------------------------------------------------------------

  .global IRQ_Handler
  .type IRQ_Handler, "function"
IRQ_Handler:
  SUB     lr, lr, #4          // Pre-adjust lr
  SRSFD   sp!, #Mode_IRQ      // Save lr and SPSR to IRQ mode stack
  PUSH    {r0-r4, r12}        // Save APCS corruptible registers to IRQ mode stack (and maintain 8 byte alignment)

  // Acknowledge the interrupt
  BL      readIntAck
  MOV     r4, r0

  //
  // This example only uses (and enables) one.  At this point
  // you would normally check the ID, and clear the source.
  //

  // Write end of interrupt reg
  MOV     r0, r4
  BL      writeEOI

  POP     {r0-r4, r12}        // Restore stacked APCS registers
  MOV     r10, #1             // Set exit flag so this CPU will leave holding pen after returning from interrupt
  RFEFD   sp!                 // Return from exception



// ------------------------------------------------------------
// Reset Handler - Generic initialization, run by all CPUs
// ------------------------------------------------------------

  .global Reset_Handler
  .type Reset_Handler, "function"
Reset_Handler:

//
// Disable caches, MMU and branch prediction in case they were left enabled from an earlier run
// This does not need to be done from a cold reset
// ------------------------------------------------------------
  MRC     p15, 0, r0, c1, c0, 0       // Read System Control Register
  BIC     r0, r0, #(0x1 << 12)        // Clear I, bit 12, to disable I Cache
  BIC     r0, r0, #(0x1 << 11)        // Clear Z, bit 11, to disable branch prediction
  BIC     r0, r0, #(0x1 <<  2)        // Clear C, bit  2, to disable D Cache
  BIC     r0, r0, #(0x1 <<  1)        // Clear A, bit  1, to disable strict alignment fault checking
  BIC     r0, r0, #0x1                // Clear M, bit  0, to disable MMU
  MCR     p15, 0, r0, c1, c0, 0       // Write System Control Register
  ISB

// The MMU is enabled later, before calling main().  Caches and branch prediction are enabled inside main(),
// after the MMU has been enabled and scatterloading has been performed.

  //
  // Setup stacks
  //---------------

  MRC     p15, 0, r0, c0, c0, 5       // Read CPU ID register
  ANDS    r0, r0, #0x03               // Mask off, leaving the CPU ID field

  MSR     CPSR_c, #Mode_IRQ | I_Bit | F_Bit
  LDR     r1, =__irq_stack
  SUB     r1, r1, r0, LSL #8          // 256 bytes of IRQ stack per CPU - see gcc.ld
  MOV     sp, r1

  MSR     CPSR_c, #Mode_SYS | I_Bit | F_Bit    // Interrupts initially disabled
  LDR     r1, =__stack
  SUB     r1, r1, r0, LSL #12         // 0x1000 bytes of App stack per CPU - see gcc.ld
  MOV     sp, r1

  //
  // Set Vector Base Address Register (VBAR)
  // ------------------------
  LDR    r0, =Vectors
  MCR    p15, 0, r0, c12, c0, 0

  BL     disableHighVecs // Ensure that V-bit is cleared

  //
  // Invalidate caches
  // ------------------
  BL      invalidateCaches

  //
  // Clear Branch Prediction Array
  // ------------------------------
  MOV     r0, #0x0
  MCR     p15, 0, r0, c7, c5, 6     // BPIALL - Invalidate entire branch predictor array

  //
  // Invalidate TLBs
  //------------------
  MOV     r0, #0x0
  MCR     p15, 0, r0, c8, c7, 0     // TLBIALL - Invalidate entire Unified TLB

  //
  // Set up Domain Access Control Reg
  // ----------------------------------
  // b00 - No Access (abort)
  // b01 - Client (respect table entry)
  // b10 - RESERVED
  // b11 - Manager (ignore access permissions)

  MRC     p15, 0, r0, c3, c0, 0      // Read Domain Access Control Register
  LDR     r0, =0x55555555            // Initialize every domain entry to b01 (client)
  MCR     p15, 0, r0, c3, c0, 0      // Write Domain Access Control Register

  ////
  //// Enable L1 Preloader - Auxiliary Control
  //// -----------------------------------------
  //// Seems to undef on panda?
  //MRC     p15, 0, r0, c1, c0, 1      // Read ACTLR
  //ORR     r0, r0, #0x4
  //MCR     p15, 0, r0, c1, c0, 1      // Write ACTLR
  ISB


  //
  // Set location of level 1 page table
  //------------------------------------
  // 31:14 - Base addr
  // 13:5  - 0x0
  // 4:3   - RGN 0x0 (Outer Noncachable)
  // 2     - P   0x0
  // 1     - S   0x0 (Non-shared)
  // 0     - C   0x0 (Inner Noncachable)
  LDR     r0, =__pagetable_start
  MCR     p15, 0, r0, c2, c0, 0


  //
  // Activate VFP/NEON, if required
  //-------------------------------

#if defined (__ARM_NEON) || defined (__ARM_FP)

  // Enable access to NEON/VFP by enabling access to Coprocessors 10 and 11.
  // Enables Full Access i.e. in both privileged and non privileged modes
      MRC     p15, 0, r0, c1, c0, 2     // Read Coprocessor Access Control Register (CPACR)
      ORR     r0, r0, #(0xF << 20)      // Enable access to CP 10 & 11
      MCR     p15, 0, r0, c1, c0, 2     // Write Coprocessor Access Control Register (CPACR)
      ISB

  // Switch on the VFP and NEON hardware
      MOV     r0, #0x40000000
      VMSR    FPEXC, r0                   // Write FPEXC register, EN bit set

#endif

  //
  // SMP initialization
  // -------------------
  MRC     p15, 0, r0, c0, c0, 5     // Read CPU ID register
  ANDS    r0, r0, #0x03             // Mask off, leaving the CPU ID field
  BEQ     primaryCPUInit
  BNE     secondaryCPUsInit



// ------------------------------------------------------------
// Initialization for PRIMARY CPU
// ------------------------------------------------------------

  .global primaryCPUInit
  .type primaryCPUInit, "function"
primaryCPUInit:

  // Translation tables
  // -------------------
  // The translation tables are generated at boot time.
  // First the table is zeroed.  Then the individual valid
  // entries are written in
  //

  LDR     r0, =__pagetable_start

  // Fill table with zeros
  MOV     r2, #1024                 // Set r3 to loop count (4 entries per iteration, 1024 iterations)
  MOV     r1, r0                    // Make a copy of the base dst
  MOV     r3, #0
  MOV     r4, #0
  MOV     r5, #0
  MOV     r6, #0
ttb_zero_loop:
  STMIA   r1!, {r3-r6}              // Store out four entries
  SUBS    r2, r2, #1                // Decrement counter
  BNE     ttb_zero_loop

  //
  // STANDARD ENTRIES
  //

  // Region covering program code and data
  LDR     r1,=__code_start          // Base physical address of program code and data
  LSR     r1,#20                    // Shift right to align to 1MB boundaries
  LDR     r3, =L1_COHERENT          // Descriptor template
  ORR     r3, r1, LSL#20            // Combine address and template
  STR     r3, [r0, r1, LSL#2]       // Store table entry

  // Entry for private address space
  // Needs to be marked as Device memory
  MRC     p15, 4, r1, c15, c0, 0    // Get base address of private address space
  LSR     r1, r1, #20               // Clear bottom 20 bits, to find which 1MB block it is in
  LSL     r2, r1, #2                // Make a copy, and multiply by four.  This gives offset into the page tables
  LSL     r1, r1, #20               // Put back in address format
  LDR     r3, =L1_DEVICE            // Descriptor template
  ORR     r1, r1, r3                // Combine address and template
  STR     r1, [r0, r2]              // Store table entry

  //
  // OPTIONAL ENTRIES
  // You will need additional translations if:
  // - No RAM at zero, so cannot use flat mapping
  // - You wish to retarget
  //
  // If you wish to output to stdio to a UART you will need
  // an additional entry
  //LDR     r1, =PABASE_UART          // Physical address of UART
  //LSR     r1, r1, #20               // Mask off bottom 20 bits to find which 1MB it is within
  //LSL     r2, r1, #2                // Make a copy and multiply by 4 to get table offset
  //LSL     r1, r1, #20               // Put back into address format
  //LDR     r3, =L1_DEVICE            // Descriptor template
  //ORR     r1, r1, r3                // Combine address and template
  //STR     r1, [r0, r2]              // Store table entry

  DSB


  // Enable MMU
  // -----------
  // Leave the caches disabled until after scatter loading.
  MRC     p15, 0, r0, c1, c0, 0       // Read System Control Register
  ORR     r0, r0, #0x1                // Set M bit 0 to enable MMU before scatter loading
  MCR     p15, 0, r0, c1, c0, 0       // Write System Control Register
  ISB


  // Enable the SCU
  // ---------------
  BL      enableSCU

  //
  // Join SMP
  // ---------
  MOV     r0, #0x0                  // Move CPU ID into r0
  MOV     r1, #0xF                  // Move 0xF (represents all four ways) into r1
  BL      secureSCUInvalidate
  BL      joinSMP
  BL      enableMaintenanceBroadcast

  //
  // GIC Init
  // ---------
  BL      enableGIC
  BL      enableGICProcessorInterface

  //
  // Branch to C lib code
  // ----------------------
  B       _start



// ------------------------------------------------------------
// Initialization for SECONDARY CPUs
// ------------------------------------------------------------

  .global secondaryCPUsInit
  .type secondaryCPUsInit, "function"
secondaryCPUsInit:

  //
  // GIC Init
  // ---------
  BL      enableGICProcessorInterface

  MOV     r0, #0x1F                 // Priority
  BL      setPriorityMask

  MOV     r0, #0x0                  // ID
  BL      enableIntID

  MOV     r0, #0x0                  // ID
  MOV     r1, #0x0                  // Priority
  BL      setIntPriority

  //
  // Join SMP
  // ---------
  MRC     p15, 0, r0, c0, c0, 5     // Read CPU ID register
  ANDS    r0, r0, #0x03             // Mask off, leaving the CPU ID field
  MOV     r1, #0xF                  // Move 0xF (represents all four ways) into r1
  BL      secureSCUInvalidate

  BL      joinSMP
  BL      enableMaintenanceBroadcast

  //
  // Holding Pen
  // ------------
  MOV     r10, #0                   // Clear exit flag
  CPSIE   i                         // Enable interrupts
holding_pen:
  CMP     r10, #0                   // Exit flag will be set to 0x1 by IRQ handler on receiving SGI
  DSB                               // Clear all pending data accesses
  WFIEQ
  BEQ     holding_pen
  CPSID   i                         // Disable interrupts


  //
  // The translation tables are generated by the primary CPU
  // The MMU cannot be enabled on the secondary CPUs until
  // they are released from the holding-pen
  //

  // Enable MMU
  // -----------
  // Leave the caches disabled until after scatter loading.
  MRC     p15, 0, r0, c1, c0, 0       // Read System Control Register
  ORR     r0, r0, #0x1                // Set M bit 0 to enable MMU before scatter loading
  MCR     p15, 0, r0, c1, c0, 0       // Write System Control Register
  ISB


  //
  // Branch to application
  // ----------------------
  B       main_app


// ------------------------------------------------------------
// End of startup.s
// ------------------------------------------------------------
