# Copyright (c) 2010-2021 Arm Limited (or its affiliates). All rights reserved.
# Use, modification and redistribution of this file is subject to your possession of a
# valid End User License Agreement for the Arm Product of which these examples are part of 
# and your compliance with all applicable terms and conditions of such licence agreement.
#
# This makefile is intended to allow command-line users to build this project outside of Eclipse
# This makefile is NOT used by the Eclipse managed builder project (which creates its own makefiles)
#
# This makefile is intended for use with GNU make
# This example is intended to be built with Arm Compiler for Embedded 6


TARGET=startup_Cortex-R7_AC6.axf

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


CORE=Cortex-R7
# If hardware floating point is either not present or not required, add -mfpu=none to CCOPT
CCOPT =-g -O1 -mthumb -mfpu=vfpv3-d16-fp16 -mfloat-abi=hard

# Enable use of TCM in assembler code with -DTCM

$(TARGET) : main.o sorts.o startup.o scatter.scat
	$(LD) main.o sorts.o startup.o --scatter=scatter.scat --entry=Start -o $(TARGET) --info=totals --info=unused

main.o : main.c
	$(CC) -c --target=arm-arm-none-eabi -mcpu=$(CORE) $(CCOPT) main.c

sorts.o : sorts.c
	$(CC) -c --target=arm-arm-none-eabi -mcpu=$(CORE) $(CCOPT) sorts.c

startup.o : startup.s
	$(CC) -c --target=arm-arm-none-eabi -mcpu=$(CORE) $(CCOPT) -x assembler-with-cpp startup.s
