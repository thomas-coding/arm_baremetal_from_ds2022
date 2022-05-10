//
// Aliases to map gld symbols to armlink symbols
//
// Copyright (c) 2016-2017 Arm Limited (or its affiliates). All rights reserved.
// Use, modification and redistribution of this file is subject to your possession of a
// valid End User License Agreement for the Arm Product of which these examples are part of 
// and your compliance with all applicable terms and conditions of such licence agreement.
//

#ifdef __ARMCC_VERSION /* armlink */

#define __el3_stack       Image$$EL3_STACKS$$ZI$$Limit
#define __stack           Image$$ARM_LIB_STACK$$ZI$$Limit
#define __ttb0_l1         Image$$TTB0_L1$$ZI$$Base
#define __ttb0_l2_ram     Image$$TTB0_L2_RAM$$ZI$$Base
#define __ttb0_l2_periph  Image$$TTB0_L2_PERIPH$$ZI$$Base
#define __code_start      Image$$EXEC$$RO$$Base
#define __top_of_ram      Image$$TOP_OF_RAM$$ZI$$Base
#define __cs3_peripherals Image$$CS3_PERIPHERALS$$ZI$$Base

#endif
