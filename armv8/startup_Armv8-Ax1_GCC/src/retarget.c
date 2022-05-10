/*
 * Copyright (c) 2016 Arm Limited (or its affiliates). All rights reserved.
 * Use, modification and redistribution of this file is subject to your possession of a
 * valid End User License Agreement for the Arm Product of which these examples are part of 
 * and your compliance with all applicable terms and conditions of such licence agreement.
 */

/*
 * This file contains re-implementations of functions whose
 * C library implementations rely on semihosting.
 *
 * Define USE_SERIAL_PORT to retarget the I/O only to the serial port.
 * Otherwise, I/O is targeted to the debugger console using semihosting.
 *
 * Define STANDALONE to eliminate all use of semihosting-using functions too.
 */

#include <stdio.h>
#include <sys/stat.h>
#include <sys/errno.h>


#ifdef STANDALONE
#define USE_SERIAL_PORT 1
#endif


#ifdef USE_SERIAL_PORT
#include "uart.h"

void initialise_monitor_handles()
{
    UartInit();
}

int _read(int file, char *ptr, int len)
{
    int i;
    for (i = 0; i < len; ++i)
    {
        ptr[i] = uart_getchar_polled();
    }

    return len;
}

int _write(int file, char *ptr, int len)
{
    int i;
    for (i = 0; i < len; ++i)
    {
        if (ptr[i] == '\n')
        {
            uart_putc_polled('\r');
        }
        uart_putc_polled(ptr[i]);
    }

    return len;
}

int _lseek(int file, int ptr, int dir)
{
    errno = ESPIPE;
    return -1;
}

int _close(int file)
{
    return -1;
}

int _fstat(int file, struct stat *st)
{
    st->st_mode = S_IFCHR;
    return 0;
}

int _isatty(int fd)
{
    errno = ENOTTY;
    return 0;
}

#endif  // USE_SERIAL_PORT

#ifdef STANDALONE

void* _sbrk(int increment)
{
    extern char end; // From linker script
    static char* heap_end = &end;

    char* current_heap_end = heap_end;
    heap_end += increment;
    return current_heap_end;
}

void _exit(int return_value)
{
    asm ("dsb sy");
    while (1)
    {
        asm ("wfi");
    }
}

int _getpid(void)
{
  return 1;
}

int _kill(int pid, int sig)
{
    errno = EINVAL;
    return -1;
}

#endif // STANDALONE
