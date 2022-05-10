# Copyright (c) 2014-2020 Arm Limited (or its affiliates). All rights reserved.
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
# QUIET        @ for terse output, or leave blank for detailed output
# OPT_LEVEL    0, 1, 2 or 3
# DEFINES      -D MYDEFINE
# PLATFORM     CORTEXA (adds extra code for initialising Cortex-A35/A53/A57/A72/A73), or AEM

APP ?= startup_Armv8-A_AArch64_GCC.axf
QUIET ?= @
OPT_LEVEL ?= 1
PLATFORM ?= CORTEXA
OUTPUTCHAN ?= SEMIHOSTED

# Other switches the user should not normally need to change:
ARCH = armv8-a
DEBUG_FLAGS = -g


ifeq "$(PLATFORM)" "CORTEXA"
	CPPFLAGS_EXTRA += -D CORTEXA
endif


ifeq ($(QUIET),@)
PROGRESS = @echo Compiling $<...
endif


CC = aarch64-none-elf-gcc
SRC_DIR = src
ASM_DIR = asm
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
LDFLAGS = -T gcc.ld --specs=rdimon.specs -Wl,--build-id=none,-Map=linkmap.txt
TARGET_ARCH = -march=$(ARCH)

APP_C_SRC := $(wildcard $(SRC_DIR)/*.c)
APP_S_SRC := $(wildcard $(ASM_DIR)/*.S)
OBJ_FILES := $(APP_C_SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o) \
             $(APP_S_SRC:$(ASM_DIR)/%.S=$(OBJ_DIR)/%.o)
DEP_FILES := $(OBJ_FILES:%=%.d)

.phony: all clean

all: $(APP)

$(APP): $(OBJ_FILES) gcc.ld
	@echo Linking $@
	$(QUIET) $(CC) $(TARGET_ARCH) $(LDFLAGS) --output $@ $(OBJ_FILES)
	@echo Done.

clean:
	$(call RM_DIRS,$(OBJ_DIR))
	$(call RM_FILES,$(APP) linkmap.txt)

$(OBJ_DIR):
	mkdir $@

$(OBJ_DIR)/%.o : $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(PROGRESS)
	$(QUIET) $(CC) -c $(TARGET_ARCH) $(CPPFLAGS) $(CFLAGS) -o $@ $<

$(OBJ_DIR)/%.o : $(ASM_DIR)/%.S | $(OBJ_DIR)
	$(PROGRESS)
	$(QUIET) $(CC) -c $(TARGET_ARCH) $(CPPFLAGS) $(ASFLAGS) -o $@ $<

# Make sure everything is rebuilt if this makefile is changed
$(OBJ_FILES) $(APP): makefile

-include $(DEP_FILES)

help:
	@echo make [OPTIONS]
	@echo 'PLATFORM=   [AEM/CORTEXA]       Choose FVP target: AEMv8 or Cortex-A35/A53/A57/A72/A73'
	@echo ''
	@echo 'NOTE: The first value in the options indicates the default setting.'
