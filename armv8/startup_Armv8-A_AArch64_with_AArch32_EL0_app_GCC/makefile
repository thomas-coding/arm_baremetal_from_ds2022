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

AARCH64_STARTUP ?= AArch64_startup.axf
AARCH32_APP ?= AArch32_app.axf
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


CC_64 = aarch64-none-elf-gcc
CC_32 = arm-none-eabi-gcc
NM_32 = arm-none-eabi-nm

ifneq ($(findstring NOT_FOUND,$(shell $(CC_32) --version || echo NOT_FOUND)),)
$(error $(CC_32) not found. Add it to PATH)
endif

APP_DIR = app_32_src
STARTUP_DIR = startup_64_src
APP_OBJ_DIR = app_32_obj
STARTUP_OBJ_DIR = startup_64_obj

INCLUDES = -I$(APP_DIR)

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
LDFLAGS = -Wl,--build-id=none
TARGET_ARCH = -march=$(ARCH)

APP_C_SRC := $(wildcard $(APP_DIR)/*.c)
APP_S_SRC := $(wildcard $(APP_DIR)/*.S)
STARTUP_C_SRC := $(wildcard $(STARTUP_DIR)/*.c)
STARTUP_S_SRC := $(wildcard $(STARTUP_DIR)/*.S)
APP_OBJS := $(APP_C_SRC:$(APP_DIR)/%.c=$(APP_OBJ_DIR)/%.o) \
             $(APP_S_SRC:$(APP_DIR)/%.S=$(APP_OBJ_DIR)/%.o)
STARTUP_OBJS := $(STARTUP_C_SRC:$(STARTUP_DIR)/%.c=$(STARTUP_OBJ_DIR)/%.o) \
             $(STARTUP_S_SRC:$(STARTUP_DIR)/%.S=$(STARTUP_OBJ_DIR)/%.o)

.phony: all clean

all: $(AARCH32_APP) $(AARCH64_STARTUP)

# Symbol definition that we need to pass to AArch64 startup linker
$(AARCH32_APP).syms := MainApp A32StartCLib

$(AARCH64_STARTUP) : $(STARTUP_OBJS) $(AARCH32_APP).syms_as_args startup_64.ld
	@echo Linking $@
	$(QUIET) $(CC_64) $(TARGET_ARCH) $(LDFLAGS) -nostdlib -T startup_64.ld --specs=rdimon.specs @$(AARCH32_APP).syms_as_args -Wl,-Map=startup_64_linkmap.txt --output $@ $(STARTUP_OBJS)
	@echo Done.

$(AARCH32_APP) : $(APP_OBJS) app_32.ld
	@echo Linking $@
	$(QUIET) $(CC_32) $(TARGET_ARCH) $(LDFLAGS) -T app_32.ld --specs=rdimon-v2m.specs -Wl,-Map=app_32_linkmap.txt --output $@ $(APP_OBJS)
	@echo Done.

clean:
	$(call RM_DIRS,$(STARTUP_OBJ_DIR) $(APP_OBJ_DIR))
	$(call RM_FILES,$(AARCH64_STARTUP) $(AARCH32_APP) $(AARCH32_APP).syms_as_args app_32_linkmap.txt startup_64_linkmap.txt)

$(APP_OBJ_DIR):
	mkdir $@

$(APP_OBJ_DIR)/%.o : $(APP_DIR)/%.c | $(APP_OBJ_DIR)
	$(PROGRESS)
	$(QUIET) $(CC_32) -c $(TARGET_ARCH) $(CPPFLAGS) $(CFLAGS) -o $@ $<

$(APP_OBJ_DIR)/%.o : $(APP_DIR)/%.S | $(APP_OBJ_DIR)
	$(PROGRESS)
	$(QUIET) $(CC_32) -c $(TARGET_ARCH) $(CPPFLAGS) $(ASFLAGS) -o $@ $<

$(STARTUP_OBJ_DIR):
	mkdir $@

$(STARTUP_OBJ_DIR)/%.o : $(STARTUP_DIR)/%.c | $(STARTUP_OBJ_DIR)
	$(PROGRESS)
	$(QUIET) $(CC_64) -c $(TARGET_ARCH) $(CPPFLAGS) $(CFLAGS) -o $@ $<

$(STARTUP_OBJ_DIR)/%.o : $(STARTUP_DIR)/%.S | $(STARTUP_OBJ_DIR)
	$(PROGRESS)
	$(QUIET) $(CC_64) -c $(TARGET_ARCH) $(CPPFLAGS) $(ASFLAGS) -o $@ $<

# Make sure everything is rebuilt if this makefile is changed
$(STARTUP_OBJS) $(APP_OBJS) $(AARCH64_STARTUP) $(AARCH32_APP): makefile

-include $(DEP_FILES)

help:
	@echo make [OPTIONS]
	@echo 'PLATFORM=   [AEM/CORTEXA]       Choose FVP target: AEMv8 or Cortex-A35/A53/A57/A72/A73'
	@echo ''
	@echo 'NOTE: The first value in the options indicates the default setting.'

%.syms_as_args: %
	@$(MAKE) --no-print-directory -f syms.mk make_link_args SYMS_TO_DEF="$($*.syms)" SYMS_FILE="$*" SYMS_LINK_ARGS_FILE="$*.syms_as_args" SYMS_NM="$(NM_32)"
