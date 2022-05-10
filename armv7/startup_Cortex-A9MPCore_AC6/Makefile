# Copyright (c) 2011-2019 Arm Limited (or its affiliates). All rights reserved.
# Use, modification and redistribution of this file is subject to your possession of a
# valid End User License Agreement for the Arm Product of which these examples are part of
# and your compliance with all applicable terms and conditions of such licence agreement.
#
# This makefile is intended to allow command-line users to build this project outside of Eclipse
# This makefile is NOT used by the Eclipse managed builder project (which creates its own makefiles)
#
# This makefile is intended for use with GNU make
# This example is intended to be built with Arm Compiler for Embedded 6


TARGET=startup_Cortex-A9MPCore_AC6.axf

CPU=Cortex-A9

CC=armclang
AS=armclang
LD=armlink
AR=armar
FE=fromelf

CFLAGS=-c -g --target=arm-arm-none-eabi -mcpu=$(CPU) -O1
AFLAGS=-c -g --target=arm-arm-none-eabi -mcpu=$(CPU)

# Select build rules based on Windows or Unix
ifdef WINDIR
DONE=@if exist $(1) echo Build completed.
RM=if exist $(1) del /q $(1)
SHELL=$(WINDIR)\system32\cmd.exe
else
ifdef windir
DONE=@if exist $(1) echo Build completed.
RM=if exist $(1) del /q $(1)
SHELL=$(windir)\system32\cmd.exe
else
DONE=@if [ -f $(1) ]; then echo Build completed.; fi
RM=rm -f $(1)
endif
endif


all: $(TARGET)
	$(call DONE,$(TARGET))

rebuild: clean all

clean:
	$(call RM,*.o)
	$(call RM,$(TARGET))

main.o: main.c v7.h MP_GIC.h MP_Mutexes.h
	$(CC) $(CFLAGS) main.c

sorts.o: sorts.c v7.h
	$(CC) $(CFLAGS) sorts.c

startup.o: startup.S
	$(AS) $(AFLAGS) startup.S

MP_GIC.o: MP_GIC.S
	$(AS) $(AFLAGS) MP_GIC.S

v7.o: v7.S
	$(AS) $(AFLAGS) v7.S

MP_SCU.o: MP_SCU.S
	$(AS) $(AFLAGS) MP_SCU.S

MP_Mutexes.o: MP_Mutexes.S
	$(AS) $(AFLAGS) MP_Mutexes.S

$(TARGET): MP_GIC.o v7.o MP_SCU.o MP_Mutexes.o main.o sorts.o startup.o scatter.scat
	$(LD) MP_GIC.o v7.o MP_SCU.o MP_Mutexes.o main.o sorts.o startup.o -o $(TARGET) --entry=Vectors --scatter=scatter.scat
