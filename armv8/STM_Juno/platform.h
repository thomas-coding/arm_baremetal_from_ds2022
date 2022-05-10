/*
 * STM Example
 *
 * Copyright (c) 2015-2017 Arm Limited (or its affiliates). All rights reserved.
 * Use, modification and redistribution of this file is subject to your possession of a
 * valid End User License Agreement for the Arm Product of which these examples are part of 
 * and your compliance with all applicable terms and conditions of such licence agreement.
 *
 */

#if !defined(__PLATFORM_H)
#define __PLATFORM_H

#define  PLATFORM_STM_AXI_ADDRESS    0x28000000ULL
#define  PLATFORM_STM_APB_ADDRESS    0x20100000ULL

/*
 * Required
 *
 * Timestamp frequency is more a hint to trace decoders
 * than any functional change in the STM itself. It allows
 * conversion of the output TS packets from raw binary
 * to hh:mm:ss.
 *
 * The frequency of sync generation will be handled in
 * hardware but its behaviour is somewhat undefined. By
 * starting trace you will get an ASYNC packet, but we
 * want it to be regular in case our output buffer wraps.
 *
 * By giving these values we get more predictable trace
 * decode.
 */
#define  CONFIG_STM_TIMESTAMP_FREQUENCY  100000000L
#define  CONFIG_STM_SYNC_FREQUENCY       1024

/*
 * Optional
 *
 * Compression is optional.
 *
 * Setting the Trace ID is *not* optional, but the
 * responsibility of setting it could be to the software
 * or an external debugger. Therefore, we only set the
 * value if an external debugger is not going to do it
 * for us.
 */
#define  CONFIG_STM_USE_COMPRESSION  1
#define  CONFIG_STM_USE_TRACEID      16

#endif // __PLATFORM_H
