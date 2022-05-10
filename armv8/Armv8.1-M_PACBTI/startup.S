// ------------------------------------------------------------
// Armv8.1-M PACBTI Example
//
// Copyright (c) 2021-2022 Arm Limited (or its affiliates). All rights reserved.
// Use, modification and redistribution of this file is subject to your possession of a
// valid End User License Agreement for the Arm Product of which these examples are part of
// and your compliance with all applicable terms and conditions of such licence agreement.
// ------------------------------------------------------------

        .cfi_sections .debug_frame  // put stack frame info into .debug_frame instead of .eh_frame

        .section VECTORS, "ax"
        .align 3

  #if !defined(__ARM_64BIT_STATE)
  #ifdef __ARM_FEATURE_PAC_DEFAULT
      .eabi_attribute Tag_PAC_extension, 1
      .eabi_attribute Tag_PACRET_use, 1
  #endif
  #ifdef __ARM_FEATURE_BTI_DEFAULT
      .eabi_attribute Tag_BTI_extension, 1
      .eabi_attribute Tag_BTI_use, 1
  #endif
  #endif

// Vector Table mapped to address 0 at reset via the scatter-file

#define INITIAL_SP     Image$$ARM_LIB_STACKHEAP$$ZI$$Limit     // Top of stack

Vectors:
                .word      INITIAL_SP                          //     Initial Stack Pointer
                .word      Reset_Handler                       //     Reset Handler
                .word      NMI_Handler                         // -14 NMI Handler
                .word      HardFault_Handler                   // -13 Hard Fault Handler
                .word      MemManage_Handler                   // -12 MPU Fault Handler
                .word      BusFault_Handler                    // -11 Bus Fault Handler
                .word      UsageFault_Handler                  // -10 Usage Fault Handler
                .word      SecureFault_Handler                 //  -9 Secure Fault Handler
                .word      0                                   //     Reserved
                .word      0                                   //     Reserved
                .word      0                                   //     Reserved
                .word      SVC_Handler                         //  -5 SVCall Handler
                .word      DebugMon_Handler                    //  -4 Debug Monitor Handler
                .word      0                                   //     Reserved
                .word      PendSV_Handler                      //  -2 PendSV Handler
                .word      SysTick_Handler                     //  -1 SysTick Handler

                // Interrupts
                .word      Interrupt0_Handler                  //   0 Interrupt 0
                .word      Interrupt1_Handler                  //   1 Interrupt 1
                .word      Interrupt2_Handler                  //   2 Interrupt 2
                .word      Interrupt3_Handler                  //   3 Interrupt 3
                .word      Interrupt4_Handler                  //   4 Interrupt 4
                .word      Interrupt5_Handler                  //   5 Interrupt 5
                .word      Interrupt6_Handler                  //   6 Interrupt 6
                .word      Interrupt7_Handler                  //   7 Interrupt 7
                .word      Interrupt8_Handler                  //   8 Interrupt 8
                .word      Interrupt9_Handler                  //   9 Interrupt 9

                .space     (470 * 4)                           // Interrupts 10 .. 479 are omitted


                .type    Reset_Handler, "function"
Reset_Handler:
                .eabi_attribute	25, 1  // Tag_ABI_align_preserved
                .global   Reset_Handler
                .global   __main

    // Register the location of the vector table
    LDR     R1, =0xE000ED08
    LDR     R0, =Vectors
    STR     R0,[R1]         // Write VTOR

    // Enable the FPU in both privileged and user modes
    LDR     R1, =0xE000ED88
    LDR     R0,[R1]         // Read CPACR
    ORR     R0, R0, #(0xF << 20) // Set bits 20-23 to enable CP10 and CP11
    STR     R0,[R1]         // Write CPACR

    // Set up a PAC signing key.
    // For security, a random encryption key should be used on every boot.
    // Lacking that, for testing purposes, a fixed random-like one is used here.

    MOVW   R0,#0xfb42
    MOVT   R0,#0x11e7
    MSR PAC_KEY_P_0,R0
    MOVW   R0,#0xeea2
    MOVT   R0,#0xfc6f
    MSR PAC_KEY_P_1,R0
    MOVW   R0,#0xc231
    MOVT   R0,#0x02c7
    MSR PAC_KEY_P_2,R0
    MOVW   R0,#0x6582
    MOVT   R0,#0xa269
    MSR PAC_KEY_P_3,R0

    // Enable PAC and BTI for the current security state and privileged mode
    MRS     R0,CONTROL      // Read CONTROL reg
    ORR     R0,R0,#(1<<6)   // Set PAC_EN bit
    ORR     R0,R0,#(1<<4)   // Set BTI_EN bit
    MSR     CONTROL,R0      // Write CONTROL reg

    // Enable UsageFault exception
    // so that a Usage Fault will occur instead of a Hard Fault if a PACBTI mismatch is detected
    LDR     R1, =0xE000ED24
    LDR     R0,[R1]         // Read SHCSR
    ORR     R0,R0,#(1<<18)  // Set USGFAULTENA bit
    STR     R0,[R1]         // Write SHCSR

    // Call the C library start-up code to initialize the C run-time environment, which ultimately calls main()
    LDR      R0, =__main
    BX       R0


// Default exception/interrupt handlers

NMI_Handler:
                B        .
HardFault_Handler:
                B        .
MemManage_Handler:
                B        .
BusFault_Handler:
                B        .
UsageFault_Handler:
                B        .
SecureFault_Handler:
                B        .
SVC_Handler:
                B        .
DebugMon_Handler:
                B        .
PendSV_Handler:
                B        .
SysTick_Handler:
                B        .

Interrupt0_Handler:
                B        .
Interrupt1_Handler:
                B        .
Interrupt2_Handler:
                B        .
Interrupt3_Handler:
                B        .
Interrupt4_Handler:
                B        .
Interrupt5_Handler:
                B        .
Interrupt6_Handler:
                B        .
Interrupt7_Handler:
                B        .
Interrupt8_Handler:
                B        .
Interrupt9_Handler:
                B        .
