/* demo.h - declarations of all the demo functions, 
 * including specifications of which ones are in which overlays.
 *
 * Copyright (c) 2016-2017 Arm Limited (or its affiliates). All rights reserved.
 * Use, modification and redistribution of this file is subject to your possession of a
 * valid End User License Agreement for the Arm Product of which these examples are part of 
 * and your compliance with all applicable terms and conditions of such licence agreement.
 */

__ARM_OVERLAY(0) void standalone_0(void);
__ARM_OVERLAY(1) void standalone_1(void);
__ARM_OVERLAY(2) void standalone_2(void);
__ARM_OVERLAY(3) void standalone_3(void);

__ARM_OVERLAY(0) void direct_chain_0(void);
__ARM_OVERLAY(1) void direct_chain_1(void);
__ARM_OVERLAY(2) void direct_chain_2(void);
__ARM_OVERLAY(3) void direct_chain_3(void);

__ARM_OVERLAY(1) void direct_chain_0b(void);
__ARM_OVERLAY(3) void direct_chain_1b(void);
__ARM_OVERLAY(0) void direct_chain_2b(void);
__ARM_OVERLAY(2) void direct_chain_3b(void);

__ARM_OVERLAY(0) void indirect_chain_0(void);
void non_overlaid_wrapper_0_to_1(void);
__ARM_OVERLAY(1) void indirect_chain_1(void);
void non_overlaid_wrapper_1_to_2(void);
__ARM_OVERLAY(2) void indirect_chain_2(void);
void non_overlaid_wrapper_2_to_3(void);
__ARM_OVERLAY(3) void indirect_chain_3(void);

typedef void (*demo_function_pointer)(int arg);
void fp_target_non_overlaid(int arg);
__ARM_OVERLAY(0) void fp_target_0(int arg);
__ARM_OVERLAY(1) void fp_target_1(int arg);
__ARM_OVERLAY(2) void fp_target_2(int arg);
__ARM_OVERLAY(3) void fp_target_3(int arg);
void fp_call_non_overlaid(demo_function_pointer);
__ARM_OVERLAY(0) void fp_call_0(demo_function_pointer);
__ARM_OVERLAY(1) void fp_call_1(demo_function_pointer);
__ARM_OVERLAY(2) void fp_call_2(demo_function_pointer);
__ARM_OVERLAY(3) void fp_call_3(demo_function_pointer);
demo_function_pointer fp_return_non_overlaid(void);
__ARM_OVERLAY(0) demo_function_pointer fp_return_0(void);
__ARM_OVERLAY(1) demo_function_pointer fp_return_1(void);
__ARM_OVERLAY(2) demo_function_pointer fp_return_2(void);
__ARM_OVERLAY(3) demo_function_pointer fp_return_3(void);

__ARM_OVERLAY(0) void interworking_chain_0a(void);
__ARM_OVERLAY(1) void interworking_chain_1a(void);
__ARM_OVERLAY(0) void interworking_chain_2a(void);
__ARM_OVERLAY(2) void interworking_chain_3a(void);
__ARM_OVERLAY(1) void interworking_chain_4a(void);
__ARM_OVERLAY(2) void interworking_chain_5a(void);
__ARM_OVERLAY(0) void interworking_chain_6a(void);
__ARM_OVERLAY(3) void interworking_chain_7a(void);
__ARM_OVERLAY(1) void interworking_chain_8a(void);
__ARM_OVERLAY(3) void interworking_chain_9a(void);
__ARM_OVERLAY(2) void interworking_chain_10a(void);
__ARM_OVERLAY(3) void interworking_chain_11a(void);
__ARM_OVERLAY(0) void interworking_chain_12a(void);

__ARM_OVERLAY(0) void interworking_chain_0t(void);
__ARM_OVERLAY(1) void interworking_chain_1t(void);
__ARM_OVERLAY(0) void interworking_chain_2t(void);
__ARM_OVERLAY(2) void interworking_chain_3t(void);
__ARM_OVERLAY(1) void interworking_chain_4t(void);
__ARM_OVERLAY(2) void interworking_chain_5t(void);
__ARM_OVERLAY(0) void interworking_chain_6t(void);
__ARM_OVERLAY(3) void interworking_chain_7t(void);
__ARM_OVERLAY(1) void interworking_chain_8t(void);
__ARM_OVERLAY(3) void interworking_chain_9t(void);
__ARM_OVERLAY(2) void interworking_chain_10t(void);
__ARM_OVERLAY(3) void interworking_chain_11t(void);
__ARM_OVERLAY(0) void interworking_chain_12t(void);

__ARM_OVERLAY(0) void many_params_0(int a, int b, int c, int d, int e, int f, int g, int h, int i, int j, int k);
__ARM_OVERLAY(1) void many_params_1(int a, int b, int c, int d, int e, int f, int g, int h, int i, int j, int k);
__ARM_OVERLAY(2) void many_params_2(int a, int b, int c, int d, int e, int f, int g, int h, int i, int j, int k);
__ARM_OVERLAY(3) void many_params_3(int a, int b, int c, int d, int e, int f, int g, int h, int i, int j, int k);
