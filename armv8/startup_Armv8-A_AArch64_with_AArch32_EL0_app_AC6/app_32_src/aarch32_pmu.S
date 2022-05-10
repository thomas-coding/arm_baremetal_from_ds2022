// -------------------------------------------------------------------
// Performance Monitoring Unit (PMU) Example Code for AArch32 Cortex-A
// -------------------------------------------------------------------

// Copyright (c) 2010-2019 Arm Limited (or its affiliates). All rights reserved.
// Use, modification and redistribution of this file is subject to your possession of a
// valid End User License Agreement for the Arm Product of which these examples are part of
// and your compliance with all applicable terms and conditions of such licence agreement.


  .text

   .cfi_sections .debug_frame  // put stack frame info into .debug_frame instead of .eh_frame
// ------------------------------------------------------------
// Performance Monitor Block
// ------------------------------------------------------------
    .global getPMN
    .type getPMN, "function"
    .cfi_startproc
getPMN:
  // Returns the number of progammable counters
  // uint32_t getPMN(void)
  MRC     p15, 0, r0, c9, c12, 0 // Read PMCR Register
  MOV     r0, r0, LSR #11        // Shift N field down to bit 0
  AND     r0, r0, #0x1F          // Mask to leave just the 5 N bits
  BX      lr
   .cfi_endproc


  // Sets the event for a programmable counter to record
  // void pmn_config(unsigned counter, uint32_t event)
  // counter (in r0) = Which counter to program (e.g. 0 for PMN0, 1 for PMN1)
  // event   (in r1) = The event code (from appropriate TRM or Arm Architecture Reference Manual)
  
    .global pmn_config
    .type pmn_config, "function"
    .cfi_startproc
pmn_config:
  AND     r0, r0, #0x1F          // Mask to leave only bits 4:0
  MCR     p15, 0, r0, c9, c12, 5 // Write PMSELR Register
  ISB                            // Synchronize context
  MCR     p15, 0, r1, c9, c13, 1 // Write PMXEVTYPER Register
  BX      lr
 .cfi_endproc



  // Enables/disables the divider (1/64) on CCNT
  // void ccnt_divider(int divider)
  // divider (in r0) = If 0 disable divider, else enable divider
    .global ccnt_divider
    .type ccnt_divider, "function"
    .cfi_startproc
ccnt_divider:
  MRC     p15, 0, r1, c9, c12, 0  // Read PMCR

  CMP     r0, #0x0                // IF (r0 == 0)
  BICEQ   r1, r1, #0x08           // THEN: Clear the D bit (disables the divisor)
  ORRNE   r1, r1, #0x08           // ELSE: Set the D bit (enables the divisor)

  MCR     p15, 0, r1, c9, c12, 0  // Write PMCR
  BX      lr
   .cfi_endproc

  // ---------------------------------------------------------------
  // Enable/Disable
  // ---------------------------------------------------------------

  // Global PMU enable
  // void enable_pmu(void)
     .global enable_pmu
    .type enable_pmu, "function"
    .cfi_startproc
enable_pmu: 
  
  MRC     p15, 0, r0, c9, c12, 0  // Read PMCR
  ORR     r0, r0, #0x01           // Set E bit
  MCR     p15, 0, r0, c9, c12, 0  // Write PMCR
  BX      lr
 .cfi_endproc



  // Global PMU disable
  // void disable_pmu(void)
  
    .global disable_pmu
    .type disable_pmu, "function"
    .cfi_startproc
disable_pmu:
  MRC     p15, 0, r0, c9, c12, 0  // Read PMCR
  BIC     r0, r0, #0x01           // Clear E bit
  MCR     p15, 0, r0, c9, c12, 0  // Write PMCR
  BX      lr
 .cfi_endproc



  // Enable the CCNT
  // void enable_ccnt(void)
    .global enable_ccnt
    .type enable_ccnt, "function"
    .cfi_startproc
enable_ccnt: 
  MOV     r0, #0x80000000         // Set C bit
  MCR     p15, 0, r0, c9, c12, 1  // Write PMCNTENSET Register
  BX      lr
 .cfi_endproc



  // Disable the CCNT
  // void disable_ccnt(void)
    .global disable_ccnt
    .type disable_ccnt, "function"
    .cfi_startproc
disable_ccnt: 
  MOV     r0, #0x80000000         // Set C bit
  MCR     p15, 0, r0, c9, c12, 2  // Write PMCNTENCLR Register
  BX      lr
   .cfi_endproc



  // Enable PMN{n}
  // void enable_pmn(uint32_t counter)
  // counter (in r0) = The counter to enable (e.g. 0 for PMN0, 1 for PMN1)
    .global enable_pmn
    .type enable_pmn, "function"
    .cfi_startproc
enable_pmn: 
  MOV     r1, #0x1
  MOV     r1, r1, LSL r0
  MCR     p15, 0, r1, c9, c12, 1  // Write PMCNTENSET Register
  BX      lr
   .cfi_endproc



  // Disable PMN{n}
  // void disable_pmn(uint32_t counter)
  // counter (in r0) = The counter to disable (e.g. 0 for PMN0, 1 for PMN1)
    .global disable_pmn
    .type disable_pmn, "function"
    .cfi_startproc
disable_pmn:
  MOV     r1, #0x1
  MOV     r1, r1, LSL r0
  MCR     p15, 0, r1, c9, c12, 2  // Write PMCNTENCLR Register
  BX      lr
   .cfi_endproc



  // Enables User mode access to the PMU (must be called in a privileged mode)
  // void enable_pmu_user_access(void)
    .global enable_pmu_user_access
    .type enable_pmu_user_access, "function"
    .cfi_startproc
enable_pmu_user_access: 
  MRC     p15, 0, r0, c9, c14, 0  // Read PMUSERENR Register
  ORR     r0, r0, #0x01           // Set EN bit (bit 0)
  MCR     p15, 0, r0, c9, c14, 0  // Write PMUSERENR Register
  ISB                             // Synchronize context
  BX      lr
   .cfi_endproc


  // Disables User mode access to the PMU (must be called in a privileged mode)
  // void disable_pmu_user_access(void)
      .global disable_pmu_user_access
    .type disable_pmu_user_access, "function"
    .cfi_startproc
disable_pmu_user_access:
  MRC     p15, 0, r0, c9, c14, 0  // Read PMUSERENR Register
  BIC     r0, r0, #0x01           // Clear EN bit (bit 0)
  MCR     p15, 0, r0, c9, c14, 0  // Write PMUSERENR Register
  ISB                             // Synchronize context
  BX      lr
   .cfi_endproc


  // ---------------------------------------------------------------
  // Counter read registers
  // ---------------------------------------------------------------


  // Returns the value of CCNT
  // uint32_t read_ccnt(void)
    .global read_ccnt
    .type read_ccnt, "function"
    .cfi_startproc
read_ccnt:   
  MRC     p15, 0, r0, c9, c13, 0 // Read CCNT Register
  BX      lr
   .cfi_endproc



  // Returns the value of PMN{n}
  // uint32_t read_pmn(uint32_t counter)
  // counter (in r0) = The counter to read (e.g. 0 for PMN0, 1 for PMN1)
      .global read_pmn
    .type read_pmn, "function"
    .cfi_startproc
read_pmn: 
  AND     r0, r0, #0x1F          // Mask to leave only bits 4:0
  MCR     p15, 0, r0, c9, c12, 5 // Write PMSELR Register
  ISB                            // Synchronize context
  MRC     p15, 0, r0, c9, c13, 2 // Read current PMNx Register
  BX      lr
   .cfi_endproc


  // ---------------------------------------------------------------
  // Software Increment
  // ---------------------------------------------------------------

 
  // Writes to software increment register
  // void pmu_software_increment(uint32_t counter)
  // counter (in r0) = The counter to increment (e.g. 0 for PMN0, 1 for PMN1)
      .global pmu_software_increment
    .type pmu_software_increment, "function"
    .cfi_startproc
pmu_software_increment: 
  MOV     r1, #0x01
  MOV     r1, r1, LSL r0
  MCR     p15, 0, r1, c9, c12, 4 // Write PMSWINCR Register
  BX      lr
   .cfi_endproc


  // ---------------------------------------------------------------
  // Overflow & Interrupt Generation
  // ---------------------------------------------------------------


  // Returns the value of the overflow flags
  // uint32_t read_flags(void)
      .global read_flags
    .type read_flags, "function"
    .cfi_startproc
read_flags: 
  MRC     p15, 0, r0, c9, c12, 3 // Read PMOVSR Register
  BX      lr
 .cfi_endproc



  // Writes the overflow flags
  // void write_flags(uint32_t flags)
      .global write_flags
    .type write_flags, "function"
    .cfi_startproc
write_flags: 
  MCR     p15, 0, r0, c9, c12, 3 // Write PMOVSR Register
  ISB                            // Synchronize context
  BX      lr
 .cfi_endproc



  // Enables interrupt generation on overflow of the CCNT
  // void enable_ccnt_irq(void)
      .global enable_ccnt_irq
    .type enable_ccnt_irq, "function"
    .cfi_startproc
enable_ccnt_irq: 
  MOV     r0, #0x80000000
  MCR     p15, 0, r0, c9, c14, 1  // Write PMINTENSET Register
  BX      lr
   .cfi_endproc


 
  // Disables interrupt generation on overflow of the CCNT
  // void disable_ccnt_irq(void)
      .global disable_ccnt_irq
    .type disable_ccnt_irq, "function"
    .cfi_startproc
disable_ccnt_irq: 
  MOV     r0, #0x80000000
  MCR     p15, 0, r0, c9, c14, 2   // Write PMINTENCLR Register
  BX      lr
   .cfi_endproc



  // Enables interrupt generation on overflow of PMN{x}
  // void enable_pmn_irq(uint32_t counter)
  // counter (in r0) = The counter to enable the interrupt for (e.g. 0 for PMN0, 1 for PMN1)
      .global enable_pmn_irq
    .type enable_pmn_irq, "function"
    .cfi_startproc
enable_pmn_irq:
  MOV     r1, #0x1
  MOV     r0, r1, LSL r0
  MCR     p15, 0, r0, c9, c14, 1   // Write PMINTENSET Register
  BX      lr
   .cfi_endproc



  // Disables interrupt generation on overflow of PMN{x}
  // void disable_pmn_irq(uint32_t counter)
  // counter (in r0) = The counter to disable the interrupt for (e.g. 0 for PMN0, 1 for PMN1)
      .global disable_pmn_irq
    .type disable_pmn_irq, "function"
    .cfi_startproc
disable_pmn_irq:
  MOV     r1, #0x1
  MOV     r0, r1, LSL r0
  MCR     p15, 0, r0, c9, c14, 2  // Write PMINTENCLR Register
  BX      lr
 .cfi_endproc


  // ---------------------------------------------------------------
  // Reset Functions
  // ---------------------------------------------------------------


  // Resets all programmable counters to zero
  // void reset_pmn(void)
      .global reset_pmn
    .type reset_pmn, "function"
    .cfi_startproc
reset_pmn:
  MRC     p15, 0, r0, c9, c12, 0  // Read PMCR
  ORR     r0, r0, #0x2            // Set P bit (Event counter reset)
  MCR     p15, 0, r0, c9, c12, 0  // Write PMCR
  BX      lr
 .cfi_endproc



  // Resets the CCNT
  // void reset_ccnt(void)
      .global reset_ccnt
    .type reset_ccnt, "function"
    .cfi_startproc
reset_ccnt: 
  MRC     p15, 0, r0, c9, c12, 0  // Read PMCR
  ORR     r0, r0, #0x4            // Set C bit (Clock counter reset)
  MCR     p15, 0, r0, c9, c12, 0  // Write PMCR
  BX      lr
   .cfi_endproc

 
