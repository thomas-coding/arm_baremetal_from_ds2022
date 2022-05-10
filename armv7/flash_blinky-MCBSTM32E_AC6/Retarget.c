/*----------------------------------------------------------------------------
 * Name:    Retarget.c
 * Purpose: 'Retarget' layer for target-dependent low level functions
 * Note(s):
 *----------------------------------------------------------------------------
 * Copyright (c) 2011-2021 Arm Limited (or its affiliates). All rights reserved.
 * Use, modification and redistribution of this file is subject to your possession of a
 * valid End User License Agreement for the Arm Product of which these examples are part of 
 * and your compliance with all applicable terms and conditions of such licence agreement.
 *----------------------------------------------------------------------------*/

#include <stdio.h>
#include "Serial.h"


#if   defined ( __ARMCC_VERSION )
asm(" .global __use_no_semihosting");

struct __FILE { int handle; /* Add whatever you need here */ };
FILE __stdout;
FILE __stdin;


int fputc(int c, FILE *f) {
  return (SER_PutChar(c));
}


int fgetc(FILE *f) {
  return (SER_GetChar());
}


int ferror(FILE *f) {
  /* Your implementation of ferror */
  return EOF;
}


void _ttywrch(int c) {
  SER_PutChar(c);
}


void _sys_exit(int return_code) {
label:  goto label;  /* endless loop */
}

#elif defined ( __GNUC__ )

int _write_r(void *reent, int fd, char *ptr, size_t len)
{

    size_t i;

    for (i=0; i<len; i++)

    {

       SER_PutChar(ptr[i]);

    }

    return len;

}
#endif

