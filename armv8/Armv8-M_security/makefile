# Armv8-M Security Extensions Example Makefile
#
# Copyright (c) 2017-2020 Arm Limited (or its affiliates). All rights reserved.
# Use, modification and redistribution of this file is subject to your possession of a
# valid End User License Agreement for the Arm Product of which these examples are part of 
# and your compliance with all applicable terms and conditions of such licence agreement.
#
# This makefile is intended for use with GNU make
# This example is intended to be built with Arm Compiler for Embedded 6

TARGET_SECURE=./IOTKit_ARMv8MBL_s/IOTKit_ARMv8MBL_s.axf
TARGET_NONSECURE=./IOTKit_ARMv8MBL_ns/IOTKit_ARMv8MBL_ns.axf

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

.phony: all clean

CFLAGS = -xc -std=c11 --target=arm-arm-none-eabi -march=armv8m.base -c -funsigned-char -fshort-enums -fshort-wchar -g -O1 -D_RTE_ -DIOTKit_ARMv8MBL
AFLAGS = --target=arm-arm-none-eabi -march=armv8m.base -c -g -D_RTE_ -DIOTKit_ARMv8MBL
INCLUDES_S  = -I ./Common -I ./IOTKit_ARMv8MBL_s/RTE  -I ./IOTKit_ARMv8MBL_s/RTE/Device/IOTKit_ARMv8MBL
INCLUDES_NS = -I ./Common -I ./IOTKit_ARMv8MBL_ns/RTE -I ./IOTKit_ARMv8MBL_ns/RTE/Device/IOTKit_ARMv8MBL
LFLAGS = --strict --info=summarysizes --map --xref --symbols --info=unused --info=veneers

all:
# Secure
	$(call MD,./IOTKit_ARMv8MBL_s/Objects)
	$(CC) $(AFLAGS) -masm=auto ./IOTKit_ARMv8MBL_s/RTE/Device/IOTKit_ARMv8MBL/startup_IOTKit_ARMv8MBL.s -o ./IOTKit_ARMv8MBL_s/Objects/startup_IOTKit_ARMv8MBL.o
	$(CC) $(CFLAGS) $(INCLUDES_S) -mcmse -o ./IOTKit_ARMv8MBL_s/Objects/main_s.o ./IOTKit_ARMv8MBL_s/main_s.c
	$(CC) $(CFLAGS) $(INCLUDES_S) -mcmse -o ./IOTKit_ARMv8MBL_s/Objects/mpu_s.o ./IOTKit_ARMv8MBL_s/mpu_s.c -Wno-dollar-in-identifier-extension
	$(CC) $(CFLAGS) $(INCLUDES_S) -mcmse -o ./IOTKit_ARMv8MBL_s/Objects/stdout_USART.o ./Common/stdout_USART.c
	$(CC) $(CFLAGS) $(INCLUDES_S) -mcmse -o ./IOTKit_ARMv8MBL_s/Objects/system_IOTKit_ARMv8MBL.o ./IOTKit_ARMv8MBL_s/RTE/Device/IOTKit_ARMv8MBL/system_IOTKit_ARMv8MBL.c
	$(CC) $(CFLAGS) $(INCLUDES_S) -mcmse -o ./IOTKit_ARMv8MBL_s/Objects/retarget_io.o ./Common/retarget_io.c
	$(LD) ./IOTKit_ARMv8MBL_s/Objects/main_s.o ./IOTKit_ARMv8MBL_s/Objects/mpu_s.o ./IOTKit_ARMv8MBL_s/Objects/stdout_USART.o ./IOTKit_ARMv8MBL_s/Objects/retarget_io.o ./IOTKit_ARMv8MBL_s/Objects/startup_IOTKit_ARMv8MBL.o ./IOTKit_ARMv8MBL_s/Objects/system_IOTKit_ARMv8MBL.o --scatter ./IOTKit_ARMv8MBL_s/IOTKit_ARMv8MBL_s.scat --import-cmse-lib-out=./IOTKit_ARMv8MBL_s/Objects/IOTKit_ARMv8MBL_s_CMSE_Lib.o $(LFLAGS) --list=./IOTKit_ARMv8MBL_s/IOTKit_ARMv8MBL_s.map -o $(TARGET_SECURE) --entry=Reset_Handler
# Non-secure
	$(call MD,./IOTKit_ARMv8MBL_ns/Objects)
	$(CC) $(AFLAGS) -masm=auto ./IOTKit_ARMv8MBL_ns/RTE/Device/IOTKit_ARMv8MBL/startup_IOTKit_ARMv8MBL.s -o ./IOTKit_ARMv8MBL_ns/Objects/startup_IOTKit_ARMv8MBL.o
	$(CC) $(CFLAGS) $(INCLUDES_NS) -o ./IOTKit_ARMv8MBL_ns/Objects/main_ns.o ./IOTKit_ARMv8MBL_ns/main_ns.c
	$(CC) $(CFLAGS) $(INCLUDES_NS) -o ./IOTKit_ARMv8MBL_ns/Objects/mpu_ns.o ./IOTKit_ARMv8MBL_ns/mpu_ns.c -Wno-dollar-in-identifier-extension
	$(CC) $(CFLAGS) $(INCLUDES_NS) -o ./IOTKit_ARMv8MBL_ns/Objects/stdout_USART.o ./Common/stdout_USART.c
	$(CC) $(CFLAGS) $(INCLUDES_NS) -o ./IOTKit_ARMv8MBL_ns/Objects/system_IOTKit_ARMv8MBL.o ./IOTKit_ARMv8MBL_ns/RTE/Device/IOTKit_ARMv8MBL/system_IOTKit_ARMv8MBL.c
	$(CC) $(CFLAGS) $(INCLUDES_NS) -o ./IOTKit_ARMv8MBL_ns/Objects/retarget_io.o ./Common/retarget_io.c
	$(LD) ./IOTKit_ARMv8MBL_ns/Objects/main_ns.o ./IOTKit_ARMv8MBL_ns/Objects/mpu_ns.o ./IOTKit_ARMv8MBL_ns/Objects/stdout_USART.o ./IOTKit_ARMv8MBL_ns/Objects/retarget_io.o ./IOTKit_ARMv8MBL_ns/Objects/startup_IOTKit_ARMv8MBL.o ./IOTKit_ARMv8MBL_ns/Objects/system_IOTKit_ARMv8MBL.o --scatter ./IOTKit_ARMv8MBL_ns/IOTKit_ARMv8MBL_ns.scat ./IOTKit_ARMv8MBL_s/Objects/IOTKit_ARMv8MBL_s_CMSE_Lib.o $(LFLAGS) --list=./IOTKit_ARMv8MBL_ns/IOTKit_ARMv8MBL_ns.map -o $(TARGET_NONSECURE) --entry=Reset_Handler

clean:
	$(call RM,./IOTKit_ARMv8MBL_s/Objects/*.o)
	$(call RM,./IOTKit_ARMv8MBL_ns/Objects/*.o)
	$(call RM,$(TARGET_SECURE))
	$(call RM,$(TARGET_NONSECURE))
