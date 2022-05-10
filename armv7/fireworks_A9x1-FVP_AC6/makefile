# Copyright (c) 2007-2018 Arm Limited (or its affiliates). All rights reserved.
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

APP ?= fireworks-Cortex-A9x1-FVP.axf
QUIET ?= @
OPT_LEVEL ?= 1
CODE_TYPE ?= thumb
DEFINES ?= -DSTANDALONE

# Other switches the user should not normally need to change:
CPU = Cortex-A9
DEBUG_FLAGS = -g


ifeq ($(QUIET),@)
PROGRESS = @echo Compiling $<...
endif


CC = armclang
SRC_DIR = src
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
CPPFLAGS = $(DEFINES) $(INCLUDES) $(DEPEND_FLAGS)
CFLAGS = $(DEBUG_FLAGS) -O$(OPT_LEVEL)
ASFLAGS = $(DEBUG_FLAGS)
LDFLAGS = -Wl,--scatter=scatter.scat -e Vectors
TARGET_ARCH = --target=arm-arm-none-eabi -mcpu=$(CPU) -m$(CODE_TYPE)

APP_C_SRC := $(wildcard $(SRC_DIR)/*.c)
APP_S_SRC := $(wildcard $(SRC_DIR)/*.S)
OBJ_FILES := $(APP_C_SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o) \
             $(APP_S_SRC:$(SRC_DIR)/%.S=$(OBJ_DIR)/%.o)
DEP_FILES := $(OBJ_FILES:%=%.d)

.phony: all clean

all: $(APP)

$(APP): $(OBJ_DIR) $(OBJ_FILES) scatter.scat
	@echo Linking $@
	$(QUIET) $(CC) $(TARGET_ARCH) $(LDFLAGS) --output $@ $(OBJ_FILES)
	@echo Done.

clean:
	$(call RM_DIRS,$(OBJ_DIR))
	$(call RM_FILES,$(APP))

$(OBJ_DIR):
	mkdir $@

$(OBJ_DIR)/%.o : $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(PROGRESS)
	$(QUIET) $(CC) -c $(TARGET_ARCH) $(CPPFLAGS) $(CFLAGS) -o $@ $<

$(OBJ_DIR)/%.o : $(SRC_DIR)/%.S | $(OBJ_DIR)
	$(PROGRESS)
	$(QUIET) $(CC) -c $(TARGET_ARCH) $(CPPFLAGS) $(ASFLAGS) -o $@ $<

# Make sure everything is rebuilt if this makefile is changed
$(OBJ_FILES) $(APP): makefile

-include $(DEP_FILES)
