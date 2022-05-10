/*---------------------------------------------------------------*/
/* Further Optimization Techniques Example                       */
/*---------------------------------------------------------------*/

/* Copyright (c) 2011-2019 Arm Limited (or its affiliates). All rights reserved.  */
/* Use, modification and redistribution of this file is subject to your possession of a     */
/* valid End User License Agreement for the Arm Product of which these examples are part of */
/* and your compliance with all applicable terms and conditions of such licence agreement.  */


#include <stdio.h>

int common_data2;


// inlineable_function2() in another source file can also be inlined when compiled with -flto and linked with --lto, and its out-of-line version can be removed.
void inlineable_function2(void)
{
    printf("Hello from inlineable_function2().\n");
}


// code_share_function2() can share the same literal with code_share_function1() in another source file when compiled with -flto and linked with --lto, and its out-of-line version will be removed by link time optimization.
void code_share_function2(void)
{
    common_data2 = 0x76543210; //Same literal as in code_share_function1
    printf("Hello from code_share_function2() %0x.\n", common_data2);
}


// Unoptimized function to add two integer arrays
void add_int_array1(int * pd, int * ps1, int * ps2, int n)
{
    int i;
    for(i = 0; i < n; i++)
        pd[i] = ps1[i] + ps2[i];
}


// Optimized function to add two integer arrays, where the arrays do not overlap, and n is a multiple of four,
// which is friendly towards NEON auto-vectorization
void add_int_array2(int * restrict pd, int * restrict ps1, int * restrict ps2, unsigned int n)
{
    unsigned int i;
    for(i = 0; i < (n >> 2) << 2; i++)
        pd[i] = ps1[i] + ps2[i];
}
