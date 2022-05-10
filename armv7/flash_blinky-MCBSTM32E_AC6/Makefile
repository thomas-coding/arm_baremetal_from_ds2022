# Copyright (c) 2011-2021 Arm Limited (or its affiliates). All rights reserved.
# Use, modification and redistribution of this file is subject to your possession of a
# valid End User License Agreement for the Arm Product of which these examples are part of 
# and your compliance with all applicable terms and conditions of such licence agreement.
#
# This makefile is intended to allow command-line users to build this project outside of Eclipse
# This makefile is NOT used by the Eclipse managed builder project (which creates its own makefiles)
#
# This makefile is intended for use with GNU make
# This example is intended to be built with Arm Compiler 6

TARGET=flash_blinky-MCBSTM32E_AC6.axf

CC=armclang
AS=armclang
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


all: $(TARGET)
	$(call DONE,$(TARGET))

rebuild: clean all

clean:
	$(call RM,*.o)
	$(call RM,$(TARGET))

CFLAGS=--target=arm-arm-none-eabi -mcpu=cortex-m3 -mfpu=none -mfloat-abi=soft -DSTM32F10X_HD -D__MICROLIB -D__USE_LCD -I"./CMSIS" -I"./STM32F10x" -g -c -O1

AFLAGS=--target=arm-arm-none-eabi -mcpu=cortex-m3 -mfpu=none -mfloat-abi=soft -g -c -masm=auto -D__MICROLIB 


ADC.o: ADC.c ADC.h ./STM32F10x/stm32f10x.h
	$(CC) $(CFLAGS) -o $@ ADC.c 

Blinky.o: Blinky.c GLCD.h Serial.h LED.h ADC.h ./STM32F10x/stm32f10x.h
	$(CC) $(CFLAGS) -o $@ Blinky.c

GLCD_16bitIF_STM32.o: GLCD_16bitIF_STM32.c Font_16x24_h.h Font_6x8_h.h GLCD.h ./STM32F10x/stm32f10x.h
	$(CC) $(CFLAGS) -o $@ GLCD_16bitIF_STM32.c

IRQ.o: IRQ.c ADC.h LED.h ./STM32F10x/stm32f10x.h
	$(CC) $(CFLAGS) -o $@ IRQ.c 

LED.o: LED.c LED.h ./STM32F10x/stm32f10x.h
	$(CC) $(CFLAGS) -o $@ LED.c 

Retarget.o: Retarget.c Serial.h
	$(CC) $(CFLAGS) -o $@ Retarget.c 
			
Serial.o: Serial.c Serial.h ./STM32F10x/stm32f10x.h
	$(CC) $(CFLAGS) -o $@ Serial.c 

startup_stm32f10x_hd.o: startup_stm32f10x_hd.s
	$(AS) $(AFLAGS) -o $@ startup_stm32f10x_hd.s 	

system_stm32f10x.o: system_stm32f10x.c ./STM32F10x/stm32f10x.h
	$(CC) -c $(CFLAGS) system_stm32f10x.c 
					
LDFLAGS=--scatter=Blinky.scat --library_type=microlib --info=totals --info=unused

$(TARGET): ADC.o Blinky.o GLCD_16bitIF_STM32.o IRQ.o LED.o Retarget.o Serial.o startup_stm32f10x_hd.o system_stm32f10x.o Blinky.scat
	$(LD)  $(LDFLAGS) -o $(TARGET) ADC.o Blinky.o GLCD_16bitIF_STM32.o IRQ.o LED.o Retarget.o Serial.o startup_stm32f10x_hd.o system_stm32f10x.o 
