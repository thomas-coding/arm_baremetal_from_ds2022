/******************************************************************************
 * @file:    system_stm32f10x_cl.h
 * @purpose: CMSIS Cortex-M3 Device Peripheral Access Layer Header File for the
 *           ST STM32 Connectivity Line Device Series
 * @version: V1.01
 * @date:    22. Jul. 2009
 *----------------------------------------------------------------------------
 * Copyright (c) 2009 Arm Limited (or its affiliates). All rights reserved.
 * Use, modification and redistribution of this file is subject to your possession of a
 * valid End User License Agreement for the Arm Product of which these examples are part of 
 * and your compliance with all applicable terms and conditions of such licence agreement.
 ******************************************************************************/


#ifndef __SYSTEM_STM32F10x_CL_H
#define __SYSTEM_STM32F10x_CL_H

#ifdef __cplusplus
 extern "C" {
#endif 

extern uint32_t SystemFrequency;    /*!< System Clock Frequency (Core Clock)  */


/**
 * Initialize the system
 *
 * @param  none
 * @return none
 *
 * @brief  Setup the microcontroller system
 *         Initialize the Embedded Flash Interface,  initialize the PLL and update the SystemFrequency variable
 */
extern void SystemInit (void);

#ifdef __cplusplus
}
#endif

#endif /* __SYSTEM_STM32F10x_CL_H */
