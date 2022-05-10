// ------------------------------------------------------------
// Cortex-A MPCore - Main
//
// Copyright (c) 2011-2014 Arm Limited (or its affiliates). All rights reserved.
// Use, modification and redistribution of this file is subject to your possession of a
// valid End User License Agreement for the Arm Product of which these examples are part of 
// and your compliance with all applicable terms and conditions of such licence agreement.
// ------------------------------------------------------------


#include <stdio.h>

#include "v7.h"
#include "MP_GIC.h"
#include "MP_Mutexes.h"


// compile-time control for the number of CPUs in the cluster
#define nCPUs 4

// per-thread space for each CPU for use by the Arm C libraries - will be zero-initialized on start-up
char user_perthread_libspace[ nCPUs ][96];

// declaration of mutex functions, marked as 'used' to prevent them being removed
int _mutex_initialize(mutex_t *m) __attribute__((used));
void _mutex_acquire(mutex_t *m) __attribute__((used));
void _mutex_release(mutex_t *m) __attribute__((used));

// ------------------------------------------------------------


extern void main_app(void);


// this main() is only executed by CPU 0
__attribute__((noreturn)) int main(void)
{
    sendSGI(0x0, 0x0F, 0x01); // Wake the secondary CPUs by sending SGI (ID 0)

    main_app();

    for(;;) {} //loop forever
}


// Called by the C library to get the address of the per-thread libspace
// Return a separate space for each CPU
void * __user_perthread_libspace(void)
{
  return user_perthread_libspace[ getCPUID() ];
}


// Called by the C library to initialize the mutex to an unlocked state.
// Return a nonzero value to indicate to the C library that it is being used in a multithreaded environment.
int _mutex_initialize(mutex_t *m)
{
  initMutex(m);
  return 1;
}

// Called by the C library to obtain a lock on the mutex.
void _mutex_acquire(mutex_t *m)
{
  lockMutex(m);
}

// Called by the C library to release the lock on the mutex previously acquired
void _mutex_release(mutex_t *m)
{
  unlockMutex(m);
}


// ------------------------------------------------------------
// End of main.c
// ------------------------------------------------------------
