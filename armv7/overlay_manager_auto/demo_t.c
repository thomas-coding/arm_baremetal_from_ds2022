/* demo_t.c - demo of overlay manager (Thumb functions)
 *
 * Copyright (c) 2016-2017 Arm Limited (or its affiliates). All rights reserved.
 * Use, modification and redistribution of this file is subject to your possession of a
 * valid End User License Agreement for the Arm Product of which these examples are part of 
 * and your compliance with all applicable terms and conditions of such licence agreement.
 */

#include <stdio.h>

#include "ovl_api.h"

#include "demo.h"

/*
 * This is the Thumb half of the interworking test from demo.c.
 */

void interworking_chain_0t(void)
{
    printf("hello from interworking_chain_0t\n");
    interworking_chain_1a();
    printf("goodbye from interworking_chain_0t\n");
}

void interworking_chain_1t(void)
{
    printf("hello from interworking_chain_1t\n");
    interworking_chain_2a();
    printf("goodbye from interworking_chain_1t\n");
}

void interworking_chain_2t(void)
{
    printf("hello from interworking_chain_2t\n");
    interworking_chain_3a();
    printf("goodbye from interworking_chain_2t\n");
}

void interworking_chain_3t(void)
{
    printf("hello from interworking_chain_3t\n");
    interworking_chain_4a();
    printf("goodbye from interworking_chain_3t\n");
}

void interworking_chain_4t(void)
{
    printf("hello from interworking_chain_4t\n");
    interworking_chain_5a();
    printf("goodbye from interworking_chain_4t\n");
}

void interworking_chain_5t(void)
{
    printf("hello from interworking_chain_5t\n");
    interworking_chain_6a();
    printf("goodbye from interworking_chain_5t\n");
}

void interworking_chain_6t(void)
{
    printf("hello from interworking_chain_6t\n");
    interworking_chain_7a();
    printf("goodbye from interworking_chain_6t\n");
}

void interworking_chain_7t(void)
{
    printf("hello from interworking_chain_7t\n");
    interworking_chain_8a();
    printf("goodbye from interworking_chain_7t\n");
}

void interworking_chain_8t(void)
{
    printf("hello from interworking_chain_8t\n");
    interworking_chain_9a();
    printf("goodbye from interworking_chain_8t\n");
}

void interworking_chain_9t(void)
{
    printf("hello from interworking_chain_9t\n");
    interworking_chain_10a();
    printf("goodbye from interworking_chain_9t\n");
}

void interworking_chain_10t(void)
{
    printf("hello from interworking_chain_10t\n");
    interworking_chain_11a();
    printf("goodbye from interworking_chain_10t\n");
}

void interworking_chain_11t(void)
{
    printf("hello from interworking_chain_11t\n");
    interworking_chain_12a();
    printf("goodbye from interworking_chain_11t\n");
}

void interworking_chain_12t(void)
{
    printf("hello and goodbye from interworking_chain_12t\n");
}

