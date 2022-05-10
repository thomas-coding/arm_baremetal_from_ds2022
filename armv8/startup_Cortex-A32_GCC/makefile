# Copyright (c) 2007-2020 Arm Limited (or its affiliates). All rights reserved.
# Use, modification and redistribution of this file is subject to your possession of a
# valid End User License Agreement for the Arm Product of which these examples are part of 
# and your compliance with all applicable terms and conditions of such licence agreement.
#
# This example is intended to be built with GCC 10.3 for bare-metal targets
#
# Environment variables for build options that the user might wish to change
#
# Variable     Example Value
# ----------   -------------
# APP          myapp.axf
# OPT_LEVEL    0, 1, 2 or 3
# CODE_TYPE    arm or thumb
# DEFINES      -D MYDEFINE

APP ?= startup_Cortex-A32_GCC.axf
OPT_LEVEL ?= 1
CODE_TYPE ?= arm

# Other switches the user should not normally need to change:
CPU = cortex-a32
DEBUG_FLAGS = -g

CC = arm-none-eabi-gcc

define EOL =


endef

ifeq ($(OS),Windows_NT)
SHELL=$(windir)\system32\cmd.exe
RM_FILES = $(foreach file,$(1),if exist $(file) del /q $(file)$(EOL))
RM_DIRS = $(foreach dir,$(1),if exist $(dir) rmdir /s /q $(dir)$(EOL))
else
RM_FILES = $(foreach file,$(1),rm -f $(file)$(EOL))
RM_DIRS = $(foreach dir,$(1),rm -rf $(dir)$(EOL))
endif

DEPEND_FLAGS = -MD

CPPFLAGS = $(DEFINES) $(INCLUDES) $(DEPEND_FLAGS)
CFLAGS = $(DEBUG_FLAGS) -O$(OPT_LEVEL)
ASFLAGS = $(DEBUG_FLAGS)
LDFLAGS = -T gcc.ld --specs=rdimon.specs -Wl,--build-id=none,-Map=linkmap.txt
TARGET_ARCH = -mcpu=$(CPU) -mfloat-abi=hard -mfpu=neon-fp-armv8 -m$(CODE_TYPE) # Used for .c
TARGET_MACH = $(TARGET_ARCH) # Used for .S

APP_C_SRC := $(wildcard *.c)
APP_ASM_SRC := $(wildcard *.S)
OBJ_FILES := $(APP_C_SRC:%.c=%.o) $(APP_ASM_SRC:%.S=%.o)
DEP_FILES := $(OBJ_FILES:%.o=%.d)

.phony: all clean

all: $(APP)

$(APP): $(OBJ_FILES) gcc.ld
	$(CC) $(TARGET_ARCH) $(LDFLAGS) -o $@ $(OBJ_FILES)

clean:
	$(call RM_FILES,$(APP) $(OBJ_FILES) $(DEP_FILES) linkmap.txt)

# Make sure everything is rebuilt if this makefile is changed
$(OBJ_FILES) $(APP): makefile

-include $(DEP_FILES)

