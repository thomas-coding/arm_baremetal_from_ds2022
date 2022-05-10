STM Example
===========

Copyright (c) 2015-2018 Arm Limited (or its affiliates). All rights reserved.
Use, modification and redistribution of this file is subject to your possession of a
valid End User License Agreement for the Arm Product of which these examples are part of
and your compliance with all applicable terms and conditions of such licence agreement.


This STM example accompanies this blog article, that should be referred to for more information:
https://community.arm.com/developer/tools-software/tools/b/tools-software-ides-blog/posts/introduction-to-arm-s-system-trace-macrocell

This example intentionally omits platform boot code, or any other startup
sequencing but has been written to operate on the Juno Arm Development Platform
(any revision). Since the boot process on Juno varies from revision to revision,
and firmware to firmware, this code overshadows the STM demonstration code.

The supplied project sets Linker options:

    --ro_base=0x80000000 --entry=__main

This makes it a pure semihosted C example loadable into the Juno DRAM.

Running on Juno
---------------

It will suffice to attach to a single core, turn off MMU and caches (set
SCTLR_ELx.{M,I,C} = 0).

Since it is a pure semihosted C example, stack and heap locations must be
configured in the Debugger.

A script (setup.ds) is provided to effect this at EL2, assuming the platform has stopped
at the UEFI prompt.

Porting to your platform of choice relies on implementation of setup code for
MMU, caches etc. as is normal, and defining a few constants in platform.h.

It can be run under an operating system as long as the constants in platform.h
reflect the memory regions for the STM peripheral.

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

