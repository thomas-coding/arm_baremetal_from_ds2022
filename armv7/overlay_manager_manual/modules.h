/* modules.h - header file containing all overlay functions
 *
 * Copyright (c) 2021 Arm Limited (or its affiliates). All rights reserved.
 * Use, modification and redistribution of this file is subject to your possession of a
 * valid End User License Agreement for the Arm Product of which these examples are part of
 * and your compliance with all applicable terms and conditions of such licence agreement.
 */

#include "ovl_api.h"

__ARM_OVERLAY(0) void standalone_1(void);
__ARM_OVERLAY(0) void standalone_2(void);
__ARM_OVERLAY(1) void standalone_3(void);

__ARM_OVERLAY(1) void direct_chain_1(void);
__ARM_OVERLAY(0) void direct_chain_2(void);
__ARM_OVERLAY(1) void direct_chain_3(void);
__ARM_OVERLAY(0) void direct_chain_end(void);

void indirect_chain_start(void);
__ARM_OVERLAY(0) void indirect_chain_1(void);
void indirect_chain_1_to_2(void);
__ARM_OVERLAY(0) void indirect_chain_2(void);
void indirect_chain_2_to_3(void);
__ARM_OVERLAY(1) void indirect_chain_3(void);


typedef void (*demo_function_pointer)(int arg);
void fp_target_non_overlaid(int arg);
__ARM_OVERLAY(0) void fp_target_0(int arg);
__ARM_OVERLAY(1) void fp_target_1(int arg);
__ARM_OVERLAY(0) void fp_target_2(int arg);
__ARM_OVERLAY(1) void fp_target_3(int arg);
void fp_call_non_overlaid(demo_function_pointer);
__ARM_OVERLAY(0) void fp_call_0(demo_function_pointer);
__ARM_OVERLAY(1) void fp_call_1(demo_function_pointer);
__ARM_OVERLAY(0) void fp_call_2(demo_function_pointer);
__ARM_OVERLAY(1) void fp_call_3(demo_function_pointer);


__ARM_OVERLAY(0) void interworking_chain_0a(void);
__ARM_OVERLAY(0) void interworking_chain_1a(void);
__ARM_OVERLAY(1) void interworking_chain_2a(void);
__ARM_OVERLAY(1) void interworking_chain_3a(void);

__ARM_OVERLAY(2) void interworking_chain_0t(void);
__ARM_OVERLAY(2) void interworking_chain_1t(void);
__ARM_OVERLAY(2) void interworking_chain_2t(void);
__ARM_OVERLAY(2) void interworking_chain_3t(void);
