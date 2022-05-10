// ------------------------------------------------------------
// Cortex-R8 MPCore SMP Prime Number Generator Example
//
// Copyright (c) 2011-2021 Arm Limited (or its affiliates). All rights reserved.
// Use, modification and redistribution of this file is subject to your possession of a
// valid End User License Agreement for the Arm Product of which these examples are part of 
// and your compliance with all applicable terms and conditions of such licence agreement.
// ------------------------------------------------------------

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
#define Device_Shared  0x01

#define Full_Access 0b011
#define Read_Only   0b110

#define Shared 0x4
  .eabi_attribute Tag_ABI_align8_preserved,1

  .section VECTORS, "ax"
  .align 3
  .cfi_sections .debug_frame  // put stack frame info into .debug_frame instead of .eh_frame


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


//----------------------------------------------------------------
// Entry point for the Reset handler
//----------------------------------------------------------------

    .global Start

Start:

//----------------------------------------------------------------
// Exception Vector Table
//----------------------------------------------------------------


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
  .global __main
  .global main_app

  .global __use_two_region_memory
  .global Image$$ARM_LIB_STACK$$ZI$$Limit
  .global Image$$IRQ_STACKS$$ZI$$Limit


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



  //
  // Setup stacks
  //---------------

  MRC     p15, 0, r0, c0, c0, 5       // Read CPU ID register
  ANDS    r0, r0, #0x03               // Mask off, leaving the CPU ID field

  MSR     CPSR_c, #Mode_IRQ | I_Bit | F_Bit
  LDR     r1, =Image$$IRQ_STACKS$$ZI$$Limit
  SUB     r1, r1, r0, LSL #8          // 256 bytes of IRQ stack per CPU - see scatter.scat
  MOV     sp, r1

  MSR     CPSR_c, #Mode_SYS | I_Bit | F_Bit    // Interrupts initially disabled
  LDR     r1, =Image$$ARM_LIB_STACK$$ZI$$Limit
  SUB     r1, r1, r0, LSL #12         // 0x1000 bytes of App stack per CPU - see scatter.scat
  MOV     sp, r1

  //
  // Invalidate caches
  // ------------------
  BL      invalidateCaches


  // Clear Branch Prediction Array
  // ------------------------------
  MOV     r0, #0x0
  MCR     p15, 0, r0, c7, c5, 6     // BPIALL - Invalidate entire branch predictor array
  
  
  //
  // Activate VFP/NEON, if required
  //-------------------------------

#if defined (__ARM_FP)

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

  BL      configMPU

  //
  // SMP initialization
  // -------------------
  MRC     p15, 0, r0, c0, c0, 5     // Read CPU ID register
  ANDS    r0, r0, #0x03             // Mask off, leaving the CPU ID field
  BEQ     primaryCPUInit
  BNE     secondaryCPUsInit


//----------------------------------------------------------------
// MPU Configuration
//----------------------------------------------------------------
   
   // Notes:
   // * Regions apply to both instruction and data accesses.
   // * Each region base address must be a multiple of its size
   // * Any address range not covered by an enabled region will abort
   // * The region at 0x0 over the Vector table is needed to support semihosting
   
   // Region 0: Init          Base = 0x48000000  Size = 64KB   Normal  Non-shared  Full access	Executable
   // Region 1: Code          Base = 0x48010000  Size = 32KB   Normal  Non-shared  Full access	Executable
   // Region 2: Data          Base = 0x48018000  Size = 16KB   Normal  Non-shared  Full access	Not Executable
   // Region 3: Stack/Heap    Base = 0x4801C000  Size =  8KB   Normal  Non-shared  Full access  Not Executable
   // Region 4: Vectors       Base = 0x0000      Size = 64KB   Normal  Non-shared  Full access	Executable
   // Region 5: GIC           Base = 0xAE000000  Size =  8KB   Device  Shared      Full access  Not Executable  

  .global configMPU
  .type configMPU, "function"
configMPU:

    MRC     p15, 0, r0, c0, c0, 5       // Read CPU ID register
    ANDS    r0, r0, #0x03               // Mask off, leaving the CPU ID field
		// Import linker symbols to get region base addresses
	.global  Image$$INIT$$Base
	.global  Image$$CODE$$Base
	.global  Image$$DATA$$Base
	.global  Image$$ARM_LIB_STACK$$Base

	// Region 0 - Init
		MOV 	r1, #0
		MCR 	p15, 0, r1, c6, c2, 0		// Set memory region number register
		ISB 								// Ensure subsequent insts execute wrt this region
		LDR 	r2, =Image$$INIT$$Base
		MCR 	p15, 0, r2, c6, c1, 0		// Set region base address register
		LDR 	r2, =0x0  |  (Region_64K << 1)	|  Region_Enable
		MCR 	p15, 0, r2, c6, c1, 2		// Set region size & enable register
		LDR 	r2, =0x0  |  (Full_Access << 8)	|  Normal_nShared
		MCR 	p15, 0, r2, c6, c1, 4		// Set region access control register

		// Region 1 - Code
		ADD 	r1, r1, #1
		MCR 	p15, 0, r1, c6, c2, 0		// Set memory region number register
		ISB 								// Ensure subsequent insts execute wrt this region
		LDR 	r2, =Image$$CODE$$Base
		MCR 	p15, 0, r2, c6, c1, 0		// Set region base address register
		LDR 	r2, =0x0  |  (Region_32K << 1)	|  Region_Enable
		MCR 	p15, 0, r2, c6, c1, 2		// Set region size & enable register
		LDR 	r2, =0x0  |  (Full_Access << 8)	|  Normal_nShared
		MCR 	p15, 0, r2, c6, c1, 4		// Set region access control register

		// Region 2 - Data
		ADD 	r1, r1, #1
		MCR 	p15, 0, r1, c6, c2, 0		// Set memory region number register
		ISB 								// Ensure subsequent insts execute wrt this region
		LDR 	r2, =Image$$DATA$$Base
		MCR 	p15, 0, r2, c6, c1, 0		// Set region base address register
		LDR 	r2, =0x0  |  (Region_16K << 1)	|  Region_Enable
		MCR 	p15, 0, r2, c6, c1, 2		// Set region size & enable register
		LDR 	r2, =0x0  |  (Full_Access << 8) |  Normal_nShared	|  Execute_Never
		MCR 	p15, 0, r2, c6, c1, 4		// Set region access control register

		// Region 3 - Stack/heap
		ADD 	r1, r1, #1
		MCR 	p15, 0, r1, c6, c2, 0		// Set memory region number register
		ISB 								// Ensure subsequent insts execute wrt this region
		LDR 	r2, =Image$$ARM_LIB_STACK$$Base
		MCR 	p15, 0, r2, c6, c1, 0		// Set region base address register
		LDR 	r2, =0x0  |  (Region_16K << 1)	|  Region_Enable
		MCR 	p15, 0, r2, c6, c1, 2		// Set region size & enable register
		LDR 	r2, =0x0  |  (Full_Access << 8) |  Normal_nShared	|  Execute_Never
		MCR 	p15, 0, r2, c6, c1, 4		// Set region access control register

		// Region 4 - Vectors
		ADD 	r1, r1, #1
		MCR 	p15, 0, r1, c6, c2, 0		// Set memory region number register
		ISB 								// Ensure subsequent insts execute wrt this region
		LDR 	r2, =0
		MCR 	p15, 0, r2, c6, c1, 0		// Set region base address register
		LDR 	r2, =0x0  |  (Region_64K  << 1)	|  Region_Enable
		MCR 	p15, 0, r2, c6, c1, 2		// Set region size & enable register
		LDR 	r2, =0x0  |  (Full_Access << 8) |  Normal_nShared  |  Shared
		MCR 	p15, 0, r2, c6, c1, 4		// Set region access control register

		// Region 5 - GIC
		ADD 	r1, r1, #1
		MCR 	p15, 0, r1, c6, c2, 0		// Set memory region number register
		ISB 								// Ensure subsequent insts execute wrt this region
		MRC     p15, 4, r2, c15, c0, 0		// Read periph base address (=0xAE000000 for Cortex-R8 FVP)
		MCR 	p15, 0, r2, c6, c1, 0		// Set region base address register
		LDR 	r2, =0x0  |  (Region_8K  << 1)	|  Region_Enable
		MCR 	p15, 0, r2, c6, c1, 2		// Set region size & enable register
		LDR 	r2, =0x0  |  (Full_Access << 8) |  Device_Shared  |  Execute_Never
		MCR 	p15, 0, r2, c6, c1, 4		// Set region access control register


		// Disable all higher priority regions (assumes unified regions, which is always true for Cortex-R8)
		MRC 	p15, 0, r0, c0, c0, 4		// Read MPU Type register (MPUIR)
		LSR 	r0, r0, #8
		AND 	r0, r0, #0xff				// r0 = Number of MPU regions (12, 16, 20, or 24 for Cortex-R8)
		MOV 	r2, #0						// Value to write to disable region
region_loop:
		ADD 	r1, r1, #1
		CMP 	r0, r1
		BLS 	regions_done
		MCR 	p15, 0, r1, c6, c2, 0		// Set memory region number register (RGNR)
		MCR 	p15, 0, r2, c6, c1, 2		// Set region size & enable register (DRSR)
		B		region_loop
regions_done:
		BX 		lr

// ------------------------------------------------------------
// Initialization for PRIMARY CPU
// ------------------------------------------------------------

  .global primaryCPUInit
  .type primaryCPUInit, "function"
primaryCPUInit:



  // Enable the SCU
  // ---------------
  BL      enableSCU

  //
  // Join SMP
  // ---------
  MOV     r0, #0x0                  // Move CPU ID into r0
  MOV     r1, #0xF                  // Move 0xF (represents all four ways) into r1
  BL      joinSMP


  //
  // GIC Init
  // ---------
  BL      enableGIC
  BL      enableGICProcessorInterface

  // Enable MPU
  // -----------
  // Leave the caches disabled until after scatter loading.
  MRC     p15, 0, r0, c1, c0, 0       // Read System Control Register
  ORR     r0, r0, #0x1                // Set M bit 0 to enable MMU before scatter loading
  MCR     p15, 0, r0, c1, c0, 0       // Write System Control Register

  //
  // Branch to C lib code
  // ----------------------
  B       __main



// ------------------------------------------------------------
// Initialization for SECONDARY CPUs
// ------------------------------------------------------------

  .global secondaryCPUsInit
  .type secondaryCPUsInit, "function"
secondaryCPUsInit:

  BL      configMPU

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


  // Enable MPU
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
