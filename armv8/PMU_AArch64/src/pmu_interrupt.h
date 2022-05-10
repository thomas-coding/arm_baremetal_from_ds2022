/* Copyright (c) 2016 Arm Limited (or its affiliates). All rights reserved. */
/* Use, modification and redistribution of this file is subject to your possession of a     */
/* valid End User License Agreement for the Arm Product of which these examples are part of */
/* and your compliance with all applicable terms and conditions of such licence agreement.  */

#ifndef INCLUDED_PMU_INTERRUPT_H
#define INCLUDED_PMU_INTERRUPT_H

/*
 * Initialize the PMU and an interrupt for sampling this core
 */
void initPmuInterrupt(void);

/*
 * Handler for the sample interrupt
 */
void pmuInterruptHandler(void);

/*
 * Handler for the PMU overflow interrupt
 */
void pmuOverflowHandler(void);

#endif
