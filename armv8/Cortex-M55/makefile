# Cortex-M55 Example Makefile
#
# Copyright (c) 2020-2021 Arm Limited (or its affiliates). All rights reserved.
# Use, modification and redistribution of this file is subject to your possession of a
# valid End User License Agreement for the Arm Product of which these examples are part of
# and your compliance with all applicable terms and conditions of such licence agreement.
#
# This makefile is intended for use with GNU make
# This example is intended to be built with Arm Compiler for Embedded 6

TARGET=Cortex-M55.axf

CC=armclang
LD=armlink
AR=armar
FE=fromelf

# Select build rules based on Windows or Unix
ifdef WINDIR
WINPATH=$(subst /,\,$(1))
DONE=@if exist $(call WINPATH,$(1)) echo Build completed.
RM=if exist $(call WINPATH,$(1)) del /q $(call WINPATH,$(1))
MD=if not exist $(call WINPATH,$(1)) mkdir $(call WINPATH,$(1))
SHELL=$(WINDIR)\system32\cmd.exe
else
ifdef windir
WINPATH=$(subst /,\,$(1))
DONE=@if exist $(call WINPATH,$(1)) echo Build completed.
RM=if exist $(call WINPATH,$(1)) del /q $(call WINPATH,$(1))
MD=if not exist $(call WINPATH,$(1)) mkdir $(call WINPATH,$(1))
SHELL=$(windir)\system32\cmd.exe
else
DONE=@if [ -f $(1) ]; then echo Build completed.; fi
RM=rm -f $(1)
MD=@if [ ! -d $(1) ]; then mkdir $(1); fi
endif
endif

C_OBJS_DIR = obj
CMSIS_OBJS_DIR = obj

.phony: all clean

CMSIS_PACK ?=/path/to/ARM/CMSIS/5.8.0

ifeq ($(wildcard $(CMSIS_PACK)),)
$(error The CMSIS Pack folder was not found at CMSIS_PACK=$(CMSIS_PACK).  To build this example, install the ARM.CMSIS.5.8.0 (or later) Pack using the Pack Manager, then set CMSIS_PACK to its folder path)
endif

CFLAGS = --target=arm-arm-none-eabi -mcpu=Cortex-M55 -g -Os -fvectorize -ffp-mode=fast \
	-mfloat-abi=hard -fshort-enums -fshort-wchar \
	-D_RTE_ -DARMCM55 \
	-I$(CMSIS_PACK)/Device/ARM/ARMCM55/Include \
	-I$(CMSIS_PACK)/CMSIS/Core/Include

ASM_FLAGS = --target=arm-arm-none-eabi -mcpu=Cortex-M55 -g \
	-D_RTE_ -DARMCM55 \
	-I$(CMSIS_PACK)/Device/ARM/ARMCM55/Include \
	-I$(CMSIS_PACK)/CMSIS/Core/Include

LDFLAGS = --entry=Reset_Handler --scatter=./RTE/Device/ARMCM55/ARMCM55_ac6.sct

all: $(TARGET)

# User source files.
C_SRCS := $(wildcard *.c)
C_OBJS := $(C_SRCS:%.c=%.o)

# CMSIS source files.
startup_ARMCM55.o: ./RTE/Device/ARMCM55/startup_ARMCM55.c
	$(CC) $(CFLAGS) -c -o $@ $<

system_ARMCM55.o: ./RTE/Device/ARMCM55/system_ARMCM55.c
	$(CC) $(CFLAGS) -c -o $@ $<


$(TARGET): $(OBJS_DIR) $(C_OBJS) $(CMSIS_OBJS) startup_ARMCM55.o system_ARMCM55.o
	armlink $(LDFLAGS) $(C_OBJS) $(CMSIS_OBJS) startup_ARMCM55.o system_ARMCM55.o -o $(TARGET)

clean:
	$(call RM,*.o)
	$(call RM,$(TARGET))
