# Copyright (c) 2014-2021 Arm Limited (or its affiliates). All rights reserved.
# Use, modification and redistribution of this file is subject to your possession of a
# valid End User License Agreement for the Arm Product of which these examples are part of 
# and your compliance with all applicable terms and conditions of such licence agreement.
#
# This example is intended to be built with Arm Compiler for Embedded 6
#
# Environment variables for build options that the user might wish to change
#
# Variable     Example Value
# ----------   -------------
# APP          myapp.axf
# QUIET        @ for terse output, or leave blank for detailed output
# OPT_LEVEL    0, 1, 2 or 3
# DEFINES      -D MYDEFINE


APP ?= startup_Cortex-A510x1_AC6.axf
QUIET ?= @
OPT_LEVEL ?= 1
OUTPUTCHAN ?= SEMIHOSTED

# Other switches the user should not normally need to change:
CPU = Cortex-A510
PLATFORM = CORTEXA_FCM
DEBUG_FLAGS = -g

REF_DIR = startup_Armv8-Ax1_AC6

ifeq ($(wildcard ../$(REF_DIR)),)
$(error To build this example, import the $(REF_DIR) example that contains the source code into Eclipse)
endif

ifeq "$(PLATFORM)" "CORTEXA_FCM"
	CPPFLAGS_EXTRA += -D USE_LSE_ATOMIC
endif


ifeq ($(QUIET),@)
PROGRESS = @echo Compiling $<...
endif


CC = armclang
SRC_DIR = ../$(REF_DIR)/src
ASM_DIR = ../$(REF_DIR)/asm
OBJ_DIR = obj

INCLUDES = -I$(SRC_DIR)

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

DEPEND_FLAGS = -MD -MF $@.d
CPPFLAGS = $(DEFINES) $(INCLUDES) $(DEPEND_FLAGS) $(CPPFLAGS_EXTRA)
CFLAGS = $(DEBUG_FLAGS) -O$(OPT_LEVEL)
ASFLAGS = $(DEBUG_FLAGS)
LDFLAGS = -Wl,--scatter=scatter.scat,--diag_suppress=6329 -e start64
TARGET_CPU = --target=aarch64-arm-none-eabi -mcpu=$(CPU)

APP_C_SRC := $(wildcard $(SRC_DIR)/*.c)
APP_S_SRC := $(wildcard $(ASM_DIR)/*.S)
OBJ_FILES := $(APP_C_SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o) \
             $(APP_S_SRC:$(ASM_DIR)/%.S=$(OBJ_DIR)/%.o)
DEP_FILES := $(OBJ_FILES:%=%.d)

.phony: all clean

all: $(APP)

$(APP): $(OBJ_DIR) $(OBJ_FILES) scatter.scat
	@echo Linking $@
	$(QUIET) $(CC) $(TARGET_CPU) $(LDFLAGS) --output $@ $(OBJ_FILES)
	@echo Done.

clean:
	$(call RM_DIRS,$(OBJ_DIR))
	$(call RM_FILES,$(APP))

$(OBJ_DIR):
	mkdir $@

$(OBJ_DIR)/%.o : $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(PROGRESS)
	$(QUIET) $(CC) -c $(TARGET_CPU) $(CPPFLAGS) $(CFLAGS) -o $@ $<

$(OBJ_DIR)/%.o : $(ASM_DIR)/%.S | $(OBJ_DIR)
	$(PROGRESS)
	$(QUIET) $(CC) -c $(TARGET_CPU) $(CPPFLAGS) $(ASFLAGS) -o $@ $<

# Make sure everything is rebuilt if this makefile is changed
$(OBJ_FILES) $(APP): makefile

-include $(DEP_FILES)

help:
	@echo make [OPTIONS]

	@echo ''
	@echo 'NOTE: The first value in the options indicates the default setting.'
