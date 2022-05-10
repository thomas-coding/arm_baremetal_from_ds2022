// ------------------------------------------------------------
// Armv8.1-M PACBTI Example
//
// Copyright (c) 2021 Arm Limited (or its affiliates). All rights reserved.
// Use, modification and redistribution of this file is subject to your possession of a
// valid End User License Agreement for the Arm Product of which these examples are part of
// and your compliance with all applicable terms and conditions of such licence agreement.
// ------------------------------------------------------------


#include <stdio.h>

extern void pacbti_asm_demo(void); // Assembler example
extern void compare_sorts(void);   // C code example


__attribute__((noreturn)) int main(void)
{
    /* Display a welcome message via semihosting */
    printf("\nPACBTI bare-metal example for Armv8.1-M\n\n");

    printf("\nDemo PAC and BTI (separately) in assembler...\n");
    pacbti_asm_demo();  // Assembler example

    printf("\nDemo PACBTI added automatically in C code...\n");
    compare_sorts();    // C code example

    printf("\nDemo complete\n");
    while( 1 )
    {
        /* Loop forever */
    }
}
