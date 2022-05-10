# Copyright (c) 2007-2021 Arm Limited (or its affiliates). All rights reserved.
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
# OPT_LEVEL    0, 1, 2 or 3
# DEFINES      -D MYDEFINE

APP ?= overlay_manager_manual.axf
OPT_LEVEL ?= 2

DEBUG_FLAGS = -g

CC = armclang
AR = armar

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
LDFLAGS = -Wl,--scatter=demo.scat,--overlay_veneers,--emit_debug_overlay_section,--strict_preserve8_require8
TARGET_ARCH = --target=arm-arm-none-eabi -march=armv7-r -mfpu=none # Used for .c
TARGET_MACH = $(TARGET_ARCH) # Used for .S

OVL_OBJ_FILES := ovl_alloc.o ovl_load.o ovl_manager.o ovl_maxstk.o ovl_notify.o ovl_perthread.o ovl_resume.o ovl_veneer.o
DEMO_OBJ_FILES := main.o module1.o module2.o module3.o module4.o module5.o module6.o
OBJ_FILES := $(OVL_OBJ_FILES) $(DEMO_OBJ_FILES)
DEP_FILES := $(OBJ_FILES:%.o=%.d)

.phony: all clean

all: $(APP)

# Use -fno-inline to avoid accidentally inlining functions from one
# overlay to another, which would prevent the demo from successfully
# demonstrating that calls between overlays work.
main.o module1.o module2.o module3.o module4.o: TARGET_ARCH += -marm -fno-inline
module5.o module6.o: TARGET_ARCH += -mthumb -fno-inline

overlay.a: $(OVL_OBJ_FILES)
	$(AR) --create $@ $(OVL_OBJ_FILES)

$(APP): $(DEMO_OBJ_FILES) overlay.a demo.scat
	$(CC) $(TARGET_ARCH) $(LDFLAGS) -o $@ $(DEMO_OBJ_FILES) overlay.a

clean:
	$(call RM_FILES,$(APP) overlay.a $(OBJ_FILES) $(DEP_FILES))

# Make sure everything is rebuilt if this makefile is changed
$(OBJ_FILES) $(APP): Makefile

-include $(DEP_FILES)
