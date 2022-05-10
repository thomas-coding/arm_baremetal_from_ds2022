# Copyright (c) 2014-2017 Arm Limited (or its affiliates). All rights reserved.
# Use, modification and redistribution of this file is subject to your possession of a
# valid End User License Agreement for the Arm Product of which these examples are part of 
# and your compliance with all applicable terms and conditions of such licence agreement.

# Helper file for extracting symbol address for gcc to pass to linker

EMPTY :=
SPACE := $(EMPTY) $(EMPTY)

# Get symbol address from file
# $1 The file
# $2 The symbol
get_sym_addr = 0x$(firstword $(subst ~,$(SPACE),$(filter %~$(2),$(subst $(SPACE)T$(SPACE),~,$(shell $(SYMS_NM) -t x $(1))))))

# Variables for recursive make magic
LINK_ARGS := $(foreach sym,$(SYMS_TO_DEF),-Wl,--defsym=$(sym)=$(call get_sym_addr,$(SYMS_FILE),$(sym)))

.PHONY: make_link_args
make_link_args:
	$(QUIET) echo $(LINK_ARGS) > $(SYMS_LINK_ARGS_FILE)
