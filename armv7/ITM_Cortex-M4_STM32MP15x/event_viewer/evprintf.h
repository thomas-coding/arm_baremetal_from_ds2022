/* ------------------------------------------------------------
 evprintf.h
 Header File

 Copyright (c) 2011 Arm Limited (or its affiliates). All rights reserved.
 Use, modification and redistribution of this file is subject to your possession of a
 valid End User License Agreement for the Arm Product of which these examples are part of 
 and your compliance with all applicable terms and conditions of such licence agreement.
------------------------------------------------------------
*/

#ifndef EVPRINTF_H
#define EVPRINTF_H

struct EVMSG;

void evprintf_init(void);
void evprintf_final(void);
int  evprintf(const char * format, ... );
void evprintf_msgGenerator(EVMSG* pMsg);

#endif

