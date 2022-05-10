/* module3.c - module containing some overlaid functions
 *
 * Copyright (c) 2021 Arm Limited (or its affiliates). All rights reserved.
 * Use, modification and redistribution of this file is subject to your possession of a
 * valid End User License Agreement for the Arm Product of which these examples are part of
 * and your compliance with all applicable terms and conditions of such licence agreement.
 */

#include <stdio.h>
#include "modules.h"

void standalone_3(void) {
	printf("Hello and goodbye from standalone 3\n");
}

void direct_chain_3(void){
	printf("Hello and goodbye from direct chain 3\n");
	direct_chain_end();
}

void indirect_chain_3(void){
	printf("Hello and goodbye from indirect chain 3\n");
}

void fp_target_1(int arg){
	printf("hello and goodbye from fp_target_1(%d)\n", arg);
}

void fp_call_1(demo_function_pointer fp){
	printf("Calling function pointer with arg 1001\n");
	fp(1001);
}

void interworking_chain_2a(void){
	printf("Hello and goodbye from interworking chain 2a\n");
	interworking_chain_2t();
}
