# Copyright (c) 2015-2021 Arm Limited (or its affiliates). All rights reserved.
# Use, modification and redistribution of this file is subject to your possession of a
# valid End User License Agreement for the Arm Product of which these examples are part of 
# and your compliance with all applicable terms and conditions of such licence agreement.
#
# This makefile is intended to allow command-line users to build this project outside of Eclipse
# This makefile is NOT used by the Eclipse managed builder project (which creates its own makefiles)
#
# This makefile is intended for use with GNU make
# This example is intended to be built with Arm Compiler for Embedded 6


TARGET=ITM_Cortex-M4_MPS2.axf

CC=armclang
LD=armlink
AR=armar
FE=fromelf

CFLAGS= --target=arm-arm-none-eabi -mcpu=cortex-m4 -mfpu=none -mfloat-abi=soft -mthumb -g -O1
OBJS=$(patsubst %.c,%.o,$(wildcard *.c))

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

$(OBJS): %.o: %.c 
	$(CC) -c $(CFLAGS) $< -o $@

$(TARGET): $(OBJS) scatter.scat
	$(LD) $(OBJS) --scatter=scatter.scat -o $(TARGET)
