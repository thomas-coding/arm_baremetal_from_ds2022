STM Example
===========

Copyright (c) 2015-2019 Arm Limited (or its affiliates). All rights reserved.
Use, modification and redistribution of this file is subject to your possession of a
valid End User License Agreement for the Arm Product of which these examples are part of
and your compliance with all applicable terms and conditions of such licence agreement.


This STM example accompanies this blog article, that should be referred to for more information:
https://community.arm.com/developer/tools-software/tools/b/tools-software-ides-blog/posts/introduction-to-arm-s-system-trace-macrocell

This example intentionally omits platform boot code, or any other startup
sequencing but has been written to operate on the STM32MP15x-EVAL board and
is targeted at the Cortex-A7.
 
It is designed to demonstrate that the STM is operational.

This project uses the scatter.scat file to set load location, as well as 
heap and stack values.
It is written to run from the SRAM at 0x10000000.
You may need to disable the MMU prior to running.
A script (setup.ds) is included that halts the core then disables the MMU plus I&C caches.

It has been tested with the STM32MP15x board in 'Engineering' boot mode (i.e. both MPU and MCU
complexes powered up) and in 'SDCard' boot mode, with either a valid u-boot or Linux kernel booted.
Either of these pre-requisites are required in order to access the STM via the main system memory bus.

If running in 'Engineering' mode on Cortex-A7_0, then the memory accesses will be in Secure world, so 
the Master channel in use will be 0x00.
If using Cortex-A7_1 under the same circumstances then the Master channel will be 0x01

If running when booting from flash or SDCard such that the Cortex-A7 is in the Normal world 
then for Cortex-A7_0 the Master ID will be 0x40.  Similarly for Cortex-A7_1 it will be 0x41.  


Other Notes
===========

It can be run under an operating system as long as the constants in platform.h
reflect the memory regions for the STM peripheral. However this has not been 
tested on the STM32MP15x.

Obviously the preferred method of using the STM would be via Linux drivers
(see the LKML and Linaro for patches) or via CoreSight Access Library (CSAL).
However, those frameworks also involve far more than simply showing the STM
working.

Configuring Trace
-----------------

The example assumes that the Debugger can appropriately set up trace. The code is
specifically written not to overwrite an existing, valid, STMTCSR.TRACEID value,
so the Debugger is in full control of the trace infrastructure.

See the companion blog article for examples.

Explanation of files
--------------------

main.c:
    code entry point. Initializes the STM and jumps to "cambridge" and "primes"
    example code

cambridge.{c,h}:
    prints the string "Cambridge" per the blog article

primes.{c,h}:
    standard "primes" example, modified to emit instrumentation via STM

stm.{c,h}:
    platform-agnostic STM functionality (based on STM-500 implementation)

platform.h:
    minimal platform support and configuration

