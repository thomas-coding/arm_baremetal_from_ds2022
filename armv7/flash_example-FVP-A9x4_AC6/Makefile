# Copyright (c) 2009-2021 Arm Limited (or its affiliates). All rights reserved.
# Use, modification and redistribution of this file is subject to your possession of a
# valid End User License Agreement for the Arm Product of which these examples are part of 
# and your compliance with all applicable terms and conditions of such licence agreement.
#
# This makefile is intended to allow command-line users to build this project outside of Eclipse
# This makefile is NOT used by the Eclipse managed builder project (which creates its own makefiles)
#
# This makefile is intended for use with GNU make
# This example is intended to be built with Arm Compiler 6

CC=armclang
LD=armlink
AR=armar

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

TARGET=configdb/Flash/algorithms/FVP_VE_Cortex-A9x4.FLM

all: $(TARGET)
	$(call DONE,$(TARGET))

rebuild: clean all

clean:
	$(call RM,*.o)
	$(call RM,$(TARGET))

CFLAGS=--target=arm-arm-none-eabi -mcpu=Cortex-A9 -mfpu=none -O1 -g -fropi -frwpi

FlashDev.o: FlashDev.c FlashOS.h
	$(CC) -c $(CFLAGS) FlashDev.c

FlashPrg.o: FlashPrg.c FlashOS.h
	$(CC) -c $(CFLAGS) FlashPrg.c

LDFLAGS=--info=totals --info=unused --diag_suppress=L6305

$(TARGET): FlashDev.o FlashPrg.o Target.scat
	$(LD) FlashDev.o FlashPrg.o --scatter=Target.scat -o $(TARGET) $(LDFLAGS)
