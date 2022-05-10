/* ------------------------------------------------------------
 cortexM4_itm.h
 Header File

 Copyright (c) 2015 Arm Limited (or its affiliates). All rights reserved.
 Use, modification and redistribution of this file is subject to your possession of a
 valid End User License Agreement for the Arm Product of which these examples are part of 
 and your compliance with all applicable terms and conditions of such licence agreement.
------------------------------------------------------------
*/

#ifndef CORTEXM4_ITM_H
#define CORTEXM4_ITM_H

/*-----------------------------------------------------------------------*/
/*                           R E G I S T E R S                           */
/*-----------------------------------------------------------------------*/

/* Where in our address map the ITM register set is located */
#define ITM_BASE_ADDR            0xE0000000

/* Stimulus Ports 0:31 */
#define ITM_STIMULUS_PORTS        ((volatile unsigned int*)(ITM_BASE_ADDR+0x000))
/* Trace Enable */
#define ITM_TRACE_ENABLE         (*(volatile unsigned int*)(ITM_BASE_ADDR+0xE00))
/* Trace Control */
#define ITM_TRACE_CONTROL        (*(volatile unsigned int*)(ITM_BASE_ADDR+0xE80))
/* Integration Write */
#define ITM_INTEGRATION_WRITE    (*(volatile unsigned int*)(ITM_BASE_ADDR+0xEF8))
/* Integration Read */
#define ITM_INTEGRATION_READ     (*(volatile unsigned int*)(ITM_BASE_ADDR+0xEFC))
/* Integration Mode Control */
#define ITM_INTEGRATION_MODE     (*(volatile unsigned int*)(ITM_BASE_ADDR+0xF00))
/* Lock Access */
#define ITM_LOCK_ACCESS          (*(volatile unsigned int*)(ITM_BASE_ADDR+0xFB0))
/* Lock Status */
#define ITM_LOCK_STATUS          (*(volatile unsigned int*)(ITM_BASE_ADDR+0xFB4))

#endif
