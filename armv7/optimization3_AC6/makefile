# Copyright (c) 2007-2020 Arm Limited (or its affiliates). All rights reserved.
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
# CODE_TYPE    arm or thumb
# DEFINES      -D MYDEFINE

APP ?= optimization3_AC6.axf
QUIET ?= #@
OPT_LEVEL ?= 3
CODE_TYPE ?= arm
DEFINES ?= -fvectorize

# Other switches the user should not normally need to change:
CPU = Cortex-A7
DEBUG_FLAGS = -g

ifeq ($(QUIET),@)
PROGRESS = @echo Compiling $<...
endif

CC = armclang
LINK = armlink
SRC_DIR = src
OBJ_DIR = obj

ifeq ($(OS),Windows_NT)
SHELL=$(windir)\system32\cmd.exe
RM_FILES = $(foreach file,$(1),if exist $(file) del /q $(file)$(EOL))
RM_DIRS = $(foreach dir,$(1),if exist $(dir) rmdir /s /q $(dir)$(EOL))
else
RM_FILES = $(foreach file,$(1),rm -f $(file)$(EOL))
RM_DIRS = $(foreach dir,$(1),rm -rf $(dir)$(EOL))
endif

DEPEND_FLAGS = 
CFLAGS = -mcpu=$(CPU) --target=arm-arm-none-eabi -mfpu=neon-vfpv4 $(DEBUG_FLAGS) -m$(CODE_TYPE) -O$(OPT_LEVEL) $(DEFINES) $(INCLUDES) $(DEPEND_FLAGS) 
LFLAGS = --scatter=scatter.scat --entry=Vectors --diag_suppress=6329

INCLUDES = -I$(SRC_DIR)

define EOL =


endef

APP_C_SRC := $(wildcard $(SRC_DIR)/*.c)
APP_ASM_SRC := $(wildcard $(SRC_DIR)/*.s)
APP_GAS_SRC := $(wildcard $(SRC_DIR)/*.S)
OBJ_FILES := $(APP_C_SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o) \
             $(APP_ASM_SRC:$(SRC_DIR)/%.s=$(OBJ_DIR)/%.o) \
             $(APP_GAS_SRC:$(SRC_DIR)/%.S=$(OBJ_DIR)/%.o)
DEP_FILES := $(OBJ_FILES:%=%.d)

VPATH = $(SRC_DIR)

.phony: all clean

all: $(APP)

$(APP): $(OBJ_DIR) $(OBJ_FILES) scatter.scat
	@echo Linking $@
	$(QUIET) $(LINK) $(LFLAGS) --output $@ $(OBJ_FILES)
	@echo Done.

clean:
	$(call RM_DIRS,$(OBJ_DIR))
	$(call RM_FILES,$(APP))

$(OBJ_DIR):
	mkdir $@

$(OBJ_DIR)/%.o : %.c
	$(PROGRESS)
	$(QUIET) $(CC) -c $(CFLAGS) -o $@ $<

$(OBJ_DIR)/%.o : %.s
	$(PROGRESS)
	$(QUIET) $(CC) $(CFLAGS) -masm=auto -x assembler-with-cpp -c -o $@ $<

$(OBJ_DIR)/%.o : %.S
	$(PROGRESS)
	$(QUIET) $(CC) $(CFLAGS) -masm=auto -x assembler-with-cpp -c -o $@ $<

-include $(DEP_FILES)

