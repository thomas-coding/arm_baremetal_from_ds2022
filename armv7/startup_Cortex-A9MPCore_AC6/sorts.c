/* sorts.c simple sorting program in a Multicore environment */

/* Copyright (c) 2007-2017 Arm Limited (or its affiliates). All rights reserved. */
/* Use, modification and redistribution of this file is subject to your possession of a     */
/* valid End User License Agreement for the Arm Product of which these examples are part of */
/* and your compliance with all applicable terms and conditions of such licence agreement.  */


#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

extern void enable_caches(void);

#include "v7.h"

#define PL310_BASE 0x1E00A000
#define L2CC_COUNTER0  (volatile unsigned int *)(PL310_BASE + 0x210)
#define L2CC_COUNTER1  (volatile unsigned int *)(PL310_BASE + 0x20C)


#define max( a, b ) ( ((a) > (b)) ? (a) : (b) )

#define N               100

#if N > 1000000
#error "Value of N too big, must be <= 1000000"
#endif

#define LOG10_N         6
#define N_FORMAT        "%06d"

#define CORE0           0
#define CORE1           1
#define CORE2           2
#define CORE3           3

/* Shared data */
volatile unsigned int total_time_CPU0 = 0, total_time_CPU1 = 0, total_time_CPU2 = 0, total_time_CPU3 = 0;
volatile unsigned int sync_1 = 0, sync_2 = 0, sync_3 = 0;

#if N <= 10000
static void insert_sort(char *strings[], int n)
{
    char *v, *t;
    char **strp, **endp;
    int i;

    endp = &strings[n-1];
    i = n-2;
    do {
        strp = &strings[i];
        v = strp[0];
        do {
            t = strp[1];
            if (strcmp(v, t) <= 0) break;
            *strp++ = t;
        } while (strp < endp);
        strp[0] = v;
    } while (--i >= 0);
}
#endif

static void shell_sort(char *strings[], int n)
{
    int h, i, j;
    char *v;

    strings--;        /* Make array 1 origin */
    h = 1;
    do {h = h * 3 + 1;} while (h <= n);
    do {
        h = h / 3;
        for (i = h + 1; i <= n; i++) {
            v = strings[i];
            j = i;
            while (j > h && strcmp(strings[j-h], v) > 0) {
                strings[j] = strings[j-h];
                j = j-h;
            }
            strings[j] = v;
        }
    }
    while (h > 1);
}

static void randomise(char *strings[], int n)
{
    int i;
    int v;
    char *t;

    srand(1);  /* for repeatable results.  Or use  srand(clock());  for 'more random' results */
    for (i = 0; i < n; i++) {
        v = rand() % n;
        t = strings[v];
        strings[v] = strings[i];
        strings[i] = t;
    }
}

static void check_order(char *sort_type, char *strings[], int n)
{
    int i;

    for (i = 0; i < n; i++) {
        if (atoi(strings[i]) != i) {
            fprintf(stderr, "%s sort failed - exiting (expected " N_FORMAT ", got %s),\n", sort_type, i, strings[i]);
            exit(1);
        }
    }
}

int qs_string_compare(const void *a, const void *b)
{
    return strcmp(*(char **)a, *(char **)b);
}


// this is executed by all CPUs
__attribute__((noreturn)) void main_app(void)
{
	char *strings_copy[N], *strings[N];
    char buffer[N*(LOG10_N+1)];
    char *p;
    clock_t starttime, endtime;
    int i;
    int max_time;

    unsigned int id;

    enable_caches();

    id = getCPUID();

    printf("CPU %d: Starting Sort algorithm\n", id);

    p = buffer;
    for (i = 0; i < N; i++)
    {
        sprintf(p, N_FORMAT, i);
        strings[i] = p;
        p += LOG10_N+1;
    }
    randomise(strings, N);

    /* Run insert_sort in CORE0 */
    if (CORE0 == id)
    {
        #if N <= 10000
        /* Do insertion sort */
        memcpy(strings_copy, strings, sizeof(strings));
        starttime = clock();
        insert_sort(strings_copy, N);
        endtime = clock();
        check_order("Insertion", strings_copy, N);
        printf("Insertion sort took %d clock ticks in CORE0\n", endtime - starttime);

        total_time_CPU0 = endtime - starttime;

        #else
            printf("Value of N too big to use insertion sort, must be <= 10000\n");
        #endif
    }


    /* Run shell_sort in CORE1 */
    if (CORE1 == id)
    {
        /* Do shell sort */
        memcpy(strings_copy, strings, sizeof(strings));
        starttime = clock();
        shell_sort(strings_copy, N);
        endtime = clock();
        check_order("Shell", strings_copy, N);
        printf("Shell sort took %d clock ticks in CORE1\n", endtime - starttime);
        total_time_CPU1 = endtime - starttime;
        sync_1 = 1;
    }

    /* Run qsort in CORE2 */
    if (CORE2 == id)
    {
        /* Do quick sort - use built-in C library sort */
        memcpy(strings_copy, strings, sizeof(strings));
        starttime = clock();
        qsort(strings_copy, N, sizeof(char *), qs_string_compare);
        endtime = clock();
        check_order("Quick", strings_copy, N);
        printf("Quick sort took %d clock ticks in CORE2\n", endtime - starttime);
        total_time_CPU2 = endtime - starttime;
        sync_2 = 1;
    }

    /* Run shell_sort and qsort in CORE3 */
    if (CORE3 == id)
    {
        /* Do shell sort */
        memcpy(strings_copy, strings, sizeof(strings));
        starttime = clock();
        shell_sort(strings_copy, N);
        endtime = clock();
        check_order("Shell", strings_copy, N);
        printf("Shell sort took %d clock ticks in CORE3\n", endtime - starttime);
        total_time_CPU3 = endtime - starttime;

        /* Do quick sort - use built-in C library sort */
        memcpy(strings_copy, strings, sizeof(strings));
        starttime = clock();
        qsort(strings_copy, N, sizeof(char *), qs_string_compare);
        endtime = clock();
        check_order("Quick", strings_copy, N);
        printf("Quick sort took %d clock ticks in CORE3\n", endtime - starttime);
        total_time_CPU3 += endtime - starttime;
        sync_3 = 1;
    }

    /* Compute the final results in CORE0 */
    if (CORE0 == id)
    {
    	/* Wait for all the CPUs to finish */
    	while ((0 == sync_1) || (0 == sync_2) || (0 == sync_3));

    	printf("Total Time for each CPU:\n");
    	printf("Total Time for CPU0: %d clock ticks\n", total_time_CPU0);
    	printf("Total Time for CPU1: %d clock ticks\n", total_time_CPU1);
    	printf("Total Time for CPU2: %d clock ticks\n", total_time_CPU2);
    	printf("Total Time for CPU3: %d clock ticks\n", total_time_CPU3);
    	printf("-----------------------------------------------------\n");
    	printf("Total Time for Unicore %d clock ticks\n", total_time_CPU0 + total_time_CPU1 + total_time_CPU2 + total_time_CPU3);

    	max_time = max(total_time_CPU0,total_time_CPU1);
    	max_time = max(max_time,total_time_CPU2);
    	max_time = max(max_time,total_time_CPU3);

    	printf("Total Time for Multicore %d clock ticks\n", max_time );
#ifdef USE_L2CC
    	printf("-----------------------------------------------------\n");
    	printf("Data write hits in the L2 cache: %d hits\n", *L2CC_COUNTER0);
    	printf("Data read  hits in the L2 cache: %d hits\n", *L2CC_COUNTER1);
#endif
    }

    for(;;) {} //loop forever
}
