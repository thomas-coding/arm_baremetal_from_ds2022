# optimization1_AC6 Example Makefile
#
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


TARGET=optimization1_AC6.axf

CC=armclang
LD=armlink

CFLAGS = --target=arm-arm-none-eabi -mcpu=cortex-a7 -mfpu=none -mfloat-abi=soft -marm -c -g
OPT_LEVEL = 3

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

sorts.o: sorts.c
	$(CC) -O$(OPT_LEVEL) $(CFLAGS) sorts.c 

perf.o: perf.c v7_pmu.h
	$(CC) -O$(OPT_LEVEL) $(CFLAGS)  perf.c

startup.o: startup.S
	$(CC) $(CFLAGS) -masm=auto startup.S

v7_pmu.o: v7_pmu.s
	$(CC) $(CFLAGS) -masm=auto v7_pmu.s

$(TARGET): startup.o v7_pmu.o perf.o sorts.o scatter.scat
	$(LD) startup.o v7_pmu.o perf.o sorts.o --scatter=scatter.scat -o $(TARGET) --entry=Vectors --info=sizes,totals,unused,inline --list=list.txt

