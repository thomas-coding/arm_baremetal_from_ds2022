# Optimization2 Example Makefile
#
# Copyright (c) 2011-2020 Arm Limited (or its affiliates). All rights reserved.
# Use, modification and redistribution of this file is subject to your possession of a
# valid End User License Agreement for the Arm Product of which these examples are part of 
# and your compliance with all applicable terms and conditions of such licence agreement.
#
# This makefile is intended for use with GNU make
# This example is intended to be built with Arm Compiler for Embedded 6

TARGET=optimization2_AC6.axf

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


all: $(TARGET)
	$(call DONE,$(TARGET))

rebuild: clean all

clean:
	$(call RM,*.o)
	$(call RM,$(TARGET))


#Link time optimization is invoked using the -flto flag in the compile (CC) step and needs the --lto flag in the linking step

$(TARGET): testmain.c testsub.c
# Compile 
	$(CC) -c -g --target=arm-arm-none-eabi -mcpu=cortex-a7 -mfloat-abi=hard -O0 -marm -xc -std=c99 -fno-lto testmain.c testsub.c
# Link 
	$(LD) testmain.o testsub.o -o $(TARGET) --info=unused,inline,totals --ro-base=0x80000000 --no_lto --no_remove --entry=StartHere

