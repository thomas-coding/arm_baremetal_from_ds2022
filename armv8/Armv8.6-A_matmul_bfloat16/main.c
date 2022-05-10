/* Copyright (c) 2019-2020 Arm Limited (or its affiliates). All rights reserved. */
/* Use, modification and redistribution of this file is subject to your possession of a     */
/* valid End User License Agreement for the Arm Product of which these examples are part of */
/* and your compliance with all applicable terms and conditions of such licence agreement.  */


#include <stdio.h>
#include <arm_neon.h>


int main(void)
{
    printf("\nbfloat16 matrix multiply example using the Armv8.6-A BFMMLA instruction\n");

    // Initialize first 2x4 input matrix
    const float32_t __attribute__ ((aligned (16))) a_row_1[] = {1.0f, 2.0f, 3.0f, 4.0f};
    float32x4_t ax = vld1q_f32(a_row_1);
    const float32_t __attribute__ ((aligned (16))) a_row_2[] = {1.0f, 1.0f, 1.0f, 1.0f};
    float32x4_t ay = vld1q_f32(a_row_2);

    bfloat16x8_t a = vcombine_bf16(vcvt_bf16_f32(ax), vcvt_bf16_f32(ay));

    // Initialize second 4x2 input matrix
    const float32_t __attribute__ ((aligned (16))) b_col_1[] = {1.0f, 2.0f, 3.0f, 4.0f};
    float32x4_t bx = vld1q_f32(b_col_1);
    const float32_t __attribute__ ((aligned (16))) b_col_2[] = {1.0f, 1.0f, 1.0f, 1.0f};
    float32x4_t by = vld1q_f32(b_col_2);

    bfloat16x8_t b = vcombine_bf16(vcvt_bf16_f32(bx), vcvt_bf16_f32(by));

    // Zero output/accumulate float matrix
    float32_t __attribute__ ((aligned (16))) cx1[] = {0.0f, 0.0f, 0.0f, 0.0f};
    float32x4_t c = vld1q_f32(cx1);

    c = vbfmmlaq_f32(c, a, b);

    // Should generate the equivalent of
    // __asm volatile(
    // "BFMMLA v0.4S, v1.8H, v2.8H\n"
    // );

    printf("Expected result:\n");
    printf("30.000000 10.000000 10.000000 4.000000\n");
    printf("Actual result  :\n");
    printf("%f ", vgetq_lane_f32(c, 0));
    printf("%f ", vgetq_lane_f32(c, 1));
    printf("%f ", vgetq_lane_f32(c, 2));
    printf("%f ", vgetq_lane_f32(c, 3));
    printf("\n");

    return 0;
}
