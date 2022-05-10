/* ------------------------------------------------------------
 evITM.h
 Header File

 Copyright (c) 2011 Arm Limited (or its affiliates). All rights reserved.
 Use, modification and redistribution of this file is subject to your possession of a
 valid End User License Agreement for the Arm Product of which these examples are part of 
 and your compliance with all applicable terms and conditions of such licence agreement.
------------------------------------------------------------
*/

#ifndef EVITM_H
#define EVITM_H

#include "evMsg.h"

/* The Coresight ATB ID we use for the ITM component */
#define ITM_ATB_ID 1
/* The number of words to attempt to write per call to evITM_pump */
#define EV_ITM_WORDS_TO_WRITE 50

/* The function type we call to collect messages */
typedef void (*EV_MSGGEN)(EVMSG* pMsg);

void evITM_init(EV_MSGGEN msgGenerators[], unsigned int evITMPortChannel, unsigned int ITMPortMask);
void evITM_pump(void);
void evITM_writeStringToPort(unsigned port, const char *string);

#endif


