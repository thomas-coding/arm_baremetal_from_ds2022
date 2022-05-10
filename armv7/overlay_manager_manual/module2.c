/* module2.c - module containing some overlaid functions
 *
 * Copyright (c) 2021 Arm Limited (or its affiliates). All rights reserved.
 * Use, modification and redistribution of this file is subject to your possession of a
 * valid End User License Agreement for the Arm Product of which these examples are part of
 * and your compliance with all applicable terms and conditions of such licence agreement.
 */

#include <stdio.h>
#include "modules.h"

void standalone_2(void) {
	printf("Hello and goodbye from standalone 2\n");
}

void direct_chain_end(void){
	printf("End of the chain\n");
}

void indirect_chain_2(void){
	printf("Hello and goodbye from indirect chain 2\n");
	indirect_chain_2_to_3();
}

void fp_target_2(int arg){
	printf("hello and goodbye from fp_target_2(%d)\n", arg);
}

void fp_call_2(demo_function_pointer fp){
	printf("Calling function pointer with arg 1002\n");
	fp(1002);
}

void interworking_chain_1a(void){
	printf("Hello and goodbye from interworking chain 1a\n");
	interworking_chain_1t();
}
