// ------------------------------------------------------------
// Prime Number Generator Code
//
// Copyright (c) 2011-2018 Arm Limited (or its affiliates). All rights reserved.
// Use, modification and redistribution of this file is subject to your possession of a
// valid End User License Agreement for the Arm Product of which these examples are part of 
// and your compliance with all applicable terms and conditions of such licence agreement.
// ------------------------------------------------------------

#include <stdio.h>
#include <arm_acle.h>

#include "primes.h"
#include "./event_viewer/evITM.h"
#include "./event_viewer/evprintf.h"


#define RAW_ITM_CHANNEL 1 /* The ITM channel we use for raw text */
//#define EV_ITM_CHANNEL 2  /* The ITM channel we use for encoded events */

#define TARGET_COUNT (100) // Number of primes to find

// ------------------------------------------------------------

static unsigned int prime_numbers[TARGET_COUNT];
static unsigned int next_number;
static unsigned int prime_count;

// Align the mutexes to the cache line size
// mutex_t print_lock __attribute__ ((aligned (32)));  // not needed for Arm-supplied C library that handles printf locking itself
mutex_t ITM_lock   __attribute__ ((aligned (32)));

// ------------------------------------------------------------

static unsigned int getNextNumber(void)
{
  return __atomic_fetch_add(&next_number, 2, __ATOMIC_RELAXED);
}

// ------------------------------------------------------------

static void addPrime(unsigned int number, unsigned int id)
{
  char outputStr[80];

  unsigned int current_prime_count = __atomic_fetch_add(&prime_count, 1, __ATOMIC_RELAXED);

  // It is possible a CPU could skid past the target number of primes
  // so check to avoid potential writes past the end of the array
  if (current_prime_count < TARGET_COUNT)
  {
    prime_numbers[current_prime_count] = number;

  //  lockMutex(&print_lock);     // not needed for Arm-supplied C library that handles printf locking itself
    printf("CPU %d: %d (prime %d of %d)\n", id, number, current_prime_count, TARGET_COUNT);
  //  unlockMutex(&print_lock);

    sprintf(outputStr, "CPU %d: %d (prime %d of %d)\n", id, number, prime_count, TARGET_COUNT);

    lockMutex(&ITM_lock);
    // Writes a RAW string to the given port
    evITM_writeStringToPort(RAW_ITM_CHANNEL, outputStr);

    // EventViewer printf implementation
    evprintf(outputStr);
    evITM_pump();
    unlockMutex(&ITM_lock);
  }

  return;
}

// ------------------------------------------------------------

void initPrimes(void)
{
//  initMutex(&print_lock);     // not needed for Arm-supplied C library that handles printf locking itself
  initMutex(&ITM_lock);

  // Give it the first few primes because there is no guarantee
  // from here on the primes will be stored in order.
  // The algorithm uses all the primes up to the square root of the number being tested,
  // This guarantees we will have no problem finding primes up to 841
  prime_numbers[0] = 2;
  prime_numbers[1] = 3;
  prime_numbers[2] = 5;
  prime_numbers[3] = 7;
  prime_numbers[4] = 11;
  prime_numbers[5] = 13;
  prime_numbers[6] = 17;
  prime_numbers[7] = 19;
  prime_numbers[8] = 23;
  prime_numbers[9] = 29;
  next_number = 31;
  prime_count = 10;

  return;
}

// ------------------------------------------------------------

void calculatePrimes(unsigned int id)
{
  int number;
  int square;
  int remainder;
  int root = 1;
  int prime;
  int i;
  unsigned int local_prime_count;

  for(local_prime_count = __atomic_load_n(&prime_count, __ATOMIC_RELAXED);
      local_prime_count < TARGET_COUNT;
      local_prime_count = __atomic_load_n(&prime_count, __ATOMIC_RELAXED))
  {
    // Get the next number
    number = getNextNumber();

    square = root * root;

    while(number > square)
    {
      root++;
      square = root * root;
    }

    for(i=1; i < local_prime_count; i++)
    {
      prime = prime_numbers[i];

      if (prime > root)
      {
        addPrime(number, id);
        break;
      }

      remainder = number % prime;

      if (remainder == 0)
      {
        // not a prime, so discard number
        break;
      }
    }

    // Not holding any locks
    // Delay to give other cores a chance, because semihosting is quite slow
    for( i=0; i <= 100000; i++) { __nop(); }

  }
  return;
}

// ------------------------------------------------------------
// End of primes.c
// ------------------------------------------------------------
