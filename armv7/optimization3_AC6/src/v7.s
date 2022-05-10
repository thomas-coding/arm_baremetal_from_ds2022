// ------------------------------------------------------------
// v7-A Cache and Branch Prediction Maintenance Operations
//
// Copyright (c) 2011-2019 Arm Limited (or its affiliates). All rights reserved.
// Use, modification and redistribution of this file is subject to your possession of a
// valid End User License Agreement for the Arm Product of which these examples are part of 
// and your compliance with all applicable terms and conditions of such licence agreement.
// ------------------------------------------------------------

  .text
  .cfi_sections .debug_frame  // put stack frame info into .debug_frame instead of .eh_frame


// ------------------------------------------------------------
// Interrupt enable/disable
// ------------------------------------------------------------

  // Could use compiler intrinsics instead of these

  .global enableInterrupts
  // void enableInterrupts(void)
  .type enableInterrupts, "function"
  .cfi_startproc  
enableInterrupts:
  CPSIE   i
  BX      lr
  .cfi_endproc


  .global disableInterrupts
  // void disableInterrupts(void)
  .type disableInterrupts, "function"
  .cfi_startproc  
disableInterrupts:
  CPSID   i
  BX      lr
  .cfi_endproc


// ------------------------------------------------------------
// Cache Maintenance
// ------------------------------------------------------------

  .global enableCaches
  // void enableCaches(void)
  .type enableCaches, "function"
  .cfi_startproc
enableCaches:
  MRC     p15, 0, r0, c1, c0, 0   // Read System Control Register
  ORR     r0, r0, #(1 << 2)       // Set C bit
  ORR     r0, r0, #(1 << 12)      // Set I bit
  MCR     p15, 0, r0, c1, c0, 0   // Write System Control Register
  ISB
  BX      lr
  .cfi_endproc



  .global disableCaches
  // void disableCaches(void)
  .type disableCaches, "function"
  .cfi_startproc
disableCaches:
  MRC     p15, 0, r0, c1, c0, 0   // Read System Control Register
  BIC     r0, r0, #(1 << 2)       // Clear C bit
  BIC     r0, r0, #(1 << 12)      // Clear I bit
  MCR     p15, 0, r0, c1, c0, 0   // Write System Control Register
  ISB
  BX      lr
  .cfi_endproc



  .global cleanDCache
  // void cleanDCache(void)
  .type cleanDCache, "function"
  .cfi_startproc
cleanDCache:
  PUSH    {r4-r12}

  //
  // Based on code example given in section 11.2.4 of Armv7-A/R Architecture Reference Manual (DDI 0406B)
  //

  MRC     p15, 1, r0, c0, c0, 1     // Read CLIDR
  ANDS    r3, r0, #0x7000000
  MOV     r3, r3, LSR #23           // Cache level value (naturally aligned)
  BEQ     clean_dcache_finished
  MOV     r10, #0

clean_dcache_loop1:
  ADD     r2, r10, r10, LSR #1      // Work out 3xcachelevel
  MOV     r1, r0, LSR r2            // bottom 3 bits are the Cache type for this level
  AND     r1, r1, #7                // get those 3 bits alone
  CMP     r1, #2
  BLT     clean_dcache_skip         // no cache or only instruction cache at this level
  MCR     p15, 2, r10, c0, c0, 0    // write the Cache Size selection register
  ISB                               // ISB to sync the change to the CacheSizeID reg
  MRC     p15, 1, r1, c0, c0, 0     // reads current Cache Size ID register
  AND     r2, r1, #7                // extract the line length field
  ADD     r2, r2, #4                // add 4 for the line length offset (log2 16 bytes)
  LDR     r4, =0x3FF
  ANDS    r4, r4, r1, LSR #3        // R4 is the max number on the way size (right aligned)
  CLZ     r5, r4                    // R5 is the bit position of the way size increment
  LDR     r7, =0x00007FFF
  ANDS    r7, r7, r1, LSR #13       // R7 is the max number of the index size (right aligned)

clean_dcache_loop2:
  MOV     r9, R4                    // R9 working copy of the max way size (right aligned)

clean_dcache_loop3:
  ORR     r11, r10, r9, LSL r5      // factor in the way number and cache number into R11
  ORR     r11, r11, r7, LSL r2      // factor in the index number
  MCR     p15, 0, r11, c7, c10, 2   // DCCSW - clean by set/way
  SUBS    r9, r9, #1                // decrement the way number
  BGE     clean_dcache_loop3
  SUBS    r7, r7, #1                // decrement the index
  BGE     clean_dcache_loop2

clean_dcache_skip:
  ADD     r10, r10, #2              // increment the cache number
  CMP     r3, r10
  BGT     clean_dcache_loop1

clean_dcache_finished:	
  POP     {r4-r12}

  BX      lr
  .cfi_endproc


  .global cleanInvalidateDCache
  // void cleanInvalidateDCache(void)
  .type cleanInvalidateDCache, "function"
  .cfi_startproc
cleanInvalidateDCache:
  PUSH    {r4-r12}

  //
  // Based on code example given in section 11.2.4 of Armv7-A/R Architecture Reference Manual (DDI 0406B)
  //

  MRC     p15, 1, r0, c0, c0, 1     // Read CLIDR
  ANDS    r3, r0, #0x7000000
  MOV     r3, r3, LSR #23           // Cache level value (naturally aligned)
  BEQ     clean_invalidate_dcache_finished
  MOV     r10, #0

clean_invalidate_dcache_loop1:
  ADD     r2, r10, r10, LSR #1      // Work out 3xcachelevel
  MOV     r1, r0, LSR r2            // bottom 3 bits are the Cache type for this level
  AND     r1, r1, #7                // get those 3 bits alone
  CMP     r1, #2
  BLT     clean_invalidate_dcache_skip // no cache or only instruction cache at this level
  MCR     p15, 2, r10, c0, c0, 0    // write the Cache Size selection register
  ISB                               // ISB to sync the change to the CacheSizeID reg
  MRC     p15, 1, r1, c0, c0, 0     // reads current Cache Size ID register
  AND     r2, r1, #7                // extract the line length field
  ADD     r2, r2, #4                // add 4 for the line length offset (log2 16 bytes)
  LDR     r4, =0x3FF
  ANDS    r4, r4, r1, LSR #3        // R4 is the max number on the way size (right aligned)
  CLZ     r5, r4                    // R5 is the bit position of the way size increment
  LDR     r7, =0x00007FFF
  ANDS    r7, r7, r1, LSR #13       // R7 is the max number of the index size (right aligned)

clean_invalidate_dcache_loop2:
  MOV     r9, R4                    // R9 working copy of the max way size (right aligned)

clean_invalidate_dcache_loop3:
  ORR     r11, r10, r9, LSL r5      // factor in the way number and cache number into R11
  ORR     r11, r11, r7, LSL r2      // factor in the index number
  MCR     p15, 0, r11, c7, c14, 2   // DCCISW - clean and invalidate by set/way
  SUBS    r9, r9, #1                // decrement the way number
  BGE     clean_invalidate_dcache_loop3
  SUBS    r7, r7, #1                // decrement the index
  BGE     clean_invalidate_dcache_loop2

clean_invalidate_dcache_skip:
  ADD     r10, r10, #2              // increment the cache number
  CMP     r3, r10
  BGT     clean_invalidate_dcache_loop1

clean_invalidate_dcache_finished:	
  POP     {r4-r12}

  BX      lr
  .cfi_endproc



  .global invalidateCaches
  // void invalidateCaches(void)
  .type invalidateCaches, "function"
  .cfi_startproc
invalidateCaches:
  PUSH    {r4-r12}

  //
  // Based on code example given in section B2.2.4/11.2.4 of Armv7-A/R Architecture Reference Manual (DDI 0406B)
  //

  MOV     r0, #0
  MCR     p15, 0, r0, c7, c5, 0     // ICIALLU - Invalidate entire I Cache, and flushes branch target cache

  MRC     p15, 1, r0, c0, c0, 1     // Read CLIDR
  ANDS    r3, r0, #0x7000000
  MOV     r3, r3, LSR #23           // Cache level value (naturally aligned)
  BEQ     invalidate_caches_finished
  MOV     r10, #0

invalidate_caches_loop1:
  ADD     r2, r10, r10, LSR #1      // Work out 3xcachelevel
  MOV     r1, r0, LSR r2            // bottom 3 bits are the Cache type for this level
  AND     r1, r1, #7                // get those 3 bits alone
  CMP     r1, #2
  BLT     invalidate_caches_skip    // no cache or only instruction cache at this level
  MCR     p15, 2, r10, c0, c0, 0    // write the Cache Size selection register
  ISB                               // ISB to sync the change to the CacheSizeID reg
  MRC     p15, 1, r1, c0, c0, 0     // reads current Cache Size ID register
  AND     r2, r1, #7                // extract the line length field
  ADD     r2, r2, #4                // add 4 for the line length offset (log2 16 bytes)
  LDR     r4, =0x3FF
  ANDS    r4, r4, r1, LSR #3        // R4 is the max number on the way size (right aligned)
  CLZ     r5, r4                    // R5 is the bit position of the way size increment
  LDR     r7, =0x00007FFF
  ANDS    r7, r7, r1, LSR #13       // R7 is the max number of the index size (right aligned)

invalidate_caches_loop2:
  MOV     r9, R4                    // R9 working copy of the max way size (right aligned)

invalidate_caches_loop3:
  ORR     r11, r10, r9, LSL r5      // factor in the way number and cache number into R11
  ORR     r11, r11, r7, LSL r2      // factor in the index number
  MCR     p15, 0, r11, c7, c6, 2    // DCISW - invalidate by set/way
  SUBS    r9, r9, #1                // decrement the way number
  BGE     invalidate_caches_loop3
  SUBS    r7, r7, #1                // decrement the index
  BGE     invalidate_caches_loop2

invalidate_caches_skip:
  ADD     r10, r10, #2              // increment the cache number
  CMP     r3, r10
  BGT     invalidate_caches_loop1

invalidate_caches_finished:
  POP     {r4-r12}
  BX      lr
  .cfi_endproc



  .global invalidateCaches_IS
  // void invalidateCaches_IS(void)
  .type invalidateCaches_IS, "function"
  .cfi_startproc
invalidateCaches_IS:
  PUSH    {r4-r12}

  MOV     r0, #0
  MCR     p15, 0, r0, c7, c1, 0     // ICIALLUIS - Invalidate entire I Cache inner shareable

  MRC     p15, 1, r0, c0, c0, 1     // Read CLIDR
  ANDS    r3, r0, #0x7000000
  MOV     r3, r3, LSR #23           // Cache level value (naturally aligned)
  BEQ     invalidate_caches_is_finished
  MOV     r10, #0

invalidate_caches_is_loop1:
  ADD     r2, r10, r10, LSR #1      // Work out 3xcachelevel
  MOV     r1, r0, LSR r2            // bottom 3 bits are the Cache type for this level
  AND     r1, r1, #7                // get those 3 bits alone
  CMP     r1, #2
  BLT     invalidate_caches_is_skip // no cache or only instruction cache at this level
  MCR     p15, 2, r10, c0, c0, 0    // write the Cache Size selection register
  ISB                               // ISB to sync the change to the CacheSizeID reg
  MRC     p15, 1, r1, c0, c0, 0     // reads current Cache Size ID register
  AND     r2, r1, #7                // extract the line length field
  ADD     r2, r2, #4                // add 4 for the line length offset (log2 16 bytes)
  LDR     r4, =0x3FF
  ANDS    r4, r4, r1, LSR #3        // R4 is the max number on the way size (right aligned)
  CLZ     r5, r4                    // R5 is the bit position of the way size increment
  LDR     r7, =0x00007FFF
  ANDS    r7, r7, r1, LSR #13       // R7 is the max number of the index size (right aligned)

invalidate_caches_is_loop2:
  MOV     r9, R4                    // R9 working copy of the max way size (right aligned)

invalidate_caches_is_loop3:
  ORR     r11, r10, r9, LSL r5      // factor in the way number and cache number into R11
  ORR     r11, r11, r7, LSL r2      // factor in the index number
  MCR     p15, 0, r11, c7, c6, 2    // DCISW - clean by set/way
  SUBS    r9, r9, #1                // decrement the way number
  BGE     invalidate_caches_is_loop3
  SUBS    r7, r7, #1                // decrement the index
  BGE     invalidate_caches_is_loop2

invalidate_caches_is_skip:
  ADD     r10, r10, #2              // increment the cache number
  CMP     r3, r10
  BGT     invalidate_caches_is_loop1

invalidate_caches_is_finished:
  POP     {r4-r12}
  BX      lr
  .cfi_endproc


// ------------------------------------------------------------
// TLB
// ------------------------------------------------------------

  .global invalidateUnifiedTLB
  // void invalidateUnifiedTLB(void)
  .type invalidateUnifiedTLB, "function"
  .cfi_startproc
invalidateUnifiedTLB:
  MOV     r0, #0
  MCR     p15, 0, r0, c8, c7, 0                 // TLBIALL - Invalidate entire unified TLB
  BX      lr
  .cfi_endproc


  .global invalidateUnifiedTLB_IS
  // void invalidateUnifiedTLB_IS(void)
  .type invalidateUnifiedTLB_IS, "function"
  .cfi_startproc
invalidateUnifiedTLB_IS:
  MOV     r0, #1
  MCR     p15, 0, r0, c8, c3, 0                 // TLBIALLIS - Invalidate entire unified TLB Inner Shareable
  BX      lr
  .cfi_endproc


// ------------------------------------------------------------
// Branch Prediction
// ------------------------------------------------------------

  .global enableBranchPrediction
  // void enableBranchPrediction(void)
  .type enableBranchPrediction, "function"
  .cfi_startproc
enableBranchPrediction:
  MRC     p15, 0, r0, c1, c0, 0                 // Read SCTLR
  ORR     r0, r0, #(1 << 11)                    // Set the Z bit (bit 11)
  MCR     p15, 0, r0, c1, c0, 0                 // Write SCTLR
  ISB
  BX      lr
  .cfi_endproc


  .global disableBranchPrediction
  // void disableBranchPrediction(void)
  .type disableBranchPrediction, "function"
  .cfi_startproc
disableBranchPrediction:
  MRC     p15, 0, r0, c1, c0, 0                 // Read SCTLR
  BIC     r0, r0, #(1 << 11)                    // Clear the Z bit (bit 11)
  MCR     p15, 0, r0, c1, c0, 0                 // Write SCTLR
  ISB
  BX      lr
  .cfi_endproc


  .global flushBranchTargetCache
  // void flushBranchTargetCache(void)
  .type flushBranchTargetCache, "function"
  .cfi_startproc
flushBranchTargetCache:
  MOV     r0, #0
  MCR     p15, 0, r0, c7, c5, 6                 // BPIALL - Invalidate entire branch predictor array
  BX      lr
  .cfi_endproc


  .global flushBranchTargetCache_IS
  // void flushBranchTargetCache_IS(void)
  .type flushBranchTargetCache_IS, "function"
  .cfi_startproc
flushBranchTargetCache_IS:
  MOV     r0, #0
  MCR     p15, 0, r0, c7, c1, 6                 // BPIALLIS - Invalidate entire branch predictor array Inner Shareable
  BX      lr
  .cfi_endproc


// ------------------------------------------------------------
// High Vecs
// ------------------------------------------------------------

  .global enableHighVecs
  // void enableHighVecs(void)
  .type enableHighVecs, "function"
  .cfi_startproc
enableHighVecs:
  MRC     p15, 0, r0, c1, c0, 0 // Read Control Register
  ORR     r0, r0, #(1 << 13)    // Set the V bit (bit 13)
  MCR     p15, 0, r0, c1, c0, 0 // Write Control Register
  ISB
  BX      lr
  .cfi_endproc


  .global disableHighVecs
  // void disable_highvecs(void)
  .type disableHighVecs, "function"
  .cfi_startproc
disableHighVecs:
  MRC     p15, 0, r0, c1, c0, 0 // Read Control Register
  BIC     r0, r0, #(1 << 13)    // Clear the V bit (bit 13)
  MCR     p15, 0, r0, c1, c0, 0 // Write Control Register
  ISB
  BX      lr
  .cfi_endproc


// ------------------------------------------------------------
// Context ID
// ------------------------------------------------------------

  .global getContextID
  // uint32_t getContextIDd(void)
  .type getContextID, "function"
  .cfi_startproc
getContextID:
  MRC     p15, 0, r0, c13, c0, 1 // Read Context ID Register
  BX      lr
  .cfi_endproc


  .global setContextID
  // void setContextID(uint32_t)
  .type setContextID, "function"
  .cfi_startproc
setContextID:
  MCR     p15, 0, r0, c13, c0, 1 // Write Context ID Register
  BX      lr
  .cfi_endproc


// ------------------------------------------------------------
// ID registers
// ------------------------------------------------------------

  .global getMIDR
  // uint32_t getMIDR(void)
  .type getMIDR, "function"
  .cfi_startproc
getMIDR:
  MRC     p15, 0, r0, c0, c0, 0 // Read Main ID Register (MIDR)
  BX      lr
  .cfi_endproc


  .global getMPIDR
  // uint32_t getMPIDR(void)
  .type getMPIDR, "function"
  .cfi_startproc
getMPIDR:
  MRC     p15, 0, r0, c0 ,c0, 5 // Read Multiprocessor ID register (MPIDR)
  BX      lr
  .cfi_endproc


// ------------------------------------------------------------
// CP15 SMP related
// ------------------------------------------------------------

  .global getBaseAddr
  // uint32_t getBaseAddr(void)
  // Returns the value CBAR (base address of the private peripheral memory space)
  .type getBaseAddr, "function"
  .cfi_startproc
getBaseAddr:
  MRC     p15, 4, r0, c15, c0, 0  // Read peripheral base address
  BX      lr
  .cfi_endproc


// ------------------------------------------------------------

  .global getCPUID
  // uint32_t getCPUID(void)
  // Returns the CPU ID (0 to 3) of the CPU executed on
  .type getCPUID, "function"
  .cfi_startproc
getCPUID:
  MRC     p15, 0, r0, c0, c0, 5   // Read CPU ID register
  AND     r0, r0, #0x03           // Mask off, leaving the CPU ID field
  BX      lr
  .cfi_endproc


// ------------------------------------------------------------

  .global goToSleep
  // void goToSleep(void)
  .type goToSleep, "function"
  .cfi_startproc
goToSleep:
  DSB                             // Clear all pending data accesses
  WFI                             // Go into standby
  B       goToSleep               // Catch in case of rogue events
  BX      lr
  .cfi_endproc


// ------------------------------------------------------------

  .global joinSMP
  // void joinSMP(void)
  // Sets the ACTRL.SMP bit
  .type joinSMP, "function"
  .cfi_startproc
joinSMP:

  // SMP status is controlled by bit 6 of the CP15 Aux Ctrl Reg

  MRC     p15, 0, r0, c1, c0, 1   // Read ACTLR
  MOV     r1, r0
  ORR     r0, r0, #0x040          // Set bit 6
  CMP     r0, r1
  MCRNE   p15, 0, r0, c1, c0, 1   // Write ACTLR
  ISB

  BX      lr
  .cfi_endproc


// ------------------------------------------------------------

  .global leaveSMP
  // void leaveSMP(void)
  // Clear the ACTRL.SMP bit
  .type leaveSMP, "function"
  .cfi_startproc
leaveSMP:

  // SMP status is controlled by bit 6 of the CP15 Aux Ctrl Reg

  MRC     p15, 0, r0, c1, c0, 1   // Read ACTLR
  BIC     r0, r0, #0x040          // Clear bit 6
  MCR     p15, 0, r0, c1, c0, 1   // Write ACTLR
  ISB

  BX      lr
  .cfi_endproc


// ------------------------------------------------------------
// End of v7.s
// ------------------------------------------------------------
