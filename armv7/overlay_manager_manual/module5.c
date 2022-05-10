/* module5.c - module containing some overlaid functions (Thumb)
 *
 * Copyright (c) 2021 Arm Limited (or its affiliates). All rights reserved.
 * Use, modification and redistribution of this file is subject to your possession of a
 * valid End User License Agreement for the Arm Product of which these examples are part of
 * and your compliance with all applicable terms and conditions of such licence agreement.
 */

#include <stdio.h>
#include "modules.h"

void interworking_chain_0t(void){
	printf("Hello and goodbye from interworking chain 0t\n");
	interworking_chain_1a();
}
void interworking_chain_1t(void){
	printf("Hello and goodbye from interworking chain 1t\n");
	interworking_chain_2a();
}
