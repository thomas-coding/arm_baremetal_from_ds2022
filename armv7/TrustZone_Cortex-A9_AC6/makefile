# TrustZone Example Makefile
#
# Copyright (c) 2011-2021 Arm Limited (or its affiliates). All rights reserved.
# Use, modification and redistribution of this file is subject to your possession of a
# valid End User License Agreement for the Arm Product of which these examples are part of 
# and your compliance with all applicable terms and conditions of such licence agreement.
#
# This makefile is intended for use with GNU make
# This example is intended to be built with Arm Compiler for Embedded 6

TARGET=TrustZone_Cortex-A9_AC6.axf

CC=armclang
LD=armlink
AR=armar
FE=fromelf

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

AFLAGS = -c --target=arm-arm-none-eabi -mcpu=Cortex-A9 -g -masm=auto
CFLAGS = -c --target=arm-arm-none-eabi -mcpu=Cortex-A9 -g -O1
all: $(TARGET)
	$(call DONE,$(TARGET))

rebuild: clean all

clean:
	$(call RM,*.o)
	$(call RM,$(TARGET))
	$(call RM,normal.axf)
	$(call RM,normal.bin)


$(TARGET): startup_normal.s main_normal.c scatter_normal.scat startup_secure.s main_secure.c bp147_tzpc.c bp147_tzpc.h monitor.s scatter_secure.scat
# Assemble common routines
	$(CC) $(AFLAGS) v7.s -o v7.o
# Compile normal world code
	$(CC) $(AFLAGS) startup_normal.s
	$(CC) $(CFLAGS) main_normal.c
# Link normal world code and create binary
	$(LD) main_normal.o startup_normal.o v7.o --scatter=scatter_normal.scat --entry=normalStart -o normal.axf
	$(FE) --bin normal.axf -o normal.bin
# Compile secure world code
	$(CC) $(AFLAGS) startup_secure.s
	$(CC) $(AFLAGS) monitor.s
	$(CC) $(CFLAGS) main_secure.c
	$(CC) $(CFLAGS) bp147_tzpc.c -O1
# Link final executable (secure + normal)
	$(LD) main_secure.o startup_secure.o v7.o monitor.o bp147_tzpc.o --scatter=scatter_secure.scat --entry=secureStart --keep="startup_secure.o(NORMAL_IMAGE)" -o $(TARGET)
