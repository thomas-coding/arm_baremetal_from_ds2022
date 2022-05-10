# Calendar Example Makefile
#
# Copyright (c) 2009-2022 Arm Limited (or its affiliates). All rights reserved.
# Use, modification and redistribution of this file is subject to your possession of a
# valid End User License Agreement for the Arm Product of which these examples are part of 
# and your compliance with all applicable terms and conditions of such licence agreement.
#
# This makefile is intended for use with GNU make
# This example is intended to be built with Arm Compiler for Embedded 6

TARGET=calendar_Armv8-A_AC6.axf


CC=armclang
LD=armlink

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

calendar.o: calendar.c
	$(CC) -c --target=aarch64-arm-none-eabi -march=armv8-a -g -O1 $^ -o $@

$(TARGET): calendar.o
# Link with specific base address to suit Base FVP model memory layout
	$(LD) --ro-base=0x80000000 $^ -o $@
