// ------------------------------------------------------------
// Armv8.1-M PACBTI Assembler Example
//
// Copyright (c) 2021 Arm Limited (or its affiliates). All rights reserved.
// Use, modification and redistribution of this file is subject to your possession of a
// valid End User License Agreement for the Arm Product of which these examples are part of 
// and your compliance with all applicable terms and conditions of such licence agreement.
// ------------------------------------------------------------

    .cfi_sections .debug_frame  // put stack frame info into .debug_frame instead of .eh_frame

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

	.global pacbti_asm_demo
    .type pacbti_asm_demo, "function"

pacbti_asm_demo:
    PUSH   {R4-R7, R12, LR}

    // *** Demonstrate PAC in assembler ***

    // PAC and BTI were enabled earlier in startup.S.  To be able to demonstrate PAC only (without BTI),
    // temporarily disable BTI (leaving PAC enabled) for the current security state and privileged mode
    MRS     R0,CONTROL      // Read CONTROL reg
    BIC     R0,R0,#(1<<4)   // Clear BTI_EN bit
    MSR     CONTROL,R0      // Write to CONTROL reg

    // Call a PAC-protected function (func1) that adds two values passed in R0 and R1
    MOV     R0, #10         // Two values to add
    MOV     R1, #11
    ADR     R2, func1+1     // Load destination address (Thumb-2) with LSB set
    BLX     R2              // Call function which is protected by PAC-AUT
ret1:
    // Call a PAC-protected function (func2) that calculates a PAC from two values passed in R0 and R1
    MOV     R0, #0xAA       // Two values to use to calculate a PAC
    MOV     R1, #0xBB
    ADR     R2, func2+1     // Load destination address (Thumb-2) with LSB set
    BLX     R2              // Call function which is protected by PAC-AUT.
ret2:
    B       next            // Jump to 'next'

func1:
    PAC     R12, LR, SP     // Computes the cryptographic code using LR, SP, and
                            // a key, then stores it in R12.
                            // This instruction works on legacy implementations.
    PUSH   {R4-R7, R12, LR} // Stack LR if it needs to be used before the AUT.

    ADD     R0,R0,R1        // Function body.

    POP    {R4-R7, R12, LR} // Restore the value of LR before authenticating.
    AUT     R12, LR, SP     // Computes the cryptographic code using LR, SP, and
                            // a key, then compares it against the value in R12.
                            // If the two values match then the LR is validated,
                            // otherwise a UsageFault is generated.
                            // This instruction works on legacy implementations.
    BX      LR              // Return to "ret1".

func2:
    PAC     R12, LR, SP     // Calculate PAC for LR.
    PUSH   {R4-R7, R12, LR} // Stack LR if it needs to be used before the AUT.
    PACG    R2, R1, R0      // Calculate PAC where desired value is in R1.
                            // The modifier value is in R0.
                            // Computed PAC is stored in R2 register.
                            // This instruction does not work on legacy implementations.

    NOP                     // Dummy function body.

    POP    {R4-R7, R12, LR} // Restore the value of LR before authenticating.
    BXAUT   R12, LR, SP     // Validate LR and branch to LR ('ret2').
                            // This instruction does not work on legacy implementations.

next:
    AUTG    R2, R1, R0      // Validate R2, R1, and R0 registers. If any of these values
                            // have changed, an INVSTATE UsageFault will be generated.
                            // This method detects any tampering which can occur when
                            // registers are stored and loaded from memory.
                            // This instruction does not work on legacy implementations.


    // *** Demonstrate BTI in assembler ***

    // Re-enable BTI for the current security state and privileged mode
    MRS     R0,CONTROL      // Read CONTROL reg
    ORR     R0,R0,#(1<<4)   // Set BTI_EN bit
    MSR     CONTROL,R0      // Write to CONTROL reg

    // Call a function (func3) using a BTI setting instruction
    ADR     R2, func3+1     // Load destination address (Thumb-2) with LSB set
    BLX     R2              // BTI setting instruction, sets EPSR.B.
    B       finish          // Jump to 'finish'

func3:
    PACBTI  R12, LR, SP     // BTI clearing instruction, valid entry point.
                            // Stacking and unstacking R12, LR, and SP is safe
                            // as any corruption will be detected by BXAUT.

    NOP                     // Dummy function body.

    BXAUT   R12, LR, SP     // Validate LR and return to address in LR.
                            // The combined authenticate and branch instruction
                            // is not backwards compatible with legacy software.

finish:
	POP    {R4-R7, R12, PC} // Return back to caller
