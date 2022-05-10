;==================================================================
; Copyright (c) 2009-2018 Arm Limited (or its affiliates). All rights reserved.
; Use, modification and redistribution of this file is subject to your possession of a
; valid End User License Agreement for the Arm Product of which these examples are part of 
; and your compliance with all applicable terms and conditions of such licence agreement.
;
; Cortex-A9 MPCore - Normal World Boot
;==================================================================

    PRESERVE8

  AREA  NormalStartUp,CODE,READONLY

; ------------------------------------------------------------
; Define some values
; ------------------------------------------------------------

; Standard definitions of mode bits and interrupt (I&F) flags in PSRs
Mode_USR          EQU   0x10
Mode_FIQ          EQU   0x11
Mode_IRQ          EQU   0x12
Mode_SVC          EQU   0x13
Mode_ABT          EQU   0x17
Mode_UND          EQU   0x1B
Mode_SYS          EQU   0x1F
I_Bit             EQU   0x80 ; when I bit is set, IRQ is disabled
F_Bit             EQU   0x40 ; when F bit is set, FIQ is disabled


L1_COHERENT       EQU   0x00014c06  ; Template descriptor for coherent memory
L1_NONCOHERENT    EQU   0x00000c1e  ; Template descriptor for non-coherent memory
L1_DEVICE         EQU   0x00000c06  ; Template descriptor for device memory

; ------------------------------------------------------------
; Code
; ------------------------------------------------------------

  ENTRY

  EXPORT normalStart
normalStart PROC

  IMPORT ||Image$$NORMAL_PAGETABLES$$ZI$$Base||
  IMPORT ||Image$$ARM_LIB_STACKHEAP$$ZI$$Limit||


  ;
  ; Put all the cores - except core 0 - into standby
  ;--------------------------------------------------
  IMPORT goToSleep
  IMPORT getCPUID
  BL      getCPUID                ; Returns CPU ID in r0
  CMP     r0, #0                  ; Is this CPU0?
  BLNE    goToSleep               ; If not, go to sleep


  ;
  ; Setup stacks
  ;---------------
  MSR     CPSR_c, #Mode_SVC:OR:I_Bit:OR:F_Bit   ; No interrupts
  LDR     sp, =||Image$$ARM_LIB_STACKHEAP$$ZI$$Limit||


  ;
  ; Disable Caches & Table Type
  ;-----------------------------
  ; 2  - C  - Level 1 data cache enable (0 to disable)
  ; 12 - I  - Level 1 instruction cache enable (0 to disable)
  MRC     p15, 0, r0, c1, c0, 0
  BIC     r0, r0, #0x00004        ; disable data cache
  BIC     r0, r0, #0x01000        ; disable instruction cache
  MCR     p15, 0, r0, c1, c0, 0
  ISB

  ;
  ; Invalidate caches
  ; -------------------
  IMPORT  invalidateCaches
  BL      invalidateCaches

  ;
  ; Invalidate TLBs
  ;------------------
  MOV     r0, #0x0
  MCR     p15, 0, r0, c8, c7, 0     ; TLBIALL - Invalidate entire Unified TLB

  ;
  ; Branch Prediction Init
  ; -----------------------
  IMPORT  flushBranchTargetCache
  IMPORT  disableBranchPrediction

  BL      flushBranchTargetCache
  BL      disableBranchPrediction
  ; Branch prediction is enabled later, after scatterloading

  ;
  ; Set Vector Base Address Register (needed here because NS VBAR has no defined reset value)
  ;
  LDR     r0, =0x0                    ; Load base address into r0
  MCR     p15, 0, r0, c12, c0, 0      ; Write Current world VBAR

  ;
  ; Set up Domain Access Control Reg
  ; ----------------------------------
  ; b00 - No Access (abort)
  ; b01 - Client (respect table entry)
  ; b10 - RESERVED
  ; b11 - Manager (ignore access permissions)
  ; Setting D0 to client, all others to No Access
  MOV     r0, #0x01
  MCR     p15, 0, r0, c3, c0, 0

  ;
  ; Activate VFP/NEON, if required
  ;-------------------------------

  IF {TARGET_FEATURE_NEON} || {TARGET_FPU_VFP}

  ; Enable access to NEON/VFP by enabling access to Coprocessors 10 and 11.
  ; Enables Full Access i.e. in both privileged and non privileged modes
      MRC     p15, 0, r0, c1, c0, 2     ; Read Coprocessor Access Control Register (CPACR)
      ORR     r0, r0, #(0xF << 20)      ; Enable access to CP 10 & 11
      MCR     p15, 0, r0, c1, c0, 2     ; Write Coprocessor Access Control Register (CPACR)
      ISB

  ; Switch on the VFP and NEON hardware
      MOV     r0, #0x40000000
      VMSR    FPEXC, r0                   ; Write FPEXC register, EN bit set

  ENDIF

  ;
  ; Set location of level 1 page table
  ;------------------------------------
  ; 31:14 - Base addr 0x8400,0000
  ; 13:5  - 0x0
  ; 4:3   - RGN 0x0 (Outer Noncachable)
  ; 2     - P   0x0
  ; 1     - S   0x0 (Non-shared)
  ; 0     - C   0x0 (Inner Noncachable)
  LDR     r0, =||Image$$NORMAL_PAGETABLES$$ZI$$Base||
  MCR     p15, 0, r0, c2, c0 ,0
  
  
  ;
  ; Generate tables
  ; ----------------
  ; Fill table with zeros
  MOV     r2, #1024                 ; Set r3 to loop count (4 entries per iteration, 1024 iterations)
  MOV     r1, r0                    ; Make a copy of the base dst
  MOV     r3, #0
  MOV     r4, #0
  MOV     r5, #0
  MOV     r6, #0
ttb_zero_loop
  STMIA   r1!, {r3-r6}              ; Store out four entries
  SUBS    r2, r2, #1                ; Decrement counter
  BNE     ttb_zero_loop

  ; Entry for VA used by code and data
  ; This region must be coherent
  MOV     r1, pc                    ; Make a copy of the PC
  LSR     r1, r1, #20               ; Clear bottom 20 bits, to find which 1MB block it is in
  LSL     r2, r1, #2                ; Make a copy, and multiply by four.  This gives offset into the page tables
  LSL     r1, r1, #20               ; Put back in address format

  LDR     r3, =L1_COHERENT          ; Descriptor template
  ORR     r1, r1, r3                ; Combine address and template
  STR     r1, [r0, r2]


  ; Entry for private address space
  ; Needs to be marked as Device memory
  ;MRC     p15, 4, r1, c15, c0, 0    ; Get base address of private address space
  ;LSR     r1, r1, #20               ; Clear bottom 20 bits, to find which 1MB block it is in
  ;LSL     r2, r1, #2                ; Make a copy, and multiply by four.  This gives offset into the page tables
  ;LSL     r1, r1, #20               ; Put back in address format

  ;LDR     r3, =L1_DEVICE            ; Descriptor template
  ;ORR     r1, r1, r3                ; Combine address and template
  ;STR     r1, [r0, r2]

  ;
  ; Set Translation Table Base Control Reg
  ; ---------------------------------
  ; 31:6  - SBZ
  ; 5     - PD[1], whether misses in TTBR1 causes a table walk
  ; 4     - PD[0], whether misses in TTBR0 causes a table walk
  ; 3     - SBZ
  ; 2:0   - N, split between TTBR0 and TTBR1
  MOV     r0,#0x0
  MCR     p15, 0, r0, c2, c0, 2


  ; Enable MMU
  ;-------------
  ; 0     - M, set to enable MMU
  MRC     p15, 0, r0, c1, c0, 0       ; Read current control reg
  ORR     r0, r0, #0x01               ; Set M bit
  MCR     p15, 0, r0, c1, c0, 0       ; Write reg back
  ISB

  ;
  ; MMU now enabled - Virtual address system now active
  ;

  ;
  ; Branch to C lib code
  ; ----------------------
  IMPORT  __main
  B       __main
  
  ENDP

  END
