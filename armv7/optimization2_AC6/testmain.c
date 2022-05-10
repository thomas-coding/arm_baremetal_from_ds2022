/*---------------------------------------------------------------*/
/* Further Optimization Techniques Example                       */
/*---------------------------------------------------------------*/

/* Copyright (c) 2011-2019 Arm Limited (or its affiliates). All rights reserved.  */
/* Use, modification and redistribution of this file is subject to your possession of a     */
/* valid End User License Agreement for the Arm Product of which these examples are part of */
/* and your compliance with all applicable terms and conditions of such licence agreement.  */


#include <stdio.h>

extern void inlineable_function2(void);
extern void code_share_function2(void);
extern void add_int_array1(int * pd, int * ps1, int * ps2, int n);
extern void add_int_array2(int * restrict pd, int * restrict ps1, int * restrict ps2, unsigned int n);


int common_data1;

#define ARRAY_SIZE 8 // a multiple of 4

int array_d1[ARRAY_SIZE];
int * restrict pd1 = &array_d1[0];
int array_d2[ARRAY_SIZE];
int * restrict pd2 = &array_d2[0];
int array_s1[ARRAY_SIZE] = { 0, 1, 2, 3, 4, 5, 6, 7 };
int * restrict ps1 = &array_s1[0];
int array_s2[ARRAY_SIZE] = { 10, 11, 12, 13, 14, 15, 16, 17 };
int * restrict ps2 = &array_s2[0];


// Bare-minimum start-up code to run NEON code

void StartHere(void)
{
	__asm volatile(
    "MRC p15,0,r0,c1,c0,0\n"    // Read System Control Register
    "ORR r0,r0,#0x00400000\n"   // Set U bit to enable unaligned mode (it is fixed as set for recent cores)
    "BIC r0,r0,#0x00000002\n"   // Clear A bit 1 to disable strict alignment fault checking
    "MCR p15,0,r0,c1,c0,0\n"    // Write System Control Register

#if defined(__ARM_NEON) || defined(__ARM_FP)
    "MRC p15,0,r0,c1,c0,2    \n"// Read CP Access register
    "ORR r0,r0,#0x00f00000   \n"// Enable full access to NEON/VFP by enabling access to Coprocessors 10 and 11
    "MCR p15,0,r0,c1,c0,2    \n"// Write CP Access register
    "ISB\n"

    "MOV r0,#0x40000000      \n"// Switch on the VFP and NEON hardware
    "VMSR FPEXC,r0            \n"// Set EN bit in FPEXC
#endif

    ".global __main\n"
    "B __main\n"                // Enter normal C run-time environment & library start-up
	);
}


// unused_function() is unused and can be removed.
void unused_function(void)
{
    printf("Hello from unused_function().\n");
}


// inlineable_function1() in this source file can be inlined as usual, and its out-of-line version can be removed.
void inlineable_function1(void)
{
    printf("Hello from inlineable_function1().\n");
}


// code_share_function1() and code_share_function2() in another source file can share the same literal when compiled with -flto and linked with --lto, and can be inlined, and their out-of-line versions can be removed.
void code_share_function1(void)
{
    common_data1 = 0x76543210; //Same literal as in code_share_function2
    printf("Hello from code_share_function1() %0x.\n", common_data1);
}


void print_array(int * pa, int n)
{
    int i;
    for(i = 0; i < n; i++)
        printf("%d ", *(pa+i) );
    printf("\n");
}



int main(void)
{
    printf("Hello from main().\n");

    printf("unused_function() is unused and can be removed.\n");
//    unused_function();  // deliberately commented out

    printf("inlineable_function1() in this source file can be inlined as usual, and its out-of-line version can be removed.\n");
    inlineable_function1();

    printf("inlineable_function2() in another source file can also be inlined when compiled with -flto and linked with --lto, and its out-of-line version can be removed.\n");
    inlineable_function2();

    printf("code_share_function1() and code_share_function2() in another source file can share the same literal when compiled with -flto and linked with --lto, and can be inlined, and their out-of-line versions can be removed.\n");
    code_share_function1();
    code_share_function2();

    // Unoptimized function to add two integer arrays
    add_int_array1(pd1, ps1, ps2, ARRAY_SIZE);
    print_array(pd1, ARRAY_SIZE);

    // Optimized function to add two integer arrays, where the arrays do not overlap, and n is a multiple of four,
    // which is friendly towards NEON auto-vectorization
    add_int_array2(pd2, ps1, pd1, ARRAY_SIZE);
    print_array(pd2, ARRAY_SIZE);

    return 0;
}
