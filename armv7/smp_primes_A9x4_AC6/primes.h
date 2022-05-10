// ------------------------------------------------------------
// Prime Number Generator Code
// Header
//
// Copyright (c) 2011-2018 Arm Limited (or its affiliates). All rights reserved.
// Use, modification and redistribution of this file is subject to your possession of a
// valid End User License Agreement for the Arm Product of which these examples are part of 
// and your compliance with all applicable terms and conditions of such licence agreement.
// ------------------------------------------------------------

#ifndef _PRIMES_H
#define _PRIMES_H

#include "MP_Mutexes.h"

// Mutex to regulate access by multiple CPUs to a resource, aligned to cache line size
// extern mutex_t print_lock __attribute__ ((aligned (32)));  // not needed for Arm-supplied C library that handles printf locking itself

// Initializes shared data used during calculation of primes
// Must be called by CPU 0 only, before any CPU calls to calculatePrimes()
void initPrimes(void);

// Starts calculation of primes on that CPU
// Must be called by each participating CPU
void calculatePrimes(unsigned int id);

#endif

// ------------------------------------------------------------
// End of primes.h
// ------------------------------------------------------------