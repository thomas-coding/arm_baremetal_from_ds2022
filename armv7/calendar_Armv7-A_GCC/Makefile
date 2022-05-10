# Calendar Example Makefile
#
# Copyright (c) 2009-2020 Arm Limited (or its affiliates). All rights reserved.
# Use, modification and redistribution of this file is subject to your possession of a
# valid End User License Agreement for the Arm Product of which these examples are part of 
# and your compliance with all applicable terms and conditions of such licence agreement.
#
# This makefile is intended for use with GNU make
# This example is intended to be built with GCC 10.3 for bare-metal targets

TARGET=calendar_Armv7-A_GCC.axf


CC=arm-none-eabi-gcc
LD=arm-none-eabi-gcc

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
	$(call RM,linkmap.txt)

calendar.o: calendar.c
	$(CC) -c -mcpu=Cortex-A9 -g -O1 $^ -o $@

$(TARGET): calendar.o
# Link with specific base address to suit VE model memory layout
	$(LD) --specs=aprofile-ve.specs -Wl,--build-id=none,-Map=linkmap.txt calendar.o -o $@
