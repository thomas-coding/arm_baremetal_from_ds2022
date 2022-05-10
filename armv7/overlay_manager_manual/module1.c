/* module1.c - module containing some overlaid functions
 *
 * Copyright (c) 2021 Arm Limited (or its affiliates). All rights reserved.
 * Use, modification and redistribution of this file is subject to your possession of a
 * valid End User License Agreement for the Arm Product of which these examples are part of
 * and your compliance with all applicable terms and conditions of such licence agreement.
 */

#include <stdio.h>
#include "modules.h"

void standalone_1(void) {
	printf("Hello and goodbye from standalone 1\n");
}

void direct_chain_2(void){
	printf("Hello and goodbye from direct chain 2\n");
	direct_chain_3();
}

void indirect_chain_1(void){
	printf("Hello and goodbye from indirect chain 1\n");
	indirect_chain_1_to_2();
}

void fp_target_0(int arg){
	printf("hello and goodbye from fp_target_0(%d)\n", arg);
}

void fp_call_0(demo_function_pointer fp){
	printf("Calling function pointer with arg 1000\n");
	fp(1000);
}

void interworking_chain_0a(void){
	printf("Hello and goodbye from interworking chain 0a\n");
	interworking_chain_0t();
}
