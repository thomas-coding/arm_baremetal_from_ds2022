/*----------------------------------------------------------------------------
 * Name:    LED.h
 * Purpose: low level LED definitions
 * Note(s):
 *----------------------------------------------------------------------------
 * Copyright (c) 2009-2011 Arm Limited (or its affiliates). All rights reserved.
 * Use, modification and redistribution of this file is subject to your possession of a
 * valid End User License Agreement for the Arm Product of which these examples are part of 
 * and your compliance with all applicable terms and conditions of such licence agreement.
 *----------------------------------------------------------------------------*/

#ifndef __LED_H
#define __LED_H

/* LED Definitions */
#define LED_NUM     8                        /* Number of user LEDs          */

extern void LED_Init(void);
extern void LED_On  (unsigned int num);
extern void LED_Off (unsigned int num);
extern void LED_Out (unsigned int value);

#endif
