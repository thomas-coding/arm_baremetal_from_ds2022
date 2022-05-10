/*----------------------------------------------------------------------------
 * Name:    Serial.h
 * Purpose: Low level serial definitions
 * Note(s):
 *----------------------------------------------------------------------------
 * Copyright (c) 2010 Arm Limited (or its affiliates). All rights reserved.
 * Use, modification and redistribution of this file is subject to your possession of a
 * valid End User License Agreement for the Arm Product of which these examples are part of 
 * and your compliance with all applicable terms and conditions of such licence agreement.
 *----------------------------------------------------------------------------*/

#ifndef __SERIAL_H
#define __SERIAL_H

extern void SER_Init      (void);
extern int  SER_GetChar   (void);
extern int  SER_PutChar   (int c);
extern int  SER_CheckChar (void);

#endif
