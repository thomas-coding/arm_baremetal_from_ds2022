/* Copyright (c) 2016 Arm Limited (or its affiliates). All rights reserved. */
/* Use, modification and redistribution of this file is subject to your possession of a     */
/* valid End User License Agreement for the Arm Product of which these examples are part of */
/* and your compliance with all applicable terms and conditions of such licence agreement.  */

#ifndef INCLUDED_DEVICE_H
#define INCLUDED_DEVICE_H
/**
 * Gets the unique core number of this MPIDR value
 * from the set [0, NUM_CORES)
 */
unsigned long mpidr_to_core_number(unsigned long mpidr);

/**
 * Gets the cluster number of this MPIDR value
 * from the set [0, NUM_CLUSTERS)
 */
unsigned long mpidr_to_cluster_number(unsigned long mpidr);

#endif
