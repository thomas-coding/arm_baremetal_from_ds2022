/* module4.c - module containing some overlaid functions
 *
 * Copyright (c) 2021 Arm Limited (or its affiliates). All rights reserved.
 * Use, modification and redistribution of this file is subject to your possession of a
 * valid End User License Agreement for the Arm Product of which these examples are part of
 * and your compliance with all applicable terms and conditions of such licence agreement.
 */

#include <stdio.h>
#include "modules.h"

void direct_chain_1(void){
	printf("Hello and goodbye from direct chain 1\n");
	direct_chain_2();
}

void fp_target_3(int arg){
	printf("hello and goodbye from fp_target_3(%d)\n", arg);
}

void fp_call_3(demo_function_pointer fp){
	printf("Calling function pointer with arg 1003\n");
	fp(1003);
}

void interworking_chain_3a(void){
	printf("Hello and goodbye from interworking chain 3a\n");
	interworking_chain_3t();
}
