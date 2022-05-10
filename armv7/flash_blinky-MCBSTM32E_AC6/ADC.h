/*----------------------------------------------------------------------------
 * Name:    ADC.h
 * Purpose: low level ADC definitions
 * Note(s):
 *----------------------------------------------------------------------------
 * Copyright (c) 2009-2011 Arm Limited (or its affiliates). All rights reserved.
 * Use, modification and redistribution of this file is subject to your possession of a
 * valid End User License Agreement for the Arm Product of which these examples are part of 
 * and your compliance with all applicable terms and conditions of such licence agreement.
 *----------------------------------------------------------------------------*/

#ifndef __ADC_H
#define __ADC_H

#define ADC_VALUE_MAX      (0xFFF)

extern uint16_t AD_last;
extern uint8_t  AD_done;

extern void     ADC_Init    (void);
extern void     ADC_StartCnv(void);
extern void     ADC_StopCnv (void);
extern uint16_t ADC_GetCnv  (void);

#endif
